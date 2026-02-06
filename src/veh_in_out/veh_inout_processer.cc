
#include "veh_inout_processer.h"

#include "../workflow/module_param.h"
#include "config_content.h"
#include "space_judge.h"
#include "veh_info_copy.h"
#include <fstream>

#include "../utils/file_utils.h"
#include "../utils/sl_logger.h"
#include "../utils/time_utils.h"
// #include "common/illegal.h"
using namespace sonli;

namespace sonli
{

VehicleInOutProcesser::VehicleInOutProcesser(const int camera_id)
{
    camera_id_ = camera_id;

    for (int i = 0; i < max_cap_buf_len; i++)
    {
        cap_buf_control_.cap_buf_control_enable_ids_.push_back(i);

        cap_buf_control_.cap_id_del_[i] = -10;
        cap_buf_control_.cap_id_frame_id_[i] = -1;
        cap_buf_control_.cap_buf_control_num_[i] = 0;
    }
    cache_veh_infos_.camera_channel = camera_id;
}

void
VehicleInOutProcesser::loadVehCacheInfo()
{
    std::string order_name_json = VEH_IN_ORDER_PATH + std::to_string(cache_veh_infos_.camera_channel + 1) + ".json";
    if (isExistFile(order_name_json))
    {
        // load json config
        std::ifstream infile(order_name_json);
        if (!infile.good())
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->error("Can not open file: {}!", order_name_json);
            return;
        }

        Json::Value config_jv;
        infile >> config_jv;
        infile.close();

        if (config_jv.isMember("order") && config_jv["order"].isArray())
        {
            auto order_list = config_jv["order"];
            for (auto order : order_list)
            {
                for (std::string order_name : order_name_list_)
                {
                    if (!order.isMember(order_name))
                    {
                        SonLiLogger::getInstance()
                            .getLogger(SonLiLogger::RotateLogger)
                            ->info("{} not json member, {}", order_name, order[order_name].asString());
                        return;
                    }
                }

                CacheVehInInfo cache_veh_in_info;

                cache_veh_in_info.report_in_timestamp = std::stol(order[order_name_list_[2]].asString());
                cache_veh_in_info.last_timestamp = std::stol(order[order_name_list_[3]].asString());

                cache_veh_in_info.veh_info.veh_chassis.space_res =
                    (VehicleSpaceStatus_t)order[order_name_list_[4]].asInt();
                cache_veh_in_info.veh_info.veh_chassis.space_rat = order[order_name_list_[5]].asDouble();
                cache_veh_in_info.veh_info.veh_chassis.space_id = order[order_name_list_[6]].asInt();
                cache_veh_in_info.veh_info.lp_info.lp_type = (SLLPType_t)order[order_name_list_[7]].asInt();
                cache_veh_in_info.veh_info.lp_info.lp_color = (SLLPColor_t)order[order_name_list_[8]].asInt();

                cache_veh_in_info.veh_info.lp_info.lp_number.lp_str = order[order_name_list_[9]].asString();
                cache_veh_in_info.veh_info.lp_info.lp_number.lp_count = order[order_name_list_[10]].asInt();
                cache_veh_in_info.veh_info.lp_info.lp_number.vlp_rec_confidence =
                    order[order_name_list_[11]].asDouble();
                cache_veh_in_info.veh_info.lp_info.lp_number.first_char_confidence =
                    order[order_name_list_[12]].asDouble();
                cache_veh_in_info.veh_info.lp_info.lp_number.second_char_confidence =
                    order[order_name_list_[13]].asDouble();

                cache_veh_in_info.is_load = true;
                cache_veh_in_infos_.push_back(cache_veh_in_info);
                // for (std::string order_name : order_name_list_)
                // {
                // 	SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
                // 		"{}: {}", order_name, order[order_name].asString());
                // }
            }
        }
        else
        {
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("Get order failed !");
            return;
        }

        try
        {
            deleteFileLinux(order_name_json);
        }
        catch (std::exception &e)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->error("deleteJsonFile {} failed :{}!", order_name_json, e.what());
        }
    }
}

void
VehicleInOutProcesser::saveVehCacheInfo()
{
    if (cache_veh_in_infos_.size() < 1)
    {
        return;
    }

    std::string order_name_json = VEH_IN_ORDER_PATH + std::to_string(cache_veh_infos_.camera_channel + 1) + ".json";
    bool create_ret = createDirectory(VEH_IN_ORDER_PATH);
    if (!create_ret)
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("create {} failed !", VEH_IN_ORDER_PATH);
        return;
    }

    std::ofstream outFile(order_name_json);
    if (!outFile.good())
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("fopen {} failed !", order_name_json);
        return;
    }

    Json::Value order_json;
    for (int in_veh_idx = 0; in_veh_idx < static_cast<int>(cache_veh_in_infos_.size()); in_veh_idx++)
    {
        order_json[in_veh_idx][order_name_list_[0]] = cache_veh_infos_.camera_channel;
        order_json[in_veh_idx][order_name_list_[1]] = std::to_string(getSystemTimeMS());
        order_json[in_veh_idx][order_name_list_[2]] =
            std::to_string(cache_veh_in_infos_[in_veh_idx].report_in_timestamp);
        order_json[in_veh_idx][order_name_list_[3]] = std::to_string(cache_veh_in_infos_[in_veh_idx].last_timestamp);
        order_json[in_veh_idx][order_name_list_[4]] =
            (int)(cache_veh_in_infos_[in_veh_idx].veh_info.veh_chassis.space_res);
        order_json[in_veh_idx][order_name_list_[5]] = cache_veh_in_infos_[in_veh_idx].veh_info.veh_chassis.space_rat;
        order_json[in_veh_idx][order_name_list_[6]] = cache_veh_in_infos_[in_veh_idx].veh_info.veh_chassis.space_id;
        order_json[in_veh_idx][order_name_list_[7]] = (int)(cache_veh_in_infos_[in_veh_idx].veh_info.lp_info.lp_type);
        order_json[in_veh_idx][order_name_list_[8]] = (int)(cache_veh_in_infos_[in_veh_idx].veh_info.lp_info.lp_color);
        order_json[in_veh_idx][order_name_list_[9]] = cache_veh_in_infos_[in_veh_idx].veh_info.lp_info.lp_number.lp_str;
        order_json[in_veh_idx][order_name_list_[10]] =
            cache_veh_in_infos_[in_veh_idx].veh_info.lp_info.lp_number.lp_count;
        order_json[in_veh_idx][order_name_list_[11]] =
            cache_veh_in_infos_[in_veh_idx].veh_info.lp_info.lp_number.vlp_rec_confidence;
        order_json[in_veh_idx][order_name_list_[12]] =
            cache_veh_in_infos_[in_veh_idx].veh_info.lp_info.lp_number.first_char_confidence;
        order_json[in_veh_idx][order_name_list_[13]] =
            cache_veh_in_infos_[in_veh_idx].veh_info.lp_info.lp_number.second_char_confidence;
    }

    Json::Value orders_json;
    orders_json["order"] = order_json;

    Json::StreamWriterBuilder builder;
    builder["emitUTF8"] = true;
    outFile << Json::writeString(builder, orders_json);
    outFile.close();
}

VehicleInOutProcesser::~VehicleInOutProcesser()
{
    SonLiLogger::getInstance()
        .getLogger(SonLiLogger::RotateLogger)
        ->info("VehicleInOutProcesser close!! camera_id {}", cache_veh_infos_.camera_channel);
    // saveVehCacheInfo();
    // park_param_.reset();
    // space_info_.reset();
    // cache_veh_infos_.reset();

    cache_veh_in_infos_.clear();
    cache_veh_out_infos_.clear();
}

void
VehicleInOutProcesser::update(VehInfoVec &vehicles, const ParkingInfoVec &space_info,
                              const std::vector<aih::TimeInfo> &park_frame_time, ParkParameter module_park_param,
                              VehInOutInfoVec &veh_inout_info_vec, CapControlMsgVec &cap_control_msg_vec,
                              ParkingResult &parking_result, bool generate_suspected_order, bool stability_enable,
                              std::map<long, int> &park_frame_id)
{
    camera_id_ = vehicles.camera_channel;
    stability_enable_ = stability_enable;
    CapControlMsgVec cap_control;

    // if (vehicles.frame_id < cache_veh_infos_.frame_id && vehicles.frame_id < 100)
    // {
    // 	cache_veh_infos_.veh_cache_infos.clear();
    // 	progress_start_count_ = 0;

    // 	cap_buf_control_.needInit_ = true;
    // 	ReProCapControl(cap_control_msg_vec, cap_control, camera_id_, vehicles.frame_id);
    // }

    cache_veh_infos_.frame_id = vehicles.frame_id;
    cache_veh_infos_.frame_timestamp = vehicles.frame_timestamp;
    cache_veh_infos_.time_info = vehicles.time_info;
    cache_veh_infos_.generate_suspected_order = generate_suspected_order;
    cache_veh_infos_.use_reid = vehicles.use_reid;

    UpdateFrameVeh(vehicles, park_frame_time);

    if (first_veh_timestamp_ < 1)
    {
        first_veh_timestamp_ = vehicles.frame_timestamp;
    }

    progress_start_count_++;
    if (progress_start_count_ > 10000000)
    {
        progress_start_count_ = 100000;
    }

    // if (progress_start_count_ == 1){
    // 	loadVehCacheInfo();
    // }

    UpdateParkParameter(module_park_param);

    UpdateSpaceInfo(space_info, vehicles.frame_width, vehicles.frame_height);

    InitCapControlMsg(cap_control, vehicles.frame_id);

    UpdateCacheVehInfoVec(cap_control);

    UpdateSpaceVehInfo();

    DetectVehPreProcess(vehicles);

    CacheVehPreProcess(vehicles);

    UpdateToUploadVehMessage(cap_control, veh_inout_info_vec);

    std::string out = "----11041520----cap_id_frame_id_ ";
    for (int pic_id = 0; pic_id < max_cap_buf_len; pic_id++)
    {
        if (cap_buf_control_.cap_id_frame_id_[pic_id] < 0)
        {
            continue;
        }
        park_frame_id[cap_buf_control_.cap_id_frame_id_[pic_id]] = 1;
        out = out + std::to_string(cap_buf_control_.cap_id_frame_id_[pic_id]) + " , ";
    }
    // SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info("{}", out);

    CacheVehProcess(veh_inout_info_vec, cap_control, vehicles);

    if (cache_veh_infos_.generate_suspected_order)
    {
        GenerateSuspectedOrder(veh_inout_info_vec, cap_control);
    }

    ReProCapControl(cap_control_msg_vec, cap_control, camera_id_, vehicles.frame_id);

    veh_inout_info_vec.size = veh_inout_info_vec.veh_inout_info.size();

    UpdateParkingResult(veh_inout_info_vec, vehicles);

    parking_result = parking_result_;

    // if(veh_inout_info_vec.size > -1) {
    // 	PrintLog(cap_control_msg_vec, veh_inout_info_vec, vehicles, parking_result);
    // }
    PrintSendMessage(veh_inout_info_vec);

    UpdateVehInoutInfo(veh_inout_info_vec);

    for (int pic_id = 0; pic_id < max_cap_buf_len; pic_id++)
    {
        if (cap_buf_control_.cap_id_frame_id_[pic_id] < 0)
        {
            continue;
        }
        park_frame_id[cap_buf_control_.cap_id_frame_id_[pic_id]] = 1;
        out = out + std::to_string(cap_buf_control_.cap_id_frame_id_[pic_id]) + " , ";
    }
    // SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info("{}", out);
}
void
VehicleInOutProcesser::UpdateVehInoutInfo(VehInOutInfoVec &veh_inout_info_vec)
{
    // if(veh_inout_info_vec.veh_inout_info.size() > 0){
    // 	return;
    // }

    std::vector<long> inout_id_vec;
    for (int i = 0; i < veh_inout_info_vec.veh_inout_info.size(); i++)
    {
        inout_id_vec.push_back(veh_inout_info_vec.veh_inout_info[i].pro_veh_info.veh_track_id);
    }

    for (int i = 0; i < cache_veh_infos_.veh_cache_infos.size(); i++)
    {
        if (cache_veh_infos_.frame_timestamp != cache_veh_infos_.veh_cache_infos[i].veh_infos.back().veh_timestamp)
        {
            continue;
        }
        if (cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.veh_state == SL_CAR_STATE_NOON)
        {
            continue;
        }
        auto it = std::find(inout_id_vec.begin(), inout_id_vec.end(), cache_veh_infos_.veh_cache_infos[i].track_id);
        if (it != inout_id_vec.end())
        {
            continue;
        }
        inout_id_vec.push_back(cache_veh_infos_.veh_cache_infos[i].track_id);

        VehInOutInfo veh_inout_info;
        veh_inout_info.veh_state = cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.veh_state;

        if (veh_inout_info.veh_state == SL_CAR_STATE_IN_PRE)
        {
            veh_inout_info.out_veh_info = cache_veh_infos_.veh_cache_infos[i].veh_inpro_out_mess.veh_info;
        }
        else if (veh_inout_info.veh_state == SL_CAR_STATE_IN)
        {
            veh_inout_info.pro_veh_info = cache_veh_infos_.veh_cache_infos[i].veh_inpro_pro_mess.veh_info;
        }
        else if (veh_inout_info.veh_state == SL_CAR_STATE_STOP)
        {
            veh_inout_info.in_veh_info = cache_veh_infos_.veh_cache_infos[i].veh_standstill_mess.veh_info;
        }
        else if (veh_inout_info.veh_state == SL_CAR_STATE_STOP_LEAVE)
        {
            veh_inout_info.in_veh_info = cache_veh_infos_.veh_cache_infos[i].veh_outpro_std_mess.veh_info;
        }
        else if (veh_inout_info.veh_state == SL_CAR_STATE_LEAVING)
        {
            veh_inout_info.pro_veh_info = cache_veh_infos_.veh_cache_infos[i].veh_outpro_pro_mess.veh_info;
        }
        else if (veh_inout_info.veh_state == SL_CAR_STATE_OUT)
        {
            veh_inout_info.out_veh_info = cache_veh_infos_.veh_cache_infos[i].veh_outpro_out_mess.veh_info;
        }
        else
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->info("---------UpdateVehInoutInfo-------------- veh_state {}, track_id {}", veh_inout_info.veh_state,
                       cache_veh_infos_.veh_cache_infos[i].track_id);
        }
        veh_inout_info.out_veh_info.time_info = cache_veh_infos_.time_info;
        veh_inout_info.pro_veh_info.time_info = cache_veh_infos_.time_info;
        veh_inout_info.in_veh_info.time_info = cache_veh_infos_.time_info;

        veh_inout_info_vec.veh_inout_info.push_back(veh_inout_info);
        veh_inout_info_vec.size = veh_inout_info_vec.veh_inout_info.size();
        // break;
    }
}

void
VehicleInOutProcesser::UpdateExpandSpace()
{
    space_info_.space_points_expand.clear();
    for (int i = 0; i < static_cast<int>(space_info_.space_points.size()); i++)
    {
        std::vector<Point> space_point = space_info_.space_points[i];
        if (space_point.size() == 4)
        {
            Point point_lt = space_point[0];
            Point point_lb = space_point[1];
            Point point_rb = space_point[2];
            Point point_rt = space_point[3];
            int width = (point_rt.x_ - point_lt.x_ + point_rb.x_ - point_lb.x_) / 2;
            int height = (point_rb.y_ - point_rt.y_ + point_lb.y_ - point_lt.y_) / 2;

            int ex_left_width = width;
            int ex_right_width = width;
            if (space_info_.park_region == IMG_LEFT)
            {
                // ex_right_width = width * 3 / 2;
                if (space_info_.space_gradient[i] < 0.8)
                {
                    ex_left_width = width / 2;
                }
            }
            else
            {
                // ex_left_width = width * 3 / 2;
                if (space_info_.space_gradient[i] < 0.8)
                {
                    ex_right_width = width / 2;
                }
            }
            std::vector<Point> space_point_ex;
            space_point_ex.push_back(
                Point(std::max(point_lt.x_ - ex_left_width, 0), std::max(point_lt.y_ - height, 0)));
            space_point_ex.push_back(Point(std::max(point_lb.x_ - ex_left_width, 0), point_lb.y_ + height));
            space_point_ex.push_back(Point(point_rb.x_ + ex_right_width, point_rb.y_ + height));
            space_point_ex.push_back(Point(point_rt.x_ + ex_right_width, std::max(point_rt.y_ - height, 0)));

            space_info_.space_points_expand.push_back(space_point_ex);

            // SonLiLogger::getInstance().getLogger(SonLiLogger::DailyLogger)->info(
            //     "---------UpdateExpandSpace-------------- park_region {}, space_gradient {}, i {}, {} {} {} {} {} {}
            //     {} {}, {} {} {} {} {} {} {} {}" ,
            // 	space_info_.park_region,
            // 	space_info_.space_gradient[i],
            // 	i,
            // 	space_point_ex[0].x_, space_point_ex[0].y_,
            // 	space_point_ex[1].x_, space_point_ex[1].y_,
            // 	space_point_ex[2].x_, space_point_ex[2].y_,
            // 	space_point_ex[3].x_, space_point_ex[3].y_,
            // 	space_point[0].x_, space_point[0].y_,
            // 	space_point[1].x_, space_point[1].y_,
            // 	space_point[2].x_, space_point[2].y_,
            // 	space_point[3].x_, space_point[3].y_);
        }
    }
}

void
VehicleInOutProcesser::UpdateSpaceInfo(const ParkingInfoVec &space_info, int img_w, int img_h)
{
    space_info_ = SpaceInfo();
    int bottom_space_x1 = -10;
    int bottom_space_x2 = -10;
    int bottom_space_y = 0;
    for (int i = 0; i < static_cast<int>(space_info.parking_info.size()); i++)
    {
        std::vector<Point> space_point = space_info.parking_info[i].points;
        if (space_point.size() != 4)
        {
            continue;
        }

        float gradient = -1.0;
        int x_t = (space_point[0].x_ + space_point[3].x_) / 2.0;
        int y_t = (space_point[0].y_ + space_point[3].y_) / 2.0;
        int x_b = (space_point[1].x_ + space_point[2].x_) / 2.0;
        int y_b = (space_point[1].y_ + space_point[2].y_) / 2.0;
        gradient = std::fabs(y_b - y_t) / std::fmax(std::fabs(x_b - x_t), 1.0);
        space_info_.space_gradient.push_back(gradient);

        space_info_.space_points.push_back(space_point);
        space_info_.space_index.push_back(i);

        if (space_point[3].y_ > bottom_space_y)
        {
            bottom_space_y = space_point[3].y_;
            bottom_space_x1 = space_point[1].x_;
            bottom_space_x2 = space_point[2].x_;
        }
    }

    if (bottom_space_x1 > -1 && bottom_space_x2 > -1)
    {
        if ((bottom_space_x1 + bottom_space_x2) > img_w)
        {
            space_info_.park_region = IMG_RIGHT;
        }
        else
        {
            space_info_.park_region = IMG_LEFT;
        }
    }

    space_info_.space_count = space_info_.space_points.size();
    this->parking_result_.parking_num = space_info_.space_count;
    UpdateExpandSpace();
}

void
VehicleInOutProcesser::UpdateParkParameter(ParkParameter module_park_param)
{
    park_param_.veh_in_rat = module_park_param.veh_in_rat;
    park_param_.veh_line_rat = module_park_param.veh_in_rat / 2.0;
    park_param_.veh_in_time_thres = module_park_param.veh_in_time_thres;
    park_param_.veh_out_time_thres = module_park_param.veh_out_time_thres;
}

static std::vector<Point>
driftSuppression(const std::vector<Point> &points, int max_y, int ps_y = 24)
{
    std::vector<Point> inhibition_point(points.size());
    for (int i = 0; i < static_cast<int>(points.size()); i++)
    {
        inhibition_point[i].x_ = points[i].x_;
        inhibition_point[i].y_ = std::min(max_y, points[i].y_ + ps_y);
    }
    return inhibition_point;
}

void
VehicleInOutProcesser::DetectVehPreProcess(VehInfoVec &vehicles)
{
    for (int i = 0; i < vehicles.size; i++)
    {
        if (vehicles.veh_info[i].veh_track_id < 1 || vehicles.veh_info[i].track_state_ != TrackState::Tracked)
        {
            continue;
        }

        std::vector<Point> veh_chassis = vehicles.veh_info[i].veh_chassis.chassis;
        auto sum_vis =
            vehicles.veh_info[i].veh_key_points[0].vis_score + vehicles.veh_info[i].veh_key_points[1].vis_score +
            vehicles.veh_info[i].veh_key_points[2].vis_score + vehicles.veh_info[i].veh_key_points[3].vis_score;

        if (sum_vis < 0.3)
        {
            veh_chassis = driftSuppression(veh_chassis, vehicles.frame_height, 26);
        }
        // 计算车辆占用扩展车位的比率
        float space_rat_ex = 0.0;
        int space_id_ex = -1;
        float expand_rat = 0.1;
        std::vector<float> space_rat_vec_ex;
        std::string space_res_ex = JudgeSpace(space_info_.space_points_expand, veh_chassis, space_rat_ex,
                                              space_rat_vec_ex, expand_rat, expand_rat, space_id_ex);
        if (space_res_ex == "yes")
        {
            vehicles.veh_info[i].veh_chassis.space_res_ex = SPACE_IN;
        }
        else if (space_res_ex == "oh")
        {
            vehicles.veh_info[i].veh_chassis.space_res_ex = SPACE_LINE;
        }
        else
        {
            vehicles.veh_info[i].veh_chassis.space_res_ex = SPACE_OUT;
        }
        vehicles.veh_info[i].veh_chassis.space_rat_ex = space_rat_ex;
        vehicles.veh_info[i].veh_chassis.space_id_ex = space_id_ex;
        vehicles.veh_info[i].veh_chassis.space_rat_vec_ex = space_rat_vec_ex;

        // 计算车辆占用车位的比率
        float space_rat = 0.0;
        int space_id = -1;
        std::vector<float> space_rat_vec;
        std::string space_res = "no";

        if (space_rat_ex < 0.05)
        {
            for (int i = 0; i < space_info_.space_points.size(); i++)
            {
                space_rat_vec.push_back(0.0);
            }
        }
        else
        {
            space_res = JudgeSpace(space_info_.space_points, veh_chassis, space_rat, space_rat_vec,
                                   park_param_.veh_in_rat, park_param_.veh_line_rat, space_id,
                                   std::addressof(vehicles.veh_info[i].veh_chassis.car_rat_vec));
        }

        if (space_res == "yes")
        {
            vehicles.veh_info[i].veh_chassis.space_res = SPACE_IN;
        }
        else if (space_res == "oh")
        {
            vehicles.veh_info[i].veh_chassis.space_res = SPACE_LINE;
        }
        else
        {
            vehicles.veh_info[i].veh_chassis.space_res = SPACE_OUT;
        }

        if (vehicles.veh_info[i].veh_chassis.space_res != SPACE_OUT)
        {
            if (space_id >= 0 && space_id < space_info_.space_points.size())
            {
                Point2f pt_car_1;
                pt_car_1.x_ =
                    (vehicles.veh_info[i].veh_chassis.chassis[0].x_ + vehicles.veh_info[i].veh_chassis.chassis[3].x_) /
                    2.0;
                pt_car_1.y_ =
                    (vehicles.veh_info[i].veh_chassis.chassis[0].y_ + vehicles.veh_info[i].veh_chassis.chassis[3].y_) /
                    2.0;

                Point2f pt_car_2;
                pt_car_2.x_ =
                    (vehicles.veh_info[i].veh_chassis.chassis[1].x_ + vehicles.veh_info[i].veh_chassis.chassis[2].x_) /
                    2.0;
                pt_car_2.y_ =
                    (vehicles.veh_info[i].veh_chassis.chassis[1].y_ + vehicles.veh_info[i].veh_chassis.chassis[2].y_) /
                    2.0;

                Point2f pt_space_1;
                pt_space_1.x_ =
                    (space_info_.space_points[space_id][0].x_ + space_info_.space_points[space_id][3].x_) / 2.0;
                pt_space_1.y_ =
                    (space_info_.space_points[space_id][0].y_ + space_info_.space_points[space_id][3].y_) / 2.0;

                Point2f pt_space_2;
                pt_space_2.x_ =
                    (space_info_.space_points[space_id][1].x_ + space_info_.space_points[space_id][2].x_) / 2.0;
                pt_space_2.y_ =
                    (space_info_.space_points[space_id][1].y_ + space_info_.space_points[space_id][2].y_) / 2.0;

                vehicles.veh_info[i].veh_chassis.line_angle =
                    get_angle_diff(pt_car_1, pt_car_2, pt_space_1, pt_space_2);
            }
        }

        vehicles.veh_info[i].veh_chassis.space_rat = space_rat;
        vehicles.veh_info[i].veh_chassis.space_id = space_id;
        vehicles.veh_info[i].veh_chassis.space_rat_vec = space_rat_vec;
    }
}

void
VehicleInOutProcesser::SmoothSpaceRat(int ind)
{
    if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() < 2)
    {
        return;
    }
    float smooth_rat = 0.5;

    // VehInfo veh_info_now = cache_veh_infos_.veh_cache_infos[ind].veh_infos.back();

    float space_rat_pre = cache_veh_infos_.veh_cache_infos[ind]
                              .veh_infos[cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() - 2]
                              .veh_chassis.space_rat;
    float space_rat_now = cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_rat;
    space_rat_now = smooth_rat * space_rat_now + (1 - smooth_rat) * space_rat_pre;

    cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_rat = space_rat_now;

    if (space_rat_now >= park_param_.veh_in_rat)
    {
        cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_res = SPACE_IN;
        if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_id < 0)
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_id = 0;
            for (int space_ind = 1;
                 space_ind < cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_rat_vec.size();
                 space_ind++)
            {
                if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_rat_vec[space_ind] >
                    cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_rat_vec[space_ind - 1])
                {
                    cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_id = space_ind;
                }
            }
        }
    }
    else if (space_rat_now >= park_param_.veh_line_rat)
    {
        cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_id = -1;
        cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_res = SPACE_LINE;
    }
    else
    {
        cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_id = -1;
        cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_res = SPACE_OUT;
    }
}

void
VehicleInOutProcesser::CacheVehPreProcess(const VehInfoVec &vehicles)
{

    for (int vi = 0; vi < vehicles.veh_info.size(); vi++)
    {
        // VehInfo veh_info = vehicles.veh_info[vi];

        if (vehicles.veh_info[vi].veh_track_id < 1 || vehicles.veh_info[vi].track_state_ != TrackState::Tracked)
        {
            continue;
        }

        int ind = -1;
        for (int i = 0; i < static_cast<int>(cache_veh_infos_.veh_cache_infos.size()); i++)
        {
            if (cache_veh_infos_.veh_cache_infos[i].track_id == vehicles.veh_info[vi].veh_track_id)
            {
                ind = i;
                break;
            }
        }

        std::string lpStr = vehicles.veh_info[vi].lp_info.lp_number.lp_str;
        float conf_lp = vehicles.veh_info[vi].lp_info.lp_number.vlp_rec_confidence;

        if (ind > -1)
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_infos.push_back(vehicles.veh_info[vi]);
            if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().lp_info.vlp_det_confidence <
                cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().lp_info.lp_number.vlp_rec_confidence)
            {
                cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().lp_info.vlp_det_confidence =
                    cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().lp_info.lp_number.vlp_rec_confidence;
            }
            if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().lp_info.vlp_det_confidence < 0.05)
            {
                cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().lp_info.location = Rect(0, 0, 5, 5);
            }

            SmoothSpaceRat(ind);
            // cache_veh_infos_.veh_cache_infos[ind].space_rate_list.enqueue(veh_info.veh_chassis.space_rat);

            if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.veh_appear_timestamp < 1)
            {
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.veh_appear_timestamp =
                    cache_veh_infos_.frame_timestamp;
            }

            if ((cache_veh_infos_.veh_cache_infos[ind].veh_infos_first_n).size() < 10)
            {
                VehInfo veh_info_t;
                VehInfoCopy(veh_info_t, vehicles.veh_info[vi]);
                cache_veh_infos_.veh_cache_infos[ind].veh_infos_first_n.push_back(veh_info_t);
            }

            cache_veh_infos_.veh_cache_infos[ind].veh_color_vec.push_back(vehicles.veh_info[vi].veh_color);
            cache_veh_infos_.veh_cache_infos[ind].veh_type_vec.push_back(vehicles.veh_info[vi].veh_type);

            if (lpStr != "-")
            {
                cache_veh_infos_.veh_cache_infos[ind].lp_str_vec.push_back(std::make_pair(lpStr, conf_lp));
                if (vehicles.veh_info[vi].lp_info.lp_color != SL_PLATE_COLOR_UNKNOWN)
                {
                    cache_veh_infos_.veh_cache_infos[ind].lp_color_vec.push_back(
                        vehicles.veh_info[vi].lp_info.lp_color);
                }

                cache_veh_infos_.veh_cache_infos[ind].no_lp_count = 0;

                cache_veh_infos_.veh_cache_infos[ind].last_lp_info.lp_str = lpStr;
                cache_veh_infos_.veh_cache_infos[ind].last_lp_info.timestamp = vehicles.frame_timestamp;

                if (lpStr == cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_str_continuous)
                {
                    cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_count_continuous++;
                    if (cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_count_continuous > 9999)
                    {
                        cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_count_continuous = 9999;
                    }

                    if (cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_count_continuous >=
                            cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_count_max &&
                        cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_count_continuous > 0)
                    {
                        cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_str_max =
                            cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_str_continuous;

                        cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_count_max =
                            cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_count_continuous;
                    }
                }
                else
                {
                    if (cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_count_continuous >=
                            cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_count_max &&
                        cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_count_continuous > 0)
                    {
                        cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_str_max =
                            cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_str_continuous;

                        cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_count_max =
                            cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_count_continuous;
                    }
                    cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_count_continuous++;
                    cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_str_continuous = lpStr;
                }
            }
            else
            {
                cache_veh_infos_.veh_cache_infos[ind].no_lp_count++;
                if (cache_veh_infos_.veh_cache_infos[ind].no_lp_count > 999999999)
                {
                    cache_veh_infos_.veh_cache_infos[ind].no_lp_count = 999999999;
                }

                if (cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_count_continuous >=
                        cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_count_max &&
                    cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_count_continuous > 0)
                {
                    cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_str_max =
                        cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_str_continuous;

                    cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_count_max =
                        cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_count_continuous;
                }

                cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_count_continuous = 0;
                cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_str_continuous = "-";
            }

            if (cache_veh_infos_.veh_cache_infos[ind].lp_str_vec.size() > LP_CACHE_LENGTH)
            {
                cache_veh_infos_.veh_cache_infos[ind].lp_str_vec.erase(
                    cache_veh_infos_.veh_cache_infos[ind].lp_str_vec.begin(),
                    cache_veh_infos_.veh_cache_infos[ind].lp_str_vec.begin() + 1);
            }

            if (cache_veh_infos_.veh_cache_infos[ind].lp_color_vec.size() > LP_CACHE_LENGTH)
            {
                cache_veh_infos_.veh_cache_infos[ind].lp_color_vec.erase(
                    cache_veh_infos_.veh_cache_infos[ind].lp_color_vec.begin(),
                    cache_veh_infos_.veh_cache_infos[ind].lp_color_vec.begin() + 1);
            }

            if (cache_veh_infos_.veh_cache_infos[ind].veh_color_vec.size() > LP_CACHE_LENGTH)
            {
                cache_veh_infos_.veh_cache_infos[ind].veh_color_vec.erase(
                    cache_veh_infos_.veh_cache_infos[ind].veh_color_vec.begin(),
                    cache_veh_infos_.veh_cache_infos[ind].veh_color_vec.begin() + 1);
            }

            if (cache_veh_infos_.veh_cache_infos[ind].veh_type_vec.size() > LP_CACHE_LENGTH)
            {
                cache_veh_infos_.veh_cache_infos[ind].veh_type_vec.erase(
                    cache_veh_infos_.veh_cache_infos[ind].veh_type_vec.begin(),
                    cache_veh_infos_.veh_cache_infos[ind].veh_type_vec.begin() + 1);
            }

            while (cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() >= VEH_CACHE_LENGTH)
            {
                for (int i = 0; i < 10; i++)
                {
                    cache_veh_infos_.veh_cache_infos[ind].veh_infos.erase(
                        cache_veh_infos_.veh_cache_infos[ind].veh_infos.begin(),
                        cache_veh_infos_.veh_cache_infos[ind].veh_infos.begin() + 1);
                }
            }

            cache_veh_infos_.veh_cache_infos[ind].sapceID_vec.push_back(vehicles.veh_info[vi].veh_chassis.space_id);
            if (cache_veh_infos_.veh_cache_infos[ind].sapceID_vec.size() > LP_CACHE_LENGTH)
            {
                cache_veh_infos_.veh_cache_infos[ind].sapceID_vec.erase(
                    cache_veh_infos_.veh_cache_infos[ind].sapceID_vec.begin(),
                    cache_veh_infos_.veh_cache_infos[ind].sapceID_vec.begin() + 1);
            }
        }
        else
        {
            CacheVehInfo cahhe_veh_info;
            cahhe_veh_info.track_id = vehicles.veh_info[vi].veh_track_id;
            cahhe_veh_info.veh_infos.push_back(vehicles.veh_info[vi]);
            cahhe_veh_info.frame_height = vehicles.frame_height;
            cahhe_veh_info.frame_width = vehicles.frame_width;

            cahhe_veh_info.veh_in_out_var.veh_appear_timestamp = cache_veh_infos_.frame_timestamp;

            cache_veh_infos_.veh_cache_infos.push_back(cahhe_veh_info);

            ind = cache_veh_infos_.veh_cache_infos.size() - 1;
            cache_veh_infos_.veh_cache_infos[ind].veh_infos.clear();
            cache_veh_infos_.veh_cache_infos[ind].veh_infos.push_back(vehicles.veh_info[vi]);
            cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().lp_info.vlp_det_confidence =
                cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().lp_info.lp_number.vlp_rec_confidence;
            if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().lp_info.lp_number.vlp_rec_confidence < 0.05)
            {
                cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().lp_info.location = Rect(0, 0, 5, 5);
            }

            cache_veh_infos_.veh_cache_infos[ind].veh_infos_first_n.clear();
            cache_veh_infos_.veh_cache_infos[ind].veh_infos_first_n.push_back(vehicles.veh_info[vi]);

            cache_veh_infos_.veh_cache_infos[ind].veh_color_vec.clear();
            cache_veh_infos_.veh_cache_infos[ind].veh_color_vec.push_back(vehicles.veh_info[vi].veh_color);
            cache_veh_infos_.veh_cache_infos[ind].veh_type_vec.clear();
            cache_veh_infos_.veh_cache_infos[ind].veh_type_vec.push_back(vehicles.veh_info[vi].veh_type);

            cache_veh_infos_.veh_cache_infos[ind].lp_str_vec.clear();
            cache_veh_infos_.veh_cache_infos[ind].lp_color_vec.clear();
            if (lpStr != "-")
            {
                cache_veh_infos_.veh_cache_infos[ind].lp_str_vec.push_back(std::make_pair(lpStr, conf_lp));

                if (vehicles.veh_info[vi].lp_info.lp_color != SL_PLATE_COLOR_UNKNOWN)
                {
                    cache_veh_infos_.veh_cache_infos[ind].lp_color_vec.push_back(
                        vehicles.veh_info[vi].lp_info.lp_color);
                }
                cache_veh_infos_.veh_cache_infos[ind].no_lp_count = 0;
            }
            else
            {
                cache_veh_infos_.veh_cache_infos[ind].no_lp_count = 1;
            }

            cache_veh_infos_.veh_cache_infos[ind].sapceID_vec.push_back(vehicles.veh_info[vi].veh_chassis.space_id);
        }

        if (ind > -1 && cache_veh_infos_.veh_cache_infos[ind].veh_locs_first_n.size() < 1)
        {
            int first_n_count_conf = 5;
            if (cache_veh_infos_.veh_cache_infos[ind].veh_infos_first_n.size() > first_n_count_conf)
            {
                Rect veh_loc_avg;
                veh_loc_avg.x_ = 0;
                veh_loc_avg.y_ = 0;
                veh_loc_avg.width_ = 0;
                veh_loc_avg.height_ = 0;

                for (int i = 0; i < first_n_count_conf; i++)
                {
                    Rect veh_loc = cache_veh_infos_.veh_cache_infos[ind].veh_infos_first_n[i].location;
                    veh_loc_avg.x_ += veh_loc.x_;
                    veh_loc_avg.y_ += veh_loc.y_;
                    veh_loc_avg.width_ += veh_loc.width_;
                    veh_loc_avg.height_ += veh_loc.height_;
                }

                veh_loc_avg.x_ = veh_loc_avg.x_ / first_n_count_conf;
                veh_loc_avg.y_ = veh_loc_avg.y_ / first_n_count_conf;
                veh_loc_avg.width_ = veh_loc_avg.width_ / first_n_count_conf;
                veh_loc_avg.height_ = veh_loc_avg.height_ / first_n_count_conf;

                cache_veh_infos_.veh_cache_infos[ind].veh_locs_first_n.clear();
                cache_veh_infos_.veh_cache_infos[ind].veh_locs_first_n.push_back(veh_loc_avg);
            }
        }
    }
}

void
VehicleInOutProcesser::CacheVehProcess(VehInOutInfoVec &veh_inout_info_vec, CapControlMsgVec &cap_control,
                                       const VehInfoVec &vehicles)
{
    for (int veh_idx = 0; veh_idx < vehicles.veh_info.size(); veh_idx++)
    {
        if (vehicles.veh_info[veh_idx].veh_track_id < 1 ||
            vehicles.veh_info[veh_idx].track_state_ != TrackState::Tracked)
        {
            continue;
        }

        int ind = -1;
        for (int i = 0; i < cache_veh_infos_.veh_cache_infos.size(); i++)
        {
            if (cache_veh_infos_.veh_cache_infos[i].track_id == vehicles.veh_info[veh_idx].veh_track_id)
            {
                ind = i;
                break;
            }
        }

        if (ind < 0)
        {
            continue;
        }

        // std::vector<VehInfo> veh_infos = cache_veh_infos_.veh_cache_infos[ind].veh_infos;
        VehicleSpaceStatus_t resu = SPACE_OUT;
        resu = cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_res;

        VehLpNumberStatus(ind);

        if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_standstill ||
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_report_in)
        {
            // 进停稳流程
            if (!cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_leave)
            {
                VehStandProcess(cap_control, ind);
            }
        }
        else
        {
            if (resu != SPACE_OUT)
            {
                if (cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_count > 5,
                    cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.vlp_rec_confidence > 0.95)
                {
                    for (int veh_in_i = 0; veh_in_i < cache_veh_in_infos_.size(); veh_in_i++)
                    {
                        std::string lp_str_i = cache_veh_in_infos_[veh_in_i].veh_info.lp_info.lp_number.lp_str;
                        std::string lp_str_ind = cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str;
                        if (lp_str_i == lp_str_ind)
                        {
                            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_standstill = true;
                            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_report_in = true;
                        }
                    }
                }

                if (resu == SPACE_IN)
                {
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_count++;
                }
                else
                {
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_count = 0;
                }

                if (!cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_standstill &&
                    !cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_report_in)
                {
                    VehInProcess(cap_control, ind);
                }
            }
            else
            {
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_count = 0;
                bool is_have_report_in = false;
                if (cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_count > 3,
                    cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.vlp_rec_confidence > 0.9)
                {
                    for (int veh_in_i = 0; veh_in_i < cache_veh_in_infos_.size(); veh_in_i++)
                    {
                        std::string lp_str_i = cache_veh_in_infos_[veh_in_i].veh_info.lp_info.lp_number.lp_str;
                        std::string lp_str_ind = cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str;
                        if (lp_str_i == lp_str_ind)
                        {
                            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_standstill = true;
                            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_report_in = true;
                            is_have_report_in = true;
                        }
                    }
                }

                if (!is_have_report_in)
                {
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_count = 0;
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_stand_count = 0;

                    if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_res_ex != SPACE_OUT &&
                        // veh_infos.back().veh_chassis.space_rat > 0.0 &&
                        !cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_standstill &&
                        !cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_report_in)
                    {
                        // 有进场趋势
                        VehMoveInProcess(cap_control, ind);
                    }
                }
            }
        }
    }
    // 处理cache_veh_infos_ 中消失的车辆信息
    std::vector<int> cache_veh_dis_ind;
    for (int i = 0; i < cache_veh_infos_.veh_cache_infos.size(); i++)
    {
        bool dis = true;
        for (VehInfo veh : vehicles.veh_info)
        {
            if (veh.veh_track_id < 1 || veh.track_state_ != TrackState::Tracked)
            {
                continue;
            }
            if (cache_veh_infos_.veh_cache_infos[i].track_id == veh.veh_track_id)
            {
                dis = false;
                break;
            }
        }

        bool ret_dis = time_compare(cache_veh_infos_.veh_cache_infos[i].veh_infos.back().veh_timestamp,
                                    cache_veh_infos_.frame_timestamp, 2);
        if (dis && ret_dis)
        {
            cache_veh_dis_ind.push_back(i);
        }
        else
        {
            cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_disappear = false;
            cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_disappear_abnormal = false;
            cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.veh_disappear_timestamp = 0;
        }
    }

    VehDisProcess(cap_control, cache_veh_dis_ind, veh_inout_info_vec);

    if (cap_buf_control_.cap_buf_control_enable_ids_.size() <= 3)
    {
        SonLiLogger::getInstance()
            .getLogger(SonLiLogger::RotateLogger)
            ->warn("cap_buf_control_ size <= 3: to do del...");

        for (int i = 0; i < cache_veh_infos_.veh_cache_infos.size(); i++)
        {
            ResetVehInInfo(cap_control, i);
            ResetVehOutInfo(cap_control, i);
        }
        cap_buf_control_.needInit_ = true;
    }
}

void
VehicleInOutProcesser::GenerateSuspectedOrder(VehInOutInfoVec &veh_inout_info_vec, CapControlMsgVec &cap_control)
{
    for (int ind = 0; ind < cache_veh_infos_.veh_cache_infos.size(); ind++)
    {
        if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_report_out ||
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_judge_suspected_out ||
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_judge_suspected_in ||
            cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str == "-")
        {
            continue;
        }
        if (!cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_report_in)
        {
            if (!cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear)
            {
                if (!cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_standstill)
                {
                    continue;
                }

                if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag >= 0)
                {
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_judge_suspected_in = true;
                }

                if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag < 200 ||
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag > 5000)
                {
                    continue;
                }
                // TODO  生成疑似驶入订单
                SonLiLogger::getInstance()
                    .getLogger(SonLiLogger::RotateLogger)
                    ->warn(
                        "-----suspected in 1 ---- camera_id {}, lp_str {}, lp_count {}, lp_conf {}, no_send_in_flag {}",
                        cache_veh_infos_.camera_channel, cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str,
                        cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_count,
                        cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.vlp_rec_confidence,
                        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag);
            }
            else
            {

                if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_in != 0)
                {
                    continue;
                }
                if (IsSatisfyInSuspected(ind))
                {
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_judge_suspected_in = true;
                    // TODO   生成疑似驶入订单
                    SonLiLogger::getInstance()
                        .getLogger(SonLiLogger::RotateLogger)
                        ->warn("-----suspected in 2 ---- camera_id {}, lp_str {}, lp_count {}, lp_conf {}, "
                               "no_send_in_flag {}",
                               cache_veh_infos_.camera_channel,
                               cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str,
                               cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_count,
                               cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.vlp_rec_confidence,
                               cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag);
                }
            }
        }
        else
        {
            if (!cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear)
            {
                if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_out_flag < 0)
                {
                    continue;
                }
                else
                {
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_judge_suspected_out = true;
                    // TODO   生成疑似驶出订单
                    SonLiLogger::getInstance()
                        .getLogger(SonLiLogger::RotateLogger)
                        ->warn("-----suspected out 1 ---- camera_id {}, lp_str {}, lp_count {}, lp_conf {}, "
                               "no_send_out_flag {}",
                               cache_veh_infos_.camera_channel,
                               cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str,
                               cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_count,
                               cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.vlp_rec_confidence,
                               cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_out_flag);
                }
            }
            else
            {

                if (!(cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str != "-" &&
                      cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_count > 2 &&
                      cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.vlp_rec_confidence > 0.9))
                {
                    continue;
                }

                if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_out_flag == 2 &&
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_out == 0)
                {
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_judge_suspected_out = true;
                    // TODO   生成疑似驶出订单
                    SonLiLogger::getInstance()
                        .getLogger(SonLiLogger::RotateLogger)
                        ->warn("-----suspected out 2 ---- camera_id {}, lp_str {}, lp_count {}, lp_conf {}, "
                               "no_send_out_flag {}",
                               cache_veh_infos_.camera_channel,
                               cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str,
                               cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_count,
                               cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.vlp_rec_confidence,
                               cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_out_flag);
                }
                else
                {
                    if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_out_flag > 0)
                    {
                        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_judge_suspected_out = true;
                        // TODO   生成疑似驶出订单
                        SonLiLogger::getInstance()
                            .getLogger(SonLiLogger::RotateLogger)
                            ->warn("-----suspected out 3 ---- camera_id {}, lp_str {}, lp_count {}, lp_conf {}, "
                                   "no_send_out_flag {}",
                                   cache_veh_infos_.camera_channel,
                                   cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str,
                                   cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_count,
                                   cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.vlp_rec_confidence,
                                   cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_out_flag);
                    }
                }
            }
        }
    }
}

} // namespace sonli

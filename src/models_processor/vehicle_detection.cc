//
// Created by gp147 on 2025/8/5.
//

#include "vehicle_detection.h"
#include "../utils/utils.h"
#include "model_process.h"
#include "../utils/sl_logger.h"

#include "../utils/time_utils.h"

using namespace std;
using namespace sonli;

enum class InputOutputId
{
    INPUT_IMG_ID = 0,

    OUTPUT_VEH_TYPE_L_ID = 0,
    // OUTPUT_VEH_TYPE_M_ID = 1,
    // OUTPUT_VEH_TYPE_H_ID = 2,

    OUTPUT_SCORE_L_ID = 3,
    // OUTPUT_SCORE_M_ID,
    // OUTPUT_SCORE_H_ID,

    OUTPUT_COORD_L_ID = 6,
    // OUTPUT_COORD_M_ID = 7,
    // OUTPUT_COORD_H_ID = 8,

    OUTPUT_KEY_POINTS_L_ID = 9,
    // OUTPUT_KEY_POINTS_M_ID = 10,
    // OUTPUT_KEY_POINTS_H_ID = 11,

    OUTPUT_KEY_POINTS_V_L_ID = 12,
    // OUTPUT_KEY_POINTS_V_M_ID = 13,
    // OUTPUT_KEY_POINTS_V_H_ID = 14,
};

template <typename T>
bool
IsImageEdge(int frame_height, int frame_width, T box)
{
    if (frame_height < 200 || frame_width < 200)
    {
        return false;
    }

    int h_rat = frame_height / 20;
    int w_rat = frame_width / 20;
    if (box.x_ < w_rat || box.y_ < h_rat || (box.x_ + box.width_) > (frame_width - w_rat) ||
        (box.y_ + box.height_) > (frame_height - h_rat))
    {
        return true;
    }
    return false;
}

VehicleDetection::VehicleDetection()
{
}

void VehicleDetection::InitVehicleDetection(){

    std::vector<int32_t> input_dims =  getInputShape(0);

SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
        "VehicleDetection  input_dims index 0, input_dims[3]={}, input_dims[2]={}", input_dims[3], input_dims[2]);
    config_.net_input_height_ = input_dims[2];
    config_.net_input_width_ = input_dims[3];

    int head_size = 3;
    for (int i = 0; i < head_size; i++)
    {
        std::vector<int32_t> output_dims =  getOutputShape(i);
SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
        "VehicleDetection  output_dims index {}, output_dims[3]={}, output_dims[2]={}", i, output_dims[3], output_dims[2]);

        std::vector<model_datatype::GridAndStride> grid_strides;
        sonli::generate_grids_and_stride_yolox(input_dims[3], input_dims[2], output_dims[3], output_dims[2], grid_strides);
        grid_strides_vec_.push_back(grid_strides);
    }
}

VehicleDetection::~VehicleDetection() {}

int32_t 
VehicleDetection::preProcess(void *data)
{
    int ret =  CreateInputBuf(data);

    if (ret != SL_SUCCESS)
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("veh det CreateInputBuf failed!");
        return ERR_PROCESS_FAILED;
    }


    ret =  CreateTaskBufAndWorkBuf();
    if (ret != SL_SUCCESS)
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("veh det CreateTaskBufAndWorkBuf failed!");
        return ERR_PROCESS_FAILED;
    }

    return SL_SUCCESS;
}

int32_t 
VehicleDetection::inference(){
    return Execute();
}

void
VehicleDetection::getProposals(std::vector<model_datatype::Kpt_Object> &proposals) const
{
    int head_size = 3;
    for (int i = 0; i < head_size; i++)
    {
        auto *feat_cls = reinterpret_cast<float *>(svp_acl_get_data_buffer_addr(
            svp_acl_mdl_get_dataset_buffer( output_, static_cast<size_t>(InputOutputId::OUTPUT_VEH_TYPE_L_ID) + i)));

        auto *feat_obj = reinterpret_cast<float *>(svp_acl_get_data_buffer_addr(
            svp_acl_mdl_get_dataset_buffer( output_, static_cast<size_t>(InputOutputId::OUTPUT_SCORE_L_ID) + i)));

        auto *feat_bbox = reinterpret_cast<float *>(svp_acl_get_data_buffer_addr(
            svp_acl_mdl_get_dataset_buffer( output_, static_cast<size_t>(InputOutputId::OUTPUT_COORD_L_ID) + i)));

        auto *feat_kpt = reinterpret_cast<float *>(svp_acl_get_data_buffer_addr(
            svp_acl_mdl_get_dataset_buffer( output_, static_cast<size_t>(InputOutputId::OUTPUT_KEY_POINTS_L_ID) + i)));

        auto *feat_vis = reinterpret_cast<float *>(svp_acl_get_data_buffer_addr(
            svp_acl_mdl_get_dataset_buffer( output_, static_cast<size_t>(InputOutputId::OUTPUT_KEY_POINTS_V_L_ID) + i)));

        sonli::decode_yolox_pose_detect<float>(feat_cls, feat_obj, feat_bbox, feat_kpt, feat_vis, proposals,
                                               config_.veh_num_class_, config_.veh_confi_thresh_, grid_strides_vec_[i]);
    }
}

int32_t VehicleDetection::postProcess(VehInfoVec& vehs_info,
                                      unsigned long long image_timestamp)
{
    std::vector<model_datatype::Kpt_Object> proposals;
    getProposals(proposals);

    if (proposals.size() > 0)
    {
        qsort_descent_inplace<model_datatype::Kpt_Object>(proposals, 0, proposals.size() - 1);
    }
    else
    {
        vehs_info.size = 0;
        return SL_SUCCESS;
    }

    std::vector<int> picked;
    nms_sorted_bboxes<model_datatype::Kpt_Object>(proposals, picked, config_.veh_nms_threshold_);

    float scale_w = (float)config_.img_width_ / config_.net_input_width_;
    float scale_h = (float)config_.img_height_ / config_.net_input_height_;
    float scale_max = std::fmax(scale_w, scale_h);
    scale_max = std::fmax(scale_max, 1.0);

    vehs_info.size = 0;
    for (int i = 0; i < picked.size(); i++)
    {
        VehInfo veh_info;
        veh_info.veh_det_confidence = std::fmax(proposals[picked[i]].score_, proposals[picked[i]].label_score_);
        // veh_info.veh_det_confidence = proposals[picked[i]].score_;
        if (veh_info.veh_det_confidence < config_.veh_confi_thresh_)
        {
            continue;
        }

        int x0 = (proposals[picked[i]].rect_.x_) * scale_w;
        int y0 = (proposals[picked[i]].rect_.y_) * scale_h;
        int x1 = (proposals[picked[i]].rect_.x_ + proposals[picked[i]].rect_.width_) * scale_w;
        int y1 = (proposals[picked[i]].rect_.y_ + proposals[picked[i]].rect_.height_) * scale_h;

        x0 = std::max(std::min(x0, config_.img_width_ - 1), 0);
        y0 = std::max(std::min(y0, config_.img_height_ - 1), 0);
        x1 = std::max(std::min(x1, config_.img_width_ - 1), 0);
        y1 = std::max(std::min(y1, config_.img_height_ - 1), 0);
        
        veh_info.location = Rect(x0, y0, x1 - x0, y1 - y0);

        if(veh_info.location.width_ > (config_.img_width_ * 0.8) || veh_info.location.height_ > (config_.img_height_ * 0.8)){
            continue;
        }

        if (veh_info.location.width_ <= 35 || veh_info.location.height_ <= 18)
        {
            continue;
        }
        
        // 14 = 224 / 16
        if (veh_info.location.width_ <= scale_max * 14 || veh_info.location.height_ <= 14 * scale_max)
        {
            continue;
        }


        veh_info.is_image_edge = IsImageEdge(config_.img_height_, config_.img_width_, veh_info.location);

        veh_info.veh_type_conf = proposals[picked[i]].label_score_;
        if (proposals[picked[i]].label_ == 0)
        {
            veh_info.veh_type = SL_VEHICLE_TYPE_SEDAN;
        }
        else if (proposals[picked[i]].label_ == 1)
        {
            veh_info.veh_type = SL_VEHICLE_TYPE_SUV;
        }

        std::vector<Point> landmark;
        float roof_visual_rate = 0.0;
        float visual_rate = 0.0;
        for (int coord_idx = 0; coord_idx < 16; coord_idx++)
        {
            int x = int(proposals[picked[i]].veh_key_points_[coord_idx].x * scale_w);
            int y = int(proposals[picked[i]].veh_key_points_[coord_idx].y * scale_h);
            veh_info.veh_key_points[coord_idx].x = x;
            veh_info.veh_key_points[coord_idx].y = y;
            veh_info.veh_key_points[coord_idx].vis_score = proposals[picked[i]].veh_key_points_[coord_idx].vis_score;
            if (coord_idx < 8 || coord_idx == 12 || coord_idx == 13)
            {
                visual_rate = visual_rate + veh_info.veh_key_points[coord_idx].vis_score / 10.0;
            }
            else if(coord_idx < 12 && coord_idx > 7)
            {
                roof_visual_rate = roof_visual_rate + veh_info.veh_key_points[coord_idx].vis_score / 4.0;
            }
 
            if (coord_idx < 4)
            {
                landmark.emplace_back(Point(x, y));
            }
        }
        veh_info.visual_rate = visual_rate * roof_visual_rate;

        veh_info.veh_chassis.chassis = landmark;
        if (landmark[0].x_ - landmark[1].x_ != 0 && landmark[3].x_ - landmark[2].x_ != 0)
        {
            veh_info.veh_chassis.gradient =
                (0.5 * (landmark[0].y_ - landmark[1].y_) / (landmark[0].x_ - landmark[1].x_)) +
                (0.5 * (landmark[3].y_ - landmark[2].y_) / (landmark[3].x_ - landmark[2].x_));
        }

        Rect2f pose_hull_location = expandConvexHull(veh_info.veh_key_points);
        int x0_p = std::max(std::min((int)pose_hull_location.x_, config_.img_width_ - 1), 0);
        int y0_p = std::max(std::min((int)pose_hull_location.y_, config_.img_height_ - 1), 0);
        int x1_p = std::max(std::min(x0_p + (int)pose_hull_location.width_, config_.img_width_ - 1), 0);
        int y1_p = std::max(std::min(y0_p + (int)pose_hull_location.height_, config_.img_height_ - 1), 0);
        veh_info.pose_hull_location = Rect(x0_p, y0_p, x1_p - x0_p, y1_p - y0_p);

        veh_info.veh_timestamp = image_timestamp;
        veh_info.time_info = vehs_info.time_info;
        vehs_info.veh_info.emplace_back(veh_info);
        vehs_info.size++;
    }


    // std::cout << "Get <" << vehs_info.veh_info.size() << "> vehicles" << std::endl;
    for (int i = 0; i < vehs_info.veh_info.size(); i++)
    {
        for (int j = 0; j < vehs_info.veh_info.size(); j++)
        {
            if (i == j)
            {
                continue;
            }

            int top_y_i = vehs_info.veh_info[i].location.y_;
            int bottom_y_i = vehs_info.veh_info[i].location.y_ + vehs_info.veh_info[i].location.height_;
            if ((vehs_info.veh_info[j].location.y_ > top_y_i && vehs_info.veh_info[j].location.y_ < bottom_y_i) ||
                (vehs_info.veh_info[j].location.y_ < top_y_i &&
                 vehs_info.veh_info[j].location.y_ + vehs_info.veh_info[j].location.height_ > bottom_y_i))
            {
                float pIOU = 0.0;
                float carRat1 = 0.0;
                float carRat2 = 0.0;

                pIOU = cal_iou(vehs_info.veh_info[i].location, vehs_info.veh_info[j].location, carRat1, carRat2);
                if (carRat1 > vehs_info.veh_info[i].cover_rat)
                {
                    vehs_info.veh_info[i].cover_rat = carRat1; // 加入遮挡比率
                }
            }
        }
    }
    return SL_SUCCESS;
}

int32_t VehicleDetection::vehDetProcess(sonli::VehInfoVec &vehs_info,
                                        unsigned long long image_timestamp,
                                        void *data)
{

time_t s_time = getSystemTimeUS();
    int ret = preProcess(data);
// std::cout << " vehDetProcess  preProcess " << getSystemTimeUS() - s_time << std::endl;



    if (ret != SL_SUCCESS)
    {
        return ret;
    }

    ret = inference();

// std::cout << " vehDetProcess  inference " << getSystemTimeUS() - s_time << std::endl;

    if (ret != SL_SUCCESS)
    {
        return ret;
    }

    ret = postProcess(vehs_info, image_timestamp);

// std::cout << " vehDetProcess  postProcess " << getSystemTimeUS() - s_time << std::endl;

    if (ret != SL_SUCCESS)
    {
        return ret;
    }
    return SL_SUCCESS;
}

#include "aih_eventinfo_convert.h"

#include "cached_frames_manager.h"
#include "../interfaces/aih_type.h"
#include "../utils/sl_logger.h"
#include "../utils/struct_2_string.h"
#include "../models_processor/char_code.h"

namespace sonli
{


/**
 * 转换车辆颜色
 * @param vehicle_color 车辆颜色
 * @return aih::CAR_COLOR 转换后的车辆颜色
 */
static aih::CAR_COLOR
CAR_COLOR_CONVERT(SLVehicleColor_t vehicle_color)
{
    switch (vehicle_color)
    {
        case SL_VEHICLE_COLOR_UNKNOWN:
            return aih::COLOR_OTHER;

        case SL_VEHICLE_COLOR_YELLOW:
            return aih::COLOR_OTHER;

        case SL_VEHICLE_COLOR_ORANGE:
            return aih::COLOR_RED;

        case SL_VEHICLE_COLOR_GREEN:
            return aih::COLOR_OTHER;

        case SL_VEHICLE_COLOR_GRAY:
            return aih::COLOR_SILVER;

        case SL_VEHICLE_COLOR_RED:
            return aih::COLOR_RED;

        case SL_VEHICLE_COLOR_BLUE:
            return aih::COLOR_BLUE;

        case SL_VEHICLE_COLOR_WHITE:
            return aih::COLOR_WHITE;

        case SL_VEHICLE_COLOR_GOLDEN:
            return aih::COLOR_OTHER;

        case SL_VEHICLE_COLOR_BROWN:
            return aih::COLOR_BROWN;

        case SL_VEHICLE_COLOR_BLACK:
            return aih::COLOR_BLACK;

        default:
            return aih::COLOR_OTHER;
    }
}

/**
 * 转换车辆类型
 * @param vehicle_type 车辆类型
 * @return aih::BRAND_TYPE 转换后的车辆类型
 */
static aih::BRAND_TYPE
CAR_TYPE_CONVERT(SLVehicleType_t vehicle_type)
{
    switch (vehicle_type)
    {
        case SL_VEHICLE_TYPE_UNKNOWN:
            return aih::TYPE_CAR;

        case SL_VEHICLE_TYPE_SEDAN:
            return aih::TYPE_BREAD;

        case SL_VEHICLE_TYPE_SUV:
            return aih::TYPE_SUV;

        case SL_VEHICLE_TYPE_VAN:
            return aih::TYPE_MPV;

        case SL_VEHICLE_TYPE_HATCHBACK:
            return aih::TYPE_CAR;

        case SL_VEHICLE_TYPE_MPV:
            return aih::TYPE_MPV;

        case SL_VEHICLE_TYPE_PICKUP:
            return aih::TYPE_SMALL_TRUCK;

        case SL_VEHICLE_TYPE_BUS:
            return aih::TYPE_SMALL_BUS;

        case SL_VEHICLE_TYPE_TRUCK:
            return aih::TYPE_SMALL_TRUCK;

        case SL_VEHICLE_TYPE_ESTATE:
            return aih::TYPE_BREAD;

        default:
            return aih::TYPE_CAR;
    }
}

/**
 * 转换车牌类型
 * @param plate_type 车牌类型
 * @return aih::PLATE_TYPE 转换后的车牌类型
 */
static aih::PLATE_TYPE
PLATE_TYPE_CONVERT(SLLPType_t plate_type)
{
    switch (plate_type)
    {
        case SL_PLATE_UNCERTAIN:
            return aih::LT_UNKNOWN;

        case SL_PLATE_BLUE:
            return aih::LT_BLUE;

        case SL_PLATE_YELLOW:
            return aih::LT_YELLOW;

        case SL_PLATE_POLICE:
            return aih::LT_POLICE;

        case SL_PLATE_WUJING:
            return aih::LT_ARMPOL;

        case SL_PLATE_DBYELLOW:
            return aih::LT_YELLOW2;

        case SL_PLATE_MOTOR:
            return aih::LT_UNKNOWN;

        case SL_PLATE_INSTRUCTIONCAR:
            return aih::LT_COACH;

        case SL_PLATE_MILITARY:
            return aih::LT_ARMY;

        case SL_PLATE_PERSONAL:
            return aih::LT_SELF_DEFINED;

        case SL_PLATE_GANGAO:
            return aih::LT_HONGKONG;

        case SL_PLATE_EMBASSY:
            return aih::LT_EMBASSY;

        case SL_PLATE_NONGLARE:
            return aih::LT_UNKNOWN;

        case SL_PLATE_AVIATION:
            return aih::LT_UNKNOWN;

        case SL_PLATE_NEWENERGY:
            return aih::LT_NEW_ENERGY;

        case SL_PLATE_NEWENERGYBIG:
            return aih::LI_ENERGY_LARGE;

        case SL_PLATE_EMERGENCY:
            return aih::LT_EMERGENCY;

        default:
            return aih::LT_UNKNOWN;
    }
}

static aih::PLATE_COLOR
PLATE_COLOR_CONVERT(SLLPColor_t plate_color)
{
    switch (plate_color)
    {
        case SL_PLATE_COLOR_UNKNOWN:
            return aih::LC_UNKNOWN;

        case SL_PLATE_COLOR_BLUE:
            return aih::LC_BLUE;

        case SL_PLATE_COLOR_YELLOW:
            return aih::LC_YELLOW;

        case SL_PLATE_COLOR_WHITE:
            return aih::LC_WHITE;

        case SL_PLATE_COLOR_BLACK:
            return aih::LC_BLACK;

        case SL_PLATE_COLOR_GREEN:
            return aih::LC_GREEN;

        case SL_PLATE_COLOR_GREENBLACK:
            return aih::LC_GREEN;

        case SL_PLATE_COLOR_GREENYELLOW:
            return aih::LC_YELLOW_GREEN;

        default:
            return aih::LC_UNKNOWN;
    }
}

/**
 * 转换Rect到aih::ALG_ObjRect
 * @param rect Rect对象
 * @param obj_rect aih::ALG_ObjRect对象
 */
static void
SLRectConvertAIHALGObject(const Rect &rect, aih::ALG_ObjRect &obj_rect)
{
    obj_rect.x_ = rect.x_;
    obj_rect.y_ = rect.y_;
    obj_rect.width_ = rect.width_;
    obj_rect.height_ = rect.height_;
}

/**
 * 转换 stuffed berth car info
 * @param data VehInfo对象
 * @param car_info aih::BerthCarInfo对象
 */
static void
SLStuffedBerthCarInfo(const VehInfo &data, aih::BerthCarInfo &car_info)
{
    SLRectConvertAIHALGObject(data.location, car_info.car_loc_);
    SLRectConvertAIHALGObject(data.lp_info.location, car_info.plate_loc_);

    car_info.covered_score_ = (1.0 - data.cover_rat) * 100;
    car_info.has_car_body_ = 1;
    car_info.plate_isocc_ = data.veh_key_points[14].vis_score > 0.5 ? 1 : 0;
    car_info.plate_occ_rate_ = data.veh_key_points[14].vis_score;
    car_info.confidence_ = (int)(data.veh_det_confidence * 100);
}

OrderProcess::OrderProcess(const std::shared_ptr<CachedFramesManager> &cached_frames_manager)
    : cached_frames_manager_(cached_frames_manager)
{
}


void
ConvertPlateInfo(aih::PlateInfo &plateinfo, LPInfo lp_info)
{

    plateinfo.plate_score_ = lp_info.lp_number.vlp_rec_confidence * 100;
    plateinfo.plate_type_prob_ = lp_info.vlp_det_confidence * 100;
    if(lp_info.lp_number.lp_str != "-"){
        plateinfo.plate_str_ = lp_info.lp_number.lp_str;
    }
    
    plateinfo.plate_type_id_ = PLATE_TYPE_CONVERT(lp_info.lp_type);
    plateinfo.plate_color_id_ = PLATE_COLOR_CONVERT(lp_info.lp_color);

    //TODO
    // int plate_layer_id_ = 0;     //车牌单双层ID
    // std::string plate_type_str = "";//车牌类型
    // std::string plate_color_str_ = "";//车牌颜色
    // std::string plate_layer_str_ = "";//车牌单、双层

    if(lp_info.lp_number.lp_str != "-"){
        plateinfo.plate_scores_.emplace_back(lp_info.lp_number.first_char_confidence * 100);
        plateinfo.plate_scores_.emplace_back(lp_info.lp_number.second_char_confidence * 100);
    }
    std::wstring veh_out_lp_strw = utf8ToUnicode(lp_info.lp_number.lp_str);
    for (int i = 0; i < veh_out_lp_strw.length(); i++)
    {
        // plateinfo.plate_chars_.emplace_back(1, ch);
        if (i > 1)
        {
            plateinfo.plate_scores_.emplace_back(lp_info.lp_number.vlp_rec_confidence * 100);
        }
    }
}

void
ConvertPlateInfo(aih::PlateInfoExt &plateinfos, LPInfo lp_info)
{
    plateinfos.plate_score = lp_info.vlp_det_confidence * 100;
    plateinfos.plate_width = lp_info.location.width_;
    plateinfos.plate_loc.x_ = lp_info.location.x_;
    plateinfos.plate_loc.y_ = lp_info.location.y_;
    plateinfos.plate_loc.width_ = lp_info.location.width_;
    plateinfos.plate_loc.height_ = lp_info.location.height_;

    ConvertPlateInfo(plateinfos.info, lp_info);
}

void
ConvertCarInfo(aih::BerthCarInfo &berth_car_info, VehInfo sl_car_info)
{
    Rect car_location = sl_car_info.location;
    SLRectConvertAIHALGObject(car_location, berth_car_info.car_loc_);

    Rect lp_location = sl_car_info.lp_info.location;
    if(car_location.width_ < 10){
        lp_location.width_ = 0;
        lp_location.height_ = 0;
    }
    SLRectConvertAIHALGObject(lp_location, berth_car_info.plate_loc_);
    ConvertPlateInfo(berth_car_info.plate_info_, sl_car_info.lp_info);

    berth_car_info.covered_score_ = (1.0 - sl_car_info.cover_rat) * 100;
    berth_car_info.has_car_body_ = 1;
    if (sl_car_info.lp_info.vlp_det_confidence < 0.3)
    {
        berth_car_info.plate_isocc_ = 1;
        berth_car_info.plate_occ_rate_ = 1.0;
    }
    else
    {
        berth_car_info.plate_isocc_ = 0;
        berth_car_info.plate_occ_rate_ = 0.0;
    }
    berth_car_info.confidence_ = sl_car_info.veh_det_confidence;

}

void
CarPropertyConvert(aih::CarProperty &car_property, VehInfo sl_veh_info)
{
    car_property.car_plate_score_ = sl_veh_info.lp_info.vlp_det_confidence * 100;
    car_property.car_plate_width_ = sl_veh_info.lp_info.location.width_;
    car_property.car_stock_score_ = 0.0;
    car_property.car_stock_width_ = 0.0;
    car_property.car_body_score_ = sl_veh_info.veh_det_confidence * 100;
    car_property.car_body_width_ = sl_veh_info.location.width_;

    aih::PlateInfoExt plate_info_ext;
    ConvertPlateInfo(plate_info_ext, sl_veh_info.lp_info);
    car_property.plate_info_ = plate_info_ext.info;
    car_property.plate_infos_.push_back(plate_info_ext);

    car_property.car_type_.type_id_ = CAR_TYPE_CONVERT(sl_veh_info.veh_type);
    car_property.car_type_.type_str_ = aih::type_pair[car_property.car_type_.type_id_];
    car_property.car_type_.type_score_ = 100.0f;

    car_property.car_color_.type_id_ = CAR_COLOR_CONVERT(sl_veh_info.veh_color);
    car_property.car_color_.type_str_ = aih::color_pair[car_property.car_color_.type_id_];
    car_property.car_color_.type_score_ = 100.0f;

    // car_property.car_brand_.type_id_ = ;
    // car_property.car_brand_.type_str_ = ;
    // car_property.car_brand_.type_score_ = ;
}

OrderProcess *
OrderProcess::ConvertAihOrder(const VehInOutInfoVec &original_orders,
                              aih::TimeInfo time_info,
                              aih::EventInfos &aih_orders,
                              std::shared_ptr<sonli::ParkingInfoVec> parking_info_ptr)
{
    discard_orders_.clear();
    if (parking_info_ptr->parking_info.size() == 0)
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error(
            "ConvertAihOrder parking_info.size {}", 
            parking_info_ptr->parking_info.size());

        aih::EventInfo aih_order;
        aih_order.event_type_ = aih::EVENT_TYPE_BERTH_REALTIME;
        aih_order.berth_event_info_.result_cnt_ = 1;
        aih_order.time_info_ = time_info;
        aih_order.berth_event_info_.parking_state_[0] = aih::LoopStatus::LOOP_STATUS_FREE;
        aih_order.berth_event_info_.frame_info_[0].tm_info_ = time_info;

        aih_orders.emplace_back(aih_order);
        return this;
    }

    if (original_orders.size == 0 || original_orders.veh_inout_info.empty())
    {
        int space_id = 0;
        aih::EventInfo aih_order;
        aih_order.event_type_ = aih::EVENT_TYPE_BERTH_REALTIME;
        aih_order.berth_event_info_.result_cnt_ = 1;
        aih_order.berth_event_info_.parking_state_[0] = aih::LoopStatus::LOOP_STATUS_FREE;
        aih_order.berth_event_info_.frame_info_[0].tm_info_ = time_info;

        for(int space_id = 0; space_id < parking_info_ptr->parking_info.size(); space_id++){
            const auto &parking_cfg = parking_info_ptr->parking_info[space_id];
            aih_order.control_id_ = parking_cfg.berth_event_list.control_id;
            aih_order.event_id_ = parking_cfg.berth_event_list.event_id;
            aih_order.time_info_ = time_info;
            aih_orders.emplace_back(aih_order);
        }

        return this;
    }

    for (const auto &ori_order : original_orders.veh_inout_info)
    {
        aih::EventInfo aih_order;

        int space_id = -1;
        // 事件类型
        aih_order.event_type_ = aih::EVENT_TYPE_BERTH_REALTIME;
        if (ori_order.in_out == 1)
        {
            aih_order.event_type_ = aih::EVENT_TYPE_ENTER_BERTH;
            aih_order.berth_event_info_.parking_state_[0] = aih::LoopStatus::LOOP_STATUS_IN;
            space_id = ori_order.pro_veh_info.veh_chassis.space_id;
            if (space_id < 0 && parking_info_ptr->parking_info.size() > 0)
            {
                SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
                    "veh_in space_id -1 ! track_id {},  lp_str {}",
                    ori_order.pro_veh_info.veh_track_id,
                    ori_order.pro_veh_info.lp_info.lp_number.lp_str);

                space_id = 0;
            }
        }
        else if (ori_order.in_out == 0)
        {
            aih_order.event_type_ = aih::EVENT_TYPE_LEAVE_BERTH;
            aih_order.berth_event_info_.parking_state_[0] = aih::LoopStatus::LOOP_STATUS_OUT;
            space_id = ori_order.pro_veh_info.veh_chassis.space_id;
            if (space_id < 0 && parking_info_ptr->parking_info.size() > 0)
            {
                SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
                    "veh_out space_id -1 ! track_id {},  lp_str {}",
                    ori_order.pro_veh_info.veh_track_id,
                    ori_order.pro_veh_info.lp_info.lp_number.lp_str);

                space_id = 0;
            }
        }
        else
        {
            // SonLiLogger::getInstance()
            //     .getLogger(SonLiLogger::RotateLogger)
            //     ->info("OrderProcess::ConvertAihOrder discard one order : ori_order.in_out [ {} ]", ori_order.in_out);

            if (ori_order.veh_state == SL_CAR_STATE_NOON)
            {
                SonLiLogger::getInstance()
                    .getLogger(SonLiLogger::RotateLogger)
                    ->info(
                        "OrderProcess::ConvertAihOrder discard one order : ori_order.in_out [ {} ], _SL_CAR_STATE_NOON_veh_state [ {} ]",
                        ori_order.in_out, ori_order.veh_state);
                continue;
            }
            else
            {
                if (ori_order.veh_state == SL_CAR_STATE_IN_PRE ||
                    ori_order.veh_state == SL_CAR_STATE_IN ||
                    ori_order.veh_state == SL_CAR_STATE_STOP)
                {
                    aih_order.event_type_ = aih::EVENT_TYPE_BERTH_REALTIME;
                }
                else
                {
                    aih_order.event_type_ = aih::EVENT_TYPE_BERTH_REALTIME;
                }
                if (ori_order.veh_state == SL_CAR_STATE_IN_PRE ||
                    ori_order.veh_state == SL_CAR_STATE_OUT)
                {
                    space_id = ori_order.out_veh_info.veh_chassis.space_id;
                }
                else if (ori_order.veh_state == SL_CAR_STATE_STOP ||
                         ori_order.veh_state == SL_CAR_STATE_STOP_LEAVE)
                {
                    space_id = ori_order.in_veh_info.veh_chassis.space_id;
                }
                else
                {
                    space_id = ori_order.pro_veh_info.veh_chassis.space_id;
                }

                if (space_id == -1)
                {
                    // SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("ConvertAihOrder  event space_id = -1 !!!");
                    space_id = 0;
                }
            }
            aih_order.event_type_ = aih::EVENT_TYPE_BERTH_REALTIME;
        }
        if (space_id == -1)
        {
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->
                                       error("ConvertAihOrder  space_id = -1 !!!");
            continue;
        }

        if (parking_info_ptr->parking_info.size() <= space_id)
        {
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error(
                "ConvertAihOrder  event space_id error {}, parking_info.size {}",
                space_id,
                parking_info_ptr->parking_info.size());

            return this;
        }

        const auto &parking_cfg = parking_info_ptr->parking_info[space_id];
        aih_order.control_id_ = parking_cfg.berth_event_list.control_id;
        aih_order.event_id_ = parking_cfg.berth_event_list.event_id;

        // 补充时间戳
        aih_order.time_info_ = time_info;

        if (ori_order.in_out == 1)
        {
            aih_order.berth_event_info_.result_cnt_ = 3;
            CarPropertyConvert(aih_order.berth_event_info_.car_property_, ori_order.pro_veh_info);
            aih_order.berth_event_info_.sharpness_[0] = ori_order.out_veh_info.lp_info.vlp_det_confidence;
            aih_order.berth_event_info_.sharpness_[1] = ori_order.pro_veh_info.lp_info.vlp_det_confidence;
            aih_order.berth_event_info_.sharpness_[2] = ori_order.in_veh_info.lp_info.vlp_det_confidence;

            int plate_mean = ori_order.out_veh_info.lp_info.location.width_;
            if(ori_order.pro_veh_info.lp_info.location.width_ > plate_mean){
                plate_mean = ori_order.pro_veh_info.lp_info.location.width_;
            }
            if(ori_order.in_veh_info.lp_info.location.width_ > plate_mean){
                plate_mean = ori_order.in_veh_info.lp_info.location.width_;
            }
            if(plate_mean < 10){
                plate_mean = 0;
            }

            aih_order.berth_event_info_.plate_mean_[0] = plate_mean; //ori_order.out_veh_info.lp_info.location.width_;
            aih_order.berth_event_info_.plate_mean_[1] = plate_mean; //ori_order.pro_veh_info.lp_info.location.width_;
            aih_order.berth_event_info_.plate_mean_[2] = plate_mean; //ori_order.in_veh_info.lp_info.location.width_;

            aih_order.berth_event_info_.can_drop_[0] = 0;
            aih_order.berth_event_info_.can_drop_[1] = 0;
            aih_order.berth_event_info_.can_drop_[2] = 0;

            aih_order.berth_event_info_.parking_state_[0] = aih::LoopStatus::LOOP_STATUS_PREIN;
            aih_order.berth_event_info_.parking_state_[1] = aih::LoopStatus::LOOP_STATUS_IN;
            aih_order.berth_event_info_.parking_state_[2] = aih::LoopStatus::LOOP_STATUS_STABLE;

            ConvertCarInfo(aih_order.berth_event_info_.car_info_[0], ori_order.out_veh_info);
            if(aih_order.berth_event_info_.car_info_[0].plate_loc_.width_ < 10){
                aih_order.berth_event_info_.car_info_[0].plate_loc_.width_ = plate_mean;
            }
            ConvertCarInfo(aih_order.berth_event_info_.car_info_[1], ori_order.pro_veh_info);
            ConvertCarInfo(aih_order.berth_event_info_.car_info_[2], ori_order.in_veh_info);
            aih_order.berth_event_info_.car_info_[0].plate_info_.plate_chars_ = aih_order.berth_event_info_.car_info_[1].plate_info_.plate_chars_;
            aih_order.berth_event_info_.car_info_[2].plate_info_.plate_chars_ = aih_order.berth_event_info_.car_info_[1].plate_info_.plate_chars_;
            aih_order.berth_event_info_.car_info_[0].plate_info_.plate_type_id_ = aih_order.berth_event_info_.car_info_[1].plate_info_.plate_type_id_;
            aih_order.berth_event_info_.car_info_[2].plate_info_.plate_type_id_ = aih_order.berth_event_info_.car_info_[1].plate_info_.plate_type_id_;
            aih_order.berth_event_info_.car_info_[0].plate_info_.plate_color_id_ = aih_order.berth_event_info_.car_info_[1].plate_info_.plate_color_id_;
            aih_order.berth_event_info_.car_info_[2].plate_info_.plate_color_id_ = aih_order.berth_event_info_.car_info_[1].plate_info_.plate_color_id_;

            aih_order.berth_event_info_.frame_info_[2].tm_info_ = cached_frames_manager_->FindTimeInfo(
                ori_order.in_veh_info.time_info.pts_);
            aih_order.berth_event_info_.frame_info_[1].tm_info_ = cached_frames_manager_->FindTimeInfo(
                ori_order.pro_veh_info.time_info.pts_);
            aih_order.berth_event_info_.frame_info_[0].tm_info_ = cached_frames_manager_->FindTimeInfo(
                ori_order.out_veh_info.time_info.pts_);

            // U32 car_gesture_ = 0;      //0无效，1车头，2车尾
            aih_order.berth_event_info_.order_score_ = ori_order.veh_state_fraction;
            // U32 covered_event = 0;      //遮挡事件，0无遮挡，1遮挡
            // U64 enter_start_timestamp_ = 0;      //进入泊位开始时间戳
            // U32 force_leave_flag_ = 0;      //强制离开标志
            // U32 group_id_ = -1;      //泊位组ID
            // U32 non_motor_car_type_ = 0;      //非机动车类型
            // U32 non_motor_id_ = -1;      //非机动车id
            // U32 out_video_start_ = 0;      //开始录制视频的标志，0:不处理，1:出场, 2 : 入场
            // U32 recent_plate_ = 0;      //车牌是否移动过
            // U32 plate_covered_ = 0;      //车牌是否被遮挡
            // U32 car_stable_count_ = 0;      //车辆停稳的帧计数

            // SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
            //     "ConvertAihOrder  inout 1 pts_time {} {} {}",
            //     ori_order.in_veh_info.time_info.pts_,
            //     ori_order.pro_veh_info.time_info.pts_,
            //     ori_order.out_veh_info.time_info.pts_);
        }
        else if (ori_order.in_out == 0)
        {
            aih_order.berth_event_info_.result_cnt_ = 3;
            CarPropertyConvert(aih_order.berth_event_info_.car_property_, ori_order.pro_veh_info);
            aih_order.berth_event_info_.sharpness_[0] = ori_order.in_veh_info.lp_info.vlp_det_confidence;
            aih_order.berth_event_info_.sharpness_[1] = ori_order.pro_veh_info.lp_info.vlp_det_confidence;
            aih_order.berth_event_info_.sharpness_[2] = 0.0;


            int plate_mean = ori_order.out_veh_info.lp_info.location.width_;
            if(ori_order.pro_veh_info.lp_info.location.width_ > plate_mean){
                plate_mean = ori_order.pro_veh_info.lp_info.location.width_;
            }
            if(ori_order.in_veh_info.lp_info.location.width_ > plate_mean){
                plate_mean = ori_order.in_veh_info.lp_info.location.width_;
            }
            if(plate_mean < 10){
                plate_mean = 0;
            }

            aih_order.berth_event_info_.plate_mean_[0] = plate_mean; //ori_order.out_veh_info.lp_info.location.width_;
            aih_order.berth_event_info_.plate_mean_[1] = plate_mean; //ori_order.pro_veh_info.lp_info.location.width_;
            aih_order.berth_event_info_.plate_mean_[2] = plate_mean; //ori_order.in_veh_info.lp_info.location.width_;


            // aih_order.berth_event_info_.plate_mean_[0] = ori_order.in_veh_info.lp_info.location.width_;
            // aih_order.berth_event_info_.plate_mean_[1] = ori_order.pro_veh_info.lp_info.location.width_;
            // aih_order.berth_event_info_.plate_mean_[2] = ori_order.out_veh_info.lp_info.location.width_;

            aih_order.berth_event_info_.can_drop_[0] = 0;
            aih_order.berth_event_info_.can_drop_[1] = 0;
            aih_order.berth_event_info_.can_drop_[2] = 0;

            aih_order.berth_event_info_.parking_state_[0] = aih::LoopStatus::LOOP_STATUS_PREOUT;
            aih_order.berth_event_info_.parking_state_[1] = aih::LoopStatus::LOOP_STATUS_OUT;
            aih_order.berth_event_info_.parking_state_[2] = aih::LoopStatus::LOOP_STATUS_FREE;

            ConvertCarInfo(aih_order.berth_event_info_.car_info_[2], ori_order.out_veh_info);
            ConvertCarInfo(aih_order.berth_event_info_.car_info_[1], ori_order.pro_veh_info);
            ConvertCarInfo(aih_order.berth_event_info_.car_info_[0], ori_order.in_veh_info);
            if(aih_order.berth_event_info_.car_info_[0].plate_loc_.width_ < 10){
                aih_order.berth_event_info_.car_info_[0].plate_loc_.width_ = plate_mean;
            }

            aih_order.berth_event_info_.car_info_[0].plate_info_.plate_chars_ = aih_order.berth_event_info_.car_info_[1].plate_info_.plate_chars_;
            aih_order.berth_event_info_.car_info_[2].plate_info_.plate_chars_ = aih_order.berth_event_info_.car_info_[1].plate_info_.plate_chars_;
            aih_order.berth_event_info_.car_info_[0].plate_info_.plate_type_id_ = aih_order.berth_event_info_.car_info_[1].plate_info_.plate_type_id_;
            aih_order.berth_event_info_.car_info_[2].plate_info_.plate_type_id_ = aih_order.berth_event_info_.car_info_[1].plate_info_.plate_type_id_;
            aih_order.berth_event_info_.car_info_[0].plate_info_.plate_color_id_ = aih_order.berth_event_info_.car_info_[1].plate_info_.plate_color_id_;
            aih_order.berth_event_info_.car_info_[2].plate_info_.plate_color_id_ = aih_order.berth_event_info_.car_info_[1].plate_info_.plate_color_id_;


            aih_order.berth_event_info_.frame_info_[0].tm_info_ = cached_frames_manager_->FindTimeInfo(
                ori_order.in_veh_info.time_info.pts_);
            aih_order.berth_event_info_.frame_info_[1].tm_info_ = cached_frames_manager_->FindTimeInfo(
                ori_order.pro_veh_info.time_info.pts_);
            aih_order.berth_event_info_.frame_info_[2].tm_info_ = cached_frames_manager_->FindTimeInfo(
                ori_order.out_veh_info.time_info.pts_);

            // U32 car_gesture_ = 0;      //0无效，1车头，2车尾
            aih_order.berth_event_info_.order_score_ = ori_order.veh_state_fraction;
            // U32 covered_event = 0;      //遮挡事件，0无遮挡，1遮挡
            // U64 enter_start_timestamp_ = 0;      //进入泊位开始时间戳
            // U32 force_leave_flag_ = 0;      //强制离开标志
            // U32 group_id_ = -1;      //泊位组ID
            // U32 non_motor_car_type_ = 0;      //非机动车类型
            // U32 non_motor_id_ = -1;      //非机动车id
            // U32 out_video_start_ = 0;      //开始录制视频的标志，0:不处理，1:出场, 2 : 入场
            // U32 recent_plate_ = 0;      //车牌是否移动过
            // U32 plate_covered_ = 0;      //车牌是否被遮挡
            // U32 car_stable_count_ = 0;      //车辆停稳的帧计数

            // SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
            //     "ConvertAihOrder  inout 0 pts_time {} {} {}",
            //     ori_order.in_veh_info.time_info.pts_,
            //     ori_order.pro_veh_info.time_info.pts_,
            //     ori_order.out_veh_info.time_info.pts_);
        }
        else
        {

            // SonLiLogger::getInstance()
            //     .getLogger(SonLiLogger::RotateLogger)
            //     ->info("---2---OrderProcess::ConvertAihOrder space_id {}, aih_order.event_type_ {}, event_id_: {}",
            //     space_id, aih_order.event_type_, aih_order.event_id_);
            aih_order.berth_event_info_.result_cnt_ = 1;

            if (ori_order.veh_state == SL_CAR_STATE_IN_PRE || ori_order.veh_state == SL_CAR_STATE_OUT)
            {
                CarPropertyConvert(aih_order.berth_event_info_.car_property_, ori_order.out_veh_info);
                aih_order.berth_event_info_.sharpness_[0] = ori_order.out_veh_info.lp_info.vlp_det_confidence;
                aih_order.berth_event_info_.plate_mean_[0] = ori_order.out_veh_info.lp_info.location.width_;
                aih_order.berth_event_info_.can_drop_[0] = 0;
                if (ori_order.veh_state == SL_CAR_STATE_IN_PRE)
                {
                    aih_order.berth_event_info_.parking_state_[0] = aih::LoopStatus::LOOP_STATUS_PREIN;
                }
                else
                {
                    aih_order.berth_event_info_.parking_state_[0] = aih::LoopStatus::LOOP_STATUS_FREE;
                }
                ConvertCarInfo(aih_order.berth_event_info_.car_info_[0], ori_order.out_veh_info);
                aih_order.berth_event_info_.frame_info_[0].tm_info_ = cached_frames_manager_->FindTimeInfo(
                    ori_order.out_veh_info.time_info.pts_);

            }
            else if (ori_order.veh_state == SL_CAR_STATE_IN || ori_order.veh_state == SL_CAR_STATE_LEAVING)
            {

                CarPropertyConvert(aih_order.berth_event_info_.car_property_, ori_order.pro_veh_info);
                aih_order.berth_event_info_.sharpness_[0] = ori_order.pro_veh_info.lp_info.vlp_det_confidence;
                aih_order.berth_event_info_.plate_mean_[0] = ori_order.pro_veh_info.lp_info.location.width_;
                aih_order.berth_event_info_.can_drop_[0] = 0;
                if (ori_order.veh_state == SL_CAR_STATE_IN)
                {
                    aih_order.berth_event_info_.parking_state_[0] = aih::LoopStatus::LOOP_STATUS_IN;
                }
                else
                {
                    aih_order.berth_event_info_.parking_state_[0] = aih::LoopStatus::LOOP_STATUS_OUT;
                }
                ConvertCarInfo(aih_order.berth_event_info_.car_info_[0], ori_order.pro_veh_info);
                aih_order.berth_event_info_.frame_info_[0].tm_info_ = cached_frames_manager_->FindTimeInfo(
                    ori_order.pro_veh_info.time_info.pts_);
            }
            else if (ori_order.veh_state == SL_CAR_STATE_STOP || ori_order.veh_state == SL_CAR_STATE_STOP_LEAVE)
            {
                CarPropertyConvert(aih_order.berth_event_info_.car_property_, ori_order.in_veh_info);
                aih_order.berth_event_info_.sharpness_[0] = ori_order.in_veh_info.lp_info.vlp_det_confidence;
                aih_order.berth_event_info_.plate_mean_[0] = ori_order.in_veh_info.lp_info.location.width_;
                aih_order.berth_event_info_.can_drop_[0] = 0;
                if (ori_order.veh_state == SL_CAR_STATE_STOP)
                {
                    aih_order.berth_event_info_.parking_state_[0] = aih::LoopStatus::LOOP_STATUS_STABLE;
                }
                else
                {
                    aih_order.berth_event_info_.parking_state_[0] = aih::LoopStatus::LOOP_STATUS_PREOUT;
                }
                ConvertCarInfo(aih_order.berth_event_info_.car_info_[0], ori_order.in_veh_info);
                aih_order.berth_event_info_.frame_info_[0].tm_info_ = cached_frames_manager_->FindTimeInfo(
                    ori_order.in_veh_info.time_info.pts_);

            }

            // U32 car_gesture_ = 0;      //0无效，1车头，2车尾
            aih_order.berth_event_info_.order_score_ = ori_order.veh_state_fraction;
            // U32 covered_event = 0;      //遮挡事件，0无遮挡，1遮挡
            // U64 enter_start_timestamp_ = 0;      //进入泊位开始时间戳
            // U32 force_leave_flag_ = 0;      //强制离开标志
            // U32 group_id_ = -1;      //泊位组ID
            // U32 non_motor_car_type_ = 0;      //非机动车类型
            // U32 non_motor_id_ = -1;      //非机动车id
            // U32 out_video_start_ = 0;      //开始录制视频的标志，0:不处理，1:出场, 2 : 入场
            // U32 recent_plate_ = 0;      //车牌是否移动过
            // U32 plate_covered_ = 0;      //车牌是否被遮挡
            // U32 car_stable_count_ = 0;      //车辆停稳的帧计数

            // SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
            //         "ConvertAihOrder  ori_order.veh_state {} pts_time {} {} {}",
            //         ori_order.veh_state,
            //         ori_order.in_veh_info.time_info.pts_,
            //         ori_order.pro_veh_info.time_info.pts_,
            //         ori_order.out_veh_info.time_info.pts_);
        }

        // SonLiLogger::getInstance()
        //         .getLogger(SonLiLogger::RotateLogger)
        //         ->info("---3---OrderProcess::ConvertAihOrder space_id {}, aih_order.event_type_ {}, event_id_: {}",
        //         space_id, aih_order.event_type_, aih_order.event_id_);

        if (aih_order.event_id_ < 0)
        {
            discard_orders_.emplace_back(aih_order);
        }
        else
        {
            aih_orders.emplace_back(aih_order);
        }
    }

    if(aih_orders.size() == 0){
        aih::EventInfo aih_order;
        aih_order.event_type_ = aih::EVENT_TYPE_BERTH_REALTIME;
        aih_order.berth_event_info_.result_cnt_ = 1;
        aih_order.time_info_ = time_info;
        aih_order.berth_event_info_.parking_state_[0] = aih::LoopStatus::LOOP_STATUS_FREE;
        aih_order.berth_event_info_.frame_info_[0].tm_info_ = time_info;

        aih_orders.emplace_back(aih_order);
    }
    return this;
}

aih::EventInfos &
OrderProcess::GetDiscardOrders()
{
    return discard_orders_;
}

void
LineMessageProcess::ConvertLineMessage(const std::vector<VehAccessInfo> &vehicles_,
                                       aih::EventInfos &output_messages)
{
    for (const auto &source : vehicles_)
    {
        std::string lp_str_now = "-";
        if (!source.veh_detail_info.lp_info.lp_number.lp_str.empty() && source.veh_detail_info.lp_info.lp_number.lp_str != "-")
        {
            lp_str_now = source.veh_detail_info.lp_info.lp_number.lp_str;
        }
        else if (!source.vlp.empty() && source.vlp != "-")
        {
            lp_str_now = source.vlp;
        }
        else
        {
            continue;
        }
        
        aih::EventInfo aih_order;
        aih_order.event_type_ = aih::EVENT_TYPE_CROSS_LINE;
        aih_order.control_id_ = source.control_id;
        aih_order.event_id_ = source.event_id;
        aih_order.time_info_ = source.time_info;

        aih_order.berth_event_info_.result_cnt_ = 1;

        CarPropertyConvert(aih_order.berth_event_info_.car_property_, source.veh_detail_info);
        aih_order.berth_event_info_.sharpness_[0] = source.veh_detail_info.lp_info.vlp_det_confidence;

        int plate_mean = source.veh_detail_info.lp_info.location.width_;
        if (plate_mean < 10)
        {
            plate_mean = 0;
        }
        aih_order.berth_event_info_.plate_mean_[0] = plate_mean;

        aih_order.berth_event_info_.can_drop_[0] = 0;

        aih_order.berth_event_info_.parking_state_[0] = aih::LoopStatus::LOOP_STATUS_RECO_TRIGGER;

        ConvertCarInfo(aih_order.berth_event_info_.car_info_[0], source.veh_detail_info);
        if (aih_order.berth_event_info_.car_info_[0].plate_loc_.width_ < 10)
        {
            aih_order.berth_event_info_.car_info_[0].plate_loc_.width_ = plate_mean;
        }

        aih_order.berth_event_info_.frame_info_[0].tm_info_ = source.veh_detail_info.time_info;

        // U32 car_gesture_ = 0;      //0无效，1车头，2车尾
        aih_order.berth_event_info_.order_score_ = (int)(source.veh_detail_info.veh_det_confidence * 100);
        // U32 covered_event = 0;      //遮挡事件，0无遮挡，1遮挡
        // U64 enter_start_timestamp_ = 0;      //进入泊位开始时间戳
        // U32 force_leave_flag_ = 0;      //强制离开标志
        // U32 group_id_ = -1;      //泊位组ID
        // U32 non_motor_car_type_ = 0;      //非机动车类型
        // U32 non_motor_id_ = -1;      //非机动车id
        // U32 out_video_start_ = 0;      //开始录制视频的标志，0:不处理，1:出场, 2 : 入场
        // U32 recent_plate_ = 0;      //车牌是否移动过
        // U32 plate_covered_ = 0;      //车牌是否被遮挡
        // U32 car_stable_count_ = 0;      //车辆停稳的帧计数


        // aih::PlateInfoExt plateinfos;
        // plateinfos.plate_score = source.veh_detail_info.lp_info.vlp_det_confidence;
        // plateinfos.plate_loc.x_ = source.veh_detail_info.lp_info.location.x_;
        // plateinfos.plate_loc.y_ = source.veh_detail_info.lp_info.location.x_;
        // plateinfos.plate_loc.y_ = source.veh_detail_info.lp_info.location.y_;
        // plateinfos.plate_loc.width_ = source.veh_detail_info.lp_info.location.width_;
        // plateinfos.plate_loc.height_ = source.veh_detail_info.lp_info.location.height_;
        // plateinfos.info.plate_score_ = source.veh_detail_info.lp_info.lp_number.vlp_rec_confidence;
        // plateinfos.info.plate_type_prob_ = source.veh_detail_info.lp_info.vlp_det_confidence;
        // if (!source.veh_detail_info.lp_info.lp_number.lp_str.empty() and source.veh_detail_info.lp_info.lp_number.lp_str
        //     != "-")
        // {
        //     plateinfos.info.plate_str_ = source.veh_detail_info.lp_info.lp_number.lp_str;
        // }
        // else if (!source.vlp.empty() and source.vlp != "-")
        // {
        //     plateinfos.info.plate_str_ = source.vlp;
        // }
        // else
        // {
        //     continue;
        // }
        // plateinfos.info.plate_type_id_ = PLATE_TYPE_CONVERT(source.veh_detail_info.lp_info.lp_type);
        // plateinfos.info.plate_color_id_ = PLATE_COLOR_CONVERT(source.veh_detail_info.lp_info.lp_color);

        // for (auto ch : source.veh_detail_info.lp_info.lp_number.lp_str)
        // {
        //     plateinfos.info.plate_chars_.emplace_back(1, ch);
        // }
        // plateinfos.info.plate_scores_.emplace_back(source.veh_detail_info.lp_info.lp_number.first_char_confidence);
        // plateinfos.info.plate_scores_.emplace_back(source.veh_detail_info.lp_info.lp_number.second_char_confidence);

        // aih_order.berth_event_info_.car_property_.plate_info_ = plateinfos.info;
        // aih_order.berth_event_info_.car_property_.plate_infos_.emplace_back(plateinfos);
        // aih_order.berth_event_info_.can_drop_[0] = false;
        // aih::BerthCarInfo &car_info = aih_order.berth_event_info_.car_info_[0];
        // car_info.plate_info_ = plateinfos.info;
        // car_info.car_loc_.x_ = source.veh_detail_info.location.x_;
        // car_info.car_loc_.y_ = source.veh_detail_info.location.y_;
        // car_info.car_loc_.width_ = source.veh_detail_info.location.width_;
        // car_info.car_loc_.height_ = source.veh_detail_info.location.height_;
        // car_info.confidence_ = std::max(0.0f, (source.veh_detail_info.veh_det_confidence * 100));
        // car_info.has_car_body_ = false;
        // car_info.plate_loc_.x_ = source.veh_detail_info.lp_info.location.x_;
        // car_info.plate_loc_.y_ = source.veh_detail_info.lp_info.location.y_;
        // car_info.plate_loc_.width_ = source.veh_detail_info.lp_info.location.width_;
        // car_info.plate_loc_.height_ = source.veh_detail_info.lp_info.location.height_;
        // car_info.covered_score_ = source.veh_detail_info.cover_rat;
        output_messages.emplace_back(aih_order);
    }
}

IllegalParkingMessageAIHTransfer::IllegalParkingMessageAIHTransfer(
    std::shared_ptr<CachedFramesManager> cached_frames_manager)
    : cached_frames_manager_(cached_frames_manager)
{
}

/**
 * @brief 填充违停告警信息
 * @param original_illegal_data 车辆违停算法输出的违停数据
 * @param aih_alarm_info 违停告警信息
 * @param aih_event_type 违停事件类型
 */
static void
SLStuffedAlarmInfo(const std::shared_ptr<illegal_parking::VehicleIllegalData> &original_illegal_data,
                   aih::AlarmInfo &aih_alarm_info, aih::EventType &aih_event_type)
{
    switch (original_illegal_data->illegal_case)
    {
        case illegal_parking::EIllegalCase::_PARALLEL_PARKING:
        case illegal_parking::EIllegalCase::_BAN_EXTENT_PARKING:
        case illegal_parking::EIllegalCase::_END_POINTS_PARKING:
            aih_alarm_info.alarm_type_ = aih::ParkErrorType_ProhibitError;
            aih_alarm_info.alarm_status_ = aih::ParkErrorType_ProhibitError;
            aih_event_type = aih::EVENT_TYPE_ENTER_VIOLATION;
            break;

        case illegal_parking::EIllegalCase::_PARKING_ONTO_LINE:
            aih_alarm_info.alarm_type_ = aih::ParkErrorType_CrossLineError;
            aih_alarm_info.alarm_status_ = aih::ParkErrorType_CrossLineError;
            aih_event_type = aih::EVENT_TYPE_ALARM_BERTH;
            break;
        case illegal_parking::EIllegalCase::_ANGLE_PARKING:
            aih_alarm_info.alarm_type_ = aih::ParkErrorType_DiagonalParkError;
            aih_alarm_info.alarm_status_ = aih::ParkErrorType_DiagonalParkError;
            aih_event_type = aih::EVENT_TYPE_ALARM_BERTH;
            break;
        case illegal_parking::EIllegalCase::_VERTICAL_PARKING:
            aih_alarm_info.alarm_type_ = aih::ParkErrorType_VerticalParkError;
            aih_alarm_info.alarm_status_ = aih::ParkErrorType_VerticalParkError;
            aih_event_type = aih::EVENT_TYPE_ALARM_BERTH;
            break;
        case illegal_parking::EIllegalCase::_OPPOSITE_PARKING:
            aih_alarm_info.alarm_type_ = aih::ParkErrorType_ReserveError;
            aih_alarm_info.alarm_status_ = aih::ParkErrorType_ReserveError;
            aih_event_type = aih::EVENT_TYPE_ALARM_BERTH;
            break;
        case illegal_parking::EIllegalCase::_CROSS_PARKING:
            aih_alarm_info.alarm_type_ = aih::ParkErrorType_CrossSlotError;
            aih_alarm_info.alarm_status_ = aih::ParkErrorType_CrossSlotError;
            aih_event_type = aih::EVENT_TYPE_ALARM_BERTH;
            break;
        case illegal_parking::EIllegalCase::_INTO_ILLEGAL_TIME:
            aih_alarm_info.alarm_type_ = aih::ParkErrorType_BanTimeError;
            aih_alarm_info.alarm_status_ = aih::ParkErrorType_BanTimeError;
            aih_event_type = aih::EVENT_TYPE_ALARM_BERTH;
            break;
        case illegal_parking::EIllegalCase::_UNKNOWN:
            aih_alarm_info.alarm_type_ = aih::ParkErrorType_Non;
            aih_alarm_info.alarm_status_ = aih::ParkErrorType_Non;
            aih_event_type = aih::EVENT_TYPE_NO_EVENT;
            break;
    }
}


void
IllegalParkingMessageAIHTransfer::ConvertIllegalParkingMessage(
    const std::shared_ptr<illegal_parking::IllegalParkingDetectionResult> &illegalParkingDetectionResult,
    aih::TimeInfo current_time_info,
    aih::EventInfos &aih_event_infos)
{
    for (const auto &vehicle_illegal_data : illegalParkingDetectionResult->vehicle_illegal_data)
    {
        aih::EventInfo aih_berth_alarm_event_info;
        aih_berth_alarm_event_info.control_id_ = vehicle_illegal_data->ctrl_id;
        aih_berth_alarm_event_info.event_id_ = vehicle_illegal_data->event_id;
        aih_berth_alarm_event_info.time_info_ = current_time_info;

        auto &berth_info_ref = aih_berth_alarm_event_info.berth_event_info_;
        berth_info_ref.result_cnt_ = 3;
        SLStuffedAlarmInfo(vehicle_illegal_data, berth_info_ref.alarm_info_, aih_berth_alarm_event_info.event_type_);


        CarPropertyConvert(aih_berth_alarm_event_info.berth_event_info_.car_property_, vehicle_illegal_data->vehicle_plate_source->veh_info);
        aih_berth_alarm_event_info.berth_event_info_.sharpness_[0] = vehicle_illegal_data->I_cap_stamp->veh_info.lp_info.vlp_det_confidence;
        aih_berth_alarm_event_info.berth_event_info_.sharpness_[1] = vehicle_illegal_data->II_cap_stamp->veh_info.lp_info.vlp_det_confidence;
        aih_berth_alarm_event_info.berth_event_info_.sharpness_[2] = vehicle_illegal_data->III_cap_stamp->veh_info.lp_info.vlp_det_confidence;
        int plate_mean =vehicle_illegal_data->I_cap_stamp->veh_info.lp_info.location.width_;
        if (vehicle_illegal_data->II_cap_stamp->veh_info.lp_info.location.width_ > plate_mean)
        {
            plate_mean = vehicle_illegal_data->II_cap_stamp->veh_info.lp_info.location.width_;
        }
        if (vehicle_illegal_data->III_cap_stamp->veh_info.lp_info.location.width_ > plate_mean)
        {
            plate_mean = vehicle_illegal_data->III_cap_stamp->veh_info.lp_info.location.width_;
        }
        if (plate_mean < 10)
        {
            plate_mean = 0;
        }

        aih_berth_alarm_event_info.berth_event_info_.plate_mean_[0] = plate_mean;
        aih_berth_alarm_event_info.berth_event_info_.plate_mean_[1] = plate_mean;
        aih_berth_alarm_event_info.berth_event_info_.plate_mean_[2] = plate_mean;

        aih_berth_alarm_event_info.berth_event_info_.can_drop_[0] = 0;
        aih_berth_alarm_event_info.berth_event_info_.can_drop_[1] = 0;
        aih_berth_alarm_event_info.berth_event_info_.can_drop_[2] = 0;

        // aih_berth_alarm_event_info.berth_event_info_.parking_state_[0] = aih::LoopStatus::LOOP_STATUS_ERROR;
        // aih_berth_alarm_event_info.berth_event_info_.parking_state_[1] = aih::LoopStatus::LOOP_STATUS_ERROR;
        // aih_berth_alarm_event_info.berth_event_info_.parking_state_[2] = aih::LoopStatus::LOOP_STATUS_ERROR;

        aih_berth_alarm_event_info.berth_event_info_.parking_state_[0] = aih::LoopStatus::LOOP_STATUS_PREIN;
        aih_berth_alarm_event_info.berth_event_info_.parking_state_[1] = aih::LoopStatus::LOOP_STATUS_IN;
        aih_berth_alarm_event_info.berth_event_info_.parking_state_[2] = aih::LoopStatus::LOOP_STATUS_STABLE;

        ConvertCarInfo(aih_berth_alarm_event_info.berth_event_info_.car_info_[0], vehicle_illegal_data->I_cap_stamp->veh_info);
        if(aih_berth_alarm_event_info.berth_event_info_.car_info_[0].plate_loc_.width_ < 10){
            aih_berth_alarm_event_info.berth_event_info_.car_info_[0].plate_loc_.width_ = plate_mean;
        }
        aih_berth_alarm_event_info.berth_event_info_.car_info_[0].plate_info_.plate_str_ = 
            aih_berth_alarm_event_info.berth_event_info_.car_property_.plate_info_.plate_str_;
        aih_berth_alarm_event_info.berth_event_info_.car_info_[0].plate_info_.plate_type_id_ = 
            aih_berth_alarm_event_info.berth_event_info_.car_property_.plate_info_.plate_type_id_;
        aih_berth_alarm_event_info.berth_event_info_.car_info_[0].plate_info_.plate_color_id_ = 
            aih_berth_alarm_event_info.berth_event_info_.car_property_.plate_info_.plate_color_id_;

        ConvertCarInfo(aih_berth_alarm_event_info.berth_event_info_.car_info_[1], vehicle_illegal_data->II_cap_stamp->veh_info);
        aih_berth_alarm_event_info.berth_event_info_.car_info_[1].plate_info_.plate_str_ = 
            aih_berth_alarm_event_info.berth_event_info_.car_property_.plate_info_.plate_str_;
        aih_berth_alarm_event_info.berth_event_info_.car_info_[1].plate_info_.plate_type_id_ = 
            aih_berth_alarm_event_info.berth_event_info_.car_property_.plate_info_.plate_type_id_;
        aih_berth_alarm_event_info.berth_event_info_.car_info_[1].plate_info_.plate_color_id_ = 
            aih_berth_alarm_event_info.berth_event_info_.car_property_.plate_info_.plate_color_id_;

        ConvertCarInfo(aih_berth_alarm_event_info.berth_event_info_.car_info_[2], vehicle_illegal_data->III_cap_stamp->veh_info);
        aih_berth_alarm_event_info.berth_event_info_.car_info_[2].plate_info_.plate_str_ = 
            aih_berth_alarm_event_info.berth_event_info_.car_property_.plate_info_.plate_str_;
        aih_berth_alarm_event_info.berth_event_info_.car_info_[2].plate_info_.plate_type_id_ = 
            aih_berth_alarm_event_info.berth_event_info_.car_property_.plate_info_.plate_type_id_;
        aih_berth_alarm_event_info.berth_event_info_.car_info_[2].plate_info_.plate_color_id_ = 
            aih_berth_alarm_event_info.berth_event_info_.car_property_.plate_info_.plate_color_id_;

        aih_berth_alarm_event_info.berth_event_info_.frame_info_[0].tm_info_ = cached_frames_manager_->FindTimeInfo(
            vehicle_illegal_data->I_cap_stamp->cap_stamp.pts_);
        aih_berth_alarm_event_info.berth_event_info_.frame_info_[1].tm_info_ = cached_frames_manager_->FindTimeInfo(
            vehicle_illegal_data->II_cap_stamp->cap_stamp.pts_);
        aih_berth_alarm_event_info.berth_event_info_.frame_info_[2].tm_info_ = cached_frames_manager_->FindTimeInfo(
            vehicle_illegal_data->III_cap_stamp->cap_stamp.pts_);

        if(aih_berth_alarm_event_info.berth_event_info_.frame_info_[0].tm_info_.pts_ == 0){
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->warn(
            "vehicle_illegal_data aih_berth_alarm_event_info.berth_event_info_.frame_info_[0] failed!  pts_time {}", 
            aih_berth_alarm_event_info.berth_event_info_.frame_info_[0].tm_info_.pts_);
            continue;
        }
        // else{
        //     SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
        //     "vehicle_illegal_data aih_berth_alarm_event_info.berth_event_info_.frame_info_[0]  pts_time {}", 
        //     aih_berth_alarm_event_info.berth_event_info_.frame_info_[0].tm_info_.pts_);
        // }
        if(aih_berth_alarm_event_info.berth_event_info_.frame_info_[1].tm_info_.pts_ == 0){
             SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->warn(
            "aih_berth_alarm_event_info.berth_event_info_.frame_info_[1] failed!  pts_time {}", 
            aih_berth_alarm_event_info.berth_event_info_.frame_info_[1].tm_info_.pts_);
            continue;
        }
        if(aih_berth_alarm_event_info.berth_event_info_.frame_info_[2].tm_info_.pts_ == 0){
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->warn(
            "aih_berth_alarm_event_info.berth_event_info_.frame_info_[2] failed!  pts_time {}", 
            aih_berth_alarm_event_info.berth_event_info_.frame_info_[2].tm_info_.pts_);
            continue;
        }

        // U32 car_gesture_ = 0;      //0无效，1车头，2车尾
        aih_berth_alarm_event_info.berth_event_info_.order_score_ = 
            (int)(vehicle_illegal_data->vehicle_plate_source->veh_info.veh_det_confidence * 100);
        // U32 covered_event = 0;      //遮挡事件，0无遮挡，1遮挡
        // U64 enter_start_timestamp_ = 0;      //进入泊位开始时间戳
        // U32 force_leave_flag_ = 0;      //强制离开标志
        // U32 group_id_ = -1;      //泊位组ID
        // U32 non_motor_car_type_ = 0;      //非机动车类型
        // U32 non_motor_id_ = -1;      //非机动车id
        // U32 out_video_start_ = 0;      //开始录制视频的标志，0:不处理，1:出场, 2 : 入场
        // U32 recent_plate_ = 0;      //车牌是否移动过
        // U32 plate_covered_ = 0;      //车牌是否被遮挡
        // U32 car_stable_count_ = 0;      //车辆停稳的帧计数

        if (vehicle_illegal_data->vehicle_plate_source->veh_info.lp_info.lp_number.lp_str.empty() ||
            vehicle_illegal_data->vehicle_plate_source->veh_info.lp_info.lp_number.lp_str == "-")
        {
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->warn(
            "aih_berth_alarm_event_info vehicle_illegal_data->vehicle_plate_source->veh_info.lp_info.lp_number.lp_str {}, {}, {}, {}", 
            vehicle_illegal_data->vehicle_plate_source->veh_info.lp_info.lp_number.lp_str,
            vehicle_illegal_data->I_cap_stamp->veh_info.lp_info.lp_number.lp_str,
            vehicle_illegal_data->II_cap_stamp->veh_info.lp_info.lp_number.lp_str,
            vehicle_illegal_data->III_cap_stamp->veh_info.lp_info.lp_number.lp_str);
            continue;
        }

        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
            "aih_berth_alarm_event_info vehicle_illegal_data lp_str {}, {}, {}, {}", 
            vehicle_illegal_data->vehicle_plate_source->veh_info.lp_info.lp_number.lp_str,
            vehicle_illegal_data->I_cap_stamp->veh_info.lp_info.lp_number.lp_str,
            vehicle_illegal_data->II_cap_stamp->veh_info.lp_info.lp_number.lp_str,
            vehicle_illegal_data->III_cap_stamp->veh_info.lp_info.lp_number.lp_str);

        // aih_berth_alarm_event_info.berth_event_info_.car_property_.car_plate_score_ =
        //     vehicle_illegal_data->vehicle_plate_source->veh_info.lp_info.lp_number.vlp_rec_confidence;
        // aih_berth_alarm_event_info.berth_event_info_.car_property_.car_body_score_ = vehicle_illegal_data->
        //     vehicle_plate_source->veh_info.veh_det_confidence;

        // aih_berth_alarm_event_info.berth_event_info_.car_property_.car_type_.type_id_ =
        //     CAR_TYPE_CONVERT(vehicle_illegal_data->vehicle_plate_source->veh_info.veh_type);
        // aih_berth_alarm_event_info.berth_event_info_.car_property_.car_type_.type_str_ =
        //     aih::type_pair[aih_berth_alarm_event_info.berth_event_info_.car_property_.car_type_.type_id_];
        // aih_berth_alarm_event_info.berth_event_info_.car_property_.car_type_.type_score_ = 1.0f;

        // aih_berth_alarm_event_info.berth_event_info_.car_property_.car_color_.type_id_ =
        //     CAR_COLOR_CONVERT(vehicle_illegal_data->vehicle_plate_source->veh_info.veh_color);
        // aih_berth_alarm_event_info.berth_event_info_.car_property_.car_color_.type_str_ =
        //     aih::color_pair[aih_berth_alarm_event_info.berth_event_info_.car_property_.car_color_.type_id_];
        // aih_berth_alarm_event_info.berth_event_info_.car_property_.car_color_.type_score_ = 1.0f;

        // aih_berth_alarm_event_info.berth_event_info_.sharpness_[0] =
        //     vehicle_illegal_data->I_cap_stamp->veh_info.lp_info.lp_number.vlp_rec_confidence;
        // aih_berth_alarm_event_info.berth_event_info_.sharpness_[1] =
        //     vehicle_illegal_data->II_cap_stamp->veh_info.lp_info.lp_number.vlp_rec_confidence;
        // aih_berth_alarm_event_info.berth_event_info_.sharpness_[2] =
        //     vehicle_illegal_data->III_cap_stamp->veh_info.lp_info.lp_number.vlp_rec_confidence;

        // aih_berth_alarm_event_info.berth_event_info_.can_drop_[0] = 0;
        // aih_berth_alarm_event_info.berth_event_info_.can_drop_[1] = 0;
        // aih_berth_alarm_event_info.berth_event_info_.can_drop_[2] = 0;

        // SLStuffedBerthCarInfo(vehicle_illegal_data->I_cap_stamp->veh_info,
        //                       aih_berth_alarm_event_info.berth_event_info_.car_info_[0]);
        // SLStuffedBerthCarInfo(vehicle_illegal_data->II_cap_stamp->veh_info,
        //                       aih_berth_alarm_event_info.berth_event_info_.car_info_[1]);
        // SLStuffedBerthCarInfo(vehicle_illegal_data->III_cap_stamp->veh_info,
        //                       aih_berth_alarm_event_info.berth_event_info_.car_info_[2]);

        // aih_berth_alarm_event_info.berth_event_info_.frame_info_[0].tm_info_ =
        //     vehicle_illegal_data->I_cap_stamp->cap_stamp;
        // aih_berth_alarm_event_info.berth_event_info_.frame_info_[1].tm_info_ =
        //     vehicle_illegal_data->II_cap_stamp->cap_stamp;
        // aih_berth_alarm_event_info.berth_event_info_.frame_info_[2].tm_info_ =
        //     vehicle_illegal_data->III_cap_stamp->cap_stamp;

        // aih::TimeInfo time_tmp = cached_frames_manager_->FindTimeInfo(aih_berth_alarm_event_info.berth_event_info_.frame_info_[0].tm_info_.pts_);
        // if(time_tmp.pts_ == 0){
        //      SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->warn(
        //     "vehicle_illegal_data aih_berth_alarm_event_info.berth_event_info_.frame_info_[0] failed!  pts_time {}", 
        //     aih_berth_alarm_event_info.berth_event_info_.frame_info_[0].tm_info_.pts_);
        //     continue;
        // }
        // else{
        //     SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
        //     "vehicle_illegal_data aih_berth_alarm_event_info.berth_event_info_.frame_info_[0]  pts_time {}", 
        //     aih_berth_alarm_event_info.berth_event_info_.frame_info_[0].tm_info_.pts_);
        // }

        // time_tmp = cached_frames_manager_->FindTimeInfo(aih_berth_alarm_event_info.berth_event_info_.frame_info_[1].tm_info_.pts_);
        // if(time_tmp.pts_ == 0){
        //      SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->warn(
        //     "aih_berth_alarm_event_info.berth_event_info_.frame_info_[1] failed!  pts_time {}", 
        //     aih_berth_alarm_event_info.berth_event_info_.frame_info_[1].tm_info_.pts_);
        //     continue;
        // }
        // time_tmp = cached_frames_manager_->FindTimeInfo(aih_berth_alarm_event_info.berth_event_info_.frame_info_[2].tm_info_.pts_);
        // if(time_tmp.pts_ == 0){
        //      SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->warn(
        //     "aih_berth_alarm_event_info.berth_event_info_.frame_info_[2] failed!  pts_time {}", 
        //     aih_berth_alarm_event_info.berth_event_info_.frame_info_[2].tm_info_.pts_);
        //     continue;
        // }

        // aih::PlateInfoExt plateinfos;
        // plateinfos.plate_score = vehicle_illegal_data->vehicle_plate_source->veh_info.lp_info.vlp_det_confidence;
        // plateinfos.plate_loc.x_ = vehicle_illegal_data->vehicle_plate_source->veh_info.lp_info.location.x_;
        // plateinfos.plate_loc.y_ = vehicle_illegal_data->vehicle_plate_source->veh_info.lp_info.location.x_;
        // plateinfos.plate_loc.y_ = vehicle_illegal_data->vehicle_plate_source->veh_info.lp_info.location.y_;
        // plateinfos.plate_loc.width_ = vehicle_illegal_data->vehicle_plate_source->veh_info.lp_info.location.width_;
        // plateinfos.plate_loc.height_ = vehicle_illegal_data->vehicle_plate_source->veh_info.lp_info.location.height_;
        // plateinfos.info.plate_score_ = vehicle_illegal_data->vehicle_plate_source->veh_info.lp_info.lp_number.
        //                                                      vlp_rec_confidence;
        // plateinfos.info.plate_type_prob_ = vehicle_illegal_data->vehicle_plate_source->veh_info.lp_info.
        //                                                          vlp_det_confidence;
        // plateinfos.info.plate_str_ = vehicle_illegal_data->plate_number;

        // plateinfos.info.plate_type_id_ = PLATE_TYPE_CONVERT(
        //     vehicle_illegal_data->vehicle_plate_source->veh_info.lp_info.lp_type);
        // plateinfos.info.plate_color_id_ = PLATE_COLOR_CONVERT(
        //     vehicle_illegal_data->plate_color);

        // for (auto ch : vehicle_illegal_data->vehicle_plate_source->veh_info.lp_info.lp_number.lp_str)
        // {
        //     plateinfos.info.plate_chars_.emplace_back(1, ch);
        // }
        // plateinfos.info.plate_scores_.emplace_back(
        //     vehicle_illegal_data->vehicle_plate_source->veh_info.lp_info.lp_number.first_char_confidence);
        // plateinfos.info.plate_scores_.emplace_back(
        //     vehicle_illegal_data->vehicle_plate_source->veh_info.lp_info.lp_number.second_char_confidence);

        // aih_berth_alarm_event_info.berth_event_info_.car_property_.plate_info_ = plateinfos.info;
        // aih_berth_alarm_event_info.berth_event_info_.car_property_.plate_infos_.emplace_back(plateinfos);
        // aih_berth_alarm_event_info.berth_event_info_.can_drop_[0] = false;

        // SLRectConvertAIHALGObject(vehicle_illegal_data->I_cap_stamp->veh_info.lp_info.location, plateinfos.plate_loc);
        // aih_berth_alarm_event_info.berth_event_info_.car_info_[0].plate_info_ = plateinfos.info;
        // SLRectConvertAIHALGObject(vehicle_illegal_data->II_cap_stamp->veh_info.lp_info.location, plateinfos.plate_loc);
        // aih_berth_alarm_event_info.berth_event_info_.car_info_[1].plate_info_ = plateinfos.info;
        // SLRectConvertAIHALGObject(vehicle_illegal_data->III_cap_stamp->veh_info.lp_info.location, plateinfos.plate_loc);
        // aih_berth_alarm_event_info.berth_event_info_.car_info_[2].plate_info_ = plateinfos.info;

        // aih::BerthCarInfo &car_info = aih_berth_alarm_event_info.berth_event_info_.car_info_[0];
        // car_info.plate_info_ = plateinfos.info;
        // car_info.car_loc_.x_ = vehicle_illegal_data->vehicle_plate_source->veh_info.location.x_;
        // car_info.car_loc_.y_ = vehicle_illegal_data->vehicle_plate_source->veh_info.location.y_;
        // car_info.car_loc_.width_ = vehicle_illegal_data->vehicle_plate_source->veh_info.location.width_;
        // car_info.car_loc_.height_ = vehicle_illegal_data->vehicle_plate_source->veh_info.location.height_;
        // car_info.confidence_ = std::max(
        //     0.0f, (vehicle_illegal_data->vehicle_plate_source->veh_info.veh_det_confidence * 100));
        // car_info.has_car_body_ = false;
        // car_info.plate_loc_.x_ = vehicle_illegal_data->vehicle_plate_source->veh_info.lp_info.location.x_;
        // car_info.plate_loc_.y_ = vehicle_illegal_data->vehicle_plate_source->veh_info.lp_info.location.y_;
        // car_info.plate_loc_.width_ = vehicle_illegal_data->vehicle_plate_source->veh_info.lp_info.location.width_;
        // car_info.plate_loc_.height_ = vehicle_illegal_data->vehicle_plate_source->veh_info.lp_info.location.height_;
        // car_info.covered_score_ = vehicle_illegal_data->vehicle_plate_source->veh_info.cover_rat;
        aih_event_infos.emplace_back(aih_berth_alarm_event_info);

    }
    for (const auto &logic_keep_frame_event : illegalParkingDetectionResult->logic_keep_frame_events)
    {
        if (!logic_keep_frame_event->vehicle_plate_source)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->error("[{}]logic_keep_frame_events:vehicle_plate_source is null:[{}]", __func__,
                        TO_STRING(*logic_keep_frame_event));
            continue;
        }
        aih::EventInfo aih_berth_alarm_event_info;
        aih_berth_alarm_event_info.control_id_ = logic_keep_frame_event->ctrl_id;
        aih_berth_alarm_event_info.event_id_ = logic_keep_frame_event->event_id;
        aih_berth_alarm_event_info.time_info_ = current_time_info;

        auto &berth_info_ref = aih_berth_alarm_event_info.berth_event_info_;
        berth_info_ref.result_cnt_ = 1;
        SLStuffedAlarmInfo(logic_keep_frame_event, berth_info_ref.alarm_info_, aih_berth_alarm_event_info.event_type_);
        aih_berth_alarm_event_info.event_type_ = aih::EventType::EVENT_TYPE_BERTH_REALTIME;

        aih_berth_alarm_event_info.berth_event_info_.car_property_.car_plate_score_ =
            (int)(logic_keep_frame_event->vehicle_plate_source->veh_info.lp_info.lp_number.vlp_rec_confidence * 100);
        aih_berth_alarm_event_info.berth_event_info_.car_property_.car_body_score_ = 
            (int)(logic_keep_frame_event->vehicle_plate_source->veh_info.veh_det_confidence * 100);

        aih_berth_alarm_event_info.berth_event_info_.car_property_.car_type_.type_id_ =
            CAR_TYPE_CONVERT(logic_keep_frame_event->vehicle_plate_source->veh_info.veh_type);
        aih_berth_alarm_event_info.berth_event_info_.car_property_.car_type_.type_str_ =
            aih::type_pair[aih_berth_alarm_event_info.berth_event_info_.car_property_.car_type_.type_id_];
        aih_berth_alarm_event_info.berth_event_info_.car_property_.car_type_.type_score_ = 100.0f;

        aih_berth_alarm_event_info.berth_event_info_.car_property_.car_color_.type_id_ =
            CAR_COLOR_CONVERT(logic_keep_frame_event->vehicle_plate_source->veh_info.veh_color);
        aih_berth_alarm_event_info.berth_event_info_.car_property_.car_color_.type_str_ =
            aih::color_pair[aih_berth_alarm_event_info.berth_event_info_.car_property_.car_color_.type_id_];
        aih_berth_alarm_event_info.berth_event_info_.car_property_.car_color_.type_score_ = 100.0f;

        aih_berth_alarm_event_info.berth_event_info_.sharpness_[0] =
            logic_keep_frame_event->vehicle_plate_source->veh_info.lp_info.lp_number.vlp_rec_confidence;
        aih_berth_alarm_event_info.berth_event_info_.can_drop_[0] = 0;
        SLStuffedBerthCarInfo(logic_keep_frame_event->vehicle_plate_source->veh_info,
                              aih_berth_alarm_event_info.berth_event_info_.car_info_[0]);

        aih_berth_alarm_event_info.berth_event_info_.frame_info_[0].tm_info_ =
            logic_keep_frame_event->vehicle_plate_source->cap_stamp;
        
        // aih::TimeInfo time_tmp = cached_frames_manager_->FindTimeInfo(aih_berth_alarm_event_info.berth_event_info_.frame_info_[0].tm_info_.pts_);
        // if(time_tmp.pts_ == 0){
        //      SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->warn(
        //     "--logic_keep_frame_events--aih_berth_alarm_event_info.berth_event_info_.frame_info_[0] failed!  pts_time {}", 
        //     aih_berth_alarm_event_info.berth_event_info_.frame_info_[0].tm_info_.pts_);
        //     continue;
        // }
        // else{
        //     SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
        //     "--logic_keep_frame_events--aih_berth_alarm_event_info.berth_event_info_.frame_info_[0]!  pts_time {}", 
        //     aih_berth_alarm_event_info.berth_event_info_.frame_info_[0].tm_info_.pts_);
        // }

        aih::PlateInfoExt plateinfos;
        plateinfos.plate_score = logic_keep_frame_event->vehicle_plate_source->veh_info.lp_info.vlp_det_confidence;
        plateinfos.plate_loc.x_ = logic_keep_frame_event->vehicle_plate_source->veh_info.lp_info.location.x_;
        plateinfos.plate_loc.y_ = logic_keep_frame_event->vehicle_plate_source->veh_info.lp_info.location.x_;
        plateinfos.plate_loc.y_ = logic_keep_frame_event->vehicle_plate_source->veh_info.lp_info.location.y_;
        plateinfos.plate_loc.width_ = logic_keep_frame_event->vehicle_plate_source->veh_info.lp_info.location.width_;
        plateinfos.plate_loc.height_ = logic_keep_frame_event->vehicle_plate_source->veh_info.lp_info.location.height_;
        plateinfos.info.plate_score_ = logic_keep_frame_event->vehicle_plate_source->veh_info.lp_info.lp_number.
                                                               vlp_rec_confidence;
        plateinfos.info.plate_type_prob_ = logic_keep_frame_event->vehicle_plate_source->veh_info.lp_info.
                                                                   vlp_det_confidence;
        if (logic_keep_frame_event->plate_number.empty() or logic_keep_frame_event->plate_number == "-")
        {
            plateinfos.info.plate_str_ =
                logic_keep_frame_event->vehicle_plate_source->veh_info.lp_info.lp_number.lp_str;
        }
        else
        {
            plateinfos.info.plate_str_ = logic_keep_frame_event->plate_number;
        }
        plateinfos.info.plate_str_ =
            logic_keep_frame_event->vehicle_plate_source->veh_info.lp_info.lp_number.lp_str;
        plateinfos.info.plate_type_id_ = PLATE_TYPE_CONVERT(
            logic_keep_frame_event->vehicle_plate_source->veh_info.lp_info.lp_type);
        plateinfos.info.plate_color_id_ = PLATE_COLOR_CONVERT(
            logic_keep_frame_event->vehicle_plate_source->veh_info.lp_info.lp_color);

        for (auto ch : logic_keep_frame_event->vehicle_plate_source->veh_info.lp_info.lp_number.lp_str)
        {
            plateinfos.info.plate_chars_.emplace_back(1, ch);
        }
        plateinfos.info.plate_scores_.emplace_back(
            logic_keep_frame_event->vehicle_plate_source->veh_info.lp_info.lp_number.first_char_confidence);
        plateinfos.info.plate_scores_.emplace_back(
            logic_keep_frame_event->vehicle_plate_source->veh_info.lp_info.lp_number.second_char_confidence);

        aih_berth_alarm_event_info.berth_event_info_.car_info_[0].plate_info_ = plateinfos.info;

        aih_berth_alarm_event_info.berth_event_info_.car_property_.plate_info_ = plateinfos.info;
        aih_berth_alarm_event_info.berth_event_info_.car_property_.plate_infos_.emplace_back(plateinfos);
        aih_berth_alarm_event_info.berth_event_info_.can_drop_[0] = false;
        aih::BerthCarInfo &car_info = aih_berth_alarm_event_info.berth_event_info_.car_info_[0];
        car_info.plate_info_ = plateinfos.info;
        car_info.car_loc_.x_ = logic_keep_frame_event->vehicle_plate_source->veh_info.location.x_;
        car_info.car_loc_.y_ = logic_keep_frame_event->vehicle_plate_source->veh_info.location.y_;
        car_info.car_loc_.width_ = logic_keep_frame_event->vehicle_plate_source->veh_info.location.width_;
        car_info.car_loc_.height_ = logic_keep_frame_event->vehicle_plate_source->veh_info.location.height_;
        car_info.confidence_ = std::max(
            0.0f, (logic_keep_frame_event->vehicle_plate_source->veh_info.veh_det_confidence * 100));
        car_info.has_car_body_ = false;
        car_info.plate_loc_.x_ = logic_keep_frame_event->vehicle_plate_source->veh_info.lp_info.location.x_;
        car_info.plate_loc_.y_ = logic_keep_frame_event->vehicle_plate_source->veh_info.lp_info.location.y_;
        car_info.plate_loc_.width_ = logic_keep_frame_event->vehicle_plate_source->veh_info.lp_info.location.width_;
        car_info.plate_loc_.height_ = logic_keep_frame_event->vehicle_plate_source->veh_info.lp_info.location.height_;
        car_info.covered_score_ = logic_keep_frame_event->vehicle_plate_source->veh_info.cover_rat;
        aih_event_infos.emplace_back(aih_berth_alarm_event_info);
    }
}

}
#include "struct_2_stream.h"
#include "../illegal_parking/illegal_parking_algorithm.h"
#include "../interfaces/aih_extern.h"
#include "../interfaces/aih_type.h"
#include "data_type.h"

template <typename E>
std::ostream &
operator<<(std::ostream &out, const sonli::Point_<E> &p)
{
    out << "(" << p.x_ << "," << p.y_ << ")";
    return out;
}

template <typename E>
std::ostream &
operator<<(std::ostream &os, const sonli::Line_<E> &obj)
{
    return os << "start_point_: " << obj.start_point_ << " end_point_: " << obj.end_point_;
}

template <typename T>
std::ostream &
operator<<(std::ostream &Os, const std::vector<T> &Obj)
{
    Os << "(";
    for (const auto &item : Obj)
    {
        Os << item << ",";
    }

    Os << ")";
    return Os;
}

namespace aih
{
template <typename T>
std::ostream &
operator<<(std::ostream &Os, const _Point<T> &point)
{
    Os << "(" << point.x << "," << point.y << ")";
    return Os;
}
} // namespace aih

std::ostream &
operator<<(std::ostream &Os, const aih::SingleCrossLineEvent &Obj)
{
    return Os << "move_direction_: [" << Obj.move_direction_ << "]"
              << " crossline_area_percent_: [" << Obj.crossline_area_percent_ << "]";
}

std::ostream &
operator<<(std::ostream &Os, const aih::_area_event &Obj)
{
    return Os << "in_area_percent_: [" << Obj.in_area_percent_ << "]";
}

std::ostream &
operator<<(std::ostream &Os, const aih::ControlInfo &Obj)
{
    return Os << "id_: [" << Obj.id_ << "] control_type_: [" << Obj.control_type_ << "]"
              << " point_infos_: [" << Obj.point_infos_ << "]";
}

std::ostream &
operator<<(std::ostream &Os, const aih::DeployTime &Obj)
{
    return Os << "start_time: [" << Obj.start_time << "] end_time: [" << Obj.end_time << "]";
}

std::ostream &
operator<<(std::ostream &Os, const aih::berth_event_cfg &Obj)
{
    return Os << "report_mode: [" << Obj.report_mode << "]"
              << " default_province: [" << Obj.default_province << "]"
              << " recognition_type: [" << Obj.recognition_type << "]"
              << " enter_pic_num: [" << Obj.enter_pic_num << "]"
              << " leave_pic_num: [" << Obj.leave_pic_num << "]";
}

std::ostream &
operator<<(std::ostream &Os, const aih::SingleEventCfg &Obj)
{
    return Os << "control_id_: [" << Obj.control_id_ << "] event_id_: [" << Obj.event_id_ << "]"
              << " event_type_: [" << Obj.event_type_ << "] enable_event_: [" << Obj.enable_event_ << "] cfg_info_: ["
              << std::pair<int, aih::SingleEventCfgInfo>(static_cast<int>(Obj.event_type_), Obj.cfg_info_) << "]";
}

std::ostream &
operator<<(std::ostream &Os, const aih::LoopAlarmParam &Obj)
{
    return Os << "bReportNonRecoPlate: " << Obj.bReportNonRecoPlate << " bReportNoPlateError: ["
              << Obj.bReportNoPlateError << "]"
              << " bReportProbablyInOutError: [" << Obj.bReportProbablyInOutError << "]"
              << " bReportReverseParkingError: [" << Obj.bReportReverseParkingError << "]"
              << " reportReverseParkingType: [" << Obj.reportReverseParkingType << "]"
              << " bReportCrossLineError: [" << Obj.bReportCrossLineError << "]"
              << " reportCrossLineSensitivity: [" << Obj.reportCrossLineSensitivity << "]"
              << " bReportCrossBerthError: [" << Obj.bReportCrossBerthError << "]"
              << " reportCrossBerthSensitivity: [" << Obj.reportCrossBerthSensitivity << "]"
              << " reportWholeCrossedBerth: [" << Obj.reportWholeCrossedBerth << "]"
              << " bReportVerticalParkingError: [" << Obj.bReportVerticalParkingError << "]"
              << " bReportDiagonalParkingError: [" << Obj.bReportDiagonalParkingError << "]"
              << " bReportLongTermParkingError: [" << Obj.bReportLongTermParkingError << "]"
              << " reportLongTermParkingWaitTime: [" << Obj.reportLongTermParkingWaitTime << "]"
              << " bReportBanTimeError: [" << Obj.bReportBanTimeError << "]"
              << " reportBanTimeWaitTime: [" << Obj.reportBanTimeWaitTime << "]"
              << " banTimeDurings: [" << Obj.banTimeDurings << "]";
}

std::ostream &
operator<<(std::ostream &Os, const aih::ViolationAlarmParam &Obj)
{
    return Os << "reportProhibitWaitTime:[" << Obj.reportProhibitWaitTime << "] violationType: [" << Obj.violationType
              << "]";
}

std::ostream &
operator<<(std::ostream &Os, std::pair<int, const aih::SingleEventCfgInfo &> Obj)
{
    auto event_type = static_cast<aih::EventType>(Obj.first);
    switch (event_type)
    {
        case aih::EventType::EVENT_TYPE_CROSS_LINE:
            Os << "line_cfg_: " << Obj.second.line_cfg_;
            break;
        case aih::EventType::EVENT_TYPE_ENTER_BERTH:
        case aih::EventType::EVENT_TYPE_STAY_BERTH:
        case aih::EventType::EVENT_TYPE_LEAVE_BERTH:
            Os << "berth_cfg_: " << Obj.second.berth_cfg_;
            break;
        case aih::EVENT_TYPE_ALARM_BERTH:
            Os << "berth_alarm_cfg_: " << Obj.second.berth_alarm_cfg_;
            break;
        case aih::EventType::EVENT_TYPE_ENTER_VIOLATION:
            Os << "violation_cfg_: " << Obj.second.violation_cfg_;
            break;
        case aih::EventType::EVENT_TYPE_ALARM_VIOLATION:
            Os << "violation_alarm_cfg_: " << Obj.second.violation_alarm_cfg_;
            break;
        default:
            break;
    }
    return Os;
}

std::ostream &
operator<<(std::ostream &Os, const aih::DeployTimeCfg &Obj)
{
    Os << "time_counter: (";
    for (int i = 0; i < 7; i++)
    {
        Os << Obj.time_durings[i][0] << ", ";
    }
    Os << ") ";
    return Os;
}

std::ostream &
operator<<(std::ostream &os, const sonli::VehInfoVec &obj)
{
    os << " size: " << obj.size << " frame_timestamp: " << obj.frame_timestamp << " time_info: " << obj.time_info.pts_
       << " frame_id: " << obj.frame_id << " frame_height: " << obj.frame_height << " frame_width: " << obj.frame_width
       << " camera_channel: " << obj.camera_channel << " is_blind_camera: " << obj.is_blind_camera
       << " use_reid: " << obj.use_reid << " veh_info : ";
    for (const auto &vehs : obj.veh_info)
    {
        os << vehs << "||\n";
    }
    return os;
}

std::ostream &
operator<<(std::ostream &os, const sonli::VehInfo &obj)
{
    return os << " veh_type: [" << obj.veh_type << "] veh_color: [" << obj.veh_color << "] location: ("
              << obj.location.x_ << ", " << obj.location.y_ << ", " << obj.location.width_ << ", "
              << obj.location.height_ << ")"
              << " veh_type_conf: [" << obj.veh_type_conf << "] veh_color_conf: [" << obj.veh_color_conf << "]"
              << " veh_det_confidence: [" << obj.veh_det_confidence << "]"
              << " lp_info: [" << obj.lp_info.lp_number.lp_str << "]"
              << " lp_info_coord: (" << obj.lp_info.location.x_ << ", " << obj.lp_info.location.y_ << ", "
              << obj.lp_info.location.width_ << ", " << obj.lp_info.location.height_ << ")"
              << " lp_info_conf: [" << obj.lp_info.vlp_det_confidence << "]"
              << " lp_info_size: [" << obj.lp_info.lp_number.size << "]"
              << " lp_info_color: [" << obj.lp_info.lp_color << "]"
              << " lp_info_number_conf: [" << obj.lp_info.lp_number.vlp_rec_confidence << "]"
              << " veh_timestamp: [" << obj.veh_timestamp << "] frame_id: [" << obj.frame_id << "]"
              << " veh_track_id: [" << obj.veh_track_id << "] cover_rat: [" << obj.cover_rat << "]"
              << " is_image_edge: [" << obj.is_image_edge << "]" << " space info: [" << obj.veh_chassis << "]";
}

std::ostream &
operator<<(std::ostream &os, const sonli::ParkingInfo &obj)
{
    using namespace sonli;
    os << " control_id: [" << obj.control_id << "]";
    os << " event_id: [" << obj.event_id << "]";
    os << " parking coord: (";
    for (auto point : obj.points)
    {
        os << "(" << point.x_ << ", " << point.y_ << "), ";
    }
    os << ")";
    os << " berth_event_list: [" << obj.berth_event_list << "]";
    return os;
}

// std::ostream &
// operator<<(std::ostream &os, const sonli::AccessConfig_t &obj)
// {
//     return os << " enable: " << obj.enable << " line_num: " << obj.line_num << " l1_control_id: " <<
//     obj.l1_control_id
//               << " l1_event_id: " << obj.l1_event_id << " line1_name: " << obj.line1_name << " line1: " << obj.line1
//               << " line1_event: " << obj.line1_event << " l2_control_id: " << obj.l2_control_id
//               << " l2_event_id: " << obj.l2_event_id << " line2_name: " << obj.line2_name << " line2: " << obj.line2
//               << " line2_event: " << obj.line2_event;
// }

std::ostream &
operator<<(std::ostream &os, const sonli::illegal_parking::NoParkingAreaConfig &obj)
{
    return os << "control_id: [" << obj.control_id << "] event_id: [" << "]"
              << " violation_cfg_: [" << obj.violation_cfg_ << "]"
              << " violation_alarm_cfg_: [" << obj.violation_alarm_cfg_ << "]"
              << " zone: [" << obj.zone << "]";
}

// std::ostream &
// operator<<(std::ostream &os, const aih::TimeInfo &time_info)
// {
//     os << "TimeInfo{ pts: " << time_info.pts_ << ", local_time: " << time_info.local_time_ << " }";
//     return os;
// }
//
// std::ostream &
// operator<<(std::ostream &os, const aih::AlarmInfo &info)
// {
//     os << "AlarmInfo{"
//        << "alarm_type_: " << info.alarm_type_ << ", alarm_status_: " << info.alarm_status_ << "}";
//     return os;
// }
//
// std::ostream &
// operator<<(std::ostream &os, const aih::PlateInfo &info)
// {
//     os << "PlateInfo{" << std::endl;
//     os << "  plate_score_: " << info.plate_score_ << "," << std::endl;
//     os << "  plate_type_prob_: " << info.plate_type_prob_ << "," << std::endl;
//     os << "  plate_str_: \"" << info.plate_str_ << "\"," << std::endl;
//     os << "  plate_type_id_: " << info.plate_type_id_ << "," << std::endl;
//     os << "  plate_color_id_: " << info.plate_color_id_ << "," << std::endl;
//     os << "  plate_layer_id_: " << info.plate_layer_id_ << "," << std::endl;
//     os << "  plate_type_str: \"" << info.plate_type_str << "\"," << std::endl;
//     os << "  plate_color_str_: \"" << info.plate_color_str_ << "\"," << std::endl;
//     os << "  plate_layer_str_: \"" << info.plate_layer_str_ << "\"," << std::endl;
//
//     // 输出字符置信度向量
//     os << "  plate_scores_: [";
//     for (size_t i = 0; i < info.plate_scores_.size(); ++i)
//     {
//         os << info.plate_scores_[i];
//         if (i != info.plate_scores_.size() - 1)
//             os << ", ";
//     }
//     os << "]," << std::endl;
//
//     // 输出字符向量
//     os << "  plate_chars_: [";
//     for (size_t i = 0; i < info.plate_chars_.size(); ++i)
//     {
//         os << "\"" << info.plate_chars_[i] << "\"";
//         if (i != info.plate_chars_.size() - 1)
//             os << ", ";
//     }
//     os << "]" << std::endl;
//     os << "}";
//     return os;
// }

std::ostream &
operator<<(std::ostream &os, const sonli::AccessConfig_t &config)
{
    os << "AccessConfig_t{";
    os << "name_: " << config.name_ << ",";
    os << "enable: " << config.enable << ",";
    os << "line_num: " << config.line_num << ",";
    os << "line1: {";
    os << "control_id: " << config.l1_control_id << ",";
    os << "event_id: " << config.l1_event_id << ",";
    os << "line: " << config.line1 << ",";
    os << "event: " << config.line1_event << "";
    os << "},";
    os << "line2: [";
    os << "control_id: " << config.l2_control_id << ",";
    os << "event_id: " << config.l2_event_id << ",";
    os << "line: " << config.line2 << ",";
    os << "event: " << config.line2_event << "";
    os << "}";
    os << "}";
    return os;
}

std::ostream &
operator<<(std::ostream &os, const sonli::ParkingInfoVec &obj)
{
    using namespace sonli;
    os << " space_angle: " << obj.space_angle << " size: " << obj.size << " park_loc: " << obj.park_loc
       << " time_info_: " << obj.time_info_;
    for (int i = 0; i < obj.vector_.size(); i++)
    {
        os << " vector_[" << i << "]: " << obj.vector_[i];
    }
    for (int i = 0; i < obj.parking_info.size(); i++)
    {
        os << " parking_info[" << i << "]: ";
        ::operator<<(os, obj.parking_info[i]);
        std::cout << std::endl;
    }
    return os;
}

std::ostream &
operator<<(std::ostream &Os, const sonli::Veh_Key_Point &Obj)
{
    return Os << "x: " << Obj.x << " y: " << Obj.y << " vis_score: " << Obj.vis_score;
}

std::ostream &
operator<<(std::ostream &os, const sonli::VehChassisInfo &obj)
{
    return os << " space_res: " << obj.space_res << " space_rat: " << obj.space_rat << " space_id: " << obj.space_id
              << " space_rat_vec: " << obj.space_rat_vec << " car_rat_vec: " << obj.car_rat_vec
              << " line_angle: " << obj.line_angle << " gradient: " << obj.gradient
              << " space_res_ex: " << obj.space_res_ex << " space_rat_ex: " << obj.space_rat_ex
              << " space_id_ex: " << obj.space_id_ex << " space_rat_vec_ex: " << obj.space_rat_vec_ex;
}
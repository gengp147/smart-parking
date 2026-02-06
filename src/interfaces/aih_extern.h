#ifndef ACL_DET_YOLO_AIH_EXTERN_H
#define ACL_DET_YOLO_AIH_EXTERN_H
#include "aih_prop.h"
#include "aih_type.h"
#include <iostream>

namespace sonli
{

inline std::ostream &
operator<<(std::ostream &os, const aih::CAR_COLOR &color)
{
    int idx = static_cast<int>(color);
    if (idx >= 0 && idx < static_cast<int>(aih::color_pair.size()))
    {
        os << aih::color_pair[idx];
    }
    else
    {
        os << "unknown_color";
    }
    return os;
}

inline std::ostream &
operator<<(std::ostream &os, const aih::BRAND_TYPE &type)
{
    int idx = static_cast<int>(type);
    if (idx >= 0 && idx < static_cast<int>(aih::type_pair.size()))
    {
        os << aih::type_pair[idx];
    }
    else
    {
        os << "unknown_type";
    }
    return os;
}

inline const std::string &
objMainClsToString(aih::ObjMainCls cls)
{
    static std::map<aih::ObjMainCls, std::string> clsMap = {{aih::OBJ_MAIN_CLS_UNKNOWN, "UNKNOWN"},
                                                            {aih::OBJ_MAIN_CLS_PERSON, "PERSON"},
                                                            {aih::OBJ_MAIN_CLS_AOTOMOBILE, "AOTOMOBILE"},
                                                            {aih::OBJ_MAIN_CLS_NONVEHICLE, "NONVEHICLE"},
                                                            {aih::OBJ_MAIN_CLS_PARKING, "PARKING"},
                                                            {aih::OBJ_MAIN_CLS_CAR_CLOTHES, "CAR_CLOTHES"},
                                                            {aih::OBJ_MAIN_CLS_MANHOLE, "MANHOLE"},
                                                            {aih::OBJ_MAIN_CLS_UNCOVERED_MANHOLE, "UNCOVERED_MANHOLE"},
                                                            {aih::OBJ_MAIN_CLS_TRICYCLE, "TRICYCLE"},
                                                            {aih::OBJ_MAIN_CLS_STREET_STALL, "STREET_STALL"},
                                                            {aih::OBJ_MAIN_CLS_HANDCART, "HANDCART"},
                                                            {aih::OBJ_MAIN_CLS_BILLBOARD, "BILLBOARD"},
                                                            {aih::OBJ_COMPT_TYPE_OTHER_CAT, "OTHER_CAT"},
                                                            {aih::OBJ_COMPT_TYPE_OTHER_DOG, "OTHER_DOG"},
                                                            {aih::OBJ_COMPT_TYPE_OTHER_CHICKEN, "OTHER_CHICKEN"},
                                                            {aih::OBJ_COMPT_TYPE_OTHER_DUCK, "OTHER_DUCK"},
                                                            {aih::OBJ_COMPT_TYPE_OTHER_CATTLE, "OTHER_CATTLE"},
                                                            {aih::OBJ_COMPT_TYPE_OTHER_SHEEP, "OTHER_SHEEP"},
                                                            {aih::OBJ_COMPT_TYPE_OTHER_PIG, "OTHER_PIG"},
                                                            {aih::OBJ_COMPT_TYPE_OTHER_GOOSE, "OTHER_GOOSE"},
                                                            {aih::CLS_OTHER, "OTHER"}};
    return clsMap[cls];
}

// ObjComponetType 转字符串
inline const std::string &
objComponetTypeToString(aih::ObjComponetType type)
{
    static std::map<aih::ObjComponetType, std::string> typeMap = {
        {aih::OBJ_COMPT_TYPE_UNKNOWN, "UNKNOWN"},
        {aih::OBJ_COMPT_TYPE_PERSION_HEAD, "PERSION_HEAD"},
        {aih::OBJ_COMPT_TYPE_PERSION_FACE, "PERSION_FACE"},
        {aih::OBJ_COMPT_TYPE_PLATE, "PLATE"},
        {aih::OBJ_COMPT_TYPE_HEADSTOCK, "HEADSTOCK"},
        {aih::OBJ_COMPT_TYPE_TAILSTOCK, "TAILSTOCK"},
        {aih::OBJ_COMPT_TYPE_CAR, "CAR"},
        {aih::OBJ_COMPT_TYPE_SIDE_WINDOW, "SIDE_WINDOW"},
        {aih::OBJ_COMPT_TYPE_WINDOW, "WINDOW"},
        {aih::OBJ_COMPT_TYPE_ROOF, "ROOF"},
        {aih::OBJ_COMPT_TYPE_CAR_CLOTHES, "CAR_CLOTHES"},
        {aih::OBJ_COMPT_TYPE_MANHOLE, "MANHOLE"},
        {aih::OBJ_COMPT_TYPE_UNCOVERED_MANHOLE, "UNCOVERED_MANHOLE"},
        {aih::OBJ_COMPT_TYPE_NONVEHICLE, "NONVEHICLE"},
        {aih::OBJ_COMPT_TYPE_TRICYCLE, "TRICYCLE"},
        {aih::OBJ_COMPT_TYPE_STREET_STALL, "STREET_STALL"},
        {aih::OBJ_COMPT_TYPE_HANDCART, "HANDCART"},
        {aih::OBJ_COMPT_TYPE_PERSION_BODY, "PERSION_BODY"},
        {aih::OBJ_COMPT_TYPE_BILLBOARD, "BILLBOARD"},
        {aih::OBJ_COMPT_TYPE_NUM, "NUM"}};
    return typeMap[type];
}

// ControlType 转字符串
inline const std::string &
controlTypeToString(aih::ControlType type)
{
    static std::map<aih::ControlType, std::string> typeMap = {{aih::CONTROL_TYPE_UNKNOWN, "UNKNOWN"},
                                                              {aih::CONTROL_TYPE_AREA, "AREA"},
                                                              {aih::CONTROL_TYPE_LINE, "LINE"},
                                                              {aih::CONTROL_TYPE_BERTH, "BERTH"},
                                                              {aih::CONTROL_TYPE_VIOLATION, "VIOLATION"},
                                                              {aih::CONTROL_TYPE_LANE, "LANE"},
                                                              {aih::CONTROL_TYPE_NUM, "NUM"}};
    return typeMap[type];
}

// EventType 转字符串
inline const std::string &
eventTypeToString(aih::EventType type)
{
    static std::map<aih::EventType, std::string> typeMap = {{aih::EVENT_TYPE_NO_EVENT, "NO_EVENT"},
                                                            {aih::EVENT_TYPE_CROSS_LINE, "CROSS_LINE"},
                                                            {aih::EVENT_TYPE_ENTER_AREA, "ENTER_AREA"},
                                                            {aih::EVENT_TYPE_STAY_AREA, "STAY_AREA"},
                                                            {aih::EVENT_TYPE_LEAVE_AREA, "LEAVE_AREA"},
                                                            {aih::EVENT_TYPE_SHIELD_AREA, "SHIELD_AREA"},
                                                            {aih::EVENT_TYPE_MUTI_STAY_AREA, "MUTI_STAY_AREA"},
                                                            {aih::EVENT_TYPE_ENTER_BERTH, "ENTER_BERTH"},
                                                            {aih::EVENT_TYPE_STAY_BERTH, "STAY_BERTH"},
                                                            {aih::EVENT_TYPE_LEAVE_BERTH, "LEAVE_BERTH"},
                                                            {aih::EVENT_TYPE_BERTH_REALTIME, "BERTH_REALTIME"},
                                                            {aih::EVENT_TYPE_ALARM_BERTH, "ALARM_BERTH"},
                                                            {aih::EVENT_TYPE_ENTER_VIOLATION, "ENTER_VIOLATION"},
                                                            {aih::EVENT_TYPE_ALARM_VIOLATION, "ALARM_VIOLATION"},
                                                            {aih::EVENT_TYPE_LANE_CONGESTION, "LANE_CONGESTION"}};
    return typeMap[type];
}

// MoveDirectionThroughLine 转字符串
inline const std::string &
moveDirToString(aih::MoveDirectionThroughLine dir)
{
    static std::map<aih::MoveDirectionThroughLine, std::string> dirMap = {
        {aih::MOVE_DIRECTION_ANY, "ANY"},
        {aih::MOVE_DIRECTION_LEFT2RIGHT, "LEFT2RIGHT"},
        {aih::MOVE_DIRECTION_RIGHT2LEFT, "RIGHT2LEFT"}};
    return dirMap[dir];
}

// LoopStatus 转字符串
inline const std::string &
loopStatusToString(aih::LoopStatus status)
{
    static std::map<aih::LoopStatus, std::string> statusMap = {
        {aih::LOOP_STATUS_UNKNOWN, "UNKNOWN"},
        {aih::LOOP_STATUS_IN, "IN"},
        {aih::LOOP_STATUS_STABLE, "STABLE"},
        {aih::LOOP_STATUS_OUT, "OUT"},
        {aih::LOOP_STATUS_FREE, "FREE"},
        {aih::LOOP_STATUS_ERROR, "ERROR"},
        {aih::LOOP_STATUS_OUTDELAY, "OUTDELAY"},
        {aih::LOOP_STATUS_INDELAY, "INDELAY"},
        {aih::LOOP_STATUS_PREIN, "PREIN"},
        {aih::LOOP_STATUS_PREOUT, "PREOUT"},
        {aih::LOOP_STATUS_PLATE_UPDATE, "PLATE_UPDATE"},
        {aih::LOOP_STATUS_RECO_TRIGGER, "RECO_TRIGGER"},
        {aih::LOOP_STATUS_USER_EXT_FRAME_FIRST, "USER_EXT_FRAME_FIRST"},
        {aih::LOOP_STATUS_USER_EXT_FRAME_SECOND, "USER_EXT_FRAME_SECOND"},
        {aih::LOOP_STATUS_USER_EXT_FRAME_THIRD, "USER_EXT_FRAME_THIRD"}};
    return statusMap[status];
}

// ParkErrorType 转字符串
inline const std::string &
parkErrorTypeToString(aih::ParkErrorType err)
{
    static std::map<aih::ParkErrorType, std::string> errMap = {
        {aih::ParkErrorType_Non, "Non"},
        {aih::ParkErrorType_VideoBlurError, "VideoBlurError"},
        {aih::ParkErrorType_PlateCoveredError, "PlateCoveredError"},
        {aih::ParkErrorType_NoPlateError, "NoPlateError"},
        {aih::ParkErrorType_NonMotorError, "NonMotorError"},
        {aih::ParkErrorType_ProhibitError, "ProhibitError"},
        {aih::ParkErrorType_ReserveError, "ReserveError"},
        {aih::ParkErrorType_CrossLineError, "CrossLineError"},
        {aih::ParkErrorType_CrossSlotError, "CrossSlotError"},
        {aih::ParkErrorType_VerticalParkError, "VerticalParkError"},
        {aih::ParkErrorType_DiagonalParkError, "DiagonalParkError"},
        {aih::ParkErrorType_BanTimeError, "BanTimeError"},
        {aih::ParkErrorType_ProbablyParkInError, "ProbablyParkInError"},
        {aih::ParkErrorType_ProbablyParkOutError, "ProbablyParkOutError"},
        {aih::ParkErrorType_LongTermParkError, "LongTermParkError"}};
    return errMap[err];
}

template <typename tp>
std::ostream &
operator<<(std::ostream &os, const aih::_Point<tp> &p)
{
    os << "Point{x=" << p.x << ", y=" << p.y << "}";
    return os;
}

// 线模板输出
template <typename tp>
std::ostream &
operator<<(std::ostream &os, const aih::_Line<tp> &l)
{
    os << "Line{start=" << l.start_point_ << ", end=" << l.end_point_ << "}";
    return os;
}

// TimeInfo 输出
inline std::ostream &
operator<<(std::ostream &os, const aih::TimeInfo &ti)
{
    os << "TimeInfo{pts=" << ti.pts_ << ", local_time=" << ti.local_time_ << "}";
    return os;
}

// ImageInfo结构体输出
inline std::ostream &
operator<<(std::ostream &os, const aih::ImageInfo &ii)
{
    os << "(ImageInfo) {\n";
    os << "  phy_: [" << ii.phy_[0] << ", " << ii.phy_[1] << ", " << ii.phy_[2] << "],\n";
    os << "  vir_: [" << ii.vir_[0] << ", " << ii.vir_[1] << ", " << ii.vir_[2] << "],\n";
    os << "  stride_: [" << ii.stride_[0] << ", " << ii.stride_[1] << ", " << ii.stride_[2] << "],\n";
    os << "  width_: " << ii.width_ << ", height_: " << ii.height_ << ",\n";
    os << "  time_info_: " << ii.time_info_ << "\n}";
    return os;
}

// ALG_ObjRect结构体输出
inline std::ostream &
operator<<(std::ostream &os, const aih::ALG_ObjRect &rect)
{
    os << "(ALG_ObjRect) { x: " << rect.x_ << ", y: " << rect.y_ << ", width: " << rect.width_
        << ", height: " << rect.height_ << " }";
    return os;
}

// PlateInfo结构体输出
inline std::ostream &
operator<<(std::ostream &os, const aih::PlateInfo &pi)
{
    os << "(PlateInfo) {\n";
    os << "  plate_score_: " << pi.plate_score_ << ", plate_type_prob_: " << pi.plate_type_prob_ << ",\n";
    os << "  plate_str_: \"" << pi.plate_str_ << "\",\n";
    os << "  plate_type_id_: " << pi.plate_type_id_ << ", plate_color_id_: " << pi.plate_color_id_
        << ", plate_layer_id_: " << pi.plate_layer_id_ << ",\n";
    os << "  plate_type_str: \"" << pi.plate_type_str << "\", plate_color_str_: \"" << pi.plate_color_str_
        << "\", plate_layer_str_: \"" << pi.plate_layer_str_ << "\",\n";
    os << "  plate_scores_: [";
    for (size_t i = 0; i < pi.plate_scores_.size(); ++i)
    {
        if (i > 0)
            os << ", ";
        os << pi.plate_scores_[i];
    }
    os << "]\n}";
    return os;
}

// PlateInfoExt结构体输出
inline std::ostream &
operator<<(std::ostream &os, const aih::PlateInfoExt &pie)
{
    os << "(PlateInfoExt) {\n";
    os << "  info: " << pie.info << ",\n";
    os << "  plate_score: " << pie.plate_score << ", plate_width: " << pie.plate_width << ",\n";
    os << "  plate_loc: " << pie.plate_loc << "\n}";
    return os;
}

// SinglePropDefine结构体输出
inline std::ostream &
operator<<(std::ostream &os, const aih::SinglePropDefine &spd)
{
    os << "(SinglePropDefine) { type_id_: " << spd.type_id_ << ", type_str_: \"" << spd.type_str_
        << "\", type_score_: " << spd.type_score_ << " }";
    return os;
}

// CarProperty结构体输出
inline std::ostream &
operator<<(std::ostream &os, const aih::CarProperty &cp)
{
    os << "(CarProperty) {\n";
    os << "  car_plate_score_: " << cp.car_plate_score_ << ", car_plate_width_: " << cp.car_plate_width_ << ",\n";
    os << "  plate_info_: " << cp.plate_info_ << ",\n";
    os << "  plate_infos_: [\n";
    for (const auto &pi : cp.plate_infos_)
    {
        os << "    " << pi << ",\n";
    }
    os << "  ],\n";
    os << "  car_type_: " << cp.car_type_ << ", car_color_: " << cp.car_color_ << ", car_brand_: " << cp.car_brand_
        << "\n}";
    return os;
}

// ControlInfo结构体输出
inline std::ostream &
operator<<(std::ostream &os, const aih::ControlInfo &ci)
{
    os << "(ControlInfo) {\n";
    os << "  id_: " << ci.id_ << ", control_type_: " << ci.control_type_ << ",\n";
    os << "  point_infos_: [";
    for (size_t i = 0; i < ci.point_infos_.size(); ++i)
    {
        if (i > 0)
            os << ", ";
        os << ci.point_infos_[i];
    }
    os << "]\n}";
    return os;
}

// SingleCrossLineEvent结构体输出
inline std::ostream &
operator<<(std::ostream &os, const aih::SingleCrossLineEvent &scle)
{
    os << "(SingleCrossLineEvent) { move_direction_: " << scle.move_direction_
        << ", crossline_area_percent_: " << scle.crossline_area_percent_ << " }";
    return os;
}

// CrossLineEventCfg结构体输出
inline std::ostream &
operator<<(std::ostream &os, const aih::CrossLineEventCfg &clec)
{
    os << "(CrossLineEventCfg) { crossline_events_: [";
    for (size_t i = 0; i < clec.crossline_events_.size(); ++i)
    {
        if (i > 0)
            os << ", ";
        os << clec.crossline_events_[i];
    }
    os << "] }";
    return os;
}

// SingleEnterAreaEvent结构体输出
inline std::ostream &
operator<<(std::ostream &os, const aih::SingleEnterAreaEvent &sea)
{
    os << "(SingleEnterAreaEvent) { in_area_percent_: " << sea.in_area_percent_ << " }";
    return os;
}

// SingleOutputAreaEvent结构体输出
inline std::ostream &
operator<<(std::ostream &os, const aih::SingleOutputAreaEvent &soa)
{
    os << "(SingleOutputAreaEvent) { enter_in_area_percent_: " << soa.enter_in_area_percent_
        << ", leave_in_area_percent_: " << soa.leave_in_area_percent_ << " }";
    return os;
}

// SingleStayAreaEvent结构体输出
inline std::ostream &
operator<<(std::ostream &os, const aih::SingleStayAreaEvent &ssa)
{
    os << "(SingleStayAreaEvent) { interval_trigger_: " << std::boolalpha << ssa.interval_trigger_
        << ", stay_time_: " << ssa.stay_time_ << ", in_area_percent_: " << ssa.in_area_percent_ << " }";
    return os;
}

// MutiStayAreaEvent结构体输出
inline std::ostream &
operator<<(std::ostream &os, const aih::MutiStayAreaEvent &msa)
{
    os << "(MutiStayAreaEvent) { interval_trigger_: " << std::boolalpha << msa.interval_trigger_
        << ", stay_count_: " << msa.stay_count_ << ", stay_time_: " << msa.stay_time_
        << ", in_area_percent_: " << msa.in_area_percent_ << " }";
    return os;
}

// DeployTime结构体输出
inline std::ostream &
operator<<(std::ostream &os, const aih::DeployTime &dt)
{
    os << "(DeployTime) { start_time: " << dt.start_time << ", end_time: " << dt.end_time << " }";
    return os;
}

// DeployTimeCfg结构体输出
inline std::ostream &
operator<<(std::ostream &os, const aih::DeployTimeCfg &dtc)
{
    os << "(DeployTimeCfg) {\n";
    for (int i = 0; i < 7; ++i)
    {
        os << "  day " << i << ": time_counter: " << dtc.time_counter[i] << ", time_durings: [";
        for (aih::U32 j = 0; j < dtc.time_counter[i]; ++j)
        {
            if (j > 0)
                os << ", ";
            os << dtc.time_durings[i][j];
        }
        os << "],\n";
    }
    os << "}";
    return os;
}

// LoopAlarmParam结构体输出
inline std::ostream &
operator<<(std::ostream &os, const aih::LoopAlarmParam &lap)
{
    os << "(LoopAlarmParam) {\n";
    os << "  bReportNonRecoPlate: " << std::boolalpha << lap.bReportNonRecoPlate << ",\n";
    os << "  bReportNoPlateError: " << lap.bReportNoPlateError << ",\n";
    os << "  bReportProbablyInOutError: " << lap.bReportProbablyInOutError << ",\n";
    os << "  bReportReverseParkingError: " << lap.bReportReverseParkingError << ",\n";
    os << "  reportReverseParkingType: " << lap.reportReverseParkingType << ",\n";
    os << "  bReportCrossLineError: " << lap.bReportCrossLineError << ",\n";
    os << "  reportCrossLineSensitivity: " << lap.reportCrossLineSensitivity << ",\n";
    os << "  bReportCrossBerthError: " << lap.bReportCrossBerthError << ",\n";
    os << "  reportCrossBerthSensitivity: " << lap.reportCrossBerthSensitivity << ",\n";
    os << "  reportWholeCrossedBerth: " << lap.reportWholeCrossedBerth << ",\n";
    os << "  bReportVerticalParkingError: " << lap.bReportVerticalParkingError << ",\n";
    os << "  bReportDiagonalParkingError: " << lap.bReportDiagonalParkingError << ",\n";
    os << "  bReportLongTermParkingError: " << lap.bReportLongTermParkingError << ",\n";
    os << "  reportLongTermParkingWaitTime: " << lap.reportLongTermParkingWaitTime << ",\n";
    os << "  bReportBanTimeError: " << lap.bReportBanTimeError << ",\n";
    os << "  reportBanTimeWaitTime: " << lap.reportBanTimeWaitTime << ",\n";
    os << "  banTimeDurings: " << lap.banTimeDurings << "\n}";
    return os;
}

// ViolationAlarmParam结构体输出
inline std::ostream &
operator<<(std::ostream &os, const aih::ViolationAlarmParam &vap)
{
    os << "(ViolationAlarmParam) { reportProhibitWaitTime: " << vap.reportProhibitWaitTime
        << ", violationType: " << vap.violationType << " }";
    return os;
}

// berth_event_cfg结构体输出
inline std::ostream &
operator<<(std::ostream &os, const aih::berth_event_cfg &bec)
{
    os << "(berth_event_cfg) {\n";
    os << "  report_mode: " << bec.report_mode << ", coil_outspread: " << bec.coil_outspread << ",\n";
    os << "  coil_sensitivity: " << bec.coil_sensitivity << ", pic_select_in_time: " << bec.pic_select_in_time << ",\n";
    os << "  pic_select_in_time_min: " << bec.pic_select_in_time_min
        << ", pic_select_out_time: " << bec.pic_select_out_time << ",\n";
    os << "  pic_select_out_time_min: " << bec.pic_select_out_time_min << ", report_nonmotor: " << std::boolalpha
        << bec.report_nonmotor << ",\n";
    os << "  default_province: " << bec.default_province << ", recognition_type: " << bec.recognition_type << ",\n";
    os << "  enter_pic_num: " << bec.enter_pic_num << ", leave_pic_num: " << bec.leave_pic_num << "\n}";
    return os;
}

// SingleEventCfgInfo联合体输出（注：联合体成员互斥，此处列出所有可能成员）
inline std::ostream &
operator<<(std::ostream &os, const aih::SingleEventCfgInfo &secf)
{
    os << "(SingleEventCfgInfo) {\n";
    os << "  line_cfg_: " << secf.line_cfg_ << ",\n";
    os << "  enter_area_cfg_: " << secf.enter_area_cfg_ << ",\n";
    os << "  leave_area_cfg_: " << secf.leave_area_cfg_ << ",\n";
    os << "  stay_area_cfg_: " << secf.stay_area_cfg_ << ",\n";
    os << "  muti_stay_area_cfg_: " << secf.muti_stay_area_cfg_ << ",\n";
    os << "  shield_area_cfg_: " << secf.shield_area_cfg_ << ",\n";
    os << "  berth_cfg_: " << secf.berth_cfg_ << ",\n";
    os << "  berth_alarm_cfg_: " << secf.berth_alarm_cfg_ << ",\n";
    os << "  violation_cfg_: " << secf.violation_cfg_ << ",\n";
    os << "  violation_alarm_cfg_: " << secf.violation_alarm_cfg_ << ",\n";
    os << "  lane_cfg_: " << secf.lane_cfg_ << "\n}";
    return os;
}

// EventFilterCfg结构体输出
inline std::ostream &
operator<<(std::ostream &os, const aih::EventFilterCfg &efc)
{
    os << "(EventFilterCfg) { obj_width_: " << efc.obj_width_ << ", obj_height_: " << efc.obj_height_
        << ", obj_score_: " << efc.obj_score_ << ", obj_det_score_: " << efc.obj_det_score_ << " }";
    return os;
}

inline std::ostream &
operator<<(std::ostream &os, const aih::SingleEventCfg &sec)
{
    os << "(SingleEventCfg) {\n";
    os << "  control_id_: " << sec.control_id_ << ", event_id_: " << sec.event_id_ << ",\n";
    os << "  event_type_: " << sec.event_type_ << ", enable_event_: " << std::boolalpha << sec.enable_event_ << ",\n";
    os << "  deploy_time_cfg_: " << sec.deploy_time_cfg_ << ",\n";
    os << "  obj_main_cls_filter_: [";
    for (auto it = sec.obj_main_cls_filter_.begin(); it != sec.obj_main_cls_filter_.end(); ++it)
    {
        if (it != sec.obj_main_cls_filter_.begin())
            os << ", ";
        os << *it;
    }
    os << "],\n";
    os << "  obj_filter_cfg_: {\n";
    for (auto it = sec.obj_filter_cfg_.begin(); it != sec.obj_filter_cfg_.end(); ++it)
    {
        if (it != sec.obj_filter_cfg_.begin())
            os << ",\n";
        os << "    " << it->first << ": " << it->second;
    }
    os << "\n  },\n";
    os << "  cfg_info_: " << sec.cfg_info_ << "\n}";
    return os;
}

inline std::ostream &
operator<<(std::ostream &os, const aih::CachedFrameInfo &cfi)
{
    os << "(CachedFrameInfo) { tm_info_: " << cfi.tm_info_ << " }";
    return os;
}

inline std::ostream &
operator<<(std::ostream &os, const aih::AlarmInfo &ai)
{
    os << "(AlarmInfo) { alarm_type_: " << static_cast<aih::ParkErrorType>(ai.alarm_type_)
        << ", alarm_status_: " << static_cast<aih::ParkErrorType>(ai.alarm_status_) << " }";
    return os;
}

// BerthCarInfo结构体输出
inline std::ostream &
operator<<(std::ostream &os, const aih::BerthCarInfo &bci)
{
    os << "(BerthCarInfo) {\n";
    os << "  car_loc_: " << bci.car_loc_ << ",\n";
    os << "  tricycle_loc_: " << bci.tricycle_loc_ << ",\n";
    os << "  plate_loc_: " << bci.plate_loc_ << ",\n";
    os << "  plate_info_: " << bci.plate_info_ << ",\n";
    os << "  covered_score_: " << bci.covered_score_ << ", has_car_body_: " << bci.has_car_body_ << ",\n";
    os << "  plate_isocc_: " << bci.plate_isocc_ << ", plate_occ_rate_: " << bci.plate_occ_rate_ << ",\n";
    os << "  confidence_: " << bci.confidence_ << "\n}";
    return os;
}

// BerthEventInfo结构体输出
inline std::ostream &
operator<<(std::ostream &os, const aih::BerthEventInfo &bei)
{
    os << "(BerthEventInfo) {\n";
    os << "  result_cnt_: " << bei.result_cnt_ << ",\n";
    os << "  alarm_info_: " << bei.alarm_info_ << ",\n";
    os << "  report_mode_: " << bei.report_mode_ << ",\n";
    os << "  car_property_: " << bei.car_property_ << ",\n";
    os << "  can_drop_: [";
    for (unsigned int i = 0; i < bei.result_cnt_ && i < MAX_RESULT_CNT; ++i)
    {
        if (i > 0)
            os << ", ";
        os << bei.can_drop_[i];
    }
    os << "],\n";

    os << "  sharpness_: [";
    for (unsigned int i = 0; i < bei.result_cnt_ && i < MAX_RESULT_CNT; ++i)
    {
        if (i > 0)
            os << ", ";
        os << bei.sharpness_[i];
    }
    os << "],\n";

    os << "  plate_mean_: [";
    for (unsigned int i = 0; i < bei.result_cnt_ && i < MAX_RESULT_CNT; ++i)
    {
        if (i > 0)
            os << ", ";
        os << bei.plate_mean_[i];
    }
    os << "],\n";


    os << "  parking_state_: [";
    for (unsigned int i = 0; i < bei.result_cnt_ && i < MAX_RESULT_CNT; ++i)
    {
        if (i > 0)
            os << ", ";
        os << bei.parking_state_[i];
    }
    os << "],\n";
    os << "  car_info_: [\n";
    for (unsigned int i = 0; i < bei.result_cnt_ && i < MAX_RESULT_CNT; ++i)
    {
        os << "    " << bei.car_info_[i] << ",\n";
    }
    os << "  ],\n";
    os << "  order_score_: " << bei.order_score_ << ",\n";
    os << "  covered_event: " << bei.covered_event << ", enter_start_timestamp_: " << bei.enter_start_timestamp_
        << ",\n";
    os << "  force_leave_flag_: " << bei.force_leave_flag_ << ", group_id_: " << bei.group_id_ << ",\n";
    os << "  non_motor_id_: " << bei.non_motor_id_ << ",\n";
    os << "  recent_plate_: " << bei.recent_plate_ << ",\n";
    os << "  cached_frame_infos_: [";
    for (unsigned int i = 0; i < bei.result_cnt_ && i < MAX_RESULT_CNT; ++i)
    {
        if (i > 0)
            os << ", ";
        os << bei.frame_info_[i];
    }
    os << "],\n";
    os << "  plate_covered_: " << bei.plate_covered_ << ", car_stable_count_: " << bei.car_stable_count_ << "\n}";


    return os;
}

// EventInfo结构体输出
inline std::ostream &
operator<<(std::ostream &os, const aih::EventInfo &ei)
{
    os << "(EventInfo) {\n";
    os << "  event_type_: " << eventTypeToString(ei.event_type_) << ", track_id_: " << ei.track_id_ << ",\n";
    os << "  control_id_: " << ei.control_id_ << ", event_id_: " << ei.event_id_ << ",\n";
    os << "  time_info_: " << ei.time_info_ << ",\n";
    os << "  move_line_dir_: " << ei.move_line_dir_ << ",\n";
    os << "  berth_event_info_: " << ei.berth_event_info_ << "\n";
    return os;
}

// CountObjCfg结构体输出
inline std::ostream &
operator<<(std::ostream &os, const aih::CountObjCfg &coc)
{
    os << "(CountObjCfg) { ctrl_id: " << coc.ctrl_id << ", obj_cls: " << coc.obj_cls
        << ", in_area_percent: " << coc.in_area_percent << " }";
    return os;
}

inline std::ostream &
operator<<(std::ostream &os, const Point &p)
{
    os << "Point(" << p.x_ << ", " << p.y_ << ")";
    return os;
}

inline std::ostream &
operator<<(std::ostream &os, const Line &l)
{
    os << "Line{start=" << l.start_point_ << ", end=" << l.end_point_ << "}";
    return os;
}

} // namespace sonli

#endif // ACL_DET_YOLO_AIH_EXTERN_H
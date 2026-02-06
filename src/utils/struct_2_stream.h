#ifndef STRUCT_2_STREAM_H
#define STRUCT_2_STREAM_H

#include "data_type.h"
#include <iosfwd>
#include <vector>
#include "../interfaces/aih_extern.h"
#include "parking_config.h"

namespace sonli::illegal_parking
{
struct NoParkingAreaConfig;
}

std::ostream &
operator<<(std::ostream &Os, const aih::SingleCrossLineEvent &Obj);

std::ostream &
operator<<(std::ostream &Os, const aih::_area_event &Obj);

std::ostream &
operator<<(std::ostream &Os, const aih::DeployTime &Obj);

std::ostream &
operator<<(std::ostream &Os, const aih::ControlInfo &Obj);

std::ostream &
operator<<(std::ostream &Os, const aih::berth_event_cfg &Obj);

std::ostream &
operator<<(std::ostream &Os, const aih::SingleEventCfg &Obj);

std::ostream &
operator<<(std::ostream &Os, const aih::LoopAlarmParam &Obj);

std::ostream &
operator<<(std::ostream &Os, const aih::DeployTimeCfg &Obj);

std::ostream &
operator<<(std::ostream &Os, const aih::ViolationAlarmParam &Obj);

std::ostream &
operator<<(std::ostream &Os, std::pair<int, const aih::SingleEventCfgInfo &> Obj);

std::ostream &
operator<<(std::ostream &os, const sonli::VehInfoVec &obj);

std::ostream &
operator<<(std::ostream &os, const sonli::VehInfo &obj);

std::ostream &
operator<<(std::ostream &os, const sonli::ParkingInfo &obj);

// std::ostream &
// operator<<(std::ostream &os, const sonli::AccessConfig_t &obj);

std::ostream &
operator<<(std::ostream &os, const sonli::illegal_parking::NoParkingAreaConfig &obj);

std::ostream &
operator<<(std::ostream &os, const sonli::AccessConfig_t &config);

std::ostream &
operator<<(std::ostream &os, const sonli::ParkingInfoVec &obj);

inline std::ostream &
operator<<(std::ostream &os, const std::vector<float> &info)
{
    os << "(";
    for (float i : info)
    {
        os << i;
    }
    return os << ")";
}

std::ostream &
operator<<(std::ostream &Os, const sonli::Veh_Key_Point &Obj);

std::ostream &
operator<<(std::ostream &os, const sonli::VehChassisInfo &obj);

inline std::ostream &
operator<<(std::ostream &os, const sonli::BerthEventConfig &obj)
{
    os
        << "control_id: " << obj.control_id
        << " event_id: " << obj.event_id
        << " event_type: " << obj.event_type;
    if (obj.event_type == aih::EVENT_TYPE_ENTER_BERTH)
    {
        os << " alarm_events[1]: " << obj.config_detail.alarm_events;
        os << " berth_events[2]: " << obj.config_detail.berth_events;
    }
    else if (obj.event_type == aih::EVENT_TYPE_ALARM_VIOLATION)
    {
        os << " violation_alarm_cfg_[1]: " << obj.area_no_parking_config.violation_alarm_cfg_;
        os << " violation_cfg_[2]: " << obj.area_no_parking_config.violation_cfg_;
    }
    return os;
}

// std::ostream &
// operator<<(std::ostream &os, const aih::TimeInfo &time_info);
//
// std::ostream &
// operator<<(std::ostream &os, const aih::AlarmInfo &info);
//
// std::ostream &
// operator<<(std::ostream &os, const aih::PlateInfo &info);
#endif // STRUCT_2_STREAM_H
#include "illegal_parking_algorithm.h"
#include "../utils/sl_logger.h"
#include "../veh_in_out/space_judge.h"

namespace sonli
{
namespace illegal_parking
{


EIllegalCase
IllegalCaseMap(int aih_illegal_case)
{
    switch (aih_illegal_case)
    {
        case 0:
            return EIllegalCase::_BAN_EXTENT_PARKING;
        case 1:
            return EIllegalCase::_PARALLEL_PARKING;
        case 2:
            return EIllegalCase::_END_POINTS_PARKING;
        default:
            throw std::runtime_error("illegal case not map: " + std::to_string(aih_illegal_case));
    }
}

/**
 * @brief JudgeIllegalBaseRule 基础违停区域检测函数
 *
 * @param veh_chassis 车辆 chassis 坐标
 * @param car_in_rat 车辆与违停区域 IoU 阈值, 灵敏度
 * @param no_parking_area_configs 违停区域配置
 * @return std::shared_ptr<NoParkingAreaConfig> 违停区域配置指针
 */
static bool
JudgeVehHitChassisRule(const std::vector<Point> &veh_chassis,
                       const std::shared_ptr<NoParkingAreaConfig> &no_parking_area_configs, float car_in_rat = 0.6f)
{
    if (!no_parking_area_configs || veh_chassis.size() != 4)
    {
        return false;
    }

    float carIou = 0.0;
    float spaceIou = 0.0;
    (void)iou_poly(veh_chassis, no_parking_area_configs->zone, carIou, spaceIou);
    return (carIou + spaceIou) * 0.5 > car_in_rat;
}

void
JudgeIllegalNoParkingArea(const std::shared_ptr<NoParkingAreaConfigVec> &no_parking_area_configs,
                          CacheVehInfo &vehInfo) try
{
    if (!no_parking_area_configs)
    {
        // SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
        //     "JudgeIllegalNoParkingArea: empty no_parking_area_configs!");
        return;
    }

    for (const auto &rule : no_parking_area_configs->no_parking_area_configs_)
    {
        auto illegal_case = vehInfo.getMultipleIllegalCaptureDetail()
            ->Find(IllegalCaseMap(rule->violation_alarm_cfg_.violationType));

        // 违停区域检测
        auto hit = JudgeVehHitChassisRule(vehInfo.veh_infos.rbegin()->veh_chassis.chassis, rule, 0.6f);
        if (hit)
        {
            BerthEventConfig stand_event{};
            stand_event.control_id = rule->control_id;
            stand_event.event_id = rule->event_id;
            stand_event.event_type = aih::EVENT_TYPE_ALARM_VIOLATION;
            stand_event.area_no_parking_config.violation_alarm_cfg_ = rule->violation_alarm_cfg_;
            stand_event.area_no_parking_config.violation_cfg_ = rule->violation_cfg_;
            illegal_case->UpdateCapStamp(stand_event, *vehInfo.veh_infos.rbegin(), no_parking_area_configs->time_info_);
            // SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
            //     "JudgeIllegalNoParkingArea: hit no_parking_area_configs! control_id: {}, violationType: {}, track_id: {}, time_info: {}",
            //     rule->control_id, rule->violation_alarm_cfg_.violationType,
            //     vehInfo.veh_infos.rbegin()->veh_track_id, no_parking_area_configs->time_info_.pts_);
        }
    }
}
catch (std::exception &e)
{
    SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error(
        "JudgeIllegalNoParkingArea error: {}", e.what());
}


}
}
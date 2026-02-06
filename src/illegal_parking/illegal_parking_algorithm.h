#ifndef ACL_DET_YOLO_ILLEGAL_PARKING_ALGORITHM_H
#define ACL_DET_YOLO_ILLEGAL_PARKING_ALGORITHM_H

#include "capture_record_container.h"
#include "sl_data_types_base.h"
#include <memory>
#include <utility>
#include <vector>
#include "../interfaces/aih_type.h"
#include "data_type.h"
#include "illegal_history_record.h"
#include "unordered_set"
#include <ostream>

namespace sonli
{

namespace illegal_parking
{
struct IllegalParkingDetectionResult : public vulcan::BaseData
{
    std::unordered_set<aih::TimeInfo, PersonHash, TimeinfoCompare> illegal_parking_time_set;
    std::vector<std::shared_ptr<VehicleIllegalData> > vehicle_illegal_data;
    std::vector<std::shared_ptr<VehicleIllegalData>> logic_keep_frame_events;
};

// 单个违停区域配置
struct NoParkingAreaConfig
{
    int control_id = -1;
    int event_id = -1;
    aih::SingleViolationEvent violation_cfg_{};
    aih::SingleViolationAlarmEvent violation_alarm_cfg_{};
    std::vector<Point> zone; // 违停区域
};

// 多个违停区域配置
struct NoParkingAreaConfigVec : vulcan::BaseData
{
    explicit NoParkingAreaConfigVec(const aih::TimeInfo& time_info):BaseData(),time_info_(time_info)
    {
        name_ = "no_parking_area";
    }

    NoParkingAreaConfigVec(std::vector<std::shared_ptr<NoParkingAreaConfig> > no_parking_area_configs,
                           aih::TimeInfo time_info, bool this_frame_check)
        : no_parking_area_configs_(std::move(no_parking_area_configs)),
          time_info_(time_info),
          this_frame_check_(this_frame_check)
    {
    }

    std::vector<std::shared_ptr<NoParkingAreaConfig> > no_parking_area_configs_{};
    aih::TimeInfo time_info_{};
    bool this_frame_check_ = false;
};

class IllegalParkingAlgorithm
{
public:
    IllegalParkingAlgorithm();
    ~IllegalParkingAlgorithm() = default;
    std::shared_ptr<IllegalParkingDetectionResult>
    IllegalParkingEntry(const std::vector<CacheVehInfo *> &refreshed,
                        const std::shared_ptr<ParkingInfoVec> &parking_info_ptr,
                        const std::shared_ptr<NoParkingAreaConfigVec> &no_parking_area_configs);
private:
    std::shared_ptr<IllegalParkingHistoryRecord> _illegal_parking_history_record;
};

void
JudgeIllegalNoParkingArea(const std::shared_ptr<NoParkingAreaConfigVec> &no_parking_area_configs,
                          CacheVehInfo &vehInfo);
}


} // namespace sonli

#endif // ACL_DET_YOLO_ILLEGAL_PARKING_ALGORITHM_H
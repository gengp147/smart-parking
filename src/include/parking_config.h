#ifndef ACL_DET_YOLO_PARKING_CONFIG_H
#define ACL_DET_YOLO_PARKING_CONFIG_H
#include "../interfaces/aih_type.h"
#include "../interfaces/aih_extern.h"
#include "vulcan/data_struct.h"
#include "sl_data_types_base.h"

namespace sonli
{

struct BerthAlarmConfig
{
    aih::LoopAlarmParam alarm_events{};
    aih::berth_event_cfg berth_events{};

    BerthAlarmConfig()
    = default;

    ~BerthAlarmConfig()
    = default;
};

struct AreaNoParkingConfig
{
    aih::SingleViolationEvent violation_cfg_{};
    aih::SingleViolationAlarmEvent violation_alarm_cfg_{};
};

struct BerthEventConfig
{
    int control_id = -1;
    int event_id = -1;
    aih::EventType event_type = aih::EventType::EVENT_TYPE_NO_EVENT;
    BerthAlarmConfig config_detail{};
    AreaNoParkingConfig area_no_parking_config{};

    unsigned long
    GetNextFrameCaptureTime() const noexcept
    {
        return 60 * 1000;
    }
};


struct ParkingInfo final
{
    std::vector<Point> points{};
    int control_id = -1;
    int event_id = -1;
    BerthEventConfig berth_event_list{};
};

struct ParkingInfoVec : public vulcan::BaseData
{
    explicit ParkingInfoVec(aih::TimeInfo time_info)
        : vulcan::BaseData(), time_info_(time_info)
    {
        name_ = "parking";
        time_info_ = time_info;
    }

    int space_angle = 0; // 0:same side 1:oppesite side 2:under the camera
    uint16_t size = 0;
    std::vector<ParkingInfo> parking_info;
    int park_loc = 0;               // 车场在路的左右侧,0 未设置, 1 左侧, 2 右侧
    std::vector<Point> vector_;     // 泊位方向，起始点与终止点坐标

    aih::TimeInfo time_info_{};     //必填项，时间戳
};

typedef std::shared_ptr<ParkingInfoVec> ParkingInfoVecPtr;


}

#endif //ACL_DET_YOLO_PARKING_CONFIG_H
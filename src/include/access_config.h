#ifndef ACL_DET_YOLO_ACCESS_CONFIG_H
#define ACL_DET_YOLO_ACCESS_CONFIG_H
#include "data_type.h"
#include "sl_data_types_base.h"
#include "vulcan/data_struct.h"

namespace sonli
{
// Access data
struct VehAccessInfo : public vulcan::BaseData
{

    VehAccessInfo() = default;

    int control_id = -1;     /** 控制ID */
    int event_id = -1;       /** 事件ID */
    aih::TimeInfo time_info; /** 时间信息 */

    unsigned long long trigger_time = 0;            /** 触发时间 */
    Rect veh_location;                              /** 车辆位置 */
    Rect vlp_location;                              /** VLP位置 */
    std::string vlp = "-";                          /** VLP号码 */
    SLLPColor_t vlp_color = SL_PLATE_COLOR_UNKNOWN; /** VLP颜色 */
    float vlp_confidence = 0.0;                     /** VLP置信度 */
    VehInfo veh_detail_info;                        /** 车辆详细信息 */
};

typedef std::shared_ptr<VehAccessInfo> VehAccessInfoPtr;
/**
 * @brief 输出VehAccessInfo的信息
 * @param os 输出流
 * @param info 要输出的VehAccessInfo对象
 * @return std::ostream& 输出流
 */
struct VehAccessInfoVec : public vulcan::BaseData
{
    std::vector<VehAccessInfo> vehicles; /** 车辆访问信息 */
};

typedef std::shared_ptr<VehAccessInfoVec> VehAccessInfoVecPtr;

struct AccCollectResult : public vulcan::BaseData
{
    bool keep_frame = false;              /** 是否保留帧 */
    std::vector<VehAccessInfo> vehicles_; /** 卡口车辆信息信息 */
};

typedef std::shared_ptr<AccCollectResult> AccCollectResultPtr;

} // namespace sonli
#endif // ACL_DET_YOLO_ACCESS_CONFIG_H
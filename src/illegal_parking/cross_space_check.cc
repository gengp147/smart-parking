#include "capture_record_container.h"
#include "data_type.h"

namespace sonli
{
namespace illegal_parking
{
void
CrossParkingDetectPerFrame(CacheVehInfo *vehicle_cache, const std::shared_ptr<ParkingInfoVec> &parking_info_ptr,
                           const BerthEventConfig *rules)
{
    if (!vehicle_cache || !parking_info_ptr || !rules)
    {
        return;
    }

    if (vehicle_cache->veh_infos.size() < 2)
    {
        return;
    }
    auto cross_parking_record = vehicle_cache->getMultipleIllegalCaptureDetail()->Find(EIllegalCase::_CROSS_PARKING);
    if (cross_parking_record->WasReported())
    {
        return;
    }
    int value = 0;
    auto cache_space_rat = vehicle_cache->veh_infos.back().veh_chassis.car_rat_vec;
    std::sort(cache_space_rat.begin(), cache_space_rat.end(), [](const float a, const float b) { return a > b; });
    if (cache_space_rat.size() >= 2)
    {
        auto main = cache_space_rat[0];
        auto sec = cache_space_rat[1];
        if ((sec / main) > 0.40)
        {
            value += 1;
        }
    }

    // note: 使用两连续帧判断
    auto cache_space_rat_r_second = ((vehicle_cache->veh_infos.end() - 2))->veh_chassis.car_rat_vec;
    std::sort(cache_space_rat_r_second.begin(), cache_space_rat_r_second.end(),
              [](const float a, const float b) { return a > b; });
    if (cache_space_rat_r_second.size() >= 2)
    {
        auto main = cache_space_rat_r_second[0];
        auto sec = cache_space_rat_r_second[1];
        if ((sec / main) > 0.40)
        {
            value += 1;
        }
    }
    if (value == 2)
    {
        cross_parking_record->UpdateCapStamp(*rules, vehicle_cache->veh_infos.back(), parking_info_ptr->time_info_);
    }
}

} // namespace illegal_parking
} // namespace sonli
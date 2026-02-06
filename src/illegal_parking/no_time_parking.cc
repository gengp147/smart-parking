#ifndef ACL_DET_YOLO_TIME_COMPARE_H
#define ACL_DET_YOLO_TIME_COMPARE_H
#include "capture_record_container.h"
#include "data_type.h"

#include <chrono>
#include <iostream>
#include <ostream>
#include <time.h>
#include <sstream>

namespace sonli
{
namespace illegal_parking
{
int
ParseTimeToMinutes(const std::string &time_str)
{
    std::istringstream iss(time_str);
    int hours, minutes;
    char colon = 0;
    if (!(iss >> hours >> colon >> minutes) || colon != ':' || hours < 0 || hours >= 24 || minutes < 0 ||
        minutes >= 60)
    {
        throw std::invalid_argument("Invalid time format: " + time_str);
    }
    return hours * 60 + minutes;
}

int
TimestampToLocalMinutes(uint64_t timestamp_ms)
{
    std::chrono::system_clock::time_point tp{std::chrono::milliseconds(timestamp_ms)};
    time_t t = std::chrono::system_clock::to_time_t(tp);
    struct tm tm;
    localtime_r(&t, &tm);
    return tm.tm_hour * 60 + tm.tm_min;
}

bool
IsRestrictedPeriod(int restricted_min, int released_min, int current_min)
{
    if (restricted_min > released_min)
    {
        return current_min >= restricted_min || current_min < released_min;
    }

    return current_min >= restricted_min && current_min < released_min;
}

bool
IsParkingRestricted(const std::string &restricted_time, const std::string &released_time, uint64_t timestamp_ms)
{
    try
    {
        int restricted_min = ParseTimeToMinutes(restricted_time);
        int released_min = ParseTimeToMinutes(released_time);
        int current_min = TimestampToLocalMinutes(timestamp_ms);

        return IsRestrictedPeriod(restricted_min, released_min, current_min);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

bool
IsParkingRestricted(int restricted_min, int released_min, uint64_t timestamp_ms)
{
    try
    {
        int current_min = TimestampToLocalMinutes(timestamp_ms);
        return IsRestrictedPeriod(restricted_min, released_min, current_min);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

static int
timestamp_to_weekday(time_t timestamp)
{
    struct tm *tm_info = localtime(&timestamp);
    if (!tm_info)
    {
        return -1;
    }
    return tm_info->tm_wday;
}

void
ParkingTimeCheck(CacheVehInfo *vehicle_cache,
                 const std::shared_ptr<ParkingInfoVec> &parking_info_ptr,
                 const BerthEventConfig *rules)
{
    if (!vehicle_cache || !parking_info_ptr || !rules)
    {
        return;
    }

    auto ntp = vehicle_cache->getMultipleIllegalCaptureDetail()->Find(EIllegalCase::_INTO_ILLEGAL_TIME);
    if (ntp->WasReported())
    {
        return;
    }

    auto veh_timestamp_ms = time_t(vehicle_cache->veh_infos.back().time_info.local_time_ / 1000);
    auto index = timestamp_to_weekday(veh_timestamp_ms);
    if (index == -1)
    {
        return;
    }
    auto time_config_counter = rules->config_detail.alarm_events.banTimeDurings.time_counter[index];

    for (int i = 0; i < time_config_counter; i++)
    {
        auto s = rules->config_detail.alarm_events.banTimeDurings.time_durings[index][i].start_time;
        auto e = rules->config_detail.alarm_events.banTimeDurings.time_durings[index][i].end_time;
        auto ret = IsParkingRestricted(static_cast<int>(s), static_cast<int>(e), veh_timestamp_ms);
        if (!ret)
        {
            ntp->UpdateCapStamp(*rules, vehicle_cache->veh_infos.back(), parking_info_ptr->time_info_);
        }
    }
}

}
}
#endif //ACL_DET_YOLO_TIME_COMPARE_H
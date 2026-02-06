#include "cached_frames_manager.h"
#include "config_content.h"
#include "../utils/sl_logger.h"
#include "../utils/struct_2_string.h"


sonli::CachedFramesManager::CachedFramesManager(int max_length)
    : _interval_cached_frames(max_cap_buf_len_interval)
{
}

sonli::CachedFramesManager::~CachedFramesManager()
= default;

aih::TimeInfo
sonli::CachedFramesManager::FindTimeInfo(unsigned long time_stamp)
{
    for (int i = 0; i < _interval_cached_frames.size(); i++)
    {
        if (_interval_cached_frames[i].pts_ == time_stamp)
        {
            return _interval_cached_frames[i];
        }
    }

    for (const auto &it : keep_frame_list)
    {
        if (it.pts_ == time_stamp)
        {
            return it;
        }
    }

    SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error(
        "FindTimeInfo failed!  pts_time {}", time_stamp);
    aih::TimeInfo refresh{};
    refresh.pts_ = 0;
    refresh.local_time_ = 0;
    return refresh;
}

std::vector<aih::TimeInfo>
sonli::CachedFramesManager::GetAllCachedFrames() const
{
    std::vector<aih::TimeInfo> all_cached_frames;
    all_cached_frames.reserve(_interval_cached_frames.size() + keep_frame_list.size());

    std::set<aih::U64> unique_pts;

    for (auto i : _interval_cached_frames.readFirstNElements(-1))
    {
        all_cached_frames.push_back(i);
        unique_pts.insert(i.pts_);
    }
    for (auto i : keep_frame_list)
    {
        if (unique_pts.find(i.pts_) == unique_pts.end())
        {
            all_cached_frames.push_back(i);
        }
    }
    return all_cached_frames;
}

void
sonli::CachedFramesManager::Adjust()
{
    auto keep_counter = max_cap_buf_len;
    while (keep_frame_list.size() > keep_counter)
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
            "Discard inout parking evi pic!  pts_time {}", keep_frame_list.begin()->pts_);
        keep_frame_list.erase(keep_frame_list.begin());
    }
}
#ifndef ACL_DET_YOLO_CACHED_FRAMES_MANAGER_H
#define ACL_DET_YOLO_CACHED_FRAMES_MANAGER_H

#include "../interfaces/aih_type.h"
#include "../utils/CircularArray.h"
#include "../illegal_parking/capture_record_container.h"
#include <unordered_set>

namespace sonli
{

class CachedFramesManager
{
public:
    explicit
    CachedFramesManager(int max_length);

    ~CachedFramesManager();

    CircularArray<aih::TimeInfo> &
    GetIntervalCachedFrames()
    {
        return _interval_cached_frames;
    }

    aih::TimeInfo
    FindTimeInfo(unsigned long time_stamp);

    std::vector<aih::TimeInfo>
    GetAllCachedFrames() const;

    void JustAddTimeInfo(aih::TimeInfo &time_info)
    {
       if (keep_frame_list.find(time_info) != keep_frame_list.end())
       {
           keep_frame_list.insert(time_info);
           return;
       }
        for (int index = 0; index < _interval_cached_frames.size(); ++index)
        {
            if (_interval_cached_frames[index].pts_ == time_info.pts_)
            {
                return;
            }
        }
        keep_frame_list.insert(time_info);
    }

    void
    UpdateEviPic(const std::vector<aih::CachedFrameInfo> &inout_parking_evi_pic,
                 const std::unordered_set<aih::TimeInfo, illegal_parking::PersonHash, illegal_parking::TimeinfoCompare>
                 &illegal_parking_time_set)
    {
        keep_frame_list.clear();
        keep_frame_list = illegal_parking_time_set;
        for (const auto &p : inout_parking_evi_pic)
        {
            if (keep_frame_list.find(p.tm_info_) == keep_frame_list.end())
            {
                keep_frame_list.insert(p.tm_info_);
            }
        }
        Adjust();
    }

    void
    Adjust();

private:
    int max_length = 34;
    CircularArray<aih::TimeInfo> _interval_cached_frames;
    std::unordered_set<aih::TimeInfo, illegal_parking::PersonHash, illegal_parking::TimeinfoCompare>
    keep_frame_list;
};

} // namespace sonli

#endif // ACL_DET_YOLO_CACHED_FRAMES_MANAGER_H
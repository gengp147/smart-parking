#ifndef _ACCESS_ACCESS_DETECTOR_H_
#define _ACCESS_ACCESS_DETECTOR_H_

#include "data_type.h"
#include "../utils/struct_2_stream.h"
#include <ostream>
#include <unordered_map>
#include "access_config.h"

namespace sonli
{

/***
 * @description: Detect vehilces trigging access.
 */

struct TraceRecord
{
    std::string vlp_code = "-";
    SLLPColor_t vlp_color = SL_PLATE_COLOR_UNKNOWN;
    float vlp_confid = 0.0;
    Rect veh_location;
    unsigned long long appear_time = 0;

    friend std::ostream &
    operator<<(std::ostream &os, const TraceRecord &obj)
    {
        return os
               << "vlp_code: " << obj.vlp_code
               << " vlp_confid: " << obj.vlp_confid
               << " veh_location: " << obj.veh_location
               << " appear_time: " << obj.appear_time;
    }
};

// typedef std::shared_ptr<TraceRecord> TraceRecordPtr;

struct TempVeh
{
    long track_id = -1;
    bool trigger_top_line = false;
    bool trigger_bottom_line = false;
    unsigned long long trigger_first_line_time = 0;

    std::vector<TraceRecord> trace_record;

    friend std::ostream &
    operator<<(std::ostream &os, const TempVeh &obj)
    {
        os << "track_id: [" << obj.track_id << "]"
            << " trigger_top_line: [" << obj.trigger_top_line << "]"
            << " trigger_bottom_line: [" << obj.trigger_bottom_line << "]"
            << " trigger_first_line_time: [" << obj.trigger_first_line_time << "]";
        // if (obj.trace_record.size() <= 6)
        // {
        //     int record_idx = 0;
        //     for (auto &record : obj.trace_record)
        //     {
        //         os << " trace_record: [" << record_idx << "]" << record << std::endl;
        //         record_idx++;
        //     }
        // }
        // else
        // {
        //     for (int i = 0; i < 2; i++)
        //     {
        //         os << " trace_record: [" << i << "]" << obj.trace_record[i] << std::endl;
        //     }
        //     os << " ..." << std::endl;
        //     for (int i = obj.trace_record.size() - 2; i < obj.trace_record.size(); i++)
        //     {
        //         os << " trace_record: [" << i << "]" << obj.trace_record[i] << std::endl;
        //     }
        // }
        return os;
    }
};

// typedef std::shared_ptr<TempVeh> TempVehPtr;

struct AccessVeh
{
    long track_id = 0;
    std::string vlp_code = "-";
    unsigned long long trigger_time = 0;
};

typedef std::shared_ptr<AccessVeh> AccessVehPtr;

class AccessDetector
{
public:
    AccessDetector()
    {
    };

    AccessDetector(AccessConfig_t access_config);

    ~AccessDetector();

    void
    updateConfig(AccessConfig_t access_config);

    /// @brief detect access, either single or double lines
    /// @param veh_info_vec
    /// @param veh_info_vec_ptr
    /// @param veh_access_info
    /// @return
    int
    detect(const VehInfoVec &veh_info_vec, std::vector<VehAccessInfo> &veh_access_info);

private:
    bool
    compareConfig(AccessConfig_t cfg1, AccessConfig_t cfg2);

    int
    reset();

    /// @brief detect access with single line
    /// @param veh_info_vec
    /// @param line
    /// @param veh_access_info
    /// @return
    int
    detect1(const VehInfoVec &veh_info_vec, Line line, std::vector<VehAccessInfo> &veh_access_info);

    /// @brief detect access with double lines
    /// @param veh_info_vec
    /// @param line1
    /// @param line2
    /// @param veh_access_info
    /// @return
    int
    detect2(VehInfoVec veh_info_vec, Line line1, Line line2, std::vector<VehAccessInfo> &veh_access_info);

    bool
    trigger_single_line(TempVeh temp_veh, Line line);

    bool
    trigger_line(Rect box, Line line);

    int
    confirm_vlp(std::vector<TraceRecord> trace_record, std::string &vlp, float &vlp_confid);

    int
    confirm_color(std::vector<TraceRecord> trace_record, std::string vlp, SLLPColor_t &vlp_color);

    void
    updateRecord(aih::TimeInfo current_time);

    Point2f
    getVectf(Point start, Point end);

private:
    AccessConfig_t access_config_;

    std::unordered_map<int, TempVeh> temp_vehs; // <track_id, TempVeh>
    // recently reported vehicles, record by lp or track_id(if no lp detected)
    std::unordered_map<std::string, unsigned long long> reported_vehs_lp; // <vlp, report_time>
    // std::unordered_map<long, unsigned long long> reported_vehs_trackid;   // <track_id, report_time>
};

// typedef std::shared_ptr<AccessDetector> AccessDetectorPtr;

} // namespace sonli 


#endif
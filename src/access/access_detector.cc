#include <csignal>
#include <utility>

#include "../access/access_detector.h"
#include "../models_processor/char_code.h"
#include "../models_processor/model_process.h"
#include "error.h"
#include "opencv2/opencv.hpp"
#include "../utils/sl_logger.h"
#include "../utils/struct_2_string.h"

namespace sonli
{
AccessDetector::AccessDetector(AccessConfig_t access_config)
{
    access_config_ = access_config;
}

AccessDetector::~AccessDetector()
{
}

void
AccessDetector::updateConfig(AccessConfig_t access_config)
{
    if (!compareConfig(access_config_, access_config))
    {
        reset();
        access_config_ = access_config;
    }
}

bool
AccessDetector::compareConfig(AccessConfig_t cfg1, AccessConfig_t cfg2)
{
    if (cfg1.enable != cfg2.enable || cfg1.line_num != cfg2.line_num)
        return false;

    if ((cfg1.line1.start_point_.x_ != cfg2.line1.start_point_.x_) ||
        (cfg1.line1.start_point_.y_ != cfg2.line1.start_point_.y_) ||
        (cfg1.line1.end_point_.x_ != cfg2.line1.end_point_.x_) ||
        (cfg1.line1.end_point_.y_ != cfg2.line1.end_point_.y_))
    {
        return false;
    }

    if ((cfg1.line2.start_point_.x_ != cfg2.line2.start_point_.x_) ||
        (cfg1.line2.start_point_.y_ != cfg2.line2.start_point_.y_) ||
        (cfg1.line2.end_point_.x_ != cfg2.line2.end_point_.x_) ||
        (cfg1.line2.end_point_.y_ != cfg2.line2.end_point_.y_))
    {
        return false;
    }

    return true;
}

int
AccessDetector::reset()
{
    for (auto &kv : temp_vehs)
    {
        kv.second.trigger_first_line_time = 0;
        kv.second.trigger_top_line = false;
        kv.second.trigger_bottom_line = false;
    }

    return SL_SUCCESS;
}

int
AccessDetector::detect(const VehInfoVec &veh_info_vec, std::vector<VehAccessInfo> &veh_access_info)
{
    if (access_config_.line_num == 1)
    {
        detect1(veh_info_vec, access_config_.line1, veh_access_info);
    }
    else
    {
        detect2(veh_info_vec, access_config_.line1, access_config_.line2, veh_access_info);
    }

    for (auto &temp_access_info : veh_access_info)
    {
        temp_access_info.control_id = access_config_.l1_control_id;
        temp_access_info.event_id = access_config_.l1_event_id;
    }

    updateRecord(veh_info_vec.time_info);

    return SL_SUCCESS;
}

int
AccessDetector::detect1(const VehInfoVec &veh_info_vec, Line line, std::vector<VehAccessInfo> &veh_access_info_vec)
{
    veh_access_info_vec.clear();

    for (int i = 0; i < veh_info_vec.veh_info.size(); i++)
    {
        const VehInfo &oneVeh = veh_info_vec.veh_info.at(i);

        long trk_id = oneVeh.veh_track_id;
        // SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
        //  "[step1]Single line trigger begin judge: {}", TO_STRING(oneVeh));
        if (trk_id < 0)
        {
            continue;
        }

        // if(reported_vehs_lp.count(oneVeh->lp_info_ptr->lp_number_ptr->lp_str)
        //   || reported_vehs_trackid.count(trk_id))
        if (reported_vehs_lp.count(oneVeh.lp_info.lp_number.lp_str))
        {
        //     SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
        // "Already reported, no check again");
            continue;
        }

        if (!temp_vehs.count(trk_id))
        {
            TempVeh temp_veh_t;
            temp_veh_t.track_id = trk_id;
            temp_vehs[trk_id] = temp_veh_t;
        }

        TraceRecord trace;
        trace.vlp_code = oneVeh.lp_info.lp_number.lp_str;
        trace.vlp_color = oneVeh.lp_info.lp_color;
        trace.vlp_confid = oneVeh.lp_info.lp_number.vlp_rec_confidence;
        trace.veh_location = oneVeh.location;
        trace.appear_time = oneVeh.time_info.local_time_/1000;
        temp_vehs[trk_id].trace_record.push_back(trace);


        // SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
        // "[Cache]<{}><{}> : {}",trk_id, temp_vehs[trk_id].trace_record.size(), TO_STRING(trace));

        if (temp_vehs[trk_id].trace_record.size() < 10)
        {
            continue;
        }

        // SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
        //   "[trigger_single_line]checking : {}", TO_STRING(temp_vehs[trk_id]));
        bool ret = trigger_single_line(temp_vehs[trk_id], line);

        if (!ret)
        {
            // SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info("No trigger");
            continue;
        }
        // else
        // {
        //     SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info("Trigger");
        // }

        std::string vlp = "-";
        float vlp_confid = 0.0;
        confirm_vlp(temp_vehs[trk_id].trace_record, vlp, vlp_confid);

        // if(reported_vehs_lp.count(vlp)
        //   || reported_vehs_trackid.count(trk_id)){
        //     continue;
        // }

        if (vlp != "-" && !vlp.empty())
        {
            reported_vehs_lp[vlp] = oneVeh.time_info.local_time_/1000;
        }
        else
        {
            // SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
            //   "[confirm_vlp]:LPSTR counter is empty: {}", TO_STRING(temp_vehs[trk_id]));
            continue;
        }

        SLLPColor_t vlp_color = SL_PLATE_COLOR_UNKNOWN;
        confirm_color(temp_vehs[trk_id].trace_record, vlp, vlp_color);

        VehAccessInfo veh_access_info;
        veh_access_info.trigger_time = oneVeh.veh_timestamp;
        veh_access_info.veh_location = oneVeh.location;
        veh_access_info.vlp_location = oneVeh.lp_info.location;
        veh_access_info.vlp = vlp;
        veh_access_info.vlp_color = vlp_color;
        veh_access_info.vlp_confidence = vlp_confid;
        veh_access_info.time_info = veh_info_vec.time_info;
        veh_access_info.veh_detail_info = oneVeh;

        veh_access_info_vec.push_back(veh_access_info);
        temp_vehs.erase(trk_id);
    }

    return SL_SUCCESS;
}

int
AccessDetector::detect2(VehInfoVec veh_info_vec, Line line1, Line line2,
                        std::vector<VehAccessInfo> &veh_access_info_vec)
{
    veh_access_info_vec.clear();

    for (int i = 0; i < veh_info_vec.veh_info.size(); i++)
    {
        VehInfo oneVeh = veh_info_vec.veh_info[i];
        long trk_id = oneVeh.veh_track_id;
        if (trk_id < 0)
        {
            continue;
        }

        // if(reported_vehs_lp.count(oneVeh->lp_info_ptr->lp_number_ptr->lp_str)
        //   || reported_vehs_trackid.count(trk_id))
        if (reported_vehs_lp.count(oneVeh.lp_info.lp_number.lp_str))
        {
            continue;
        }

        if (!temp_vehs.count(trk_id))
        {
            TempVeh temp_veh_t;
            temp_vehs[trk_id] = temp_veh_t;
        }

        TraceRecord trace;
        trace.vlp_code = oneVeh.lp_info.lp_number.lp_str;
        trace.vlp_color = oneVeh.lp_info.lp_color;
        trace.vlp_confid = oneVeh.lp_info.lp_number.vlp_rec_confidence;
        trace.veh_location = oneVeh.location;
        trace.appear_time = oneVeh.veh_timestamp;
        temp_vehs[trk_id].trace_record.push_back(trace);

        if (temp_vehs[trk_id].trace_record.size() < 10)
        {
            continue;
        }

        bool ret1 = trigger_line(oneVeh.location, line1);
        bool ret2 = trigger_line(oneVeh.location, line2);

        if ((ret1 && ret2) || (ret1 && temp_vehs[trk_id].trigger_bottom_line) ||
            (ret2 && temp_vehs[trk_id].trigger_top_line))
        {
            std::string vlp = "-";
            float vlp_confid = 0.0;
            confirm_vlp(temp_vehs[trk_id].trace_record, vlp, vlp_confid);

            // if(reported_vehs_lp.count(vlp)
            //   || reported_vehs_trackid.count(trk_id)){
            //     continue;
            // }

            if (vlp != "-")
            {
                reported_vehs_lp[vlp] = oneVeh.veh_timestamp;
            }
            else
            {
                // reported_vehs_trackid[trk_id] = oneVeh->veh_timestamp;
                continue;
            }

            SLLPColor_t vlp_color = SL_PLATE_COLOR_UNKNOWN;
            confirm_color(temp_vehs[trk_id].trace_record, vlp, vlp_color);

            VehAccessInfo veh_access_info;
            veh_access_info.trigger_time = oneVeh.veh_timestamp;
            veh_access_info.veh_location = oneVeh.location;
            veh_access_info.vlp_location = oneVeh.lp_info.location;
            veh_access_info.vlp = vlp;
            veh_access_info.vlp_color = vlp_color;
            veh_access_info.vlp_confidence = vlp_confid;
            veh_access_info.time_info = veh_info_vec.time_info;
            veh_access_info.veh_detail_info = oneVeh;
            veh_access_info_vec.push_back(veh_access_info);
            temp_vehs.erase(trk_id);
        }
        else
        {
            if (ret1)
            {
                temp_vehs[trk_id].trigger_top_line = true;
                temp_vehs[trk_id].trigger_first_line_time = oneVeh.veh_timestamp;
            }
            else if (ret2)
            {
                temp_vehs[trk_id].trigger_bottom_line = true;
                temp_vehs[trk_id].trigger_first_line_time = oneVeh.veh_timestamp;
            }
        }
    }

    return SL_SUCCESS;
}

bool
AccessDetector::trigger_single_line(TempVeh temp_veh, Line line)
{
    if (temp_veh.trace_record.size() < 10)
        return false;

    Point2f vect0_f = getVectf(line.start_point_, line.end_point_);

    Rect bbox1 = temp_veh.trace_record.back().veh_location;
    Point point1 = Point(bbox1.x_ + bbox1.width_ / 2, bbox1.y_ + bbox1.height_ / 2);
    Point2f vect1_f = getVectf(line.start_point_, point1);

    Rect bbox2 = temp_veh.trace_record[5].veh_location;
    Point point2 = Point(bbox2.x_ + bbox2.width_ / 2, bbox2.y_ + bbox2.height_ / 2);
    Point2f vect2_f = getVectf(line.start_point_, point2);

    float cross1 = vect0_f.x_ * vect1_f.y_ - vect0_f.y_ * vect1_f.x_;
    float cross2 = vect0_f.x_ * vect2_f.y_ - vect0_f.y_ * vect2_f.x_;
    float res1 = cross1 * cross2;

    Point2f vect0_f1 = getVectf(point1, point2);
    Point2f vect1_f1 = getVectf(point1, line.start_point_);
    Point2f vect2_f1 = getVectf(point1, line.end_point_);
    float cross11 = vect0_f1.x_ * vect1_f1.y_ - vect0_f1.y_ * vect1_f1.x_;
    float cross21 = vect0_f1.x_ * vect2_f1.y_ - vect0_f1.y_ * vect2_f1.x_;
    float res2 = cross11 * cross21;

    if (res1 < 0.0 && res2 < 0.0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
AccessDetector::trigger_line(Rect box, Line line)
{
    cv::Rect rect = cv::Rect(box.x_, box.y_, box.width_, box.height_);
    cv::Point p1 = cv::Point(line.start_point_.x_, line.start_point_.y_);
    cv::Point p2 = cv::Point(line.end_point_.x_, line.end_point_.y_);

    return cv::clipLine(rect, p1, p2);
}

int
AccessDetector::confirm_vlp(std::vector<TraceRecord> trace_record, std::string &vlp, float &vlp_confid)
{
    std::vector<std::pair<std::string, float> > lp_str_vec;
    for (int i = 0; i < trace_record.size(); i++)
    {
        lp_str_vec.push_back(std::make_pair(trace_record[i].vlp_code, trace_record[i].vlp_confid));
    }

    int lp_count_now = 0;
    LpStrCount(lp_str_vec, lp_count_now, vlp_confid, vlp);

    return SL_SUCCESS;
}

int
AccessDetector::confirm_color(std::vector<TraceRecord> trace_record, std::string vlp, SLLPColor_t &vlp_color)
{
    vlp_color = SL_PLATE_COLOR_UNKNOWN;
    if (vlp == "-")
    {
        return SL_SUCCESS;
    }

    std::unordered_map<SLLPColor_t, int> vlp_color_statistic;
    for (int i = 0; i < trace_record.size(); i++)
    {
        if (vlp_color_statistic.count(trace_record[i].vlp_color))
        {
            vlp_color_statistic[trace_record[i].vlp_color] += 1;
        }
        else if (trace_record[i].vlp_color != SL_PLATE_COLOR_UNKNOWN)
        {
            vlp_color_statistic[trace_record[i].vlp_color] = 1;
        }
    }

    int max_cnt = 0;
    for (const auto &kv : vlp_color_statistic)
    {
        if (kv.second > max_cnt)
        {
            vlp_color = kv.first;
            max_cnt = kv.second;
        }
    }

    std::wstring lp_wstr = utf8ToUnicode(vlp);
    if (lp_wstr.size() > 6)
    {
        if (lp_wstr.size() == 8)
        {
            if (vlp_color != SL_PLATE_COLOR_GREENYELLOW || vlp_color != SL_PLATE_COLOR_GREENBLACK)
            {
                vlp_color = SL_PLATE_COLOR_GREENBLACK;
            }
        }
        if (lp_wstr.size() == 7)
        {
            if (vlp.find("学") != std::string::npos || vlp.find("挂") != std::string::npos)
            {
                vlp_color = SL_PLATE_COLOR_YELLOW;
            }
            if (vlp.find("警") != std::string::npos)
            {
                vlp_color = SL_PLATE_COLOR_WHITE;
            }
        }
    }

    return SL_SUCCESS;
}

void
AccessDetector::updateRecord(aih::TimeInfo current_time)
{
    std::vector<int> to_del;
    for (auto &kv : temp_vehs)
    {
        if (kv.second.trace_record.size() == 0)
        {
            to_del.push_back(kv.first);
            continue;
        }

        if (current_time.local_time_/1000 - kv.second.trace_record[0].appear_time > 5 * 1000 || kv.second.trace_record.size() > 100)
        {
            kv.second.trace_record.erase(kv.second.trace_record.begin());
        }

        if (kv.second.trace_record.size() == 0)
        {
            to_del.push_back(kv.first);
        }
    }
    for (auto td : to_del)
    {
        temp_vehs.erase(td);
    }

    std::vector<std::string> to_del2;
    for (auto kv : reported_vehs_lp)
    {
        if (current_time.local_time_/1000 - kv.second > 5 * 60 * 1000)
        {
            to_del2.push_back(kv.first);
        }
    }
    for (auto td2 : to_del2)
    {
        reported_vehs_lp.erase(td2);
    }
}

Point2f
AccessDetector::getVectf(Point start, Point end)
{
    Point2f vect(end.x_ - start.x_, end.y_ - start.y_);
    float vect_norm = sqrt(vect.x_ * vect.x_ + vect.y_ * vect.y_);
    Point2f vect_f(0, 0);
    if (vect_norm != 0.0)
        vect_f = Point2f(vect.x_ / vect_norm, vect.y_ / vect_norm);

    return vect_f;
}

} // namespace sonli
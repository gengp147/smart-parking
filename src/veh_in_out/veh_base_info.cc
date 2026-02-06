#include "veh_base_info.h"

#include "parking_info.h"

#include "../models_processor/char_code.h"
#include "../utils/sl_logger.h"
#include "trend_acc.h"
#include <numeric>

namespace sonli
{

void
VehBaseInfo::PrintCacheVehInfo()
{
    for (int i = 0; i < cache_veh_infos_.veh_cache_infos.size(); i++)
    {
        std::string out = "";
        out =
            out + " camera_channel " + std::to_string(cache_veh_infos_.camera_channel) + " veh_track_id " +
            std::to_string(cache_veh_infos_.veh_cache_infos[i].track_id) + " space_id " +
            std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_infos.back().veh_chassis.space_id) + " space_rat " +
            std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_infos.back().veh_chassis.space_rat) +
            " space_rat_ex " +
            std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_infos.back().veh_chassis.space_rat_ex) +
            " line_angle " +
            std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_infos.back().veh_chassis.line_angle) +
            " space_res " + std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_infos.back().veh_chassis.space_res) +
            " lp_color " + std::to_string(cache_veh_infos_.veh_cache_infos[i].lp_info.lp_color) + " lp_color_count " +
            std::to_string(cache_veh_infos_.veh_cache_infos[i].lp_info.lp_color_count) + " lp_str " +
            cache_veh_infos_.veh_cache_infos[i].lp_info.lp_number.lp_str + " lp_count " +
            std::to_string(cache_veh_infos_.veh_cache_infos[i].lp_info.lp_number.lp_count) + " vlp_rec_confidence " +
            std::to_string(cache_veh_infos_.veh_cache_infos[i].lp_info.lp_number.vlp_rec_confidence) +
            " in_move_count " + std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.in_move_count) +
            " in_count " + std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.in_count) +
            " in_stand_count " + std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.in_stand_count) +
            " is_standstill " + std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_standstill) +
            " no_send_in " + std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.no_send_in) +
            " no_send_in_flag " + std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.no_send_in_flag) +
            " out_move_count " + std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.out_move_count) +
            " out_count " + std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.out_count) +
            " is_report_in " + std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_report_in) +
            " is_report_out " + std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_report_out) +
            " is_disappear " + std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_disappear) +
            " veh_disappear_timestamp " +
            std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.veh_disappear_timestamp) +
            " is_disappear_abnormal " +
            std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_disappear_abnormal) +
            " is_satisfy_in " + std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_satisfy_in) +
            " is_leave " + std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_leave) +
            " is_satisfy_out " + std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_satisfy_out) +
            " is_satisfy_out_flag " +
            std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_satisfy_out_flag) + " is_combine " +
            std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_combine) + " cover_rat " +
            std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_infos.back().cover_rat) + " out_max_iou " +
            std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.out_max_iou) + " veh_inpro_out_mess " +
            std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_inpro_out_mess.veh_pic_id) + " veh_inpro_pro_mess " +
            std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_inpro_pro_mess.veh_pic_id) +
            " veh_standstill_mess " +
            std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_standstill_mess.veh_pic_id) +
            " veh_outpro_std_mess " +
            std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_outpro_std_mess.veh_pic_id) +
            " veh_outpro_pro_mess " +
            std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_outpro_pro_mess.veh_pic_id) +
            " veh_outpro_out_mess " +
            std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_outpro_out_mess.veh_pic_id)
            // + " vehicles.veh_info[i].frame_id " +
            // std::to_string(cache_veh_infos_.veh_cache_infos[i].veh_infos.back().frame_id)
            + " cache_veh_infos_.frame_id " + std::to_string(cache_veh_infos_.frame_id) +
            " cache_veh_infos_.frame_timestamp " + std::to_string(cache_veh_infos_.frame_timestamp) +
            " cache_veh_infos_.camera_channel " + std::to_string(cache_veh_infos_.camera_channel);

        if (cache_veh_infos_.veh_cache_infos[i].veh_inpro_out_mess.veh_pic_id > -1)
        {
            out = out + " veh_inpro_out_mess frameid " +
                  std::to_string(
                      cap_buf_control_
                          .cap_id_frame_id_[cache_veh_infos_.veh_cache_infos[i].veh_inpro_out_mess.veh_pic_id]);
        }
        if (cache_veh_infos_.veh_cache_infos[i].veh_inpro_pro_mess.veh_pic_id > -1)
        {
            out = out + " veh_inpro_pro_mess frameid " +
                  std::to_string(
                      cap_buf_control_
                          .cap_id_frame_id_[cache_veh_infos_.veh_cache_infos[i].veh_inpro_pro_mess.veh_pic_id]);
        }
        if (cache_veh_infos_.veh_cache_infos[i].veh_standstill_mess.veh_pic_id > -1)
        {
            out = out + " veh_standstill_mess frameid " +
                  std::to_string(
                      cap_buf_control_
                          .cap_id_frame_id_[cache_veh_infos_.veh_cache_infos[i].veh_standstill_mess.veh_pic_id]);
        }
        if (cache_veh_infos_.veh_cache_infos[i].veh_outpro_std_mess.veh_pic_id > -1)
        {
            out = out + " veh_outpro_std_mess frameid " +
                  std::to_string(
                      cap_buf_control_
                          .cap_id_frame_id_[cache_veh_infos_.veh_cache_infos[i].veh_outpro_std_mess.veh_pic_id]);
        }
        if (cache_veh_infos_.veh_cache_infos[i].veh_outpro_pro_mess.veh_pic_id > -1)
        {
            out = out + " veh_outpro_pro_mess frameid " +
                  std::to_string(
                      cap_buf_control_
                          .cap_id_frame_id_[cache_veh_infos_.veh_cache_infos[i].veh_outpro_pro_mess.veh_pic_id]);
        }
        if (cache_veh_infos_.veh_cache_infos[i].veh_outpro_out_mess.veh_pic_id > -1)
        {
            out = out + " veh_outpro_out_mess frameid " +
                  std::to_string(
                      cap_buf_control_
                          .cap_id_frame_id_[cache_veh_infos_.veh_cache_infos[i].veh_outpro_out_mess.veh_pic_id]);
        }

        SonLiLogger::getInstance()
            .getLogger(SonLiLogger::DailyLogger)
            ->info("cache_veh_infos_ ---------110---------- {}", out);
    }
}

void
VehBaseInfo::PrintInSpaceVehInfo()
{

    for (int i = 0; i < cache_veh_in_infos_.size(); i++)
    {
        std::string out = "";
        out = out + "size " + std::to_string(cache_veh_in_infos_.size()) + " veh_track_id " +
              std::to_string(cache_veh_in_infos_[i].veh_info.veh_track_id) + " no_send_in " +
              std::to_string(cache_veh_in_infos_[i].no_send_in) + " space_id " +
              std::to_string(cache_veh_in_infos_[i].veh_info.veh_chassis.space_id) + " space_rat " +
              std::to_string(cache_veh_in_infos_[i].veh_info.veh_chassis.space_rat) + " space_res " +
              std::to_string(cache_veh_in_infos_[i].veh_info.veh_chassis.space_res) + " lp_color " +
              std::to_string(cache_veh_in_infos_[i].veh_info.lp_info.lp_color) + " lp_str " +
              cache_veh_in_infos_[i].veh_info.lp_info.lp_number.lp_str + " last_timestamp " +
              std::to_string(cache_veh_in_infos_[i].last_timestamp) + " cha_timestamp " +
              std::to_string(cache_veh_infos_.frame_timestamp - cache_veh_in_infos_[i].last_timestamp);

        SonLiLogger::getInstance()
            .getLogger(SonLiLogger::DailyLogger)
            ->info("cache_veh_in_infos_ ---------109---------- {}", out);
    }
}

void
VehBaseInfo::PrintSendMessage(const VehInOutInfoVec &veh_inout_info_vec)
{
    for (int i = 0; i < veh_inout_info_vec.veh_inout_info.size(); i++)
    {
        std::string out = "";
        out = out + " camera_channel " + std::to_string(veh_inout_info_vec.veh_inout_info[i].camera_channel) +
              " in_out " + std::to_string(veh_inout_info_vec.veh_inout_info[i].in_out) + " veh_track_id " +
              std::to_string(veh_inout_info_vec.veh_inout_info[i].pro_veh_info.veh_track_id) + " space_id " +
              std::to_string(veh_inout_info_vec.veh_inout_info[i].pro_veh_info.veh_chassis.space_id) + " space_rat " +
              std::to_string(veh_inout_info_vec.veh_inout_info[i].pro_veh_info.veh_chassis.space_rat) + " space_res " +
              std::to_string(veh_inout_info_vec.veh_inout_info[i].pro_veh_info.veh_chassis.space_res) + " lp_color " +
              std::to_string(veh_inout_info_vec.veh_inout_info[i].pro_veh_info.lp_info.lp_color) +
              " vlp_det_confidence " +
              std::to_string(veh_inout_info_vec.veh_inout_info[i].pro_veh_info.lp_info.vlp_det_confidence) +
              " vlp_rec_confidence " +
              std::to_string(veh_inout_info_vec.veh_inout_info[i].pro_veh_info.lp_info.lp_number.vlp_rec_confidence) +
              " lp_str " + veh_inout_info_vec.veh_inout_info[i].pro_veh_info.lp_info.lp_number.lp_str + " lp_count " +
              std::to_string(veh_inout_info_vec.veh_inout_info[i].pro_veh_info.lp_info.lp_number.lp_count)

              + " vlp_det_conf_out " +
              std::to_string(veh_inout_info_vec.veh_inout_info[i].out_veh_info.lp_info.vlp_det_confidence) +
              " vlp_rec_conf_out " +
              std::to_string(veh_inout_info_vec.veh_inout_info[i].out_veh_info.lp_info.lp_number.vlp_rec_confidence) +
              " vlp_det_conf_in " +
              std::to_string(veh_inout_info_vec.veh_inout_info[i].in_veh_info.lp_info.vlp_det_confidence) +
              " vlp_rec_conf_in " +
              std::to_string(veh_inout_info_vec.veh_inout_info[i].in_veh_info.lp_info.lp_number.vlp_rec_confidence)

              + " vvv_veh_state_fraction " + std::to_string(veh_inout_info_vec.veh_inout_info[i].veh_state_fraction) +
              " order_generate_type " + std::to_string(veh_inout_info_vec.veh_inout_info[i].order_generate_type);

        SonLiLogger::getInstance()
            .getLogger(SonLiLogger::RotateLogger)
            ->info("202601291716 veh inout info ----------115--------- {} {}", out, stability_enable_);
    }
}

void
VehBaseInfo::PrintVehicles(const VehInfoVec &vehicles)
{
    for (int i = 0; i < vehicles.veh_info.size(); i++)
    {
        std::string out = "";
        out =
            out + " camera_channel " + std::to_string(vehicles.camera_channel) + " veh_track_id " +
            std::to_string(vehicles.veh_info[i].veh_track_id) +
            " space_id: " + std::to_string(vehicles.veh_info[i].veh_chassis.space_id) +
            " space_rat: " + std::to_string(vehicles.veh_info[i].veh_chassis.space_rat) +
            " space_res: " + std::to_string(vehicles.veh_info[i].veh_chassis.space_res) +
            " veh_det_confidence: " + std::to_string(vehicles.veh_info[i].veh_det_confidence) +
            " lp_color: " + std::to_string(vehicles.veh_info[i].lp_info.lp_color) +
            " lp_str: " + vehicles.veh_info[i].lp_info.lp_number.lp_str +
            " vlp_det_confidence: " + std::to_string(vehicles.veh_info[i].lp_info.vlp_det_confidence) +
            " cover_rat: " + std::to_string(vehicles.veh_info[i].cover_rat) +
            " is_image_edge: " + std::to_string(vehicles.veh_info[i].is_image_edge) +
            " vlp_rec_confidence: " + std::to_string(vehicles.veh_info[i].lp_info.lp_number.vlp_rec_confidence) +
            " first_char_confidence: " + std::to_string(vehicles.veh_info[i].lp_info.lp_number.first_char_confidence) +
            " second_char_confidence: " +
            std::to_string(vehicles.veh_info[i].lp_info.lp_number.second_char_confidence) +
            " vlp_rec_min_conf: " + std::to_string(vehicles.veh_info[i].lp_info.lp_number.vlp_rec_min_conf) +
            " line_angle: " + std::to_string(vehicles.veh_info[i].veh_chassis.line_angle) +
            " x_: " + std::to_string(vehicles.veh_info[i].lp_info.location.x_) +
            " y_: " + std::to_string(vehicles.veh_info[i].lp_info.location.y_) +
            " width_: " + std::to_string(vehicles.veh_info[i].lp_info.location.width_) +
            " height_: " + std::to_string(vehicles.veh_info[i].lp_info.location.height_) +
            " veh_x_: " + std::to_string(vehicles.veh_info[i].location.x_) +
            " veh_y_: " + std::to_string(vehicles.veh_info[i].location.y_) +
            " veh_width_: " + std::to_string(vehicles.veh_info[i].location.width_) + " veh_height_: " +
            std::to_string(vehicles.veh_info[i].location.height_)
            // + " is_in_road: " + std::to_string(vehicles.veh_info[i].is_in_road)
            + " veh_color: " + std::to_string(vehicles.veh_info[i].veh_color) +
            " veh_color_conf: " + std::to_string(vehicles.veh_info[i].veh_color_conf) +
            " veh_type: " + std::to_string(vehicles.veh_info[i].veh_type) +
            " veh_type_conf: " + std::to_string(vehicles.veh_info[i].veh_type_conf) +
            " 15_vis_score: " + std::to_string(vehicles.veh_info[i].veh_key_points[14].vis_score) +
            " 16_vis_score: " + std::to_string(vehicles.veh_info[i].veh_key_points[15].vis_score) +
            " frame_timestamp: " + std::to_string(vehicles.frame_timestamp);

        SonLiLogger::getInstance().getLogger(SonLiLogger::DailyLogger)->info("vehicles ---------111---------- {}", out);
    }
}

void
VehBaseInfo::PrintCapCtrlMsg(const CapControlMsgVec &cap_control_msg_vec)
{
    for (int i = 0; i < cap_control_msg_vec.cap_buf_control_vec.size(); i++)
    {
        std::string out = "";
        out = out + " camera_channel " + std::to_string(cache_veh_infos_.camera_channel) + " cap_buff_cmd " +
              std::to_string(cap_control_msg_vec.cap_buf_control_vec[i].cap_buff_cmd) + " pic_id " +
              std::to_string(cap_control_msg_vec.cap_buf_control_vec[i].pic_id) + " frame_id " +
              std::to_string(cap_control_msg_vec.cap_buf_control_vec[i].frame_id);

        SonLiLogger::getInstance()
            .getLogger(SonLiLogger::DailyLogger)
            ->info("cap_control ----------113--------- {}", out);
    }

    SonLiLogger::getInstance()
        .getLogger(SonLiLogger::DailyLogger)
        ->info("cap_buf_control_enable_ids_ ---------114---------- {}",
               cap_buf_control_.cap_buf_control_enable_ids_.size());
}

void
VehBaseInfo::PrintParkingResult(const ParkingResult &parking_result)
{
    for (int i = 0; i < parking_result.parking_num; i++)
    {
        std::string out = "";
        out = out + " camera_channel " + std::to_string(parking_result.camera_channel) + " space_id " +
              std::to_string(i) + " parking_state " + std::to_string(parking_result.parking_mess[i].parking_state);

        for (auto &t : parking_result.parking_mess[i].veh_info)
        {
            out = out + " veh_track_id " + std::to_string(t.second.vehinfo.veh_track_id) + " lp_str " + t.first +
                  " counter " + std::to_string(t.second.counter) + " firstRefreshTime " +
                  std::to_string(t.second.firstRefreshTime) + " nearRefreshTime " +
                  std::to_string(t.second.nearRefreshTime);
        }

        SonLiLogger::getInstance().getLogger(SonLiLogger::DailyLogger)->info("vehicles ---------116---------- {}", out);
    }
}

void
PrintParkingResultWhenChange(ParkingResult parking_result)
{
    std::string summary = {};
    for (int i = 0; i < parking_result.parking_num; i++)
    {
        std::string out = "";
        out = out + "{ ch:[" + std::to_string(parking_result.camera_channel) + "], " + "id:[" + std::to_string(i) +
              "], " + "state:[" + std::to_string(parking_result.parking_mess[i].parking_state) + "] },";
        summary.append(out);
    }
    SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info("parking change:{}", summary);
}

void
VehBaseInfo::PrintLog(const CapControlMsgVec &cap_control_msg_vec, const VehInOutInfoVec &veh_inout_info_vec,
                      const VehInfoVec &vehicles, const ParkingResult &parking_result)
{
    PrintInSpaceVehInfo();
    PrintCacheVehInfo();
    PrintVehicles(vehicles);
    PrintCapCtrlMsg(cap_control_msg_vec);
    PrintParkingResult(parking_result);
}

void
VehBaseInfo::LpColorConfirm(LPInfo &lp_info)
{
    std::string lp_str_now = lp_info.lp_number.lp_str;
    if (lp_str_now == "-")
    {
        return;
    }
    std::wstring lp_wstr = utf8ToUnicode(lp_str_now);
    if (lp_wstr.size() > 6)
    {
        if (lp_wstr.size() == 8)
        {
            if (lp_info.lp_color != SL_PLATE_COLOR_GREENYELLOW && lp_info.lp_color != SL_PLATE_COLOR_GREENBLACK)
            {
                lp_info.lp_color = SL_PLATE_COLOR_GREENBLACK;
            }
        }

        if (lp_wstr.size() == 7)
        {
            if (lp_info.lp_color != SL_PLATE_COLOR_YELLOW)
            {
                lp_info.lp_color = SL_PLATE_COLOR_BLUE;
            }

            if (lp_str_now.find("领") != std::string::npos || lp_str_now.find("使") != std::string::npos)
            {
                lp_info.lp_color = SL_PLATE_COLOR_BLACK;
            }

            if (lp_str_now.find("学") != std::string::npos || lp_str_now.find("挂") != std::string::npos)
            {
                lp_info.lp_color = SL_PLATE_COLOR_YELLOW;
            }

            if (lp_str_now.find("警") != std::string::npos)
            {
                lp_info.lp_color = SL_PLATE_COLOR_WHITE;
            }
        }
    }
}

bool
TwoLpCompare(int lp_count_now, float lp_confidence_now, int lp_count_pre, float lp_confidence_pre, int count_thr = 20)
{
    lp_count_now = std::min(lp_count_now, count_thr);
    lp_count_pre = std::min(lp_count_pre, count_thr);

    float conf_min = std::fmin(lp_confidence_now, lp_confidence_pre);
    float conf_max = std::fmax(lp_confidence_now, lp_confidence_pre);
    float conf_thr = conf_min - 0.01;
    if (conf_max < 0.95)
    {
        conf_thr -= 0.01;
    }
    if (conf_max < 0.9)
    {
        conf_thr -= 0.01;
    }

    lp_confidence_now -= conf_thr;
    lp_confidence_pre -= conf_thr;

    if (lp_count_now * lp_confidence_now >= lp_count_pre * lp_confidence_pre)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void
VehBaseInfo::VehLpNumberCount(int ind, std::vector<std::pair<std::string, float>> &lp_str_vec)
{
    if (lp_str_vec.size() > 0)
    {
        int lp_count_now = 0;
        float lp_confidence_now = 0.0;
        std::string lp_str_now = "-";
        LpStrCount(lp_str_vec, lp_count_now, lp_confidence_now, lp_str_now);

        int lp_count_pre = cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_count;
        float lp_confidence_pre = cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.vlp_rec_confidence;
        std::string lp_str_pre = cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str;

        float min_lp_conf = std::fmin(lp_confidence_pre, 0.8);
        if (lp_str_now != "-" && lp_confidence_now > min_lp_conf)
        {
            std::wstring lp_strw_now = utf8ToUnicode(lp_str_now);
            std::wstring lp_strw_pre = utf8ToUnicode(lp_str_pre);
            int dis = wstr_distance(lp_strw_now, lp_strw_pre);

            bool lp_comp = TwoLpCompare(lp_count_now, lp_confidence_now, lp_count_pre, lp_confidence_pre);

            // SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
            // 				"lp_str_now {} lp_count_now {} lp_confidence_now {}; lp_str_pre {} lp_count_pre
            // {} lp_confidence_pre {} lp_comp {}!", 				lp_str_now, 				lp_count_now, 				lp_confidence_now, 				lp_str_pre,
            // 				lp_count_pre,
            // 				lp_confidence_pre,
            // 				lp_comp);

            if (dis == 0)
            {
                if (lp_count_pre >= lp_count_now)
                {
                    lp_count_now = lp_count_pre;
                }
                if (lp_confidence_pre > lp_confidence_now && lp_count_pre >= 10)
                {
                    lp_confidence_now = lp_confidence_pre;
                }
            }
            else if (dis == 1)
            {
                if (lp_strw_pre.size() == 8 && lp_strw_now.size() == 7 && lp_confidence_pre > 0.93 && lp_count_pre > 10)
                {
                    lp_str_now = lp_str_pre;
                    lp_count_now = lp_count_pre;
                    lp_confidence_now = lp_confidence_pre;
                }
                else
                {
                    if (!lp_comp)
                    {
                        lp_str_now = lp_str_pre;
                        lp_count_now = lp_count_pre;
                        lp_confidence_now = lp_confidence_pre;
                    }
                }
            }
            else
            {
                if (lp_count_now >= 10 && lp_confidence_now > 0.98)
                {
                }
                else
                {
                    if (!lp_comp)
                    {
                        lp_str_now = lp_str_pre;
                        lp_count_now = lp_count_pre;
                        lp_confidence_now = lp_confidence_pre;
                    }
                }
            }

            cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_count = lp_count_now;
            cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str = lp_str_now;

            cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.vlp_rec_confidence = lp_confidence_now;
            cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.first_char_confidence = lp_confidence_now;
            cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.second_char_confidence = lp_confidence_now;
        }
    }
}

void
VehBaseInfo::VehLpColorCount(int ind, std::vector<SLLPColor_t> &lp_color_vec)
{

    if (lp_color_vec.size() >= 2)
    {
        std::map<SLLPColor_t, int> lp_color_count;
        SLLPColor_t max_fre_color = lp_color_vec[0];
        int max_count = 1;
        for (int i = 0; i < lp_color_vec.size(); i++)
        {
            if (lp_color_count.find(lp_color_vec[i]) != lp_color_count.end())
            {
                lp_color_count[lp_color_vec[i]]++;
                if (lp_color_count[lp_color_vec[i]] > max_count)
                {
                    max_count = lp_color_count[lp_color_vec[i]];
                    max_fre_color = lp_color_vec[i];
                }
            }
            else
            {
                lp_color_count[lp_color_vec[i]] = 1;
            }
        }

        int lp_color_count_now = max_count;
        SLLPColor_t lp_color_now = max_fre_color;

        int lp_color_count_pre = cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_color_count;
        SLLPColor_t lp_color_pre = cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_color;

        if (lp_color_pre == lp_color_count_now)
        {
            if (lp_color_count_now < lp_color_count_pre)
            {
                lp_color_count_now = lp_color_count_pre;
            }
        }
        else
        {
            int temp = lp_color_count_pre / 2;
            if (lp_color_count_now > temp || lp_color_count_now > 6)
            {
                if (lp_color_count_pre > 6)
                {
                    lp_color_count_now = lp_color_count_pre;
                    lp_color_now = lp_color_pre;
                }
            }
            else
            {
                lp_color_count_now = lp_color_count_pre;
                lp_color_now = lp_color_pre;
            }
        }

        cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_color = lp_color_now;
        cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_color_count = lp_color_count_now;
    }
}

bool
VehBaseInfo::IsLpNumberOkay(int lp_count, float lp_confidence)
{
    if ((lp_confidence > 0.97 && lp_count > 10) || (lp_count > 20 && lp_confidence > 0.945))
    {
        return true;
    }
    else
    {
        return false;
    }
}

std::vector<std::pair<std::string, float>>
VehBaseInfo::GetVehLpStrVec(const std::vector<VehInfo> &veh_infos, std::vector<SLLPColor_t> &lp_color_vec)
{
    int veh_infos_size = veh_infos.size();
    std::vector<std::pair<std::string, float>> lp_str_vec;
    int have_used = 0;
    for (int i = 0; i < veh_infos_size; i++)
    {
        std::string lpStr = veh_infos[veh_infos_size - 1 - i].lp_info.lp_number.lp_str;
        if (lpStr != "-")
        {
            have_used++;
            float conf_lp = veh_infos[veh_infos_size - 1 - i].lp_info.lp_number.vlp_rec_confidence;
            lp_str_vec.push_back(std::make_pair(lpStr, conf_lp));
            lp_color_vec.push_back(veh_infos[veh_infos_size - 1 - i].lp_info.lp_color);
        }
        if (have_used >= LP_CACHE_LENGTH)
        {
            break;
        }
    }
    return lp_str_vec;
}

bool
VehBaseInfo::VehLpNumberStatus(int ind)
{
    // std::vector<VehInfo> veh_infos = cache_veh_infos_.veh_cache_infos[ind].veh_infos;
    // std::vector<std::pair<std::string, float>> lp_str_vec = cache_veh_infos_.veh_cache_infos[ind].lp_str_vec;
    // std::vector<SLLPColor_t> lp_color_vec = cache_veh_infos_.veh_cache_infos[ind].lp_color_vec;
    if (cache_veh_infos_.veh_cache_infos[ind].lp_str_vec.size() < 1 ||
        cache_veh_infos_.veh_cache_infos[ind].no_lp_count > 0)
    {
        return false;
    }

    VehLpNumberCount(ind, cache_veh_infos_.veh_cache_infos[ind].lp_str_vec);
    VehLpColorCount(ind, cache_veh_infos_.veh_cache_infos[ind].lp_color_vec);

    if (cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str != "-")
    {
        cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.vlp_status = PLATE_NUMBER_CORRECT;
    }

    return IsLpNumberOkay(cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_count,
                          cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.vlp_rec_confidence);
}

bool
VehBaseInfo::JudgeImageTime(int ind, int inout)
{
    if (inout == 1)
    {
        bool veh_intime_status =
            time_compare(cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.veh_timestamp,
                         cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_timestamp, 420);
        if (veh_intime_status)
        {
            return false;
        }
        veh_intime_status =
            time_compare(cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.veh_timestamp,
                         cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_timestamp, 1200);
        if (veh_intime_status)
        {
            return false;
        }
    }
    else
    {
    }
    return true;
}

// loc_id 0:first  1: stand  2: now
int
VehBaseInfo::IsHaveSameLocVeh(int ind, float &max_iou_rat, std::vector<std::pair<float, int>> &iou_rat_vec, int loc_id,
                              bool except_dis)
{
    int same_loc_veh_i = -1;
    Rect box_ind;
    if (cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_locs.size() > 0)
    {
        box_ind = cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_locs[0];
    }
    else
    {
        return same_loc_veh_i;
    }

    for (int i = 0; i < cache_veh_infos_.veh_cache_infos.size(); i++)
    {
        int veh_infos_size = cache_veh_infos_.veh_cache_infos[i].veh_infos.size();
        if (i == ind || veh_infos_size < 1 || cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.to_delete)
        {
            continue;
        }

        if (except_dis &&
            (cache_veh_infos_.veh_cache_infos[i].veh_infos.back().veh_timestamp < cache_veh_infos_.frame_timestamp ||
             cache_veh_infos_.veh_cache_infos[i].veh_infos.back().veh_det_confidence < 0.85))
        {
            continue;
        }

        Rect box_i = cache_veh_infos_.veh_cache_infos[i]
                         .veh_infos[cache_veh_infos_.veh_cache_infos[i].veh_infos.size() - 1]
                         .location;

        if (loc_id == 1 && cache_veh_infos_.veh_cache_infos[i].veh_standstill_mess.veh_locs.size() > 0)
        {
            box_i = cache_veh_infos_.veh_cache_infos[i].veh_standstill_mess.veh_locs[0];
        }

        if (loc_id == 0)
        {
            if (cache_veh_infos_.veh_cache_infos[i].veh_locs_first_n.size() < 1)
            {
                if (cache_veh_infos_.veh_cache_infos[i].veh_infos_first_n.size() > 0)
                {
                    box_i = cache_veh_infos_.veh_cache_infos[i].veh_infos_first_n[0].location;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                box_i = cache_veh_infos_.veh_cache_infos[i].veh_locs_first_n[0];
            }
        }

        float pIOU = 0.0;
        float carRat1 = 0.0;
        float carRat2 = 0.0;

        pIOU = cal_iou(box_ind, box_i, carRat1, carRat2);

        iou_rat_vec.push_back(std::make_pair(pIOU, i));

        if (pIOU > max_iou_rat)
        {
            max_iou_rat = pIOU;
            same_loc_veh_i = i;
        }
    }
    return same_loc_veh_i;
}

int
VehBaseInfo::IsHaveSend(int ind)
{
    int same_lp_veh_i = -1;
    std::string lp_str_ind = cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.lp_str;
    if (lp_str_ind == "-")
    {
        return same_lp_veh_i;
    }
    for (int i = 0; i < cache_veh_in_infos_.size(); i++)
    {

        std::string lp_str_i = cache_veh_in_infos_[i].veh_info.lp_info.lp_number.lp_str;
        if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_move_count < 1)
        {
            if (lp_str_i == lp_str_ind)
            {
                bool veh_dis_time_status =
                    time_compare(cache_veh_in_infos_[i].last_timestamp, cache_veh_infos_.frame_timestamp, 60);
                if (!veh_dis_time_status)
                {
                    same_lp_veh_i = i;
                    return same_lp_veh_i;
                }
            }
        }
        else
        {
            if (!cache_veh_in_infos_[i].no_send_in && lp_str_i == lp_str_ind)
            {

                bool veh_dis_time_status =
                    time_compare(cache_veh_in_infos_[i].report_in_timestamp, cache_veh_infos_.frame_timestamp, 60);
                if (!veh_dis_time_status)
                {
                    same_lp_veh_i = i;
                    return same_lp_veh_i;
                }
            }
        }
    }
    return same_lp_veh_i;
}

int
VehBaseInfo::IsHaveSameLpVehIn(int ind, std::string lp_str_ind, int in_out)
{
    int same_lp_veh_i = -1;
    if (lp_str_ind == "-")
    {
        return same_lp_veh_i;
    }

    for (int i = 0; i < cache_veh_infos_.veh_cache_infos.size(); i++)
    {
        if (cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_disappear || i == ind ||
            cache_veh_infos_.veh_cache_infos[i].veh_infos.size() < 5)
        {
            continue;
        }

        if (cache_veh_infos_.frame_timestamp - cache_veh_infos_.veh_cache_infos[i].veh_infos.back().veh_timestamp > 500)
        {
            continue;
        }

        int veh_infos_size = cache_veh_infos_.veh_cache_infos[i].veh_infos.size();
        int use_size = std::min(veh_infos_size, 10);
        int lp_count = 0;
        for (int j = 0; j < use_size; j++)
        {
            std::string lpStr =
                cache_veh_infos_.veh_cache_infos[i].veh_infos[veh_infos_size - 1 - j].lp_info.lp_number.lp_str;
            float vlp_rec_confidence = cache_veh_infos_.veh_cache_infos[i]
                                           .veh_infos[veh_infos_size - 1 - j]
                                           .lp_info.lp_number.vlp_rec_confidence;
            if (in_out == 1)
            {
                // cache_veh_infos_.veh_cache_infos[i].veh_infos[veh_infos_size - 1 - j].veh_chassis.space_res ==
                // SPACE_OUT &&
                if (cache_veh_infos_.veh_cache_infos[i].veh_infos[veh_infos_size - 1 - j].cover_rat < 0.35)
                {
                    // && vlp_rec_confidence > 0.9
                    if (lpStr == lp_str_ind)
                    {
                        lp_count++;
                    }
                }
            }
            else
            {
                // && cache_veh_infos_.veh_cache_infos[i].veh_infos[veh_infos_size - 1 - j].cover_rat < 0.35
                if (cache_veh_infos_.veh_cache_infos[i].veh_infos[veh_infos_size - 1 - j].veh_chassis.space_res !=
                    SPACE_OUT)
                {
                    // && vlp_rec_confidence > 0.9
                    if (lpStr == lp_str_ind)
                    {
                        lp_count++;
                    }
                }
            }
        }
        if (lp_count > 2)
        {
            same_lp_veh_i = i;
            return same_lp_veh_i;
        }
    }

    return same_lp_veh_i;
}

bool
VehBaseInfo::IsSatisfyInSuspected(int ind)
{
    if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear)
    {
        bool is_have_same_loc_veh = false;
        bool is_cover = false;

        if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear_abnormal &&
            // cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_pic_id >= 0 &&
            cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str != "-" &&
            cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_count > 2 &&
            cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.vlp_rec_confidence > 0.93 &&
            cache_veh_infos_.veh_cache_infos[ind]
                    .veh_infos[cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() - 1]
                    .veh_chassis.space_res_ex == SPACE_IN)
        {
            if (cache_veh_infos_.veh_cache_infos[ind]
                    .veh_infos[cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() - 1]
                    .cover_rat > 0.5)
            {
                is_cover = true;
            }

            if (!is_cover)
            {
                std::vector<std::pair<float, int>> iou_rat_vec;
                float max_iou_rat = 0.0;
                int same_loc_veh_i = IsHaveSameLocVeh(ind, max_iou_rat, iou_rat_vec, 1, true);
                same_loc_veh_i = IsHaveSameLocVeh(ind, max_iou_rat, iou_rat_vec, 2, true);
                std::sort(iou_rat_vec.begin(), iou_rat_vec.end(), cmp_first);

                if (max_iou_rat > 0.6)
                {
                    is_have_same_loc_veh = true;
                }
            }
        }

        if (is_have_same_loc_veh || is_cover)
        {
            return true;
        }
    }
    else
    {
    }
    return false;
}

bool
VehBaseInfo::IsSatisfyIn(int ind)
{
    bool is_dis = cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear;

    if (is_dis)
    {

        // 消失
        bool is_satisfy_space_rat = false;
        bool is_have_same_loc_veh = false;
        bool is_cover = false;
        bool same_spaceid = false;
        if ( // cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_count > 10 &&
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_move_count > 0 &&
            cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_pic_id >= 0 &&
            cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_pic_id >= 0 &&
            cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str != "-" &&
            cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_count > 2 &&
            cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.vlp_rec_confidence > 0.93)
        {
            float space_rat = 0.0;
            int veh_infos_size = cache_veh_infos_.veh_cache_infos[ind].veh_infos.size();
            int min_size = 5;
            veh_infos_size = std::min(veh_infos_size, min_size);
            if (veh_infos_size >= min_size)
            {
                for (int i = 0; i < veh_infos_size; i++)
                {
                    space_rat += cache_veh_infos_.veh_cache_infos[ind]
                                     .veh_infos[cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() - 1 - i]
                                     .veh_chassis.space_rat;
                }
                space_rat = space_rat / veh_infos_size;
                if (space_rat > 0.45 && cache_veh_infos_.veh_cache_infos[ind]
                                                .veh_infos[cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() - 1]
                                                .veh_chassis.space_res == SPACE_IN)
                {
                    is_satisfy_space_rat = true;
                }
            }

            // 连续多帧在同一车位
            int same_spaceid_count = 5;
            int sapceID_vec_size = cache_veh_infos_.veh_cache_infos[ind].sapceID_vec.size();
            int spaceId = cache_veh_infos_.veh_cache_infos[ind]
                              .veh_infos[cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() - 1]
                              .veh_chassis.space_id;
            if (spaceId >= 0 && is_satisfy_space_rat && sapceID_vec_size > same_spaceid_count)
            {
                same_spaceid = true;
                for (int i = 1; i < same_spaceid_count; i++)
                {
                    if (cache_veh_infos_.veh_cache_infos[ind].sapceID_vec[sapceID_vec_size - 1 - i] != spaceId)
                    {
                        same_spaceid = false;
                        break;
                    }
                }
            }

            // if (cache_veh_infos_.veh_cache_infos[ind]
            // 		.veh_infos[cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() - 1]
            // 		.cover_rat > 0.5)
            // {
            // 	is_cover = true;
            // }

            // if (!is_cover && same_spaceid && is_satisfy_space_rat) {
            // 	std::vector<std::pair<float, int>> iou_rat_vec;
            // 	float max_iou_rat = 0.0;
            // 	int same_loc_veh_i = IsHaveSameLocVeh(ind, max_iou_rat, iou_rat_vec, 1, true);
            // 	same_loc_veh_i = IsHaveSameLocVeh(ind, max_iou_rat, iou_rat_vec, 2, true);
            // 	std::sort(iou_rat_vec.begin(), iou_rat_vec.end(), cmp_first);

            // 	if (max_iou_rat > 0.6) {
            // 		is_have_same_loc_veh = true;
            // 	}
            // }
        }

        // && (is_have_same_loc_veh || is_cover)
        if (same_spaceid && is_satisfy_space_rat)
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_in = 1;
            return true;
        }
        else
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_in = 0;
        }
    }
    else
    {
        // 未消失
        if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_count > 60 &&
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_stand_count > 5 &&
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_move_count < 1)
        {

            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in = true;
            return true;
        }

        if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_count > 60)
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in = false;
            return true;
        }
    }

    return false;
}

void
VehBaseInfo::GetOutVehMaxIou(int ind)
{
    std::vector<std::pair<float, int>> iou_rat_vec;
    float max_iou_rat = 0.0;
    int same_loc_veh_i = IsHaveSameLocVeh(ind, max_iou_rat, iou_rat_vec, 2, true);
    // same_loc_veh_i = IsHaveSameLocVeh(ind, max_iou_rat, iou_rat_vec, 1, true);
    std::sort(iou_rat_vec.begin(), iou_rat_vec.end(), cmp_first);
    if (iou_rat_vec.size() > 0)
    {
        max_iou_rat = iou_rat_vec[0].first;
    }

    if (max_iou_rat > 0.3)
    {
        max_iou_rat = 0.25;
        for (int i = 0; i < iou_rat_vec.size(); i++)
        {
            int same_loc_veh_i_t = iou_rat_vec[i].second;
            float max_iou_rat_t = iou_rat_vec[i].first;
            if (max_iou_rat_t > 0.3)
            {
                if (cache_veh_infos_.use_reid && cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear &&
                    cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_locs.size() > 0 &&
                    !cache_veh_infos_.veh_cache_infos[same_loc_veh_i_t].veh_in_out_var.is_disappear)
                {
                    cv::Mat feature_stand_ind =
                        cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.feature;
                    cv::Mat feature_a = cache_veh_infos_.veh_cache_infos[same_loc_veh_i_t].veh_infos.back().feature;
                    cv::Mat AB = feature_a * feature_stand_ind.t();
                    float feature_cos = AB.at<float>(0, 0);
                    if (feature_cos > 0.95)
                    {
                        max_iou_rat = 1.1;
                        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_max_iou = max_iou_rat;
                        continue;
                    }
                }

                if (cache_veh_infos_.veh_cache_infos[same_loc_veh_i_t].veh_in_out_var.in_move_count < 2)
                {
                    // 未消失、未移动 判断入画面位置
                    Rect box_ind;
                    if (cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_locs.size() > 0)
                    {
                        box_ind = cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_locs[0];
                    }
                    else
                    {
                        max_iou_rat = 1.5;
                        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_max_iou = max_iou_rat;
                        break;
                    }

                    Rect box_i = cache_veh_infos_.veh_cache_infos[same_loc_veh_i_t].veh_infos.back().location;

                    if (cache_veh_infos_.veh_cache_infos[same_loc_veh_i_t].veh_locs_first_n.size() > 0)
                    {
                        box_i = cache_veh_infos_.veh_cache_infos[same_loc_veh_i_t].veh_locs_first_n[0];
                    }

                    if (cache_veh_infos_.veh_cache_infos[same_loc_veh_i_t].veh_standstill_mess.veh_locs.size() > 0)
                    {
                        box_i = cache_veh_infos_.veh_cache_infos[same_loc_veh_i_t].veh_standstill_mess.veh_locs[0];
                    }

                    float pIOU = 0.0;
                    float carRat1 = 0.0;
                    float carRat2 = 0.0;
                    pIOU = cal_iou(box_ind, box_i, carRat1, carRat2);

                    if (pIOU > 0.33 || carRat1 > 0.33 || carRat2 > 0.33)
                    {
                        max_iou_rat = max_iou_rat_t;
                        same_loc_veh_i = same_loc_veh_i_t;
                        break;
                    }
                }
            }
            else
            {
                break;
            }
        }
        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_max_iou = max_iou_rat;
    }
    else
    {
        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_max_iou = max_iou_rat;
    }
}

bool
VehBaseInfo::IsSatisfyOut(int ind)
{

    if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() < 1)
    {
        return false;
    }

    bool is_satisfy_out = false;

    if (!cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear)
    {
        // 车辆未消失
        if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_count < 1)
        {
            if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_move_count <= out_move_count_conf_)
            {
                return false;
            }
        }
        else
        {
            if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_count < out_count_conf_)
            {
                return false;
            }
        }
        return true;
    }
    else
    {
        // 车辆消失

        if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_res != SPACE_OUT)
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_count = 0;
        }

        GetOutVehMaxIou(ind);
        // if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_max_iou > 0.8) {
        // 	is_satisfy_out = false;
        // 	cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_out = 0;
        // 	return is_satisfy_out;
        // }

        std::string lp_str_now = cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str;

        if (lp_str_now == "-")
        {
            // && cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().cover_rat< 0.25
            if (!cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear_abnormal &&
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_max_iou < 0.35)
            {

                if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_move_count > 10 &&
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_count > 5)
                {
                    is_satisfy_out = true;
                }
            }
        }
        else
        {
            if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_max_iou < 0.6)
            {

                if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear_abnormal &&
                    cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_rat > 0.1)
                {

                    // 异常消失
                    if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_max_iou < 0.2)
                    {

                        if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_rat < 0.2)
                        {
                            // 	if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_move_count > 5) {
                            // 		is_satisfy_out = true;
                            // 	}
                            // }
                            // else {
                            if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_move_count > 5 &&
                                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_max_iou < 0.05 &&
                                cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_count > 5 &&
                                cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.vlp_rec_confidence > 0.97)
                            {
                                int use_size = 10;
                                int veh_infos_size = cache_veh_infos_.veh_cache_infos[ind].veh_infos.size();
                                bool is_cover = false;
                                if (veh_infos_size > 10)
                                {
                                    for (int i = 0; i < use_size; i++)
                                    {
                                        if (cache_veh_infos_.veh_cache_infos[ind]
                                                .veh_infos[veh_infos_size - 1 - i]
                                                .cover_rat > 0.05)
                                        {
                                            is_cover = true;
                                            break;
                                        }
                                    }
                                }
                                if (!is_cover)
                                {
                                    is_satisfy_out = true;
                                }
                            }
                        }
                    }
                }
                else
                {
                    if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_res != SPACE_OUT)
                    {
                        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_count = 0;
                    }

                    if (!cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear_abnormal)
                    {
                        if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_move_count > 1 &&
                            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_count > 1)
                        {
                            is_satisfy_out = true;
                        }
                    }
                    else
                    {
                        if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_max_iou < 0.2)
                        {
                            // if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_move_count > 5) {
                            // 	is_satisfy_out = true;
                            // }
                            if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_move_count > 1 &&
                                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_count > 1)
                            {
                                is_satisfy_out = true;
                            }
                        }
                        else
                        {
                            // if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_move_count > 10) {
                            // 	is_satisfy_out = true;
                            // }
                            if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_move_count > 5 &&
                                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_count > 2)
                            {
                                is_satisfy_out = true;
                            }
                        }
                    }
                }
            }

            if (!is_satisfy_out && ((!cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear_abnormal &&
                                     cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_count > 5) ||
                                    (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear_abnormal &&
                                     cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_count > 100)))
            {
                std::string lp_str_out = "-";
                int lp_count_out = 0;
                float lp_confidence_out = 0.0;

                std::vector<SLLPColor_t> lp_color_vec;
                std::vector<std::pair<std::string, float>> lp_str_vec =
                    GetVehLpStrVec(cache_veh_infos_.veh_cache_infos[ind].veh_infos, lp_color_vec);
                LpStrCount(lp_str_vec, lp_count_out, lp_confidence_out, lp_str_out);

                if (lp_count_out > 10 && lp_str_out != "-")
                {
                    bool have_in_lp = false;
                    for (int i = 0; i < cache_veh_in_infos_.size(); i++)
                    {
                        if (!cache_veh_in_infos_[i].no_send_in &&
                            cache_veh_in_infos_[i].veh_info.lp_info.lp_number.lp_str == lp_str_out)
                        {
                            have_in_lp = true;
                            break;
                        }
                    }
                    if (have_in_lp)
                    {
                        is_satisfy_out = true;
                    }
                }
            }
        }

        if (is_satisfy_out)
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_out = 1;
        }
        else
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_out = 0;
        }
    }
    return is_satisfy_out;
}

void
VehBaseInfo::UpdateVehStandstillLoc(const std::vector<VehInfo> &veh_infos, int ind)
{
    if (veh_infos.size() > 10)
    {
        Rect veh_standstill_loc_avg;
        veh_standstill_loc_avg.x_ = 0;
        veh_standstill_loc_avg.y_ = 0;
        veh_standstill_loc_avg.width_ = 0;
        veh_standstill_loc_avg.height_ = 0;
        int veh_standstill_loc_count = 10;
        float space_rat_avg = 0.0;
        int line_angle_avg = 0;

        for (int i = 0; i < veh_standstill_loc_count; i++)
        {
            Rect veh_standstill_loc = veh_infos[veh_infos.size() - 1 - i].location;
            veh_standstill_loc_avg.x_ += veh_standstill_loc.x_;
            veh_standstill_loc_avg.y_ += veh_standstill_loc.y_;
            veh_standstill_loc_avg.width_ += veh_standstill_loc.width_;
            veh_standstill_loc_avg.height_ += veh_standstill_loc.height_;

            space_rat_avg += veh_infos[veh_infos.size() - 1 - i].veh_chassis.space_rat;

            line_angle_avg += veh_infos[veh_infos.size() - 1 - i].veh_chassis.line_angle;
        }

        line_angle_avg = line_angle_avg / veh_standstill_loc_count;
        cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_chassis.line_angle = line_angle_avg;

        veh_standstill_loc_avg.x_ = veh_standstill_loc_avg.x_ / veh_standstill_loc_count;
        veh_standstill_loc_avg.y_ = veh_standstill_loc_avg.y_ / veh_standstill_loc_count;
        veh_standstill_loc_avg.width_ = veh_standstill_loc_avg.width_ / veh_standstill_loc_count;
        veh_standstill_loc_avg.height_ = veh_standstill_loc_avg.height_ / veh_standstill_loc_count;

        cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_locs.clear();
        cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_locs.push_back(veh_standstill_loc_avg);

        space_rat_avg = space_rat_avg / veh_standstill_loc_count;
        cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_chassis.space_rat = space_rat_avg;
    }
}

// 判断车辆进场状态  1.车辆停在泊位内，2.车辆在移动，3.车辆驶离泊位
int
VehBaseInfo::VehInStatus(int ind)
{

    // std::vector<VehInfo> veh_infos = cache_veh_infos_.veh_cache_infos[ind].veh_infos;
    if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() < 1 || space_info_.space_points.size() < 1)
    {
        return 0;
    }

    int first_n_count_conf = 5;
    if (cache_veh_infos_.veh_cache_infos[ind].veh_locs_first_n.size() < 1)
    {
        if (cache_veh_infos_.veh_cache_infos[ind].veh_infos_first_n.size() > first_n_count_conf)
        {
            Rect veh_loc_avg;
            veh_loc_avg.x_ = 0;
            veh_loc_avg.y_ = 0;
            veh_loc_avg.width_ = 0;
            veh_loc_avg.height_ = 0;

            for (int i = 0; i < first_n_count_conf; i++)
            {
                Rect veh_loc = cache_veh_infos_.veh_cache_infos[ind].veh_infos_first_n[i].location;
                veh_loc_avg.x_ += veh_loc.x_;
                veh_loc_avg.y_ += veh_loc.y_;
                veh_loc_avg.width_ += veh_loc.width_;
                veh_loc_avg.height_ += veh_loc.height_;
            }

            veh_loc_avg.x_ = veh_loc_avg.x_ / first_n_count_conf;
            veh_loc_avg.y_ = veh_loc_avg.y_ / first_n_count_conf;
            veh_loc_avg.width_ = veh_loc_avg.width_ / first_n_count_conf;
            veh_loc_avg.height_ = veh_loc_avg.height_ / first_n_count_conf;

            cache_veh_infos_.veh_cache_infos[ind].veh_locs_first_n.clear();
            cache_veh_infos_.veh_cache_infos[ind].veh_locs_first_n.push_back(veh_loc_avg);
        }
    }

    int box_size = 24;
    std::vector<Rect> boxes(box_size);
    int veh_size = cache_veh_infos_.veh_cache_infos[ind].veh_infos.size();
    if (veh_size % 2 == 0 && veh_size > box_size)
    {
        for (int i = 0; i < veh_size; i++)
        {
            boxes[i] = cache_veh_infos_.veh_cache_infos[ind].veh_infos[veh_size - 1 - i].location;
            if (i == box_size - 1)
            {
                break;
            }
        }

        bool is_Move_In = is_move_in_loc(boxes, cache_veh_infos_.veh_cache_infos[ind].veh_locs_first_n);
        if (is_Move_In)
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_move_count++;
        }
    }
    else
    {
        box_size = 0;
    }

    // 当前车辆是否停下
    bool isstand = false;
    if (box_size > 20)
    {
        isstand = is_stand(boxes);
    }
    else
    {
        if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_stand_count > 0)
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_stand_count++;
            if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.line_angle > -1 &&
                cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.line_angle < 30)
            {
                return 1;
            }
            else
            {
                return 2;
            }
        }
    }

    if (isstand)
    {
        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_stand_count++;
        if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.line_angle > -1 &&
            cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.line_angle < 30)
        {
            return 1;
        }
        else
        {
            return 2;
        }
    }
    else
    {
        if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_stand_count > 0)
        {
            float piou_rat = 0.6;
            if (box_size > 20)
            {
                bool isMoveOut = is_move_out(boxes, piou_rat, 0);
                if (isMoveOut)
                {
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_stand_count = 0;
                }
                else
                {
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_stand_count++;
                }
            }
            else
            {
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_stand_count = 0;
            }
        }
    }

    if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_stand_count == 0)
    {

        // 连续多帧在同一车位
        bool same_spaceid = true;
        int same_spaceid_count = 40;
        int sapceID_vec_size = cache_veh_infos_.veh_cache_infos[ind].sapceID_vec.size();
        if (sapceID_vec_size > same_spaceid_count)
        {
            int spaceId = cache_veh_infos_.veh_cache_infos[ind].sapceID_vec[sapceID_vec_size - 1];
            if (spaceId < 0)
            {
                return 0;
            }
            for (int i = 1; i < same_spaceid_count; i++)
            {
                if (cache_veh_infos_.veh_cache_infos[ind].sapceID_vec[sapceID_vec_size - 1 - i] != spaceId)
                {
                    same_spaceid = false;
                    break;
                }
            }
        }
        else
        {
            same_spaceid = false;
        }
        if (same_spaceid && cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.line_angle > -1 &&
            cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.line_angle < 30)
        {
            return 2;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.line_angle > -1 &&
            cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.line_angle < 30)
        {
            return 1;
        }
        else
        {
            return 2;
        }
    }
}

// 判断车辆出场状态
bool
VehBaseInfo::VehOutStatus(int ind)
{
    // std::vector<VehInfo> veh_infos = cache_veh_infos_.veh_cache_infos[ind].veh_infos;
    if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() < 1)
    {
        return false;
    }

    if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_rat < 0.05 ||
        (cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().is_image_edge &&
         cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_rat < 0.25))
    {
        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_count++;
    }
    // 		else if(veh_infos[veh_infos.size() - 1].veh_chassis.space_rat < 0.25){

    // 			if(cache_veh_infos_.veh_cache_infos[ind].space_rate_list->size() > 150)
    // 			{
    // 				auto space_occupies_rate_data = cache_veh_infos_.veh_cache_infos[ind]
    // 						.space_rate_list->readFirstNElements(150);

    // //				cache_veh_infos_->veh_cache_infos[ind]->space_rate_list->print();
    // 				auto moved_data = basic_algorithm::MovingAverage(space_occupies_rate_data, 8);
    // 				auto slope_intercept = basic_algorithm::LinearRegression(moved_data,0,
    // moved_data.size()); 				auto r2 = basic_algorithm::CalculateRSquared(moved_data,slope_intercept.first,
    // slope_intercept.second,0, moved_data.size()); 				auto trend = basic_algorithm::DetermineTrend(slope_intercept.first,
    // r2, 0.75);
    // //				std::cout << "trend1:<" << (int)trend.first<< "," << trend.second<<">\t" << r2
    // << "\t" << std::endl; 				if(trend.first == basic_algorithm::Trend::DECREASING)
    // 				{
    // 					cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_count++;
    // 				}
    // 				else if(trend.first == basic_algorithm::Trend::INCREASING)
    // 				{
    // 					cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_count = 0;
    // 				}
    // 			}
    // 			else
    // 			{
    // 				cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_count = 0;
    // 			}
    // 		}
    else
    {
        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_count = 0;
    }

    int box_size = 30;
    int veh_size = cache_veh_infos_.veh_cache_infos[ind].veh_infos.size();
    std::vector<Rect> boxes(box_size);
    if (veh_size > box_size)
    {
        for (int i = 0; i < box_size; i++)
        {
            boxes[i] = cache_veh_infos_.veh_cache_infos[ind].veh_infos[veh_size - 1 - i].location;
        }
    }
    else
    {
        box_size = 0;
    }

    bool isMoveOut = false;

    if (box_size > 20)
    {
        float piou_rat = 0.4;
        if (cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_locs.size() > 0)
        {
            isMoveOut = is_move_standstill_loc(
                boxes, cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_locs, piou_rat);
        }
        else
        {
            isMoveOut = is_move_out(boxes, piou_rat);
        }

        if (isMoveOut)
        {
            if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_move_count > 50 &&
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_count == 0)
            {
                if (is_stand(boxes))
                {
                    isMoveOut = false;
                }
            }
        }
        else
        {
            float space_rat_cha =
                cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_chassis.space_rat -
                cache_veh_infos_.veh_cache_infos[ind]
                    .veh_infos[cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() - 1]
                    .veh_chassis.space_rat;

            if (space_rat_cha > 0.3 &&
                cache_veh_infos_.veh_cache_infos[ind]
                        .veh_infos[cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() - 1]
                        .veh_chassis.space_id >= 0 &&
                cache_veh_infos_.veh_cache_infos[ind]
                        .veh_infos[cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() - 1]
                        .veh_chassis.space_id !=
                    cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_chassis.space_id)
            {
                isMoveOut = true;
            }
        }
    }

    if (!isMoveOut)
    {
        int line_angle_stand =
            cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_chassis.line_angle;
        int line_angle_now = cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.line_angle;
        int angle_ch = line_angle_now - line_angle_stand;
        if (angle_ch > 30 && line_angle_now > 15)
        {
            isMoveOut = true;
        }
    }

    if (isMoveOut)
    {
        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_move_count++;
        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_stand_count = 0;
        return true;
    }
    else
    {
        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_move_count = 0;
        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_stand_count++;

        if (cache_veh_infos_.veh_cache_infos[ind]
                    .veh_infos[cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() - 1]
                    .veh_chassis.space_rat > 0.2 &&
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_stand_count % 45 == 0)
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_stand_count = 1;
            UpdateVehStandstillLoc(cache_veh_infos_.veh_cache_infos[ind].veh_infos, ind);
        }

        return false;
    }
    return false;
}

void
VehBaseInfo::IsDisAbnormal(int ind)
{
    if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() < 1 ||
        cache_veh_infos_.veh_cache_infos[ind].frame_height < 200 ||
        cache_veh_infos_.veh_cache_infos[ind].frame_width < 200)
    {
        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear_abnormal = false;
        return;
    }

    int h_rat = cache_veh_infos_.veh_cache_infos[ind].frame_height / 20;
    int w_rat = cache_veh_infos_.veh_cache_infos[ind].frame_width / 20;
    Rect box = cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().location;
    if (box.x_ < w_rat || box.y_ < h_rat ||
        (box.x_ + box.width_) > (cache_veh_infos_.veh_cache_infos[ind].frame_width - w_rat) ||
        (box.y_ + box.height_) > (cache_veh_infos_.veh_cache_infos[ind].frame_height - h_rat))
    {
        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear_abnormal = false;
    }
    else
    {
        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear_abnormal = true;
    }
}

int
VehBaseInfo::GetVehStateFraction(const VehInfo &veh_info, int ind)
{
    int veh_state_fraction = 90;
    int count = 10;
    if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() > 0)
    {
        if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() > count)
        {
            float avg_confidence = 0.0;
            for (int i = 0; i < count; i++)
            {
                avg_confidence += cache_veh_infos_.veh_cache_infos[ind]
                                      .veh_infos[cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() - 1 - i]
                                      .veh_det_confidence;
            }
            avg_confidence = avg_confidence / count;
            veh_state_fraction = int(avg_confidence * 100);
        }
        else
        {
            veh_state_fraction = int(veh_info.veh_det_confidence * 100);
        }
    }

    return veh_state_fraction;
}

float
VehBaseInfo::GetVehAvgCoverRat(const VehInfo &veh_info, int ind)
{
    float avg_cover_rat = 0.0;
    int veh_size = cache_veh_infos_.veh_cache_infos[ind].veh_infos.size();
    if (veh_size < 1)
    {
        return avg_cover_rat;
    }
    int count = std::min(veh_size, 10);
    for (int i = 0; i < count; i++)
    {
        avg_cover_rat += cache_veh_infos_.veh_cache_infos[ind]
                             .veh_infos[cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() - 1 - i]
                             .cover_rat;
    }
    avg_cover_rat = avg_cover_rat / count;

    return avg_cover_rat;
}

std::shared_ptr<VehiclesCacheList>
VehBaseInfo::GetNewVehiclesCacheList(const VehInfoVec &vehicles)
{
    std::shared_ptr<VehiclesCacheList> vehicles_cache_list = std::make_shared<VehiclesCacheList>();

    for (const auto &veh_info : vehicles.veh_info)
    {
        if (veh_info.veh_track_id < 1 || veh_info.track_state_ != TrackState::Tracked)
        {
            continue;
        }

        for (auto &veh_cache_info : cache_veh_infos_.veh_cache_infos)
        {
            if (veh_cache_info.track_id == veh_info.veh_track_id)
            {
                vehicles_cache_list->data.push_back(std::addressof(veh_cache_info));
            }
        }
    }
    return vehicles_cache_list;
}

// 判断当前车辆space_id
int
VehBaseInfo::VehInSpaceId(int ind)
{
    int space_id = cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_chassis.space_id;

    int space_id_count = 0;
    int space_id_b = -1;
    int space_id_b_count = -1;
    if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() > 10)
    {
        int veh_standstill_loc_count = 10;
        int line_angle_avg = 0;
        for (int i = 0; i < veh_standstill_loc_count; i++)
        {
            line_angle_avg += cache_veh_infos_.veh_cache_infos[ind]
                                  .veh_infos[cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() - 1 - i]
                                  .veh_chassis.line_angle;

            int spaceid_temp = cache_veh_infos_.veh_cache_infos[ind]
                                   .veh_infos[cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() - 1 - i]
                                   .veh_chassis.space_id;

            if (spaceid_temp == space_id)
            {
                space_id_count++;
            }
            else
            {
                if (space_id_b < 0)
                {
                    space_id_b = spaceid_temp;
                    space_id_b_count = 1;
                }
                else
                {
                    if (spaceid_temp == space_id_b)
                    {
                        space_id_b_count++;
                    }
                }
            }
        }
        line_angle_avg = line_angle_avg / veh_standstill_loc_count;
        cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_chassis.line_angle = line_angle_avg;
    }

    if (space_id_count >= space_id_b_count && space_id >= 0)
    {
        cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_chassis.space_id = space_id;
    }
    else
    {
        if (space_id_count < space_id_b_count && space_id_b >= 0)
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_chassis.space_id = space_id_b;
        }
    }
    space_id = cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_chassis.space_id;

    return space_id;
}

// 判断出场车辆与入场车辆的相似性
// 0：不同 1：相同 2：相似度较高 3: 入出场均无车牌 4: 入场车辆无车牌号 5：出场车辆无车牌号 6: 无法比较
int
VehBaseInfo::CompareInOutVehLp(LPInfo veh_in_lp_info, LPInfo veh_out_lp_info)
{
    int status = 3;
    std::string veh_in_lp_str = veh_in_lp_info.lp_number.lp_str;
    std::string veh_out_lp_str = veh_out_lp_info.lp_number.lp_str;

    if (veh_in_lp_str == "-" && veh_out_lp_str == "-")
    {
        return 3;
    }
    if (veh_in_lp_str == "-")
    {
        return 4;
    }
    if (veh_out_lp_str == "-")
    {
        return 5;
    }
    std::wstring veh_in_lp_strw = utf8ToUnicode(veh_in_lp_str);
    std::wstring veh_out_lp_strw = utf8ToUnicode(veh_out_lp_str);

    int dis = 0;
    dis = wstr_distance(veh_in_lp_strw, veh_out_lp_strw);

    if (dis == 0)
    {
        status = 1;
    }
    else if (dis <= 2)
    {
        if (veh_in_lp_info.lp_color != SL_PLATE_COLOR_UNKNOWN && veh_out_lp_info.lp_color != SL_PLATE_COLOR_UNKNOWN)
        {
            if (veh_in_lp_info.lp_color != veh_out_lp_info.lp_color)
            {
                dis++;
            }
        }

        if (dis <= 2)
        {
            status = 2;
        }
        else
        {
            status = 0;
        }
    }
    else
    {
        status = 0;
    }

    return status;
}

} // namespace sonli
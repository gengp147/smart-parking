#ifndef _INOUT_VEHCOPY_INFO_H_
#define _INOUT_VEHCOPY_INFO_H_

#include "data_type.h"

namespace sonli
{
// lp_b.lp_a
void inline LpNumberCopy(LPNumber &lp_a, const LPNumber& lp_b)
{
    lp_a.lp_str = lp_b.lp_str;
    lp_a.vlp_rec_confidence = lp_b.vlp_rec_confidence;
    lp_a.lp_count = lp_b.lp_count;
    lp_a.first_char_confidence = lp_b.first_char_confidence;
    lp_a.second_char_confidence = lp_b.second_char_confidence;
}

// lp_b.lp_a
void inline LpInfoCopy(LPInfo &lp_a, const LPInfo& lp_b)
{
    lp_a.lp_type = lp_b.lp_type;
    lp_a.lp_color = lp_b.lp_color;
    lp_a.location = lp_b.location;
    lp_a.vlp_det_confidence = lp_b.vlp_det_confidence;
    lp_a.lp_color_count = lp_b.lp_color_count;
    lp_a.lp_status = lp_b.lp_status;

    LpNumberCopy(lp_a.lp_number, lp_b.lp_number);
}

// veh_b.veh_a
void inline VehInfoCopy(VehInfo &veh_a, const VehInfo& veh_b)
{
    veh_a.feature = veh_b.feature.clone();
    veh_a.location = veh_b.location;
    veh_a.veh_type = veh_b.veh_type;
    veh_a.veh_color = veh_b.veh_color;
    veh_a.veh_det_confidence = veh_b.veh_det_confidence;
    veh_a.veh_timestamp = veh_b.veh_timestamp;
    veh_a.time_info = veh_b.time_info;
    // veh_a.frame_id = veh_b.frame_id;
    veh_a.veh_track_id = veh_b.veh_track_id;
    veh_a.send_veh_id = veh_b.send_veh_id;
    veh_a.cover_rat = veh_b.cover_rat;
    veh_a.is_image_edge = veh_b.is_image_edge;
    // veh_a.is_move = veh_b.is_move;
    // veh_a.move_rat = veh_b.move_rat;
    veh_a.track_state_ = veh_b.track_state_;

    veh_a.veh_chassis.space_id = veh_b.veh_chassis.space_id;
    veh_a.veh_chassis.space_rat = veh_b.veh_chassis.space_rat;
    veh_a.veh_chassis.space_res = veh_b.veh_chassis.space_res;
    veh_a.veh_chassis.chassis = veh_b.veh_chassis.chassis;

    for (int i = 0; i < 16; i++)
    {
        veh_a.veh_key_points[i] = veh_b.veh_key_points[i];
    }

    LpInfoCopy(veh_a.lp_info, veh_b.lp_info);
}

// msg_b.meg_a
void inline VehSendMesCopy(VehSendMessage &meg_a, const VehSendMessage& msg_b)
{
    VehInfoCopy(meg_a.veh_info, msg_b.veh_info);
    meg_a.biger_box = msg_b.biger_box;
    meg_a.veh_locs = msg_b.veh_locs;
    meg_a.frame_timestamp = msg_b.frame_timestamp;
    meg_a.veh_pic_id = msg_b.veh_pic_id;
    meg_a.channel_id = msg_b.channel_id;
    meg_a.veh_state_fraction = msg_b.veh_state_fraction;
    meg_a.veh_state = msg_b.veh_state;
}

// order_b.order_a
void inline VehOrderCopy(VehInOutInfo &order_a, const VehInOutInfo& order_b)
{
    order_a.in_out = order_b.in_out;
    order_a.camera_channel = order_b.camera_channel;
    order_a.inout_timestamp = order_b.inout_timestamp;
    order_a.in_timestamp = order_b.in_timestamp;
    order_a.send_timestamp = order_b.send_timestamp;
    order_a.space_code = order_b.space_code;
    order_a.veh_state_fraction = order_b.veh_state_fraction;
    order_a.is_save = order_b.is_save;

    VehInfoCopy(order_a.out_veh_info, order_b.out_veh_info);
    VehInfoCopy(order_a.pro_veh_info, order_b.pro_veh_info);
    VehInfoCopy(order_a.in_veh_info, order_b.in_veh_info);
}

// veh_vec_b.veh_vec_a
void inline VehVecCopy(std::vector<VehInfo> &veh_vec_a, const std::vector<VehInfo>& veh_vec_b)
{
    for (VehInfo veh : veh_vec_b)
    {
        VehInfo veh_t;
        VehInfoCopy(veh_t, veh);
        veh_vec_a.push_back(veh_t);
    }
}

// veh_inout_var_b.veh_inout_var_a
void inline VehInOutVarCopy(VehInOutVar &veh_inout_var_a, const VehInOutVar& veh_inout_var_b)
{
    veh_inout_var_a.is_combine = veh_inout_var_b.is_combine;
    veh_inout_var_a.veh_state = veh_inout_var_b.veh_state;

    veh_inout_var_a.out_max_iou = veh_inout_var_b.out_max_iou;
    veh_inout_var_a.is_satisfy_out = veh_inout_var_b.is_satisfy_out;
    veh_inout_var_a.is_satisfy_out_flag = veh_inout_var_b.is_satisfy_out_flag;
    veh_inout_var_a.is_satisfy_in = veh_inout_var_b.is_satisfy_in;

    veh_inout_var_a.is_disappear = veh_inout_var_b.is_disappear;
    veh_inout_var_a.is_disappear_abnormal = veh_inout_var_b.is_disappear_abnormal;
    veh_inout_var_a.obscured = veh_inout_var_b.obscured;
    veh_inout_var_a.is_report_in = veh_inout_var_b.is_report_in;
    veh_inout_var_a.is_report_out = veh_inout_var_b.is_report_out;
    veh_inout_var_a.is_judge_suspected_in = veh_inout_var_b.is_judge_suspected_in;
    veh_inout_var_a.is_judge_suspected_out = veh_inout_var_b.is_judge_suspected_out;

    veh_inout_var_a.to_delete = veh_inout_var_b.to_delete;
    veh_inout_var_a.to_report_in = veh_inout_var_b.to_report_in;
    veh_inout_var_a.to_report_out = veh_inout_var_b.to_report_out;
    veh_inout_var_a.order_generate_type = veh_inout_var_b.order_generate_type;

    veh_inout_var_a.is_leave = veh_inout_var_b.is_leave;
    veh_inout_var_a.is_standstill = veh_inout_var_b.is_standstill;

    veh_inout_var_a.no_send_in = veh_inout_var_b.no_send_in;
    veh_inout_var_a.no_send_in_flag = veh_inout_var_b.no_send_in_flag;

    veh_inout_var_a.in_count = veh_inout_var_b.in_count;
    veh_inout_var_a.in_move_count = veh_inout_var_b.in_move_count;
    veh_inout_var_a.in_stand_count = veh_inout_var_b.in_stand_count;
    veh_inout_var_a.out_count = veh_inout_var_b.out_count;
    veh_inout_var_a.out_move_count = veh_inout_var_b.out_move_count;

    veh_inout_var_a.veh_appear_timestamp = veh_inout_var_b.veh_appear_timestamp;
    veh_inout_var_a.veh_disappear_timestamp = veh_inout_var_b.veh_disappear_timestamp;
    veh_inout_var_a.report_in_timestamp = veh_inout_var_b.report_in_timestamp;
    veh_inout_var_a.report_out_timestamp = veh_inout_var_b.report_out_timestamp;

    // veh_inout_var_a.access_veh_timestamp = veh_inout_var_b.access_veh_timestamp;
}

// cache_b.cache_a
void inline CacheVehInfoCopy(CacheVehInfo &cache_a, const CacheVehInfo& cache_b)
{
    cache_a.track_id = cache_b.track_id;

    cache_a.frame_height = cache_b.frame_height;
    cache_a.frame_width = cache_b.frame_width;

    VehInOutVarCopy(cache_a.veh_in_out_var, cache_b.veh_in_out_var);

    // VehVecCopy(cache_a.veh_infos, cache_b.veh_infos);
    // VehVecCopy(cache_a.veh_infos_first_n, cache_b.veh_infos_first_n);
    cache_a.veh_locs_first_n = cache_b.veh_locs_first_n;

    cache_a.lp_str_vec = cache_b.lp_str_vec;
    cache_a.lp_color_vec = cache_b.lp_color_vec;
    cache_a.veh_color_vec = cache_b.veh_color_vec;
    cache_a.veh_type_vec = cache_b.veh_type_vec;
    cache_a.no_lp_count = cache_b.no_lp_count;
    cache_a.sapceID_vec = cache_b.sapceID_vec;

    // cache_a.veh_inpro_out_mess = cache_b.veh_inpro_out_mess;
    // cache_a.veh_inpro_pro_mess = cache_b.veh_inpro_pro_mess;
    // cache_a.veh_standstill_mess = cache_b.veh_standstill_mess;
    // cache_a.veh_outpro_std_mess = cache_b.veh_outpro_std_mess;
    // cache_a.veh_outpro_pro_mess = cache_b.veh_outpro_pro_mess;
    // cache_a.veh_outpro_out_mess = cache_b.veh_outpro_out_mess;

    LpInfoCopy(cache_a.lp_info, cache_b.lp_info);

    // cache_a.space_rate_list = cache_b.space_rate_list;
}

// cache_vec_b.cache_vec_a
void inline CacheVehInfoVecCopy(CacheVehInfoVec &cache_vec_a, const CacheVehInfoVec & cache_vec_b)
{
    cache_vec_a.frame_timestamp = cache_vec_b.frame_timestamp;
    cache_vec_a.frame_id = cache_vec_b.frame_id;
    cache_vec_a.camera_channel = cache_vec_b.camera_channel;
    cache_vec_a.generate_suspected_order = cache_vec_b.generate_suspected_order;

    for (CacheVehInfo veh : cache_vec_b.veh_cache_infos)
    {
        CacheVehInfo veh_t;
        CacheVehInfoCopy(veh_t, veh);
        cache_vec_a.veh_cache_infos.push_back(veh_t);
    }
}

} // namespace sonli

#endif

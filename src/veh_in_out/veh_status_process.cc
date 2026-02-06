#include "veh_status_process.h"
#include "../models_processor/char_code.h"
#include "../utils/sl_logger.h"
#include "error.h"
#include "veh_info_copy.h"
#include <algorithm>

namespace sonli
{

bool
VehicleInOutProcess::CacheFrameChange(int ind, std::string lp_str_pre, float lp_str_pre_conf, bool use_report_in,
                                      SLVehicleState_t veh_state)
{
    std::string lp_str_now = cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().lp_info.lp_number.lp_str;
    if (lp_str_now == "-")
    {
        return false;
    }
    if (lp_str_pre == "-")
    {
        return true;
    }

    bool is_change = false;
    float lp_str_now_conf = cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().lp_info.lp_number.vlp_rec_confidence;
    std::string lp_str_cache = cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str;
    if (use_report_in &&
        cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.lp_str != "-")
    {
        lp_str_cache = cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.lp_str;
    }

    if (lp_str_cache != lp_str_pre)
    {
        if (lp_str_now == lp_str_cache)
        {
            is_change = true;
        }
    }
    else
    {
        if (veh_state != SL_CAR_STATE_IN_PRE && lp_str_now == lp_str_cache && lp_str_now_conf > lp_str_pre_conf)
        {
            is_change = true;
        }
    }

    return is_change;
}

// 车辆有驶入趋势
void
VehicleInOutProcess::VehMoveInProcess(CapControlMsgVec &cap_control, int ind)
{

    if (ind < 0)
    {
        SonLiLogger::getInstance()
            .getLogger(SonLiLogger::RotateLogger)
            ->info("Vehicle VehMoveInProcess ind < 0 error ! channel {}", cache_veh_infos_.camera_channel);
    }
    else
    {
        if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() > 0)
        {
            bool is_change = false;
            if (cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_pic_id < 0)
            {
                is_change = true;
            }
            else
            {
                if (cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str ==
                    cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.lp_info.lp_number.lp_str)
                {
                    is_change = false;
                }
                else
                {
                    if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_rat < 0.02 &&
                        cache_veh_infos_.frame_timestamp -
                                cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.frame_timestamp >
                            300)
                    {
                        is_change = CacheFrameChange(
                            ind,
                            cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.lp_info.lp_number.lp_str,
                            cache_veh_infos_.veh_cache_infos[ind]
                                .veh_inpro_out_mess.veh_info.lp_info.lp_number.vlp_rec_confidence,
                            false, SL_CAR_STATE_IN_PRE);
                    }
                }
            }
            if (is_change)
            {
                int ret = UpdateCacheVehSendMessage(cap_control, ind, SL_CAR_STATE_IN_PRE);
                if (ret != SL_SUCCESS)
                {
                    SonLiLogger::getInstance()
                        .getLogger(SonLiLogger::RotateLogger)
                        ->info("VehMoveInProcess UpdateCacheVehSendMessage failed ! track_id {}, channel {}, line {}",
                               cache_veh_infos_.veh_cache_infos[ind].track_id, cache_veh_infos_.camera_channel,
                               __LINE__);
                }
                return;
            }
        }
        else
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->info("Vehicle VehMoveInProcess veh_infos.size() <= 0 error ! channel {}",
                       cache_veh_infos_.camera_channel);
        }
    }
}

// 车辆驶入流程
void
VehicleInOutProcess::VehInProcess(CapControlMsgVec &cap_control, int ind)
{

    int ret;

    if (ind < 0)
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info("Vehicle VehInProcess ind < 0 error !");
    }
    else
    {
        if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() < 1)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->info("Vehicle VehInProcess veh_infos.size() <= 0 error !");
            return;
        }

        if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().location.area() >
            cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.biger_box.area())
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.biger_box =
                cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().location;
        }

        bool is_change = false;
        // if (cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.lp_info.lp_number.lp_str != "-" &&
        // 	cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str != "-" &&
        // 	cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str !=
        // 		cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.lp_info.lp_number.lp_str)
        // {
        // 	is_change = true;
        // }

        if (cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_pic_id < 0 || is_change)
        {
            ret = UpdateCacheVehSendMessage(cap_control, ind, SL_CAR_STATE_IN_PRE);
            if (ret != SL_SUCCESS)
            {
                SonLiLogger::getInstance()
                    .getLogger(SonLiLogger::RotateLogger)
                    ->info("VehInProcess UpdateCacheVehSendMessage failed ! track_id {}, channel {}, line {}",
                           cache_veh_infos_.veh_cache_infos[ind].track_id, cache_veh_infos_.camera_channel, __LINE__);
            }
            return;
        }

        if (cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_pic_id < 0)
        {
            ret = UpdateCacheVehSendMessage(cap_control, ind, SL_CAR_STATE_IN);
            if (ret != SL_SUCCESS)
            {
                SonLiLogger::getInstance()
                    .getLogger(SonLiLogger::RotateLogger)
                    ->info("VehInProcess UpdateCacheVehSendMessage failed ! track_id {}, channel {}, line {}",
                           cache_veh_infos_.veh_cache_infos[ind].track_id, cache_veh_infos_.camera_channel, __LINE__);
            }
            return;
        }

        bool time_status = time_compare(cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.veh_timestamp,
                                        cache_veh_infos_.frame_timestamp, 300);
        if (time_status)
        {
            ret = ResetVehInInfo(cap_control, ind);
            if (ret != SL_SUCCESS)
            {
                SonLiLogger::getInstance()
                    .getLogger(SonLiLogger::RotateLogger)
                    ->info("ResetVehInInfo failed! in long time. track_id {} lp_str {}",
                           cache_veh_infos_.veh_cache_infos[ind].track_id,
                           cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str);
            }
            return;
        }

        is_change = CacheFrameChange(
            ind, cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.lp_str,
            cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.vlp_rec_confidence);

        if (cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.frame_timestamp <=
            cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.frame_timestamp)
        {
            is_change = true;
        }

        // 判断车辆位置及运动状态 1.车辆停在泊位内，2.车辆在移动
        int veh_status = VehInStatus(ind);

        // if (cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str ==
        // 		cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.lp_info.lp_number.lp_str &&
        // 	cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.veh_chassis.space_id !=
        // 		cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_id &&
        // 	veh_status == 2)
        // {
        // 	is_change = true;
        // }

        if (is_change)
        {
            ret = UpdateCacheVehSendMessage(cap_control, ind, SL_CAR_STATE_IN);
            if (ret != SL_SUCCESS)
            {
                SonLiLogger::getInstance()
                    .getLogger(SonLiLogger::RotateLogger)
                    ->info("VehInProcess UpdateCacheVehSendMessage failed ! track_id {}, channel {}, line {}",
                           cache_veh_infos_.veh_cache_infos[ind].track_id, cache_veh_infos_.camera_channel, __LINE__);
            }
            return;
        }

        // 判断车牌识别是否识别到
        bool veh_lp_number_status =
            IsLpNumberOkay(cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_count,
                           cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.vlp_rec_confidence);

        if (veh_status == 1 && veh_lp_number_status)
        {
            // 车辆停在泊位内 识别到车牌

            if (IsSatisfyIn(ind))
            {
                if (cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_pic_id < 0)
                {
                    ret = UpdateCacheVehSendMessage(cap_control, ind, SL_CAR_STATE_STOP);
                    if (ret != SL_SUCCESS)
                    {
                        SonLiLogger::getInstance()
                            .getLogger(SonLiLogger::RotateLogger)
                            ->info("VehInProcess UpdateCacheVehSendMessage failed ! track_id {}, channel {}, line {}",
                                   cache_veh_infos_.veh_cache_infos[ind].track_id, cache_veh_infos_.camera_channel,
                                   __LINE__);
                    }
                    return;
                }

                int space_id = VehInSpaceId(ind);
                if (space_id != cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_chassis.space_id)
                {
                    ret = UpdateCacheVehSendMessage(cap_control, ind, SL_CAR_STATE_STOP);
                    if (ret != SL_SUCCESS)
                    {
                        SonLiLogger::getInstance()
                            .getLogger(SonLiLogger::RotateLogger)
                            ->info("VehInProcess UpdateCacheVehSendMessage failed ! track_id {}, channel {}, line {}",
                                   cache_veh_infos_.veh_cache_infos[ind].track_id, cache_veh_infos_.camera_channel,
                                   __LINE__);
                    }
                    return;
                }
                if (space_id == -1)
                {
                    SonLiLogger::getInstance()
                        .getLogger(SonLiLogger::RotateLogger)
                        ->info("VehInProcess space_id -1 ! track_id {}, channel {}, line {}, lp_str {}",
                               cache_veh_infos_.veh_cache_infos[ind].track_id, cache_veh_infos_.camera_channel,
                               __LINE__, cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str);
                    return;
                }

                // 判断车辆是否停稳
                bool veh_standstill_status =
                    time_compare(cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_timestamp,
                                 cache_veh_infos_.frame_timestamp, park_param_.veh_in_time_thres);
                if (veh_standstill_status)
                {
                    // 缓存入场停稳图片
                    ret = UpdateCacheVehSendMessage(cap_control, ind, SL_CAR_STATE_STOP);
                    if (ret != SL_SUCCESS)
                    {
                        SonLiLogger::getInstance()
                            .getLogger(SonLiLogger::RotateLogger)
                            ->info("VehInProcess UpdateCacheVehSendMessage failed ! track_id {}, channel {}, line {}",
                                   cache_veh_infos_.veh_cache_infos[ind].track_id, cache_veh_infos_.camera_channel,
                                   __LINE__);
                    }
                    VehInProUpload(ind);
                }
            }
            else
            {
                // 车辆未停稳
            }
        }
        else if (veh_status == 2 || (veh_status == 1 && !veh_lp_number_status))
        {
            // 车辆在泊位内移动 或 停在车位内 但无车牌
            if (IsSatisfyIn(ind))
            {
                if (cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_pic_id < 0)
                {
                    ret = UpdateCacheVehSendMessage(cap_control, ind, SL_CAR_STATE_STOP);
                    if (ret != SL_SUCCESS)
                    {
                        SonLiLogger::getInstance()
                            .getLogger(SonLiLogger::RotateLogger)
                            ->info("VehInProcess UpdateCacheVehSendMessage failed ! track_id {}, channel {}, line {}",
                                   cache_veh_infos_.veh_cache_infos[ind].track_id, cache_veh_infos_.camera_channel,
                                   __LINE__);
                    }
                    return;
                }

                int space_id = VehInSpaceId(ind);
                if (space_id != cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_chassis.space_id)
                {
                    ret = UpdateCacheVehSendMessage(cap_control, ind, SL_CAR_STATE_STOP);
                    if (ret != SL_SUCCESS)
                    {
                        SonLiLogger::getInstance()
                            .getLogger(SonLiLogger::RotateLogger)
                            ->info("VehInProcess UpdateCacheVehSendMessage failed ! track_id {}, channel {}, line {}",
                                   cache_veh_infos_.veh_cache_infos[ind].track_id, cache_veh_infos_.camera_channel,
                                   __LINE__);
                    }
                    return;
                }
                if (space_id == -1)
                {
                    SonLiLogger::getInstance()
                        .getLogger(SonLiLogger::RotateLogger)
                        ->info("VehInProcess space_id -1 ! track_id {}, channel {}, line {}, lp_str {}",
                               cache_veh_infos_.veh_cache_infos[ind].track_id, cache_veh_infos_.camera_channel,
                               __LINE__, cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str);
                    return;
                }

                int standstill_time_conf = 2 * park_param_.veh_in_time_thres;
                if (veh_lp_number_status)
                {
                    standstill_time_conf = 2 * park_param_.veh_in_time_thres;
                }
                if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in)
                {
                    standstill_time_conf = park_param_.veh_in_time_thres;
                }

                // 判断车辆入场时间是否超阈值
                bool veh_intime_status =
                    time_compare(cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_timestamp,
                                 cache_veh_infos_.frame_timestamp, standstill_time_conf);
                // 车辆入场时间超阈值
                if (veh_intime_status)
                {
                    // 缓存当前图片作为停稳图片
                    ret = UpdateCacheVehSendMessage(cap_control, ind, SL_CAR_STATE_STOP);
                    if (ret != SL_SUCCESS)
                    {
                        SonLiLogger::getInstance()
                            .getLogger(SonLiLogger::RotateLogger)
                            ->info("VehInProcess UpdateCacheVehSendMessage failed ! track_id {}, channel {}, line {}",
                                   cache_veh_infos_.veh_cache_infos[ind].track_id, cache_veh_infos_.camera_channel,
                                   __LINE__);
                    }
                    VehInProUpload(ind);
                }
            }
            else
            {
                // 车辆未停稳
            }
        }
        else
        {
        }
    }
}

// 车辆停稳流程
void
VehicleInOutProcess::VehStandProcess(CapControlMsgVec &cap_control, int ind)
{
    int ret;
    // std::vector<VehInfo> veh_infos = cache_veh_infos_.veh_cache_infos[ind].veh_infos;

    if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() < 1)
    {
        return;
    }

    // 判断车辆是否离开泊位
    bool is_leave = VehOutStatus(ind);

    bool is_satisfy_out = IsSatisfyOut(ind);

    if (!is_satisfy_out)
    {
        if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_out_flag > -1 ||
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_out > -1)
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_out_flag = -1;
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_out = -1;
            if (!cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_report_in)
            {
                ResetVehOutInfo(cap_control, ind);
            }
        }
    }

    if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_res == SPACE_IN &&
        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_report_in && !is_leave && !is_satisfy_out)
    {
        if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_pic_id < 0)
        {
            ret = UpdateCacheVehSendMessage(cap_control, ind, SL_CAR_STATE_STOP_LEAVE);
            if (ret != SL_SUCCESS)
            {
                SonLiLogger::getInstance()
                    .getLogger(SonLiLogger::RotateLogger)
                    ->info("VehStandProcess UpdateCacheVehSendMessage failed ! track_id {}, channel {}, line {}",
                           cache_veh_infos_.veh_cache_infos[ind].track_id, cache_veh_infos_.camera_channel, __LINE__);
            }
            return;
        }
        else
        {
            if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_id ==
                    cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_chassis.space_id &&
                cache_veh_infos_.frame_timestamp -
                        cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.frame_timestamp >
                    2500)
            {
                bool is_change = time_compare(cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.frame_timestamp,
                                              cache_veh_infos_.frame_timestamp, 120);
                if (!is_change)
                {
                    bool use_report_in = false;
                    if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number.lp_str !=
                        "-")
                    {
                        use_report_in = true;
                    }

                    is_change = CacheFrameChange(
                        ind,
                        cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_info.lp_info.lp_number.lp_str,
                        cache_veh_infos_.veh_cache_infos[ind]
                            .veh_outpro_std_mess.veh_info.lp_info.lp_number.vlp_rec_confidence,
                        use_report_in);
                }

                if (is_change)
                {
                    ret = UpdateCacheVehSendMessage(cap_control, ind, SL_CAR_STATE_STOP_LEAVE);
                    if (ret != SL_SUCCESS)
                    {
                        SonLiLogger::getInstance()
                            .getLogger(SonLiLogger::RotateLogger)
                            ->info(
                                "VehStandProcess UpdateCacheVehSendMessage failed ! track_id {}, channel {}, line {}",
                                cache_veh_infos_.veh_cache_infos[ind].track_id, cache_veh_infos_.camera_channel,
                                __LINE__);
                    }
                    return;
                }
            }
        }
    }

    if (is_leave || is_satisfy_out)
    {
        // 车辆预驶离泊位

        if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().location.area() >
            cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.biger_box.area())
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.biger_box =
                cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().location;
        }

        // 判断车辆出场有无车牌号
        //  bool veh_lp_number_status = IsLpNumberOkay(cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_count,
        //  										   cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.vlp_rec_confidence);
        //  if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_move_count < 2) {
        //  	LpNumberCopy(cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number,
        //  		cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number);
        //  }

        if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_pic_id < 0)
        {
            ret = UpdateCacheVehSendMessage(cap_control, ind, SL_CAR_STATE_STOP_LEAVE);
            if (ret != SL_SUCCESS)
            {
                SonLiLogger::getInstance()
                    .getLogger(SonLiLogger::RotateLogger)
                    ->info("VehStandProcess UpdateCacheVehSendMessage failed ! track_id {}, channel {}, line {}",
                           cache_veh_infos_.veh_cache_infos[ind].track_id, cache_veh_infos_.camera_channel, __LINE__);
            }
            return;
        }

        if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_pic_id < 0 ||
            cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.frame_timestamp <
                cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.frame_timestamp)
        {
            ret = UpdateCacheVehSendMessage(cap_control, ind, SL_CAR_STATE_LEAVING);
            if (ret != SL_SUCCESS)
            {
                SonLiLogger::getInstance()
                    .getLogger(SonLiLogger::RotateLogger)
                    ->info("VehStandProcess UpdateCacheVehSendMessage failed ! track_id {}, channel {}, line {}",
                           cache_veh_infos_.veh_cache_infos[ind].track_id, cache_veh_infos_.camera_channel, __LINE__);
            }
            return;
        }

        bool time_status =
            time_compare(cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_info.veh_timestamp,
                         cache_veh_infos_.frame_timestamp, 300);
        if (time_status)
        {
            ret = ResetVehOutInfo(cap_control, ind);
            if (ret != SL_SUCCESS)
            {
                SonLiLogger::getInstance()
                    .getLogger(SonLiLogger::RotateLogger)
                    ->info("ResetVehInInfo failed! out long time. track_id {} lp_str {}",
                           cache_veh_infos_.veh_cache_infos[ind].track_id,
                           cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str);
            }
            return;
        }

        // int space_id = cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_chassis.space_id;
        // if (space_id >= 0 &&
        // 	space_id < cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_rat_vec.size() &&
        // 	cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_rat_vec[space_id] > 0.4) {
        // 	LpNumberCopy(cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number,
        // 		cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number);
        // }

        bool use_report_in = false;
        if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number.lp_str != "-")
        {
            use_report_in = true;
        }

        bool is_change = CacheFrameChange(
            ind, cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number.lp_str,
            cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number.vlp_rec_confidence,
            use_report_in);

        if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.veh_chassis.space_rat > 0.02 &&
            cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_rat < 0.02 &&
            cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().lp_info.lp_number.lp_str != "-")
        {
            if (cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.lp_str != "-")
            {
                if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().lp_info.lp_number.lp_str ==
                    cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.lp_str)
                {
                    is_change = true;
                }
            }
            else
            {
                if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().lp_info.lp_number.lp_str ==
                    cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str)
                {
                    is_change = true;
                }
            }
        }

        if (is_change)
        {
            ret = UpdateCacheVehSendMessage(cap_control, ind, SL_CAR_STATE_LEAVING);
            if (ret != SL_SUCCESS)
            {
                SonLiLogger::getInstance()
                    .getLogger(SonLiLogger::RotateLogger)
                    ->info("VehStandProcess UpdateCacheVehSendMessage failed ! track_id {}, channel {}, line {}",
                           cache_veh_infos_.veh_cache_infos[ind].track_id, cache_veh_infos_.camera_channel, __LINE__);
            }
            return;
        }

        // if (veh_lp_number_status) {
        if (is_satisfy_out)
        {
            ret = UpdateCacheVehSendMessage(cap_control, ind, SL_CAR_STATE_OUT);
            if (ret != SL_SUCCESS)
            {
                SonLiLogger::getInstance()
                    .getLogger(SonLiLogger::RotateLogger)
                    ->info("VehStandProcess UpdateCacheVehSendMessage failed ! track_id {}, channel {}, line {}",
                           cache_veh_infos_.veh_cache_infos[ind].track_id, cache_veh_infos_.camera_channel, __LINE__);
            }
            VehOutProUpload(cap_control, ind);
        }
        // }
    }
    else
    {

        bool veh_stdtime_ch =
            time_compare(cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_info.veh_timestamp,
                         cache_veh_infos_.frame_timestamp, 30);
        if (veh_stdtime_ch)
        {
            ResetVehOutInfo(cap_control, ind);
            ret = UpdateCacheVehSendMessage(cap_control, ind, SL_CAR_STATE_STOP_LEAVE);
            if (ret != SL_SUCCESS)
            {
                SonLiLogger::getInstance()
                    .getLogger(SonLiLogger::RotateLogger)
                    ->info("VehStandProcess UpdateCacheVehSendMessage failed ! track_id {}, channel {}, line {}",
                           cache_veh_infos_.veh_cache_infos[ind].track_id, cache_veh_infos_.camera_channel, __LINE__);
            }
        }

        if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_pic_id >= 0)
        {
            bool veh_outtime_ch =
                time_compare(cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.veh_timestamp,
                             cache_veh_infos_.frame_timestamp, 120);
            if (veh_outtime_ch)
            {
                ResetVehOutInfo(cap_control, ind);
            }
        }
    }
}

static inline bool
CalculateOcclusionRateForCache(const CacheVehInfoVec &cache_veh_infos_, int a, int b)
{
    auto count = std::min((std::size_t)5, std::min(cache_veh_infos_.veh_cache_infos[a].veh_infos.size(),
                                                   cache_veh_infos_.veh_cache_infos[b].veh_infos.size()));
    int a_s_i = cache_veh_infos_.veh_cache_infos[a].veh_infos.size() - count;
    int b_s_i = cache_veh_infos_.veh_cache_infos[b].veh_infos.size() - count;
    float c1, c2, tc = 0;
    for (unsigned int i = 0; i < count; ++i)
    {
        cal_iou(cache_veh_infos_.veh_cache_infos[a].veh_infos[a_s_i + i].location,
                cache_veh_infos_.veh_cache_infos[b].veh_infos[b_s_i + i].location, c1, c2);
        tc += c1;
    }
    return tc / (float)count > 0.99;
}

// 消失车辆处理
void
VehicleInOutProcess::VehDisProcess(CapControlMsgVec &cap_control, std::vector<int> cache_veh_dis_ind,
                                   VehInOutInfoVec &veh_inout_info_vec)
{
    std::sort(cache_veh_dis_ind.rbegin(), cache_veh_dis_ind.rend());

    for (int ind : cache_veh_dis_ind)
    {
        if (!cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear)
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear = true;
            IsDisAbnormal(ind);
            if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear_abnormal)
            {
                for (int i = 0; i < cache_veh_infos_.veh_cache_infos.size(); i++)
                {
                    if (i != ind)
                    {
                        if (CalculateOcclusionRateForCache(cache_veh_infos_, ind, i))
                        {
                            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.obscured = true;
                            break;
                        }
                    }
                }
            }

            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.veh_disappear_timestamp =
                cache_veh_infos_.frame_timestamp;
            continue;
        }

        bool ret_dis =
            time_compare(cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.veh_disappear_timestamp,
                         cache_veh_infos_.frame_timestamp, std::max(360, ((int)PARKING_EMPTY_CONTINUE_TIME)) + 60);
        if (ret_dis || cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.to_delete)
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.to_delete = true;
            int ret1_dis = ClearVehInInfo(cap_control, ind);
            if (ret1_dis != SL_SUCCESS)
            {
                SonLiLogger::getInstance()
                    .getLogger(SonLiLogger::RotateLogger)
                    ->info("ClearVehInInfo failed ! track_id {}", cache_veh_infos_.veh_cache_infos[ind].track_id);
            }
            continue;
        }

        ret_dis = time_compare(cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.veh_disappear_timestamp,
                               cache_veh_infos_.frame_timestamp, 60);

        if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_report_in &&
            !cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_report_out)
        {
            // 有进场没出场
            if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_leave)
            {
                continue;
            }

            if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_out_flag < 0)
            {
                bool ret_out_jug =
                    time_compare(cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.veh_disappear_timestamp,
                                 cache_veh_infos_.frame_timestamp, 5);
                if (ret_out_jug)
                {
                    IsSatisfyOut(ind);
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_out_flag = 1;
                }
                else
                {
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_out = -1;
                }
            }

            if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_out_flag >= 1 &&
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_out > 0)
            {
                if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_out_flag != 2)
                {
                    bool ret_out_jug =
                        time_compare(cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.veh_disappear_timestamp,
                                     cache_veh_infos_.frame_timestamp, 10);
                    if (ret_out_jug)
                    {
                        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_out = -1;
                        IsSatisfyOut(ind);
                        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_out_flag = 2;
                    }
                }

                if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_out_flag == 2 &&
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_out > 0)
                {

                    if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_pic_id < 0)
                    {
                        int ret11 = UpdateCacheVehSendMessage(cap_control, ind, SL_CAR_STATE_STOP_LEAVE);
                        if (ret11 != SL_SUCCESS)
                        {
                            SonLiLogger::getInstance()
                                .getLogger(SonLiLogger::RotateLogger)
                                ->info(
                                    "VehDisProcess UpdateCacheVehSendMessage failed ! track_id {}, channel {}, line {}",
                                    cache_veh_infos_.veh_cache_infos[ind].track_id, cache_veh_infos_.camera_channel,
                                    __LINE__);
                        }
                        continue;
                    }

                    if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_pic_id < 0)
                    {
                        int ret11 = UpdateCacheVehSendMessage(cap_control, ind, SL_CAR_STATE_LEAVING);
                        if (ret11 != SL_SUCCESS)
                        {
                            SonLiLogger::getInstance()
                                .getLogger(SonLiLogger::RotateLogger)
                                ->info(
                                    "VehDisProcess UpdateCacheVehSendMessage failed ! track_id {}, channel {}, line {}",
                                    cache_veh_infos_.veh_cache_infos[ind].track_id, cache_veh_infos_.camera_channel,
                                    __LINE__);
                        }
                        continue;
                    }

                    if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_pic_id < 0)
                    {
                        int ret1 = UpdateCacheVehSendMessage(cap_control, ind, SL_CAR_STATE_OUT);
                        if (ret1 != SL_SUCCESS)
                        {
                            SonLiLogger::getInstance()
                                .getLogger(SonLiLogger::RotateLogger)
                                ->info(
                                    "VehDisProcess UpdateCacheVehSendMessage failed ! track_id {}, channel {}, line {}",
                                    cache_veh_infos_.veh_cache_infos[ind].track_id, cache_veh_infos_.camera_channel,
                                    __LINE__);
                        }
                        VehOutProUpload(cap_control, ind);
                    }
                    else
                    {
                        VehOutProUpload(cap_control, ind);
                    }
                }
                else
                {
                    if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_out_flag == 2 &&
                        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_out == 0)
                    {
                        // PrintInSpaceVehInfo();
                        // PrintCacheVehInfo();
                        // PrintNowVehInfo();
                    }
                }
            }
            if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_out_flag > 0 &&
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_out == 0)
            {
                // if (ret_dis) {
                // 	cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.to_delete = true;
                // }
                ResetVehInInfo(cap_control, ind);
                ResetVehOutInfo(cap_control, ind);

                bool ret_dis_40 =
                    time_compare(cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.veh_disappear_timestamp,
                                 cache_veh_infos_.frame_timestamp, 40);
                if (ret_dis_40)
                {
                    int cache_veh_in_index_del = -1;
                    bool park_out = ParkVehOut(ind, cache_veh_infos_, cache_veh_in_infos_, veh_inout_info_vec,
                                               space_info_, cache_veh_in_index_del);

                    if (park_out)
                    {
                        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.to_delete = true;
                        DeleteSpaceVeh(cache_veh_in_index_del); // 处理 cache_veh_in_infos_
                    }
                }
            }
        }
        else if ((!cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_report_in) &&
                 (!cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_report_out) &&
                 (!cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_standstill) &&
                 cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str != "-" &&
                 cache_veh_infos_.veh_cache_infos[ind]
                         .veh_infos[cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() - 1]
                         .veh_chassis.space_res_ex == SPACE_IN)
        {
            // 没进场 没出场 没停稳 有车牌

            if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_in < 0 && !stability_enable_)
            {
                bool ret_in_jug =
                    time_compare(cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.veh_disappear_timestamp,
                                 cache_veh_infos_.frame_timestamp, 6);
                if (ret_in_jug)
                {
                    IsSatisfyIn(ind);
                }
            }

            if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_in > 0)
            {
                // 缓存入场停稳图片
                int ret_sendin;
                ret_sendin = UpdateCacheVehSendMessage(cap_control, ind, SL_CAR_STATE_STOP);
                if (ret_sendin != SL_SUCCESS)
                {
                    SonLiLogger::getInstance()
                        .getLogger(SonLiLogger::RotateLogger)
                        ->info("VehDisProcess UpdateCacheVehSendMessage failed ! track_id {}, channel {}, line {}",
                               cache_veh_infos_.veh_cache_infos[ind].track_id, cache_veh_infos_.camera_channel,
                               __LINE__);
                }
                VehInProUpload(ind);
            }
            else
            {
                if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_in == 0)
                {
                    if (ret_dis)
                    {
                        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.to_delete = true;
                    }
                    ResetVehInInfo(cap_control, ind);
                    ResetVehOutInfo(cap_control, ind);
                }
            }
        }
        else
        {
            if (!cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_report_in &&
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_standstill)
            {
            }
            else
            {
                if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_report_out &&
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear)
                {
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.to_delete = true;
                }
                else
                {
                    ResetVehInInfo(cap_control, ind);
                    ResetVehOutInfo(cap_control, ind);
                }
            }

            if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_standstill ||
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_report_in ||
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_report_out ||
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear_abnormal)
            {
                if (ret_dis)
                {
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.to_delete = true;
                }
            }
            else
            {
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.to_delete = true;
            }
        }
    }

    int cache_veh_in_index_del2 = -1;
    bool park_out2 =
        ParkVehOut2(cache_veh_infos_, cache_veh_in_infos_, veh_inout_info_vec, space_info_, cache_veh_in_index_del2);

    if (park_out2)
    {
        DeleteSpaceVeh(cache_veh_in_index_del2); // 处理 cache_veh_in_infos_
    }
}

} // namespace sonli

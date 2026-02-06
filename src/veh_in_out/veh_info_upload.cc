#include "veh_info_upload.h"
#include "../models_processor/char_code.h"
#include "../models_processor/model_process.h"
#include "../utils/sl_logger.h"
#include "error.h"
#include "trend_acc.h"
#include "veh_info_copy.h"

namespace sonli
{

void
VehInfoUpload::IsPassingVehIn(int ind, const std::vector<std::pair<float, int>>& iou_rat_vec)
{

    for (int i = 0; i < iou_rat_vec.size(); i++)
    {
        int same_loc_veh_i_t = iou_rat_vec[i].second;
        float max_iou_rat_t = iou_rat_vec[i].first;
        if (max_iou_rat_t > 0.7)
        {
            bool is_dis = cache_veh_infos_.veh_cache_infos[same_loc_veh_i_t].veh_in_out_var.is_disappear;
            if (is_dis)
            {
                bool veh_time_ch;
                unsigned long long time_1 =
                    cache_veh_infos_.veh_cache_infos[same_loc_veh_i_t].veh_in_out_var.veh_disappear_timestamp;
                unsigned long long time_2 =
                    cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.veh_timestamp;
                if (time_1 > time_2)
                {
                    veh_time_ch = time_compare(time_2, time_1, 20);
                }
                else
                {
                    veh_time_ch = time_compare(time_1, time_2, 20);
                }
                if (!veh_time_ch)
                {

                    std::string lp_ind_str = cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str;
                    int lp_count = cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_count;
                    float vlp_rec_confidence =
                        cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.vlp_rec_confidence;
                    if (lp_ind_str == "-")
                    {
                        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in = true;
                        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag = 901;
                        break;
                    }

                    if (lp_ind_str == cache_veh_infos_.veh_cache_infos[same_loc_veh_i_t].lp_info.lp_number.lp_str)
                    {
                        continue;
                    }

                    if (cache_veh_infos_.veh_cache_infos[same_loc_veh_i_t].veh_in_out_var.is_disappear_abnormal)
                    {
                        if (cache_veh_infos_.veh_cache_infos[same_loc_veh_i_t].veh_in_out_var.is_satisfy_out < 0)
                        {
                            int is_out_s = IsSatisfyOut(same_loc_veh_i_t);
                        }
                        if (cache_veh_infos_.veh_cache_infos[same_loc_veh_i_t].veh_in_out_var.is_satisfy_out == 0)
                        {
                            cache_veh_infos_.veh_cache_infos[same_loc_veh_i_t].veh_in_out_var.is_satisfy_out = -1;
                            if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_move_count < 1)
                            {
                                // cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in = true;
                                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag = 911;
                            }
                            else
                            {
                                if (lp_count < 3 || vlp_rec_confidence < 0.9)
                                {
                                    // cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in = true;
                                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag = 921;
                                }
                                else
                                {
                                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag = 931;
                                }
                            }

                            break;
                        }
                        else
                        {
                            cache_veh_infos_.veh_cache_infos[same_loc_veh_i_t].veh_in_out_var.is_satisfy_out = -1;
                        }
                    }
                }
            }
            else
            {
                if (!cache_veh_infos_.veh_cache_infos[same_loc_veh_i_t].veh_in_out_var.is_standstill &&
                    cache_veh_infos_.veh_cache_infos[same_loc_veh_i_t].lp_info.lp_number.lp_str != "-" &&
                    cache_veh_infos_.veh_cache_infos[same_loc_veh_i_t].lp_info.lp_number.lp_str ==
                        cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str)
                {
                    continue;
                }

                if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_move_count < 1)
                {
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in = true;
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag = 941;
                }
                else
                {
                    // cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in = true;
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag = 951;
                }

                break;
            }
        }
        else
        {
            break;
        }
    }
}

bool
VehInfoUpload::IsSendVehInInfo(int ind)
{

    int space_id_ind = cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.veh_chassis.space_id;

    int same_lp_i = IsHaveSend(ind);
    if (same_lp_i > -1)
    {
        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in = true;
        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag = 101;
        return false;
    }

    if (!JudgeImageTime(ind, 1))
    {
        if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_rat < 0.35)
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in = true;
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag = 211;
            return false;
        }
        else
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in = true;
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag = 201;
            return false;
        }
    }

    std::string lp_str_ind = cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.lp_str;
    float vlp_rec_confidence =
        cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.vlp_rec_confidence;
    int lp_count = cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.lp_count;

    if (cache_veh_infos_.use_reid)
    {
        cv::Mat standstill_feature = cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.feature;
        for (int i = 0; i < cache_veh_in_infos_.size(); i++)
        {
            if (cache_veh_in_infos_[i].veh_info.veh_track_id == cache_veh_infos_.veh_cache_infos[ind].track_id)
            {
                continue;
            }

            cv::Mat veh_in_feature = cache_veh_in_infos_[i].veh_info.feature;
            cv::Mat AB = standstill_feature * veh_in_feature.t();
            float feature_cos = AB.at<float>(0, 0);

            if (feature_cos > 0.9)
            {
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in = true;
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag = 301;

                return false;
            }
        }
    }

    if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_rat < 0.5)
    {
        int time_c_now_appear = cache_veh_infos_.frame_timestamp -
                                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.veh_appear_timestamp;
        if (time_c_now_appear > 10 * 60000)
        {
            if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_move_count < 5)
            {
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in = true;
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag = 1001;
                return false;
            }
            else
            {
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in = true;
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag = 1011;
                return false;
            }
        }
    }

    // 查看之前是否有停稳车辆  no_send_in  true . false
    // 有车牌 车辆没有驶入过程  未消失
    if (lp_str_ind != "-" && !cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear)
    {

        if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_move_count < 2)
        {
            if (vlp_rec_confidence < 0.9 || lp_count < 3)
            {
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in = true;
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag = 401;

                return false;
            }

            // int veh_fraction = GetVehStateFraction(cache_veh_infos_.veh_cache_infos[ind].veh_infos.back(), ind);
            // if (cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_state_fraction < 95 ||
            // 	cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_state_fraction < 95 ||
            // 	cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_state_fraction < 95 ||
            // 	veh_fraction < 95)
            // {
            // 	cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in = true;
            // 	cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag = 501;

            // 	return false;
            // }

            int lp_dis = 8;
            for (int i = 0; i < cache_veh_in_infos_.size(); i++)
            {

                if (cache_veh_in_infos_[i].veh_info.lp_info.lp_number.lp_str == "-")
                {
                    continue;
                }
                std::wstring lp_strw_1 = utf8ToUnicode(lp_str_ind);
                std::wstring lp_strw_2 = utf8ToUnicode(cache_veh_in_infos_[i].veh_info.lp_info.lp_number.lp_str);
                int lp_dis_temp = 0;
                lp_dis_temp = wstr_distance(lp_strw_1, lp_strw_2);

                if (lp_dis_temp < lp_dis)
                {
                    lp_dis = lp_dis_temp;
                }

                if (lp_dis <= 1)
                {
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in = true;
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag = 601;
                    return false;
                }
            }

            for (int i = 0; i < cache_veh_infos_.veh_cache_infos.size(); i++)
            {
                if (cache_veh_infos_.veh_cache_infos[i].lp_info.lp_number.lp_str == "-" || i == ind ||
                    !cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_report_in ||
                    cache_veh_infos_.veh_cache_infos[i].veh_infos.back().cover_rat < 0.5)
                {
                    continue;
                }

                int space_id_i = cache_veh_infos_.veh_cache_infos[i].veh_standstill_mess.veh_info.veh_chassis.space_id;
                if (space_id_i == space_id_ind)
                {
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in = true;
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag = 701;

                    return false;
                }
            }
        }

        int same_lp_i = VehBaseInfo::IsHaveSameLpVehIn(ind, lp_str_ind, 1);
        if (same_lp_i > -1)
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in = true;
            LPInfo lp_info_tmp;
            cache_veh_infos_.veh_cache_infos[ind].lp_info = lp_info_tmp;
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag = 801;
            return false;
        }

        if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in &&
            !cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear && progress_start_count_ > 3000 &&
            IsLpNumberOkay(lp_count, vlp_rec_confidence))
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in = false;
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag = 8;
        }
    }

    // 判断相同位置或车牌号是否有未出场的车辆
    std::vector<std::pair<float, int>> iou_rat_vec;
    float max_iou_rat = 0.0;
    int same_loc_veh_i = IsHaveSameLocVeh(ind, max_iou_rat, iou_rat_vec, 2, false);
    // same_loc_veh_i = IsHaveSameLocVeh(ind, max_iou_rat, iou_rat_vec, 1, false);
    std::sort(iou_rat_vec.begin(), iou_rat_vec.end(), cmp_first);

    // 对侧 过路车影响
    if (!cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear &&
        !cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in && space_id_ind > -1 &&
        space_info_.space_gradient.size() > space_id_ind && space_info_.space_gradient[space_id_ind] < 0.8 &&
        space_info_.space_gradient[space_id_ind] > 0.05)
    {
        IsPassingVehIn(ind, iou_rat_vec);
        if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->info("IsPassingVehIn no_send_in. channel {}, track_id {}, lp {}", cache_veh_infos_.camera_channel,
                       cache_veh_infos_.veh_cache_infos[ind].track_id,
                       cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str);
            return false;
        }
    }

    // if (!cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in &&
    // 	cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear) {
    // 	//针对消失车辆，停稳位置没车、没遮挡 则不上报
    // 	if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_out < 0) {
    // 		bool is_out_s = IsSatisfyOut(ind);
    // 	}

    // 	if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_out < 1) {
    // 		float max_iou_rat_te = 0.0;
    // 		for (int i = 0; i < iou_rat_vec.size(); i++) {
    // 			int same_loc_veh_i_t = iou_rat_vec[i].second;
    // 			float max_iou_rat_t = iou_rat_vec[i].first;
    // 			if (max_iou_rat_t >= 0.6) {
    // 				if (!cache_veh_infos_.veh_cache_infos[same_loc_veh_i_t].veh_in_out_var.is_disappear) {
    // 					max_iou_rat_te = max_iou_rat_t;
    // 				}
    // 			}
    // 			else {
    // 				break;
    // 			}
    // 		}

    // 		if (max_iou_rat_te < 0.6 &&
    // 			cache_veh_infos_.veh_cache_infos[ind].veh_infos[cache_veh_infos_.veh_cache_infos[ind].veh_infos.size()
    // - 1].cover_rat < 0.5)
    // 		{
    // 			cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in = true;
    // 			cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag = 8001;
    // 		}
    // 	}
    // 	cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_out = -1;
    // }

    if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in &&
        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_move_count < 2)
    {

        if (progress_start_count_ < 800)
        {
            if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag < 0)
            {
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag = 10001;
            }
            return false;
        }
    }

    if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_move_count < 2 &&
        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear)
    {
        return false;
    }

    max_iou_rat = 0.35;
    for (int i = 0; i < iou_rat_vec.size(); i++)
    {
        int same_loc_veh_i_t = iou_rat_vec[i].second;
        float max_iou_rat_t = iou_rat_vec[i].first;
        if (max_iou_rat_t > 0.4)
        {
            bool is_dis = cache_veh_infos_.veh_cache_infos[same_loc_veh_i_t].veh_in_out_var.is_disappear;
            bool is_disappear_abnormal =
                cache_veh_infos_.veh_cache_infos[same_loc_veh_i_t].veh_in_out_var.is_disappear_abnormal;
            //|| is_disappear_abnormal
            if (!is_dis)
            {
                max_iou_rat = max_iou_rat_t;
                same_loc_veh_i = same_loc_veh_i_t;
                break;
            }
        }
        else
        {
            break;
        }
    }

    if (lp_str_ind == "-")
    {
        // 无车牌号
        if (cache_veh_infos_.veh_cache_infos[ind].veh_locs_first_n.size() > 0 &&
            cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_locs.size() > 0 &&
            cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() > 10 &&
            // max_iou_rat <= 0.4 &&
            // cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_state_fraction > 90 &&
            // cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_state_fraction > 90 &&
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_move_count >= 10 &&
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_count >= 30)
        {

            // int veh_infos_size = cache_veh_infos_.veh_cache_infos[ind].veh_infos.size();
            // float avg_space_rat = 0.0;
            // for (int i = 0; i < 10; i++) {
            // 	avg_space_rat += cache_veh_infos_.veh_cache_infos[ind].veh_infos[veh_infos_size - 1 -
            // i].veh_chassis.space_rat;
            // }
            // avg_space_rat = avg_space_rat / 10;

            // if (avg_space_rat < 0.4) {
            // 	cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in = true;
            // 	cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag = 9013;
            // 	return false;
            // }

            float pIOU = 1.0;
            float carRat1 = 1.0;
            float carRat2 = 1.0;

            pIOU = cal_iou(cache_veh_infos_.veh_cache_infos[ind].veh_locs_first_n[0],
                           cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_locs[0], carRat1, carRat2);
            if (pIOU > 0.6)
            {
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in = true;
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag = 9001;
                return false;
            }
            for (int i = cache_veh_in_infos_.size() - 1; i >= 0; i--)
            {
                if (cache_veh_in_infos_[i].veh_info.veh_chassis.space_id == space_id_ind &&
                    cache_veh_in_infos_[i].veh_info.lp_info.lp_number.lp_str != "-" &&
                    !cache_veh_in_infos_[i].no_send_in)
                {

                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in = true;
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag = 9011;
                    return false;
                }
            }

            if (max_iou_rat > 0.4 ||
                cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_state_fraction <= 90 ||
                cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_state_fraction <= 90)
            {
                SonLiLogger::getInstance()
                    .getLogger(SonLiLogger::RotateLogger)
                    ->info("nolp not in, channel {}, space_id {}, track_id {}, max_iou_rat {}, fraction_pro {}, "
                           "fraction_sts {}",
                           cache_veh_infos_.camera_channel,
                           cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_chassis.space_id,
                           cache_veh_infos_.veh_cache_infos[ind].track_id, max_iou_rat,
                           cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_state_fraction,
                           cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_state_fraction);

                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in = true;
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag = 9013;
                return false;
            }
        }
        else
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in = true;
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in_flag = 9012;
            return false;
        }
    }
    else
    {
        // 有车牌
        return true;
    }
    return true;
}

bool
VehInfoUpload::IsSendVehNOtDisOutInfo(CapControlMsgVec &cap_control, int ind)
{
    if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() < 1)
    {
        return false;
    }

    bool is_dis = cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear;
    bool is_satisfy_out = false;

    if (!is_dis)
    {
        // 车辆未消失

        if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_count < 1)
        {

            // if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_move_count <= out_move_count_conf_)
            {
                return false;
            }
        }
        else
        {
            // cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_move_count < out_move_count_conf_ ||
            if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_count < out_count_conf_ * 10)
            {
                return false;
            }
        }

        std::string lp_str_ind = cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str;
        std::string lp_str_in_send =
            cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.lp_info.lp_number.lp_str;

        if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_count < 1 &&
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_move_count > out_move_count_conf_)
        {
            std::string lp_str_now = "-";
            int lp_count_now = 0;
            float lp_confidence_now = 0.0;

            std::vector<SLLPColor_t> lp_color_vec;
            std::vector<std::pair<std::string, float>> lp_str_vec =
                GetVehLpStrVec(cache_veh_infos_.veh_cache_infos[ind].veh_infos, lp_color_vec);
            LpStrCount(lp_str_vec, lp_count_now, lp_confidence_now, lp_str_now);

            int space_id_stand =
                cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_chassis.space_id;
            int space_id_now = cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_id;

            std::vector<float> space_rat_vec_stand =
                cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_chassis.space_rat_vec;
            std::vector<float> space_rat_vec_now =
                cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_rat_vec;

            if (space_id_stand >= space_rat_vec_stand.size() || space_id_stand >= space_rat_vec_now.size() ||
                space_id_now >= space_rat_vec_stand.size() || space_id_now >= space_rat_vec_now.size())
            {
                return false;
            }

            if (space_id_stand < 0 || space_id_now < 0)
            {
                return false;
            }

            if (space_id_stand == space_id_now)
            {
                return false;
            }

            float pre_space_stand_id_rat = space_rat_vec_stand[space_id_stand];
            float pre_space_now_id_rat = space_rat_vec_stand[space_id_now];
            float now_space_stand_id_rat = space_rat_vec_now[space_id_stand];
            float now_space_now_id_rat = space_rat_vec_now[space_id_now];

            GetOutVehMaxIou(ind);

            // 车位变换
            // 判断当前位置和停稳位置
            if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_res != SPACE_IN)
            {
                return false;
            }

            if (lp_count_now <= 4 || lp_confidence_now <= 0.95)
            {
                return false;
            }

            bool is_same_send_in_lp = false;
            bool have_in_lp = false;
            int space_id_have_lp = -1;

            if (lp_str_now != "-")
            {
                if (lp_str_now == lp_str_in_send)
                {
                    is_same_send_in_lp == true;
                }
                for (int i = 0; i < cache_veh_in_infos_.size(); i++)
                {
                    if (!cache_veh_in_infos_[i].no_send_in &&
                        cache_veh_in_infos_[i].veh_info.lp_info.lp_number.lp_str == lp_str_now)
                    {
                        have_in_lp = true;
                        space_id_have_lp = cache_veh_in_infos_[i].veh_info.veh_chassis.space_id;
                        break;
                    }
                }
            }
            else
            {
                return false;
            }

            if (have_in_lp || is_same_send_in_lp)
            {

                if (space_id_have_lp > -1 && space_id_stand != space_id_have_lp)
                {
                    return false;
                }

                if (space_id_have_lp > -1 && space_id_now == space_id_have_lp)
                {
                    return false;
                }

                int veh_infos_size = cache_veh_infos_.veh_cache_infos[ind].veh_infos.size();
                int size_conf = 20;
                if (veh_infos_size < size_conf)
                {
                    return false;
                }
                else
                {
                    for (int ik = 0; ik < size_conf; ik++)
                    {
                        int s_id = cache_veh_infos_.veh_cache_infos[ind]
                                       .veh_infos[veh_infos_size - 1 - ik]
                                       .veh_chassis.space_id;
                        if (s_id == space_id_stand)
                        {
                            return false;
                        }
                    }
                }

                if (now_space_now_id_rat < 0.05)
                {
                    return false;
                }
                float conf = (now_space_now_id_rat + pre_space_stand_id_rat) / 6.0;
                if (now_space_now_id_rat - pre_space_now_id_rat > conf ||
                    pre_space_stand_id_rat - now_space_stand_id_rat > conf ||
                    now_space_now_id_rat - now_space_stand_id_rat > 0.6)
                {

                    is_satisfy_out = true;
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_max_iou = 0.0;

                    return true;
                }
            }
            else
            {
                if ((lp_count_now > 4 && lp_confidence_now > 0.9) || (lp_count_now > 9 && lp_confidence_now > 0.8))
                {

                    if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_max_iou > 0.5)
                    {
                        ReplaceVehInfo(cap_control, ind);
                        return false;
                    }
                }
            }
        }
        else
        {

            if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_count < out_count_conf_)
            {
                return false;
            }

            if (lp_str_ind != "-")
            {
                bool have_in_lp = false;
                for (int i = 0; i < cache_veh_in_infos_.size(); i++)
                {
                    if (!cache_veh_in_infos_[i].no_send_in &&
                        cache_veh_in_infos_[i].veh_info.lp_info.lp_number.lp_str == lp_str_ind)
                    {
                        have_in_lp = true;
                    }
                }

                int veh_state_fraction = 100;
                float avg_cover_rat = 0.0;
                int space_id_ind =
                    cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_chassis.space_id;
                if (space_id_ind < 0)
                {
                    space_id_ind = 0;
                }
                if (space_info_.space_gradient.size() > static_cast<unsigned int>(space_id_ind) &&
                    space_info_.space_gradient[space_id_ind] < 0.8 && space_info_.space_gradient[space_id_ind] > 0.05)
                {
                    veh_state_fraction =
                        GetVehStateFraction(cache_veh_infos_.veh_cache_infos[ind].veh_infos.back(), ind);
                    avg_cover_rat = GetVehAvgCoverRat(cache_veh_infos_.veh_cache_infos[ind].veh_infos.back(), ind);

                    // SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
                    // 	"IsSendVehNOtDisOutInfo veh_state_fraction ! track_id {}, channel {}, lp_str {},
                    // veh_state_fraction {}, avg_cover_rat {}", 	cache_veh_infos_.veh_cache_infos[ind].track_id,
                    // 	cache_veh_infos_.camera_channel,
                    // 	lp_str_ind,
                    // 	veh_state_fraction,
                    // 	avg_cover_rat);
                }

                if (have_in_lp && veh_state_fraction > 93 && avg_cover_rat < 0.5)
                {
                    GetOutVehMaxIou(ind);
                    if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_max_iou < 0.3)
                    {
                        return true;
                    }
                }
            }
        }
    }

    return is_satisfy_out;
}

bool
VehInfoUpload::IsSendNoLpVehOutInfo(int ind)
{
    if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() < 1 ||
        cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_state_fraction < 80)
    {
        return false;
    }
    if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_max_iou > 0.5)
    {
        return false;
    }
    // if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in)
    // {
    // 	return false;
    // }

    return true;
}

bool
VehInfoUpload::IsSendVehOutInfoTwice(int ind)
{
    bool is_send = true;
    std::vector<int> need_del_inds;
    std::string tem_lp = cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number.lp_str;

    int send_veh_id = cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.send_veh_id;

    for (unsigned int i = 0; i < cache_veh_out_infos_.size(); i++)
    {
        bool ret_time =
            time_compare(cache_veh_out_infos_[i].report_out_timestamp, cache_veh_infos_.frame_timestamp, 60);
        if (ret_time)
        {
            need_del_inds.push_back(i);
            continue;
        }
        if (send_veh_id == cache_veh_out_infos_[i].veh_info.send_veh_id)
        {
            is_send = false;
        }
        if (tem_lp != "-")
        {
            if (tem_lp == cache_veh_out_infos_[i].veh_info.lp_info.lp_number.lp_str)
            {
                is_send = false;
                for (int i = 0; i < cache_veh_in_infos_.size(); i++)
                {
                    if (cache_veh_in_infos_[i].veh_info.lp_info.lp_number.lp_str == tem_lp &&
                        cache_veh_in_infos_[i].report_in_timestamp > 0)
                    {
                        bool ret_time_1 = time_compare(cache_veh_in_infos_[i].report_in_timestamp,
                                                       cache_veh_infos_.frame_timestamp, 60);
                        if (!ret_time_1)
                        {
                            is_send = true;
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            int space_id = cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.veh_chassis.space_id;
            if (cache_veh_out_infos_[i].veh_info.veh_chassis.space_id == space_id &&
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_combine > 0)
            {
                is_send = false;
            }
        }
    }

    for (int i = need_del_inds.size() - 1; i >= 0; i--)
    {
        cache_veh_out_infos_.erase(cache_veh_out_infos_.begin() + need_del_inds[i]);
    }

    if (is_send)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool
VehInfoUpload::IsSendLowConf(int ind)
{
    std::string lp_str_ind =
        cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number.lp_str;
    float vlp_rec_confidence =
        cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number.vlp_rec_confidence;
    int lp_count = cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number.lp_count;
    if (lp_str_ind == "-")
    {
        return true;
    }

    if (vlp_rec_confidence < 0.9 && lp_count < 6)
    {
        bool is_send = false;
        std::wstring veh_out_lp_strw = utf8ToUnicode(lp_str_ind);
        for (int i = 0; i < cache_veh_in_infos_.size(); i++)
        {
            std::wstring veh_in_lp_strw = utf8ToUnicode(cache_veh_in_infos_[i].veh_info.lp_info.lp_number.lp_str);
            int dis = 0;
            dis = wstr_distance(veh_in_lp_strw, veh_out_lp_strw);

            if (dis <= 1 && cache_veh_in_infos_[i].no_send_in)
            {
                return true;
            }
        }
        return is_send;
    }

    return true;
}

void
VehInfoUpload::ReplaceVehInfo(CapControlMsgVec &cap_control, int ind)
{

    ResetVehInInfo(cap_control, ind);
    ResetVehOutInfo(cap_control, ind);

    // cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var_ptr.reset();
    // cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.reset();
    // cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.reset();
    // cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.reset();
    // cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.reset();

    VehInOutVar veh_in_out_var_tmp;
    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var = veh_in_out_var_tmp;
    VehSendMessage veh_mess_tmp;
    cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess = veh_mess_tmp;
    cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess = veh_mess_tmp;
    cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess = veh_mess_tmp;
    cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess = veh_mess_tmp;
}

void 
VehInfoUpload::EvidenceChainConfirm(int ind, int inout, std::string order_lp_str){
    if(inout == 1){
        if(order_lp_str == "-"){
            if(cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.lp_info.lp_number.lp_str != "-"){
                cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.lp_info.location = Rect(0, 0, 5, 5);
                // cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.location = Rect(0, 0, 20, 20);

                // cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.veh_det_confidence = 0.0;
                cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.lp_info.vlp_det_confidence = 0.0;
                cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.lp_info.lp_number.vlp_rec_confidence = 0.0;

                SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                            "order_lp_str null, veh_inpro_out_mess inout {}, lpstr {}" , 
                            inout,
                            cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.lp_info.lp_number.lp_str);
                
            }
            if(cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.lp_str != "-"){
                cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.location = Rect(0, 0, 5, 5);
                // cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.location = Rect(0, 0, 20, 20);

                // cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.veh_det_confidence = 0.0;
                cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.vlp_det_confidence = 0.0;
                cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.vlp_rec_confidence = 0.0;
                SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                            "order_lp_str null, veh_inpro_pro_mess inout {}, lpstr {}" , 
                            inout,
                            cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.lp_str);
            }
            // if(cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.lp_info.lp_number.lp_str != "-"){
            //     cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.lp_info.location = Rect(0, 0, 5, 5);
            //     cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.location = Rect(0, 0, 20, 20);

            //     cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_det_confidence = 0.0;
            //     cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.lp_info.vlp_det_confidence = 0.0;
            //     cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.lp_info.lp_number.vlp_rec_confidence = 0.0;
            //     SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
            //                 "order_lp_str null, veh_standstill_mess inout {}, lpstr {}" , 
            //                 inout,
            //                 cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.lp_info.lp_number.lp_str);
            // }
        }
        else{
            std::wstring order_lp_str_strw = utf8ToUnicode(order_lp_str);

            if (cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.lp_info.lp_number.lp_str != "-")
            {
                if (cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.lp_info.lp_number.lp_str != order_lp_str)
                {
                    std::wstring lp_str_strw = utf8ToUnicode(cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.lp_info.lp_number.lp_str);
                    int dis = wstr_distance(lp_str_strw, order_lp_str_strw);
                    if (dis > 2)
                    {
                        cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.lp_info.location = Rect(0, 0, 5, 5);
                        // cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.location = Rect(0, 0, 20, 20);

                        // cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.veh_det_confidence = 0.0;
                        // cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.lp_info.vlp_det_confidence = 0.0;
                        // cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.lp_info.lp_number.vlp_rec_confidence = 0.0;
                        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                            "order_lp_str {}, veh_inpro_out_mess inout {}, lpstr {}" , 
                            order_lp_str,
                            inout,
                            cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.lp_info.lp_number.lp_str);
                    }
                }
            }
            else
            {
                cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.lp_info.location = Rect(0, 0, 5, 5);
            }

            if (cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.lp_str != "-")
            {
                if (cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.lp_str != order_lp_str)
                {
                    std::wstring lp_str_strw = utf8ToUnicode(cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.lp_str);
                    int dis = wstr_distance(lp_str_strw, order_lp_str_strw);
                    if (dis > 2)
                    {
                        cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.location = Rect(0, 0, 5, 5);
                        // cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.location = Rect(0, 0, 20, 20);

                        // cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.veh_det_confidence = 0.0;
                        // cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.vlp_det_confidence = 0.0;
                        // cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.vlp_rec_confidence = 0.0;
                        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                            "order_lp_str {}, veh_inpro_pro_mess inout {}, lpstr {}" , 
                            order_lp_str,
                            inout,
                            cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.lp_str);
                    }
                }
            }
            else
            {
                cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.location = Rect(0, 0, 5, 5);
            }

            // if (cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.lp_info.lp_number.lp_str != "-")
            // {
            //     if (cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.lp_info.lp_number.lp_str != order_lp_str)
            //     {
            //         std::wstring lp_str_strw = utf8ToUnicode(cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.lp_info.lp_number.lp_str);
            //         int dis = wstr_distance(lp_str_strw, order_lp_str_strw);
            //         if (dis > 2)
            //         {
            //             cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.lp_info.location = Rect(0, 0, 5, 5);
            //             cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.location = Rect(0, 0, 20, 20);

            //             cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_det_confidence = 0.0;
            //             cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.lp_info.vlp_det_confidence = 0.0;
            //             cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.lp_info.lp_number.vlp_rec_confidence = 0.0;
            //             SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
            //                 "order_lp_str {}, veh_standstill_mess inout {}, lpstr {}" , 
            //                 order_lp_str,
            //                 inout,
            //                 cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.lp_info.lp_number.lp_str);
            //         }
            //     }
            // }
            // else
            // {
            //     cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.lp_info.location = Rect(0, 0, 5, 5);
            // }

        }
    }
    else if(inout == 0){
        if(order_lp_str == "-"){
            if(cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_info.lp_info.lp_number.lp_str != "-"){
                cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_info.lp_info.location = Rect(0, 0, 5, 5);
                // cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_info.location = Rect(0, 0, 20, 20);
                SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                    "order_lp_str null, veh_outpro_std_mess inout {}, lpstr {}" , 
                    inout,
                    cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_info.lp_info.lp_number.lp_str);
            }
            if(cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number.lp_str != "-"){
                cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.location = Rect(0, 0, 5, 5);
                // cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.location = Rect(0, 0, 20, 20);
                SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                    "order_lp_str null, veh_outpro_pro_mess inout {}, lpstr {}" , 
                    inout,
                    cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number.lp_str);
            }
            // if(cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info.lp_info.lp_number.lp_str != "-"){
            //     cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info.lp_info.location = Rect(0, 0, 5, 5);
            //     cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info.location = Rect(0, 0, 20, 20);
            //     SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
            //         "order_lp_str null, veh_outpro_out_mess inout {}, lpstr {}" , 
            //         inout,
            //         cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info.lp_info.lp_number.lp_str);
            // }
        }
        else{
            std::wstring order_lp_str_strw = utf8ToUnicode(order_lp_str);

            if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_info.lp_info.lp_number.lp_str != "-")
            {
                if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_info.lp_info.lp_number.lp_str != order_lp_str)
                {
                    std::wstring lp_str_strw = utf8ToUnicode(cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_info.lp_info.lp_number.lp_str);
                    int dis = wstr_distance(lp_str_strw, order_lp_str_strw);
                    if (dis > 2)
                    {
                        cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_info.lp_info.location = Rect(0, 0, 5, 5);
                        // cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_info.location = Rect(0, 0, 20, 20);

                        // cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_info.veh_det_confidence = 0.0;
                        // cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_info.lp_info.vlp_det_confidence = 0.0;
                        // cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_info.lp_info.lp_number.vlp_rec_confidence = 0.0;
                        
                        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                            "order_lp_str {}, veh_outpro_std_mess inout {}, lpstr {}" , 
                            order_lp_str,
                            inout,
                            cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_info.lp_info.lp_number.lp_str);
                    }
                }
            }
            else
            {
                cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_info.lp_info.location = Rect(0, 0, 5, 5);
            }

            if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number.lp_str != "-")
            {
                if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number.lp_str != order_lp_str)
                {
                    std::wstring lp_str_strw = utf8ToUnicode(cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number.lp_str);
                    int dis = wstr_distance(lp_str_strw, order_lp_str_strw);
                    if (dis > 2)
                    {
                        cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.location = Rect(0, 0, 5, 5);
                        // cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.location = Rect(0, 0, 20, 20);

                        // cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.veh_det_confidence = 0.0;
                        // cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.vlp_det_confidence = 0.0;
                        // cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number.vlp_rec_confidence = 0.0;
                        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                            "order_lp_str {}, veh_outpro_pro_mess inout {}, lpstr {}" , 
                            order_lp_str,
                            inout,
                            cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number.lp_str);
                    }
                }
            }
            else
            {
                cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.location = Rect(0, 0, 5, 5);
            }

            // if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info.lp_info.lp_number.lp_str != "-")
            // {
            //     if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info.lp_info.lp_number.lp_str != order_lp_str)
            //     {
            //         std::wstring lp_str_strw = utf8ToUnicode(cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info.lp_info.lp_number.lp_str);
            //         int dis = wstr_distance(lp_str_strw, order_lp_str_strw);
            //         if (dis > 2)
            //         {
            //             cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info.lp_info.location = Rect(0, 0, 5, 5);
            //             cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info.location = Rect(0, 0, 20, 20);

            //             cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info.veh_det_confidence = 0.0;
            //             cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info.lp_info.vlp_det_confidence = 0.0;
            //             cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info.lp_info.lp_number.vlp_rec_confidence = 0.0;
            //             SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
            //                 "order_lp_str {}, veh_outpro_out_mess inout {}, lpstr {}" , 
            //                 order_lp_str,
            //                 inout,
            //                 cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info.lp_info.lp_number.lp_str);
            //         }
            //     }
            // }
            // else
            // {
            //     cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info.lp_info.location = Rect(0, 0, 5, 5);
            // }
        }
    }

}

// 驶入过程上传
void
VehInfoUpload::VehInProUpload(int ind)
{

    if (!cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_standstill)
    {
        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_standstill = true;
        cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.frame_timestamp = cache_veh_infos_.frame_timestamp;
        cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.frame_timestamp = cache_veh_infos_.frame_timestamp;
        cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.frame_timestamp = cache_veh_infos_.frame_timestamp;

        int space_id = VehInSpaceId(ind);
        if(space_id == -1)
        {
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
                "VehInProUpload space_id -1 ! track_id {}, channel {}, line {}, lp_str {}",
                cache_veh_infos_.veh_cache_infos[ind].track_id, 
                cache_veh_infos_.camera_channel, 
                __LINE__,
                cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str);
            return;
        }

        cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.veh_chassis.space_id =
            cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_chassis.space_id;
        cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.veh_chassis.space_rat_vec =
            cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_chassis.space_rat_vec;

        // VehSendMessage temp_mess;
        // VehSendMesCopy(temp_mess, cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess);

        if (cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_chassis.space_id < 0)
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.veh_chassis.space_id = 0;

        }
        else
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.veh_chassis.space_id =
                cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_chassis.space_id;
        }

        EvidenceChainConfirm(ind, 1, cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str);

        LpNumberCopy(cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number,
                     cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number);
        cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_color = cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_color;
        
        if (cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_color != SL_PLATE_COLOR_UNKNOWN)
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_status = PLATE_YES;
        }

        float veh_det_confidence_pro = cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.veh_det_confidence;
        if (veh_det_confidence_pro < 0.05)
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.location = Rect(0, 0, 5, 5);
            cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.location = Rect(0, 0, 20, 20);

            cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.veh_det_confidence = 0.0;
            cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.vlp_det_confidence = 0.0;
            cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.vlp_rec_confidence = 0.0;
        }

        return;
    }

    if (!cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.to_report_in &&
        !cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_report_in)
    {

        bool is_send = IsSendVehInInfo(ind);
        cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.veh_track_id = cache_veh_infos_.veh_cache_infos[ind].track_id;
        
        //有车牌 证据链中车辆都在边缘且都没有检测到车牌 画面下方
        if (cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.lp_info.vlp_det_confidence < 0.1 &&
            cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.vlp_det_confidence < 0.1 &&
            cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.lp_info.vlp_det_confidence < 0.1 &&
            cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.is_image_edge &&
            cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.is_image_edge &&
            cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.is_image_edge &&
            ((cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.location.y_ + 
                cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.location.height_) > 
                (cache_veh_infos_.veh_cache_infos[ind].frame_height * 0.6667)))
        {
            is_send = false;
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                    "----is_send set false---- lp_str {} conf {} lp_count {} track_id {}" , 
                    cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.lp_str,
                    cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.vlp_rec_confidence,
                    cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.lp_count,
                    cache_veh_infos_.veh_cache_infos[ind].track_id);
        }

        if (is_send && 
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_move_count < 2 &&
            cache_veh_infos_.veh_cache_infos[ind].last_lp_info.timestamp > 0 &&
            cache_veh_infos_.veh_cache_infos[ind].last_lp_info.lp_str != "-" &&
            cache_veh_infos_.veh_cache_infos[ind].last_lp_info.lp_str == 
                cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.lp_str)
        {
            // 判断上一次识别到车牌的时间
            if (cache_veh_infos_.frame_timestamp - cache_veh_infos_.veh_cache_infos[ind].last_lp_info.timestamp > 1000 * 300)
            {
                is_send = false;
                SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                    "----is_send set false--1-- lp_str {} conf {} lp_count {} track_id {} {} {}" , 
                    cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.lp_str,
                    cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.vlp_rec_confidence,
                    cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.lp_count,
                    cache_veh_infos_.veh_cache_infos[ind].track_id,
                    cache_veh_infos_.frame_timestamp,
                    cache_veh_infos_.veh_cache_infos[ind].last_lp_info.timestamp);

            }
        }

        if (is_send)
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.send_veh_id = send_veh_id_;
            send_veh_id_++;

            if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.in_move_count < 2)
            {
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.order_generate_type = 1;
            }
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in = false;
            AddSpaceVeh(ind);
        }
        else
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in = true;
        }

        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.to_report_in = true;
        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.report_in_timestamp = cache_veh_infos_.frame_timestamp;
    }

    return;
}

// 驶出过程上传
void
VehInfoUpload::VehOutProUpload(CapControlMsgVec &cap_control, int ind)
{

    if (!cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_leave)
    {
        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_leave = true;

        cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.frame_timestamp = cache_veh_infos_.frame_timestamp;
        cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.frame_timestamp = cache_veh_infos_.frame_timestamp;
        cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.frame_timestamp = cache_veh_infos_.frame_timestamp;

        return;
    }

    if (!cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.to_report_out &&
        !cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_report_out)
    {

        if (!cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear)
        {
            bool is_leave = VehOutStatus(ind);
            if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().cover_rat > 0.3)
            {
                if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_rat > 0.02)
                {
                    SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
                        "Veh not Dis track_id {}, lp_str {}, out_move_count {}, out_count {}, cover_rat {}, space_rat {}", 
                        cache_veh_infos_.veh_cache_infos[ind].track_id, 
                        cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str, 
                        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_move_count, 
                        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_count, 
                        cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().cover_rat, 
                        cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_rat);

                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_move_count = 0;
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_count = 0;
                }
            }
            else
            {
                if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_rat > 0.1)
                {
                    SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
                        "Veh not Dis track_id {}, lp_str {}, out_move_count {}, out_count {}, cover_rat {}, space_rat {}", 
                        cache_veh_infos_.veh_cache_infos[ind].track_id, 
                        cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str, 
                        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_move_count, 
                        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_count, 
                        cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().cover_rat, 
                        cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_rat);

                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_move_count = 0;
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_count = 0;
                }
            }

            if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_move_count == 0 &&
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.out_count == 0)
            {
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_leave = false;
                ResetVehInInfo(cap_control, ind);
                ResetVehOutInfo(cap_control, ind);
                return;
            }
        }
        else
        {
            if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_out_flag < 2)
            {
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_leave = false;
                return;
            }
        }

        // VehSendMessage temp_mess;

        // VehSendMesCopy(temp_mess, cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess);

        cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.veh_track_id = cache_veh_infos_.veh_cache_infos[ind].track_id;

        cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.veh_chassis.space_id =
            cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_chassis.space_id;
        if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.veh_chassis.space_id < 0)
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.veh_chassis.space_id =
                cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_info.veh_chassis.space_id;
        }
        if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.veh_chassis.space_id < 0)
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.veh_chassis.space_id = 0;
        }

        // LpInfoCopy(temp_mess.veh_info.lp_info,
        //            cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info);
        // LpNumberCopy(cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number,
        //              cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number);

        // if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number.lp_str == "-")
        // {
        //     if (cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.lp_str != "-")
        //     {
        //         LpNumberCopy(cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number,
        //                      cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number);
        //     }
        // }

        if (cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str == "-")
        {
            if (cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.lp_str != "-")
            {
                LpInfoCopy(cache_veh_infos_.veh_cache_infos[ind].out_lp_info,
                             cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info);
            }
        }
        else{
            LpInfoCopy(cache_veh_infos_.veh_cache_infos[ind].out_lp_info, cache_veh_infos_.veh_cache_infos[ind].lp_info);
        }

        EvidenceChainConfirm(ind, 0, cache_veh_infos_.veh_cache_infos[ind].out_lp_info.lp_number.lp_str);

        LpNumberCopy(cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number, 
            cache_veh_infos_.veh_cache_infos[ind].out_lp_info.lp_number);
        cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_color = 
            cache_veh_infos_.veh_cache_infos[ind].out_lp_info.lp_color;
        cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_status = 
            cache_veh_infos_.veh_cache_infos[ind].out_lp_info.lp_status;
        cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_type = 
            cache_veh_infos_.veh_cache_infos[ind].out_lp_info.lp_type;

        float veh_det_confidence_pro = cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.veh_det_confidence;
        if (veh_det_confidence_pro < 0.05)
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.location = Rect(0, 0, 5, 5);
            cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.location = Rect(0, 0, 20, 20);

            cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.veh_det_confidence = 0.0;
            cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.vlp_det_confidence = 0.0;
            cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number.vlp_rec_confidence = 0.0;
        }


        int temp_mess_send_veh_id = cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.send_veh_id;

        // 判断出场车辆与入场车辆的相似性
        // 0：不同 1：相同 2：相似度较高 3: 入出场均无车牌 4: 入场车辆无车牌号 5：出场车辆无车牌号 6: 无法比较
        int veh_in_out_status = CompareInOutVehLp(cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info,
                                                  cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info);

        if (veh_in_out_status == 0 || veh_in_out_status == 4 || veh_in_out_status == 6)
        {
            temp_mess_send_veh_id = send_veh_id_;
            send_veh_id_++;
        }


        if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_color == SL_PLATE_COLOR_UNKNOWN &&
            cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_color !=
                SL_PLATE_COLOR_UNKNOWN)
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_color =
                cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_color;
        }

        if (!cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear)
        {
            if (!IsSendVehNOtDisOutInfo(cap_control, ind))
            {
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_leave = false;
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_satisfy_out_flag = -1;
                return;
            }
        }

        // 无牌车出场 驶出条件严格
        if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number.lp_str == "-")
        {
            if (!IsSendNoLpVehOutInfo(ind))
            {
                if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear)
                {
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_report_out = true;
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.to_report_out = false;
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_leave = false;
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_standstill = false;
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_out_flag = 100;
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.report_out_timestamp =
                        cache_veh_infos_.frame_timestamp;
                }
                else
                {
                    ResetInOutInfo(cap_control, ind);
                }

                return;
            }
        }
        else
        {
            int same_lp_i = IsHaveSameLpVehIn(ind, cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number.lp_str, 0);
            if (same_lp_i > -1)
            {
                if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear)
                {
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_report_out = true;
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.to_report_out = false;
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_leave = false;
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_standstill = false;
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_out_flag = 200;
                    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.report_out_timestamp =
                        cache_veh_infos_.frame_timestamp;
                    SonLiLogger::getInstance()
                        .getLogger(SonLiLogger::RotateLogger)
                        ->info("------IsHaveSameLpVehIn------: camera_id {} track_id {} lp_str {}",
                               cache_veh_infos_.camera_channel, cache_veh_infos_.veh_cache_infos[ind].track_id,
                               cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str);
                }
                else
                {
                    SonLiLogger::getInstance()
                        .getLogger(SonLiLogger::RotateLogger)
                        ->info("------IsHaveSameLpVehIn---1---: camera_id {} track_id {} lp_str {}",
                               cache_veh_infos_.camera_channel, cache_veh_infos_.veh_cache_infos[ind].track_id,
                               cache_veh_infos_.veh_cache_infos[ind].lp_info.lp_number.lp_str);
                    ResetInOutInfo(cap_control, ind);
                }

                return;
            }
        }

        if (temp_mess_send_veh_id > 0 &&
            temp_mess_send_veh_id == cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.send_veh_id)
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_color =
                cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_color;
        }

        if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_color != SL_PLATE_COLOR_UNKNOWN)
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_status = PLATE_YES;
        }

        if (temp_mess_send_veh_id < 0)
        {
            temp_mess_send_veh_id = send_veh_id_;
            send_veh_id_++;
        }
        cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.send_veh_id = temp_mess_send_veh_id;

        // VehSendMesCopy(cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess, temp_mess);
        

        if (IsSendVehOutInfoTwice(ind))
        {
            if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear)
            {
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_report_out = true;
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.to_report_out = false;
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_leave = false;
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_standstill = false;
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_out_flag = 300;
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.report_out_timestamp =
                    cache_veh_infos_.frame_timestamp;
            }
            else
            {
                ResetInOutInfo(cap_control, ind);
            }

            return;
        }

        int del_in_index = GetToDelSpaceVehIndex(ind, temp_mess_send_veh_id);

        // 处理 驶入无牌 + 驶出有牌 + 强关联驶入驶出
        if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_report_in &&
            !cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in &&
            cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.lp_str == "-")
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.order_generate_type = 997;
        }

        // 处理 异常重启，无驶入+有驶出 + 强关联驶入驶出
        if (del_in_index == -1 && first_veh_timestamp_ > 1)
        {
            int time_cha =
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.veh_appear_timestamp - first_veh_timestamp_;
            if (time_cha < 1000)
            {
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.order_generate_type = 998;
            }
        }

        if (del_in_index > -1 && del_in_index < cache_veh_in_infos_.size())
        {
            if (cache_veh_in_infos_[del_in_index].veh_info.veh_chassis.space_id !=
                cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.veh_chassis.space_id)
            {
                cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.veh_chassis.space_id =
                    cache_veh_in_infos_[del_in_index].veh_info.veh_chassis.space_id;
            }
            if (cache_veh_in_infos_[del_in_index].veh_info.lp_info.lp_color !=
                cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_color)
            {
                cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_color =
                    cache_veh_in_infos_[del_in_index].veh_info.lp_info.lp_color;
            }

            if (cache_veh_in_infos_[del_in_index].is_load)
            {
                cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.order_generate_type = 999;
                if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number.lp_str !=
                        cache_veh_in_infos_[del_in_index].veh_info.lp_info.lp_number.lp_str &&
                    cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number.lp_str !=
                        "-" &&
                    cache_veh_in_infos_[del_in_index].veh_info.lp_info.lp_number.lp_str != "-")
                {
                    LpNumberCopy(cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number,
                                 cache_veh_in_infos_[del_in_index].veh_info.lp_info.lp_number);
                    cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_color =
                        cache_veh_in_infos_[del_in_index].veh_info.lp_info.lp_color;
                }
            }

            DeleteSpaceVeh(del_in_index);
        }

        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.to_report_out = true;
        cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.report_out_timestamp = cache_veh_infos_.frame_timestamp;

        std::string tem_lp =
            cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number.lp_str;
        if (tem_lp != "-")
        {
            CacheVehOutInfo cache_veh_out_info;
            cache_veh_out_info.report_out_timestamp = cache_veh_infos_.frame_timestamp;
            VehInfoCopy(cache_veh_out_info.veh_info,
                        cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info);
            cache_veh_out_infos_.push_back(cache_veh_out_info);
        }
    }
    return;
}

void
VehInfoUpload::VehInUpload(VehInOutInfoVec &veh_inout_info_vec, CapControlMsgVec &cap_control, int ind)
{

    if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_report_in)
    {
        return;
    }

    if (cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_pic_id < 0)
    {
        int ret_out = UpdateCacheVehSendMessage(cap_control, ind, SL_CAR_STATE_IN_PRE);
        if (ret_out != SL_SUCCESS)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->info("VehOutUpload 06 UpdateCacheVehSendMessage error!  track_id {}",
                       cache_veh_infos_.veh_cache_infos[ind].track_id);
        }
        SonLiLogger::getInstance()
            .getLogger(SonLiLogger::RotateLogger)
            ->info("VehOutUpload time SL_CAR_STATE_IN_PRE error!  track_id {}, camera_channel {}, frame_id {}",
                   cache_veh_infos_.veh_cache_infos[ind].track_id, cache_veh_infos_.camera_channel,
                   cache_veh_infos_.frame_id);
        return;
    }

    if (cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_pic_id < 0)
    {
        int ret_out = UpdateCacheVehSendMessage(cap_control, ind, SL_CAR_STATE_IN);
        if (ret_out != SL_SUCCESS)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->info("VehOutUpload 07 UpdateCacheVehSendMessage error!  track_id {}",
                       cache_veh_infos_.veh_cache_infos[ind].track_id);
        }
        SonLiLogger::getInstance()
            .getLogger(SonLiLogger::RotateLogger)
            ->info("VehOutUpload time SL_CAR_STATE_IN error!  track_id {}, camera_channel {}, frame_id {}",
                   cache_veh_infos_.veh_cache_infos[ind].track_id, cache_veh_infos_.camera_channel,
                   cache_veh_infos_.frame_id);
        return;
    }

    if (cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_pic_id < 0)
    {
        int ret_out = UpdateCacheVehSendMessage(cap_control, ind, SL_CAR_STATE_STOP);
        if (ret_out != SL_SUCCESS)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->info("VehOutUpload 08 UpdateCacheVehSendMessage error!  track_id {}",
                       cache_veh_infos_.veh_cache_infos[ind].track_id);
        }
        SonLiLogger::getInstance()
            .getLogger(SonLiLogger::RotateLogger)
            ->info("VehOutUpload time SL_CAR_STATE_STOP error!  track_id {}, camera_channel {}, frame_id {}",
                   cache_veh_infos_.veh_cache_infos[ind].track_id, cache_veh_infos_.camera_channel,
                   cache_veh_infos_.frame_id);
        return;
    }

    if (cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.lp_str == "-")
    {
        bool veh_in_time_status =
            time_compare(cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.veh_timestamp,
                         cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_timestamp, 400);
        if (veh_in_time_status)
        {
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_report_in = true;
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.report_in_timestamp = cache_veh_infos_.frame_timestamp;
            cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.to_report_in = false;

            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->info("veh_inpro_out_mess time error! lp_str {}",
                       cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.lp_str);
            return;
        }
    }

    // int space_id = VehInSpaceId(ind);
    // cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.veh_chassis.space_id = space_id;
    if(cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.veh_chassis.space_id < 0){
        cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.veh_chassis.space_id = 0;
    }

    // VehSendMessage temp_mess;
    // VehSendMesCopy(temp_mess, cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess);

    LpNumberCopy(cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.lp_info.lp_number,
                 cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number);
    cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.lp_info.lp_color =
        cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_color;
    if (cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.lp_info.lp_color != SL_PLATE_COLOR_UNKNOWN)
    {
        cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.lp_info.lp_status = PLATE_YES;
    }

    cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_track_id = cache_veh_infos_.veh_cache_infos[ind].track_id;
    cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.send_veh_id = cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.send_veh_id;

    // VehSendMesCopy(cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess, temp_mess);
    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.report_in_timestamp = cache_veh_infos_.frame_timestamp;
    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_report_in = true;
    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.to_report_in = false;

    // VehSendMessage temp_mess_out;
    // VehSendMesCopy(temp_mess_out, cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess);

    cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.veh_chassis.space_id =
        cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.veh_chassis.space_id;

    LpNumberCopy(cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.lp_info.lp_number,
                 cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number);
    cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.lp_info.lp_color =
        cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_color;
    if (cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.lp_info.lp_color != SL_PLATE_COLOR_UNKNOWN)
    {
        cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.lp_info.lp_status = PLATE_YES;
    }

    cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.veh_track_id = cache_veh_infos_.veh_cache_infos[ind].track_id;
    cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info.send_veh_id = cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.send_veh_id;

    // VehSendMesCopy(cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess, temp_mess_out);

    // 入场过程  车辆未动
    if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in)
    {
        return;
    }

    VehInOutInfo veh_inout_info;
    veh_inout_info.in_out = 1;
    veh_inout_info.camera_channel = cache_veh_infos_.camera_channel;
    veh_inout_info.veh_state_fraction = cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_state_fraction;

    VehInfoCopy(veh_inout_info.out_veh_info, cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info);
    VehInfoCopy(veh_inout_info.pro_veh_info, cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info);
    VehInfoCopy(veh_inout_info.in_veh_info, cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info);

    veh_inout_info.out_veh_info.frame_id =
        cap_buf_control_.cap_id_frame_id_[cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_pic_id];

    veh_inout_info.pro_veh_info.frame_id =
        cap_buf_control_.cap_id_frame_id_[cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_pic_id];

    veh_inout_info.in_veh_info.frame_id =
        cap_buf_control_.cap_id_frame_id_[cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_pic_id];

    // veh_inout_info.inout_timestamp = cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.frame_timestamp;
    veh_inout_info.inout_timestamp = cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.report_in_timestamp;
    veh_inout_info.in_timestamp = veh_inout_info.inout_timestamp;
    veh_inout_info.send_timestamp = cache_veh_infos_.frame_timestamp;

    lpColorTypeConfirm(veh_inout_info.pro_veh_info.lp_info);
    lpColorTypeConfirm(veh_inout_info.out_veh_info.lp_info);
    lpColorTypeConfirm(veh_inout_info.in_veh_info.lp_info);

    veh_inout_info.order_generate_type = cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.order_generate_type;

    veh_inout_info_vec.veh_inout_info.push_back(veh_inout_info);
    veh_inout_info_vec.size++;


    SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                    "----in-----lp_continuous---------- lp_str_max {}  lp_count_max {} last_lp_str {} {}" , 
                    cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_str_max,
                    cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_count_max,
                    cache_veh_infos_.veh_cache_infos[ind].last_lp_info.lp_str,
                    cache_veh_infos_.frame_timestamp - cache_veh_infos_.veh_cache_infos[ind].last_lp_info.timestamp);
}

void
VehInfoUpload::VehOutUpload(VehInOutInfoVec &veh_inout_info_vec, CapControlMsgVec &cap_control, int ind)
{

    if (cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_report_out)
    {
        return;
    }
    if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_pic_id < 0)
    {
        int ret_out = UpdateCacheVehSendMessage(cap_control, ind, SL_CAR_STATE_STOP_LEAVE);
        if (ret_out != SL_SUCCESS)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->info("VehOutUpload 09 UpdateCacheVehSendMessage error!  track_id {}",
                       cache_veh_infos_.veh_cache_infos[ind].track_id);
        }
        SonLiLogger::getInstance()
            .getLogger(SonLiLogger::RotateLogger)
            ->info("VehOutUpload time SL_CAR_STATE_STOP_LEAVE error!  track_id {}, camera_channel {}, frame_id {}",
                   cache_veh_infos_.veh_cache_infos[ind].track_id, cache_veh_infos_.camera_channel,
                   cache_veh_infos_.frame_id);
        return;
    }

    if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_pic_id < 0)
    {
        int ret_out = UpdateCacheVehSendMessage(cap_control, ind, SL_CAR_STATE_LEAVING);
        if (ret_out != SL_SUCCESS)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->info("VehOutUpload 10 UpdateCacheVehSendMessage error!  track_id {}",
                       cache_veh_infos_.veh_cache_infos[ind].track_id);
        }
        SonLiLogger::getInstance()
            .getLogger(SonLiLogger::RotateLogger)
            ->info("VehOutUpload time SL_CAR_STATE_LEAVING error!  track_id {}, camera_channel {}, frame_id {}",
                   cache_veh_infos_.veh_cache_infos[ind].track_id, cache_veh_infos_.camera_channel,
                   cache_veh_infos_.frame_id);
        return;
    }

    if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_pic_id < 0)
    {
        int ret_out = UpdateCacheVehSendMessage(cap_control, ind, SL_CAR_STATE_OUT);
        if (ret_out != SL_SUCCESS)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->info("VehOutUpload 11 UpdateCacheVehSendMessage error!  track_id {}",
                       cache_veh_infos_.veh_cache_infos[ind].track_id);
        }
        SonLiLogger::getInstance()
            .getLogger(SonLiLogger::RotateLogger)
            ->info("VehOutUpload time SL_CAR_STATE_OUT error!  track_id {}, camera_channel {}, frame_id {}",
                   cache_veh_infos_.veh_cache_infos[ind].track_id, cache_veh_infos_.camera_channel,
                   cache_veh_infos_.frame_id);
        return;
    }

    bool veh_out_time_status =
        time_compare(cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.veh_timestamp,
                     cache_veh_infos_.frame_timestamp, 300);

    // if (veh_out_time_status)
    // {
    //     int ret_out = UpdateCacheVehSendMessage(cap_control, ind, SL_CAR_STATE_LEAVING);
    //     if (ret_out != SL_SUCCESS)
    //     {
    //         SonLiLogger::getInstance()
    //             .getLogger(SonLiLogger::RotateLogger)
    //             ->info("VehOutUpload 12 UpdateCacheVehSendMessage error!  track_id {}",
    //                    cache_veh_infos_.veh_cache_infos[ind].track_id);
    //     }
    //     SonLiLogger::getInstance()
    //         .getLogger(SonLiLogger::RotateLogger)
    //         ->info("VehOutUpload time cha big!  track_id {}, camera_channel {}, frame_id {}",
    //                cache_veh_infos_.veh_cache_infos[ind].track_id, cache_veh_infos_.camera_channel,
    //                cache_veh_infos_.frame_id);
    //     return;
    // }

    if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.veh_timestamp >=
            cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info.veh_timestamp)
    {
        SonLiLogger::getInstance()
            .getLogger(SonLiLogger::RotateLogger)
            ->info("veh_outpro_pro_mess time > veh_outpro_out_mess time! track_id {}, camera_channel {}, lp_str {}",
                   cache_veh_infos_.veh_cache_infos[ind].track_id, 
                   cache_veh_infos_.camera_channel,
                   cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number.lp_str);
                   
        int ret_out = UpdateCacheVehSendMessage(cap_control, ind, SL_CAR_STATE_OUT);
        if (ret_out != SL_SUCCESS)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->info("VehOutUpload 13 UpdateCacheVehSendMessage error!  track_id {}",
                       cache_veh_infos_.veh_cache_infos[ind].track_id);
        }
        
        return;
    }

    veh_out_time_status =
        time_compare(cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.veh_timestamp,
                     cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info.veh_timestamp, 400);
    if (veh_out_time_status)
    {

        SonLiLogger::getInstance()
            .getLogger(SonLiLogger::RotateLogger)
            ->info("veh_outpro_out_mess time error!  camera_channel {} track_id {} lp_str {}",
                   cache_veh_infos_.camera_channel, cache_veh_infos_.veh_cache_infos[ind].track_id,
                   cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number.lp_str);
    }

    // VehSendMessage temp_mess;
    // VehSendMesCopy(temp_mess, cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess);

    cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info.veh_chassis.space_id =
        cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.veh_chassis.space_id;
    if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info.veh_chassis.space_id < 0)
    {
        cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info.veh_chassis.space_id =
            cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_info.veh_chassis.space_id;
    }
    if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info.veh_chassis.space_id < 0)
    {
        cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info.veh_chassis.space_id = 0;
    }

    LpNumberCopy(cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info.lp_info.lp_number,
                 cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number);
    cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info.lp_info.lp_color =
        cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_color;
    if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info.lp_info.lp_color != SL_PLATE_COLOR_UNKNOWN)
    {
        cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info.lp_info.lp_status = PLATE_YES;
    }

    cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info.veh_track_id = cache_veh_infos_.veh_cache_infos[ind].track_id;
    cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info.send_veh_id = cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.send_veh_id;

    // VehSendMesCopy(cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess, temp_mess);

    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_report_out = true;
    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.report_out_timestamp = cache_veh_infos_.frame_timestamp;
    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.to_report_out = false;
    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_leave = false;
    cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_standstill = false;

    // VehSendMessage temp_mess_std;
    // VehSendMesCopy(temp_mess_std, cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess);

    cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_info.veh_chassis.space_id = 
        cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info.veh_chassis.space_id;
    if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_info.veh_chassis.space_id < 0)
    {
        cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_info.veh_chassis.space_id = 0;
    }

    LpNumberCopy(cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_info.lp_info.lp_number,
                 cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number);

    cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_info.lp_info.lp_color =
        cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_color;
    if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_info.lp_info.lp_color != SL_PLATE_COLOR_UNKNOWN)
    {
        cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_info.lp_info.lp_status = PLATE_YES;
    }

    cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_info.veh_track_id = cache_veh_infos_.veh_cache_infos[ind].track_id;
    cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_info.send_veh_id = 
        cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.send_veh_id;

    // VehSendMesCopy(cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess, temp_mess_std);

    VehInOutInfo veh_inout_info;
    veh_inout_info.in_out = 0;
    veh_inout_info.camera_channel = cache_veh_infos_.camera_channel;
    veh_inout_info.veh_state_fraction = cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_state_fraction;

    VehInfoCopy(veh_inout_info.out_veh_info, cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info);
    VehInfoCopy(veh_inout_info.pro_veh_info, cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info);
    VehInfoCopy(veh_inout_info.in_veh_info, cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_info);

    veh_inout_info.out_veh_info.frame_id =
        cap_buf_control_.cap_id_frame_id_[cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_pic_id];

    veh_inout_info.pro_veh_info.frame_id =
        cap_buf_control_.cap_id_frame_id_[cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_pic_id];

    veh_inout_info.in_veh_info.frame_id =
        cap_buf_control_.cap_id_frame_id_[cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_pic_id];

    veh_inout_info.inout_timestamp = cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.report_out_timestamp;
    veh_inout_info.in_timestamp = cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.report_in_timestamp;
    veh_inout_info.send_timestamp = cache_veh_infos_.frame_timestamp;

    int space_id = veh_inout_info.pro_veh_info.veh_chassis.space_id;

    // if (space_id >= space_info_.space_code.size() || space_id < 0)
    // {
    //     space_id = veh_inout_info.in_veh_info.veh_chassis.space_id;
    // }

    // if (space_id < space_info_.space_code.size() && space_id > -1)
    // {
    //     veh_inout_info.space_code = space_info_.space_code[space_id];
    // }
    // else
    // {
    //     veh_inout_info.space_code = space_info_.space_code[0];
    // }

    lpColorTypeConfirm(veh_inout_info.pro_veh_info.lp_info);
    lpColorTypeConfirm(veh_inout_info.out_veh_info.lp_info);
    lpColorTypeConfirm(veh_inout_info.in_veh_info.lp_info);

    veh_inout_info.order_generate_type = cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.order_generate_type;

    veh_inout_info_vec.veh_inout_info.push_back(veh_inout_info);
    veh_inout_info_vec.size++;

    SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                    "----out-----lp_continuous---------- lp_str_max {}  lp_count_max {} last_lp_str {} {}" , 
                    cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_str_max,
                    cache_veh_infos_.veh_cache_infos[ind].lp_continuous.lp_count_max,
                    cache_veh_infos_.veh_cache_infos[ind].last_lp_info.lp_str,
                    cache_veh_infos_.frame_timestamp - cache_veh_infos_.veh_cache_infos[ind].last_lp_info.timestamp);
}

// 更新上报车辆message
void
VehInfoUpload::UpdateToUploadVehMessage(CapControlMsgVec &cap_control, VehInOutInfoVec &veh_inout_info_vec)
{

    std::vector<int> del_veh_cache_ind;

    int ret;
    for (int i = cache_veh_infos_.veh_cache_infos.size() - 1; i >= 0; i--)
    {

        if (cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_standstill &&
            cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_leave)
        {
            bool time_status_ =
                time_compare(cache_veh_infos_.veh_cache_infos[i].veh_standstill_mess.veh_info.veh_timestamp,
                             cache_veh_infos_.veh_cache_infos[i].veh_outpro_pro_mess.veh_info.veh_timestamp,
                             park_param_.veh_out_time_thres);

            if (!time_status_ && !cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.no_send_in &&
                !cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_report_in)
            {
                // 入出场间隔时间短
                if (cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_disappear)
                {
                    del_veh_cache_ind.push_back(i);
                }
                else
                {
                    ResetInOutInfo(cap_control, i);
                }
                continue;
            }

            // 				if (cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.no_send_in &&
            // trend_of_space_rate != "false")
            // 				{
            // 					auto space_rate_vec =
            // cache_veh_infos_.veh_cache_infos[i].space_rate_list.readFirstNElements(240); 					if (space_rate_vec.size() >
            // 30)
            // 					{
            // //						std::cout << "LinearRegression P:" <<
            // cache_veh_infos_.veh_cache_infos[i].track_id << std::endl;

            // // cache_veh_infos_.veh_cache_infos[i].space_rate_list.print(); 						auto ma =
            // basic_algorithm::MovingAverage(space_rate_vec,  8); 						auto maxI = basic_algorithm::FindKeyPoint(ma, 8);

            // 						auto l_e = basic_algorithm::LinearRegression(ma, 0, ma.size());
            // 						auto R2 = basic_algorithm::CalculateRSquared(ma, l_e.first,
            // l_e.second, 0, ma.size()); 						auto trend = basic_algorithm::DetermineTrend(l_e.first, R2, 0.8);

            // 						auto l_e1 = basic_algorithm::LinearRegression(ma, 0, maxI);
            // 						auto R21 = basic_algorithm::CalculateRSquared(ma, l_e1.first,
            // l_e1.second, 0, maxI); 						auto trend1 = basic_algorithm::DetermineTrend(l_e1.first, R21, 0.75);

            // 						auto l_e2 = basic_algorithm::LinearRegression(ma, maxI,
            // ma.size()); 						auto R22 = basic_algorithm::CalculateRSquared(ma, l_e2.first, l_e2.second, maxI, ma.size());
            // 						auto trend2 = basic_algorithm::DetermineTrend(l_e2.first, R22,
            // 0.8);

            // //						std::cout << "trend1:<" << (int)trend1.first<< "," <<
            // trend1.second<<">\t" << R21 << "\t" << std::endl;
            // //						std::cout << "trend2:<" << (int)trend2.first<< "," <<
            // trend2.second<<">\t" << R22 << "\t" << std::endl;
            // //						std::cout << "trend:<" << (int)trend.first<< "," <<
            // trend.second<<">\t" << R2 << "\t" << std::endl; 						if(trend1.first == basic_algorithm::Trend::INCREASING) {
            // 							if(trend2.first == basic_algorithm::Trend::DECREASING){
            // 								if(trend.first ==
            // basic_algorithm::Trend::CONSTANT) { 									cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_standstill =
            // false;
            // 								}
            // 							}
            // 						}

            // //						if (trend.first == basic_algorithm::Trend::DECREASING &&
            // trend.second > 0.8
            // //							&&
            // (cache_veh_infos_.veh_cache_infos[i].veh_infos[totalofveh - 1 - 1].veh_chassis.space_rat < 0.25))
            // //						{
            // //							ResetInOutInfo(cap_control, i);
            // //						}
            // 					}
            // 				}
        }

        if (cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_report_in)
        {
            if (cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_leave)
            {
                if (cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.to_report_out)
                {
                    VehOutUpload(veh_inout_info_vec, cap_control, i);
                    // del_veh_cache_ind.push_back(i);
                    continue;
                }

                bool time_status = true;
                int time_thres = std::min(5, park_param_.veh_out_time_thres);
                time_status =
                    time_compare(cache_veh_infos_.veh_cache_infos[i].veh_outpro_pro_mess.veh_info.veh_timestamp,
                                 cache_veh_infos_.frame_timestamp, time_thres);

                if (time_status)
                {
                    if (cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.to_report_out)
                    {
                        VehOutUpload(veh_inout_info_vec, cap_control, i);
                        // del_veh_cache_ind.push_back(i);
                        continue;
                    }
                    else
                    {
                        if (!cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_disappear)
                        {
                            VehOutProUpload(cap_control, i);
                        }
                        else
                        {
                            if (cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.out_max_iou < -1)
                            {
                                cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_leave = false;
                            }
                            else
                            {
                                VehOutProUpload(cap_control, i);
                            }
                        }
                    }
                }
                else
                {
                    if (!cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_disappear)
                    {
                        bool is_leave = VehOutStatus(i);
                        if (cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.out_move_count == 0 &&
                            cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.out_count == 0)
                        {
                            cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_leave = false;
                            ResetVehInInfo(cap_control, i);
                            ResetVehOutInfo(cap_control, i);

                            return;
                        }
                    }
                    // else
                    // {
                    // 	if (cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.out_max_iou < -1)
                    // 	{
                    // 		cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_leave = false;
                    // 	}
                    // }
                }
            }
        }
        else
        {
            if (cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_standstill)
            {
                if (cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.to_report_in)
                {
                    VehInUpload(veh_inout_info_vec, cap_control, i);
                    continue;
                }

                unsigned long long stand_time =
                    cache_veh_infos_.veh_cache_infos[i].veh_standstill_mess.veh_info.veh_timestamp;
                unsigned long long outpro_time =
                    cache_veh_infos_.veh_cache_infos[i].veh_outpro_pro_mess.veh_info.veh_timestamp;

                bool time_status =
                    time_compare(stand_time, cache_veh_infos_.frame_timestamp, park_param_.veh_out_time_thres);

                if (time_status && outpro_time > stand_time &&
                    cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.out_move_count > 2)
                {
                    if (cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.out_move_count > 5 ||
                        cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.out_count > 0)
                    {
                        time_status = time_compare(stand_time, outpro_time, park_param_.veh_out_time_thres);
                    }
                }

                if (time_status || cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.no_send_in)
                {
                    if (cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.to_report_in)
                    {
                        VehInUpload(veh_inout_info_vec, cap_control, i);
                        continue;
                    }
                    else
                    {
                        VehInProUpload(i);
                        continue;
                    }
                }
            }
        }
    }

    for (int ind : del_veh_cache_ind)
    {
        int ret = ClearVehInInfo(cap_control, ind);
        if (ret != SL_SUCCESS)
        {
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info("ClearVehInInfo error !");
        }
    }

    return;
}

} // namespace sonli
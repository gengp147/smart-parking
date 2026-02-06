#include "veh_cache_info.h"
#include "error.h"
#include "veh_info_copy.h"
#include "../utils/sl_logger.h"
#include "../models_processor/char_code.h"

namespace sonli{

	int VehCacheInfo::ResetVehInInfo(CapControlMsgVec& cap_control, int ind) {
		int temp = -1;
		if (cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_pic_id >= 0) {
			int cap_id = cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_pic_id;
			if (cap_id >= 0 && cap_id < max_cap_buf_len) {
				long frame_id = cap_buf_control_.cap_id_frame_id_[cap_id];
				int ret = UpdateCapControlMsg(cap_control, SL_CAPBUF_CMD_DEL, cap_id, frame_id, temp);
				if (ret != SL_SUCCESS) {
					SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                		"UpdateCapControlMsg failed ! track_id {}, line {}, channel {}", 
						cache_veh_infos_.veh_cache_infos[ind].track_id, __LINE__, cache_veh_infos_.camera_channel);
				}
			}
			cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_pic_id = -1;
		}

		if (cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_pic_id >= 0) {
			int cap_id = cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_pic_id;
			if (cap_id >= 0 && cap_id < max_cap_buf_len) {
				long frame_id = cap_buf_control_.cap_id_frame_id_[cap_id];
				int ret = UpdateCapControlMsg(cap_control, SL_CAPBUF_CMD_DEL, cap_id, frame_id, temp);
				if (ret != SL_SUCCESS) {
					SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                		"UpdateCapControlMsg failed ! track_id {}, line {}, channel {}", 
						cache_veh_infos_.veh_cache_infos[ind].track_id, __LINE__, cache_veh_infos_.camera_channel);
				}
			}
			cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_pic_id = -1;
		}

		if (cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_pic_id >= 0) {
			int cap_id = cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_pic_id;
			if (cap_id >= 0 && cap_id < max_cap_buf_len) {
				long frame_id = cap_buf_control_.cap_id_frame_id_[cap_id];
				int ret = UpdateCapControlMsg(cap_control, SL_CAPBUF_CMD_DEL, cap_id, frame_id, temp);
				if (ret != SL_SUCCESS) {
					SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                		"UpdateCapControlMsg failed ! track_id {}, line {}, channel {}", 
						cache_veh_infos_.veh_cache_infos[ind].track_id, __LINE__, cache_veh_infos_.camera_channel);
				}
			}
			cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_pic_id = -1;
		}

		return SL_SUCCESS;
	}

	int VehCacheInfo::ResetVehOutInfo(CapControlMsgVec& cap_control, int ind) {

		int temp = -1;

		if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_pic_id >= 0) {
			int cap_id = cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_pic_id;
			if (cap_id >= 0 && cap_id < max_cap_buf_len) {
				long frame_id = cap_buf_control_.cap_id_frame_id_[cap_id];
				int ret = UpdateCapControlMsg(cap_control, SL_CAPBUF_CMD_DEL, cap_id, frame_id, temp);
				if (ret != SL_SUCCESS) {
					SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                		"UpdateCapControlMsg failed ! track_id {}, line {}, channel {}", 
						cache_veh_infos_.veh_cache_infos[ind].track_id, __LINE__, cache_veh_infos_.camera_channel);
				}
			}
			cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_pic_id = -1;
		}

		if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_pic_id >= 0) {
			int cap_id = cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_pic_id;
			if (cap_id >= 0 && cap_id < max_cap_buf_len) {
				long frame_id = cap_buf_control_.cap_id_frame_id_[cap_id];
				int ret = UpdateCapControlMsg(cap_control, SL_CAPBUF_CMD_DEL, cap_id, frame_id, temp);
				if (ret != SL_SUCCESS) {
					SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                		"UpdateCapControlMsg failed ! track_id {}, line {}, channel {}", 
						cache_veh_infos_.veh_cache_infos[ind].track_id, __LINE__, cache_veh_infos_.camera_channel);
				}
			}
			cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_pic_id = -1;
		}

		if (cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_pic_id >= 0) {
			int cap_id = cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_pic_id;
			if (cap_id >= 0 && cap_id < max_cap_buf_len) {
				long frame_id = cap_buf_control_.cap_id_frame_id_[cap_id];
				int ret = UpdateCapControlMsg(cap_control, SL_CAPBUF_CMD_DEL, cap_id, frame_id, temp);
				if (ret != SL_SUCCESS) {
					SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                		"UpdateCapControlMsg failed ! track_id {}, line {}, channel {}", 
						cache_veh_infos_.veh_cache_infos[ind].track_id, __LINE__, cache_veh_infos_.camera_channel);
				}
			}
			cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_pic_id = -1;
		}

		return SL_SUCCESS;
	}

	void VehCacheInfo::ResetInOutVar(int ind){
		// cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.reset();
        VehInOutVar veh_in_out_var_tem;
		cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var = veh_in_out_var_tem;
	}

	int VehCacheInfo::ResetInOutInfo(CapControlMsgVec& cap_control, int ind){
		int ret;
		ret = ResetVehInInfo(cap_control, ind);
		ret = ResetVehOutInfo(cap_control, ind);
		
		ResetInOutVar(ind);

		return SL_SUCCESS;
	}

	//清空缓存图片及车辆入场信息
	int VehCacheInfo::ClearVehInInfo(CapControlMsgVec& cap_control, int ind) {
		if(!cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.is_disappear){
			int ret;
			ret = ResetInOutInfo(cap_control, ind);

			return SL_SUCCESS;
		}

		int ret;
		ret = ResetVehInInfo(cap_control, ind);
		ret = ResetVehOutInfo(cap_control, ind);
		
		// cache_veh_infos_.veh_cache_infos[ind].reset();
		cache_veh_infos_.veh_cache_infos.erase(cache_veh_infos_.veh_cache_infos.begin() + ind);

		return SL_SUCCESS;
	}

	//更新上报过的车辆信息
	int VehCacheInfo::UpdateHaveSendVehInfo(CapControlMsgVec& cap_control) {

		for (int i = cache_veh_infos_.veh_cache_infos.size() - 1; i >= 0; i--) {
			if (cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_report_out) {
				if(cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_disappear){
					int ret = ClearVehInInfo(cap_control, i);
					if (ret != SL_SUCCESS) {
						SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->warn("ClearVehInInfo error");
					}
				}
				else{
					int ret;
					ret = ResetInOutInfo(cap_control, i);
					if (ret != SL_SUCCESS) {
						SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->warn("ResetInOutInfo error");
					}
				}
			}
			else {
				if (cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_report_in) {
					int ret = ResetVehInInfo(cap_control, i);
					if (ret != SL_SUCCESS) {
						SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->warn("ResetVehInInfo error");
					}
				}
			}
		}

		return SL_SUCCESS;
	}

	void VehCacheInfo::CombineVehInfo(int index_i, int index_j) {

		if (cache_veh_infos_.veh_cache_infos[index_j].lp_info.lp_number.lp_count >
				cache_veh_infos_.veh_cache_infos[index_i].lp_info.lp_number.lp_count &&
			cache_veh_infos_.veh_cache_infos[index_j].lp_info.lp_number.vlp_rec_confidence >
				cache_veh_infos_.veh_cache_infos[index_i].lp_info.lp_number.vlp_rec_confidence)
		{
            LpNumberCopy(cache_veh_infos_.veh_cache_infos[index_i].lp_info.lp_number,
							cache_veh_infos_.veh_cache_infos[index_j].lp_info.lp_number);
		}

		cache_veh_infos_.veh_cache_infos[index_i].veh_infos_first_n = cache_veh_infos_.veh_cache_infos[index_j].veh_infos_first_n;
		cache_veh_infos_.veh_cache_infos[index_i].veh_locs_first_n = cache_veh_infos_.veh_cache_infos[index_j].veh_locs_first_n;
		cache_veh_infos_.veh_cache_infos[index_i].veh_in_out_var.is_combine++;
	}

	void VehCacheInfo::CombineVehInfo(CapControlMsgVec& cap_control, std::vector<int>& todel_vec){
		for (int i = 0; i < cache_veh_infos_.veh_cache_infos.size(); i++) {
			if (cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_combine > 0 ||
				cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_report_in ||
				cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_report_out ||
				cache_veh_infos_.veh_cache_infos[i].veh_infos.back().is_image_edge)
			{
				continue;
			}
			if (cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_standstill &&
				!cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_report_in)
			{
				continue;
			}
			if (cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_leave &&
				!cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.is_report_out)
			{
				continue;
			}

			int timec = cache_veh_infos_.frame_timestamp - cache_veh_infos_.veh_cache_infos[i].veh_infos.back().veh_timestamp;
			if (timec > 500 || cache_veh_infos_.veh_cache_infos[i].veh_infos.back().veh_chassis.space_id < 0)
			{
				continue;
			}

			int ret_st = cache_veh_infos_.frame_timestamp - cache_veh_infos_.veh_cache_infos[i].veh_in_out_var.veh_appear_timestamp;
			if(ret_st < 3000 || ret_st > 30000){
				continue;
			}

			int lp_count_i = 0;
			float lp_confidence_i = 0.0;
			std::string lp_str_i = "-";
			std::vector<SLLPColor_t> lp_color_vec_tmp1;
			std::vector<std::pair<std::string, float>> lp_str_vec = 
				GetVehLpStrVec(cache_veh_infos_.veh_cache_infos[i].veh_infos, lp_color_vec_tmp1);
			LpStrCount(lp_str_vec, lp_count_i, lp_confidence_i, lp_str_i);

			for (int j = 0; j < i; j++){
				if (cache_veh_infos_.veh_cache_infos[j].veh_in_out_var.is_report_out || 
					cache_veh_infos_.veh_cache_infos[j].veh_in_out_var.is_report_in ||
					cache_veh_infos_.veh_cache_infos[j].veh_in_out_var.is_leave) {
					continue;
				}
				IsDisAbnormal(j);
				if (!cache_veh_infos_.veh_cache_infos[j].veh_in_out_var.is_disappear_abnormal) {
					continue;
				}

				int dis_time_c = cache_veh_infos_.frame_timestamp - cache_veh_infos_.veh_cache_infos[j].veh_infos.back().veh_timestamp;
				if(dis_time_c < 2000 || dis_time_c > 30000){
					continue;
				}
				if(cache_veh_infos_.veh_cache_infos[j].veh_infos.size() < 20){
					continue;
				}

				if (lp_str_i != "-" && lp_count_i > 10 && lp_confidence_i > 0.95)
				{
					int lp_count_j = 0;
					float lp_confidence_j = 0.0;
					std::string lp_str_j = "-";
					std::vector<SLLPColor_t> lp_color_vec_tmp2;
					std::vector<std::pair<std::string, float>> lp_str_vec = 
						GetVehLpStrVec(cache_veh_infos_.veh_cache_infos[j].veh_infos, lp_color_vec_tmp2);
					LpStrCount(lp_str_vec, lp_count_j, lp_confidence_j, lp_str_j);

					if (lp_count_j > 10 && lp_confidence_j > 0.95 && lp_str_j == lp_str_i)
					{
						CombineVehInfo(i, j);
						SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
							"------combine 21---: camera_id {}, track_id {}={}",
							cache_veh_infos_.camera_channel,
							cache_veh_infos_.veh_cache_infos[i].track_id,
							cache_veh_infos_.veh_cache_infos[j].track_id);

						cache_veh_infos_.veh_cache_infos[j].veh_in_out_var.is_combine = 10000;
						todel_vec.push_back(j);
						break;
					}
				}

				if(cache_veh_infos_.use_reid){
					cv::Mat feature_j = cache_veh_infos_.veh_cache_infos[j].
						veh_infos[cache_veh_infos_.veh_cache_infos[j].veh_infos.size() - 10].feature;
					for (int veh_j_ind = cache_veh_infos_.veh_cache_infos[j].veh_infos.size() - 11; veh_j_ind > 20; veh_j_ind--)
					{
						if (cache_veh_infos_.veh_cache_infos[j].veh_infos[veh_j_ind].cover_rat < 0.2 &&
							!cache_veh_infos_.veh_cache_infos[j].veh_infos[veh_j_ind].is_image_edge)
						{
							feature_j = cache_veh_infos_.veh_cache_infos[j].veh_infos[veh_j_ind].feature;
							break;
						}
					}
					cv::Mat AB = cache_veh_infos_.veh_cache_infos[i].veh_infos.back().feature * feature_j.t();
					float feature_cos = AB.at<float>(0, 0);

					if(feature_cos > 0.9){
						SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
							"------combine 22---: cosi {} camera_id {}, track_id {}={}",
							feature_cos,
							cache_veh_infos_.camera_channel,
							cache_veh_infos_.veh_cache_infos[i].track_id,
							cache_veh_infos_.veh_cache_infos[j].track_id);

						CombineVehInfo(i, j);
						cache_veh_infos_.veh_cache_infos[j].veh_in_out_var.is_combine = 10000;
						todel_vec.push_back(j);
						break;
					}
				}
			}

		}

	}

	void VehCacheInfo::UpdateCacheVehInfoVec(CapControlMsgVec& cap_control){
		UpdateHaveSendVehInfo(cap_control);

		std::vector<int> todel_vec;
		CombineVehInfo(cap_control, todel_vec);
		for (int i = todel_vec.size() - 1; i >= 0; i--) {
			ClearVehInInfo(cap_control, todel_vec[i]);
			if(!cache_veh_infos_.veh_cache_infos[todel_vec[i]].veh_in_out_var.is_disappear){
				cache_veh_infos_.veh_cache_infos[todel_vec[i]].veh_in_out_var.is_combine = 10000;
			}
		}
	}


	int VehCacheInfo::GetToDelSpaceVehIndex(int ind, int veh_send_id) {
		int del_index = -1;

		int send_veh_id = cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.send_veh_id;
		int veh_track_id = cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.veh_track_id;
		std::string lp_str = cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number.lp_str;
		float vlp_rec_confidence = cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number.vlp_rec_confidence;

		if (lp_str != "-") {
			for (int i = cache_veh_in_infos_.size() - 1; i >= 0; i--) {
				if (lp_str == cache_veh_in_infos_[i].veh_info.lp_info.lp_number.lp_str) {
					del_index = i;
					return del_index;
				}
					
				std::wstring veh_in_lp_strw = utf8ToUnicode(lp_str);
				std::wstring veh_out_lp_strw = utf8ToUnicode(cache_veh_in_infos_[i].veh_info.lp_info.lp_number.lp_str);
				int dis = 0;
				dis = wstr_distance(veh_in_lp_strw, veh_out_lp_strw);
				if(dis <= 1){
					// if(cache_veh_in_infos_[i].veh_info.lp_info.lp_number.vlp_rec_confidence >= vlp_rec_confidence
					// && ((cache_veh_in_infos_[i].veh_info.lp_info.lp_number.vlp_rec_confidence > 0.97
					// && cache_veh_in_infos_[i].veh_info.lp_info.lp_number.lp_count > 5)
					// || cache_veh_in_infos_[i].veh_info.veh_track_id == veh_track_id)){
					// 	cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number.lp_str = 
					// 	cache_veh_in_infos_[i].veh_info.lp_info.lp_number.lp_str;
					// }
					del_index = i;
					return del_index;
				}
			}
		}
		else{
			int space_id = cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_chassis.space_id;
			if (space_id > -1) {
				for (int i = cache_veh_in_infos_.size() - 1; i >= 0; i--) {
					if (cache_veh_in_infos_[i].veh_info.lp_info.lp_number.lp_str == "-"  && 
						cache_veh_in_infos_[i].veh_info.veh_chassis.space_id == space_id) {
						del_index = i;
						break;
					}
				}

				for (int i = cache_veh_in_infos_.size() - 1; i >= 0; i--) {
					if (cache_veh_in_infos_[i].veh_info.lp_info.lp_number.vlp_rec_confidence < 0.95 && 
						cache_veh_in_infos_[i].veh_info.veh_chassis.space_id == space_id) {
						del_index = i;
						break;
					}
				}
			}
		}

		if (del_index < 0) {
			for (int i = cache_veh_in_infos_.size() - 1; i >= 0; i--){
				if (veh_send_id == cache_veh_in_infos_[i].veh_info.send_veh_id) {
					del_index = i;
					if(cache_veh_in_infos_[i].veh_info.lp_info.lp_number.lp_str != "-"){
						cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info.lp_number.lp_str = 
							cache_veh_in_infos_[i].veh_info.lp_info.lp_number.lp_str;
					}
					return del_index;
				}
			}
		}

		return del_index;
	}

	void VehCacheInfo::AddSpaceVeh(int ind)
	{
		if (cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.lp_str == "-" ||
			cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in){
			
			return;
		}
		CacheVehInInfo cache_veh_in_info;
		cache_veh_in_info.no_send_in = cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.no_send_in;
		cache_veh_in_info.report_in_timestamp = cache_veh_infos_.frame_timestamp;
		cache_veh_in_info.last_timestamp = cache_veh_infos_.frame_timestamp;
		cache_veh_in_info.veh_infos_first_n = cache_veh_infos_.veh_cache_infos[ind].veh_infos_first_n;
		cache_veh_in_info.veh_locs_first_n = cache_veh_infos_.veh_cache_infos[ind].veh_locs_first_n;
		VehInfoCopy(cache_veh_in_info.veh_info, cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info);

		if (cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_locs.size() > 0)
		{
			cache_veh_in_info.veh_info.location = cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_locs[0];
		}

		if(cache_veh_infos_.use_reid){
			cache_veh_in_info.veh_info.feature = cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().feature.clone();
		}
		

		cache_veh_in_infos_.push_back(cache_veh_in_info);
		return;

		if (cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info.lp_info.lp_number.lp_str != "-")
		{

			int space_id = cache_veh_in_info.veh_info.veh_chassis.space_id;
			int space_id_count = 0;
			int space_id_earliest_index = -1;
			unsigned long long last_timestamp = 0;

			for (int i = 0; i < cache_veh_in_infos_.size(); i++)
			{
				int space_id_i = cache_veh_in_infos_[i].veh_info.veh_chassis.space_id;
				if (space_id == space_id_i)
				{
					if (space_id_earliest_index < 0)
					{
						space_id_earliest_index = i;
						last_timestamp = cache_veh_in_infos_[i].last_timestamp;
					}
					else
					{
						if (cache_veh_in_infos_[i].last_timestamp < last_timestamp)
						{
							space_id_earliest_index = i;
							last_timestamp = cache_veh_in_infos_[i].last_timestamp;
						}
					}
					space_id_count++;
				}
			}
			if (space_id_earliest_index >= 0 && space_id_count > 2)
			{
				cache_veh_in_infos_.erase(cache_veh_in_infos_.begin() + space_id_earliest_index);
			}

			int need_del_nolp_index = -1;
			for (int i = 0; i < cache_veh_in_infos_.size(); i++)
			{
				int space_id_i = cache_veh_in_infos_[i].veh_info.veh_chassis.space_id;
				if (space_id == space_id_i &&
					cache_veh_in_infos_[i].veh_info.lp_info.lp_number.lp_str == "-")
				{
					need_del_nolp_index = i;
					break;
				}
			}
			if (need_del_nolp_index >= 0)
			{
				cache_veh_in_infos_.erase(cache_veh_in_infos_.begin() + need_del_nolp_index);
			}

			cache_veh_in_infos_.push_back(cache_veh_in_info);
		}
		else
		{
			int space_id = cache_veh_in_info.veh_info.veh_chassis.space_id;
			bool in_flag = true;
			int need_del_nolp_index = -1;

			for (int i = 0; i < cache_veh_in_infos_.size(); i++)
			{
				int space_id_i = cache_veh_in_infos_[i].veh_info.veh_chassis.space_id;
				if (space_id == space_id_i)
				{
					if (cache_veh_in_infos_[i].veh_info.lp_info.lp_number.lp_str == "-")
					{
						need_del_nolp_index = i;
					}
					else
					{
						in_flag = false;
						break;
					}
				}
			}

			if (need_del_nolp_index >= 0)
			{
				cache_veh_in_infos_.erase(cache_veh_in_infos_.begin() + need_del_nolp_index);
			}

			if (in_flag)
			{
				cache_veh_in_infos_.push_back(cache_veh_in_info);
			}
		}
	}

	void VehCacheInfo::DeleteSpaceVeh(int del_i) {
		
		if (del_i > -1 && del_i < cache_veh_in_infos_.size()) {
			cache_veh_in_infos_.erase(cache_veh_in_infos_.begin() + del_i);
			
		}
		else{
			SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                "cache_veh_in_infos_.erase failed !  lp_str {}", 
				cache_veh_in_infos_[del_i].veh_info.lp_info.lp_number.lp_str);
		}

	}

	void VehCacheInfo::UpdateSpaceVehInfo()
	{
		std::vector<int> need_del;

		for (int i = 0; i < cache_veh_in_infos_.size(); i++)
		{
			bool veh_dis_time_status = time_compare(cache_veh_in_infos_[i].last_timestamp, cache_veh_infos_.frame_timestamp, 3*24*3600);
			if (veh_dis_time_status)
			{
				need_del.push_back(i);
				continue;
			}

			for (int j = i + 1; j < cache_veh_in_infos_.size(); j++)
			{
				std::string lp_str_i = cache_veh_in_infos_[i].veh_info.lp_info.lp_number.lp_str;
				std::string lp_str_j = cache_veh_in_infos_[j].veh_info.lp_info.lp_number.lp_str;
				if (lp_str_i != "-" && lp_str_i == lp_str_j)
				{
					need_del.push_back(i);
					break;
				}
			}
		}
		for (int i = need_del.size() - 1; i >= 0; i--)
		{
			cache_veh_in_infos_.erase(cache_veh_in_infos_.begin() + need_del[i]);
		}
		need_del.clear();

		for (int ind = 0; ind < cache_veh_infos_.veh_cache_infos.size(); ind++)
		{
			int veh_infos_size = cache_veh_infos_.veh_cache_infos[ind].veh_infos.size();
			if (veh_infos_size < 10)
			{
				continue;
			}
			if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_res != SPACE_IN)
			{
				continue;
			}

			std::string lp_str_now = cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().lp_info.lp_number.lp_str;

			if (lp_str_now == "-")
			{
				continue;
			}
			unsigned long long veh_timestamp = cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().veh_timestamp;
			for (int i = 0; i < cache_veh_in_infos_.size(); i++)
			{
				std::string lp_str_i = cache_veh_in_infos_[i].veh_info.lp_info.lp_number.lp_str;
				if (lp_str_i != "-")
				{
					if (lp_str_i == lp_str_now && veh_timestamp > cache_veh_in_infos_[i].last_timestamp)
					{
						cache_veh_in_infos_[i].last_timestamp = veh_timestamp;
						cache_veh_in_infos_[i].veh_info.veh_track_id = cache_veh_infos_.veh_cache_infos[ind].track_id;
						if(cache_veh_infos_.use_reid){
							cache_veh_in_infos_[i].veh_info.feature = 
								cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().feature.clone();
						}
						continue;
					}
				}
			}
		}

		if (cache_veh_in_infos_.size() > 100)
		{
			for (int i = 0; i < cache_veh_in_infos_.size() - 18; i++)
			{
				bool ddel = true;
				for (int j = 0; j < cache_veh_infos_.veh_cache_infos.size(); j++)
				{
					if (cache_veh_in_infos_[i].veh_info.veh_track_id == cache_veh_infos_.veh_cache_infos[j].track_id)
					{
						ddel = false;
						break;
					}
					if (cache_veh_in_infos_[i].veh_info.lp_info.lp_number.lp_str != "-" &&
						cache_veh_in_infos_[i].veh_info.lp_info.lp_number.lp_str ==
							cache_veh_infos_.veh_cache_infos[j].veh_inpro_pro_mess.veh_info.lp_info.lp_number.lp_str)
					{
						ddel = false;
						break;
					}
				}
				if (ddel)
				{
					need_del.push_back(i);
				}
			}
			for (int i = need_del.size() - 1; i >= 0; i--)
			{
				cache_veh_in_infos_.erase(cache_veh_in_infos_.begin() + need_del[i]);
			}
			need_del.clear();
		}

		while (cache_veh_in_infos_.size() > 125)
		{
			cache_veh_in_infos_.erase(cache_veh_in_infos_.begin());
		}
	}

	//更新缓存车辆可能上报message
	int VehCacheInfo::UpdateCacheVehSendMessage(CapControlMsgVec &cap_control, int ind, SLVehicleState_t veh_state)
	{
		long frame_id = cache_veh_infos_.frame_id;
		int cap_id_tmp = -1;
		// VehInfo veh_info = cache_veh_infos_.veh_cache_infos[ind].veh_infos.back();

		CapBufCmd_t cap_state = SL_CAPBUF_CMD_REPLACE;
		
		if (veh_state == SL_CAR_STATE_IN_PRE){
			VehInfoCopy(cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_info, cache_veh_infos_.veh_cache_infos[ind].veh_infos.back());
			int cap_id = cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_pic_id;
			if (cap_id < 0) {
				cap_id = GetCapEnableId();
				if (cap_id < 0) {
					SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                	"GetCapEnableId failed !  veh_state {}, cap_id {}, line {}, channel {}",
					veh_state, cap_id, __LINE__, cache_veh_infos_.camera_channel);
					return ERR_CAPTURE_CONTROL_PICID_ABNORMAL;
				}
				cap_state = SL_CAPBUF_CMD_NEW;
			}

			int ret = UpdateCapControlMsg(cap_control, cap_state, cap_id, frame_id, cap_id_tmp);
			if (ret != SL_SUCCESS) {
				SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                	"UpdateCapControlMsg failed !  veh_state {}, cap_id {}, line {}, frame_id {}, now_f_id {}, channel {}",
					veh_state, cap_id, __LINE__, frame_id, cache_veh_infos_.frame_id, cache_veh_infos_.camera_channel);
				return ret;
			}

			if (cap_id_tmp >= 0) {
				cap_id = cap_id_tmp;
			}
			cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_pic_id = cap_id;
			cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_state = veh_state;
			cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.veh_state = veh_state;
			cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.frame_timestamp = cache_veh_infos_.frame_timestamp;
			cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_state_fraction = GetVehStateFraction(cache_veh_infos_.veh_cache_infos[ind].veh_infos.back(), ind);
			cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_locs.clear();
			cache_veh_infos_.veh_cache_infos[ind].veh_inpro_out_mess.veh_locs.push_back(cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().location);
		}
		else if (veh_state == SL_CAR_STATE_IN) {
			VehInfoCopy(cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_info, cache_veh_infos_.veh_cache_infos[ind].veh_infos.back());
			int cap_id = cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_pic_id;
			if (cap_id < 0) {
				cap_id = GetCapEnableId();
				if (cap_id < 0) {
					SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                	"GetCapEnableId failed !  veh_state {}, cap_id {}, line {}, channel {}",
					veh_state, cap_id, __LINE__, cache_veh_infos_.camera_channel);
					return ERR_CAPTURE_CONTROL_PICID_ABNORMAL;
				}
				cap_state = SL_CAPBUF_CMD_NEW;
			}

			int ret = UpdateCapControlMsg(cap_control, cap_state, cap_id, frame_id, cap_id_tmp);
			if (ret != SL_SUCCESS) {
				SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                	"UpdateCapControlMsg failed !  veh_state {}, cap_id {}, line {}, frame_id {}, now_f_id {}, channel {}",
					veh_state, cap_id, __LINE__, frame_id, cache_veh_infos_.frame_id, cache_veh_infos_.camera_channel);
				return ret;
			}
			if (cap_id_tmp >= 0) {
				cap_id = cap_id_tmp;
			}
			cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_pic_id = cap_id;
			cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_state = veh_state;
			cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.veh_state = veh_state;
			cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.frame_timestamp = cache_veh_infos_.frame_timestamp;
			cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_state_fraction = GetVehStateFraction(cache_veh_infos_.veh_cache_infos[ind].veh_infos.back(), ind);
			cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_locs.clear();
			cache_veh_infos_.veh_cache_infos[ind].veh_inpro_pro_mess.veh_locs.push_back(cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().location);
		}
		else if (veh_state == SL_CAR_STATE_STOP) {
			VehInfoCopy(cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info, cache_veh_infos_.veh_cache_infos[ind].veh_infos.back());
			int cap_id = cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_pic_id;
			if (cap_id < 0) {
				cap_id = GetCapEnableId();
				if (cap_id < 0) {
					SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                	"GetCapEnableId failed !  veh_state {}, cap_id {}, line {}, channel {}",
					veh_state, cap_id, __LINE__, cache_veh_infos_.camera_channel);
					return ERR_CAPTURE_CONTROL_PICID_ABNORMAL;
				}
				cap_state = SL_CAPBUF_CMD_NEW;
			}

			int ret = UpdateCapControlMsg(cap_control, cap_state, cap_id, frame_id, cap_id_tmp);
			if (ret != SL_SUCCESS) {
				SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                	"UpdateCapControlMsg failed !  veh_state {}, cap_id {}, line {}, frame_id {}, now_f_id {}, channel {}",
					veh_state, cap_id, __LINE__, frame_id, cache_veh_infos_.frame_id, cache_veh_infos_.camera_channel);
				return ret;
			}

			if (cap_id_tmp >= 0) {
				cap_id = cap_id_tmp;
			}
			cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_pic_id = cap_id;
			cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_state = veh_state;
			cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.veh_state = veh_state;
			cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.frame_timestamp = cache_veh_infos_.frame_timestamp;
			cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_state_fraction = GetVehStateFraction(cache_veh_infos_.veh_cache_infos[ind].veh_infos.back(), ind);

			Rect veh_standstill_loc_avg = cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().location;
			if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() > 10) {
				veh_standstill_loc_avg.x_ = 0;
				veh_standstill_loc_avg.y_ = 0;
				veh_standstill_loc_avg.width_ = 0;
				veh_standstill_loc_avg.height_ = 0;
				int veh_standstill_loc_count = 10;
				for (int i = 0; i < veh_standstill_loc_count; i++) {
					Rect veh_standstill_loc =
						cache_veh_infos_.veh_cache_infos[ind].veh_infos[cache_veh_infos_.veh_cache_infos[ind].veh_infos.size() - 1 - i].location;
					veh_standstill_loc_avg.x_ += veh_standstill_loc.x_;
					veh_standstill_loc_avg.y_ += veh_standstill_loc.y_;
					veh_standstill_loc_avg.width_ += veh_standstill_loc.width_;
					veh_standstill_loc_avg.height_ += veh_standstill_loc.height_;
				}

				veh_standstill_loc_avg.x_ = veh_standstill_loc_avg.x_ / veh_standstill_loc_count;
				veh_standstill_loc_avg.y_ = veh_standstill_loc_avg.y_ / veh_standstill_loc_count;
				veh_standstill_loc_avg.width_ = veh_standstill_loc_avg.width_ / veh_standstill_loc_count;
				veh_standstill_loc_avg.height_ = veh_standstill_loc_avg.height_ / veh_standstill_loc_count;


			}
			cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_locs.clear();
			cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_locs.push_back(veh_standstill_loc_avg);
			int space_id = VehInSpaceId(ind);
			cache_veh_infos_.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_chassis.space_id = space_id;
		}
		else if (veh_state == SL_CAR_STATE_STOP_LEAVE){
			VehInfoCopy(cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_info, cache_veh_infos_.veh_cache_infos[ind].veh_infos.back());
			int cap_id = cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_pic_id;
			if (cap_id < 0) {
				cap_id = GetCapEnableId();
				if (cap_id < 0) {
					SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                	"GetCapEnableId failed !  veh_state {}, cap_id {}, line {}, channel {}",
					veh_state, cap_id, __LINE__, cache_veh_infos_.camera_channel);
					return ERR_CAPTURE_CONTROL_PICID_ABNORMAL;
				}
				cap_state = SL_CAPBUF_CMD_NEW;
			}


			int ret = UpdateCapControlMsg(cap_control, cap_state, cap_id, frame_id, cap_id_tmp);
			if (ret != SL_SUCCESS) {
				SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                	"UpdateCapControlMsg failed !  veh_state {}, cap_id {}, line {}, frame_id {}, now_f_id {}, channel {}",
					veh_state, cap_id, __LINE__, frame_id, cache_veh_infos_.frame_id, cache_veh_infos_.camera_channel);
				return ret;
			}

			if (cap_id_tmp >= 0) {
				cap_id = cap_id_tmp;
			}
			cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_pic_id = cap_id;
			cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_state = veh_state;
			cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.veh_state = veh_state;
			cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.frame_timestamp = cache_veh_infos_.frame_timestamp;
			cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_state_fraction = GetVehStateFraction(cache_veh_infos_.veh_cache_infos[ind].veh_infos.back(), ind);
			cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_locs.clear();
			cache_veh_infos_.veh_cache_infos[ind].veh_outpro_std_mess.veh_locs.push_back(cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().location);
		}
		else if (veh_state == SL_CAR_STATE_LEAVING) {
			VehInfoCopy(cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info, cache_veh_infos_.veh_cache_infos[ind].veh_infos.back());
			int cap_id = cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_pic_id;
			if (cap_id < 0) {
				cap_id = GetCapEnableId();
				if (cap_id < 0) {
					SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                	"GetCapEnableId failed !  veh_state {}, cap_id {}, line {}, channel {}",
					veh_state, cap_id, __LINE__, cache_veh_infos_.camera_channel);
					return ERR_CAPTURE_CONTROL_PICID_ABNORMAL;
				}
				cap_state = SL_CAPBUF_CMD_NEW;
			}

			int ret = UpdateCapControlMsg(cap_control, cap_state, cap_id, frame_id, cap_id_tmp);
			if (ret != SL_SUCCESS) {
				SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                	"UpdateCapControlMsg failed !  veh_state {}, cap_id {}, line {}, frame_id {}, now_f_id {}, channel {}",
					veh_state, cap_id, __LINE__, frame_id, cache_veh_infos_.frame_id, cache_veh_infos_.camera_channel);
				return ret;
			}

			if (cap_id_tmp >= 0) {
				cap_id = cap_id_tmp;
			}
			cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_pic_id = cap_id;
			cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_state = veh_state;
			cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.veh_state = veh_state;
			cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.frame_timestamp = cache_veh_infos_.frame_timestamp;
			cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_state_fraction = GetVehStateFraction(cache_veh_infos_.veh_cache_infos[ind].veh_infos.back(), ind);
			cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_locs.clear();
			cache_veh_infos_.veh_cache_infos[ind].veh_outpro_pro_mess.veh_locs.push_back(cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().location);

		}
		else if (veh_state == SL_CAR_STATE_OUT) {
			VehInfoCopy(cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info, cache_veh_infos_.veh_cache_infos[ind].veh_infos.back());
			int cap_id = cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_pic_id;
			if (cap_id < 0) {
				cap_id = GetCapEnableId();
				if (cap_id < 0) {
					SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                	"GetCapEnableId failed !  veh_state {}, cap_id {}, line {}, channel {}",
					veh_state, cap_id, __LINE__, cache_veh_infos_.camera_channel);
					return ERR_CAPTURE_CONTROL_PICID_ABNORMAL;
				}
				cap_state = SL_CAPBUF_CMD_NEW;
			}


			int ret = UpdateCapControlMsg(cap_control, cap_state, cap_id, frame_id, cap_id_tmp);
			if (ret != SL_SUCCESS) {
				SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                	"UpdateCapControlMsg failed !  veh_state {}, cap_id {}, line {}, frame_id {}, now_f_id {}, channel {}",
					veh_state, cap_id, __LINE__, frame_id, cache_veh_infos_.frame_id, cache_veh_infos_.camera_channel);
				return ret;
			}

			if (cap_id_tmp >= 0) {
				cap_id = cap_id_tmp;
			}
			cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_pic_id = cap_id;
			cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_state = veh_state;
			cache_veh_infos_.veh_cache_infos[ind].veh_in_out_var.veh_state = veh_state;
			cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.frame_timestamp = cache_veh_infos_.frame_timestamp;
			cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_state_fraction = GetVehStateFraction(cache_veh_infos_.veh_cache_infos[ind].veh_infos.back(), ind);

			if (cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().frame_id != cache_veh_infos_.frame_id)
			{
				cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_locs.clear();
				cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_locs.push_back(Rect(0, 0, 15, 15));
				cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_info.location = Rect(0, 0, 15, 15);
			}
			else
			{
				cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_locs.clear();
				cache_veh_infos_.veh_cache_infos[ind].veh_outpro_out_mess.veh_locs.push_back(cache_veh_infos_.veh_cache_infos[ind].veh_infos.back().location);
			}
		}
		else {
			return -1;
		}

		return SL_SUCCESS;
	}
}
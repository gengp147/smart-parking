#include "park_order.h"
#include "veh_info_copy.h"
#include "../utils/sl_logger.h"

namespace sonli
{

	void VehParkOrder::UpdateFrameVeh(const VehInfoVec& vehicles, std::vector<aih::TimeInfo> park_frame_time)
    {
		// && park_frame_info[vehicles.frame_timestamp] == vehicles.frame_id
        if(park_frame_time.size() > 0 && park_frame_time[park_frame_time.size() - 1].local_time_ == vehicles.time_info.local_time_)
        {
            park_frame_veh_.emplace(vehicles.frame_timestamp, vehicles);
        }

        if(park_frame_veh_.size() > park_frame_time.size())
        {
            park_frame_veh_.erase(park_frame_veh_.begin());
        }
	}

	int IsHaveSameLpVehIn(const CacheVehInfoVec& cache_veh_infos, std::string lp_str_ind, int in_out) {
		int same_lp_veh_i = -1;
		if (lp_str_ind == "-") {
			return same_lp_veh_i;
		}

		for (int i = 0; i < cache_veh_infos.veh_cache_infos.size(); i++) {
			if (cache_veh_infos.veh_cache_infos[i].veh_in_out_var.is_disappear ||
				cache_veh_infos.veh_cache_infos[i].veh_infos.size() < 10) {
				continue;
			}

			if(cache_veh_infos.frame_timestamp != cache_veh_infos.veh_cache_infos[i].veh_infos.back().veh_timestamp){
				continue;
			}

			int veh_infos_size = cache_veh_infos.veh_cache_infos[i].veh_infos.size();
			int use_size = std::min(veh_infos_size, 10);
			int lp_count = 0;
			for (int j = 0; j < use_size; j++) {
				std::string lpStr = cache_veh_infos.veh_cache_infos[i].veh_infos[veh_infos_size - 1 - j].lp_info.lp_number.lp_str;
				float vlp_rec_confidence = 
					cache_veh_infos.veh_cache_infos[i].veh_infos[veh_infos_size - 1 - j].lp_info.lp_number.vlp_rec_confidence;
				if(in_out == 1){
					if( cache_veh_infos.veh_cache_infos[i].veh_infos[veh_infos_size - 1 - j].veh_chassis.space_res == SPACE_OUT &&
						cache_veh_infos.veh_cache_infos[i].veh_infos[veh_infos_size - 1 - j].cover_rat < 0.35){
						// && vlp_rec_confidence > 0.9
						if (lpStr == lp_str_ind)
						{
							lp_count++;
						}
					}
				}
				else{
					if( cache_veh_infos.veh_cache_infos[i].veh_infos[veh_infos_size - 1 - j].veh_chassis.space_res != SPACE_OUT &&
						cache_veh_infos.veh_cache_infos[i].veh_infos[veh_infos_size - 1 - j].cover_rat < 0.35){
						//  && vlp_rec_confidence > 0.9
						if (lpStr == lp_str_ind)
						{
							lp_count++;
						}
					}
				}
				
			}
			if (lp_count > 4) {
				same_lp_veh_i = i;
				return same_lp_veh_i;
			}
		}

		return same_lp_veh_i;
	}

	bool VehParkOrder::ParkVehOut2(const CacheVehInfoVec &cache_veh_infos,
								   std::vector<CacheVehInInfo> &cache_veh_in_infos,
								   VehInOutInfoVec &veh_inout_info_vec,
								   SpaceInfo space_info,
								   int &cache_veh_in_index_del)
	{
		int order_generate_type = 4;
		CacheVehInfo vehInfo;

		unsigned long long report_in_timestamp = 0;

		for (int space_id = 0; space_id < space_info.space_points.size(); space_id++)
		{
			if (parking_result_.parking_mess[space_id].parking_state == SL_PARKING_STATE_EMPTY)
			{
				int time_cha = (cache_veh_infos.frame_timestamp - parking_result_.parking_mess[space_id].refresh_time) / 1000;

				// SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
				// 				"------ParkVehOut2  refresh_time {}, space_id {}, frame_timestamp {}", 
				// 				parking_result_.parking_mess[space_id].refresh_time, 
				// 				space_id,
				// 				cache_veh_infos.frame_timestamp);
				if (time_cha < 50)
				{
					continue;
				}
				for (int i = 0; i < cache_veh_in_infos.size(); i++)
				{
					if (!cache_veh_in_infos[i].no_send_in &&
						cache_veh_in_infos[i].veh_info.lp_info.lp_number.lp_str != "-" &&
						cache_veh_in_infos[i].veh_info.veh_chassis.space_id == space_id)
					{
						time_cha = (cache_veh_infos.frame_timestamp - cache_veh_in_infos[i].last_timestamp) / 1000;
						if (time_cha < 120){
							continue;
						}
						time_cha = (cache_veh_infos.frame_timestamp - cache_veh_in_infos[i].judge_out_last_timestamp) / 1000;
						if (time_cha < 120){
							continue;
						}
						

						bool not_send = false;
						for (int ind = 0; ind < cache_veh_infos.veh_cache_infos.size(); ind++)
						{
							if (!cache_veh_infos.veh_cache_infos[ind].veh_in_out_var.is_disappear)
							{
								if ((cache_veh_infos.veh_cache_infos[ind].lp_info.lp_number.lp_str ==
									 cache_veh_in_infos[i].veh_info.lp_info.lp_number.lp_str) ||
									(cache_veh_infos.veh_cache_infos[ind].track_id ==
									 cache_veh_in_infos[i].veh_info.veh_track_id))
								{
									not_send = true;
									break;
								}
								continue;
							}

							if (cache_veh_infos.veh_cache_infos[ind].lp_info.lp_number.lp_str != "-" &&
								cache_veh_infos.veh_cache_infos[ind].lp_info.lp_number.lp_str ==
									cache_veh_in_infos[i].veh_info.lp_info.lp_number.lp_str)
							{
								// vehInfo = cache_veh_infos.veh_cache_infos[ind];
                                CacheVehInfoCopy(vehInfo, cache_veh_infos.veh_cache_infos[ind]);
								break;
							}
						}

						LpInfoCopy(vehInfo.veh_outpro_pro_mess.veh_info.lp_info, cache_veh_in_infos[i].veh_info.lp_info);
						vehInfo.veh_standstill_mess.veh_info.veh_chassis.space_id = space_id;

						report_in_timestamp = cache_veh_in_infos[i].report_in_timestamp;

						cache_veh_in_index_del = i;

						int same_lp_i = IsHaveSameLpVehIn(cache_veh_infos,
														  cache_veh_in_infos[i].veh_info.lp_info.lp_number.lp_str,
														  0);
						cache_veh_in_infos[cache_veh_in_index_del].judge_out_last_timestamp = cache_veh_infos.frame_timestamp;
						if (same_lp_i > -1 || not_send)
						{
							// SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
							// 	"------park out--4--: camera_id {} space_id {} have lp_str {}", 
							// 	cache_veh_infos.camera_channel, 
							// 	space_id,
							// 	cache_veh_in_infos[cache_veh_in_index_del].veh_info.lp_info.lp_number.lp_str);
						}
						else
						{
							SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
								"------park out--4--: camera_id {} space_id {} lp_str {} out 4", 
								cache_veh_infos.camera_channel, 
								space_id,
								cache_veh_in_infos[cache_veh_in_index_del].veh_info.lp_info.lp_number.lp_str);

							return ParkVehOutOrder(cache_veh_infos,
												   vehInfo,
												   veh_inout_info_vec,
												   space_info,
												   cache_veh_infos.frame_timestamp,
												   report_in_timestamp,
												   order_generate_type);
						}
					}
				}
			}
		}

		return false;
	}

	bool VehParkOrder::ParkVehOut(int ind,
								  CacheVehInfoVec& cache_veh_infos,
								  std::vector<CacheVehInInfo>& cache_veh_in_infos,
								  VehInOutInfoVec& veh_inout_info_vec,
								  SpaceInfo space_info,
								  int& cache_veh_in_index_del)
	{
		int space_id = cache_veh_infos.veh_cache_infos[ind].veh_standstill_mess.veh_info.veh_chassis.space_id;
		if (space_id < 0 ||
			space_id >= parking_result_.parking_num ||
			cache_veh_infos.veh_cache_infos[ind].veh_infos.size() < 1 ||
			!cache_veh_infos.veh_cache_infos[ind].veh_in_out_var.is_disappear ||
			!cache_veh_infos.veh_cache_infos[ind].veh_in_out_var.is_report_in ||
			cache_veh_infos.veh_cache_infos[ind].veh_in_out_var.is_report_out)
		{
			return false;
		}

		std::vector<int> spaceID = {};
		if (parking_result_.parking_mess[space_id].parking_state == SL_PARKING_STATE_EMPTY &&
			cache_veh_infos.frame_timestamp - parking_result_.parking_mess[space_id].refresh_time > 30000)
		{
			// SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
			// 					"------ParkVehOut  refresh_time {}, space_id {}, frame_timestamp {}", 
			// 					parking_result_.parking_mess[space_id].refresh_time, 
			// 					space_id,
			// 					cache_veh_infos.frame_timestamp);


			spaceID.emplace_back(space_id);
		}

		auto lastFrameSpaceID 		= cache_veh_infos.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_id;
		auto lastFrameSpaceRate 	= cache_veh_infos.veh_cache_infos[ind].veh_infos.back().veh_chassis.space_rat;
		if(lastFrameSpaceRate > 0.3 && lastFrameSpaceID!= -1 && space_id != lastFrameSpaceID )
		{
			if(parking_result_.parking_mess[lastFrameSpaceID].parking_state == SL_PARKING_STATE_EMPTY)
			{
				spaceID.emplace_back(lastFrameSpaceID);
			}
			else
			{
				spaceID.clear();
			}
		}

		int order_generate_type = 1;

		unsigned long long max_timestamp = 1;
		unsigned long long report_in_timestamp = 0;

		auto findOut = [&max_timestamp, &report_in_timestamp](int spaceID, int ind,
						std::vector<CacheVehInInfo> &cache_veh_in_infos, CacheVehInfoVec& cache_veh_infos, int& cache_veh_in_index_del)
		{
			for(int i = 0; i < cache_veh_in_infos.size(); i++){
				if (!cache_veh_in_infos[i].no_send_in &&
					cache_veh_in_infos[i].veh_info.lp_info.lp_number.lp_str != "-" &&
					cache_veh_in_infos[i].veh_info.veh_chassis.space_id == spaceID) {
					if(cache_veh_in_infos[i].last_timestamp > max_timestamp){
						if(cache_veh_infos.use_reid){
							cv::Mat feature_ind = cache_veh_infos.veh_cache_infos[ind].veh_infos[0].feature.clone();
							if(cache_veh_infos.veh_cache_infos[ind].veh_infos[0].veh_chassis.space_id < 0)
							{
								feature_ind = cache_veh_infos.veh_cache_infos[ind].veh_standstill_mess.veh_info.feature.clone();
							}
							cv::Mat AB = cache_veh_in_infos[i].veh_info.feature * feature_ind.t();
							float feature_cos = AB.at<float>(0, 0);
							if(feature_cos <  0.45)
							{
								// SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
								// 	"------park out--1--nosim--: camera_id {} lp_str {} feature_cos {}",
								// 	cache_veh_infos.camera_channel,
								// 	cache_veh_in_infos[i].veh_info.lp_info.lp_number.lp_str,
								// 	feature_cos);
								continue;
							}
						}
						

						max_timestamp = cache_veh_in_infos[i].last_timestamp;
						LpInfoCopy(cache_veh_infos.veh_cache_infos[ind].veh_outpro_pro_mess.veh_info.lp_info,
								cache_veh_in_infos[i].veh_info.lp_info);

						report_in_timestamp = cache_veh_in_infos[i].report_in_timestamp;

						cache_veh_in_index_del = i;

						int same_lp_i = IsHaveSameLpVehIn(cache_veh_infos,
														  cache_veh_in_infos[cache_veh_in_index_del].veh_info.lp_info.lp_number.lp_str,
														  0);
						
						if (same_lp_i > -1)
						{
							max_timestamp = 1;
						}
						else{
							SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
								"------park out--1--: camera_id {} lp_str {}",
								cache_veh_infos.camera_channel,
								cache_veh_in_infos[cache_veh_in_index_del].veh_info.lp_info.lp_number.lp_str);
						}
					}
				}
			}
		};

		if(!spaceID.empty() && cache_veh_infos.veh_cache_infos[ind].veh_infos.back().cover_rat < 0.5)
		{
			for (const auto& va: spaceID)
			{	
				findOut(va, ind, cache_veh_in_infos, cache_veh_infos, cache_veh_in_index_del);
			}

			order_generate_type = 2;
		}
		
		if (max_timestamp > 1)
		{
			return ParkVehOutOrder(cache_veh_infos,
								   cache_veh_infos.veh_cache_infos[ind],
								   veh_inout_info_vec,
								   space_info,
								   cache_veh_infos.frame_timestamp,
								   report_in_timestamp,
								   order_generate_type);
		}

		return false;
	}

	bool VehParkOrder::ParkVehOutOrder(const CacheVehInfoVec& cache_veh_infos,
									   const CacheVehInfo& cache_veh_info,
									   VehInOutInfoVec& veh_inout_info_vec,
									   SpaceInfo space_info,
									   unsigned long long now_timestamp,
									   unsigned long long report_in_timestamp,
									   int order_generate_type)
	{
		unsigned long long last_timestamp = 1;
		long veh_track_id = -1;
		if (cache_veh_info.veh_infos.size() < 1)
		{
			SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
				"ParkVehOutOrder out 4 ! channel {} no car feature",
				cache_veh_infos.camera_channel);
		}
		else
		{
			last_timestamp = cache_veh_info.veh_infos.back().veh_timestamp;
			veh_track_id = cache_veh_info.veh_infos.back().veh_track_id;
		}
		
		int space_id = cache_veh_info.veh_standstill_mess.veh_info.veh_chassis.space_id;

		unsigned long long out_timestamp = 0;
		unsigned long long pro_timestamp = 0;
		unsigned long long in_timestamp = 0;

		std::vector<unsigned long long> timestamps;
		for (auto pair : park_frame_veh_) {
			timestamps.push_back(pair.first);
		}
		std::sort(timestamps.begin(), timestamps.end());
		if(timestamps.size() < 4){
			return false;
		}

		if (cache_veh_info.veh_infos.size() < 1)
		{
			in_timestamp = timestamps[timestamps.size() - 3];
			pro_timestamp = timestamps[timestamps.size() - 2];
			out_timestamp = timestamps[timestamps.size() - 1];
		}
		else
		{
			in_timestamp = timestamps[0];
			pro_timestamp = timestamps[1];
			out_timestamp = timestamps[2];

			for (int i = 3; i < timestamps.size(); i++)
			{
				if (park_frame_veh_.find(timestamps[i - 1]) != park_frame_veh_.end())
				{
					bool have_veh = false;
					for (int j = 0; j < park_frame_veh_.at(timestamps[i - 1]).veh_info.size(); j++)
					{
						if (park_frame_veh_.at(timestamps[i - 1]).veh_info[j].veh_track_id == veh_track_id)
						{
							have_veh = true;
							// break;
						}
					}
					if (have_veh)
					{
						in_timestamp = timestamps[i - 2];
						pro_timestamp = timestamps[i - 1];
						out_timestamp = timestamps[i];
					}
				}

				if (timestamps[i] > last_timestamp)
				{
					out_timestamp = timestamps[i];
					// break;
				}
			}
		}

		if (park_frame_veh_.find(in_timestamp) == park_frame_veh_.end() ||
			park_frame_veh_.find(pro_timestamp) == park_frame_veh_.end() ||
			park_frame_veh_.find(out_timestamp) == park_frame_veh_.end())
		{
			SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error(
				"ParkVehOutOrder error ! channel {}, track_id {}, in_timestamp {}, pro_timestamp {}, out_timestamp {}",
				cache_veh_infos.camera_channel, cache_veh_info.track_id, in_timestamp, pro_timestamp, out_timestamp);

			return false;
		}

		VehInOutInfo veh_inout_info;
		veh_inout_info.in_out = 0;
		veh_inout_info.camera_channel = cache_veh_infos.camera_channel;
		veh_inout_info.veh_state_fraction = -1;

		VehInfo veh_out;
		VehInfo veh_in;
		VehInfo veh_pro;


		for(int i = 0; i < park_frame_veh_.at(in_timestamp).veh_info.size(); i++){
			if(park_frame_veh_.at(in_timestamp).veh_info[i].veh_track_id == veh_track_id){
				veh_in = park_frame_veh_.at(in_timestamp).veh_info[i];
                // VehInfoCopy(veh_in, park_frame_veh_[in_timestamp].veh_info[i]);
				break;
			}
		}

		for(int i = 0; i < park_frame_veh_.at(out_timestamp).veh_info.size(); i++){
			if(park_frame_veh_.at(out_timestamp).veh_info[i].veh_track_id == veh_track_id){
				veh_out = park_frame_veh_.at(out_timestamp).veh_info[i];
				break;
			}
		}

		for(int i = 0; i < park_frame_veh_.at(pro_timestamp).veh_info.size(); i++){
			if(park_frame_veh_.at(pro_timestamp).veh_info[i].veh_track_id == veh_track_id){
				veh_pro = park_frame_veh_.at(pro_timestamp).veh_info[i];
				break;
			}
		}

		veh_out.frame_id = park_frame_veh_.at(out_timestamp).frame_id;
		veh_out.veh_timestamp = park_frame_veh_.at(out_timestamp).frame_timestamp;
		veh_out.time_info = park_frame_veh_.at(out_timestamp).time_info;

		veh_in.frame_id = park_frame_veh_.at(in_timestamp).frame_id;
		veh_in.veh_timestamp = park_frame_veh_.at(in_timestamp).frame_timestamp;
		veh_in.time_info = park_frame_veh_.at(in_timestamp).time_info;

		veh_pro.frame_id = park_frame_veh_.at(pro_timestamp).frame_id;
		veh_pro.veh_timestamp = park_frame_veh_.at(pro_timestamp).frame_timestamp;
		veh_pro.time_info = park_frame_veh_.at(pro_timestamp).time_info;

		SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
			"ParkVehOutOrder veh_out {}, veh_in {}, veh_pro {}", 
			veh_out.time_info.pts_, veh_in.time_info.pts_, veh_pro.time_info.pts_);
			
		VehInfoCopy(veh_inout_info.out_veh_info, veh_out);
		VehInfoCopy(veh_inout_info.pro_veh_info, veh_pro);
		VehInfoCopy(veh_inout_info.in_veh_info, veh_in);

		if(veh_inout_info.out_veh_info.time_info.pts_ == 0){
			SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
                "ParkVehOutOrder out_veh_info  local_time_ {} pts_time {}", 
                cache_veh_infos.time_info.local_time_, cache_veh_infos.time_info.pts_);
			veh_inout_info.out_veh_info.time_info = cache_veh_infos.time_info;
		}
		if(veh_inout_info.pro_veh_info.time_info.pts_ == 0){
			SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
                "ParkVehOutOrder pro_veh_info local_time_ {} pts_time {}", 
                cache_veh_infos.time_info.local_time_, cache_veh_infos.time_info.pts_);
			veh_inout_info.pro_veh_info.time_info = cache_veh_infos.time_info;
		}

		if(veh_inout_info.in_veh_info.time_info.pts_ == 0){
			SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
                "ParkVehOutOrder in_veh_info  local_time_ {} pts_time {}", 
                cache_veh_infos.time_info.local_time_, cache_veh_infos.time_info.pts_);
			veh_inout_info.in_veh_info.time_info = cache_veh_infos.time_info;
		}

		LpNumberCopy(veh_inout_info.pro_veh_info.lp_info.lp_number, 
			cache_veh_info.veh_outpro_pro_mess.veh_info.lp_info.lp_number);
		veh_inout_info.pro_veh_info.lp_info.lp_color = cache_veh_info.veh_outpro_pro_mess.veh_info.lp_info.lp_color;

		LpNumberCopy(veh_inout_info.out_veh_info.lp_info.lp_number, 
			cache_veh_info.veh_outpro_pro_mess.veh_info.lp_info.lp_number);
		veh_inout_info.out_veh_info.lp_info.lp_color = cache_veh_info.veh_outpro_pro_mess.veh_info.lp_info.lp_color;

		LpNumberCopy(veh_inout_info.in_veh_info.lp_info.lp_number, 
			cache_veh_info.veh_outpro_pro_mess.veh_info.lp_info.lp_number);
		veh_inout_info.in_veh_info.lp_info.lp_color = cache_veh_info.veh_outpro_pro_mess.veh_info.lp_info.lp_color;

		veh_inout_info.out_veh_info.veh_chassis.space_id = space_id;
		veh_inout_info.pro_veh_info.veh_chassis.space_id = space_id;
		veh_inout_info.in_veh_info.veh_chassis.space_id = space_id;

		veh_inout_info.inout_timestamp = veh_inout_info.pro_veh_info.veh_timestamp;
		if (veh_inout_info.inout_timestamp <= report_in_timestamp){

			veh_inout_info.inout_timestamp = report_in_timestamp + 10000;

		}
		veh_inout_info.send_timestamp = now_timestamp;


		// if(veh_inout_info.pro_veh_info.veh_chassis.space_id < space_info.space_code.size()
		// 	&& veh_inout_info.pro_veh_info.veh_chassis.space_id > -1){
		// 	veh_inout_info.space_code = space_info.space_code[veh_inout_info.pro_veh_info.veh_chassis.space_id];
		// }
		// else{
		// 	veh_inout_info.space_code = space_info.space_code[0];
		// }

		veh_inout_info.order_generate_type = order_generate_type;

		veh_inout_info_vec.veh_inout_info.push_back(veh_inout_info);
		veh_inout_info_vec.size++;

		return true;
	}

} // namespace sonli

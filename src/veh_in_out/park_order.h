#ifndef _INOUT_PARK_ORDER_H_
#define _INOUT_PARK_ORDER_H_
#include "data_type.h"
#include "parking_info.h"

namespace sonli
{
	class VehParkOrder : public sonli::VehParkingInfo
	{

	public:
		void UpdateFrameVeh(const VehInfoVec& vehicles, std::vector<aih::TimeInfo> park_frame_time);

		bool ParkVehOut(int ind,
						CacheVehInfoVec& cache_veh_infos,
						std::vector<CacheVehInInfo>& cache_veh_in_infos,
						VehInOutInfoVec& veh_inout_info_vec,
						SpaceInfo space_info,
						int& cache_veh_in_index_del);

		bool ParkVehOut2(const CacheVehInfoVec& cache_veh_infos,
						std::vector<CacheVehInInfo>& cache_veh_in_infos,
						VehInOutInfoVec& veh_inout_info_vec,
						SpaceInfo space_info,
						int &cache_veh_in_index_del);

		bool ParkVehOutOrder(const CacheVehInfoVec& cache_veh_infos,
							 const CacheVehInfo& cache_veh_info,
							 VehInOutInfoVec& veh_inout_info_vec,
							 SpaceInfo space_info,
							 unsigned long long now_timestamp,
							 unsigned long long report_in_timestamp,
							 int order_generate_type);

	public:
		std::map<unsigned long long, VehInfoVec> park_frame_veh_;
	};
} // namespace sonli

#endif

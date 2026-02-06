#ifndef _INOUT_VEHICLE_INOUT_PROCESSER_H_
#define _INOUT_VEHICLE_INOUT_PROCESSER_H_
#include <vector>
#include "data_type.h"
#include "parking_config.h"
#include "veh_status_process.h"

namespace sonli
{

    class VehicleInOutProcesser : public sonli::VehicleInOutProcess
    {
    public:
        VehicleInOutProcesser(const int camera_id);
        ~VehicleInOutProcesser();

        void update(VehInfoVec& vehicles,
                    const ParkingInfoVec& space_info,
                    const std::vector<aih::TimeInfo>& park_frame_info,
                    ParkParameter module_park_param,
                    VehInOutInfoVec& veh_inout_info_vec,
                    CapControlMsgVec& cap_control_msg_vec,
                    ParkingResult& parking_result,
                    bool generate_suspected_order,
                    bool stability_enable,
					std::map<long, int>& park_frame_id);

        /***
         * @description: Get camera_id which camera be self-tracker own
         * @return {int} return camera id
         */
        int getCameraID() const
        {
            return camera_id_;
        }

        void saveVehCacheInfo();

    private:
        void loadVehCacheInfo();

        void UpdateExpandSpace();
        void UpdateSpaceInfo(const ParkingInfoVec& space_info, int img_w, int img_h);
        void UpdateParkParameter(ParkParameter module_park_param);

        void DetectVehPreProcess(VehInfoVec& vehicles);
        void CacheVehPreProcess(const VehInfoVec& vehicles);
        void SmoothSpaceRat(int ind);
        void CacheVehProcess(VehInOutInfoVec& veh_inout_info_vec, CapControlMsgVec& cap_control, const VehInfoVec& vehicles);

        void GenerateSuspectedOrder(VehInOutInfoVec& veh_inout_info_vec, CapControlMsgVec& cap_control);

        void UpdateVehInoutInfo(VehInOutInfoVec& veh_inout_info_vec);

		// void CrossParkingDetectPerFrame(const VehInfoVec vehicles, const CacheVehInfo cache) const;

// #ifdef ILLEGAL_DETECTION
// 		void IllegalDetectionExtent(const VehInfoVec& vehicles, const CacheVehInfo& cache);
// #endif

    private:
        int camera_id_ = -1;

        std::vector<std::string> order_name_list_ = {
			"camera_id",
			"time",
			"report_in_timestamp",
			"last_timestamp",
			"space_res",
			"space_rat",
			"space_id",
			"lp_type",
			"lp_color",
			"lp_str",
			"lp_count",
			"vlp_rec_confidence",
			"first_char_confidence",
			"second_char_confidence"};
    };

} // namespace sonli

#endif
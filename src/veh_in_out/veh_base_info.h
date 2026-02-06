#ifndef _INOUT_VEH_BASE_INFO_H_
#define _INOUT_VEH_BASE_INFO_H_

#include "data_type.h"
#include "../models_processor/model_process.h"
#include "../utils/time_utils.h"

namespace sonli {
class ParkingResult;
    class VehBaseInfo{
    public:
        void PrintInSpaceVehInfo();
        void PrintCacheVehInfo();
        void PrintSendMessage(const VehInOutInfoVec& veh_inout_info_vec);
        void PrintVehicles(const VehInfoVec& vehicles);
        void PrintCapCtrlMsg(const CapControlMsgVec & cap_control_msg_vec);
        void PrintParkingResult(const ParkingResult &parking_result);
        void PrintLog(const CapControlMsgVec &cap_control_msg_vec,
                      const VehInOutInfoVec &veh_inout_info_vec,
                      const VehInfoVec &vehicles,
                      const ParkingResult &parking_result);

        void LpColorConfirm(LPInfo& lp_info);
        void VehLpNumberCount(int ind, std::vector<std::pair<std::string, float>>& lp_str_vec);
        void VehLpColorCount(int ind, std::vector<SLLPColor_t>& lp_color_vec);
        bool IsLpNumberOkay(int lp_count, float lp_confidence);
        std::vector<std::pair<std::string, float>> GetVehLpStrVec(const std::vector<VehInfo>& veh_infos, std::vector<SLLPColor_t> &lp_color_vec);
        bool VehLpNumberStatus(int ind);

        bool JudgeImageTime(int ind, int inout);

        int IsHaveSameLocVeh(int ind, float& max_iou_rat, std::vector<std::pair<float, int>>& iou_rat_vec, 
            int loc_id, bool except_dis=false);//loc_id 0:first  1: stand  2: now
        int IsHaveSend(int ind);
        int IsHaveSameLpVehIn(int ind, std::string lp_str_ind, int in_out);

        bool IsSatisfyInSuspected(int ind);
        bool IsSatisfyIn(int ind);//是否满足入场条件
        void GetOutVehMaxIou(int ind);
        bool IsSatisfyOut(int ind);
        void IsDisAbnormal(int ind); //是否异常消失

        void UpdateVehStandstillLoc(const std::vector<VehInfo>& veh_infos, int ind);

        int VehInStatus(int ind); //判断车辆进场状态
        bool VehOutStatus(int ind); //判断车辆出场状态


        int VehInSpaceId(int ind);
        int CompareInOutVehLp(LPInfo veh_in_lp_info, LPInfo veh_out_lp_info);
        int GetVehStateFraction(const VehInfo& veh_info, int ind);
        float GetVehAvgCoverRat(const VehInfo& veh_info, int ind);

        std::shared_ptr<VehiclesCacheList> GetNewVehiclesCacheList(const VehInfoVec& vehicles);


    public:
        CacheVehInfoVec cache_veh_infos_;
        std::vector<CacheVehInInfo>  cache_veh_in_infos_; //停在车位内有车牌的车辆信息
        std::vector<CacheVehOutInfo>  cache_veh_out_infos_; //驶出的车辆信息

        ParkParameter park_param_;
        SpaceInfo space_info_;

        int send_veh_id_ = 1; //上报的车辆id，与车辆追踪id不同
        float cover_rat_conf_ = 0.25;

        int progress_start_count_ = 0;
        unsigned long long first_veh_timestamp_ = 0; //第一次处理的数据帧时间

        CapBufControl cap_buf_control_;

        int out_count_conf_ = 50;
        int out_move_count_conf_ = 20;

		std::string trend_of_space_rate = "false";

        bool stability_enable_ = false;

    };
} // namespace sonli 

#endif

#ifndef _INOUT_VEH_STATUS_PROCESS_H_
#define _INOUT_VEH_STATUS_PROCESS_H_

#include "veh_info_upload.h"

namespace sonli {
    class VehicleInOutProcess : public sonli::VehInfoUpload
    {
    public:
        bool CacheFrameChange(int ind,
                              std::string lp_str_pre,
                              float lp_str_pre_conf,
                              bool use_report_in = false,
                              SLVehicleState_t veh_state = SL_CAR_STATE_NOON);
        void VehMoveInProcess(CapControlMsgVec& cap_control, int ind);
        void VehInProcess(CapControlMsgVec& cap_control, int ind); //车辆驶入流程
        void VehStandProcess(CapControlMsgVec& cap_control,  int ind); //车辆停稳流程
        void VehDisProcess(CapControlMsgVec& cap_control, std::vector<int> cache_veh_dis_ind, VehInOutInfoVec& veh_inout_info_vec); //处理消失的车辆
    };
} // namespace sonli 

#endif

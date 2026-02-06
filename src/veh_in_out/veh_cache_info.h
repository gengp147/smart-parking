#ifndef _INOUT_VEH_CACHE_INFO_H_
#define _INOUT_VEH_CACHE_INFO_H_
#include "capture_control.h"

namespace sonli {
    class VehCacheInfo : public sonli::CapControl {
    public:
        void ResetInOutVar(int ind);
        int ResetInOutInfo(CapControlMsgVec& cap_control, int ind);
        int ResetVehInInfo(CapControlMsgVec& cap_control, int ind);
        int ResetVehOutInfo(CapControlMsgVec& cap_control, int ind);
        int ClearVehInInfo(CapControlMsgVec& cap_control, int ind); //清空缓存车辆

        int UpdateHaveSendVehInfo(CapControlMsgVec& cap_control);//更新缓存中上报过的车辆信息

        void CombineVehInfo(int index_i, int index_j) ;
        void CombineVehInfo(CapControlMsgVec& cap_control, std::vector<int>& todel_vec);
        void UpdateCacheVehInfoVec(CapControlMsgVec& cap_control);

        int GetToDelSpaceVehIndex(int ind, int veh_send_id);
        void DeleteSpaceVeh(int del_i);
        void AddSpaceVeh(int ind);
        void UpdateSpaceVehInfo();

        int UpdateCacheVehSendMessage(CapControlMsgVec &cap_control, int ind, SLVehicleState_t veh_state); // 更新缓存车辆可能上报message
    };
} // namespace sonli 

#endif

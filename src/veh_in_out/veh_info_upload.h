#ifndef _INOUT_VEH_INFO_UPLOAD_H_
#define _INOUT_VEH_INFO_UPLOAD_H_

#include "veh_cache_info.h"
#include "park_order.h"

namespace sonli {
    class VehInfoUpload : public sonli::VehCacheInfo, public sonli::VehParkOrder{
    public:
        void IsPassingVehIn(int ind, const std::vector<std::pair<float, int>>& iou_rat_vec);
        bool IsSendVehInInfo(int ind);//是否上传入场信息
        bool IsSendVehNOtDisOutInfo(CapControlMsgVec& cap_control, int ind);//是否上传出场信息
        bool IsSendNoLpVehOutInfo(int ind);//是否上传无车牌车辆出场信息
        bool IsSendVehOutInfoTwice(int ind);
        bool IsSendLowConf(int ind);
        
        void ReplaceVehInfo(CapControlMsgVec& cap_control, int ind);

        void EvidenceChainConfirm(int ind, int inout, std::string order_lp_str);
        void VehInProUpload(int ind); // 驶入过程上传
        void VehOutProUpload(CapControlMsgVec& cap_control, int ind); // 驶出过程上传
        void VehInUpload(VehInOutInfoVec& veh_inout_info_vec, CapControlMsgVec& cap_control, int ind); // 驶入订单上传
        void VehOutUpload(VehInOutInfoVec& veh_inout_info_vec, CapControlMsgVec& cap_control, int ind); // 驶出订单上传

        void UpdateToUploadVehMessage(CapControlMsgVec& cap_control, VehInOutInfoVec& veh_inout_info_vec); //更新上报车辆message

    };
} // namespace sonli 

#endif

#ifndef _INOUT_CAPTURE_CONTROL_H_
#define _INOUT_CAPTURE_CONTROL_H_
#include "veh_base_info.h"

namespace sonli {
    class CapControl : public sonli::VehBaseInfo {
    public:

        void InitCapControlMsg(CapControlMsgVec& cap_control, long frame_id);
        int  UpdateCapControlMsg(CapControlMsgVec& cap_control, CapBufCmd_t cap_buf_cmd, 
                                        int cap_buf_control_id, long frame_id, int& cap_id_tmp); //更新抓拍缓存控制信息
        int  GetCapEnableId();
        void ReProCapControl(CapControlMsgVec& cap_control_msg_vec, CapControlMsgVec& cap_control, int camera_id, long frame_id_now);
        
    };
} // namespace sonli 

#endif

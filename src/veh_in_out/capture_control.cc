#include "capture_control.h"
#include "error.h"
#include "../utils/sl_logger.h"

namespace sonli
{
    // 初始化抓拍缓存控制信息
    void CapControl::InitCapControlMsg(CapControlMsgVec &cap_control, long frame_id)
    {
        cap_buf_control_.frame_id_now_ = frame_id;
        cap_buf_control_.cap_id_ = -1;

        for (int i = 0; i < cap_buf_control_.cap_buf_control_del_ids_.size(); i++)
        {
            int del_cap_id = cap_buf_control_.cap_buf_control_del_ids_[i];
            if (del_cap_id >= max_cap_buf_len || del_cap_id < 0)
            {
                continue;
            }
            cap_buf_control_.cap_buf_control_enable_ids_.push_back(del_cap_id);
        }

        cap_buf_control_.cap_buf_control_del_ids_.clear();

        for (int pic_id = 0; pic_id < max_cap_buf_len; pic_id++)
        {

            if (cap_buf_control_.cap_id_del_[pic_id] > 0)
            {
                CapBufControlMsg cap_buf_control;
                cap_buf_control.cap_buff_cmd = SL_CAPBUF_CMD_DEL;
                cap_buf_control.enable = true;
                cap_buf_control.pic_id = pic_id;
                cap_buf_control.frame_id = cap_buf_control_.cap_id_frame_id_[pic_id];

                cap_control.cap_buf_control_vec.push_back(cap_buf_control);
                cap_buf_control_.cap_buf_control_del_ids_.push_back(pic_id);
                cap_buf_control_.cap_buf_control_num_[pic_id] = 0;
                cap_buf_control_.cap_id_frame_id_[pic_id] = -1;
                cap_buf_control_.cap_id_del_[pic_id] = -10;
            }
            else
            {
                if (cap_buf_control_.cap_id_del_[pic_id] > -1)
                {
                    cap_buf_control_.cap_id_del_[pic_id]++;
                }
            }
        }
    }

    // 更新抓拍缓存控制信息
    int CapControl::UpdateCapControlMsg(CapControlMsgVec &cap_control, CapBufCmd_t cap_buf_cmd,
                                        int pic_id, long frame_id, int &cap_id_tmp)
    {

        if (pic_id < 0 || pic_id >= max_cap_buf_len)
        {
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
                "UpdateCapControlMsg error! , line {}, channel {}, pic_id {}", 
                __LINE__, cache_veh_infos_.camera_channel, pic_id);
            return ERR_CAPTURE_CONTROL_PICID_ABNORMAL;
        }

        CapBufControlMsg cap_buf_control;
        cap_buf_control.cap_buff_cmd = cap_buf_cmd;
        cap_buf_control.enable = true;
        cap_buf_control.pic_id = pic_id;
        cap_buf_control.frame_id = frame_id;

        if (cap_buf_cmd == SL_CAPBUF_CMD_NEW)
        {

            if (frame_id != cap_buf_control_.frame_id_now_)
            {
                SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
                    "SL_CAPBUF_CMD_NEW error! , line {}, channel {}, frame_id_now_ {}, frame_id {}, f_id_now {}", 
                    __LINE__, cache_veh_infos_.camera_channel, cap_buf_control_.frame_id_now_, 
                    frame_id, cache_veh_infos_.frame_id);
                return ERR_CAPTURE_CONTROL_FRAMEID;
            }

            cap_buf_control_.cap_buf_control_num_[pic_id]++;
            if (cap_buf_control_.cap_buf_control_num_[pic_id] == 1)
            {
                cap_control.cap_buf_control_vec.push_back(cap_buf_control);
                cap_buf_control_.cap_id_frame_id_[pic_id] = frame_id;
                return SL_SUCCESS;
            }
            else if (cap_buf_control_.cap_buf_control_num_[pic_id] > 1)
            {
                if (cap_buf_control_.cap_id_frame_id_[pic_id] != frame_id)
                {
                    SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
                        "SL_CAPBUF_CMD_NEW error! , line {}, channel {}, cap_id_frame_id_ {}, frame_id {}, f_id_now {}", 
                        __LINE__, cache_veh_infos_.camera_channel, cap_buf_control_.cap_id_frame_id_[pic_id], 
                        frame_id, cache_veh_infos_.frame_id);

                    cap_buf_control_.cap_buf_control_num_[pic_id]--;
                    return ERR_CAPTURE_CONTROL_FRAMEID;
                }
                return SL_SUCCESS;
            }
            else
            {
                SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
                    "SL_CAPBUF_CMD_NEW error! , line {}, channel {}, cap_buf_control_num_ {}, frame_id {}, f_id_now {}", 
                    __LINE__, cache_veh_infos_.camera_channel, cap_buf_control_.cap_buf_control_num_[pic_id], 
                    frame_id, cache_veh_infos_.frame_id);

                cap_buf_control_.cap_buf_control_num_[pic_id] = 0;
                return ERR_CAPTURE_CONTROL_PICID;
            }
        }
        else if (cap_buf_cmd == SL_CAPBUF_CMD_REPLACE)
        {

            if (frame_id != cap_buf_control_.frame_id_now_)
            {
                SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
                    "SL_CAPBUF_CMD_REPLACE error! , line {}, channel {}, frame_id_now_ {}, frame_id {}, f_id_now {}", 
                    __LINE__, cache_veh_infos_.camera_channel, cap_buf_control_.frame_id_now_, frame_id, cache_veh_infos_.frame_id);
                return ERR_CAPTURE_CONTROL_FRAMEID;
            }

            if (cap_buf_control_.cap_id_frame_id_[pic_id] == frame_id)
            {
                cap_buf_control_.cap_buf_control_num_[pic_id]++;
                return SL_SUCCESS;
            }

            if (cap_buf_control_.cap_buf_control_num_[pic_id] == 1)
            {
                if (cap_buf_control_.cap_id_ < 0)
                {
                    cap_buf_control.frame_id_pre = cap_buf_control_.cap_id_frame_id_[pic_id];
                    cap_control.cap_buf_control_vec.push_back(cap_buf_control);
                    cap_buf_control_.cap_id_frame_id_[pic_id] = frame_id;
                    cap_buf_control_.cap_id_ = pic_id;
                }
                else
                {
                    int cap_id_tmp_tt = -1;
                    int ret1 = UpdateCapControlMsg(cap_control, SL_CAPBUF_CMD_DEL, pic_id,
                                                   cap_buf_control_.cap_id_frame_id_[pic_id], cap_id_tmp_tt);
                    if (ret1 != SL_SUCCESS)
                    {
                        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
                            "SL_CAPBUF_CMD_REPLACE error! , line {}, channel {}, frame_id_now_ {}, f_id_now {}", 
                            __LINE__, cache_veh_infos_.camera_channel, cap_buf_control_.frame_id_now_, cache_veh_infos_.frame_id);
                        return ret1;
                    }
                    cap_id_tmp = cap_buf_control_.cap_id_;
                    cap_buf_control_.cap_buf_control_num_[cap_buf_control_.cap_id_]++;
                }
            }
            // else if (cap_buf_control_.cap_buf_control_num_[pic_id] <= 0) {
            // 	cap_buf_control_.cap_buf_control_num_[pic_id] = 0;
            // 	SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( "SL_CAPBUF_CMD_REPLACE error ");
            // 	return ERR_CAPTURE_CONTROL_PICID;
            // }
            else
            {
                if (cap_buf_control_.cap_buf_control_num_[pic_id] <= 0)
                {
                    cap_buf_control_.cap_buf_control_num_[pic_id] = 0;
                }
                else
                {
                    cap_buf_control_.cap_buf_control_num_[pic_id]--;
                }

                if (cap_buf_control_.cap_id_ < 0)
                {
                    int cap_id = GetCapEnableId();

                    if (cap_id < 0 || cap_id >= max_cap_buf_len)
                    {
                        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
                            "SL_CAPBUF_CMD_REPLACE error , line {}, channel {}, cap_id {}", 
                            __LINE__, cache_veh_infos_.camera_channel, cap_id);
                        return ERR_CAPTURE_CONTROL_PICID_ABNORMAL;
                    }
                    cap_id_tmp = cap_id;
                    int ttt = -1;
                    int ret1 = UpdateCapControlMsg(cap_control, SL_CAPBUF_CMD_NEW, cap_id, frame_id, ttt);

                    if (ret1 != SL_SUCCESS)
                    {
                        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
                            "SL_CAPBUF_CMD_REPLACE error! , line {}, channel {}, frame_id {}, f_id_now {}", 
                            __LINE__, cache_veh_infos_.camera_channel, frame_id, cache_veh_infos_.frame_id);
                        return ret1;
                    }
                }
                else
                {
                    cap_id_tmp = cap_buf_control_.cap_id_;
                    cap_buf_control_.cap_buf_control_num_[cap_buf_control_.cap_id_]++;
                }
            }
        }

        else if (cap_buf_cmd == SL_CAPBUF_CMD_DEL)
        {

            if (cap_buf_control_.cap_id_frame_id_[pic_id] != frame_id)
            {
                SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
                    "SL_CAPBUF_CMD_DEL error! , line {}, channel {}, cap_id_frame_id_ {}, frame_id {}, f_id_now {}", 
                    __LINE__, cache_veh_infos_.camera_channel, 
                    cap_buf_control_.cap_id_frame_id_[pic_id], 
                    frame_id, cache_veh_infos_.frame_id);
                return ERR_CAPTURE_CONTROL_FRAMEID;
            }
            if (cap_buf_control_.cap_id_frame_id_[pic_id] < 0)
            {
                return SL_SUCCESS;
            }

            cap_buf_control_.cap_buf_control_num_[pic_id]--;
            if (cap_buf_control_.cap_buf_control_num_[pic_id] == 0)
            {
                cap_buf_control_.cap_id_del_[pic_id] = 1;
                cap_buf_control.frame_id = cap_buf_control_.cap_id_frame_id_[pic_id];

                cap_control.cap_buf_control_vec.push_back(cap_buf_control);
                cap_buf_control_.cap_buf_control_del_ids_.push_back(pic_id);
                cap_buf_control_.cap_buf_control_num_[pic_id] = 0;
                cap_buf_control_.cap_id_frame_id_[pic_id] = -1;
                cap_buf_control_.cap_id_del_[pic_id] = -10;

                return SL_SUCCESS;
            }
            else if (cap_buf_control_.cap_buf_control_num_[pic_id] > 0)
            {
                return SL_SUCCESS;
            }
            else
            {
                if (cap_buf_control_.cap_id_del_[pic_id] < 1)
                {
                    SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
                        "SL_CAPBUF_CMD_DEL error! , line {}, channel {}, cap_id_frame_id_ {}, f_id_now {}", 
                        __LINE__, cache_veh_infos_.camera_channel, 
                        cap_buf_control_.cap_id_frame_id_[pic_id], 
                        cache_veh_infos_.frame_id);
                    cap_buf_control_.needInit_ = true;
                    return ERR_CAPTURE_CONTROL_PICID;
                }
            }
        }

        return SL_SUCCESS;
    }

    int CapControl::GetCapEnableId()
    {

        if (cap_buf_control_.cap_id_ >= 0)
        {
            return cap_buf_control_.cap_id_;
        }
        if (cap_buf_control_.cap_buf_control_enable_ids_.size() > 0)
        {
            cap_buf_control_.cap_id_ = cap_buf_control_.cap_buf_control_enable_ids_[0];
            cap_buf_control_.cap_buf_control_enable_ids_.erase(cap_buf_control_.cap_buf_control_enable_ids_.begin());
        }

        return cap_buf_control_.cap_id_;
    }

    void CapControl::ReProCapControl(CapControlMsgVec &cap_control_msg_vec,
                                     CapControlMsgVec &cap_control, int camera_id, long frame_id_now)
    {
        if (cap_buf_control_.needInit_)
        {
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info("to init cap_buf_control_ pic_id: ");
            cap_control.cap_buf_control_vec.clear();
            cap_buf_control_.cap_buf_control_enable_ids_.clear();
            cap_buf_control_.cap_buf_control_del_ids_.clear();
            for (int pic_id = 0; pic_id < max_cap_buf_len; pic_id++)
            {
                cap_buf_control_.cap_buf_control_enable_ids_.push_back(pic_id);

                if (cap_buf_control_.cap_id_frame_id_[pic_id] < 0)
                {
                    cap_buf_control_.cap_id_del_[pic_id] = -10;
                    cap_buf_control_.cap_id_frame_id_[pic_id] = -1;
                    cap_buf_control_.cap_buf_control_num_[pic_id] = 0;
                    continue;
                }

                CapBufControlMsg cap_buf_control;
                cap_buf_control.cap_buff_cmd = SL_CAPBUF_CMD_DEL;
                cap_buf_control.enable = true;
                cap_buf_control.pic_id = pic_id;
                cap_buf_control.frame_id = cap_buf_control_.cap_id_frame_id_[pic_id];

                cap_control.cap_buf_control_vec.push_back(cap_buf_control);

                cap_buf_control_.cap_buf_control_num_[pic_id] = 0;
                cap_buf_control_.cap_id_frame_id_[pic_id] = -1;
                cap_buf_control_.cap_id_del_[pic_id] = -10;
            }

            cap_buf_control_.needInit_ = false;
        }

        bool is_have_frame_id_now = false;
        for (int i = 0; i < cap_control.cap_buf_control_vec.size(); i++)
        {
            cap_control.cap_buf_control_vec[i].channel = camera_id;

            if (cap_control.cap_buf_control_vec[i].frame_id == frame_id_now)
            {
                if (cap_control.cap_buf_control_vec[i].cap_buff_cmd != SL_CAPBUF_CMD_NEW &&
                    cap_control.cap_buf_control_vec[i].cap_buff_cmd != SL_CAPBUF_CMD_REPLACE)
                {
                    SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
                        "cap_control ---------157---------- channel {} cap_buff_cmd {}  pic_id {} frame_id {}",
                         cache_veh_infos_.camera_channel, cap_control.cap_buf_control_vec[i].cap_buff_cmd,
                          cap_control.cap_buf_control_vec[i].pic_id, cap_control.cap_buf_control_vec[i].frame_id);
                }
                is_have_frame_id_now = true;
            }

            CapBufControlMsg cap_buf_control = cap_control.cap_buf_control_vec[i];
            if (cap_control.cap_buf_control_vec[i].cap_buff_cmd == SL_CAPBUF_CMD_DEL)
            {
                cap_control_msg_vec.cap_buf_control_vec.push_back(cap_buf_control);
            }

            if (cap_control.cap_buf_control_vec[i].cap_buff_cmd == SL_CAPBUF_CMD_REPLACE)
            {
                if (cap_buf_control.frame_id == frame_id_now)
                {
                    is_have_frame_id_now = true;
                }
                cap_buf_control.cap_buff_cmd = SL_CAPBUF_CMD_DEL;
                cap_buf_control.frame_id = cap_buf_control.frame_id_pre;

                cap_control_msg_vec.cap_buf_control_vec.push_back(cap_buf_control);
            }
        }

        if (!is_have_frame_id_now)
        {
            CapBufControlMsg cap_buf_control;
            cap_buf_control.cap_buff_cmd = SL_CAPBUF_CMD_DEL;
            cap_buf_control.enable = true;
            cap_buf_control.pic_id = -100;
            cap_buf_control.frame_id = frame_id_now;
            cap_buf_control.channel = camera_id;
            cap_control_msg_vec.cap_buf_control_vec.push_back(cap_buf_control);
        }

        cap_control_msg_vec.size = cap_control_msg_vec.cap_buf_control_vec.size();
    }
} // namespace sonli
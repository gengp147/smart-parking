//
// Created by root on 8/7/25.
//
#include "lp_detection.h"
#include "../utils/utils.h"
#include "model_process.h"
#include "../utils/sl_logger.h"
#include "../utils/time_utils.h"

using namespace std;
using namespace sonli;

enum class InputOutputId
{
    INPUT_IMG_ID = 0,

    OUTPUT_LP_DET_TYPE_L_ID = 0,
};


LpDetection::LpDetection() : ModelProcess(){

}

void LpDetection::InitLpDetection(){

    std::vector<int32_t> input_dims =    getInputShape(0);

SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
        "LpDetection  input_dims index 0, input_dims[3]={}, input_dims[2]={}", input_dims[3], input_dims[2]);

//     std::vector<int32_t> output_dims =    getOutputShape(0);
// SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
//         "LpDetection  output_dims index 0, output_dims[0]={}, output_dims[1]={}, output_dims[2]={}", 
//         output_dims[0], output_dims[1], output_dims[2]);
}

LpDetection::~LpDetection() {
}


int32_t 
LpDetection::preProcess(void *data)
{
    int ret = CreateInputBuf(data);

    if (ret != SL_SUCCESS)
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("vlp det CreateInputBuf failed!");
        return ERR_PROCESS_FAILED;
    }

    ret =  CreateTaskBufAndWorkBuf();
    if (ret != SL_SUCCESS)
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("vlp det CreateTaskBufAndWorkBuf failed!");
        return ERR_PROCESS_FAILED;
    }

    return SL_SUCCESS;
}

int32_t 
LpDetection::inference(){
    return Execute();
}


template <typename T>
bool
IsImageEdgeLp(int frame_height, int frame_width, T box)
{
    if (frame_height < 200 || frame_width < 200)
    {
        return false;
    }

    int h_rat = frame_height / 100;
    int w_rat = frame_width / 100;
    if (box.x_ < w_rat || box.y_ < h_rat || (box.x_ + box.width_) > (frame_width - w_rat) ||
        (box.y_ + box.height_) > (frame_height - h_rat))
    {
        return true;
    }
    return false;
}

int32_t LpDetection::postProcess(VehInfoVec& vehicles){

    std::vector<int> dims =    getOutputShape(0);
    if(dims.size() != 3){
        return ERR_INITED;
    }
time_t s_time = getSystemTimeUS();

    auto *feat = reinterpret_cast<float *>(svp_acl_get_data_buffer_addr(
            svp_acl_mdl_get_dataset_buffer(   output_, static_cast<size_t>(InputOutputId::OUTPUT_LP_DET_TYPE_L_ID))));
// std::cout << " ------------12123----lpDetProcess postProcess feat time: " << getSystemTimeUS() - s_time << std::endl;

    // auto *obj = reinterpret_cast<float *>(svp_acl_get_data_buffer_addr(
    //         svp_acl_mdl_get_dataset_buffer(output_, 1)));

    // auto *objIdx = reinterpret_cast<float *>(svp_acl_get_data_buffer_addr(
    //         svp_acl_mdl_get_dataset_buffer(output_, 2)));

    std::vector<model_datatype::Object> proposals;
    // decode_lp_detect<float>(obj, objIdx, feat, 200, proposals, config_.vlp_num_class_, config_.vlp_confi_thresh_);

    decode_lp_detect<float>(feat, dims[2], proposals, config_.vlp_num_class_, config_.vlp_confi_thresh_, 1);

// std::cout << config_.vlp_confi_thresh_ << " -------0930---------lpDetProcess postProcess decode_lp_detect time: " << getSystemTimeUS() - s_time << std::endl;

    if (!proposals.empty())
    {
        qsort_descent_inplace<model_datatype::Object>(proposals, 0, proposals.size() - 1);
    }
    else
    {
        return SL_SUCCESS;
    }
// std::cout << " ----------------lpDetProcess postProcess qsort_descent_inplace time: " << getSystemTimeUS() - s_time << std::endl;

    std::vector<int> picked;
    nms_sorted_bboxes(proposals, picked, config_.vlp_nms_threshold_);
    float scale_w = (float)config_.img_width_ / config_.net_input_width_;
    float scale_h = (float)config_.img_height_ / config_.net_input_height_;

    // std::cout << "In lpDetProcess postProcess nms_sorted_bboxes, picked size: " << picked.size() << std::endl;

// std::cout << " ----------------lpDetProcess postProcess nms_sorted_bboxes time: " << getSystemTimeUS() - s_time << std::endl;



    // std::cout  << "**************************************************" << std::endl;
    // for (int i = 0; i < picked.size(); i++)
    // {
    //     std::cout  << "lp det proposals index: " << i << " proposals score: " << proposals[picked[i]].score_ << std::endl;
    //     std::cout << "location: (" << proposals[picked[i]].rect_.x_ << ", " << proposals[picked[i]].rect_.y_ << ", "
    //               << proposals[picked[i]].rect_.width_ << ", " << proposals[picked[i]].rect_.height_ << ")" << std::endl;
    //     std::cout << "-------------------------------------------------------------------------" << std::endl;
    // }
    // std::cout  << "**************************************************" << std::endl;

    for (int i = 0; i < picked.size(); i++)
    {

        int x0 = (proposals[picked[i]].rect_.x_) * scale_w;
        int y0 = (proposals[picked[i]].rect_.y_) * scale_h;
        int x1 = (proposals[picked[i]].rect_.x_ + proposals[picked[i]].rect_.width_) * scale_w;
        int y1 = (proposals[picked[i]].rect_.y_ + proposals[picked[i]].rect_.height_) * scale_h;

        int tempw = x1 - x0;
        int temph = y1 - y0;
        // pad
        x0 = x0 - tempw / 20;
        y0 = y0 - temph / 20;
        x1 = x1 + tempw / 20;
        y1 = y1 + temph / 20;

        x0 = std::max(std::min(x0, config_.img_width_ - 1), 0);
        y0 = std::max(std::min(y0, config_.img_height_ - 1), 0);
        x1 = std::max(std::min(x1, config_.img_width_ - 1), 0);
        y1 = std::max(std::min(y1, config_.img_height_ - 1), 0);

        // std::cout << "Config:w:" << config_.img_width_ << ", h:" << config_.img_height_ << std::endl;

        int lp_w = x1 - x0;
        int lp_h = y1 - y0;

        if (lp_w < 26 || lp_h < 16 || lp_w > 490 || lp_h >= 370)
        {
            // std::cout << "lp det proposals index continue: " << i << " lp w: " << lp_w << " lp h: " << lp_h << std::endl;
            continue;
        }

        sonli::BBox bbox(x0, y0, x1, y1);

        for (int veh_i = 0; veh_i < vehicles.veh_info.size(); veh_i++)
        {
            if (vehicles.veh_info[veh_i].veh_key_points[14].vis_score < 0.1 &&
                vehicles.veh_info[veh_i].veh_key_points[15].vis_score < 0.1)
            {
                continue;
            }
            // if (vehicles.veh_info[veh_i].lp_info.vlp_det_confidence > proposals[picked[i]].score_)
            // {
            //     // std::cout << "2lp det proposals index: " << i << " lp w: " << lp_w << " lp h: " << lp_h << std::endl;
            //     continue;
            // }

            Rect lp_loc = Rect(x0, y0, lp_w, lp_h);

            float car_rat = 0.0;
            float lp_rat = 0.0;

            float iou_rat = cal_iou(vehicles.veh_info[veh_i].location, lp_loc, car_rat, lp_rat);

            // std::cout << "lp det proposals index: " << i << " lp w: " << lp_w << " lp h: " << lp_h << " iou: " << iou_rat << " car_rat: " << car_rat << " lp_rat: " << lp_rat << std::endl;
            if (lp_rat > 0.95)
            {
                if (vehicles.veh_info[veh_i].lp_info.lp_rat > 0.95)
                {
                     bool is_rel = lp_veh_relate(vehicles.veh_info[veh_i], x0, y0, lp_w, lp_h);

                    // SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
                    // "---------lpdet---1----------- pre x {}, y {}, w {}, h {}, conf {} lp_rat {}; now x {}, y {}, w {}, h {}, conf {}, lp_rat {}; is_rel {}" , 
					// vehicles.veh_info[veh_i].lp_info.location.x_,
					// vehicles.veh_info[veh_i].lp_info.location.y_,
                    // vehicles.veh_info[veh_i].lp_info.location.width_,
                    // vehicles.veh_info[veh_i].lp_info.location.height_,
                    // vehicles.veh_info[veh_i].lp_info.vlp_det_confidence,
                    // vehicles.veh_info[veh_i].lp_info.lp_rat,
                    // lp_loc.x_, lp_loc.y_, lp_loc.width_, lp_loc.height_,
                    // proposals[picked[i]].score_, lp_rat,
                    // is_rel);

                    if(!is_rel || vehicles.veh_info[veh_i].lp_info.vlp_det_confidence - proposals[picked[i]].score_ > 0.2){
                        continue;
                    }

                    vehicles.veh_info[veh_i].lp_info.vlp_det_confidence = 0.0;
                    vehicles.veh_info[veh_i].lp_info.lp_status = PLATE_NO;
                    vehicles.veh_info[veh_i].lp_info.location = Rect(0, 0, 5, 5);
                    
                    continue;
                }
                vehicles.veh_info[veh_i].lp_info.lp_rat = lp_rat;
                vehicles.veh_info[veh_i].lp_info.vlp_det_confidence = proposals[picked[i]].score_;
                vehicles.veh_info[veh_i].lp_info.location = lp_loc;
                vehicles.veh_info[veh_i].lp_info.lp_status = PLATE_YES;

                lp_det_confirm(vehicles.veh_info[veh_i]);
            }
            else
            {
                continue;
            }

            if (vehicles.veh_info[veh_i].lp_info.vlp_det_confidence >= config_.vlp_confi_thresh_)
            {
                switch (proposals[picked[i]].label_)
                {
                case 0:
                    vehicles.veh_info[veh_i].lp_info.lp_color = SL_PLATE_COLOR_BLUE;
                    break;
                case 1:
                    vehicles.veh_info[veh_i].lp_info.lp_color = SL_PLATE_COLOR_GREENBLACK;
                    break;
                case 2:
                    vehicles.veh_info[veh_i].lp_info.lp_color = SL_PLATE_COLOR_YELLOW;
                    break;
                case 3:
                    vehicles.veh_info[veh_i].lp_info.lp_color = SL_PLATE_COLOR_BLACK;
                    break;
                case 4:
                    vehicles.veh_info[veh_i].lp_info.lp_color = SL_PLATE_COLOR_WHITE;
                    break;
                case 5:
                    vehicles.veh_info[veh_i].lp_info.lp_color = SL_PLATE_COLOR_GREENYELLOW;
                    break;
                case 6:
                    vehicles.veh_info[veh_i].lp_info.lp_color = SL_PLATE_COLOR_GREEN;
                    break;
                default:
                    vehicles.veh_info[veh_i].lp_info.lp_color = SL_PLATE_COLOR_UNKNOWN;
                }

                if (vehicles.veh_info[veh_i].lp_info.lp_color == SL_PLATE_COLOR_GREENYELLOW &&
                    vehicles.veh_info[veh_i].veh_type == SL_VEHICLE_TYPE_SEDAN)
                {
                    vehicles.veh_info[veh_i].lp_info.lp_color = SL_PLATE_COLOR_GREENBLACK;
                }

                break;
            }
        }
    }
    // SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
    //                             "lp detection get count:{}", picked.size());

    for (int veh_i = 0; veh_i < vehicles.veh_info.size(); veh_i++)
    {
        if (vehicles.veh_info[veh_i].lp_info.vlp_det_confidence < config_.vlp_confi_thresh_)
        {
            continue;
        }

        int x0 = vehicles.veh_info[veh_i].lp_info.location.x_;
        int y0 = vehicles.veh_info[veh_i].lp_info.location.y_;
        int lp_w = vehicles.veh_info[veh_i].lp_info.location.width_;
        int lp_h = vehicles.veh_info[veh_i].lp_info.location.height_;

        bool is_edge_lp = IsImageEdgeLp(config_.img_height_, config_.img_width_, vehicles.veh_info[veh_i].lp_info.location);
        if (is_edge_lp)
        {
            // SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
            //     "---------lpdet---EdgeLp----------- x {}, y {}, w {}, h {}, conf {}", 
            //     x0, y0, lp_w, lp_h, vehicles.veh_info[veh_i].lp_info.vlp_det_confidence);

            vehicles.veh_info[veh_i].lp_info.vlp_det_confidence = 0.0;
            vehicles.veh_info[veh_i].lp_info.lp_status = PLATE_NO;
            vehicles.veh_info[veh_i].lp_info.location = Rect(0, 0, 5, 5);
            continue;
        }
    }

    return SL_SUCCESS;
}

bool LpDetection::lp_veh_relate(const sonli::VehInfo& veh_info, int lp_x, int lp_y, int lp_w, int lp_h){

    int lp_x1 = lp_x;
    int lp_y1 = lp_y;
    int lp_x2 = lp_w + lp_x1;
    int lp_y2 = lp_h + lp_y1;

    int lp_x0 = veh_info.veh_key_points[15].x;
    int lp_y0 = veh_info.veh_key_points[15].y;

    if (veh_info.veh_key_points[14].vis_score > veh_info.veh_key_points[15].vis_score)
    {
        lp_x0 = veh_info.veh_key_points[14].x;
        lp_y0 = veh_info.veh_key_points[14].y;
    }

    int w_cha = 0;
    int h_cha = 0;
    if (lp_x0 < lp_x1)
    {
        w_cha = lp_x1 - lp_x0;
    }

    if (lp_x0 > lp_x2)
    {
        w_cha = lp_x0 - lp_x2;
    }

    if (lp_y0 < lp_y1)
    {
        h_cha = lp_y1 - lp_y0;
    }

    if (lp_y0 > lp_y2)
    {
        h_cha = lp_y0 - lp_y2;
    }

    float w_cha_r = (float)w_cha / lp_w;
    float h_cha_r = (float)h_cha / lp_h;

    if (w_cha_r > 1.0 || h_cha_r > 2.5)
    {
        return false;
    }

    return true;
}

void LpDetection::lp_det_confirm(VehInfo& veh_info)
{
    int lp_x1 = veh_info.lp_info.location.x_;
    int lp_y1 = veh_info.lp_info.location.y_;
    int lp_x2 = veh_info.lp_info.location.width_ + lp_x1;
    int lp_y2 = veh_info.lp_info.location.height_ + lp_y1;

    int lp_x0 = veh_info.veh_key_points[15].x;
    int lp_y0 = veh_info.veh_key_points[15].y;

    if (veh_info.veh_key_points[14].vis_score > veh_info.veh_key_points[15].vis_score)
    {
        lp_x0 = veh_info.veh_key_points[14].x;
        lp_y0 = veh_info.veh_key_points[14].y;
    }

    int w_cha = 0;
    int h_cha = 0;
    if (lp_x0 < lp_x1)
    {
        w_cha = lp_x1 - lp_x0;
    }

    if (lp_x0 > lp_x2)
    {
        w_cha = lp_x0 - lp_x2;
    }

    if (lp_y0 < lp_y1)
    {
        h_cha = lp_y1 - lp_y0;
    }

    if (lp_y0 > lp_y2)
    {
        h_cha = lp_y0 - lp_y2;
    }

    float w_cha_r = (float)w_cha / veh_info.lp_info.location.width_;
    float h_cha_r = (float)h_cha / veh_info.lp_info.location.height_;

    if (w_cha_r > 1.0 || h_cha_r > 2.5)
    {
        veh_info.lp_info.vlp_det_confidence = 0.0;
        veh_info.lp_info.lp_status = PLATE_NO;
        veh_info.lp_info.location = Rect(0, 0, 5, 5);
        veh_info.lp_info.lp_rat = 0.0;
    }
}

int32_t LpDetection::lpDetProcess(sonli::VehInfoVec &vehicles, void *data)
{

time_t s_time = getSystemTimeUS();
    int ret = preProcess(data);
// std::cout << " ----------------lpDetProcess preProcess time: " << getSystemTimeUS() - s_time << std::endl;

    if (ret != SL_SUCCESS)
    {
        return ret;
    }

    ret = inference();
// std::cout << " ----------------lpDetProcess inference time: " << getSystemTimeUS() - s_time << std::endl;

    if (ret != SL_SUCCESS)
    {
        return ret;
    }

    ret = postProcess(vehicles);
// std::cout << " ----------------lpDetProcess postProcess time: " << getSystemTimeUS() - s_time << std::endl;
    if (ret != SL_SUCCESS)
    {
        return ret;
    }

    return SL_SUCCESS;
}




int32_t LpDetection::lpDetProcess(void *data, LPInfoVecPtr lp_infos_ptr)
{

time_t s_time = getSystemTimeUS();
    int ret = preProcess(data);
std::cout << " ----------------lpDetProcess preProcess time: " << getSystemTimeUS() - s_time << std::endl;

    if (ret != SL_SUCCESS)
    {
        return ret;
    }

    ret = inference();
std::cout << " ----------------lpDetProcess inference time: " << getSystemTimeUS() - s_time << std::endl;

    if (ret != SL_SUCCESS)
    {
        return ret;
    }

    ret = postProcess(lp_infos_ptr);
std::cout << lp_infos_ptr->size << " ----------------lpDetProcess postProcess time: " << getSystemTimeUS() - s_time << std::endl;
    if (ret != SL_SUCCESS)
    {
        return ret;
    }

    return SL_SUCCESS;
}



int32_t LpDetection::postProcess(sonli::LPInfoVecPtr lp_infos_ptr){

    std::vector<int> dims =    getOutputShape(0);
    if(dims.size() != 3){
        return ERR_INITED;
    }
time_t s_time = getSystemTimeUS();

    auto *feat = reinterpret_cast<float *>(svp_acl_get_data_buffer_addr(
            svp_acl_mdl_get_dataset_buffer(   output_, static_cast<size_t>(InputOutputId::OUTPUT_LP_DET_TYPE_L_ID))));
std::cout << " ------------1009----lpDetProcess postProcess feat time: " << getSystemTimeUS() - s_time << std::endl;

    std::vector<model_datatype::Object> proposals;
    decode_lp_detect<float>(feat, 7056, proposals, config_.vlp_num_class_, config_.vlp_confi_thresh_, 1);

std::cout << config_.vlp_confi_thresh_ << 
" -------1009---------lpDetProcess postProcess decode_lp_detect time: " << getSystemTimeUS() - s_time 
<< std::endl;

    if (!proposals.empty())
    {
        qsort_descent_inplace<model_datatype::Object>(proposals, 0, proposals.size() - 1);
    }
    else
    {
        return SL_SUCCESS;
    }
// std::cout << " ----------------lpDetProcess postProcess qsort_descent_inplace time: " << getSystemTimeUS() - s_time << std::endl;

    std::vector<int> picked;
    nms_sorted_bboxes(proposals, picked, config_.vlp_nms_threshold_);
    float scale_w = (float)config_.img_width_ / config_.net_input_width_;
    float scale_h = (float)config_.img_height_ / config_.net_input_height_;

    std::cout << "In lpDetProcess postProcess nms_sorted_bboxes, picked size: " << picked.size() << std::endl;

// std::cout << " ----------------lpDetProcess postProcess nms_sorted_bboxes time: " << getSystemTimeUS() - s_time << std::endl;

    for (int i = 0; i < picked.size(); i++)
    {
        int x0 = (proposals[picked[i]].rect_.x_) * scale_w;
        int y0 = (proposals[picked[i]].rect_.y_) * scale_h;
        int x1 = (proposals[picked[i]].rect_.x_ + proposals[picked[i]].rect_.width_) * scale_w;
        int y1 = (proposals[picked[i]].rect_.y_ + proposals[picked[i]].rect_.height_) * scale_h;
        int tempw = x1 - x0;
        int temph = y1 - y0;
        // pad
        x0 = x0 - tempw / 20;
        y0 = y0 - temph / 20;
        x1 = x1 + tempw / 20;
        y1 = y1 + temph / 20;
        x0 = std::max(std::min(x0, config_.img_width_ - 1), 0);
        y0 = std::max(std::min(y0, config_.img_height_ - 1), 0);
        x1 = std::max(std::min(x1, config_.img_width_ - 1), 0);
        y1 = std::max(std::min(y1, config_.img_height_ - 1), 0);
        // std::cout << "Config:w:" << config_.img_width_ << ", h:" << config_.img_height_ << std::endl;

        int lp_w = x1 - x0;
        int lp_h = y1 - y0;
        if (lp_w < 26 || lp_h < 16 || lp_w > 490 || lp_h >= 370)
        {
            std::cout << "lp det proposals index continue: " << i << " lp w: " << lp_w << " lp h: " << lp_h << std::endl;
            continue;
        }
        LPInfo lp_info;
        Rect lp_loc = Rect(x0, y0, lp_w, lp_h);
        lp_info.vlp_det_confidence = proposals[picked[i]].score_;
        lp_info.location = lp_loc;
        lp_info.lp_status = PLATE_YES;
        if (lp_info.vlp_det_confidence >= config_.vlp_confi_thresh_)
        {
            switch (proposals[picked[i]].label_)
            {
            case 0:
                lp_info.lp_color = SL_PLATE_COLOR_BLUE;
                break;
            case 1:
                lp_info.lp_color = SL_PLATE_COLOR_GREENBLACK;
                break;
            case 2:
                lp_info.lp_color = SL_PLATE_COLOR_YELLOW;
                break;
            case 3:
                lp_info.lp_color = SL_PLATE_COLOR_BLACK;
                break;
            case 4:
                lp_info.lp_color = SL_PLATE_COLOR_WHITE;
                break;
            case 5:
                lp_info.lp_color = SL_PLATE_COLOR_GREENYELLOW;
                break;
            case 6:
                lp_info.lp_color = SL_PLATE_COLOR_GREEN;
                break;
            default:
                lp_info.lp_color = SL_PLATE_COLOR_UNKNOWN;
            }
        }
        lp_infos_ptr->lp_info.push_back(lp_info);
        lp_infos_ptr->size++;
    }

    return SL_SUCCESS;
}

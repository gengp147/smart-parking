//
// Created by root on 8/8/25.
//

#include "lp_recognition.h"
#include "char_code.h"
#include "model_process.h"
#include "../utils/utils.h"
#include "../utils/sl_logger.h"
#include "../utils/time_utils.h"

#include <memory>
#include <regex>

using namespace std;
using namespace sonli;

enum class InputOutputId
{
    INPUT_IMG_ID = 0,

    OUTPUT_LP_REG_TYPE_L_ID = 0,
};



LPRecognition::LPRecognition():ModelProcess()
{
    proc_ptr_ = std::move(std::unique_ptr<ImageProcess>(new ImageProcess()));
}

LPRecognition::~LPRecognition() {
    sample_svp_mmz_free(dst_img_.phys_addr[0], dst_img_.virt_addr[0]);
}

void LPRecognition::InitLPRecognition()
{
    std::vector<int32_t> input_dims =     getInputShape(0);

    SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
        "LPRecognition  input_dims index 0, input_dims[3]={}, input_dims[2]={}", 
        input_dims[3], input_dims[2]);

    int ret = proc_ptr_->init(input_dims[3], input_dims[2]);
    if (ret != SL_SUCCESS)
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
        "lprec proc_ptr_  init failed! ret {}", ret);
    }

    ret = proc_ptr_->imageResizeInit(&dst_img_);
    if (ret != SL_SUCCESS)
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
        "lprec proc_ptr_  imageResizeInit failed! ret {}", ret);
    }

    dims_ =     getOutputShape(0);
    SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
        "LPRecognition  output_dims index 0, output_dims[0]={}, output_dims[1]={}, output_dims[2]={}", 
        dims_[0], dims_[1], dims_[2]);

    stride_ =     getModelOutputStride(0);

    SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
        "LPRecognition  getModelOutputStride {}", stride_);

}

int32_t 
LPRecognition::preProcess(void *data)
{
// time_t s_time = getSystemTimeUS();
    int ret =     CreateInputBuf(data);
// std::cout << " ----------------lprec CreateInputBuf time: " << getSystemTimeUS() - s_time << std::endl;
    if (ret != SL_SUCCESS)
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("vlp rec CreateInputBuf failed!");
        return ERR_PROCESS_FAILED;
    }

    ret =     CreateTaskBufAndWorkBuf();

// std::cout << " ----------------lprec CreateInputBuf time: " << getSystemTimeUS() - s_time << std::endl;
    if (ret != SL_SUCCESS)
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("vlp rec CreateTaskBufAndWorkBuf failed!");
        return ERR_PROCESS_FAILED;
    }

    return SL_SUCCESS;
}

int32_t 
LPRecognition::inference(){
    return  Execute();
}

void LPRecognition::lpColorPostprocess(LPInfo& lp_info)
{
    if (lp_info.lp_number.size > 6)
    {
        if (lp_info.lp_number.size == 8)
        {
            if (lp_info.lp_color != SL_PLATE_COLOR_GREENYELLOW &&
                lp_info.lp_color != SL_PLATE_COLOR_GREENBLACK)
            {
                lp_info.lp_color = SL_PLATE_COLOR_GREENBLACK;
            }
        }

        if (lp_info.lp_number.size == 7)
        {
            std::string str_1 = lp_info.lp_number.lp_number[0];
            std::string str_7 = lp_info.lp_number.lp_number[6];
            if (lp_info.lp_color == SL_PLATE_COLOR_WHITE && str_7 != "警")
            {
                lp_info.lp_color = SL_PLATE_COLOR_BLUE;
            }

            if (lp_info.lp_color == SL_PLATE_COLOR_BLACK &&
                (str_7 != "领" && str_7 != "使" && str_7 != "澳" && str_7 != "港"))
            {
                lp_info.lp_color = SL_PLATE_COLOR_BLUE;
            }

            if (str_7 == "学" || str_7 == "挂")
            {
                lp_info.lp_color = SL_PLATE_COLOR_YELLOW;
            }
            if (str_7 == "警")
            {
                lp_info.lp_color = SL_PLATE_COLOR_WHITE;
            }
        }
    }

    if (lp_info.lp_number.lp_str == "-")
    {
        lp_info.lp_color = SL_PLATE_COLOR_UNKNOWN;
    }
}

int32_t LPRecognition::postProcess(LPNumber &lp_number_info)
{
    std::vector<std::vector<float>> topk_values; // 24 X k, k == 1
    std::vector<std::vector<int>> topk_indices;

    if(dims_.size() != 3){
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error(
            	"vlp rec postprocess error ! dims_.size {} ", 
                dims_.size());
        return ERR_PROCESS_FAILED;
    }

    auto *feat = reinterpret_cast<float *>(svp_acl_get_data_buffer_addr(
            svp_acl_mdl_get_dataset_buffer(    output_, static_cast<size_t>(InputOutputId::OUTPUT_LP_REG_TYPE_L_ID))));
    topK<float>(feat, 1, dims_, topk_values, topk_indices, 76);

    std::wstring lp_wstr;

    std::vector<int> select_idx;
    int decode_len = dims_[1];
    int prev_idx = config_.blank_idx_;
    for (int t = 0; t < decode_len; t++)
    {
        int tmp_value = topk_indices[t][0]; // max_idx;
        if (tmp_value != prev_idx && tmp_value != config_.blank_idx_)
        {
            select_idx.emplace_back(t);
        }
        prev_idx = tmp_value;
    }

    if (select_idx.size() <= 0)
    {
        lp_number_info.vlp_status = PLATE_NUMBER_NONE;
        return SL_SUCCESS;
    }

    std::vector<float> scores;
    std::vector<int> label_idx;
    float vlpr_confidence = 0;
    float vlpr_min_conf = 1.0;
    std::vector<float> chars_prediction;
    for (int i = 0; i < select_idx.size(); i++)
    {
        float char_score = topk_values[select_idx[i]][0];
        if (char_score < 0.1)
        {
            continue;
        }

        chars_prediction.push_back(char_score);
        int label = topk_indices[select_idx[i]][0];
        scores.emplace_back(char_score);
        label_idx.emplace_back(label);
        vlpr_confidence += char_score;
        if (char_score < vlpr_min_conf)
        {
            vlpr_min_conf = char_score;
        }
    }

    vlpr_confidence /= select_idx.size();

    // lp_info->vlp_rec_confidence = vlpr_confidence;
    lp_number_info.vlp_rec_min_conf = vlpr_min_conf;
    lp_number_info.vlp_rec_confidence = vlpr_confidence * 0.7 + vlpr_min_conf * 0.3;

    if (vlpr_confidence < config_.vlpr_confi_thresh_)
    {
        lp_number_info.vlp_status = PLATE_NUMBER_NONE;
// std::cout << " vlpr_confidence " << vlpr_confidence << " " << dims_[1] << " " << dims_[2] <<std::endl;
        return SL_SUCCESS;
    }
    std::string str;
    std::wregex reg;
    bool lp_char_type_check = true;
    switch (SLLPCharType_t((int)config_.priorty_province_))
    {
    case SL_CHARTYPE_CHARACTER:
        for (int i = 0; i < label_idx.size(); i++)
        {
            int label = label_idx[i];
            if (label <= SL_DIGITAL_START || label > SL_ALPHABET_END)
            {
                lp_char_type_check = false;
            }
            memcpy(lp_number_info.lp_number[i], label2char[label], sizeof(label2char[label]));
        }
        lp_number_info.size = label_idx.size();
        if (!lp_char_type_check)
        {
            lp_number_info.vlp_status = PLATE_NUMBER_ERROR;
            return SL_SUCCESS;
        }

        lp_number_info.vlp_status = PLATE_NUMBER_CORRECT;
        for (int idx = 0; idx < lp_number_info.size; idx++)
        {
            str += lp_number_info.lp_number[idx];
        }
        reg = dig_let_regix_reg;
        lp_wstr = utf8ToUnicode(str);
        lp_number_info.lp_str = str;
        break;
        ;
    case SL_CHARTYPE_ONLYALPHABET:
        for (int i = 0; i < label_idx.size(); i++)
        {
            int label = label_idx[i];
            if (label < SL_ALPHABET_START || label > SL_ALPHABET_END)
            {
                lp_char_type_check = false;
            }
            memcpy(lp_number_info.lp_number[i], label2char[label], sizeof(label2char[label]));
        }
        lp_number_info.size = label_idx.size();
        if (!lp_char_type_check)
        {
            lp_number_info.vlp_status = PLATE_NUMBER_ERROR;
            return SL_SUCCESS;
        }
        lp_number_info.vlp_status = PLATE_NUMBER_CORRECT;
        for (int idx = 0; idx < lp_number_info.size; idx++)
        {
            str += lp_number_info.lp_number[idx];
        }

        reg = letter_regix_reg;
        lp_wstr = utf8ToUnicode(str);
        lp_number_info.lp_str = str;
        break;

    case SL_CHARTYPE_ONLYDIGITAL:
        for (int i = 0; i < label_idx.size(); i++)
        {
            int label = label_idx[i];
            if (label < SL_DIGITAL_START || label > SL_DIGITAL_END)
            {
                lp_char_type_check = false;
            }
            memcpy(lp_number_info.lp_number[i], label2char[label], sizeof(label2char[label]));
        }
        lp_number_info.size = label_idx.size();
        if (!lp_char_type_check)
        {
            lp_number_info.vlp_status = PLATE_NUMBER_ERROR;
            return SL_SUCCESS;
        }
        lp_number_info.vlp_status = PLATE_NUMBER_CORRECT;
        for (int idx = 0; idx < lp_number_info.size; idx++)
        {
            str += lp_number_info.lp_number[idx];
        }

        reg = digial_regix_reg;
        lp_wstr = utf8ToUnicode(str);
        lp_number_info.lp_str = str;
        break;
        // all china or any province
    default:
        // find first char for simplifier of province or char "shi"
        int char_idx = -1;
        for (int i = 0; i < label_idx.size(); i++)
        {
            int label = label_idx[i];
            if (label >= SL_HANZI_BEIJING && label <= SL_HANZI_XINJIANG)
            {
                char_idx = i;
                break;
            }
            else if (label == SL_HANZI_SHI)
            {
                char_idx = 0;
                break;
            }
        }

        if (char_idx < 0)
        {
            for (int i = 0; i < label_idx.size(); i++)
            {
                memcpy(lp_number_info.lp_number[i], label2char[label_idx[i]], sizeof(label2char[label_idx[i]]));
            }
            lp_number_info.size = label_idx.size();
            lp_number_info.vlp_status = PLATE_NUMBER_ERROR;
            return SL_SUCCESS;
        }
        int lp_length = label_idx.size() - char_idx;
        lp_number_info.first_char_confidence = scores[char_idx];
        lp_number_info.second_char_confidence = scores[char_idx + 1];
        for (int i = 0; i < lp_length; i++)
        {
            memcpy(lp_number_info.lp_number[i], label2char[label_idx[i]], sizeof(label2char[label_idx[i]]));
        }
        lp_number_info.size = lp_length;
        if (SLLPCharType_t((int)config_.priorty_province_) != SL_HANZI_CHN)
        {
            // not all china
            if (lp_number_info.first_char_confidence < config_.province_char_confi_thresh_)
            {
                memcpy(lp_number_info.lp_number[0], label2char[config_.priorty_province_], sizeof(label2char[config_.priorty_province_]));
            }
            if (lp_number_info.second_char_confidence < config_.city_char_confi_thresh_ && config_.priorty_city_ != 0)
            {
                memcpy(lp_number_info.lp_number[1], label2char[config_.priorty_city_], sizeof(label2char[config_.priorty_city_]));
            }
        }
        lp_number_info.vlp_status = PLATE_NUMBER_CORRECT;
        for (int idx = 0; idx < lp_number_info.size; idx++)
        {
            str += lp_number_info.lp_number[idx];
        }
        reg = zh_cn_lp_regix_reg;
        lp_wstr = utf8ToUnicode(str);
        lp_number_info.lp_str = str;
        break;
    }

    // std::cout << "lp:[" << lp_number_info.lp_str << "]" << "scores:(";
    // for (const auto& score : chars_prediction)
    // {
        // std::cout << score << ",";
    // }
    // std::cout << std::endl;

    std::wsmatch match;
    if (!std::regex_match(lp_wstr, match, reg))
    {
        lp_number_info.vlp_status = PLATE_NUMBER_ERROR;
    }

    if (lp_number_info.vlp_status != PLATE_NUMBER_CORRECT || lp_number_info.vlp_rec_confidence < config_.vlpr_confi_thresh_)
    {
        lp_number_info.lp_str = "-";
    }

    if (lp_number_info.vlp_rec_confidence > 0.05 && lp_number_info.vlp_rec_confidence < config_.vlpr_confi_thresh_)
    {
        lp_number_info.lp_str = "-";
        lp_number_info.vlp_status = PLATE_NUMBER_CORRECT;
    }
    return SL_SUCCESS;
}

bool LPRecognition::checkPriorty(int priorty, const std::string &opt)
{
    if (opt == "priorty_province")
    {
        return (priorty >= SL_HANZI_CHN && priorty <= SL_HANZI_XINJIANG) ||
               priorty == SL_CHARTYPE_CHARACTER ||
               priorty == SL_CHARTYPE_ONLYALPHABET ||
               priorty == SL_CHARTYPE_ONLYDIGITAL;
    }
    else if (opt == "priorty_city")
    {
        return (priorty >= SL_ALPHABET_START && priorty <= SL_ALPHABET_END) ||
               priorty == SL_HANZI_CHN;
    }
    else
    {
        return false;
    }
}

bool LPRecognition::checkConfidence(float confi)
{
    return confi >= 0 && confi <= 1;
}


int32_t LPRecognition::lpRecProcess(sonli::VehInfoVec &vehicles, FrameImageData* imgae_data, bool lp_qa_enable)
{
    float wrat = vehicles.frame_width / 2688.0;
    if (wrat > 1.0)
    {
        wrat = 1.0;
    }

    for (int i = 0; i < vehicles.veh_info.size(); i++)
    {
        if (vehicles.veh_info[i].lp_info.vlp_det_confidence < 0.1)
        {
        	continue;
        }
        if (vehicles.veh_info[i].lp_info.location.height_ < 16 ||
        	vehicles.veh_info[i].lp_info.location.width_ < 32)
        {
        	continue;
        }
        auto lp_region = vehicles.veh_info[i].lp_info.location;
        if (lp_region.width_ < config_.vlp_min_width_ * wrat || lp_region.width_ > config_.vlp_max_width_ * wrat)
        {
        	continue;
        }

        if (lp_region.height_ > config_.vlp_max_height_ || lp_region.width_ > config_.vlp_max_width_)
        {
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->warn(
                "Lp too big! lp_height {}, lp_width {}", lp_region.height_, lp_region.width_);
            continue;
        }

        // if (lp_qa_enable)
        // {
        //     // bool lp_quality = lp_qa(image, vehicles->veh_info, i);
        //     // if (!lp_quality)
        //     // {
        //     //     continue;
        //     // }
        // }

time_t s_time = getSystemTimeUS();

        int ret = proc_ptr_->imageCropResize(imgae_data, &dst_img_, lp_region);
        if(ret != SL_SUCCESS)
        {
        	SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error(
        		"vlp rec call image_crop_resize_csc failed! crop_rect xywh: {}, {}, {}, {}",
        		lp_region.x_, lp_region.y_, lp_region.width_, lp_region.height_);
        	continue;
        }

// std::cout << " ----------------lprec imageCropResize time: " << getSystemTimeUS() - s_time << std::endl;

        ret = preProcess((void*)(dst_img_.virt_addr[0]));
// std::cout << " ----------------lprec preProcess time: " << getSystemTimeUS() - s_time << std::endl;

        if (ret != SL_SUCCESS)
        {
            continue;
        }

        ret = inference();
        if (ret != SL_SUCCESS)
        {
            continue;
        }
// std::cout << " ----------------lprec inference time: " << getSystemTimeUS() - s_time << std::endl;

        ret = postProcess(vehicles.veh_info[i].lp_info.lp_number);
// std::cout << " ----------------lprec postProcess time: " << getSystemTimeUS() - s_time << std::endl;

        if(ret != SL_SUCCESS){
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error(
        		"vlp rec postProcess error");
            continue;
        }
        // lpColorTypeConfirm(vehicles.veh_info[i].lp_info);
// std::cout << " ----------------lprec lpColorPostprocess time: " << getSystemTimeUS() - s_time << std::endl;

        if (vehicles.veh_info[i].lp_info.lp_color == SL_PLATE_COLOR_GREENBLACK &&
            vehicles.veh_info[i].lp_info.lp_number.size != 8 &&
            vehicles.veh_info[i].lp_info.lp_number.lp_str != "-")
        {
            vehicles.veh_info[i].lp_info.lp_number.lp_str = "-";
            vehicles.veh_info[i].lp_info.lp_number.vlp_rec_confidence = 0.0;
        }
    }

    return SL_SUCCESS;
}
#include "reid.h"
#include "../utils/utils.h"
#include "../utils/sl_logger.h"

#include "../utils/time_utils.h"

enum class InputOutputId
{
    INPUT_IMG_ID = 0,
    OUTPUT_LP_DET_TYPE_L_ID = 0,
};

sonli::Reid::Reid(): ModelProcess() {
    proc_ptr_ = std::move(std::unique_ptr<ImageProcess>(new ImageProcess()));
}

sonli::Reid::~Reid()
{
    proc_ptr_.reset();
    sample_svp_mmz_free(dst_img_.phys_addr[0], dst_img_.virt_addr[0]);
}


void sonli::Reid::InitReid()
{
    std::vector<int32_t> input_dims =     getInputShape(0);

    SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
        "reid  input_dims index 0, input_dims[3]={}, input_dims[2]={}", 
        input_dims[3], input_dims[2]);

    int ret = proc_ptr_->init(input_dims[3], input_dims[2]);
    if (ret != SL_SUCCESS)
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
        "reid proc_ptr_  init failed!");
    }

    ret = proc_ptr_->imageResizeInit(&dst_img_);
    if (ret != SL_SUCCESS)
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
        "reid proc_ptr_  imageResizeInit failed! ret {}", ret);
    }

    dims_ =     getOutputShape(0);
    SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
        "reid  output_dims index 0, output_dims_size={}, output_dims[0]={}, output_dims[1]={}", 
        dims_.size(), dims_[0], dims_[1]);

    stride_ =     getModelOutputStride(0);
    SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
        "reid  getModelOutputStride {}", stride_);
}


int32_t 
sonli::Reid::preProcess(void *data)
{
    int ret =     CreateInputBuf(data);

    if (ret != SL_SUCCESS)
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("reid CreateInputBuf failed!");
        return ERR_PROCESS_FAILED;
    }

    ret =     CreateTaskBufAndWorkBuf();
    if (ret != SL_SUCCESS)
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("reid CreateTaskBufAndWorkBuf failed!");
        return ERR_PROCESS_FAILED;
    }

    return SL_SUCCESS;
}

int32_t 
sonli::Reid::inference(){
    return Execute();
}

int32_t
sonli::Reid::postProcess(VehInfo &veh_info)
{
    auto *feat = reinterpret_cast<float *>(svp_acl_get_data_buffer_addr(
           svp_acl_mdl_get_dataset_buffer(    output_, static_cast<size_t>(InputOutputId::OUTPUT_LP_DET_TYPE_L_ID))));

    std::vector<int> dims =     getOutputShape(0);

    if (dims.size() > 0 && dims[1] > 0)
    {
        float feat_norm = 0;
        for (int i = 0; i < dims[1]; i++)
        {
            feature_[i] = feat[i];
            feat_norm += feat[i] * feat[i];
        }
        if (feat_norm != 0)
        {
            feat_norm = sqrt(feat_norm);
            for (int i = 0; i < dims[1]; i++)
            {
                feature_[i] = feat[i] / feat_norm;
                veh_info.feature.at<float>(0, i) = feature_[i];
            }
        }
    }
    return SL_SUCCESS;
}

int32_t sonli::Reid::reidProcess(sonli::VehInfoVec &vehicles, FrameImageData* imgae_data, ReIdResultPtr reid_res_ptr)
{
    for (int i = 0; i < vehicles.veh_info.size(); i++)
    {

        if (vehicles.veh_info[i].visual_rate < VEHICLE_VIS_THRESHOLD)
        {
            std::cout << "[REID]:continue" << std::endl;
            reid_res_ptr->embeddings_.push_back(std::make_shared<Embedding<float>>());
            reid_res_ptr->valid_features_.push_back(false);
            continue;
        }

        reid_res_ptr->valid_features_.push_back(true);
        if (!vehicles.use_reid)
        {
            for (int i = 0; i < REID_FEATURE_LENGTH; i++)
            {
                feature_[i] = 0.0625;
                vehicles.veh_info[i].feature.at<float>(0, i) = 0.0625;
            }

            EmbeddingPtr embeding_ptr = std::make_shared<Embedding<float>>(feature_, REID_FEATURE_LENGTH);
            reid_res_ptr->embeddings_.push_back(embeding_ptr);
            continue;
        }

// time_t s_time = getSystemTimeUS();

        int ret = proc_ptr_->imageCropResize(imgae_data, &dst_img_, vehicles.veh_info[i].location);
        if(ret != SL_SUCCESS)
        {
        	SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error(
        		"reid call imageCropResize failed! crop_rect xywh: {}, {}, {}, {}",
        		vehicles.veh_info[i].location.x_, 
                vehicles.veh_info[i].location.y_, 
                vehicles.veh_info[i].location.width_, 
                vehicles.veh_info[i].location.height_);
        	continue;
        }
// std::cout << " ----------------reid imageCropResize time: " << getSystemTimeUS() - s_time << std::endl;

        ret = preProcess((void*)(dst_img_.virt_addr[0]));

        if (ret != SL_SUCCESS)
        {
            continue;
        }
// std::cout << " ----------------reid preProcess time: " << getSystemTimeUS() - s_time << std::endl;

        ret = inference();
        if (ret != SL_SUCCESS)
        {
            continue;
        }
// std::cout << " ----------------reid inference time: " << getSystemTimeUS() - s_time << std::endl;

        ret = postProcess(vehicles.veh_info[i]);

        if(ret != SL_SUCCESS){
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error(
        		"reid postProcess error");
            continue;
        }
// std::cout << " ----------------reid postProcess time: " << getSystemTimeUS() - s_time << std::endl;
        
        EmbeddingPtr embeding_ptr = std::make_shared<Embedding<float>>(feature_, REID_FEATURE_LENGTH);
        reid_res_ptr->embeddings_.push_back(embeding_ptr);
    }

    return SL_SUCCESS;
}
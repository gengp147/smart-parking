/**
 * @file model_process.cpp
 *
 * Copyright (C) 2021. Shenshu Technologies Co., Ltd. All rights reserved.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "model_process_base.h"
#include "../utils/sl_logger.h"
#include "../utils/utils.h"
#include "acl/svp_acl.h"
#include "error.h"
#include <algorithm>
#include <iomanip>
// #include "../managers/device_manager.h"
// #include "../utils/Singleton.h"

#include "../utils/time_utils.h"

#include <iostream>
#include <ostream>

using namespace std;
using namespace sonli;

static const int BYTE_BIT_NUM = 8; // 1 byte = 8 bit

ModelProcess::ModelProcess()
{
    auto &device_manager_ = Singleton<DeviceManager>::GetInstance();
    subContext_ = std::make_shared<SubContext>(&device_manager_);
    if (!subContext_->IsInit())
    {
        auto ret = subContext_->InitContext();
        if (ret != SL_SUCCESS)
        {
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("lp rec InitContext failed!");
        }
    }
    auto ret = InitInput();
    if (ret != SL_SUCCESS)
    {
        SonLiLogger::getInstance()
            .getLogger(sonli::SonLiLogger::RotateLogger)
            ->error("input or output dataset create failed");
    }
}

ModelProcess::~ModelProcess()
{
    Unload();
    DestroyDesc();
    DestroyInput();
    DestroyOutput();
}

int
ModelProcess::LoadModelFromFileWithMem(const std::string &modelPath)
{
    uint32_t fileSize = 0;
    modelMemPtr_ = Utils::ReadBinFile(modelPath, fileSize);
    modelMemSize_ = fileSize;
    SonLiLogger::getInstance()
        .getLogger(sonli::SonLiLogger::RotateLogger)
        ->info("Load modelpath {}, file size: {}", modelPath, fileSize);
    svp_acl_error ret = svp_acl_mdl_load_from_mem(static_cast<uint8_t *>(modelMemPtr_), modelMemSize_, &modelId_);
    if (ret != SVP_ACL_SUCCESS)
    {
        svp_acl_rt_free(modelMemPtr_);
        SonLiLogger::getInstance()
            .getLogger(sonli::SonLiLogger::RotateLogger)
            ->error("load model from file failed, model file is {}, err {}", modelPath.c_str(), ret);
        return ERR_INIT_FAIL;
    }

    SonLiLogger::getInstance()
        .getLogger(sonli::SonLiLogger::RotateLogger)
        ->info("Load modelpath {}, modelId_: {}", modelPath, modelId_);
    CreateDesc();
    svp_acl_mdl_io_dims inputDims;
    // only support single input model
    ret = GetInputStrideParam(0, devSize, stride, inputDims);
    if (ret != SL_SUCCESS)
    {
        SonLiLogger::getInstance().getLogger(sonli::SonLiLogger::RotateLogger)->error("Error, GetStrideParam error");
        return ERR_INIT_FAIL;
    }
    size_t dataSize = GetInputDataSize(0);
    if (dataSize == 0)
    {
        SonLiLogger::getInstance()
            .getLogger(sonli::SonLiLogger::RotateLogger)
            ->error("Error, GetInputDataSize == 0 error");
        return ERR_INIT_FAIL;
    }
    loadFlag_ = true;
    return SL_SUCCESS;
}

int
ModelProcess::CreateDesc()
{
    modelDesc_ = svp_acl_mdl_create_desc();
    if (modelDesc_ == nullptr)
    {
        SonLiLogger::getInstance()
            .getLogger(sonli::SonLiLogger::RotateLogger)
            ->error("create model description failed");
        return ERR_INIT_FAIL;
    }

    svp_acl_error ret = svp_acl_mdl_get_desc(modelDesc_, modelId_);
    if (ret != SVP_ACL_SUCCESS)
    {
        SonLiLogger::getInstance().getLogger(sonli::SonLiLogger::RotateLogger)->error("get model description failed");
        return ERR_INIT_FAIL;
    }
    SonLiLogger::getInstance().getLogger(sonli::SonLiLogger::RotateLogger)->info("create model description SL_SUCCESS");

    return SL_SUCCESS;
}

void
ModelProcess::DestroyDesc()
{
    if (modelDesc_ != nullptr)
    {
        (void)svp_acl_mdl_destroy_desc(modelDesc_);
        modelDesc_ = nullptr;
    }
}

int
ModelProcess::InitInput()
{
    if (input_ == nullptr)
    {
        input_ = svp_acl_mdl_create_dataset();
        if (input_ == nullptr)
        {
            SonLiLogger::getInstance()
                .getLogger(sonli::SonLiLogger::RotateLogger)
                ->error("can't create dataset, create input failed");
            return ERR_INIT_FAIL;
        }
    }

    if (output_ == nullptr)
    {
        output_ = svp_acl_mdl_create_dataset();
        if (output_ == nullptr)
        {
            SonLiLogger::getInstance()
                .getLogger(sonli::SonLiLogger::RotateLogger)
                ->error("can't create dataset, create output failed");
            return ERR_INIT_FAIL;
        }
    }
    return SL_SUCCESS;
}

int
ModelProcess::CreateInput(svp_acl_data_buffer **inputData, void *inputDataBuffer, size_t bufferSize, int stride) const
{
    if (*inputData != nullptr)
    {
        auto ret = svp_acl_update_data_buffer(*inputData, inputDataBuffer, bufferSize, stride);
        if (ret != SVP_ACL_SUCCESS)
        {
            SonLiLogger::getInstance()
                .getLogger(sonli::SonLiLogger::RotateLogger)
                ->error("update input dataset buffer failed");
            return ERR_INIT_FAIL;
        }
    }
    else
    {
        *inputData = svp_acl_create_data_buffer(inputDataBuffer, bufferSize, stride);
        if (*inputData == nullptr)
        {
            SonLiLogger::getInstance()
                .getLogger(sonli::SonLiLogger::RotateLogger)
                ->error("can't create dataset, create input failed");
            return ERR_INIT_FAIL;
        }
        svp_acl_error ret = svp_acl_mdl_add_dataset_buffer(input_, *inputData);
        if (ret != SVP_ACL_SUCCESS)
        {
            SonLiLogger::getInstance()
                .getLogger(sonli::SonLiLogger::RotateLogger)
                ->error("add input dataset buffer failed");
            svp_acl_destroy_data_buffer(*inputData);
            return ERR_INIT_FAIL;
        }
    }
    return SL_SUCCESS;
}

size_t
ModelProcess::GetInputDataSize(int index) const
{
    svp_acl_data_type dataType = svp_acl_mdl_get_input_data_type(modelDesc_, index);
    return svp_acl_data_type_size(dataType) / BYTE_BIT_NUM;
}

size_t
ModelProcess::GetOutputDataSize(int index) const
{
    svp_acl_data_type dataType = svp_acl_mdl_get_output_data_type(modelDesc_, index);
    return svp_acl_data_type_size(dataType) / BYTE_BIT_NUM;
}

int
ModelProcess::GetInputStrideParam(int index, size_t &bufSize, size_t &stride, svp_acl_mdl_io_dims &dims) const
{
    svp_acl_error ret = svp_acl_mdl_get_input_dims(modelDesc_, index, &dims);
    if (ret != SVP_ACL_SUCCESS)
    {
        SonLiLogger::getInstance()
            .getLogger(sonli::SonLiLogger::RotateLogger)
            ->error("svp_acl_mdl_get_input_dims error!");
        return ERR_INIT_FAIL;
    }
    stride = svp_acl_mdl_get_input_default_stride(modelDesc_, index);
    if (stride == 0)
    {
        SonLiLogger::getInstance()
            .getLogger(sonli::SonLiLogger::RotateLogger)
            ->error("svp_acl_mdl_get_input_default_stride error!");
        return ERR_INIT_FAIL;
    }
    bufSize = svp_acl_mdl_get_input_size_by_index(modelDesc_, index);
    if (bufSize == 0)
    {
        SonLiLogger::getInstance()
            .getLogger(sonli::SonLiLogger::RotateLogger)
            ->error("svp_acl_mdl_get_input_size_by_index error!");
        return ERR_INIT_FAIL;
    }
    return SL_SUCCESS;
}

int
ModelProcess::GetOutputStrideParam(int index, size_t &bufSize, size_t &stride, svp_acl_mdl_io_dims &dims) const
{
    svp_acl_error ret = svp_acl_mdl_get_output_dims(modelDesc_, index, &dims);
    if (ret != SVP_ACL_SUCCESS)
    {
        SonLiLogger::getInstance()
            .getLogger(sonli::SonLiLogger::RotateLogger)
            ->error("svp_acl_mdl_get_input_default_stride error!");
        return ERR_INIT_FAIL;
    }
    stride = svp_acl_mdl_get_output_default_stride(modelDesc_, index);
    if (stride == 0)
    {
        SonLiLogger::getInstance()
            .getLogger(sonli::SonLiLogger::RotateLogger)
            ->error("svp_acl_mdl_get_output_default_stride error!");
        return ERR_INIT_FAIL;
    }
    bufSize = svp_acl_mdl_get_output_size_by_index(modelDesc_, index);
    if (bufSize == 0)
    {
        SonLiLogger::getInstance()
            .getLogger(sonli::SonLiLogger::RotateLogger)
            ->error("svp_acl_mdl_get_output_size_by_index error!");
        return ERR_INIT_FAIL;
    }
    return SL_SUCCESS;
}

void
ModelProcess::DestroyInput()
{
    if (input_ == nullptr)
    {
        return;
    }

    int ret = 0;

    for (size_t i = 1; i < svp_acl_mdl_get_dataset_num_buffers(input_); ++i)
    {
        svp_acl_data_buffer *dataBuffer = svp_acl_mdl_get_dataset_buffer(input_, i);

        SonLiLogger::getInstance()
            .getLogger(SonLiLogger::RotateLogger)
            ->info("i {}, idx {}", i, svp_acl_mdl_get_dataset_num_buffers(input_));

        void *tmp = svp_acl_get_data_buffer_addr(dataBuffer);
        ret = svp_acl_rt_free(tmp);
        if (ret != SVP_ACL_SUCCESS)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->error("svp_acl_rt_free failed, ret {}, i={}, tmp {}", ret, i, tmp);
        }
        ret = svp_acl_destroy_data_buffer(dataBuffer);
        if (ret != SVP_ACL_SUCCESS)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->error("svp_acl_destroy_data_buffer failed, ret {}, i={}", ret, i);
        }
    }

    void *tmp = svp_acl_get_data_buffer_addr(inputData_);
    ret = svp_acl_rt_free(tmp);
    if (ret != SVP_ACL_SUCCESS)
    {
        SonLiLogger::getInstance()
            .getLogger(SonLiLogger::RotateLogger)
            ->error("svp_acl_rt_free failed, ret {}, tmp {}", ret, tmp);
    }
    ret = svp_acl_destroy_data_buffer(inputData_);
    if (ret != SVP_ACL_SUCCESS)
    {
        SonLiLogger::getInstance()
            .getLogger(SonLiLogger::RotateLogger)
            ->error("svp_acl_destroy_data_buffer failed, ret {}", ret);
    }
    ret = svp_acl_mdl_destroy_dataset(input_);
    if (ret != SVP_ACL_SUCCESS)
    {
        SonLiLogger::getInstance()
            .getLogger(SonLiLogger::RotateLogger)
            ->error("svp_acl_mdl_destroy_dataset failed, ret {}", ret);
    }
    inputData_ = nullptr;
    input_ = nullptr;
}

int
ModelProcess::CreateOutput() const
{

    size_t outputSize = svp_acl_mdl_get_num_outputs(modelDesc_);
    for (size_t i = 0; i < outputSize; ++i)
    {
        size_t stride = svp_acl_mdl_get_output_default_stride(modelDesc_, i);
        if (stride == 0)
        {
            SonLiLogger::getInstance()
                .getLogger(sonli::SonLiLogger::RotateLogger)
                ->error("Error, output default stride is {}.", stride);
            return ERR_INIT_FAIL;
        }
        size_t bufferSize = svp_acl_mdl_get_output_size_by_index(modelDesc_, i);
        if (bufferSize == 0)
        {
            SonLiLogger::getInstance()
                .getLogger(sonli::SonLiLogger::RotateLogger)
                ->error("Error, output size is {}.", bufferSize);
            return ERR_INIT_FAIL;
        }

        void *outputBuffer = nullptr;
        svp_acl_error ret = svp_acl_rt_malloc(&outputBuffer, bufferSize, SVP_ACL_MEM_MALLOC_NORMAL_ONLY);
        if (ret != SVP_ACL_SUCCESS)
        {
            SonLiLogger::getInstance()
                .getLogger(sonli::SonLiLogger::RotateLogger)
                ->error("can't malloc buffer, size is {}.", bufferSize);
            return ERR_INIT_FAIL;
        }
        Utils::InitData(static_cast<int8_t *>(outputBuffer), bufferSize);

        svp_acl_data_buffer *outputData = svp_acl_create_data_buffer(outputBuffer, bufferSize, stride);
        if (outputData == nullptr)
        {
            SonLiLogger::getInstance()
                .getLogger(sonli::SonLiLogger::RotateLogger)
                ->error("can't create data buffer, create output failed");
            svp_acl_rt_free(outputBuffer);
            return ERR_INIT_FAIL;
        }
        ret = svp_acl_mdl_add_dataset_buffer(output_, outputData);
        if (ret != SVP_ACL_SUCCESS)
        {
            SonLiLogger::getInstance()
                .getLogger(sonli::SonLiLogger::RotateLogger)
                ->error("can't add data buffer, create output failed");
            svp_acl_rt_free(outputBuffer);
            svp_acl_destroy_data_buffer(outputData);
            return ERR_INIT_FAIL;
        }
    }

    SonLiLogger::getInstance().getLogger(sonli::SonLiLogger::RotateLogger)->info("create model output SL_SUCCESS");
    return SL_SUCCESS;
}

int
ModelProcess::ClearOutputStrideInvalidBuf(std::vector<int8_t> &buffer, size_t index) const
{
    size_t bufSize = 0;
    size_t bufStride = 0;
    svp_acl_mdl_io_dims dims;
    svp_acl_error ret = GetOutputStrideParam(index, bufSize, bufStride, dims);
    if (ret != SL_SUCCESS)
    {
        SonLiLogger::getInstance()
            .getLogger(sonli::SonLiLogger::RotateLogger)
            ->error("Error, GetOutputStrideParam failed");
        return ERR_INIT_FAIL;
    }
    if ((bufSize == 0) || (bufStride == 0))
    {
        SonLiLogger::getInstance()
            .getLogger(sonli::SonLiLogger::RotateLogger)
            ->error("Error, bufSize({}) bufStride({}) invalid", bufSize, bufStride);
        return ERR_INIT_FAIL;
    }
    if ((dims.dim_count == 0) || (dims.dims[dims.dim_count - 1] <= 0))
    {
        SonLiLogger::getInstance().getLogger(sonli::SonLiLogger::RotateLogger)->error("Error, dims para invalid");
        return ERR_INIT_FAIL;
    }
    int64_t lastDim = dims.dims[dims.dim_count - 1];

    size_t dataSize = GetOutputDataSize(index);
    if (dataSize == 0)
    {
        SonLiLogger::getInstance().getLogger(sonli::SonLiLogger::RotateLogger)->error("Error, dataSize == 0 invalid");
        return ERR_INIT_FAIL;
    }
    size_t lastDimSize = dataSize * lastDim;
    size_t loopNum = bufSize / bufStride;
    size_t invalidSize = bufStride - lastDimSize;
    if (invalidSize == 0)
    {
        // not stride invalid space, return directly.
        return SL_SUCCESS;
    }

    for (size_t i = 0; i < loopNum; ++i)
    {
        size_t offset = bufStride * i + lastDimSize;
        int8_t *ptr = &buffer[offset];
        for (size_t index = 0; index < invalidSize; index++)
        {
            ptr[index] = 0;
        }
    }
    return SL_SUCCESS;
}

void
ModelProcess::DestroyOutput()
{
    if (output_ == nullptr)
    {
        return;
    }

    for (size_t i = 0; i < svp_acl_mdl_get_dataset_num_buffers(output_); ++i)
    {
        svp_acl_data_buffer *dataBuffer = svp_acl_mdl_get_dataset_buffer(output_, i);
        void *data = svp_acl_get_data_buffer_addr(dataBuffer);
        (void)svp_acl_rt_free(data);
        (void)svp_acl_destroy_data_buffer(dataBuffer);
    }

    (void)svp_acl_mdl_destroy_dataset(output_);
    output_ = nullptr;
}

int
ModelProcess::SubContextInit() const
{
    if (!subContext_->IsInit())
    {
        auto ret = subContext_->InitContext();
        if (ret != SL_SUCCESS)
        {
            SonLiLogger::getInstance().getLogger(sonli::SonLiLogger::RotateLogger)->error("Error, InitContext failed");
            return ERR_INIT_FAIL;
        }
    }
    return SL_SUCCESS;
}

int
ModelProcess::Execute()
{
    subContext_->SetCurrentContext();
    svp_acl_error ret = svp_acl_mdl_execute(modelId_, input_, output_);
    if (ret != SVP_ACL_SUCCESS)
    {
        SonLiLogger::getInstance()
            .getLogger(sonli::SonLiLogger::RotateLogger)
            ->error("Error, execute model failed, modelId is {}, ret {}", modelId_, ret);
        return ERR_INIT_FAIL;
    }
    executeNum_++;
    return SL_SUCCESS;
}

int
ModelProcess::CreateBuf(int index) const
{
    void *bufPtr = nullptr;
    size_t bufSize = 0;
    size_t bufStride = 0;
    svp_acl_mdl_io_dims inDims;
    svp_acl_error ret = GetInputStrideParam(index, bufSize, bufStride, inDims);

    if (ret != SL_SUCCESS)
    {
        SonLiLogger::getInstance()
            .getLogger(sonli::SonLiLogger::RotateLogger)
            ->error("Error, GetInputStrideParam failed, index is {}", index);
        return ERR_INIT_FAIL;
    }
    ret = svp_acl_rt_malloc(&bufPtr, bufSize, SVP_ACL_MEM_MALLOC_NORMAL_ONLY);
    if (ret != SVP_ACL_SUCCESS)
    {
        SonLiLogger::getInstance()
            .getLogger(sonli::SonLiLogger::RotateLogger)
            ->error("Error, malloc device buffer failed. size is {}", bufSize);
        return ERR_INIT_FAIL;
    }
    Utils::InitData(static_cast<int8_t *>(bufPtr), bufSize);

    svp_acl_data_buffer *inputData = nullptr;
    ret = CreateInput(&inputData, bufPtr, bufSize, bufStride);

    if (ret != SL_SUCCESS)
    {
        SonLiLogger::getInstance()
            .getLogger(sonli::SonLiLogger::RotateLogger)
            ->error("Error, execute CreateInput failed");
        svp_acl_rt_free(bufPtr);
        return ERR_INIT_FAIL;
    }
    return SL_SUCCESS;
}

int
ModelProcess::CreateInputBuf(const string &filePath)
{
    svp_acl_mdl_io_dims inputDims;
    // only support single input model
    int ret = GetInputStrideParam(0, devSize, stride, inputDims);
    if (ret != SL_SUCCESS)
    {
        SonLiLogger::getInstance().getLogger(sonli::SonLiLogger::RotateLogger)->error("Error, GetStrideParam error");
        return ERR_INIT_FAIL;
    }
    size_t dataSize = GetInputDataSize(0);
    if (dataSize == 0)
    {
        SonLiLogger::getInstance()
            .getLogger(sonli::SonLiLogger::RotateLogger)
            ->error("Error, GetInputDataSize == 0 error");
        return ERR_INIT_FAIL;
    }
    void *picDevBuffer = Utils::GetDeviceBufferOfFile(filePath, inputDims, stride, dataSize);
    if (picDevBuffer == nullptr)
    {
        SonLiLogger::getInstance()
            .getLogger(sonli::SonLiLogger::RotateLogger)
            ->error("Error, get pic device buffer failed");
        return ERR_INIT_FAIL;
    }

    ret = CreateInput(&inputData_, picDevBuffer, devSize, stride);
    if (ret != SL_SUCCESS)
    {
        SonLiLogger::getInstance()
            .getLogger(sonli::SonLiLogger::RotateLogger)
            ->error("Error, execute CreateInput failed");
        svp_acl_rt_free(picDevBuffer);
        return ERR_INIT_FAIL;
    }
    return SL_SUCCESS;
}

int
ModelProcess::CreateInputBuf(void *picDevBuffer)
{
    auto ret = CreateInput(&inputData_, picDevBuffer, devSize, stride);
    if (ret != SL_SUCCESS)
    {
        SonLiLogger::getInstance()
            .getLogger(sonli::SonLiLogger::RotateLogger)
            ->error("Error, execute CreateInput failed");
        return ERR_INIT_FAIL;
    }
    return SL_SUCCESS;
}

int
ModelProcess::CreateTaskBufAndWorkBuf()
{
    if (workBufCreated_)
    {
        return SL_SUCCESS;
    }

    if (svp_acl_mdl_get_num_inputs(modelDesc_) <= 2)
    {
        SonLiLogger::getInstance()
            .getLogger(sonli::SonLiLogger::RotateLogger)
            ->error("Error, input dataset Num is error.");
        return ERR_INIT_FAIL;
    }
    size_t datasetSize = svp_acl_mdl_get_dataset_num_buffers(input_);

    if (datasetSize == 0)
    {
        SonLiLogger::getInstance().getLogger(sonli::SonLiLogger::RotateLogger)->error("Error, input dataset Num is 0.");
        return ERR_INIT_FAIL;
    }
    for (size_t loop = datasetSize; loop < svp_acl_mdl_get_num_inputs(modelDesc_); loop++)
    {
        int ret = CreateBuf(loop);
        if (ret != SL_SUCCESS)
        {
            SonLiLogger::getInstance()
                .getLogger(sonli::SonLiLogger::RotateLogger)
                ->error("Error, execute Create taskBuffer and workBuffer failed");
            return ERR_INIT_FAIL;
        }
    }
    workBufCreated_ = true;
    return SL_SUCCESS;
}

void
ModelProcess::Unload()
{
    if (!loadFlag_)
    {
        SonLiLogger::getInstance()
            .getLogger(sonli::SonLiLogger::RotateLogger)
            ->warn("Error, no model had been loaded, unload failed");
        return;
    }

    svp_acl_error ret = svp_acl_mdl_unload(modelId_);
    if (ret != SVP_ACL_SUCCESS)
    {
        SonLiLogger::getInstance()
            .getLogger(sonli::SonLiLogger::RotateLogger)
            ->error("Error, unload model failed, modelId is {}, error is {}", modelId_, ret);
    }

    if (modelDesc_ != nullptr)
    {
        (void)svp_acl_mdl_destroy_desc(modelDesc_);
        modelDesc_ = nullptr;
    }

    if (modelMemPtr_ != nullptr)
    {
        svp_acl_rt_free(modelMemPtr_);
        modelMemPtr_ = nullptr;
        modelMemSize_ = 0;
    }

    if (modelWeightPtr_ != nullptr)
    {
        svp_acl_rt_free(modelWeightPtr_);
        modelWeightPtr_ = nullptr;
        modelWeightSize_ = 0;
    }

    loadFlag_ = false;
    SonLiLogger::getInstance()
        .getLogger(sonli::SonLiLogger::RotateLogger)
        ->info("unload model SL_SUCCESS, modelId is {}", modelId_);
}

std::vector<int32_t>
ModelProcess::getInputShape(int index) const
{
    std::vector<int32_t> m_dims;
    svp_acl_mdl_io_dims inputDims;
    svp_acl_error ret = svp_acl_mdl_get_input_dims(modelDesc_, index, &inputDims);
    if (ret != SVP_ACL_SUCCESS)
    {
        SonLiLogger::getInstance()
            .getLogger(sonli::SonLiLogger::RotateLogger)
            ->error("Error, svp_acl_mdl_get_input_dims error!");
        return m_dims;
    }
    for (int i = 0; i < inputDims.dim_count; i++)
    {
        m_dims.emplace_back(inputDims.dims[i]);
    }
    return m_dims;
}

std::vector<int32_t>
ModelProcess::getOutputShape(int index) const
{
    std::vector<int32_t> m_dims;
    svp_acl_mdl_io_dims outputDims;
    svp_acl_error ret = svp_acl_mdl_get_output_dims(modelDesc_, index, &outputDims);
    if (ret != SVP_ACL_SUCCESS)
    {
        SonLiLogger::getInstance()
            .getLogger(sonli::SonLiLogger::RotateLogger)
            ->error("Error, svp_acl_mdl_get_output_dims error!");
        return m_dims;
    }
    for (int i = 0; i < outputDims.dim_count; i++)
    {
        m_dims.emplace_back(outputDims.dims[i]);
    }
    return m_dims;
}

size_t
ModelProcess::getModelInputNum() const
{
    return svp_acl_mdl_get_num_inputs(modelDesc_);
}

size_t
ModelProcess::getModelOutputNum() const
{
    return svp_acl_mdl_get_num_outputs(modelDesc_);
}

size_t
ModelProcess::getModelOutputStride(int inndex) const
{
    return svp_acl_mdl_get_output_default_stride(modelDesc_, inndex);
}

size_t
ModelProcess::getModelInputStride(int inndex) const
{
    return svp_acl_mdl_get_output_default_stride(modelDesc_, inndex);
}
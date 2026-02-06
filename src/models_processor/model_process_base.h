/**
 * @file model_process.h
 *
 * Copyright (C) 2021. Shenshu Technologies Co., Ltd. All rights reserved.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifndef MODEL_PROCESS_BASE_H
#define MODEL_PROCESS_BASE_H

#include "../utils/utils.h"
#include "../managers/device_manager.h"
#include "../utils/Singleton.h"

#include <memory>
#include <vector>

class SubContext;
class ModelProcess
{
public:
    /**
     * @brief Constructor
     */
    ModelProcess();

    /**
     * @brief Destructor
     */
    virtual ~ModelProcess();

    /**
     * @brief load model from file with mem
     * @param [in] modelPath: model path
     * @return int
     */
    int
    LoadModelFromFileWithMem(const std::string &modelPath);

    /**
     * @brief unload model
     */
    void
    Unload();

    int
    InitInput();


    /**
     * @brief create model desc
     * @return int
     */
private:
    int
    CreateDesc();
    /**
     * @brief destroy desc
     */
    void
    DestroyDesc();
public:

    /**
     * @brief create model input
     * @param
     * @param [in] inputDataBuffer: input buffer
     * @param [in] bufferSize: input buffer size
     * @return int
     */
    int
    CreateInput(svp_acl_data_buffer **, void *inputDataBuffer, size_t bufferSize, int stride) const;

    int
    CreateInputBuf(const std::string &filePath);


    int
    CreateInputBuf(void *picDevBuffer);

    int
    CreateTaskBufAndWorkBuf();

    /**
     * @brief destroy input resource
     */
    void
    DestroyInput();

    /**
     * @brief create output buffer
     * @return int
     */
    int
    CreateOutput() const;

    /**
     * @brief destroy output resource
     */
    void
    DestroyOutput();
    int
    SubContextInit() const;

    /**
     * @brief model execute
     * @return int
     */
    int
    Execute();

    int
    CreateBuf(int index) const;

    int
    GetInputStrideParam(int index, size_t &bufSize, size_t &stride, svp_acl_mdl_io_dims &dims) const;

    int
    GetOutputStrideParam(int index, size_t &bufSize, size_t &stride, svp_acl_mdl_io_dims &dims) const;

    size_t
    GetInputDataSize(int index) const;

    size_t
    GetOutputDataSize(int index) const;

    std::vector<int32_t>
    getInputShape(int index) const;
    std::vector<int32_t>
    getOutputShape(int index) const;
    size_t
    getModelInputNum() const;
    size_t
    getModelOutputNum() const;
    size_t
    getModelOutputStride(int inndex) const;
    size_t
    getModelInputStride(int inndex) const;

    std::shared_ptr<SubContext> getSubContext() const;

public:
    int
    ClearOutputStrideInvalidBuf(std::vector<int8_t> &buffer, size_t index) const;

    uint32_t executeNum_{0};
    
    size_t modelMemSize_{0};
    size_t modelWeightSize_{0};
    void *modelMemPtr_{nullptr};
    void *modelWeightPtr_{nullptr};
    bool loadFlag_{false};
    svp_acl_mdl_desc *modelDesc_{nullptr};
    svp_acl_mdl_dataset *input_{nullptr};
    svp_acl_mdl_dataset *output_{nullptr};


    size_t devSize = 0;
    size_t stride = 0;
    svp_acl_data_buffer *inputData_{nullptr};
    bool workBufCreated_{false};

public:
    std::shared_ptr<SubContext> subContext_;


    int model_name_ = 0;
    uint32_t modelId_{0};
};

#endif // MODEL_PROCESS_BASE_H

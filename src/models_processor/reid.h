//
// Created by gp147 on 2025/8/12.
//

#ifndef REID_H
#define REID_H

#include "model_process_base.h"
#include "data_type.h"

#include "../image_processor/image_process.h"
// #include "model_datatype.h"

#include "error.h"

namespace sonli
{

class Reid : public ModelProcess
{
public:
    Reid();
    ~Reid() override;

    void InitReid();
    int32_t reidProcess(sonli::VehInfoVec &vehicles, FrameImageData* imgae_data, ReIdResultPtr reid_res_ptr);

private:
    int32_t preProcess(void *data);
    int32_t inference();
    int32_t postProcess(VehInfo &veh_info);

    std::unique_ptr<ImageProcess> proc_ptr_ = nullptr;
    std::vector<int32_t> dims_;
    int stride_ = 0;
    float feature_[REID_FEATURE_LENGTH];
    ot_svp_dst_img dst_img_;
};

} // namespace sonli

#endif // REID_H

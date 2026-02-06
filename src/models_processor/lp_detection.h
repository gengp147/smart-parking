
#ifndef om_model_test_lp_detection_H
#define om_model_test_lp_detection_H

#include "data_type.h"
#include "model_process_base.h"

class LpDetectionConfig
{
    public:
    LpDetectionConfig() {};
    ~LpDetectionConfig() {};

    int img_width_{3840};
    int img_height_{2160};

    int net_input_width_{768};
    int net_input_height_{448};

    double veh_color_confi_thresh_{ 0.8 };
    double veh_type_confi_thresh_{ 0.8 };

    double vlp_confi_thresh_{ 0.3 };
    int vlp_num_class_{ 8 };
    float vlp_nms_threshold_{ 0.1 };

};

class LpDetection : public  ModelProcess
{
public:
    LpDetection();
    ~LpDetection() override;
    void InitLpDetection();

    int32_t lpDetProcess(sonli::VehInfoVec &vehicles, void *data);
    int32_t lpDetProcess(void *data, sonli::LPInfoVecPtr lp_infos_ptr);

    LpDetectionConfig config_;

private:
    int32_t preProcess(void *data);
    int32_t inference();
    int32_t postProcess(sonli::VehInfoVec &vehicles);
    void lp_det_confirm(sonli::VehInfo &veh_info);
    bool lp_veh_relate(const sonli::VehInfo& veh_info, int lp_x, int lp_y, int lp_w, int lp_h);


    int32_t postProcess(sonli::LPInfoVecPtr lp_infos_ptr);

};

#endif

//om_model_test_lp_detection_H

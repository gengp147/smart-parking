#ifndef VEHICLE_DETECTION_H
#define VEHICLE_DETECTION_H

#include "data_type.h"
#include "model_datatype.h"
#include "model_process_base.h"

class VehicleDetectionConfig
{
public:
    VehicleDetectionConfig() {};
    ~VehicleDetectionConfig() {};


    float veh_nms_threshold_{0.65};
    int img_width_{3840};
    int img_height_{2160};

    int net_input_width_{768};
    int net_input_height_{448};

    float veh_confi_thresh_{0.5};
    int veh_num_class_{ 2 };
};

class VehicleDetection : public ModelProcess
{
public:
    VehicleDetection();
    ~VehicleDetection() override;
    void InitVehicleDetection();

    int32_t vehDetProcess(sonli::VehInfoVec &vehs_info,
                          unsigned long long image_timestamp,
                          void *data);

    VehicleDetectionConfig config_;

private:
    int32_t preProcess(void *data);
    int32_t inference();
    void getProposals(std::vector<sonli::model_datatype::Kpt_Object> &proposals) const;
    int32_t postProcess(sonli::VehInfoVec &vehs_info,
                        unsigned long long image_timestamp);

    std::vector<std::vector<sonli::model_datatype::GridAndStride>> grid_strides_vec_;
};

#endif // VEHICLE_DETECTION_H

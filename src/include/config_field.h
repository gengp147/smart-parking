/***
 * @Author: lwn
 * @Date: 2023-01-09 13:26:11
 * @LastEditors: lwn
 * @LastEditTime: 2024-08-15 15:32:43
 * @FilePath: \FalconEye\include\common\config_field.h
 * @Description:
 */

#ifndef _SONLI_CONFIG_FIELD_H_
#define _SONLI_CONFIG_FIELD_H_

namespace sonli
{

// config
#define GLOBAL_CONFIG_NAME "common_config.json"
#define STREAM_CONFIG_NAME "stream_config.json"

#define VEHICLE_DETECT_MODULE_CONFIG "vehicle_detect_module.json"
#define VLP_DETECT_MODULE_CONFIG "vlp_detect_module.json"
#define VLP_RECOGNIZE_MODULE_CONFIG "vlp_recognize_module.json"
#define VEHICLE_TRACK_MODULE_CONFIG "vehicle_track_module.json"

// Module Unique Name
#define MUN_VLPR "vlpr_module"
#define MUN_VLPD "vlpd_module"
#define MUN_VEHD "veh_det_module"
#define MUN_EMB "veh_reid_module"
#define MUN_MOT "mot_module"
#define MUN_VIO "vehicle_in_out_park_module"
#define MUN_SUM "summary_module"
#define MUN_ACM "access_module"
#define MUN_ACC "access_collect"

// model config
#define SONLI_OBJ_BLUR_SIZE "blur_size"
#define SONLI_OBJ_BIN_THRESHOLD "binay_thr"
#define SONLI_OBJ_CONTOUR_SIZE_THRESHOLD                                                                               \
    "model"                                                                                                            \
    "contour_size_thr"

#define SONLI_OBJ_ANALYSIS_CONFIG "analysis_config"
#define SONLI_OBJ_MODEL_SETTING "model_setting"
#define SONLI_ELM_MODEL "model"
#define SONLI_ELM_MODEL_COLOR "model_color"

#define SONLI_OBJ_VLPR "vlpr"
#define SONLI_OBJ_VLPD "vlpd"

#define SONLI_OBJ_INPUT_SET "input_setting"
#define SONLI_ELM_INDEX "index"
#define SONLI_ELM_MAX_WIDTH "max_width"
#define SONLI_ELM_MIN_WIDTH "min_width"
#define SONLI_ELM_PRIORTY_PROVINCE "priorty_province"
#define SONLI_ELM_PRIORTY_CITY "priorty_city"
#define SONLI_ELM_VLPR_CONFI_THRESH "vlpr_confi_thresh"
#define SONLI_ELM_PROVINCE_CONFI_THRESH "first_char_confi_thresh"
#define SONLI_ELM_CITY_CONFI_THRESH "second_char_confi_thresh"
#define SONLI_ELM_VEH_CONFI_THRESH "veh_confi_thresh"
#define SONLI_ELM_ANCHOR "anchors"
#define SONLI_ELM_ANCHOR_HEIGHT "anchor_height"
#define SONLI_ELM_ANCHOR_WIDTH "anchor_width"
#define SONLI_ELM_VLPD_CONFI_THRESH "vlpd_confi_thresh"

#define SONLI_OBJ_TRACKER_SETTING "tracker_setting"
#define SONLI_ELM_TRACKER_NN_BUDGET "tracker_nn_budget"
#define SONLI_ELM_TRACKER_MAX_COSINE_DISTANCE "tracker_max_cosine_distance"
#define SONLI_ELM_TRACKER_MAX_IOU_DISTANCE "tracker_max_iou_distance"
#define SONLI_ELM_TRACKER_MAX_AGE "tracker_max_age"
#define SONLI_ELM_TRACKER_N_INIT "tracker_n_init"

#define SONLI_OBJ_SPACE_SETTING "space_setting"
#define SONLI_ELM_SPACE_LIST "space_list"
#define SONLI_ELM_SPACE_COUNT "space_count"
#define SONLI_ELM_SPACE_INDEX "space_index"
#define SONLI_ELM_SPACE_POINT "space_point"
#define SONLI_ELM_POINT_X "point_x"
#define SONLI_ELM_POINT_Y "point_y"

#define SONLI_OBJ_PARKING_SETTING "parking_setting"
#define SONLI_ELM_VEH_IN_RAT "veh_in_rat"
#define SONLI_ELM_VEH_LINE_RAT "veh_line_rat"
#define SONLI_ELM_VEH_OUT_RAT "veh_out_rat"
#define SONLI_ELM_STANDSTILL_TIME "standstill_time"
#define SONLI_ELM_VEH_IN_TIME_THRES "veh_in_time_thres"
#define SONLI_ELM_VEH_OUT_TIME_THRES "veh_out_time_thres"
#define SONLI_ELM_DISAPPEAR_TIME_THRESHOLD "disappear_time_threshold"
#define SONLI_ELM_VEH_LINE_TIME_THRES "veh_line_time_thres"

// config fields
#define SONLI_OBJ_STREAMS_SETTING "stream_cfg"
#define SONLI_ELM_REDIRECT "redirection"

#define SONLI_ELM_ENABLE "enable"

#define SONLI_OBJ_LOG_SETTING "logging"
#define SONLI_OBJ_ROTATE_LOG "rotate_log"
#define SONLI_ELM_LOG_LEVEL "log_level"
#define SONLI_ELM_FILE_NUM "file_num"
#define SONLI_ELM_FILE_SIZE "file_size"
#define SONLI_ELM_LOG_PATH "log_path"
#define SONLI_ELM_FILE_NAME "file_name"
#define SONLI_ELM_MIN_LOG_LEVEL "min_log_level"

// device config 2025 0826
#define SONLI_DEVICE_CONFIG_PATH "device_config"

#define SONLI_OBJ_PROFILER "profiler"
#define SONLI_ELM_FPS_INTERVAL "frame_interval"
#define SONLI_ELM_TIMER_INTERVAL "timer_interval"
#define SONLI_OBJ_UPLOAD_STRUCT_DATA "upload_struct_data"

#define SONLI_ORDER_MATCH_ENABLE "order_match_enable"
#define SONLI_STABILITY_ENABLE "stability_enable"

} // namespace sonli

#endif

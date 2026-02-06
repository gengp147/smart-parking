/***
 * @Author: lwn
 * @Date: 2024-03-25 17:45:07
 * @LastEditors: lwn
 * @LastEditTime: 2024-07-17 15:57:46
 * @FilePath: \FalconEye\include\common\config_content.h
 * @Description:
 */

#ifndef _COMMON_CONFIG_CONTENT_H_
#define _COMMON_CONFIG_CONTENT_H_

#define MAX_CAMERA_NUM 32 // channel or camera index must in range [0, MAX_CAMERA_NUM - 1]

#define MIN_EDGE_SIDE_DIST 40  /**最小边缘距离*/
#define REID_FEATURE_LENGTH 256 /** ReID 特征向量长度*/
#define BAYONET_DELAY_UPLOAD_TIME 300   // seconds
#define PARKING_IN_CONTINUE_TIME 150    // seconds
#define PARKING_EMPTY_CONTINUE_TIME 180 // seconds
#define ACCESS_INFO_SAVING_TIME 120     // seconds
#define TIME_INTERVAL_THRESH 60000      // ms

#define ORDER_DELAY_UPLOAD_TIME 60 // seconds /** 订单上传延迟时间*/
#define ORDER_UPLOAD_MAX_TIMES 10

#define LP_CACHE_LENGTH 50   /**最大车牌缓存数量*/
#define VEH_CACHE_LENGTH 151 /**最大车辆缓存数量*/

#define MIN_STREAK_COUNT 3   /**最小连续检测次数*/

#define TIMESTAMP_DAY 86400000
#define TIMESTAMP_ACCESS_SETTING (TIMESTAMP_DAY * 7)
namespace sonli
{
enum ConfigType
{
    // move area detect
    SL_CONFIG_BLUR_SIZE = 101,
    SL_CONFIG_BINAY_THRESH = 102,
    SL_CONFIG_CONTOUR_SIZE_THRESH = 103,

    // vehicle detect
    SL_CONFIG_VEHD_CONFI_THR = 201, // Confidence threshold of vehicle detector

    // vlp detect
    SL_CONFIG_CONFI_VLPD = 301, // Confidence threshold of vehicle license plate detector
                                // vlp recognize
    SL_CONFIG_CONFI_VLPR = 401, // Confidence threshold of vehicle license plate recognizor
    SL_CONFIG_FIRST_CHAR_CONFI_THR = 402,
    SL_CONFIG_SECOND_CHAR_CONFI_THR = 403,
    SL_CONFIG_VLP_TYPE_SUPPORT = 404,
    SL_CONFIG_VLP_MAX_WIDTH = 405, //
    SL_CONFIG_VLP_MIN_WIDTH = 406,
    SL_CONFIG_PRIORTY_PROVINCE = 407,
    SL_CONFIG_PRIORTY_CITY = 408,
    SL_CONFIG_CONFI_PROVINCE = 409,
    SL_CONFIG_CONFI_CITY = 410,

    // track
    SL_CONFIG_CAR_IN_SENSI = 501,       // Sensitivity of car in to park
    SL_CONFIG_CAR_OUT_SENSI = 502,      // Sensitivity of car out to park
    SL_CONFIG_CAR_IN_STABLETIME = 503,  // Stabletime of car into park
    SL_CONFIG_CAR_OUT_STABLETIME = 504, // Stabletime of car out park

    // combine image
    // SL_CONFIG_COMBINED_IMAGE_TYPE = 000,

    // alarm policy
    SL_CONFIG_ALARM_ENABLE = 1001, // Switch of uploading alarm
    SL_CONFIG_ALARM_VCW = 1002,    // Warning of cover vehice
    SL_CONFIG_ALARM_VPW = 1003,    // Warning of valid plate
    SL_CONFIG_ALARM_NMV = 1004,    // Warning of no motorized vehicle
    SL_CONFIG_ALARM_CPW = 1005,    // Warning of cross parking
    SL_CONFIG_ALARM_DVW = 1006,    // Warning of direct of vehicle error
    SL_CONFIG_ALARM_RPIO = 1007,   // Warning of repeated inout parking for vehicle
    SL_CONFIG_ALARM_PCW = 1008,    // Warning of cover vehice license plate
    SL_CONFIG_ALARM_IPW = 1009,    // Warning of illegel parking
    SL_CONFIG_ALARM_TPW = 1010,    // Warning of tilt parking

    // illegal parking info
    SL_CONFIG_IP_TRI_INV = 1101, // Trigger interval of illgel parking
    SL_CONFIG_IP_REGIONS = 1102, // Regions of illgeal parking

    SL_CONFIG_DEBUG_MODE = 1201,
};
} // namespace sonli

#define alarm_num_max 6
#define park_num_max 8
#define park_veh_num_max 3
#define max_veh_num 50
#define max_lp_num 3

// max_cap_buf_len + max_cap_buf_len_interval <=34
#define max_cap_buf_len 22
#define max_cap_buf_len_interval 10

#define vlp_max_width 500
#define vlp_min_width 50

#define VEHDETECT_SORT_MAX_LEN 500
#define VLPDETECT_SORT_MAX_LEN 200
#define MAX_PROPOSAL_NUM_SL 200

#define VEH_IN_ORDER_PATH "/media/feature/lib/order/"
#define VEHICLE_VIS_THRESHOLD 0.25
#endif
#ifndef BUSINESS_DATATYPE_H_
#define BUSINESS_DATATYPE_H_

#include "config_content.h"
#include "vulcan/data_struct.h"
#include <map>
#include <memory>
#include <string>

#include "../utils/CircularArray.h"
#include "sl_data_types_base.h"
#include <opencv2/opencv.hpp>
#include <ostream>
#include "../interfaces/aih_type.h"
#include "../interfaces/aih_extern.h"

namespace sonli
{
namespace illegal_parking
{
class MultipleIllegalCaptureDetail;
}

enum SLGPImageFormat_t
{
    SLGP_FORMATE_GRAY = 0,
    SLGP_FORMATE_BGR,
    SLGP_FORMATE_RGB,
    SLGP_FORMATE_BGRA,
    SLGP_FORMATE_RGBA,
    SLGP_FORMATE_YUV_420P,
    SLGP_FORMATE_YUV_420SP_NV12,
    SLGP_FORMATE_YUV_420SP_NV21,
    SLGP_FORMATE_YUV_422P,
    SLGP_FORMATE_YUV_422SP,
    SLGP_FORMATE_YUV_444

};

enum class AssociateMode
{
    IOU,
    CENTER_DISTANCE,
    HMIOU,
    WMIOU,
    MAHA_DISTANCE,
    UNKNOWN
};

#define VIDEO_DECODE_FORMAT SLGP_FORMATE_YUV_420P

typedef enum SLVehicleType_s
{
    SL_VEHICLE_TYPE_UNKNOWN = 0,
    SL_VEHICLE_TYPE_SEDAN = 1,
    SL_VEHICLE_TYPE_SUV = 2,
    SL_VEHICLE_TYPE_VAN = 3,
    SL_VEHICLE_TYPE_HATCHBACK = 4,
    SL_VEHICLE_TYPE_MPV = 5,
    SL_VEHICLE_TYPE_PICKUP = 6,
    SL_VEHICLE_TYPE_BUS = 7,
    SL_VEHICLE_TYPE_TRUCK = 8,
    SL_VEHICLE_TYPE_ESTATE = 9,
} SLVehicleType_t;

//! 车辆状态码
enum SLVehicleState_t
{
    SL_CAR_STATE_NOON = 0, //!< 无
    SL_CAR_STATE_IN_PRE = 1, //!< 车辆预驶入
    // SL_CAR_STATE_NO_CHANGE = 0x01,			//!<车位中为空
    SL_CAR_STATE_IN = 2, //!< 车辆正在驶入
    SL_CAR_STATE_STOP = 3, //!< 车辆停在车位

    SL_CAR_STATE_STOP_LEAVE = 4, //!< 车辆准备驶离还在车位
    SL_CAR_STATE_LEAVING = 5, //!< 车辆正在驶离
    SL_CAR_STATE_OUT = 6 //!< 车辆已驶离

};

typedef enum SLVehicleColor_s
{
    SL_VEHICLE_COLOR_UNKNOWN = 0,
    SL_VEHICLE_COLOR_YELLOW = 1,
    SL_VEHICLE_COLOR_ORANGE = 2,
    SL_VEHICLE_COLOR_GREEN = 3,
    SL_VEHICLE_COLOR_GRAY = 4,
    SL_VEHICLE_COLOR_RED = 5,
    SL_VEHICLE_COLOR_BLUE = 6,
    SL_VEHICLE_COLOR_WHITE = 7,
    SL_VEHICLE_COLOR_GOLDEN = 8,
    SL_VEHICLE_COLOR_BROWN = 9,
    SL_VEHICLE_COLOR_BLACK = 10,

} SLVehicleColor_t;

typedef enum SLLPColor_s
{
    SL_PLATE_COLOR_UNKNOWN = 0, //!<  未知
    SL_PLATE_COLOR_BLUE = 1, //!<  蓝底白字
    SL_PLATE_COLOR_YELLOW = 2, //!<  黄底黑字
    SL_PLATE_COLOR_WHITE = 3, //!<  白底黑字
    SL_PLATE_COLOR_BLACK = 4, //!<  黑底白字
    SL_PLATE_COLOR_GREEN = 5, //!<  绿底白字
    SL_PLATE_COLOR_GREENBLACK = 6, //!<  绿底黑字
    SL_PLATE_COLOR_GREENYELLOW = 7 //!<  绿黄底黑字(大型新能源车辆)
} SLLPColor_t;

typedef enum SLLPType_s
{
    SL_PLATE_UNCERTAIN = 0, //!< 不确定的
    SL_PLATE_BLUE [[maybe_unused]] = 1, //!< 蓝牌车
    SL_PLATE_YELLOW = 2, //!< 单层黄牌车
    SL_PLATE_POLICE = 4, //!< 警车
    SL_PLATE_WUJING = 8, //!< 武警车辆
    SL_PLATE_DBYELLOW = 16, //!< 双层黄牌
    SL_PLATE_MOTOR = 32, //!< 摩托车
    SL_PLATE_INSTRUCTIONCAR = 64, //!< 教练车
    SL_PLATE_MILITARY = 128, //!< 军车
    SL_PLATE_PERSONAL = 256, //!< 个性化车
    SL_PLATE_GANGAO = 512, //!< 港澳车
    SL_PLATE_EMBASSY = 1024, //!< 使馆车
    SL_PLATE_NONGLARE = 2048, //!< 老式车牌(不反光)
    SL_PLATE_AVIATION = 4096, //!< 民航车牌
    SL_PLATE_NEWENERGY = 8192, //!< 新能源车牌
    SL_PLATE_NEWENERGYBIG = 0x00004000, //!< 新能源车牌大车；
    SL_PLATE_EMERGENCY = 0x00008000, //!< 应急车；
} SLLPType_t;

typedef enum SLLPCharType_s
{
    SL_HANZI_CHN = 0, // 全国
    SL_HANZI_BEIJING = 1,
    SL_HANZI_SHANGHAI = 2,
    SL_HANZI_TIANJIN = 3,
    SL_HANZI_CHONGQING = 4,
    SL_HANZI_HEBEI = 5,
    SL_HANZI_SHANXI = 6,
    SL_HANZI_NEIMENG = 7,
    SL_HANZI_LIAONING = 8,
    SL_HANZI_JILIN = 9,
    SL_HANZI_HEILONGJIANG = 10,
    SL_HANZI_JIANGSU = 11,
    SL_HANZI_ZHEJIANG = 12,
    SL_HANZI_ANHUI = 13,
    SL_HANZI_FUJIAN = 14,
    SL_HANZI_JIANGXI = 15,
    SL_HANZI_SHANDONG = 16,
    SL_HANZI_HENAN = 17,
    SL_HANZI_HUBEI = 18,
    SL_HANZI_HUNAN = 19,
    SL_HANZI_GUANGDONG = 20,
    SL_HANZI_GUANGXI = 21,
    SL_HANZI_HAINAN = 22,
    SL_HANZI_SICHUAN = 23,
    SL_HANZI_GUIZHOU = 24,
    SL_HANZI_YUNNAN = 25,
    SL_HANZI_XIZANG = 26,
    SL_HANZI_XIAN = 27,
    SL_HANZI_GANSU = 28,
    SL_HANZI_QINGHAI = 29,
    SL_HANZI_NINGXIA = 30,
    SL_HANZI_XINJIANG = 31,

    SL_DIGITAL_START = 32,
    SL_DIGITAL_END = 41,

    SL_ALPHABET_START = 42,
    SL_ALPHABET_END = 65,

    SL_HANZI_XUE = 66,
    SL_HANZI_JING = 67,
    SL_HANZI_GANG = 68,
    SL_HANZI_AO = 69,
    SL_HANZI_GUA = 70,
    SL_HANZI_LING = 71,
    SL_HANZI_SHI = 72,

    SL_CHARTYPE_CHARACTER = 100, //!< 数字及字母
    SL_CHARTYPE_ONLYDIGITAL = 101, //!< 数字
    SL_CHARTYPE_ONLYALPHABET = 102, //!< 字母
} SLLPCharType_t;

typedef enum LPNumberStatus_s
{
    PLATE_NUMBER_CORRECT = 0, // correct lp num
    PLATE_NUMBER_ERROR = 1, // error lp num
    PLATE_NUMBER_NONE = 2, // have no lp num

} LPNumberStatus_t;

typedef enum LPStatus_s
{
    PLATE_YES = 0, // have lp
    PLATE_NO = 1, // have no lp

} LPStatus_t;

typedef enum VehicleSpaceStatus_s
{
    SPACE_IN = 0, // in
    SPACE_OUT = 1, // out
    SPACE_LINE = 2, // line

} VehicleSpaceStatus_t;

struct IntervalFrameList : public vulcan::BaseData
{
    IntervalFrameList()
        : vulcan::BaseData()
    {
        name_ = "interval_frame_list";
    }

    std::vector<aih::TimeInfo> datas;
};


struct LPNumber : public vulcan::BaseData
{
    uint8_t size = 0;
    char lp_number[24][4]{};
    std::string lp_str = "-";
    int lp_count = 0;
    float first_char_confidence = 0.0; // province
    float second_char_confidence = 0.0; // city
    float vlp_rec_confidence = 0.0;
    float vlp_rec_min_conf = 0.0;
    LPNumberStatus_t vlp_status = PLATE_NUMBER_NONE;
};

typedef std::shared_ptr<LPNumber> LPNumberPtr;

struct LPInfo : public vulcan::BaseData
{
    Rect location = Rect(0, 0, 5, 5); // xywh, xy is LeftTop
    SLLPType_t lp_type = SL_PLATE_UNCERTAIN;
    SLLPColor_t lp_color = SL_PLATE_COLOR_UNKNOWN;
    int lp_color_count = 0;
    float vlp_det_confidence = 0.0;
    // float                         lp_line_rat = 0.0;
    LPNumber lp_number;
    LPStatus_t lp_status = PLATE_NO;
    float lp_rat = 0.0;
};

typedef std::shared_ptr<LPInfo> LPInfoPtr;

struct LPInfoVec : public vulcan::BaseData
{
    uint16_t size = 0;
    std::vector<LPInfo> lp_info;
};

typedef std::shared_ptr<LPInfoVec> LPInfoVecPtr;

struct Veh_Key_Point
{
    float x = 0;
    float y = 0;
    float vis_score = 0;
};

struct VehInfo;
typedef std::shared_ptr<VehInfo> VehInfoPtr;

struct VehChassisInfo : public vulcan::BaseData
{
    std::vector<Point> chassis; // LeftTop, LeftBottom, RightBottom, RightTop
    // std::string                   space_res = "no"; //占用车位结果
    VehicleSpaceStatus_t space_res = SPACE_OUT;
    float space_rat = -2.0; // 车位占比
    int space_id = -1; // 第几个车位 按照给的车位顺序 从 0 开始
    std::vector<float> space_rat_vec;
    std::vector<float> car_rat_vec;
    int line_angle = -1; // 车方向与车位方向夹角

    float gradient = -1.0;

    // std::string                   space_res_ex = "no";
    VehicleSpaceStatus_t space_res_ex = SPACE_OUT;
    float space_rat_ex = -2.0;
    int space_id_ex = 0;
    std::vector<float> space_rat_vec_ex;
};

typedef std::shared_ptr<VehChassisInfo> VehChassisInfoPtr;

template <typename _Tp> struct Embedding : public vulcan::BaseData
{
    int32_t length_{};
    _Tp *value_{};
    bool allocd = false;
    Embedding() { type_ = "Embedding"; }

    Embedding(const _Tp *value, size_t length)
    {
        type_ = "Embedding";
        size_t len = sizeof(_Tp) * length;
        value_ = static_cast<_Tp *>(malloc(len));
        allocd = true;
        memcpy(value_, value, len);
        length_ = static_cast<int32_t>(length);
    }

    ~Embedding() override
    {
        if (allocd)
            free(value_);
        length_ = 0;
    }

    friend std::ostream &
    operator<<(std::ostream &out, Embedding &feat)
    {
        out << "(feature length: " << feat.length_ << ")";
        return out;
    }
};

typedef std::shared_ptr<Embedding<float> > EmbeddingPtr;

struct ReIdResult : public vulcan::BaseData
{
    std::vector<EmbeddingPtr> embeddings_;
    std::vector<bool> valid_features_;
    int32_t size_ = 0;
    int camera_channel{};
    int frame_id{};
    ReIdResult() { type_ = "ReIdResult"; }

    friend std::ostream &
    operator<<(std::ostream &out, ReIdResult &feat)
    {
        out << "(feature length: " << feat.size_ << ")";
        return out;
    }
};

typedef std::shared_ptr<ReIdResult> ReIdResultPtr;

enum class TrackState
{
    New = 0,
    Tracked = 1,
    Lost = 2,
    Removed = 3

};

// enum class VehInOutState
// {
//     VehOut = 0,
//     VehIn = 1,
//     UnKnown = 2
// };

typedef enum ParkRegion_s
{
    IMG_LEFT = 0,
    IMG_CENTER = 1,
    IMG_RIGHT = 2
}ParkRegion_t;

struct VehInfo : public vulcan::BaseData
{
    cv::Mat feature = cv::Mat(1, REID_FEATURE_LENGTH, CV_32F);
    Rect location = Rect(0, 0, 20, 20); // xywh, xy is LeftTop
    Rect pose_hull_location = Rect(0,0,20,20); //xywh, xy is LeftTop
    Rect kalman_prediction = Rect(0, 0, 0,20);
    VehChassisInfo veh_chassis;
    SLVehicleType_t veh_type = SL_VEHICLE_TYPE_UNKNOWN;
    SLVehicleColor_t veh_color = SL_VEHICLE_COLOR_UNKNOWN;
    float veh_type_conf = 0.0;
    float veh_color_conf = 0.0;
    float veh_det_confidence = 0.0;
    LPInfo lp_info;
    unsigned long long veh_timestamp = 0; //ms
    aih::TimeInfo time_info;
    long frame_id = -1;
    long veh_track_id = -1;
    int send_veh_id = -1;
    float cover_rat = 0.0; // 车辆被其他车辆遮挡部分占此车的比例——车下部
    bool is_image_edge = false; // 画面边缘
    // bool                          is_move = false;
    // float                         move_rat = -1;
    TrackState track_state_ = TrackState::Removed;
    // bool                          is_in_road = true;
    Veh_Key_Point veh_key_points[16];
    float visual_rate = 1.0;  //基于关键点可视化程度得出的车辆可视化率
};

struct VehInfoVec : public vulcan::BaseData
{
    explicit VehInfoVec(aih::TimeInfo time_info)
        : vulcan::BaseData(), time_info(time_info)
    {
        name_ = "veh_info";
    }

    uint16_t size = 0;
    unsigned long long frame_timestamp = 0;
    aih::TimeInfo time_info;
    long frame_id = -1;
    int frame_height = 0;
    int frame_width = 0;
    int camera_channel = -1;
    bool is_blind_camera = false;
    bool use_reid = true;
    std::vector<VehInfo> veh_info;
};

struct VehSendMessage : public vulcan::BaseData
{
    VehInfo veh_info; // 车辆信息
    SLVehicleState_t veh_state = SL_CAR_STATE_NOON; // 车辆状态
    int veh_state_fraction = 0; // 状态打分
    int channel_id = -1; // camera or channel id
    int veh_pic_id = -1; // 该状态对应抓拍图的ID
    unsigned long long frame_timestamp = 0;
    std::vector<Rect> veh_locs;
    Rect biger_box;

    VehSendMessage()
    {
        // veh_info_ptr = std::make_shared<VehInfo>();
        biger_box.x_ = 0;
        biger_box.y_ = 0;
        biger_box.width_ = 0;
        biger_box.height_ = 0;
    }
};

struct VehInOutInfo : public vulcan::BaseData
{
    int in_out = -1; // 进场：1 ，出场：0 ，其他
    SLVehicleState_t veh_state = SL_CAR_STATE_NOON; // 车辆状态
    int camera_channel = -1; //
    unsigned long long inout_timestamp = 0;
    VehInfo out_veh_info; // 车场外车辆信息
    VehInfo pro_veh_info; // 入出车场过程车辆信息
    VehInfo in_veh_info; // 车场内车辆信息
    LPInfo lp_info;

    unsigned long long in_timestamp = 0;
    unsigned long long send_timestamp = 0;
    std::string space_code = "0";

    int veh_state_fraction = 0; // 状态打分

    bool is_save = true;

    int order_generate_type = 0; // 驶入: 0 正常, 1 无驶入过程；驶出: 0 正常, 1 卡口, 2 泊位状态, 3 异常消失
};

// typedef std::shared_ptr<VehInOutInfo> VehInOutInfoPtr;

struct VehInOutInfoVec : public vulcan::BaseData
{
    uint16_t size = 0;
    std::vector<VehInOutInfo> veh_inout_info;
    bool order_match_enable = false;
};

struct VehInOutVar : public vulcan::BaseData
{
    int is_combine = 0; // 是否合并的

    SLVehicleState_t veh_state = SL_CAR_STATE_NOON; // 车辆状态

    float out_max_iou = -10.0;
    int is_satisfy_out = -1; // 针对消失未出场车辆 0:no 1:yes  -1:未判断
    int is_satisfy_out_flag = -1; // 针对消失未出场车辆
    int is_satisfy_in = -1; // 针对消失未进场车辆 0:no 1:yes  -1:未判断

    bool is_disappear = false;
    bool is_disappear_abnormal = false;
    bool obscured = false; // 是否被大车完全遮挡
    bool is_report_in = false; // 是否已经上传入场订单
    bool is_report_out = false; // 是否已经上传出场订单
    bool is_judge_suspected_in = false; // 是否已经判断疑似入场订单
    bool is_judge_suspected_out = false; // 是否已经判断疑似出场订单

    bool to_delete = false; // 是否需要删除
    bool to_report_in = false; // 是否需要上传入场订单
    bool to_report_out = false; // 是否需要上传出场订单
    int order_generate_type = 0;

    bool is_leave = false;
    bool is_standstill = false;

    bool no_send_in = false;
    int no_send_in_flag = -1;
    int no_send_out_flag = -1;

    int in_count = 0; // 连续in_count次检测到在场内
    int in_move_count = 0; // 连续in_move_count次检测到运动入场
    int in_stand_count = 0; // 连续in_stand_count次检测到场内不动
    int out_count = 0; // 连续out_count次检测到在场外
    int out_move_count = 0; // 连续out_move_count次检测到运动出场

    unsigned long long veh_appear_timestamp = 0; // 车辆出现时间
    unsigned long long veh_disappear_timestamp = 0; // 车辆消失时间
    unsigned long long report_in_timestamp = 0; // 上传入场订单时间
    unsigned long long report_out_timestamp = 0; // 上传出场订单时间

    // unsigned long long access_veh_timestamp = 0; // 判断卡口时间
};

// typedef std::shared_ptr<VehInOutVar> VehInOutVarPtr;

struct LpContinuous : public vulcan::BaseData
{
    std::string lp_str_continuous = "-";
    int         lp_count_continuous = 0;
    std::string lp_str_max = "-";
    int         lp_count_max = 0;
};

struct LastLpInfo : public vulcan::BaseData
{
    std::string lp_str = "-";
    unsigned long long timestamp = 0; //ms
};

struct CacheVehInfo : public vulcan::BaseData
{
    int channel = -1;
    long track_id = -1; // 车辆追踪ID

    int frame_height = 0;
    int frame_width = 0;

    VehInOutVar veh_in_out_var;

    std::vector<VehInfo> veh_infos;
    std::vector<VehInfo> veh_infos_first_n;
    std::vector<Rect> veh_locs_first_n;

    std::vector<std::pair<std::string, float> > lp_str_vec; // 连续n帧识别的正确车牌
    std::vector<SLLPColor_t> lp_color_vec; // 连续n帧识别的正确车牌颜色
    std::vector<SLVehicleColor_t> veh_color_vec; // 连续n帧识别的车辆颜色
    std::vector<SLVehicleType_t> veh_type_vec; // 连续n帧识别的车辆类型
    int no_lp_count = 0;
    std::vector<int> sapceID_vec; // 连续n帧的SpaceID

    VehSendMessage veh_inpro_out_mess; // 预驶入
    VehSendMessage veh_inpro_pro_mess; // 驶入
    VehSendMessage veh_standstill_mess; // 车停稳
    LPInfo in_lp_info;

    VehSendMessage veh_outpro_std_mess; // 车停稳 二阶段
    VehSendMessage veh_outpro_pro_mess; // 驶出
    VehSendMessage veh_outpro_out_mess;
    LPInfo out_lp_info;

    LPInfo lp_info;

    LpContinuous lp_continuous;

    LastLpInfo   last_lp_info;

private:
    std::shared_ptr<illegal_parking::MultipleIllegalCaptureDetail> multiple_illegal_capture_detail;

public:
    std::shared_ptr<illegal_parking::MultipleIllegalCaptureDetail> &
    getMultipleIllegalCaptureDetail();
};

struct CacheVehInInfo : public vulcan::BaseData
{
    bool no_send_in = false; // 是否上报
    unsigned long long report_in_timestamp = 0; // 上传入场订单时间
    unsigned long long last_timestamp = 0; // 最新出现时间

    unsigned long long judge_out_last_timestamp = 0; // 最新根据泊位状态判断驶出时间

    std::vector<VehInfo> veh_infos_first_n;
    std::vector<Rect> veh_locs_first_n;

    VehInfo veh_info;

    bool is_load = false;

};

// typedef std::shared_ptr<CacheVehInInfo> CacheVehInInfoPtr;

struct CacheVehOutInfo : public vulcan::BaseData
{
    unsigned long long report_out_timestamp = 0; // 上传订单时间
    VehInfo veh_info;
};

struct CacheVehInfoVec : public vulcan::BaseData
{
    aih::TimeInfo time_info;
    unsigned long long frame_timestamp = 0;
    long frame_id = -1;
    int camera_channel = -1;
    // bool is_blind_camera = false;
    bool generate_suspected_order = false;
    bool use_reid = true;
    std::vector<CacheVehInfo> veh_cache_infos;
};

//! 车位状态码
typedef enum ParkingState_s
{
    SL_PARKING_STATE_NOON = 0x00, //!< 不确定
    SL_PARKING_STATE_EMPTY = 0x01, //!< 车位为空
    SL_PARKING_STATE_IN = 0x02, //!< 车位占用
} ParkingState_t;



// 报警类别码
typedef enum ParkingAlaemType_s
{
    SL_PARKING_NO_ALARM = 0XA100, //!< 无报警
    SL_PARKING_ALARM_COVER = 0XA101, //!< 遮挡报警
    SL_PARKING_ALARM_NO_PLATE = 0XA102, //!< 无牌车报警
    SL_PARKING_ALARM_NO_VEHICLE = 0XA103, //!< 非机动车报警
    SL_PARKING_ALARM_ACROSS = 0XA104, //!< 跨位报警
    SL_PARKING_ALARM_WRONG_AIRECTION = 0XA105, //!< 车头车尾不一致
    SL_PARKING_ALARM_REPEATED_INOUT = 0XA106, //!< 反复出入车位
    SL_PARKING_ALARM_COVER_PLATE = 0XA107, //!< 车牌遮挡
    SL_PARKING_ALARM_ILLEGAL_PARK = 0XA108, //!< 违章停车
    SL_PARKING_ALARM_HILT_PARK = 0XA109, //!< 斜位停车

    //======================报警消除=============================================
    SL_PARKING_ALARM_COVER_DISAPPEAR = 0XA201, //!< 遮挡报警
    SL_PARKING_ALARM_NO_PLATE_DISAPPEAR = 0XA202, //!< 无牌车报警
    SL_PARKING_ALARM_NO_VEHICLE_DISAPPEAR = 0XA203, //!< 非机动车报警
    SL_PARKING_ALARM_ACROSS_DISAPPEAR = 0XA204, //!< 跨位报警
    SL_PARKING_ALARM_WRONG_AIRECTION_DISAPPEAR = 0XA205, //!< 车头车尾不一致
    SL_PARKING_ALARM_REPEATED_INOUT_DISAPPEAR = 0XA206, //!< 反复出入车位；
    SL_PARKING_ALARM_COVER_PLATE_DISAPPEAR = 0XA207, //!< 车牌遮挡
    SL_PARKING_ALARM_ILLEGAL_PARK_DISAPPEAR = 0XA208, //!< 违章停车
    SL_PARKING_ALARM_HILT_PARK_DISAPPEAR = 0XA209, //!< 斜位停车
} ParkingAlaemType_t;

// 报警需要关联的车辆信息
struct AlarmMessVehParking : public vulcan::BaseData
{
    // SL_BBox_S						stCarRect;                    		//!<
    // 车辆目标矩形框 SL_BBox_S						stPlateRect;
    // //!< 车牌矩形框 SL_S8							as8PlateNum[SL_VLPR_PLATE_BUFLEN];
    // //!< 车位中的车牌号 SL_FLOAT						flConfidence;
    // //!< 车牌置信度 SL_PLATECOLOR_E			    	ePlateColor; //!< 车牌颜色 SL_PLATETYPE_E
    // ePlateType;							//!< 车牌类型 SL_S32 s32ParkingID[2];
    // //!< 对应车位的ID编号 SL_PARKING_STATE_E				eParkingState;
    // //!< 车位的状态 SL_U32							u32CarID;
    // //!< 数据对应ID编号

    Rect veh_location; //!< 车辆目标xywh
    LPInfo lp_info;
    int parking_id[2] = {0}; //!< 对应车位的ID编号
    ParkingState_t parking_state = SL_PARKING_STATE_NOON; //!< 车位的状态
    long veh_track_id = 0; //!< 数据对应ID编号

    int send_veh_id = 0;
};

//! 单个报警信息
struct AlarmMessage : public vulcan::BaseData
{
    ParkingAlaemType_t alarm_type = SL_PARKING_NO_ALARM; //!< 报警信息类型
    int alarm_score = 0; //!< 报警信息分值；
    bool alarm_save_mess = false; //!< 报警信息是否需要上传（取值: false 不需要 true 需要）
    AlarmMessVehParking alarm_mess; //!< 报警相关的信息
    int as32Reserved[16]; //!< 保留
};

// 所有报警信息
struct AlarmResult : public vulcan::BaseData
{
    int alarm_num = 0; //!< 报警的总数
    AlarmMessage alarm_mess[alarm_num_max]; //!< 报警信息
    int as32Reserved[16]; //!< 保留
};

// typedef std::shared_ptr<AlarmResult> AlarmResultPtr;

struct CapBufControl
{
    std::vector<int> cap_buf_control_enable_ids_; // 可用的cap_buf_control id
    std::vector<int> cap_buf_control_del_ids_; // 需要删除的cap_buf_control id
    int cap_buf_control_num_[max_cap_buf_len] = {0};
    long cap_id_frame_id_[max_cap_buf_len] = {-1};

    int cap_id_del_[max_cap_buf_len] = {-10};

    int cap_id_ = -1;
    long frame_id_now_ = -1;

    bool needInit_ = false;
};

//! 抓拍缓存控制--指令码
typedef enum CapBufCmd_s
{
    SL_CAPBUF_CMD_NOON = 0x00, //!< 无
    SL_CAPBUF_CMD_NEW = 0x01, //!< 创建
    SL_CAPBUF_CMD_REPLACE = 0x02, //!< 替换
    SL_CAPBUF_CMD_DEL = 0x03, //!< 删除

    SL_CAPBUF_CMD_TOTAL //!< 结束符
} CapBufCmd_t;

//! 抓拍缓存控制--消息格式定义
struct CapBufControlMsg : public vulcan::BaseData
{
    bool enable = false; //!< 控制使能 （FALSE / TRUE）
    CapBufCmd_t cap_buff_cmd = SL_CAPBUF_CMD_NOON; //!< 抓拍缓存区控制指令
    int channel = -1;
    int pic_id; //!< 对应图片ID （取值：范围 参考宏定义 max_cap_buf_len）
    long frame_id; //!< 对应图片的帧序号
    long frame_id_pre = -1; // SL_CAPBUF_CMD_REPLACE 时使用
    int as32Reserved[16]; //!< 保留
};

// typedef std::shared_ptr<CapBufControlMsg> CapBufControlMsgPtr;

// 所有抓拍缓存控制信息
struct CapControlMsgVec : public vulcan::BaseData
{
    int size = 0;
    std::vector<CapBufControlMsg> cap_buf_control_vec; // 控制信息
};

// typedef std::shared_ptr<CapControlMsgVec> CapControlMsgVecPtr;

struct SpaceInfo : public vulcan::BaseData
{
    int space_count = 0; // 泊位数量
    std::vector<int> space_index; // 车位的index
    std::vector<std::string> space_code; // 车位的编号
    std::vector<float> space_gradient; // 车位的斜率
    std::vector<std::vector<Point> >
    space_points; // 实际车位信息，四个点一个车位，LeftTop, LeftBottom, RightBottom, RightTop
    std::vector<std::vector<Point> >
    space_points_expand; // 扩大的车位信息，四个点一个车位，LeftTop, LeftBottom, RightBottom, RightTop
    std::vector<Point> space_vector_; // 泊位方向，始末点坐标

    ParkRegion_s  park_region = IMG_CENTER;
};

// typedef std::shared_ptr<SpaceInfo> SpaceInfoPtr;

struct ParkParameter : public vulcan::BaseData
{
    float veh_in_rat = 0.25;
    float veh_line_rat = 0.1;

    int veh_in_time_thres = 20; // 入场时间阈值 秒
    int veh_out_time_thres = 20; // 出场时间阈值 秒
};

// data process workflow framework outputs
struct DataProcOutputs : public vulcan::BaseData
{
    VehInOutInfoVec veh_inout_info_vec;
    // CapControlMsgVec cap_control;
    std::shared_ptr<VehInfoVec> vehicles;
    // ParkingResult parking_result;
    std::vector<aih::CachedFrameInfo> frame_infos;
    // std::vector<VehInfosCloudAssist> cloud_assist_veh_infos;

};

typedef std::shared_ptr<DataProcOutputs> DataProcOutputsPtr;

// access configure
struct AccessConfig_t : public vulcan::BaseData
{
    AccessConfig_t()
    {
        name_ = "access_config";
    }

    int enable = 0;
    int line_num = 0;
    int l1_control_id = -1;
    int l1_event_id = -1;
    std::string line1_name = "-"; // top line
    Line line1 = Line(Point(0, 0), Point(1, 0));
    aih::SingleCrossLineEvent line1_event;

    int l2_control_id = -1;
    int l2_event_id = -1;
    std::string line2_name = "-"; // bottom line
    Line line2 = Line(Point(0, 1), Point(1, 1));
    aih::SingleCrossLineEvent line2_event;

};

typedef std::shared_ptr<AccessConfig_t> AccessConfigPtr;

struct TrajectoryInfo : public vulcan::BaseData
{
    int camera_id;
    long track_id;
    TrackState track_state;
    size_t start_trk_time;
    size_t last_trk_time;

    TrajectoryInfo()
        : camera_id(0),
          track_id(0),
          track_state(),
          start_trk_time(0),
          last_trk_time(0)
    {
        type_ = "TrajectoryInfo";
    };
};


class VehiclesCacheList : public vulcan::BaseData
{
public:
    std::vector<CacheVehInfo *> data;
};


} // namespace sonli

#endif
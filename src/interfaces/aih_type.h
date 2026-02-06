//
// AIH库数据类型定义
// 包含所有输入输出的结构体定义，与原有结构体一一对应
//

#ifndef AIH_TYPE_H_
#define AIH_TYPE_H_

#include "aih_prop.h"
#include <cstdint>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <ostream>
namespace aih {

typedef short S16;
typedef int S32;
typedef unsigned int U32;
typedef unsigned char U8;
typedef long long PHYS_ADDR_T;
typedef long long VIRT_ADDR_T;
typedef void HANDLE;
typedef int BOOL;
typedef char CHAR;
typedef unsigned long long U64;
typedef float FLOAT;
typedef double DOUBLE;

// 点模板
template<typename tp>
class _Point {
 public:
  tp x;
  tp y;
};

typedef _Point<int> Point2i;
typedef _Point<long> Point2l;
typedef _Point<float> Point2f, Vector2f;
typedef _Point<double> Point2d;

//线模板
template<typename tp>
class _Line {
 public:
  _Point<tp> start_point_;
  _Point<tp> end_point_;
};
typedef _Line<int> Line2i;
typedef _Line<long> Line2l;
typedef _Line<float> Line2f;
typedef _Line<double> Line2d;

const static int frame_chn_num = 3;
struct TimeInfo {
  U64 pts_ = 0;                        //系统运行时间戳,算法内部默认使用此时间
  U64 local_time_ = 0;                 //unix/windows系统时间，通常会受时间矫正影响，一般直接输出
};

struct ImageInfo {
  U64 phy_[frame_chn_num];
  U64 vir_[frame_chn_num];
  U64 stride_[frame_chn_num];
  S32 width_;
  S32 height_;
  TimeInfo time_info_;
};

//对象分类-主类
enum ObjMainCls {
  OBJ_MAIN_CLS_UNKNOWN = 0,
  //通用
  OBJ_MAIN_CLS_PERSON,            //人
  OBJ_MAIN_CLS_AOTOMOBILE,        //机动车
  OBJ_MAIN_CLS_NONVEHICLE,        //非机动车
  OBJ_MAIN_CLS_PARKING,           //泊位

  OBJ_MAIN_CLS_CAR_CLOTHES = 10,   //车衣
  OBJ_MAIN_CLS_MANHOLE = 18,   //井盖
  OBJ_MAIN_CLS_UNCOVERED_MANHOLE = 19,   //空井
  OBJ_MAIN_CLS_TRICYCLE = 23,   //三轮车
  OBJ_MAIN_CLS_STREET_STALL = 29,   //小摊
  OBJ_MAIN_CLS_HANDCART = 98,   //手推车
  OBJ_MAIN_CLS_BILLBOARD = 216,  //广告牌

  //动物类
  OBJ_COMPT_TYPE_OTHER_CAT,//猫
  OBJ_COMPT_TYPE_OTHER_DOG,//狗
  OBJ_COMPT_TYPE_OTHER_CHICKEN,//鸡
  OBJ_COMPT_TYPE_OTHER_DUCK,//鸭子
  OBJ_COMPT_TYPE_OTHER_CATTLE, //牛
  OBJ_COMPT_TYPE_OTHER_SHEEP,//羊
  OBJ_COMPT_TYPE_OTHER_PIG,//猪
  OBJ_COMPT_TYPE_OTHER_GOOSE,//鹅
  //
  CLS_OTHER                       //异物
};

enum ObjComponetType {
  OBJ_COMPT_TYPE_UNKNOWN = -1,

  OBJ_COMPT_TYPE_PERSION_HEAD = 0,        //人头
  OBJ_COMPT_TYPE_PERSION_FACE = 1,        //人脸
  OBJ_COMPT_TYPE_PLATE = 2,        //车牌
  OBJ_COMPT_TYPE_HEADSTOCK = 3,        //车头
  OBJ_COMPT_TYPE_TAILSTOCK = 4,        //车尾
  OBJ_COMPT_TYPE_CAR = 5,        //机动车
  OBJ_COMPT_TYPE_SIDE_WINDOW = 6,        //侧车窗
  OBJ_COMPT_TYPE_WINDOW = 7,        //车窗
  OBJ_COMPT_TYPE_ROOF = 8,        //车顶
  OBJ_COMPT_TYPE_CAR_CLOTHES = 10,       //车衣
  OBJ_COMPT_TYPE_MANHOLE = 18,       //井盖
  OBJ_COMPT_TYPE_UNCOVERED_MANHOLE = 19,  //空井
  OBJ_COMPT_TYPE_NONVEHICLE = 20,       //非机动车
  OBJ_COMPT_TYPE_TRICYCLE = 23,       //三轮车
  OBJ_COMPT_TYPE_STREET_STALL = 29,       //小摊
  OBJ_COMPT_TYPE_HANDCART = 98,       //手推车
  OBJ_COMPT_TYPE_PERSION_BODY = 118,      //人形
  OBJ_COMPT_TYPE_BILLBOARD = 216,      //广告牌

  OBJ_COMPT_TYPE_NUM
};

//目标外接矩形框
typedef struct _obj_rect {
  S32 x_ = 0;
  S32 y_ = 0;
  S32 width_ = 0;
  S32 height_ = 0;
} ALG_ObjRect;

//控件类型
enum ControlType {
  CONTROL_TYPE_UNKNOWN = -1,
  CONTROL_TYPE_AREA = 0, //区域
  CONTROL_TYPE_LINE,     //线
  CONTROL_TYPE_BERTH,     //泊位
  CONTROL_TYPE_VIOLATION, //违停
  CONTROL_TYPE_LANE,      //车道

  CONTROL_TYPE_NUM
};

//车牌信息
struct PlateInfo {
  float plate_score_ = 0.f;         //车牌号置信度
  float plate_type_prob_ = 0.f;     //车牌类型置信度
  std::string plate_str_ = "";      //车牌号

  int plate_type_id_ = 0;      //车牌类型ID,见PLATE_TYPE
  int plate_color_id_ = 0;     //车牌颜色ID,见PLATE_COLOR
  int plate_layer_id_ = 0;     //车牌单双层ID

  std::string plate_type_str = "";//车牌类型
  std::string plate_color_str_ = "";//车牌颜色
  std::string plate_layer_str_ = "";//车牌单、双层

  std::vector<float> plate_scores_;      //车牌字符置信度
  std::vector<std::string> plate_chars_; //车牌每一个字符
};

struct PlateInfoExt {
  PlateInfo info;        //车牌信息
  float plate_score = 0.f; //车牌检测分值
  float plate_width = 0.f; //车牌宽度
  ALG_ObjRect plate_loc; //车牌位置
};

const int PROP_UNKNOWN = -1;
struct SinglePropDefine {
  int type_id_ = PROP_UNKNOWN;
  std::string type_str_ = "";
  float type_score_ = 0.f;
};

typedef SinglePropDefine CarTypeProp;     //车型
typedef SinglePropDefine CarColorProp;    //车色
typedef SinglePropDefine CarBrandProp;    //车标

//机动车属性
struct CarProperty {
  float car_plate_score_ = 0.f; //车牌检测分值
  float car_plate_width_ = 0.f; //车牌宽度
  float car_stock_score_ = 0.f; //车头、车尾检测分值
  float car_stock_width_ = 0.f; //车头、车尾宽度
  float car_body_score_ = 0.f;  //车身检测分值
  float car_body_width_ = 0.f;  //车身宽度

  PlateInfo plate_info_;  //车牌
  std::list<PlateInfoExt> plate_infos_; //多个车牌信息，note:尽量用这个
  CarTypeProp car_type_;  //车型
  CarColorProp car_color_;//车色
  CarBrandProp car_brand_;//车标
};

enum EventType {
  EVENT_TYPE_NO_EVENT = 0,          //无事件

  EVENT_TYPE_CROSS_LINE = 1,        //拌线事件

  EVENT_TYPE_ENTER_AREA = 100,      //进入区域事件
  EVENT_TYPE_STAY_AREA,             //停留区域事件
  EVENT_TYPE_LEAVE_AREA,            //离开区域事件
  EVENT_TYPE_SHIELD_AREA,           //屏蔽区域,区域内的跟踪目标，会被屏蔽掉其他事件的触发
  EVENT_TYPE_MUTI_STAY_AREA,        //多个目标停留区域事件

  EVENT_TYPE_ENTER_BERTH = 200,     //进入泊位事件
  EVENT_TYPE_STAY_BERTH,            //停留泊位事件
  EVENT_TYPE_LEAVE_BERTH,           //离开泊位事件
  EVENT_TYPE_BERTH_REALTIME,        //泊位实时结果
  EVENT_TYPE_ALARM_BERTH,           //泊位告警结果

  EVENT_TYPE_ENTER_VIOLATION = 300, //进入违停区域事件
  EVENT_TYPE_ALARM_VIOLATION = 304, //违停告警事件

  EVENT_TYPE_LANE_CONGESTION = 400, // 车道拥堵事件类型 todo 待定
};

/******************************事件模块配置******************************/
//控件信息
struct ControlInfo {
  S32 id_ = -1;    //控件control ID >=0 有效
  ControlType control_type_ = CONTROL_TYPE_UNKNOWN;
  std::vector<Point2i> point_infos_; // 输入点连续，且0-1-2...n为线段方向

};

//拌线事件配置

//目标通过拌线时，运动方向
enum MoveDirectionThroughLine {
  MOVE_DIRECTION_ANY = 0,         //任何方向通过拌线
  MOVE_DIRECTION_LEFT2RIGHT,      //从左向右通过拌线
  MOVE_DIRECTION_RIGHT2LEFT       //从右向左通过拌线
};

//单个拌线事件定义
struct SingleCrossLineEvent {
  MoveDirectionThroughLine move_direction_ = MOVE_DIRECTION_ANY;
  FLOAT crossline_area_percent_ = 0.5;                              // 过线面积比


};

//拌线控件，包含的所有事件配置
struct CrossLineEventCfg {
  std::vector<SingleCrossLineEvent> crossline_events_;
};

typedef struct _area_event {
  FLOAT in_area_percent_ = 0.5f; // 面积比阈值，大于阈值，才认为进入区域
} SingleEnterAreaEvent, SingleShieldAreaEvent;

typedef struct _leave_area {
  FLOAT enter_in_area_percent_ = 0.8f; //面积比阈值，大于阈值进入区域后，才能触发离开事件
  FLOAT leave_in_area_percent_ = 0.1;  //面积比阈值，小于阈值后，触发离开事件
} SingleOutputAreaEvent;

//滞留事件配置
struct SingleStayAreaEvent {
  bool interval_trigger_ = false;       //事件是否间隔触发
  U32 stay_time_ = 5000;             //滞留时间阈值,单位毫秒
  FLOAT in_area_percent_ = 0.5;      //滞留在区域内的面积比
};

typedef struct MutiStayAreaEvent {
  bool interval_trigger_ = false;       //事件是否间隔触发
  U32 stay_count_ = 5;
  U32 stay_time_ = 5000;             //滞留时间阈值,单位毫秒
  FLOAT in_area_percent_ = 0.5;      //滞留在区域内的面积比
} MutiStayAreaEvent, SingleLaneCongestionEvent;

#define MAX_DEPLOY_TIME_COUNT 10

struct DeployTime {
  U32 start_time;                    //0-1439，单位分
  U32 end_time;                      // 0-1439，单位分

};

struct DeployTimeCfg {
  U32 time_counter[7] = {0}; // 每天的布防个数，最大值为 MAX_DEPLOY_TIME_COUNT
  DeployTime time_durings[7][MAX_DEPLOY_TIME_COUNT]; // 布防时间段
  DeployTimeCfg()
  {
      for (int i = 0; i < 7; i++)
      {
          time_counter[i] = 1;
          time_durings[i][0].start_time = 0;
          time_durings[i][0].end_time = 1439;
      }
  }

};

typedef struct LoopAlarmParam {
  //异常事件配置
  BOOL bReportNonRecoPlate = false; // 车牌未识别/遮挡报警
  BOOL bReportNoPlateError = false; // 无牌车报警
  BOOL bReportProbablyInOutError = false; // 疑似出入场报警

  BOOL bReportReverseParkingError = false; // 逆停报警
  U32 reportReverseParkingType = 1;     // 车头车尾朝向，1车头报警，2车尾报警

  BOOL bReportCrossLineError = false; // 泊位内压线报警
  U32 reportCrossLineSensitivity = 50;    // 压线停车报警灵敏度 0-100

  BOOL bReportCrossBerthError = false; // 跨双车位报警
  U32 reportCrossBerthSensitivity = 50;    // 跨双车位报警灵敏度 0-100
  BOOL reportWholeCrossedBerth = 0;     // 跨泊位停车是否上报所有信息

  BOOL bReportVerticalParkingError = false; // 垂直停车报警
  BOOL bReportDiagonalParkingError = false; // 斜停报警

  BOOL bReportLongTermParkingError = false; // 长时间停车报警
  U32 reportLongTermParkingWaitTime = 36000; // 长时间停车阈值时间

  BOOL bReportBanTimeError = false; // 禁停时间段停车
  U32 reportBanTimeWaitTime = 180;   // 禁停时间段停车报警等待时间
  DeployTimeCfg banTimeDurings;                 // 可停车时间段，时间段外视为禁停


} LoopAlarmParam, SingleBerthAlarmEvent;

typedef struct ViolationAlarmParam {
  U32 reportProhibitWaitTime = 180;   // 违停区域报警等待时间
  U32 violationType = 0;              // 0禁区违停 1泊位外违停 2泊位外尽头违停

}
    SingleViolationAlarmEvent;

typedef struct berth_event_cfg {
  //泊位事件
  U32 report_mode = 0;              //上报模式，0证据链模式，1事件模式
  U32 coil_outspread = 0;          //线圈外扩比例
  U32 coil_sensitivity = 0;        //线圈灵敏度
  U32 pic_select_in_time = 180;    //入场阈值时间
  U32 pic_select_in_time_min = 10;  //入场阈值时间最小值
  U32 pic_select_out_time = 180;   //出场阈值时间
  U32 pic_select_out_time_min = 10; //出场阈值时间最小值
  BOOL report_nonmotor = false;       //是否上报非机动车
  //高级设置
  U32 default_province = 255;        //默认省份  PROVINCE_TYPE
  U32 recognition_type = 1 | 2 | 8 | 16 | (1 << 31);  //车牌识别的类型

  //custom设置
  U32 enter_pic_num = 3;  // 入场证据链上报图片数量
  U32 leave_pic_num = 3;  // 出场证据链上报图片数量

} SingleBerthEvent, SingleViolationEvent;

#define MAX_RESULT_CNT 6

union SingleEventCfgInfo {
  SingleEventCfgInfo() {
    stay_area_cfg_.interval_trigger_ = false;
    stay_area_cfg_.in_area_percent_ = 0.5;
    stay_area_cfg_.stay_time_ = 5000;
  }
  SingleCrossLineEvent line_cfg_;
  SingleEnterAreaEvent enter_area_cfg_;
  SingleOutputAreaEvent leave_area_cfg_;
  SingleStayAreaEvent stay_area_cfg_;
  MutiStayAreaEvent muti_stay_area_cfg_;
  SingleShieldAreaEvent shield_area_cfg_;
  SingleBerthEvent berth_cfg_;
  SingleBerthAlarmEvent berth_alarm_cfg_;
  SingleViolationEvent violation_cfg_;
  SingleViolationAlarmEvent violation_alarm_cfg_;
  SingleLaneCongestionEvent lane_cfg_;
};

struct EventFilterCfg {
  S32 obj_width_ = -1;          //过滤宽度小于阈值的目标, -1 不生效
  S32 obj_height_ = -1;         //过滤高度小于阈值的目标, -1 不生效
  FLOAT obj_score_ = -0.f;      //过滤评分小于阈值的目标，负值不生效。评分指目标的综合评分，如人脸评分、车牌评分
  FLOAT obj_det_score_ = -0.f;  //过滤检测评分小于阈值的目标，负值不生效。
};

struct SingleEventCfg {
  S32 control_id_ = -1;
  S32 event_id_ = -1;      //事件ID，同一控件里事件ID唯一，由调用者负责唯一性
  EventType event_type_ = EVENT_TYPE_NO_EVENT;
  bool enable_event_ = false;                                 //配置事件是否生效，默认所有事件不生效
  DeployTimeCfg deploy_time_cfg_;                            //布防时间段配置
  std::set<ObjMainCls> obj_main_cls_filter_;                  //空时，支持任何主目标类别；非空，只支持map中的主类别触发事件
  std::map<ObjComponetType, EventFilterCfg> obj_filter_cfg_;  //空时，事件判断只以主类别的box计算；非空，map中的子类别box计算
  SingleEventCfgInfo cfg_info_;

};

struct CachedFrameInfo {
  TimeInfo tm_info_;
};

typedef std::vector<CachedFrameInfo> CachedFrameInfos;

enum LoopStatus : U32 {
  LOOP_STATUS_UNKNOWN = 0,
  LOOP_STATUS_IN = 1,             // 入场
  LOOP_STATUS_STABLE = 2,         // 停稳
  LOOP_STATUS_OUT = 4,            // 出场
  LOOP_STATUS_FREE = 8,           // 空闲
  LOOP_STATUS_ERROR = 16,         // 异常
  LOOP_STATUS_OUTDELAY = 32,      // 出场延迟上报
  LOOP_STATUS_INDELAY = 64,       // 入场延迟上报
  LOOP_STATUS_PREIN = 128,        // 预入场
  LOOP_STATUS_PREOUT = 256,       // 预出场
  LOOP_STATUS_PLATE_UPDATE = 512,  // 车牌更新
  LOOP_STATUS_RECO_TRIGGER = 1024,  // 过路车抓拍触发
  LOOP_STATUS_USER_EXT_FRAME_FIRST = 2048,  // 客户定制图片1
  LOOP_STATUS_USER_EXT_FRAME_SECOND = 4096,  // 客户定制图片2
  LOOP_STATUS_USER_EXT_FRAME_THIRD = 8192  // 客户定制图片3
};

enum ParkErrorType : U32 {
  ParkErrorType_Non = 0,           // 正常

  // 视频画面类问题
  ParkErrorType_VideoBlurError = 1 << 0,      // 视频模糊
  ParkErrorType_PlateCoveredError = 1 << 1,      // 车牌遮挡

  // 车辆本身问题
  ParkErrorType_NoPlateError = 1 << 2,      // 无牌车占位
  ParkErrorType_NonMotorError = 1 << 3,      // 非机动车报警

  // 停车违规类问题
  ParkErrorType_ProhibitError = 1 << 4,      // 禁停区域停车
  ParkErrorType_ReserveError = 1 << 5,      // 逆向停车
  ParkErrorType_CrossLineError = 1 << 6,      // 压线停车
  ParkErrorType_CrossSlotError = 1 << 7,      // 跨泊位停车
  ParkErrorType_VerticalParkError = 1 << 8,      // 垂直停车
  ParkErrorType_DiagonalParkError = 1 << 9,      // 斜向停车
  ParkErrorType_BanTimeError = 1 << 10,     // 禁止时段停车

  // 出入场异常
  ParkErrorType_ProbablyParkInError = 1 << 11,     // 疑似入场报警
  ParkErrorType_ProbablyParkOutError = 1 << 12,     // 疑似出场报警

  // 泊位相关问题
  ParkErrorType_LongTermParkError = 1 << 13      // 长期停车
};

struct AlarmInfo {
  U32 alarm_type_ = ParkErrorType_Non;
  U32 alarm_status_ = ParkErrorType_Non;
};

struct BerthCarInfo {
  ALG_ObjRect car_loc_;
  ALG_ObjRect non_motor_loc_;
  ALG_ObjRect tricycle_loc_;
  ALG_ObjRect plate_loc_;
  PlateInfo plate_info_;
  ALG_ObjRect hk_plate_loc_;
  PlateInfo hk_plate_info_;
  ALG_ObjRect mc_plate_loc_;
  PlateInfo mc_plate_info_;
  ALG_ObjRect roof_loc_;
  ALG_ObjRect side_window_loc_;
  ALG_ObjRect window_loc_;
  ALG_ObjRect head_loc_;
  ALG_ObjRect tail_loc_;
  U32 covered_score_ = 100; //遮挡打分,取值0-100,越高越好(越不遮挡)，用以外部取最不遮挡坐标识别车身颜色以及车型
  U32 has_car_body_ = 0;  //是否有机动车信息
  U32 plate_isocc_ = 0; //车牌是否被遮挡
  FLOAT plate_occ_rate_ = 0; //车牌被遮挡比例
  U32 confidence_ = 0; //置信度
};

struct BerthEventInfo {
  U32 result_cnt_ = 0;      //结果个数，最大为MAX_RESULT_CNT
  AlarmInfo alarm_info_;      //报警信息
  U32 report_mode_ = 0;      //上报模式
  CarProperty car_property_;      //车辆属性
  FLOAT sharpness_[MAX_RESULT_CNT] = {0};  //车牌清晰度
  FLOAT plate_mean_[MAX_RESULT_CNT] = {0};  //车牌像素均值
  U32 can_drop_[MAX_RESULT_CNT] = {0};  //是否可丢弃
  LoopStatus parking_state_[MAX_RESULT_CNT] = {LOOP_STATUS_UNKNOWN};      //停车状态
  BerthCarInfo car_info_[MAX_RESULT_CNT];
  CachedFrameInfo frame_info_[MAX_RESULT_CNT];
  U32 car_gesture_ = 0;      //0无效，1车头，2车尾
  U32 order_score_ = 0;      //订单分数
  U32 covered_event = 0;      //遮挡事件，0无遮挡，1遮挡
  U64 enter_start_timestamp_ = 0;      //进入泊位开始时间戳
  U32 force_leave_flag_ = 0;      //强制离开标志
  U32 group_id_ = -1;      //泊位组ID
  U32 non_motor_car_type_ = 0;      //非机动车类型
  U32 non_motor_id_ = -1;      //非机动车id
  U32 out_video_start_ = 0;      //开始录制视频的标志，0:不处理，1:出场, 2 : 入场
  U32 recent_plate_ = 0;      //车牌是否移动过
  U32 plate_covered_ = 0;      //车牌是否被遮挡
  U32 car_stable_count_ = 0;      //车辆停稳的帧计数
};

/******************************事件模块输出******************************/
struct EventInfo {
  EventType event_type_ = EVENT_TYPE_NO_EVENT;
  U64 track_id_ = 0;
  S32 control_id_ = -1;
  S32 event_id_ = -1;
  TimeInfo time_info_;
  MoveDirectionThroughLine move_line_dir_ = MOVE_DIRECTION_ANY;
  BerthEventInfo berth_event_info_;
  std::list<U64> event_related_ids_;
};
typedef std::vector<EventInfo> EventInfos;

//计数统计配置
struct CountObjCfg {
  S32 ctrl_id = -1;              // < 0 非法ID，直接返回0
  S32 obj_cls = -1;              // < 0 统计所有跟踪目标类别 >= 0 只统计指定类别
  FLOAT in_area_percent = 0.5f;  // 目标在区域内，面积比例阈值
};

} // namespace AIH

#endif // AIH_TYPE_H_ 
#pragma once
#include <string>
#include <vector>
namespace aih {

const std::vector<std::string> color_pair = {
    "其他颜色",
    "棕色",
    "白色"
    "红色"
    "蓝色"
    "银色"
    "黑色"
};

enum CAR_COLOR {
  COLOR_OTHER = 0,  //其他颜色
  COLOR_BROWN = 1,  //棕色
  COLOR_WHITE = 2,  //白色
  COLOR_RED = 3,    //红色
  COLOR_BLUE = 4,   //蓝色
  COLOR_SILVER = 5, //银色
  COLOR_BLACK = 6   //黑色
};

const std::vector<std::string> type_pair = {
    "MPV",
    "SUV",
    "中型客车"
    "中型货车"
    "小型客车"
    "小型货车"
    "轿车"
    "重型货车",
    "面包车"
};

enum BRAND_TYPE {
  TYPE_MPV = 0,
  TYPE_SUV = 1,
  TYPE_MEDIUM_BUS = 2,
  TYPE_MEDIUM_TRUCK = 3,
  TYPE_SMALL_BUS = 4,
  TYPE_SMALL_TRUCK = 5,
  TYPE_CAR = 6,
  TYPE_HEAVY_TRUCK = 7,
  TYPE_BREAD = 8
};

typedef enum {
  LT_UNKNOWN = 0, // 未知车牌
  LT_BLUE,        // 蓝牌小汽车
  LT_BLACK,       // 黑牌小汽车
  LT_YELLOW,      // 单排黄牌
  LT_YELLOW2,     // 双排黄牌（大车尾牌，农用车）
  LT_POLICE,      // 警车车牌
  LT_ARMPOL,      // 武警车牌
  LT_INDIVI = 7,  // 个性化车牌
  LT_ARMY,        // 单排军车牌
  LT_ARMY2,       // 双排军车牌
  LT_EMBASSY,     // 使馆车牌 //10
  LT_HONGKONG,    // 香港进出中国大陆车牌
  LT_BAK2,                 // 留空2
  LT_COACH,                // 教练车牌
  LT_MACAO,                // 澳门进出中国大陆车牌
  LT_ARMPOL2,              // 双层武警车牌
  LT_ARMPOL_ZONGDUI,       // 武警总队车牌
  LT_ARMPOL2_ZONGDUI,      // 双层武警总队车牌
  LT_CA,                   // 民航车牌
  LT_NEW_ENERGY,           // 新能源车牌
  LI_ENERGY_LARGE,         // 新能源大型车 //20
  LT_EMERGENCY,            // 应急
  LT_CONSULATE,            // 领馆车牌
  LT_NEW_FUEL_CAR,         // 新标准小型燃油车
  LT_NEW_NE_CAR,           // 新标准小型新能源车
  LT_AIRPORT,              // 机场车牌
  LT_OVERSEA,              // 海外车牌(所有海外车牌类型) //26
  LT_BAK0,                 // 留空0
  LT_BAK1,                 // 留空1
  LT_FAKEPLATE,            // 假车牌
  LT_CAR_LOGO,             // 车标
  LT_NO_PLATE,             // 无牌车 //31
  LT_HK_LOCAL_SINGLE,      // 香港单层车牌
  LT_HK_LOCAL_DOUBLE,      // 香港双层车牌
  LT_MC_LOCAL_SINGLE,      // 澳门单层车牌
  LT_MC_LOCAL_DOUBLE,      // 澳门双层车牌
  LT_INDIVI_DIVIDING_LINE, // 个性化车牌分割线 //36
  LT_CHANGNEI,             // 场内/厂内
  LT_ZHATUCHE,             // 渣
  LT_DIAN8888,             // 电
  LT_TEMPORARY,            // 临时车牌
  LT_TRACTOR,              // 农用车牌
  LT_SELF_DEFINED,         // 自定义车牌
  LT_MAX_TYPE_NUM,         // 车牌类型最大数量
} PLATE_TYPE;

typedef enum {
  LC_UNKNOWN = 0,  // 未知
  LC_BLUE,         // 蓝色
  LC_YELLOW,       // 黄色
  LC_WHITE,        // 白色
  LC_BLACK,        // 黑色
  LC_GREEN,        // 绿色
  LC_YELLOW_GREEN, // 黄绿色,大型新能源颜色
  LC_RED,          // 红色
} PLATE_COLOR;

}

typedef enum  {
  PROVINCE_BEIJING    = 0,   // 京
  PROVINCE_TIANJIN    = 1,   // 津
  PROVINCE_HEBEI      = 2,   // 冀
  PROVINCE_SHANXI     = 3,   // 晋
  PROVINCE_NEIMENGGU  = 4,   // 蒙
  PROVINCE_LIAONING   = 5,   // 辽
  PROVINCE_JILIN      = 6,   // 吉
  PROVINCE_HEILONGJIANG = 7, // 黑
  PROVINCE_SHANGHAI   = 8,   // 沪
  PROVINCE_JIANGSU    = 9,   // 苏
  PROVINCE_ZHEJIANG   = 10,  // 浙
  PROVINCE_ANHUI      = 11,  // 皖
  PROVINCE_FUJIAN     = 12,  // 闽
  PROVINCE_JIANGXI    = 13,  // 赣
  PROVINCE_SHANDONG   = 14,  // 鲁
  PROVINCE_HENAN      = 15,  // 豫
  PROVINCE_HUBEI      = 16,  // 鄂
  PROVINCE_HUNAN      = 17,  // 湘
  PROVINCE_GUANGDONG  = 18,  // 粤
  PROVINCE_GUANGXI    = 19,  // 桂
  PROVINCE_HAINAN     = 20,  // 琼
  PROVINCE_CHONGQING  = 21,  // 渝
  PROVINCE_SICHUAN    = 22,  // 川
  PROVINCE_GUIZHOU    = 23,  // 贵
  PROVINCE_YUNNAN     = 24,  // 云
  PROVINCE_XIZANG     = 25,  // 藏
  PROVINCE_SHAANXI    = 26,  // 陕
  PROVINCE_GANSU      = 27,  // 甘
  PROVINCE_QINGHAI    = 28,  // 青
  PROVINCE_NINGXIA    = 29,  // 宁
  PROVINCE_XINJIANG   = 30,  // 新
  PROVINCE_HONGKONG   = 31,  // 港
  PROVINCE_MACAO      = 32,  // 澳
  PROVINCE_TAIWAN     = 33,  // 台
  PROVINCE_POLICE     = 34,  // 警
  PROVINCE_EMBASSY    = 35,  // 使
  PROVINCE_WUJING     = 36,  // WJ (武警)
  PROVINCE_DIPLOMAT   = 37,  // 领
  PROVINCE_SCHOOL     = 38,  // 学
  PROVINCE_NONE       = 255  // 无
} PROVINCE_TYPE;
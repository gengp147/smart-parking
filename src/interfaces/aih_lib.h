//
// AIH库接口定义
// 提供AIH算法的统一接口，支持检测、跟踪、识别、配置、事件等模块的组合
//

#ifndef AIH_LIB_H_
#define AIH_LIB_H_

#include "aih_type.h"

#ifdef __cplusplus
extern "C" {
#endif

// 初始化配置结构体
struct InitConfig {
    int32_t src_frame_width;              // 原始帧宽度
    int32_t src_frame_height;             // 原始帧高度
    int32_t scale_frame_width;              // 缩放帧宽度
    int32_t scale_frame_height;             // 缩放帧高度
    int32_t max_controls;        // 最大控件数量
    int32_t max_events;          // 最大事件数量
    int32_t cache_frame_count;   // 缓存帧数量
    int32_t video_fps;           // 视频帧率
    
};

// AIH库句柄类型
typedef void* AIH_Handle_t;

/**
 * @brief 创建AIH算法对象
 * @param config 初始化配置参数
 * @param handle 返回的AIH库句柄
 * @return 0表示成功，非0表示失败
 */
int AIH_Create(AIH_Handle_t* handle, const InitConfig& config);

/**
 * @brief 销毁AIH算法对象
 * @param handle AIH库句柄
 * @return 0表示成功，非0表示失败
 */
int AIH_Destroy(AIH_Handle_t handle);

#ifdef __cplusplus
}
#endif

// C++接口定义
#ifdef __cplusplus

class AIH_Lib {
public:
    
    /**
     * @brief 析构函数
     */
    virtual ~AIH_Lib() = default;

    /**
     * @brief 处理输入帧并返回结果
     * @param frame 输入帧数据
     * @param result 返回的处理结果
     * @return 0表示成功，非0表示失败
     */
    virtual int ProcessFrame(aih::ImageInfo frame[2], aih::EventInfos& result) = 0;

    /**
     * @brief 添加控件区域
     * @param control 控件信息
     * @return 0表示成功，非0表示失败
     */
    virtual int AddControl(const aih::ControlInfo& control) = 0;

    /**
     * @brief 删除控件区域
     * @param control_id 控件ID
     * @return 0表示成功，非0表示失败
     */
    virtual int RemoveControl(int control_id) = 0;

    /**
     * @brief 获取控件信息
     * @param control_id 控件ID
     * @param control 返回的控件信息
     * @return 0表示成功，非0表示失败
     */
    virtual int GetControl(int control_id, aih::ControlInfo& control) = 0;

    /**
     * @brief 添加事件配置
     * @param control_id 控件ID
     * @param event 事件配置
     * @return 0表示成功，非0表示失败
     */
    virtual int AddEvent(int control_id, const aih::SingleEventCfg& event) = 0;

    /**
     * @brief 删除事件配置
     * @param control_id 控件ID
     * @param event_id 事件ID
     * @return 0表示成功，非0表示失败
     */
    virtual int RemoveEvent(int control_id, int event_id) = 0;

    /**
     * @brief 获取事件配置
     * @param control_id 控件ID
     * @param event_id 事件ID
     * @param event 返回的事件配置
     * @return 0表示成功，非0表示失败
     */
    virtual int GetEvent(int control_id, int event_id, aih::SingleEventCfg& event) = 0;

    /**
     * @brief 获取当前所有缓存帧信息
     * @param frame_infos 返回的缓存帧信息数组
     * @param frame_count 返回的缓存帧数量
     * @return 0表示成功，非0表示失败
     */
    virtual int GetCachedFrames(std::vector<aih::CachedFrameInfo>& frame_infos) = 0;

    /**
     * @brief 获取算法版本信息
     * @param version 返回的版本字符串
     * @param max_len 版本字符串最大长度
     * @return 0表示成功，非0表示失败
     */
    virtual std::string GetVersion() = 0;


    /**
     * @brief 获取私有化数据
     * @param buffer 缓存区首地址(内存调用者开辟)
     * @param buffer_max_length 缓存区最大长度
     * @param read_length 有效读取的长度
     * @return 0表示成功，非0表示失败, -1表示buffer长度过小
     */
    virtual int
    GetPrivateData(void *buffer, aih::U32 buffer_max_length, aih::U32 *read_length, aih::U32 pu32_data_type) = 0;

    /**
     * @brief 使能/关闭私有化数据
     * @param u32_enable 0表示关闭，1表示使能
     * @return 0表示成功，非0表示失败
     */
    virtual int
    EnablePrivateData(aih::U32 u32_enable) = 0;

};


#endif // __cplusplus

#endif // AIH_LIB_H_ 
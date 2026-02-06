#ifndef ACL_DET_YOLO_ORDER_PROCESS_H
#define ACL_DET_YOLO_ORDER_PROCESS_H

#include "access_config.h"
#include "data_type.h"
#include "../illegal_parking/illegal_parking_algorithm.h"

namespace sonli
{

class CachedFramesManager;

class OrderProcess
{
public:
    explicit
    OrderProcess(const std::shared_ptr<CachedFramesManager> &cached_frames_manager);

    ~OrderProcess() = default;

    OrderProcess *
    ConvertAihOrder(const sonli::VehInOutInfoVec &original_order,
                    aih::TimeInfo time_info, aih::EventInfos &,
                    std::shared_ptr<sonli::ParkingInfoVec>);


    aih::EventInfos &
    GetDiscardOrders();

private:
    aih::EventInfos discard_orders_;
    std::shared_ptr<CachedFramesManager> cached_frames_manager_;
};

class LineMessageProcess
{
public:
    LineMessageProcess() = default;

    ~LineMessageProcess() = default;

    static void
    ConvertLineMessage(const std::vector<sonli::VehAccessInfo> &vehicles_, aih::EventInfos &);
};

class IllegalParkingMessageAIHTransfer
{
public:
    explicit
    IllegalParkingMessageAIHTransfer(std::shared_ptr<CachedFramesManager> cached_frames_manager);

    ~IllegalParkingMessageAIHTransfer() = default;

    /**
     * @brief 转换非法停车消息
     *
     * @param illegal_parking_result 非法停车检测结果
     * @param current_time_info 当前时间戳
     * @param event_infos 非法停车事件列表
     */
    void
    ConvertIllegalParkingMessage(const std::shared_ptr<sonli::illegal_parking::IllegalParkingDetectionResult> &,
                                 aih::TimeInfo current_time_info, aih::EventInfos &);

private:
    std::shared_ptr<CachedFramesManager> cached_frames_manager_;
};
}

#endif // ACL_DET_YOLO_ORDER_PROCESS_H
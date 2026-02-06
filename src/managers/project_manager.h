
#ifndef LIB_MANAGER_H
#define LIB_MANAGER_H
#include "cached_frames_manager.h"
#include "private_data_manager.h"
#include "aih_eventinfo_convert.h"
#include "device_manager.h"
#include "param_manager.h"
#include "vulcan/vulcan_sdk.h"
#include "json/value.h"
#include <atomic>
#include <string>
#include <../interfaces/aih_lib.h>

namespace sonli
{

class ProjectManager
{
public:
    ProjectManager(const InitConfig &config);
    ~ProjectManager();

    int
    LogInit(Json::Value &cfg_jv) const;

    int
    DeviceInit(Json::Value &cfg_jv) const;

    int
    WorkflowInit(Json::Value &cfg_jv) const;
    int
    Init();

    bool StatusCheck() const noexcept;

    std::shared_ptr<ParamManager>
    GetParamManager() const;
    std::shared_ptr<CachedFramesManager>
    GetCachedFramesManager() const;

    int
    InferenceFrame(aih::ImageInfo frame[2], aih::EventInfos &result);

    void
    SetConfig(const InitConfig &config);

private:
    InitConfig config;
    vulcan::VulcanSDK *stream_handle;
    std::atomic<bool> inited{false};
    std::shared_ptr<ParamManager> param_manager;
    std::shared_ptr<CachedFramesManager> cached_frames_manager;
    std::shared_ptr<PrivateDataManager> private_data_manager;

    std::unique_ptr<OrderProcess> inout_order_transfer;
    std::unique_ptr<LineMessageProcess> line_message_transfer;
    std::unique_ptr<IllegalParkingMessageAIHTransfer> illegal_parking_transfer;

public:
    PrivateDataManager*
    GetPrivateDataManager() const;
private:
    long frame_id = 0;

};

} // namespace sonli

#endif // LIB_MANAGER_H

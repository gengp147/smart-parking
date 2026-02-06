#include "../managers/project_manager.h"
#include "../utils/Singleton.h"
#include "../utils/sl_logger.h"
#include "../utils/struct_2_string.h"
#include "aih_lib.h"
#include "error.h"
#include "version.h"
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sys/time.h>



#if 1
#include "../utils/function_timer.h"
static int DEBUG_INFERENCE_BEGIN_TIME_STAMP_MS = 100;
static int DEBUG_INFERENCE_DONE_TIME_STAMP_MS = 1;
#define DEBUG_PRINT_TIMESTAMP_MS(prefix) print_timestamp_ms(prefix);
#define DEBUG_RECORD_INFERENCE_BEGIN_TIME_STAMP_MS DEBUG_INFERENCE_BEGIN_TIME_STAMP_MS = get_timestamp_ms();

#define DEBUG_RECORD_INFERENCE_DONE_TIME_STAMP_MS DEBUG_INFERENCE_DONE_TIME_STAMP_MS = get_timestamp_ms();

#define DEBUG_GET_INFERENCE_TIME_COST_MS                                                                               \
    if (DEBUG_INFERENCE_BEGIN_TIME_STAMP_MS < DEBUG_INFERENCE_DONE_TIME_STAMP_MS)                                      \
    {                                                                                                                  \
        auto cost = -(DEBUG_INFERENCE_BEGIN_TIME_STAMP_MS - DEBUG_INFERENCE_DONE_TIME_STAMP_MS);                        \
        std::cout << "[SL]:Time cost:" << cost << std::endl;    \
    }

#else
#define DEBUG_PRINT_TIMESTAMP_MS(prefix)
#define DEBUG_RECORD_INFERENCE_BEGIN_TIME_STAMP_MS
#define DEBUG_RECORD_INFERENCE_DONE_TIME_STAMP_MS
#define DEBUG_GET_INFERENCE_TIME_COST_MS
#endif

#define PROJECTMANAGERCHECK(project_manager)                                                                           \
    if (!project_manager || !project_manager->StatusCheck())                                                           \
    {                                                                                                                  \
        SonLiLogger::getInstance()                                                                                     \
            .getLogger(SonLiLogger::RotateLogger)                                                                      \
            ->error("project manager not inited, cant work.");                                                         \
        return ERROR_INVALID_ARGUMENTS;                                                                                \
    }

namespace sonli
{

std::ostream &
operator<<(std::ostream &os, const InitConfig &obj)
{
    return os << "src_frame_width: " << obj.src_frame_width << " src_frame_height: " << obj.src_frame_height
              << " scale_frame_width: " << obj.scale_frame_width << " scale_frame_height: " << obj.scale_frame_height
              << " max_controls: " << obj.max_controls << " max_events: " << obj.max_events
              << " cache_frame_count: " << obj.cache_frame_count << " video_fps: " << obj.video_fps;
}

class AIH_Lib_Impl final : public AIH_Lib
{
public:
    explicit AIH_Lib_Impl(const InitConfig &config_)
        : AIH_Lib(), project_manager(std::make_shared<ProjectManager>(config_)), config(config_)
    {
        std::cout << "[SL]:" << __func__ << ":AIH_Lib_Impl object created." << std::endl;
        std::cout << "[SL]:" << __func__ << ":config: " << config << std::endl;
    }

    ~AIH_Lib_Impl() override
    {
        project_manager.reset();
        std::cout << "[SL]:" << __func__ << ":AIH_Lib_Impl object destroyed." << std::endl;
    }

    int
    InitEverything() const
    {
        std::cout << "[SL]:" << __func__ << ":InitEverything." << std::endl;
        int return_code = SL_SUCCESS;
        if (project_manager)
        {
            return_code = project_manager->Init();
        }
        if (return_code != SL_SUCCESS)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->error("project manager init failed, error code %d", return_code);
        }
        else
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->info("project manager init InitEverything success");
        }
        std::cout << "[SL]:" << __func__ << ":InitEverything done." << std::endl;
        return return_code;
    }

    int
    ProcessFrame(aih::ImageInfo frame[2], aih::EventInfos &result) override
    {
        // DEBUG_PRINT_TIMESTAMP_MS("ProcessFrame Start");
        DEBUG_RECORD_INFERENCE_BEGIN_TIME_STAMP_MS
        PROJECTMANAGERCHECK(project_manager)
        try
        {
            if (frame[0].time_info_.pts_ != frame[1].time_info_.pts_)
            {
                SonLiLogger::getInstance()
                    .getLogger(SonLiLogger::RotateLogger)
                    ->error("frame image data pts not equal, pts1 {}, pts2 {}", frame[0].time_info_.pts_,
                            frame[1].time_info_.pts_);
                return ERROR_INVALID_ARGUMENTS;
            }

            if (frame[0].vir_[0] == 0)
            {
                SonLiLogger::getInstance()
                    .getLogger(SonLiLogger::RotateLogger)
                    ->error("first frame image data is null, pts {}", frame[0].time_info_.pts_);
                return ERROR_INVALID_ARGUMENTS;
            }
            if (frame[1].vir_[0] == 0)
            {
                SonLiLogger::getInstance()
                    .getLogger(SonLiLogger::RotateLogger)
                    ->error("second frame image data is null, pts {}", frame[1].time_info_.pts_);
                return ERROR_INVALID_ARGUMENTS;
            }

            result.clear();
            auto _ret = project_manager->InferenceFrame(frame, result);
            if (_ret != SL_SUCCESS)
            {
                SonLiLogger::getInstance()
                    .getLogger(SonLiLogger::RotateLogger)
                    ->error("inference frame failed, pts {}, error code %d", frame[0].time_info_.pts_, _ret);
                return ERR_RUNTIME_ERROR;
            }
        }
        catch (const std::exception &e)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->error("process frame failed, pts {}, error message {}", frame[0].time_info_.pts_, e.what());
            return ERR_RUNTIME_ERROR;
        }
        catch (...)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->error("process frame failed, pts {}, error message {}", frame[0].time_info_.pts_, "unknown error");
            return ERR_RUNTIME_ERROR;
        }
        // DEBUG_PRINT_TIMESTAMP_MS("ProcessFrame Done");
        DEBUG_RECORD_INFERENCE_DONE_TIME_STAMP_MS
        DEBUG_GET_INFERENCE_TIME_COST_MS
        return SL_SUCCESS;
    }

    int
    AddControl(const aih::ControlInfo &control) override
    {
        PROJECTMANAGERCHECK(project_manager)
        auto _id = control.id_;
        if (_id < 0)
        {
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("control id {} Invalid", _id);
            return ERROR_INVALID_ARGUMENTS;
        }

        auto _param_manager = project_manager->GetParamManager();
        if (_param_manager == nullptr)
        {
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("param manager is null");
            return ERROR_INVALID_ARGUMENTS;
        }

        auto _ret = _param_manager->GetControlManager()->RegisterControl(control);
        if (_ret != SL_SUCCESS)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->error("register control failed, control info {}, error code {}", _id, _ret);
            return ERROR_INVALID_ARGUMENTS;
        }

        // SonLiLogger::getInstance()
        //     .getLogger(SonLiLogger::RotateLogger)
        //     ->info("***************************************************************************");
        //
        // SonLiLogger::getInstance()
        //     .getLogger(SonLiLogger::RotateLogger)
        //     ->info("Controls: {}", _param_manager->GetControlManager()->TO_STRING());
        //
        // SonLiLogger::getInstance()
        //     .getLogger(SonLiLogger::RotateLogger)
        //     ->info("***************************************************************************");
        return SL_SUCCESS;
    }

    int
    RemoveControl(int control_id) override
    {
        PROJECTMANAGERCHECK(project_manager)
        auto _id = control_id;
        if (_id < 0)
        {
            SL_LOG_ROTATE_ERROR("control id {} Invalid", _id);
            return ERROR_INVALID_ARGUMENTS;
        }
        auto _param_manager = project_manager->GetParamManager();
        if (_param_manager == nullptr)
        {
            SL_LOG_ROTATE_ERROR("param manager is null");
            return ERROR_INVALID_ARGUMENTS;
        }

        auto _ret = _param_manager->GetControlManager()->UnregisterControl(_id);
        if (_ret != SL_SUCCESS)
        {
            // SonLiLogger::getInstance()
            //     .getLogger(SonLiLogger::RotateLogger)
            //     ->error("unregister control failed, control id %d, error code %d", _id, _ret);
            return ERROR_INVALID_ARGUMENTS;
        }
        // SonLiLogger::getInstance()
        //     .getLogger(SonLiLogger::RotateLogger)
        //     ->info("***************************************************************************");
        //
        // SonLiLogger::getInstance()
        //     .getLogger(SonLiLogger::RotateLogger)
        //     ->info("Controls: {}", _param_manager->GetControlManager()->TO_STRING());
        //
        // SonLiLogger::getInstance()
        //     .getLogger(SonLiLogger::RotateLogger)
        //     ->info("***************************************************************************");
        _param_manager->SetConfigUpdated(true);
        return SL_SUCCESS;
    }

    int
    GetControl(int control_id, aih::ControlInfo &control) override
    {
        PROJECTMANAGERCHECK(project_manager)
        if (control_id < 0)
        {
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("control id {} Invalid", control_id);
            return ERROR_INVALID_ARGUMENTS;
        }
        auto _param_manager = project_manager->GetParamManager();
        if (_param_manager == nullptr)
        {
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("param manager is null");
            return ERROR_INVALID_ARGUMENTS;
        }
        auto _control_config_info = _param_manager->GetControlManager()->GetControlConfigInfo(control_id);
        if (_control_config_info == nullptr)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->error("get control config info failed, control id %d", control_id);
            return ERROR_INVALID_ARGUMENTS;
        }
        auto _control_info_desc = _control_config_info->GetControlInfoDesc();
        if (_control_info_desc == nullptr)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->error("get control info desc failed, control id %d", control_id);
            return ERROR_INVALID_ARGUMENTS;
        }
        control = *_control_info_desc;
        SonLiLogger::getInstance()
            .getLogger(SonLiLogger::RotateLogger)
            ->info("get control info success, control id %d, control info %s", control_id, TO_STRING(control).c_str());

        // SonLiLogger::getInstance()
        //     .getLogger(SonLiLogger::RotateLogger)
        //     ->info("***************************************************************************");
        //
        // SonLiLogger::getInstance()
        //     .getLogger(SonLiLogger::RotateLogger)
        //     ->info("Controls: {}", _param_manager->GetControlManager()->TO_STRING());
        //
        // SonLiLogger::getInstance()
        //     .getLogger(SonLiLogger::RotateLogger)
        //     ->info("***************************************************************************");
        return SL_SUCCESS;
    }

    int
    AddEvent(int control_id, const aih::SingleEventCfg &event) override
    {
        PROJECTMANAGERCHECK(project_manager)
        auto _id = control_id;
        if (_id < 0)
        {
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("control id {} Invalid", _id);
            return ERROR_INVALID_ARGUMENTS;
        }
        auto _param_manager = project_manager->GetParamManager();
        if (_param_manager == nullptr)
        {
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("param manager is null");
            return ERROR_INVALID_ARGUMENTS;
        }
        auto _control_config_info = _param_manager->GetControlManager()->GetControlConfigInfo(_id);
        if (_control_config_info == nullptr)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->error("get control config info failed, control id {}", _id);
            return ERROR_INVALID_ARGUMENTS;
        }
        auto _event_info_desc = _control_config_info->GetEventConfigList();
        if (_event_info_desc == nullptr)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->error("get event info desc failed, control id {}", _id);
            return ERROR_INVALID_ARGUMENTS;
        }
        auto _ret = _event_info_desc->RegisterSingleEvent(event);
        if (_ret != SL_SUCCESS)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->error("register event failed, control id {}, error code {}", _id, _ret);
            return ERROR_INVALID_ARGUMENTS;
        }
        int ret = SL_SUCCESS;
        switch (_control_config_info->GetControlType())
        {
            case aih::CONTROL_TYPE_BERTH:
                if (event.event_type_ == aih::EVENT_TYPE_ENTER_BERTH)
                {
                    ret = _param_manager->GetControlManager()->UpdatePriorCityConfig(event.cfg_info_.berth_cfg_);
                }

                break;
            case aih::CONTROL_TYPE_VIOLATION:
                if (event.event_type_ == aih::EVENT_TYPE_ENTER_VIOLATION)
                {
                    ret = _param_manager->GetControlManager()->UpdatePriorCityConfig(event.cfg_info_.violation_cfg_);
                }
            default:
                break;
        }
        if (ret != SL_SUCCESS)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->warn("UpdatePriorCityConfig: update control config failed, control id {}, error code {}", _id, ret);
        }

        // SonLiLogger::getInstance()
        //     .getLogger(SonLiLogger::RotateLogger)
        //     ->info("***************************************************************************");
        //
        // SonLiLogger::getInstance()
        //     .getLogger(SonLiLogger::RotateLogger)
        //     ->info("Control infos: {}", _param_manager->GetControlManager()->TO_STRING());
        //
        // SonLiLogger::getInstance()
        //     .getLogger(SonLiLogger::RotateLogger)
        //     ->info("***************************************************************************");
        _param_manager->SetConfigUpdated(true);
        return SL_SUCCESS;
    }

    int
    RemoveEvent(int control_id, int event_id) override
    {
        PROJECTMANAGERCHECK(project_manager)
        auto _id = control_id;
        if (_id < 0)
        {
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("control id %d Invalid", _id);
            return ERROR_INVALID_ARGUMENTS;
        }
        if (event_id < 0)
        {
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("event id %d Invalid", event_id);
            return ERROR_INVALID_ARGUMENTS;
        }

        auto _param_manager = project_manager->GetParamManager();
        if (_param_manager == nullptr)
        {
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("param manager is null");
            return ERROR_INVALID_ARGUMENTS;
        }
        auto _control_config_info = _param_manager->GetControlManager()->GetControlConfigInfo(_id);
        if (_control_config_info == nullptr)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->error("get control config info failed, control id %d", _id);
            return ERROR_INVALID_ARGUMENTS;
        }
        auto _event_info_desc = _control_config_info->GetEventConfigList();
        if (_event_info_desc == nullptr)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->error("get event info desc failed, control id %d", _id);
            return ERROR_INVALID_ARGUMENTS;
        }
        _event_info_desc->UnregisterSingleEvent(event_id);
        SonLiLogger::getInstance()
            .getLogger(SonLiLogger::RotateLogger)
            ->info("remove event success, control id %d, event id %d", _id, event_id);

        // SonLiLogger::getInstance()
        //     .getLogger(SonLiLogger::RotateLogger)
        //     ->info("***************************************************************************");
        //
        // SonLiLogger::getInstance()
        //     .getLogger(SonLiLogger::RotateLogger)
        //     ->info("Control infos: {}", _param_manager->GetControlManager()->TO_STRING());
        //
        // SonLiLogger::getInstance()
        //     .getLogger(SonLiLogger::RotateLogger)
        //     ->info("***************************************************************************");
        _param_manager->SetConfigUpdated(true);
        return SL_SUCCESS;
    }

    int
    GetEvent(int control_id, int event_id, aih::SingleEventCfg &event) override
    {
        PROJECTMANAGERCHECK(project_manager)
        if (control_id < 0)
        {
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("control id %d Invalid", control_id);
            return ERROR_INVALID_ARGUMENTS;
        }
        if (event_id < 0)
        {
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("event id %d Invalid", event_id);
            return ERROR_INVALID_ARGUMENTS;
        }
        auto _param_manager = project_manager->GetParamManager();
        if (_param_manager == nullptr)
        {
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("param manager is null");
            return ERROR_INVALID_ARGUMENTS;
        }
        auto _control_config_info = _param_manager->GetControlManager()->GetControlConfigInfo(control_id);
        if (_control_config_info == nullptr)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->error("get control config info failed, control id %d", control_id);
            return ERROR_INVALID_ARGUMENTS;
        }
        auto _event_info_desc = _control_config_info->GetEventConfigList();
        if (_event_info_desc == nullptr)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->error("get event info desc failed, control id %d", control_id);
            return ERROR_INVALID_ARGUMENTS;
        }
        auto _ret = _event_info_desc->GetSingleEventDesc(event_id, event);
        if (_ret != SL_SUCCESS)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->error("get event failed, control id %d, event id %d, error code %d", control_id, event_id, _ret);
            return ERROR_INVALID_ARGUMENTS;
        }
        SonLiLogger::getInstance()
            .getLogger(SonLiLogger::RotateLogger)
            ->info("get event success, control id %d, event id %d, event %s", control_id, event_id,
                   TO_STRING(event).c_str());

        // SonLiLogger::getInstance()
        //     .getLogger(SonLiLogger::RotateLogger)
        //     ->info("***************************************************************************");
        //
        // SonLiLogger::getInstance()
        //     .getLogger(SonLiLogger::RotateLogger)
        //     ->info("Controls: {}", _param_manager->GetControlManager()->TO_STRING());
        //
        // SonLiLogger::getInstance()
        //     .getLogger(SonLiLogger::RotateLogger)
        //     ->info("***************************************************************************");
        return SL_SUCCESS;
    }

    int
    GetCachedFrames(std::vector<aih::CachedFrameInfo> &frame_infos) override
    {
        DEBUG_PRINT_TIMESTAMP_MS("GetCachedFrames START");
        PROJECTMANAGERCHECK(project_manager)
        auto _cached_frames_manager = project_manager->GetCachedFramesManager()->GetAllCachedFrames();
        frame_infos.clear();
        frame_infos.reserve(_cached_frames_manager.size());
        for (const auto &i : _cached_frames_manager)
        {
            aih::CachedFrameInfo info;
            info.tm_info_ = i;
            frame_infos.push_back(info);
        }
        DEBUG_PRINT_TIMESTAMP_MS("GetCachedFrames Done");
        return SL_SUCCESS;
    }

    std::string
    GetVersion() override
    {
        std::cout << "[SL]:" << __func__ << ":" << COMLETE_VERSION << std::endl;
        std::string version_sdk(COMLETE_VERSION);
        return version_sdk;
    }

    int
    GetPrivateData(void *buffer, aih::U32 buffer_max_length, aih::U32 *read_length, aih::U32 pu32_data_type) override
    {
        PROJECTMANAGERCHECK(project_manager)
        if (project_manager->GetPrivateDataManager()->Enabled1())
        {
            auto code_ptr = project_manager->GetPrivateDataManager()->LoadTraceData();
            if (buffer_max_length < code_ptr->data.size() + 1)
            {
                SL_LOG_ROTATE_ERROR("[sl_spd_sdk]<GetPrivateData>: buffer too small, need {}, got {}",
                                    code_ptr->data.size() + 1, buffer_max_length);
                *read_length = code_ptr->data.size() + 1;
                return ERR_RUNTIME_ERROR;
            }
            memcpy(buffer, code_ptr->data.data(), code_ptr->data.size());
            static_cast<char *>(buffer)[code_ptr->data.size()] = '\0';
            *read_length = code_ptr->data.size();
            // std::cout << "[SL]:<GetPrivateData>: Private Data pop:" << *read_length << " bytes" << std::endl;
            return SL_SUCCESS;
        }
        // std::cout << "[SL]:<GetPrivateData>: Private Data not enabled but try get." << std::endl;
        return SL_SUCCESS;
    }

    int
    EnablePrivateData(aih::U32 u32_enable) override
    {
        PROJECTMANAGERCHECK(project_manager)
        if (project_manager && project_manager->StatusCheck())
        {
           SL_LOG_ROTATE_INFO("EnablePrivateData, u32_enable {}", u32_enable);
        }

        std::cout << "[SL]:<EnablePrivateData>:u32_enable " << u32_enable << std::endl;

        project_manager->GetPrivateDataManager()->SetEnabled(u32_enable != 0);
        return SL_SUCCESS;
    }

private:
    std::shared_ptr<ProjectManager> project_manager;
    InitConfig config{};
};

} // namespace sonli
__attribute__((visibility("default"))) int
AIH_Create(AIH_Handle_t *handle, const InitConfig &config)
{
    auto _sl_spd_sdk = new sonli::AIH_Lib_Impl(config);
    if (_sl_spd_sdk->InitEverything() != SL_SUCCESS)
    {
        delete _sl_spd_sdk;
        return ERR_INIT_FAIL;
    }
    *handle = _sl_spd_sdk;
    return SL_SUCCESS;
}

__attribute__((visibility("default"))) int
AIH_Destroy(AIH_Handle_t handle)
{
    if (handle != nullptr)
    {
        delete static_cast<sonli::AIH_Lib_Impl *>(handle);
        handle = nullptr;
    }
    return SL_SUCCESS;
}
#include "project_manager.h"
#include "../include/common_define.h"
#include "../include/error.h"
#include "../interfaces/aih_extern.h"
#include "../utils/Singleton.h"
#include "../utils/file_utils.h"
#include "../utils/sl_logger.h"
#include "../utils/struct_2_string.h"
#include "../workflow/workflow.h"
#include "config_field.h"
#include "project_manager.h"

#include "frame_inage_data.h"

#include "../utils/time_utils.h"
#include "json/reader.h"
#include "json/value.h"
using namespace std;
using namespace sonli;

ProjectManager::ProjectManager(const InitConfig &config_arg)
    : config(config_arg),
      cached_frames_manager(std::make_shared<CachedFramesManager>(config_arg.cache_frame_count)),
      private_data_manager(new PrivateDataManager()),
      inout_order_transfer(new OrderProcess(cached_frames_manager)),
      line_message_transfer(new LineMessageProcess()),
      illegal_parking_transfer(new IllegalParkingMessageAIHTransfer(cached_frames_manager))

{
    this->stream_handle = vulcan::VulcanSDK::CreateSDK();
    this->param_manager = std::make_shared<ParamManager>();
    this->param_manager->GetControlManager()->SetStreamHandle(stream_handle);
}

ProjectManager::~ProjectManager()
{
    if (this->stream_handle != nullptr)
    {
        vulcan::VulcanSDK::DestroySDK(this->stream_handle);
        this->stream_handle = nullptr;
    }
    auto ret = Singleton<DeviceManager>::GetInstance().DestroyDevice();
    if (ret != SL_SUCCESS)
    {
        SL_LOG_ROTATE_ERROR("destroy device failed!");
    }
}

int
ProjectManager::LogInit(Json::Value &cfg_jv) const
{
    if (!(cfg_jv.isMember(SONLI_OBJ_LOG_SETTING) && cfg_jv[SONLI_OBJ_LOG_SETTING].isObject()))
    {
        std::cerr << "[SL] LogInit: error, config file not set logging!" << std::endl;
        return ERR_CONFIG_ILLEGAL;
    }

    const auto &logging_jv = cfg_jv[SONLI_OBJ_LOG_SETTING];
    std::string logging;
    if (logging_jv.isMember(SONLI_ELM_REDIRECT))
    {
        logging = logging_jv[SONLI_ELM_REDIRECT].asString();
    }
    else
    {
        logging = logging_jv.toStyledString();
    }

    int ret = SonLiLogger::getInstance().init(logging);

    if (ret != SL_SUCCESS)
    {
        std::cerr << "[SL] LogInit: error, init logger failed, " << std::to_string(ret) << std::endl;
        return ERR_INIT_FAIL;
    }
    return SL_SUCCESS;
}

int
ProjectManager::DeviceInit(Json::Value &cfg_jv) const
{
    if (!(cfg_jv.isMember(SONLI_DEVICE_CONFIG_PATH) && cfg_jv[SONLI_DEVICE_CONFIG_PATH].isString()))
    {
        SL_LOG_ROTATE_ERROR("config file not set device config!");
        return ERR_CONFIG_ILLEGAL;
    }
    std::string device_config = cfg_jv[SONLI_DEVICE_CONFIG_PATH].asString();

    auto ret = Singleton<DeviceManager>::GetInstance().InitDevice(device_config);
    if (ret != SL_SUCCESS)
    {
        SL_LOG_ROTATE_ERROR("init device failed!");
        return ERR_INIT_FAIL;
    }
    return SL_SUCCESS;
}

int
ProjectManager::WorkflowInit(Json::Value &cfg_jv) const
{
    if (!(cfg_jv.isMember(SONLI_OBJ_STREAMS_SETTING) && cfg_jv[SONLI_OBJ_STREAMS_SETTING].isObject()))
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("config file not set steam config!");
        return ERR_CONFIG_ILLEGAL;
    }
    Json::Value tmp_strem_cfg;
    std::string stream_cfg;
    const auto &stream_cfg_jv = cfg_jv[SONLI_OBJ_STREAMS_SETTING];
    if (stream_cfg_jv.isMember(SONLI_ELM_REDIRECT))
    {
        stream_cfg = stream_cfg_jv[SONLI_ELM_REDIRECT].asString();

        // load config
        std::ifstream infile(stream_cfg);
        if (!infile.good())
        {
            SL_LOG_ROTATE_ERROR("open config <{}> failed !", stream_cfg);
            return ERR_FILE_NOT_EXIST;
        }

        infile >> tmp_strem_cfg;
        infile.close();

        if (stream_handle->SetConfig("config_file", stream_cfg) != SL_SUCCESS)
        {
            SL_LOG_ROTATE_ERROR("stream_handle set config failed!");
            return ERR_CONFIG_ILLEGAL;
        }
    }
    else
    {
        tmp_strem_cfg = stream_cfg_jv;
        stream_cfg = stream_cfg_jv.toStyledString();
        if (stream_handle->SetConfig("config_string", stream_cfg) != SL_SUCCESS)
        {
            SL_LOG_ROTATE_ERROR("stream_handle set config failed!");
            return ERR_CONFIG_ILLEGAL;
        }
    }

    auto ret = stream_handle->Init(std::make_shared<workflow>());
    if (ret != SL_SUCCESS)
    {
        SL_LOG_ROTATE_ERROR("stream_handle init failed!");
        return ret;
    }
    return SL_SUCCESS;
}

int
ProjectManager::Init()
{
    if (this->inited)
    {
        SL_LOG_ROTATE_ERROR("Init: app is inited twice.")
        std::cout << "[SL] Init: app is inited twice." << std::endl;
        return ERR_INIT_TWICE;
    }

    if (!isExistFile(SL_CONFIG_COMMON_PATH))
    {
        SL_LOG_ROTATE_ERROR("init: error, file not exist at {}", SL_CONFIG_COMMON_PATH)
        std::cout << "[SL] Init: error, file not exist at " << SL_CONFIG_COMMON_PATH << std::endl;
        return ERR_INIT_FAIL;
    }

    std::ifstream infile(SL_CONFIG_COMMON_PATH);
    if (!infile.good())
    {
        infile.close();
        SL_LOG_ROTATE_ERROR("init: config file open failed!");
        std::cout << "[SL] Init: error, config file open failed!" << std::endl;
        return ERR_INIT_FAIL;
    }

    Json::Value cfg_jv;
    infile >> cfg_jv;
    infile.close();
    if (cfg_jv.isNull())
    {
        SL_LOG_ROTATE_ERROR("config file is null or not json!");
        std::cout << "[SL] Init: error, config file is null or not json!" << std::endl;
        cfg_jv.clear();
        return ERR_CONFIG_ILLEGAL;
    }
    SL_LOG_ROTATE_INFO("Project start init!");
    std::cout << "[SL] Init: Project start init!" << std::endl;
    auto ret = LogInit(cfg_jv);
    if (ret != SL_SUCCESS)
    {
        SL_LOG_ROTATE_ERROR("Log init failed!, {}", ret);
        return ret;
    }
    std::cout << "[SL] Init: Log init success!" << std::endl;

    if (DeviceInit(cfg_jv) != SL_SUCCESS)
    {
        SL_LOG_ROTATE_ERROR("Device init failed!, {}", ret);
        return ERR_CONFIG_ILLEGAL;
    }
    SL_LOG_ROTATE_INFO("Device init success!");
    std::cout << "[SL] Init: Device init success!" << std::endl;

    if (WorkflowInit(cfg_jv) != SL_SUCCESS)
    {
        SL_LOG_ROTATE_ERROR("Workflow init failed!, {}", ret);
        return ERR_CONFIG_ILLEGAL;
    }
    SL_LOG_ROTATE_INFO("Workflow init success!");
    std::cout << "[SL] Init: Workflow init success!" << std::endl;
    this->inited = true;
    std::cout << "[SL] Init: Project init success!" << std::endl;
    return SL_SUCCESS;
}

bool
ProjectManager::StatusCheck() const noexcept
{
    return inited;
}

std::shared_ptr<ParamManager>
ProjectManager::GetParamManager() const
{
    return this->param_manager;
}

std::shared_ptr<CachedFramesManager>
ProjectManager::GetCachedFramesManager() const
{
    return cached_frames_manager;
}

static void
SaveIntervalFrame(const aih::TimeInfo &time_info, const std::shared_ptr<CachedFramesManager> &cached_frames_manager,
                  const std::shared_ptr<illegal_parking::NoParkingAreaConfigVec> &noParkingAreaConfigs,
                  const std::shared_ptr<sonli::IntervalFrameList> &interval_frame_list)
{
    noParkingAreaConfigs->this_frame_check_ = false;
    if (cached_frames_manager->GetIntervalCachedFrames().isEmpty())
    {
        cached_frames_manager->GetIntervalCachedFrames().enqueue(time_info);
        noParkingAreaConfigs->this_frame_check_ = true;
        interval_frame_list->datas.emplace_back(time_info);
        return;
    }
    auto l = cached_frames_manager->GetIntervalCachedFrames().readLastNElements(1).front();
    uint64_t interval_ms = (time_info.local_time_ > l.local_time_) ? (time_info.local_time_ - l.local_time_)
                                                                   : (l.local_time_ - time_info.local_time_);
    if (interval_ms > 30*1000*1000)
    {
        cached_frames_manager->GetIntervalCachedFrames().enqueue(time_info);
        noParkingAreaConfigs->this_frame_check_ = true;
    }
    auto saved_frame_list = cached_frames_manager->GetIntervalCachedFrames().readLastNElements(-1);
    for (const auto &t : saved_frame_list)
    {
        interval_frame_list->datas.emplace_back(t);
    }
}

static void
AssembleFrame(aih::ImageInfo *source, std::shared_ptr<FrameImageData> &frameData)
{
    if (!frameData)
    {
        frameData = std::make_shared<FrameImageData>();
    }
    frameData->width = source->width_;
    frameData->height = source->height_;
    frameData->phy_addr[0] = source->phy_[0];
    frameData->phy_addr[1] = source->phy_[1];
    frameData->phy_addr[2] = source->phy_[2];
    frameData->stride[0] = source->stride_[0];
    frameData->stride[1] = source->stride_[1];
    frameData->stride[2] = source->stride_[2];
    frameData->phy_addr[0] = source->phy_[0];
    frameData->phy_addr[1] = source->phy_[1];
    frameData->phy_addr[2] = source->phy_[2];
    frameData->vir_[0] = source->vir_[0];
    frameData->vir_[1] = source->vir_[1];
    frameData->vir_[2] = source->vir_[2];
    frameData->time_info_ = source->time_info_;
}

int
ProjectManager::InferenceFrame(aih::ImageInfo frame[2], aih::EventInfos &result)
{
    auto frameData = std::make_shared<FrameImageDataExt>();
    frameData->time_info = frame[0].time_info_;
    frameData->frame_id = (++frame_id) % (1024 * 1024 * 1024);
    // std::cout << "frame index: " << frameData->frame_id << std::endl;
    AssembleFrame(&frame[0], frameData->_original_frame);
    AssembleFrame(&frame[1], frameData->_thumbnail);
    // parking data
    auto parking_info_ptr = param_manager->GetControlManager()->GenerateParkingList(frameData->time_info);
    auto access_config = param_manager->GetControlManager()->GenerateAccessConfig();
    auto illegal_config = param_manager->GetControlManager()->GenerateIllegalConfigVec(frameData->time_info);

    auto &time_info = frame[0].time_info_;
    auto interval_frame_list = std::make_shared<sonli::IntervalFrameList>();
    SaveIntervalFrame(time_info, cached_frames_manager, illegal_config, interval_frame_list);

    vulcan::InputDataPtr inputs = std::make_shared<vulcan::InputData>();
    inputs->datas_.emplace_back(std::static_pointer_cast<vulcan::BaseData>(frameData));
    inputs->datas_.emplace_back(std::static_pointer_cast<vulcan::BaseData>(parking_info_ptr));
    inputs->datas_.emplace_back(std::static_pointer_cast<vulcan::BaseData>(access_config));
    inputs->datas_.emplace_back(std::static_pointer_cast<vulcan::BaseData>(illegal_config));
    inputs->datas_.emplace_back(std::static_pointer_cast<vulcan::BaseData>(interval_frame_list));
    inputs->datas_.emplace_back(std::static_pointer_cast<vulcan::BaseData>(private_data_manager));
    if (param_manager->ConfigUpdated())
    {
        SL_LOG_ROTATE_INFO("[ProjectManager]: {}", TO_STRING(frame[0].time_info_.pts_));
        SL_LOG_ROTATE_INFO("[ProjectManager]<access>: {}", TO_STRING(*access_config));
        for (const auto &config : illegal_config->no_parking_area_configs_)
        {
            SL_LOG_ROTATE_INFO("[ProjectManager]<illegal>: {}", TO_STRING(*config));
        }
        if (illegal_config->no_parking_area_configs_.empty())
        {
            SL_LOG_ROTATE_INFO("[ProjectManager]<illegal>: no settings.");
        }
        SL_LOG_ROTATE_INFO("[ProjectManager]<parking>:{}", TO_STRING(*parking_info_ptr));
        SL_LOG_ROTATE_INFO("[ProjectManager]<private data> enable:{}", private_data_manager->Enabled1());
        param_manager->SetConfigUpdated(false);
    }
    auto outputs = stream_handle->SyncPredict2(inputs);
    auto inout_output = std::static_pointer_cast<sonli::DataProcOutputs>(outputs[0]->datas_.at(0));

    auto access_info = std::static_pointer_cast<sonli::AccCollectResult>(outputs[0]->datas_.at(1));
    auto IllegalParkingDetectionResult =
        std::static_pointer_cast<illegal_parking::IllegalParkingDetectionResult>(outputs[0]->datas_.at(2));
    if (IllegalParkingDetectionResult)
    {
        illegal_parking_transfer->ConvertIllegalParkingMessage(IllegalParkingDetectionResult, time_info, result);
    }

    cached_frames_manager->UpdateEviPic(inout_output->frame_infos,
                                        IllegalParkingDetectionResult->illegal_parking_time_set);

    if (access_info && access_info->keep_frame && !access_info->vehicles_.empty())
    {
        LineMessageProcess::ConvertLineMessage(access_info->vehicles_, result);
        cached_frames_manager->JustAddTimeInfo(time_info);
    }

    auto &discardOrder =
        inout_order_transfer
            ->ConvertAihOrder(inout_output->veh_inout_info_vec, frame[0].time_info_, result, parking_info_ptr)
            ->GetDiscardOrders();

    ///////////////////////////////////////////////////////PRINT//////////////////////////////////////////////////////////
    if (!result.empty())
    {
        for (auto &pair : result)
        {
            if (pair.event_type_ == aih::EVENT_TYPE_BERTH_REALTIME)
            {
                continue;
            }
            SL_LOG_ROTATE_INFO("DetectionResult [ {} ]", TO_STRING(pair));
        }
        // auto caches = cached_frames_manager->GetAllCachedFrames();
        // std::stringstream ss;
        // ss << "(";
        // for (auto &pair : caches)
        // {
        //     ss << pair.pts_ << ", ";
        // }
        // ss << ")";
        // SL_LOG_ROTATE_INFO("------------InferenceFrame ---- CachedFrames: {}", ss.str());
    }

    private_data_manager->StoreTraceData(std::static_pointer_cast<PrivateOutputData>(outputs[0]->datas_.at(3)));
    return SL_SUCCESS;
}

void
ProjectManager::SetConfig(const InitConfig &config)
{
    this->config = config;
}

PrivateDataManager *
ProjectManager::GetPrivateDataManager() const
{
    return private_data_manager.get();
}
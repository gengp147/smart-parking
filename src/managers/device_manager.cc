#include "device_manager.h"
#include "../utils/sl_logger.h"
#include "error.h"
using namespace std;
using namespace sonli;

DeviceManager::DeviceManager()
{
    init_status = false;
}
DeviceManager::~DeviceManager()
{
    if (GetInitStatus())
    {
        DestroyDevice();
    }
}

int
DeviceManager::InitDevice(const std::string &config_file_path_arg)
{
    if (GetInitStatus())
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("device already inited");
        return ERR_INITED;
    }
    this->config_file_path_ = config_file_path_arg;
    const char *aclConfigPath = this->config_file_path_.data();

    SonLiLogger::getInstance()
        .getLogger(SonLiLogger::RotateLogger)
        ->info("device acl config file: {}", config_file_path_);

    svp_acl_error ret(SVP_ACL_SUCCESS);
    if (config_file_path_.empty())
    {
        // ret = svp_acl_init(nullptr);
    }
    else
    {
        ret = svp_acl_init(aclConfigPath);
    }
    if (ret != SVP_ACL_SUCCESS)
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("acl init failed");
        return ERR_INITED;
    }
    else
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info("acl init success");
    }

    ret = svp_acl_rt_set_device(deviceId_);
    if (ret != SVP_ACL_SUCCESS)
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("acl open device {} failed", deviceId_);
        return ERR_INITED;
    }
    else
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info("acl open device {} success", deviceId_);
    }

    // set no timeout
    ret = svp_acl_rt_set_op_wait_timeout(0);
    if (ret != SVP_ACL_SUCCESS)
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("acl set op wait time failed");
        return ERR_INITED;
    }
    SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info("acl set op wait time success");
    init_status = true;
    return SL_SUCCESS;
}

int
DeviceManager::DestroyDevice()
{
    svp_acl_error ret;

    ret = svp_acl_rt_reset_device(deviceId_);
    if (ret != SVP_ACL_SUCCESS)
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("reset device failed");
    }

    ret = svp_acl_finalize();
    if (ret != SVP_ACL_SUCCESS)
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("finalize acl failed");
    }
    SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info("finalize acl success");
    init_status = false;
    return SL_SUCCESS;
}

bool
DeviceManager::GetInitStatus() const noexcept
{
    return init_status;
}

SubContext::SubContext(DeviceManager *device_manager) : device_manager_(device_manager) {}

SubContext::~SubContext()
{
    if (init)
    {
        DestroyContext();
        init = false;
    }
}

int
SubContext::InitContext()
{
    svp_acl_error ret = svp_acl_rt_set_device(deviceId_);
    if (ret != SVP_ACL_SUCCESS)
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("acl set device failed");
        return ERR_INITED;
    }

    ret = svp_acl_rt_create_context(&context_, deviceId_);
    if (ret != SVP_ACL_SUCCESS)
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("acl create context failed");
        goto UNINIT_RESET_DEVICE;
    }
    SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info("acl create context success");

    // create stream
    ret = svp_acl_rt_create_stream(&stream_);
    if (ret != SVP_ACL_SUCCESS)
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("acl create stream failed");
        goto UNINIT_CONTEXT;
    }
    SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info("acl create stream success");
    init = true;
    return SL_SUCCESS;

UNINIT_CONTEXT:
    ret = svp_acl_rt_destroy_context(context_);
    if (ret != SVP_ACL_SUCCESS)
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("destroy context failed!, ret {}", ret);
    }

UNINIT_RESET_DEVICE:
    ret = svp_acl_rt_reset_device(deviceId_);
    if (ret != SVP_ACL_SUCCESS)
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("reset device failed");
    }

    return ERR_INITED;
}

int
SubContext::DestroyContext()
{
    if (!init)
    {
        return SL_SUCCESS;
    }
    SetCurrentContext();
    svp_acl_error ret;
    SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info("SubContext DestroyContext");
    if (stream_ != nullptr)
    {
        ret = svp_acl_rt_destroy_stream(stream_);
        if (ret != SVP_ACL_SUCCESS)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->error("destroy stream failed!, ret {}", ret);
        }
        stream_ = nullptr;
    }

    if (context_ != nullptr)
    {
        ret = svp_acl_rt_destroy_context(context_);
        if (ret != SVP_ACL_SUCCESS)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->error("destroy context failed!, ret {}", ret);
        }
        context_ = nullptr;
    }
    ret = svp_acl_rt_reset_device(deviceId_);
    if (ret != SVP_ACL_SUCCESS)
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("reset device failed");
    }
    init = false;
    SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info("SubContext DestroyContext over.");
    return SL_SUCCESS;
}

void
SubContext::SetCurrentContext()
{
    auto ret = svp_acl_rt_set_current_context(context_);
    if (ret != SVP_ACL_SUCCESS)
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("set current context failed");
    }
}

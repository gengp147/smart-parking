#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include "acl/svp_acl.h"

#include <string>

class DeviceManager
{
public:
    DeviceManager();
    ~DeviceManager();
    int
    InitDevice(const std::string &config_file_path);
    int
    DestroyDevice();

    int
    DeviceID() const
    {
        return deviceId_;
    }

    bool
    GetInitStatus() const noexcept;

private:
    int deviceId_ = 0;
    svp_acl_rt_context context_{nullptr};
    svp_acl_rt_stream stream_{nullptr};
    std::string config_file_path_;
    bool init_status = false;
};

class SubContext
{
public:
    explicit SubContext(DeviceManager *device_manager);
    ~SubContext();
    int
    InitContext();
    int
    DestroyContext();
    bool
    IsInit() const noexcept
    {
        return init;
    }

    void SetCurrentContext();

private:
    int deviceId_ = 0;
    svp_acl_rt_context context_{nullptr};
    svp_acl_rt_stream stream_{nullptr};
    DeviceManager *device_manager_;
    bool init = false;
};

#endif // DEVICE_MANAGER_H

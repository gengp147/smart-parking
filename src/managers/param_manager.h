#ifndef PARAM_MANAGER_H
#define PARAM_MANAGER_H

#include "../interfaces/aih_type.h"
#include "data_type.h"
#include "parking_config.h"

namespace sonli
{
namespace illegal_parking
{
struct NoParkingAreaConfig;
struct NoParkingAreaConfigVec;
}
class SingleEventConfigInfo
{
public:
    SingleEventConfigInfo();
    explicit SingleEventConfigInfo(const aih::SingleEventCfg &event_desc);
    ~SingleEventConfigInfo();

    int
    RegisterSingleEvent(const aih::SingleEventCfg &event_desc);

    void
    UnregisterSingleEvent(int event_id);

    int
    GetSingleEventDesc(int event_id, aih::SingleEventCfg &event_desc) const;

    std::vector<aih::SingleEventCfg>
    GetAllSingleEventDesc() const;

    friend std::ostream &
    operator<<(std::ostream &os, const SingleEventConfigInfo &obj);

    std::string
    TO_STRING() const;

private:
    std::vector<aih::SingleEventCfg> _event_desc_list;
};

class ControlConfigInfo
{
public:
    ControlConfigInfo();
    explicit ControlConfigInfo(const aih::ControlInfo &control);
    ~ControlConfigInfo();

    int
    RegisterControl(const aih::ControlInfo &control);

    aih::S32
    GetControlId() const;
    aih::ControlType
    GetControlType() const;
    std::vector<aih::Point2i>
    GetPointInfos() const;

    aih::ControlInfo *
    GetControlInfoDesc();

    SingleEventConfigInfo *
    GetEventConfigList();

    // 获取泊位控制
    ParkingInfo
    GenerateParkingInfo() const;

    // 获取卡口参数
    std::tuple<bool, Line, int, aih::SingleCrossLineEvent>
    GenerateAccessConfig() const;

    // 获取违停区域参数
    std::shared_ptr<illegal_parking::NoParkingAreaConfig>
    GenerateIllegalConfigVec() const;

    friend std::ostream &
    operator<<(std::ostream &os, const ControlConfigInfo &obj);

    std::string
    TO_STRING() const;

private:
    aih::ControlInfo _control_desc;
    SingleEventConfigInfo _event_desc_list;

};

class ControlManager
{
public:
    ControlManager();
    ~ControlManager();

    int
    RegisterControl(const aih::ControlInfo &control);

    ControlConfigInfo *
    GetControlConfigInfo(aih::S32 control_id);

    int
    UnregisterControl(aih::S32 control_id);

    std::shared_ptr<sonli::ParkingInfoVec>
    GenerateParkingList(const aih::TimeInfo&) const;

    std::shared_ptr<AccessConfig_t>
    GenerateAccessConfig() const;

    std::shared_ptr<illegal_parking::NoParkingAreaConfigVec>
    GenerateIllegalConfigVec(const aih::TimeInfo&) const;

    friend std::ostream &
    operator<<(std::ostream &os, const ControlManager &obj);

    std::string
    TO_STRING() const;

    void *
    GetStreamHandle() const;
    void
    SetStreamHandle(void *handle);
    int
    UpdatePriorCityConfig(const aih::berth_event_cfg &) const;

private:
    std::map<aih::S32, ControlConfigInfo> _control_map;
    void *stream_handle = nullptr;
};

class ParamManager
{
public:
    ParamManager();
    ~ParamManager();

    ControlManager *
    GetControlManager();

    std::string
    TO_STRING() const;

    bool
    ConfigUpdated() const;

    void
    SetConfigUpdated(bool ConfigUpdated);

private:
    ControlManager _control_manager;
    bool configUpdated = false;

};

} // namespace sonli

#endif // PARAM_MANAGER_H

#include "param_manager.h"
#include "../illegal_parking/illegal_parking_algorithm.h"
#include "../utils/sl_logger.h"
#include "../utils/struct_2_stream.h"
#include "../utils/struct_2_string.h"
#include "../workflow/module_param.h"
#include "config_field.h"
#include "error.h"
#include "vulcan/vulcan_sdk.h"

using namespace sonli;

SingleEventConfigInfo::SingleEventConfigInfo()
= default;

SingleEventConfigInfo::SingleEventConfigInfo(const aih::SingleEventCfg &event_desc)
{
    _event_desc_list.push_back(event_desc);
}

SingleEventConfigInfo::~SingleEventConfigInfo()
= default;

int
SingleEventConfigInfo::RegisterSingleEvent(const aih::SingleEventCfg &event_desc)
{
    for (auto &it : _event_desc_list)
    {
        if (it.event_id_ == event_desc.event_id_)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->error("RegisterSingleEvent: event_id_ is exist.");
            return ERR_EVENT_ID_EXISTED;
        }
    }
    _event_desc_list.push_back(event_desc);
    SonLiLogger::getInstance()
        .getLogger(SonLiLogger::RotateLogger)
        ->info("RegisterSingleEvent success: {}", ::TO_STRING(event_desc));
    return 0;
}

void
SingleEventConfigInfo::UnregisterSingleEvent(int event_id)
{
    for (auto it = _event_desc_list.begin(); it != _event_desc_list.end(); ++it)
    {
        if (it->event_id_ == event_id)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->info("UnregisterSingleEvent: {}", ::TO_STRING(*it));
            _event_desc_list.erase(it);
            return;
        }
    }

    SonLiLogger::getInstance()
        .getLogger(SonLiLogger::RotateLogger)
        ->warn("UnregisterSingleEvent: event_id_ is not exist.");
}

int
SingleEventConfigInfo::GetSingleEventDesc(int event_id, aih::SingleEventCfg &event_desc) const
{
    for (auto it = _event_desc_list.begin(); it != _event_desc_list.end(); ++it)
    {
        if (it->event_id_ == event_id)
        {
            event_desc = *it;
            return SL_SUCCESS;
        }
    }
    SonLiLogger::getInstance()
        .getLogger(SonLiLogger::RotateLogger)
        ->warn("GetSingleEventDesc: event_id_ is not exist.");
    return ERR_EVENT_ID_NOT_FOUND;
}

std::vector<aih::SingleEventCfg>
SingleEventConfigInfo::GetAllSingleEventDesc() const
{
    return _event_desc_list;
}

std::string
SingleEventConfigInfo::TO_STRING() const
{
    std::stringstream ss;
    ss << *this;
    return ss.str();
}

ControlConfigInfo::ControlConfigInfo() = default;

ControlConfigInfo::ControlConfigInfo(const aih::ControlInfo &control)
{
    _control_desc = control;
}

ControlConfigInfo::~ControlConfigInfo() = default;

int
ControlConfigInfo::RegisterControl(const aih::ControlInfo &control)
{
    _control_desc = control;
    return 0;
}

aih::S32
ControlConfigInfo::GetControlId() const
{
    return _control_desc.id_;
}

aih::ControlType
ControlConfigInfo::GetControlType() const
{
    return _control_desc.control_type_;
}

std::vector<aih::Point2i>
ControlConfigInfo::GetPointInfos() const
{
    return _control_desc.point_infos_;
}

aih::ControlInfo *
ControlConfigInfo::GetControlInfoDesc()
{
    return &_control_desc;
}

SingleEventConfigInfo *
ControlConfigInfo::GetEventConfigList()
{
    return &_event_desc_list;
}

ParkingInfo
ControlConfigInfo::GenerateParkingInfo() const
{
    ParkingInfo parking_info;
    if (_control_desc.control_type_ != aih::ControlType::CONTROL_TYPE_BERTH)
    {
        return parking_info;
    }

    for (const auto &it : _control_desc.point_infos_)
    {
        parking_info.points.emplace_back(it.x, it.y);
    }
    parking_info.control_id = _control_desc.id_;

    auto _el = _event_desc_list.GetAllSingleEventDesc();
    BerthEventConfig berth_event_config;
    for (const auto &it : _el)
    {
        if (it.enable_event_ == false)
        {
            continue;
        }
        if (it.event_type_ == aih::EventType::EVENT_TYPE_ENTER_BERTH ||
            it.event_type_ == aih::EventType::EVENT_TYPE_STAY_BERTH ||
            it.event_type_ == aih::EventType::EVENT_TYPE_LEAVE_BERTH)
        {
            // 正常泊位进出场订单
            berth_event_config.control_id = _control_desc.id_;
            berth_event_config.event_id = it.event_id_;
            parking_info.event_id = it.event_id_;
            berth_event_config.event_type = aih::EventType::EVENT_TYPE_ENTER_BERTH;
            berth_event_config.config_detail.berth_events = it.cfg_info_.berth_cfg_;
        }
        else if (it.event_type_ == aih::EventType::EVENT_TYPE_ALARM_BERTH)
        {
            berth_event_config.control_id = _control_desc.id_;
            if (berth_event_config.event_id == -1)
            {
                berth_event_config.event_id = it.event_id_;
                parking_info.event_id = it.event_id_;
            }
            berth_event_config.config_detail.alarm_events = it.cfg_info_.berth_alarm_cfg_;
        }
    }
    parking_info.berth_event_list = berth_event_config;
    return parking_info;
}

std::tuple<bool, Line, int, aih::SingleCrossLineEvent>
ControlConfigInfo::GenerateAccessConfig() const
{
    Line line({0, 0}, {0, 0});
    if (_control_desc.control_type_ != aih::ControlType::CONTROL_TYPE_LINE)
    {
        return {false, line, -1, aih::SingleCrossLineEvent()};
    }
    line = Line({_control_desc.point_infos_[0].x, _control_desc.point_infos_[0].y},
                {_control_desc.point_infos_[1].x, _control_desc.point_infos_[1].y});
    for (const auto &it : _event_desc_list.GetAllSingleEventDesc())
    {
        if (it.event_type_ == aih::EventType::EVENT_TYPE_CROSS_LINE)
        {
            return {true, line, it.event_id_, it.cfg_info_.line_cfg_};
        }
    }
    return {false, line, -1, aih::SingleCrossLineEvent()};
}

std::shared_ptr<illegal_parking::NoParkingAreaConfig>
ControlConfigInfo::GenerateIllegalConfigVec() const
{
    if (_event_desc_list.GetAllSingleEventDesc().empty())
    {
        return nullptr;
    }
    auto violation_cfg = std::make_shared<illegal_parking::NoParkingAreaConfig>();
    violation_cfg->control_id = _control_desc.id_;
    for (auto point_info : _control_desc.point_infos_)
    {
        violation_cfg->zone.emplace_back(point_info.x, point_info.y);
    }
    for (const auto &it : _event_desc_list.GetAllSingleEventDesc())
    {
        if (it.event_type_ == aih::EventType::EVENT_TYPE_ENTER_VIOLATION && it.enable_event_)
        {
            violation_cfg->event_id = it.event_id_;
            violation_cfg->violation_cfg_ = it.cfg_info_.violation_cfg_;
        }
        else if (it.event_type_ == aih::EventType::EVENT_TYPE_ALARM_VIOLATION && it.enable_event_)
        {
            violation_cfg->violation_alarm_cfg_ = it.cfg_info_.violation_alarm_cfg_;
        }
    }
    return violation_cfg;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
sonli::ControlManager::ControlManager()
= default;

sonli::ControlManager::~ControlManager()
= default;

int
sonli::ControlManager::RegisterControl(const aih::ControlInfo &control)
{
    auto id = control.id_;
    if (_control_map.find(id) == _control_map.end())
    {
        ControlConfigInfo ctl(control);
        _control_map[id] = ctl;
        SonLiLogger::getInstance()
            .getLogger(SonLiLogger::RotateLogger)
            ->info("register control  success, control info: {}", ::TO_STRING(control).c_str());
    }
    else
    {
        SonLiLogger::getInstance()
            .getLogger(SonLiLogger::RotateLogger)
            ->error("control id {} already registered, control info: {}", id, ::TO_STRING(control).c_str());
        return ERR_CONTROL_CONTROLID_EXISTED;
    }
    return SL_SUCCESS;
}

sonli::ControlConfigInfo *
sonli::ControlManager::GetControlConfigInfo(aih::S32 control_id)
{
    auto it = _control_map.find(control_id);
    if (it == _control_map.end())
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("control id {} not found", control_id);
        return nullptr;
    }
    return &it->second;
}

int
sonli::ControlManager::UnregisterControl(aih::S32 control_id)
{
    auto it = _control_map.find(control_id);
    if (it == _control_map.end())
    {
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("control id {} not found", control_id);
        return ERR_CONTROL_CONTROLID_NOT_FOUND;
    }
    _control_map.erase(it);
    SonLiLogger::getInstance()
        .getLogger(SonLiLogger::RotateLogger)
        ->info("unregister control id {} success", control_id);
    return SL_SUCCESS;
}

std::shared_ptr<sonli::ParkingInfoVec>
sonli::ControlManager::GenerateParkingList(const aih::TimeInfo& current_time_info) const
{
    auto parking_info_ptr = std::make_shared<sonli::ParkingInfoVec>(current_time_info);
    for (const auto &it : _control_map)
    {
        // 如果该控件未进行事件配置，直接跳过
        auto _pi = it.second.GenerateParkingInfo();
        if (_pi.points.empty())
        {
            continue;
        }
        parking_info_ptr->parking_info.emplace_back(_pi);
    }
    parking_info_ptr->size = parking_info_ptr->parking_info.size();

    for (int i = 0; i < static_cast<int>(parking_info_ptr->parking_info.size()); i++)
    {
        std::vector<Point> space_point = parking_info_ptr->parking_info[i].points;
        if (space_point.size() != 4)
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->info("space_point.size() {} idx={}", space_point.size(), i);
            continue;
        }

        int top1_point_idx = 0;
        Point top1_point = space_point[0];

        int top2_point_idx = 1;
        Point top2_point = space_point[1];

        for (int idx = 1; idx < 4; idx++)
        {
            if (space_point[idx].y_ < top1_point.y_)
            {
                top2_point = top1_point;
                top2_point_idx = top1_point_idx;
                top1_point = space_point[idx];
                top1_point_idx = idx;
            }
            else
            {
                if (space_point[idx].y_ < top2_point.y_)
                {
                    top2_point = space_point[idx];
                    top2_point_idx = idx;
                }
            }
        }

        Point left_top_point = space_point[0];
        int left_top_point_idx = - 1;

        
        if (top1_point_idx != top2_point_idx && std::abs(top2_point_idx - top1_point_idx) != 2)
        {
            int top1_point_idx_next = top1_point_idx + 1;
            if(top1_point_idx_next == 4){
                top1_point_idx_next = 0;
            }
            if(top1_point_idx_next == top2_point_idx){
                left_top_point = top1_point;
                left_top_point_idx = top1_point_idx;
            }
            else{
                left_top_point = top2_point;
                left_top_point_idx = top2_point_idx;
            }
        }
        else
        {
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error(
                "---------space_idx {} top1_point_idx {} top2_point_idx {} left_top_point_idx {}", 
                i, top1_point_idx, top2_point_idx, left_top_point_idx);
            left_top_point_idx = -1;
        }

        // SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info(
        //         "---------space_idx {} top1_point_idx {} top2_point_idx {} left_top_point_idx {}", 
        //         i, top1_point_idx, top2_point_idx, left_top_point_idx);

        if (left_top_point_idx != -1)
        {
            std::vector<Point> space_point_new;
            for (int idx = 0; idx < 4; idx++)
            {
                space_point_new.push_back(space_point[left_top_point_idx]);
                left_top_point_idx--;
                if (left_top_point_idx == -1)
                {
                    left_top_point_idx = 3;
                }
            }
            parking_info_ptr->parking_info[i].points = space_point_new;
        }
        // else
        // {
        //     SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info( 
        //         "---------space_point-------1------- {} {} {} {} {} {} {} {}" , 
        //         space_point[0].x_, space_point[0].y_,
        //         space_point[1].x_, space_point[1].y_,
        //         space_point[2].x_, space_point[2].y_,
        //         space_point[3].x_, space_point[3].y_);
        // }
    }

    return parking_info_ptr;
}

std::shared_ptr<sonli::AccessConfig_t>
sonli::ControlManager::GenerateAccessConfig() const
{
    auto access_config_ptr = std::make_shared<sonli::AccessConfig_t>();
    for (const auto &it : _control_map)
    {
        if (it.second.GetControlType() != aih::ControlType::CONTROL_TYPE_LINE)
        {
            continue;
        }

        auto _pi = it.second.GenerateAccessConfig();
        if (std::get<0>(_pi) == false)
        {
            continue;
        }
        access_config_ptr->enable = 1;
        if (access_config_ptr->line_num == 0)
        {
            access_config_ptr->line1 = std::get<1>(_pi);
            access_config_ptr->l1_event_id = std::get<2>(_pi);
            access_config_ptr->line1_event = std::get<3>(_pi);
            access_config_ptr->l1_control_id = it.first;
            access_config_ptr->line1_name = "l1";
            access_config_ptr->line_num++;
        }
        else if (access_config_ptr->line_num == 1)
        {
            access_config_ptr->line2 = std::get<1>(_pi);
            access_config_ptr->l2_event_id = std::get<2>(_pi);
            access_config_ptr->line2_event = std::get<3>(_pi);
            access_config_ptr->l2_control_id = it.first;
            access_config_ptr->line2_name = "l2";
            access_config_ptr->line_num++;
        }
    }
    return access_config_ptr;
}

std::shared_ptr<illegal_parking::NoParkingAreaConfigVec>
ControlManager::GenerateIllegalConfigVec(const aih::TimeInfo& current_time_info) const
{
    auto illegal_config_ptr = std::make_shared<illegal_parking::NoParkingAreaConfigVec>(current_time_info);
    for (const auto &it : _control_map)
    {
        if (it.second.GetControlType() == aih::ControlType::CONTROL_TYPE_VIOLATION)
        {
            auto _register_events = it.second.GenerateIllegalConfigVec();
            if (_register_events == nullptr)
            {
                continue;
            }
            illegal_config_ptr->no_parking_area_configs_.emplace_back(_register_events);
        }
    }
    return illegal_config_ptr;
}

std::string
ControlManager::TO_STRING() const
{
    std::stringstream ss;
    ss << *this;
    return ss.str();
}

void *
ControlManager::GetStreamHandle() const
{
    return stream_handle;
}

void
ControlManager::SetStreamHandle(void *handle)
{
    stream_handle = handle;
}

int
ControlManager::UpdatePriorCityConfig(const aih::berth_event_cfg &ctl) const
{
    std::shared_ptr<VlprParam> pVlprParam = std::make_shared<VlprParam>(MUN_VLPR);

    unsigned int config_province = 0;
    if(ctl.default_province == 0){
        config_province = 1;
    }
    else if(ctl.default_province == 1){
        config_province = 3;
    }
    else if(ctl.default_province == 2){
        config_province = 5;
    }
    else if(ctl.default_province >= 3 && ctl.default_province <=7){
        config_province = ctl.default_province + 3;
    }
    else if(ctl.default_province==8){
        config_province = 2;
    }
    else if(ctl.default_province >= 9 && ctl.default_province <=20){
        config_province = ctl.default_province + 2;
    }
    else if(ctl.default_province == 21){
        config_province = 4;
    }
    else if(ctl.default_province >= 22 && ctl.default_province <=30){
        config_province = ctl.default_province + 1;
    }
    else {
        config_province = 0;
    }

    pVlprParam->setProvPriorty(config_province);
    vulcan::InputParamPtr param = std::dynamic_pointer_cast<vulcan::InputParam>(pVlprParam);

    auto ret_val = static_cast<vulcan::VulcanSDK *>(stream_handle)->UpdateConfig(MUN_VLPR, param);
    if (0 != ret_val)
    {
        SonLiLogger::getInstance()
            .getLogger(SonLiLogger::RotateLogger)
            ->error("UpdateConfig to MUN_VLPR module failed , ret_val: {}, set: {}", ret_val, ctl.default_province);
        return ERR_CONFIG_ILLEGAL;
    }
    return SL_SUCCESS;
}

sonli::ParamManager::ParamManager()
= default;

sonli::ParamManager::~ParamManager()
= default;

sonli::ControlManager *
sonli::ParamManager::GetControlManager()
{
    return &_control_manager;
}

std::string
ParamManager::TO_STRING() const
{
    return this->_control_manager.TO_STRING();
}

bool
ParamManager::ConfigUpdated() const
{
    return configUpdated;
}

void
ParamManager::SetConfigUpdated(bool configUpdatedA)
{
    configUpdated = configUpdatedA;
}

namespace sonli
{
std::ostream &
operator<<(std::ostream &os, const SingleEventConfigInfo &obj)
{
    for (auto &it : obj._event_desc_list)
    {
        os << "event_desc: " << ::TO_STRING(it) << std::endl;
    }
    return os;
}

std::ostream &
operator<<(std::ostream &os, const ControlConfigInfo &obj)
{
    return os << "_control_desc: " << ::TO_STRING(obj._control_desc) << " _event_desc_list: " << obj._event_desc_list;
}

std::ostream &
operator<<(std::ostream &os, const ControlManager &obj)
{
    for (const auto &it : obj._control_map)
    {
        os << "control_id: " << it.first << " control_config: " << it.second << std::endl;
    }
    return os;
}

}


std::string
ControlConfigInfo::TO_STRING() const
{
    std::stringstream ss;
    ss << *this;
    return ss.str();
}
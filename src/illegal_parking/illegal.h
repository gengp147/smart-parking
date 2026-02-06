//
// Created by gp147 on 4/17/2025.
//

#ifndef ILLEGAL_H
#define ILLEGAL_H

#include "../interfaces/aih_type.h"
#include "common/data_type.h"
#include "fileio/utils.h"
#include "http/http_protocol.h"
#include "manager/camara_manager.h"
#include "manager/frame_memory.h"
#include "order/save_order_images.h"
#include "utils/Singleton.h"
#include "utils/producer_consumer.h"
#include "utils/time_utils.h"
#include "vehinout/space_judge.h"
#include "vulcan/cv_types.h"
#include <chrono>
#include <exception>
#include <filesystem>
#include <memory>
#include <mutex>
#include <ostream>
#include <shared_mutex>
#include <string>
#include <utility>
#include <vector>
namespace sonli::illegal
{

enum class EIllegalCase
{
    _PARALLEL_PARKING = 0, // 线外停车
    _BAN_EXTENT_PARKING,   // 禁停区域停车
    _END_POINTS_PARKING,   // 道路两端停车

    _PARKING_ONTO_LINE, // 平行压线停车
    _ANGLE_PARKING,     // 斜停压线停车
    _VERTICAL_PARKING,  // 垂直压线停车
    _OPPOSITE_PARKING,  // 逆向停车

    _CROSS_PARKING,     // 跨位停车
    _INTO_ILLEGAL_TIME, // 禁停时段停车
};

static constexpr std::string_view IllegalCaseArrayPreDef[9] = {
    "PARALLEL_PARKING", "BAN_EXTENT_PARKING", "END_POINTS_PARKING", "PARKING_ONTO_LINE", "ANGLE_PARKING",
    "VERTICAL_PARKING", "OPPOSITE_PARKING",   "CROSS_PARKING",      "INTO_ILLEGAL_TIME"};

static constexpr std::string_view IllegalCaseCode[9] = {"01", "03", "02", "06", "07", "05", "09", "08", "04"};

inline std::string_view
IllegalCaseStr(EIllegalCase eIllegalCase)
{
    return IllegalCaseArrayPreDef[static_cast<int>(eIllegalCase)];
}

inline std::string_view
ILLEgalCaseCode(EIllegalCase eIllegalCase)
{
    return IllegalCaseCode[static_cast<int>(eIllegalCase)];
}

struct IllegalRule
{
    IllegalRule() = default;
    virtual ~IllegalRule() = default;

    inline bool
    Enabled() const noexcept
    {
        return _enabled;
    }

    bool _enabled = false;

    mutable std::shared_mutex _mutex;
};

struct IllegalRuleIllegalParking: public IllegalRule
{
    IllegalRuleIllegalParking():IllegalRule()
    {

    }
    ~IllegalRuleIllegalParking() override = default;
    std::vector<aih::Point2i> areas;
};



struct IllegalConfig
{
    std::vector<vulcan::Point> zone; // 违停区域
    std::string code;                // 区域代码

    IllegalConfig *
    updateWH(int h, int w)
    {
        auto older = zone;
        zone.clear();
        for (auto &point : older)
        {
            zone.emplace_back(vulcan::Point{(int)(point.x_ / 10000.0 * w), (int)(point.y_ / 10000.0 * h)});
        }

        return this;
    }
};

struct IllegalConfigVec : IllegalRule
{
    int __w = 1;
    int __h = 1;
    std::vector<std::shared_ptr<IllegalConfig>> illegal_rules;

    IllegalConfigVec *
    updateWH(int h, int w)
    {
        __h = h;
        __w = w;
        for (auto &rule : illegal_rules)
        {
            rule->updateWH(h, w);
        }
        return this;
    }

    auto
    JudgeIllegalBaseRule(const std::vector<vulcan::Point> &veh_chassis, float car_in_rat)
        -> std::shared_ptr<illegal::IllegalConfig>
    try
    {
        if (illegal_rules.empty() || veh_chassis.size() != 4)
        {
            return nullptr;
        }
        {
            std::shared_lock<std::shared_mutex> _sp_locker(_mutex);
            //			if (_sp_locker.try_lock())
            {
                for (const auto &rule : illegal_rules)
                {
                    float carIou = 0.0;
                    float spaceIou = 0.0;
                    (void)iou_poly(veh_chassis, rule->zone, carIou, spaceIou);
                    auto avg = (carIou + spaceIou) * 0.5;
                    if (avg > car_in_rat)
                    {
                        return rule;
                    }
                    else
                    {
                        return nullptr;
                    }
                }
            }
        }
        return nullptr;
    }
    catch (std::exception &error)
    {
        return nullptr;
    }
};

struct IllegalOppositeParkingVec : public IllegalRule
{
    int __w = 1;
    int __h = 1;
    std::vector<vulcan::Point> vector_; // 方向向量
    IllegalOppositeParkingVec *
    updateWH(int h, int w)
    {
        __h = h;
        __w = w;
        for (auto &pp : vector_)
        {
            pp.x_ = pp.x_ / 10000.0 * w;
            pp.y_ = pp.y_ / 10000.0 * h;
        }
        return this;
    }
};

struct IllegalConfigTimeNoParking : public IllegalRule
{
    std::vector<std::tuple<std::string, std::string>> time_interval;

    static int
    ParseTimeToMinutes(const std::string &time_str)
    {
        std::istringstream iss(time_str);
        int hours, minutes;
        char colon;
        if (!(iss >> hours >> colon >> minutes) || colon != ':' || hours < 0 || hours >= 24 || minutes < 0 ||
            minutes >= 60)
        {
            throw std::invalid_argument("Invalid time format: " + time_str);
        }
        return hours * 60 + minutes;
    }

    static int
    TimestampToLocalMinutes(uint64_t timestamp_ms)
    {
        std::chrono::system_clock::time_point tp{std::chrono::milliseconds(timestamp_ms)};
        time_t t = std::chrono::system_clock::to_time_t(tp);
        struct tm tm;
        localtime_r(&t, &tm);
        return tm.tm_hour * 60 + tm.tm_min;
    }

    static bool
    IsRestrictedPeriod(int restricted_min, int released_min, int current_min)
    {
        if (restricted_min > released_min)
        {
            return current_min >= restricted_min || current_min < released_min;
        }
        else
        {
            return current_min >= restricted_min && current_min < released_min;
        }
    }

    static bool
    IsParkingRestricted(const std::string &restricted_time, const std::string &released_time, uint64_t timestamp_ms)
    {
        try
        {
            int restricted_min = ParseTimeToMinutes(restricted_time);
            int released_min = ParseTimeToMinutes(released_time);
            int current_min = TimestampToLocalMinutes(timestamp_ms);

            return IsRestrictedPeriod(restricted_min, released_min, current_min);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            return false;
        }
    }

    bool
    isParkingRestricted(uint64_t timestamp_ms)
    {
        for (const auto &rule : time_interval)
        {
            auto [s, e] = rule;
            if (IsParkingRestricted(s, e, timestamp_ms))
            {
                return true;
            }
        }
        return false;
    }
};

struct IllegalCommonConfig : public IllegalRule
{
    int mode = 0;                // 取证方式 0：norm 1：capture
    int evidence_interval = 900; // 两次取证间隔
    int capture_interval = 60;   // 抓拍间隔 60s
    bool enabled = false;        //
};

struct IllegalConfigDict
{
    std::shared_ptr<IllegalCommonConfig>
    CommonConfig(const std::shared_ptr<IllegalCommonConfig> &arg = nullptr)
    {
        std::lock_guard<std::mutex> lock(commonConfig_mutex);
        if (arg)
        {
            commonConfig = arg;
        }
        if (!commonConfig)
        {
            commonConfig = std::make_shared<IllegalCommonConfig>();
        }
        return commonConfig;
    }

    std::shared_ptr<IllegalConfigVec>
    ParallelParking(const std::shared_ptr<IllegalConfigVec> &arg = nullptr)
    {
        std::lock_guard<std::mutex> lock(parallelParking_mutex);
        if (arg)
        {
            parallelParking = arg;
        }
        if (!parallelParking)
        {
            parallelParking = std::make_shared<IllegalConfigVec>();
        }
        return parallelParking;
    }
    std::shared_ptr<IllegalConfigVec>
    YellowBanExtentParking(const std::shared_ptr<IllegalConfigVec> &arg = nullptr)
    {
        std::lock_guard<std::mutex> lock(yellowBanExtentParking_mutex);
        if (arg)
        {
            yellowBanExtentParking = arg;
        }
        if (!yellowBanExtentParking)
        {
            yellowBanExtentParking = std::make_shared<IllegalConfigVec>();
        }
        return yellowBanExtentParking;
    }
    std::shared_ptr<IllegalConfigVec>
    EndsPointParking(const std::shared_ptr<IllegalConfigVec> &arg = nullptr)
    {
        std::lock_guard<std::mutex> lock(endsPointParking_mutex);
        if (arg)
        {
            endsPointParking = arg;
        }
        if (!endsPointParking)
        {
            endsPointParking = std::make_shared<IllegalConfigVec>();
        }
        return endsPointParking;
    }
    std::shared_ptr<IllegalOppositeParkingVec>
    OppositeParkingVec(const std::shared_ptr<IllegalOppositeParkingVec> &arg = nullptr)
    {
        std::lock_guard<std::mutex> lock(oppositeParkingVec_mutex);
        if (arg)
        {
            oppositeParkingVec = arg;
        }
        if (!oppositeParkingVec)
        {
            oppositeParkingVec = std::make_shared<IllegalOppositeParkingVec>();
        }
        return oppositeParkingVec;
    }
    std::shared_ptr<IllegalRule>
    getPlace_(const std::shared_ptr<IllegalRule> &arg = nullptr)
    {
        std::lock_guard<std::mutex> lock(place__mutex);
        if (arg)
        {
            place_ = arg;
        }
        if (!place_)
        {
            place_ = std::make_shared<IllegalRule>();
        }
        return place_;
    }
    std::shared_ptr<IllegalRule>
    CrossParking(const std::shared_ptr<IllegalRule> &arg = nullptr)
    {
        std::lock_guard<std::mutex> lock(crossParking_mutex);
        if (arg)
        {
            crossParking = arg;
        }
        if (!crossParking)
        {
            crossParking = std::make_shared<IllegalRule>();
        }
        return crossParking;
    }
    std::shared_ptr<IllegalRule>
    ParkingOneTheLine(const std::shared_ptr<IllegalRule> &arg = nullptr)
    {
        std::lock_guard<std::mutex> lock(parkingOneTheLine_mutex);
        if (arg)
        {
            parkingOneTheLine = arg;
        }
        if (!parkingOneTheLine)
        {
            parkingOneTheLine = std::make_shared<IllegalRule>();
        }
        return parkingOneTheLine;
    }

    std::shared_ptr<IllegalRule>
    AngledParking(const std::shared_ptr<IllegalRule> &arg = nullptr)
    {
        std::lock_guard<std::mutex> lock(angledParking_mutex);
        if (arg)
        {
            angledParking = arg;
        }
        if (!angledParking)
        {
            angledParking = std::make_shared<IllegalRule>();
        }
        return angledParking;
    }
    std::shared_ptr<IllegalRule>
    VerticalParking(const std::shared_ptr<IllegalRule> &arg = nullptr)
    {
        std::lock_guard<std::mutex> lock(verticalParking_mutex);
        if (arg)
        {
            verticalParking = arg;
        }
        if (!verticalParking)
        {
            verticalParking = std::make_shared<IllegalRule>();
        }
        return verticalParking;
    }
    std::shared_ptr<IllegalConfigTimeNoParking>
    TimeNoParking(const std::shared_ptr<IllegalConfigTimeNoParking> &arg = nullptr)
    {
        std::lock_guard<std::mutex> lock(timeNoParking_mutex);
        if (arg)
        {
            timeNoParking = arg;
        }
        if (!timeNoParking)
        {
            timeNoParking = std::make_shared<IllegalConfigTimeNoParking>();
        }
        return timeNoParking;
    }

private:

    std::mutex commonConfig_mutex;
    std::shared_ptr<IllegalCommonConfig> commonConfig = nullptr;        // 公共参数
    std::mutex parallelParking_mutex;
    std::shared_ptr<IllegalConfigVec> parallelParking = nullptr;        // 平行区域停车
    std::mutex yellowBanExtentParking_mutex;
    std::shared_ptr<IllegalConfigVec> yellowBanExtentParking = nullptr; // 禁停区域停车
    std::mutex endsPointParking_mutex;
    std::shared_ptr<IllegalConfigVec> endsPointParking = nullptr;       // 首尾禁停
    std::mutex place__mutex;
    std::shared_ptr<IllegalRule> place_ = nullptr;
    std::mutex crossParking_mutex;
    std::shared_ptr<IllegalRule> crossParking = nullptr; // 跨泊位计算参数
    std::mutex parkingOneTheLine_mutex;
    std::shared_ptr<IllegalRule> parkingOneTheLine = nullptr; // 压线停车
    std::mutex angledParking_mutex;
    std::shared_ptr<IllegalRule> angledParking = nullptr;                    // 斜停，锐角
    std::mutex verticalParking_mutex;
    std::shared_ptr<IllegalRule> verticalParking = nullptr;                  // 垂直停车
    std::mutex oppositeParkingVec_mutex;
    std::shared_ptr<IllegalOppositeParkingVec> oppositeParkingVec = nullptr; // 反向停车
    std::mutex timeNoParking_mutex;
    std::shared_ptr<IllegalConfigTimeNoParking> timeNoParking = nullptr;
};

struct IllegalConfigDictMulti
{
private:
    std::map<int, std::unique_ptr<IllegalConfigDict>> all_config;

public:
    std::unique_ptr<IllegalConfigDict> &
    Pick(int channel)
    {
        if (!all_config.count(channel))
        {
            auto tmp = std::make_unique<IllegalConfigDict>();
            all_config.emplace(channel, std::move(tmp));
        }
        return all_config[channel];
    }
};

using IllegalConfigSL = Singleton<IllegalConfigDictMulti>;

struct IllegalCaptureDetail
{
    unsigned long cap_stamp{0};
    std::shared_ptr<IllegalConfig> rule;
    int frame_id = -1;
    VehInfoPtr veh_info_ptr; // 保存坐标信息
    std::string event;

    bool
    hasData() const noexcept
    {
        return frame_id != -1;
    }

    inline friend std::ostream &
    operator<<(std::ostream &os, const IllegalCaptureDetail &captureDetail)
    {
        os << "cap_stamp: " << captureDetail.cap_stamp << " frame_id: " << captureDetail.frame_id << " veh_info_ptr: "
           << (captureDetail.veh_info_ptr->lp_info_ptr->lp_number_ptr->lp_str != "-" &&
                       !captureDetail.veh_info_ptr->lp_info_ptr->lp_number_ptr->lp_str.empty()
                   ? captureDetail.veh_info_ptr->lp_info_ptr->lp_number_ptr->lp_str
                   : std::to_string(captureDetail.veh_info_ptr->veh_track_id))
           << " event: " << captureDetail.event;
        return os;
    }

    void
    resetCache()
    {
        cap_stamp = 0;
        rule.reset();
        frame_id = -1;
        veh_info_ptr.reset();
        event.clear();
    }
};

struct IllegalParkingDetail
{
    [[nodiscard]] virtual bool
    isNeedJudgeAgain() const noexcept
    {
        return is_illegal < 1.0f;
    }

    IllegalParkingDetail &
    selfAdd() noexcept
    {
        is_illegal += 0.5;
        return *this;
    }

    IllegalParkingDetail &
    selfReduce() noexcept
    {
        is_illegal = std::max(is_illegal - 0.5, 0.0);
        return *this;
    }

protected:
    float is_illegal = 0.0; // 是否已经违停
public:
    virtual ~IllegalParkingDetail() = default;

    EIllegalCase illegal_case;             // 违停类型
    std::string illegal_type;              // 违停类型
    IllegalCaptureDetail I_cap_stamp{};    // 第一次抓拍
    IllegalCaptureDetail II_cap_stamp{};   // 第二次抓拍
    IllegalCaptureDetail III_cap_stamp{};  // 第三次抓拍
    IllegalCaptureDetail IIII_cap_stamp{}; // 第四次抓拍
    IllegalCaptureDetail *vehicle_plate_source = nullptr;
    unsigned long need_cd = 0; // 两次违规抓拍需要的cd
    int channel = -1;

    virtual bool
    isSufficientEvidence() const noexcept
    {
        return IIII_cap_stamp.hasData();
    }

    friend std::ostream &
    operator<<(std::ostream &os, const IllegalParkingDetail &parkingDetail);
    virtual void
    UpdateCapStamp(std::shared_ptr<IllegalConfig> rule, unsigned long frame_timestamp, int camera_channel, int frame_id,
                   VehInfoPtr veh);

    void
    resetCache()
    {
        vehicle_plate_source = nullptr;
        IIII_cap_stamp.resetCache();
        III_cap_stamp.resetCache();
        II_cap_stamp.resetCache();
        I_cap_stamp.resetCache();
    }
};

struct IllegalParkingDetailNormal : public IllegalParkingDetail
{
    [[nodiscard]] virtual bool
    isNeedJudgeAgain() const noexcept
    {
        return is_illegal < 0.5f;
    }

    virtual bool
    isSufficientEvidence() const noexcept
    {
        return III_cap_stamp.hasData();
    }

    void
    UpdateCapStamp(std::shared_ptr<IllegalConfig> rule, unsigned long frame_timestamp, int camera_channel, int frame_id,
                   VehInfoPtr veh) override;
};

class IllegalParkingHistoryRecord
{
public:
    struct _Item
    {
        _Item(std::string vehiclePlateNumber, int vehicleTrackId, unsigned long parkingTimestamp,
              std::string illegal_case_a)
            : vehicle_plate_number(std::move(vehiclePlateNumber)),
              vehicle_track_id(vehicleTrackId),
              parking_timestamp(parkingTimestamp),
              illegal_case(illegal_case_a)
        {
        }
        std::string vehicle_plate_number = "";
        int vehicle_track_id = -1;
        unsigned long parking_timestamp = 0;
        int channel = 0;
        std::string illegal_case = "";

    public:
        _Item *
        selfAdd() noexcept
        {
            count++;
            return this;
        }
        bool
        enough() const noexcept;
        bool
        operator==(const _Item &o) const noexcept
        {
            if (illegal_case != o.illegal_case)
            {
                return false;
            }
            if (vehicle_track_id == o.vehicle_track_id)
            {
                return true;
            }

            if (!o.vehicle_plate_number.empty() && !vehicle_plate_number.empty())
            {
                return vehicle_plate_number == o.vehicle_plate_number;
            }
            return false;

        }
        bool
        operator!=(const _Item &o) const noexcept
        {
            return !(*this == o);
        }

    private:
        int count = 0;
    };

    _Item *
    find(const std::unique_ptr<_Item> &obj) const noexcept
    {
        if (obj)
        {
            for (const auto &_item : _history)
            {
                if (*_item == *obj)
                {
                    return _item.get();
                }
            }
        }
        else
        {
            return nullptr;
        }
        return nullptr;
    }

    IllegalParkingHistoryRecord *
    addNew(std::unique_ptr<_Item> &&new_obj)
    {
        new_obj->channel = this->channel;
        _history.emplace_back(std::forward<std::unique_ptr<_Item>>(new_obj));
        return this;
    }
    int channel = 0;

private:
    std::vector<std::unique_ptr<_Item>> _history;
};

struct MultipleIllegalHistory
{

    std::unique_ptr<IllegalParkingHistoryRecord> &
    getChannel(int channel)
    {
        if (!_multiple_channel_illegal_history.count(channel))
        {
            _multiple_channel_illegal_history[channel] = std::make_unique<IllegalParkingHistoryRecord>();
            _multiple_channel_illegal_history[channel]->channel = channel;
        }
        return _multiple_channel_illegal_history[channel];
    }

private:
    std::map<int, std::unique_ptr<IllegalParkingHistoryRecord>> _multiple_channel_illegal_history;
};



inline void
IllegalParkingDetail::UpdateCapStamp(std::shared_ptr<IllegalConfig> rule, unsigned long frame_timestamp,
                                     int camera_channel, int frame_id, VehInfoPtr veh)
{
    if (!isNeedJudgeAgain())
    {
        return;
    }

    channel = camera_channel;
    auto time_config = IllegalConfigSL::GetInstance().Pick(camera_channel)->CommonConfig();
    unsigned long latest_timestamp = 0; // 最后抓拍时间
    if (I_cap_stamp.cap_stamp != 0)
    {
        latest_timestamp = I_cap_stamp.cap_stamp;
    }
    else
    {
        I_cap_stamp.cap_stamp = frame_timestamp;
        I_cap_stamp.frame_id = frame_id;
        I_cap_stamp.rule = rule;
        I_cap_stamp.veh_info_ptr = veh;
        return;
    }

    if (II_cap_stamp.cap_stamp != 0)
    {
        latest_timestamp = II_cap_stamp.cap_stamp;
    }
    else
    {
        if ((frame_timestamp - latest_timestamp) / 1000 > time_config->capture_interval)
        {
            II_cap_stamp.cap_stamp = frame_timestamp;
            II_cap_stamp.frame_id = frame_id;
            II_cap_stamp.rule = rule;
            II_cap_stamp.veh_info_ptr = veh;
        }
        return;
    }

    if (III_cap_stamp.cap_stamp != 0)
    {
        latest_timestamp = III_cap_stamp.cap_stamp;
    }
    else
    {
        if ((frame_timestamp - latest_timestamp) / 1000 > time_config->capture_interval)
        {
            III_cap_stamp.cap_stamp = frame_timestamp;
            III_cap_stamp.frame_id = frame_id;
            III_cap_stamp.rule = rule;
            III_cap_stamp.veh_info_ptr = veh;
        }
        return;
    }

    if (IIII_cap_stamp.cap_stamp == 0)
    {
        if ((frame_timestamp - latest_timestamp) / 1000 > time_config->capture_interval)
        {
            IIII_cap_stamp.cap_stamp = frame_timestamp;
            IIII_cap_stamp.frame_id = frame_id;
            IIII_cap_stamp.rule = rule;
            IIII_cap_stamp.veh_info_ptr = veh;
        }
    }
}

inline void
IllegalParkingDetailNormal::UpdateCapStamp(std::shared_ptr<IllegalConfig> rule, unsigned long frame_timestamp,
                                           int camera_channel, int frame_id, VehInfoPtr veh)
{
    if (is_illegal == 0.5f)
    {
        return;
    }

    auto time_config = IllegalConfigSL::GetInstance().Pick(camera_channel)->CommonConfig();
    if ((frame_timestamp - need_cd) / 1000 < time_config->evidence_interval)
        return;
    unsigned long latest_timestamp = 0; // 最后抓拍时间
    if (I_cap_stamp.cap_stamp != 0)
    {
        latest_timestamp = I_cap_stamp.cap_stamp;
    }
    else
    {
        I_cap_stamp.cap_stamp = frame_timestamp;
        I_cap_stamp.frame_id = frame_id;
        I_cap_stamp.rule = rule;
        I_cap_stamp.veh_info_ptr = veh;
        return;
    }

    if (II_cap_stamp.cap_stamp != 0)
    {
        latest_timestamp = II_cap_stamp.cap_stamp;
    }
    else
    {
        if ((frame_timestamp - latest_timestamp) / 1000 > time_config->capture_interval)
        {
            II_cap_stamp.cap_stamp = frame_timestamp;
            II_cap_stamp.frame_id = frame_id;
            II_cap_stamp.rule = rule;
            II_cap_stamp.veh_info_ptr = veh;
        }
        return;
    }

    if (III_cap_stamp.cap_stamp == 0)
    {
        if ((frame_timestamp - latest_timestamp) / 1000 > time_config->capture_interval)
        {
            III_cap_stamp.cap_stamp = frame_timestamp;
            III_cap_stamp.frame_id = frame_id;
            III_cap_stamp.rule = rule;
            III_cap_stamp.veh_info_ptr = veh;
        }
    }
}

inline std::ostream &
operator<<(std::ostream &os, const IllegalParkingDetail &parkingDetail)
{
    os << "is_illegal: " << parkingDetail.is_illegal << " illegal_type: " << parkingDetail.illegal_type
       << " I_cap_stamp: " << parkingDetail.I_cap_stamp << " II_cap_stamp: " << parkingDetail.II_cap_stamp
       << " III_cap_stamp: " << parkingDetail.III_cap_stamp << " IIII_cap_stamp: " << parkingDetail.IIII_cap_stamp
       << " need_cd: " << parkingDetail.need_cd;
    return os;
}

class IllegalCaptureCropBase
{
public:
    explicit IllegalCaptureCropBase(std::shared_ptr<IllegalParkingDetail> task_args = nullptr)
        : _task_args(std::move(task_args)), _messageData(std::make_unique<VehicleIllegalData>())
    {
    }
    virtual ~IllegalCaptureCropBase() {}
    virtual bool
    Run() = 0;
    IllegalCaptureCropBase &
    setTaskArgs(const std::shared_ptr<IllegalParkingDetail> &taskArgs)
    {
        _task_args = taskArgs;
        return *this;
    }

    std::shared_ptr<IllegalParkingDetail>
    getIllegalParkingDetail()
    {
        if (!_task_args)
        {
            _task_args = std::make_shared<IllegalParkingDetail>();
        }
        return _task_args;
    };

    [[nodiscard]] const std::string &
    getErrorMessage() const
    {
        return _error_message;
    }
    void
    setErrorMessage(const std::string &errorMessage)
    {
        _error_message = errorMessage;
    }
    [[nodiscard]] int
    getChannelId() const
    {
        return channel_id;
    }
    void
    setChannelId(int channelId)
    {
        channel_id = channelId;
    }
    [[nodiscard]] std::string
    getSaveTo() const
    {
        std::time_t currentTime = std::time(nullptr);
        std::tm *localTime = std::localtime(&currentTime);
        std::ostringstream oss;
        oss << std::put_time(localTime, "%Y/%m/%d");
        auto save_path = save_to + "/" + oss.str();
        if (!std::filesystem::exists(save_path))
        {
            std::filesystem::create_directories(save_path);
        }
        return save_path;
    }
    void
    setSaveTo(const std::string &saveTo)
    {
        save_to = saveTo;
    }
    [[nodiscard]] const std::vector<std::string> &
    getAlreadySaveImage() const noexcept
    {
        return _already_save_image;
    }
    std::unique_ptr<VehicleIllegalData> &
    getMessageData()
    {
        if (!_messageData)
        {
            _messageData = std::make_unique<VehicleIllegalData>();
        }
        return _messageData;
    }

    bool
    filter()
    {

        auto &imp_h = Singleton<MultipleIllegalHistory>::GetInstance().getChannel(_task_args->channel);
        auto opt = std::make_unique<illegal::IllegalParkingHistoryRecord::_Item>(
            _messageData->plate_number, _task_args->vehicle_plate_source->veh_info_ptr->veh_track_id,
            _task_args->vehicle_plate_source->veh_info_ptr->veh_timestamp, _task_args->illegal_type);
        auto imp_i = imp_h->find(opt);
        if (!imp_i)
        {
            imp_h->addNew(std::move(opt));
            return false;
        }
        else
        {
            if (imp_i->enough())
            {
                return true;
            }
            imp_i->selfAdd();
            return false;
        }
    }

    static std::unique_ptr<IllegalCaptureCropBase>
    CreateTask(std::shared_ptr<IllegalParkingDetail> task_args);
    IllegalCaptureCropBase &
    setCameraManagerWeakPtr(CameraManager *CameraManagerWeakPtr)
    {
        cameraManagerWeakPtr = CameraManagerWeakPtr;
        return *this;
    }

    [[nodiscard]] bool getRunResult() const noexcept {
        return __run_result;
    }

protected:
    static inline long
    getIndex() noexcept
    {
        auto tmp = _index;
        _index++;
        if (_index > 1000)
        {
            _index = 0;
        }
        return tmp;
    }

protected:
    std::vector<std::string> _already_save_image;
    std::unique_ptr<VehicleIllegalData> _messageData;

    bool __run_result = false;

private:
    static inline long _index = 0;

    std::string _error_message;
    std::string save_to;

protected:
    std::shared_ptr<IllegalParkingDetail> _task_args;
    CameraManager *cameraManagerWeakPtr;
    int channel_id = -1;
};

class IllegalCaptureCropTask : public IllegalCaptureCropBase
{
public:
    explicit IllegalCaptureCropTask(std::shared_ptr<IllegalParkingDetail> task_args = nullptr)
        : IllegalCaptureCropBase(task_args)
    {
    }

    ~IllegalCaptureCropTask() override = default;

public:
    bool
    Run() override
    {
        if (!_task_args)
        {
            setErrorMessage("Task args is nullptr, invalid arguments.");
            return false;
        }
        if (_task_args->IIII_cap_stamp.frame_id < 0)
        {
            if (_task_args->IIII_cap_stamp.veh_info_ptr)
            {
                _task_args->IIII_cap_stamp.frame_id = (int)_task_args->IIII_cap_stamp.veh_info_ptr->frame_id;
            }
            else
            {
                setErrorMessage(
                    "_task_args->IIII_cap_stamp.frame_id < 0 && _task_args->IIII_cap_stamp.veh_info_ptr is nullptr");
                return false;
            }
        }
        _task_args->vehicle_plate_source = std::addressof(_task_args->IIII_cap_stamp);

        if (_task_args->III_cap_stamp.frame_id < 0)
        {
            if (_task_args->III_cap_stamp.veh_info_ptr)
            {
                _task_args->III_cap_stamp.frame_id = (int)_task_args->III_cap_stamp.veh_info_ptr->frame_id;
            }
            else
            {
                setErrorMessage(
                    "_task_args->III_cap_stamp.frame_id < 0 && _task_args->III_cap_stamp.veh_info_ptr is nullptr");
                return false;
            }
        }

        if (_task_args->III_cap_stamp.veh_info_ptr->lp_info_ptr->lp_number_ptr->vlp_rec_confidence >
            _task_args->vehicle_plate_source->veh_info_ptr->lp_info_ptr->lp_number_ptr->vlp_rec_confidence)
        {
            _task_args->vehicle_plate_source = std::addressof(_task_args->III_cap_stamp);
        }

        if (_task_args->II_cap_stamp.frame_id < 0)
        {
            if (_task_args->II_cap_stamp.veh_info_ptr)
            {
                _task_args->II_cap_stamp.frame_id = (int)_task_args->II_cap_stamp.veh_info_ptr->frame_id;
            }
            else
            {
                setErrorMessage(
                    "_task_args->II_cap_stamp.frame_id < 0 && _task_args->II_cap_stamp.veh_info_ptr is nullptr");
                return false;
            }
        }

        if (_task_args->II_cap_stamp.veh_info_ptr->lp_info_ptr->lp_number_ptr->vlp_rec_confidence >
            _task_args->vehicle_plate_source->veh_info_ptr->lp_info_ptr->lp_number_ptr->vlp_rec_confidence)
        {
            _task_args->vehicle_plate_source = std::addressof(_task_args->II_cap_stamp);
        }

        if (_task_args->I_cap_stamp.frame_id < 0)
        {
            if (_task_args->I_cap_stamp.veh_info_ptr)
            {
                _task_args->I_cap_stamp.frame_id = (int)_task_args->I_cap_stamp.veh_info_ptr->frame_id;
            }
            else
            {
                setErrorMessage(
                    "_task_args->I_cap_stamp.frame_id < 0 && _task_args->I_cap_stamp.veh_info_ptr is nullptr");
                return false;
            }
        }

        if (_task_args->I_cap_stamp.veh_info_ptr->lp_info_ptr->lp_number_ptr->vlp_rec_confidence >
            _task_args->vehicle_plate_source->veh_info_ptr->lp_info_ptr->lp_number_ptr->vlp_rec_confidence)
        {
            _task_args->vehicle_plate_source = std::addressof(_task_args->I_cap_stamp);
        }

        if (_task_args->IIII_cap_stamp.cap_stamp < 100)
        {
            if (_task_args->IIII_cap_stamp.veh_info_ptr)
            {
                _task_args->IIII_cap_stamp.cap_stamp = _task_args->IIII_cap_stamp.veh_info_ptr->veh_timestamp;
            }
            else
            {
                setErrorMessage(
                    "_task_args->IIII_cap_stamp.cap_stamp < 100 && _task_args->IIII_cap_stamp.veh_info_ptr is nullptr");
                return false;
            }
        }

        if (_task_args->III_cap_stamp.cap_stamp < 100)
        {
            if (_task_args->III_cap_stamp.veh_info_ptr)
            {
                _task_args->III_cap_stamp.cap_stamp = _task_args->III_cap_stamp.veh_info_ptr->veh_timestamp;
            }
            else
            {
                setErrorMessage(
                    "_task_args->III_cap_stamp.cap_stamp < 100 && _task_args->IIII_cap_stamp.veh_info_ptr is nullptr");
                return false;
            }
        }

        if (_task_args->II_cap_stamp.cap_stamp < 100)
        {
            if (_task_args->II_cap_stamp.veh_info_ptr)
            {
                _task_args->II_cap_stamp.cap_stamp = _task_args->II_cap_stamp.veh_info_ptr->veh_timestamp;
            }
            else
            {
                setErrorMessage(
                    "_task_args->II_cap_stamp.cap_stamp < 100 && _task_args->II_cap_stamp.veh_info_ptr is nullptr");
                return false;
            }
        }

        if (_task_args->I_cap_stamp.cap_stamp < 100)
        {
            if (_task_args->I_cap_stamp.veh_info_ptr)
            {
                _task_args->I_cap_stamp.cap_stamp = _task_args->I_cap_stamp.veh_info_ptr->veh_timestamp;
            }
            else
            {
                setErrorMessage(
                    "_task_args->I_cap_stamp.cap_stamp < 100 && _task_args->I_cap_stamp.veh_info_ptr is nullptr");
                return false;
            }
        }
        if (filter())
        {
            return true;
        }

        __run_result = _Run_Implement();
        return __run_result;
    }

private:
    bool
    _Run_Implement()
    try
    {
        FrameImageData frame_data_iiii;
        frame_data_iiii.frame_id = _task_args->IIII_cap_stamp.frame_id;
        auto ret =
            InterruptFrameMemoryManager::GetInstance().Choice(getChannelId()).readFrame(&frame_data_iiii, nullptr);
        if (ret == -1)
        {
            setErrorMessage("_Run_Implement: readFrame from channel:" + std::to_string(getChannelId()) + ", frame id" +
                            std::to_string(_task_args->IIII_cap_stamp.frame_id) + " error.");
            return false;
        }
        auto vehicle_location_iiii = _task_args->IIII_cap_stamp.veh_info_ptr->location;
        auto vehicle_location_cv_iiii = cv::Rect(vehicle_location_iiii.x_, vehicle_location_iiii.y_,
                                                 vehicle_location_iiii.width_, vehicle_location_iiii.height_);
        auto original_iiii = frame_data_iiii.ToMat();
        auto veh_iiii = original_iiii(vehicle_location_cv_iiii);
        std::string filename_iiii("VI_" + std::to_string(getIndex()) + ".jpg");
        std::string save_path_iiii(getSaveTo() + "/" + filename_iiii);
        cv::imwrite(save_path_iiii, veh_iiii);
        _already_save_image.emplace_back(save_path_iiii);
        _messageData->image4_path = save_path_iiii;
        _messageData->mage_cap_time4 = timestampToDataTime((long)frame_data_iiii.timestamp / 1000, formater);

        FrameImageData frame_data_iii;
        frame_data_iii.frame_id = _task_args->III_cap_stamp.frame_id;
        ret = InterruptFrameMemoryManager::GetInstance().Choice(getChannelId()).readFrame(&frame_data_iii, nullptr);
        if (ret == -1)
        {
            setErrorMessage("_Run_Implement: readFrame from channel:" + std::to_string(getChannelId()) + ", frame id" +
                            std::to_string(_task_args->III_cap_stamp.frame_id) + " error.");
            return false;
        }
        auto original_iii = frame_data_iii.ToMat();
        std::string filename_iii("VI_" + std::to_string(getIndex()) + ".jpg");
        std::string save_path_iii(getSaveTo() + "/" + filename_iii);
        cv::imwrite(save_path_iii, original_iii);
        _already_save_image.emplace_back(save_path_iii);
        _messageData->image3_path = save_path_iii;
        _messageData->mage_cap_time3 = timestampToDataTime((long)frame_data_iii.timestamp / 1000, formater);

        FrameImageData frame_data_ii;
        frame_data_ii.frame_id = _task_args->II_cap_stamp.frame_id;
        ret = InterruptFrameMemoryManager::GetInstance().Choice(getChannelId()).readFrame(&frame_data_ii, nullptr);
        if (ret == -1)
        {
            setErrorMessage("_Run_Implement: readFrame from channel:" + std::to_string(getChannelId()) + ", frame id" +
                            std::to_string(_task_args->II_cap_stamp.frame_id) + " error.");
            return false;
        }
        auto vehicle_location_ii = _task_args->IIII_cap_stamp.veh_info_ptr->location;
        auto vehicle_location_cv_ii = cv::Rect(vehicle_location_ii.x_, vehicle_location_ii.y_,
                                               vehicle_location_ii.width_, vehicle_location_ii.height_);
        auto original_ii = frame_data_ii.ToMat();
        auto veh_ii = original_iiii(vehicle_location_cv_ii);
        std::string filename_ii("VI_" + std::to_string(getIndex()) + ".jpg");
        std::string save_path_ii(getSaveTo() + "/" + filename_ii);
        cv::imwrite(save_path_ii, veh_ii);
        _already_save_image.emplace_back(save_path_ii);
        _messageData->image2_path = save_path_ii;
        _messageData->mage_cap_time2 = timestampToDataTime((long)frame_data_ii.timestamp / 1000, formater);

        FrameImageData frame_data_i;
        frame_data_i.frame_id = _task_args->I_cap_stamp.frame_id;
        ret = InterruptFrameMemoryManager::GetInstance().Choice(getChannelId()).readFrame(&frame_data_i, nullptr);
        if (ret == -1)
        {
            setErrorMessage("_Run_Implement: readFrame from channel:" + std::to_string(getChannelId()) + ", frame id" +
                            std::to_string(_task_args->I_cap_stamp.frame_id) + " error.");
            return false;
        }
        auto original_i = frame_data_i.ToMat();
        std::string filename_i("VI_" + std::to_string(getIndex()) + ".jpg");
        std::string save_path_i(getSaveTo() + "/" + filename_i);
        cv::imwrite(save_path_i, original_i);
        _already_save_image.emplace_back(save_path_i);
        _messageData->image1_path = save_path_i;
        _messageData->mage_cap_time1 = timestampToDataTime((long)frame_data_i.timestamp / 1000, formater);

        FrameImageData plate_number_frame;
        plate_number_frame.frame_id = _task_args->vehicle_plate_source->frame_id;
        ret = InterruptFrameMemoryManager::GetInstance().Choice(getChannelId()).readFrame(&plate_number_frame, nullptr);
        if (ret == -1)
        {
            setErrorMessage("_Run_Implement: readFrame from channel:" + std::to_string(getChannelId()) + ", frame id" +
                            std::to_string(_task_args->vehicle_plate_source->frame_id) + " error.");
            return false;
        }
        auto vehicle_location_plate = _task_args->vehicle_plate_source->veh_info_ptr->lp_info_ptr->location;
        auto vehicle_location_cv_plate = cv::Rect(vehicle_location_plate.x_, vehicle_location_plate.y_,
                                                  vehicle_location_plate.width_, vehicle_location_plate.height_);
        auto original = plate_number_frame.ToMat();
        auto veh_plate = original(vehicle_location_cv_plate);
        std::string filename_plate = ("VI_" + std::to_string(getIndex()) + ".jpg");
        std::string _save_filename_plate(getSaveTo() + "/" + filename_plate);
        cv::imwrite(_save_filename_plate, veh_plate);
        _messageData->image5_path = _save_filename_plate;
        _already_save_image.emplace_back(_save_filename_plate);

        if (_task_args->III_cap_stamp.rule)
        {
            _messageData->space_code = _task_args->III_cap_stamp.rule->code;
        }
        _messageData->exception_time = _messageData->mage_cap_time4;
        _messageData->trigger_time = _messageData->mage_cap_time1;
        _messageData->exception_type = _task_args->illegal_type;

        return true;
    }
    catch (std::exception &error)
    {
        setErrorMessage(error.what());
        return false;
    }
    catch (...)
    {
        setErrorMessage("Unknown error.");
        return false;
    }

public:
    inline friend std::ostream &
    operator<<(std::ostream &os, const IllegalCaptureCropTask &task)
    {
        if (task._task_args)
        {
            os << " _task_args: " << *task._task_args;
        }
        return os;
    }
};

class IllegalCaptureCropTaskNormal : public IllegalCaptureCropBase
{
public:
    explicit IllegalCaptureCropTaskNormal(std::shared_ptr<IllegalParkingDetail> task_args = nullptr)
        : IllegalCaptureCropBase(task_args)
    {
    }

    ~IllegalCaptureCropTaskNormal() override = default;

public:
    bool
    Run() override
    {
        if (!_task_args)
        {
            setErrorMessage("Task args is nullptr, invalid arguments.");
            return false;
        }

        if (_task_args->III_cap_stamp.frame_id < 0)
        {
            if (_task_args->III_cap_stamp.veh_info_ptr)
            {
                _task_args->III_cap_stamp.frame_id = (int)_task_args->III_cap_stamp.veh_info_ptr->frame_id;
            }
            else
            {
                setErrorMessage(
                    "_task_args->III_cap_stamp.frame_id < 0 && _task_args->III_cap_stamp.veh_info_ptr is nullptr");
                return false;
            }
        }
        _task_args->vehicle_plate_source = std::addressof(_task_args->III_cap_stamp);

        if (_task_args->II_cap_stamp.frame_id < 0)
        {
            if (_task_args->II_cap_stamp.veh_info_ptr)
            {
                _task_args->II_cap_stamp.frame_id = (int)_task_args->II_cap_stamp.veh_info_ptr->frame_id;
            }
            else
            {
                setErrorMessage(
                    "_task_args->II_cap_stamp.frame_id < 0 && _task_args->II_cap_stamp.veh_info_ptr is nullptr");
                return false;
            }
        }

        if (_task_args->II_cap_stamp.veh_info_ptr->lp_info_ptr->lp_number_ptr->vlp_rec_confidence >
            _task_args->vehicle_plate_source->veh_info_ptr->lp_info_ptr->lp_number_ptr->vlp_rec_confidence)
        {
            _task_args->vehicle_plate_source = std::addressof(_task_args->II_cap_stamp);
        }

        if (_task_args->I_cap_stamp.frame_id < 0)
        {
            if (_task_args->I_cap_stamp.veh_info_ptr)
            {
                _task_args->I_cap_stamp.frame_id = (int)_task_args->I_cap_stamp.veh_info_ptr->frame_id;
            }
            else
            {
                setErrorMessage(
                    "_task_args->I_cap_stamp.frame_id < 0 && _task_args->I_cap_stamp.veh_info_ptr is nullptr");
                return false;
            }
        }

        if (_task_args->I_cap_stamp.veh_info_ptr->lp_info_ptr->lp_number_ptr->vlp_rec_confidence >
            _task_args->vehicle_plate_source->veh_info_ptr->lp_info_ptr->lp_number_ptr->vlp_rec_confidence)
        {
            _task_args->vehicle_plate_source = std::addressof(_task_args->I_cap_stamp);
        }

        if (_task_args->III_cap_stamp.cap_stamp < 100)
        {
            if (_task_args->III_cap_stamp.veh_info_ptr)
            {
                _task_args->III_cap_stamp.cap_stamp = _task_args->III_cap_stamp.veh_info_ptr->veh_timestamp;
            }
            else
            {
                setErrorMessage(
                    "_task_args->III_cap_stamp.cap_stamp < 100 && _task_args->IIII_cap_stamp.veh_info_ptr is nullptr");
                return false;
            }
        }

        if (_task_args->II_cap_stamp.cap_stamp < 100)
        {
            if (_task_args->II_cap_stamp.veh_info_ptr)
            {
                _task_args->II_cap_stamp.cap_stamp = _task_args->II_cap_stamp.veh_info_ptr->veh_timestamp;
            }
            else
            {
                setErrorMessage(
                    "_task_args->II_cap_stamp.cap_stamp < 100 && _task_args->II_cap_stamp.veh_info_ptr is nullptr");
                return false;
            }
        }

        if (_task_args->I_cap_stamp.cap_stamp < 100)
        {
            if (_task_args->I_cap_stamp.veh_info_ptr)
            {
                _task_args->I_cap_stamp.cap_stamp = _task_args->I_cap_stamp.veh_info_ptr->veh_timestamp;
            }
            else
            {
                setErrorMessage(
                    "_task_args->I_cap_stamp.cap_stamp < 100 && _task_args->I_cap_stamp.veh_info_ptr is nullptr");
                return false;
            }
        }
        if (filter())
        {
            return true;
        }

        __run_result = _Run_Implement();
        return __run_result;
    }

private:
    bool
    _Run_Implement()
    try
    {
        FrameImageData frame_data_iii;
        frame_data_iii.frame_id = _task_args->III_cap_stamp.frame_id;
        auto ret =
            InterruptFrameMemoryManager::GetInstance().Choice(getChannelId()).readFrame(&frame_data_iii, nullptr);
        if (ret == -1)
        {
            setErrorMessage("_Run_Implement iii: readFrame from channel:" + std::to_string(getChannelId()) +
                            ", frame id" + std::to_string(_task_args->III_cap_stamp.frame_id) + " error.");
            return false;
        }
        auto original_iii = frame_data_iii.ToMat();

        FrameImageData frame_data_ii;
        frame_data_ii.frame_id = _task_args->II_cap_stamp.frame_id;
        ret = InterruptFrameMemoryManager::GetInstance().Choice(getChannelId()).readFrame(&frame_data_ii, nullptr);
        if (ret == -1)
        {
            setErrorMessage("_Run_Implement ii: readFrame from channel:" + std::to_string(getChannelId()) +
                            ", frame id" + std::to_string(_task_args->II_cap_stamp.frame_id) + " error.");
            return false;
        }
        auto original_ii = frame_data_ii.ToMat();

        FrameImageData frame_data_i;
        frame_data_i.frame_id = _task_args->I_cap_stamp.frame_id;
        ret = InterruptFrameMemoryManager::GetInstance().Choice(getChannelId()).readFrame(&frame_data_i, nullptr);
        if (ret == -1)
        {
            setErrorMessage("_Run_Implement i: readFrame from channel:" + std::to_string(getChannelId()) +
                            ", frame id" + std::to_string(_task_args->I_cap_stamp.frame_id) + " error.");
            return false;
        }
        auto original_i = frame_data_i.ToMat();

        FrameImageData plate_number_frame;
        plate_number_frame.frame_id = _task_args->vehicle_plate_source->frame_id;
        ret = InterruptFrameMemoryManager::GetInstance().Choice(getChannelId()).readFrame(&plate_number_frame, nullptr);
        if (ret == -1)
        {
            setErrorMessage("_Run_Implement plate: readFrame from channel:" + std::to_string(getChannelId()) +
                            ", frame id" + std::to_string(_task_args->vehicle_plate_source->frame_id) + " error.");
            return false;
        }
        auto original = plate_number_frame.ToMat();

        if (_task_args->vehicle_plate_source->veh_info_ptr->lp_info_ptr->lp_status == PLATE_YES)
        {
            auto vehicle_location_plate = _task_args->vehicle_plate_source->veh_info_ptr->lp_info_ptr->location;
            auto vehicle_location_cv_plate = cv::Rect(vehicle_location_plate.x_, vehicle_location_plate.y_,
                                                      vehicle_location_plate.width_, vehicle_location_plate.height_);
            auto veh_plate = original(vehicle_location_cv_plate);
            std::string filename_veh_plate = getSaveTo() + "/" + "VI_" + std::to_string(getIndex()) + ".jpg";
            cv::imwrite(filename_veh_plate, veh_plate);

            _already_save_image.emplace_back(filename_veh_plate);
            _messageData->image3_path = filename_veh_plate;
            _messageData->mage_cap_time3 = timestampToDataTime((long)frame_data_iii.timestamp / 1000, formater);
        }

        auto vehicle_location = _task_args->vehicle_plate_source->veh_info_ptr->location;
        auto vehicle_location_cv =
            cv::Rect(vehicle_location.x_, vehicle_location.y_, vehicle_location.width_, vehicle_location.height_);
        auto veh = original(vehicle_location_cv);
        cv::resize(veh, veh, cv::Size(original_i.cols, original_i.rows));
        std::string filename_veh = getSaveTo() + "/" + "VI_" + std::to_string(getIndex()) + ".jpg";
        cv::imwrite(filename_veh, veh);
        _already_save_image.emplace_back(filename_veh);
        _messageData->image2_path = filename_veh;
        _messageData->mage_cap_time2 = timestampToDataTime((long)frame_data_ii.timestamp / 1000, formater);

        std::vector<cv::Mat> image_list{original_i, original_ii, original_iii, veh};
        const CameraOSDConfig_t &osdConfig = cameraManagerWeakPtr->getCamera(channel_id)->getCameraOsdConfig();

        cv::Mat osd_img;
        ret = generate_stitch_image_d2(image_list, osd_img, osdConfig);
        if (ret != 0)
        {
            setErrorMessage("_Run_Implement: in function: generate_stitch_image error:" + std::to_string(ret));
            return false;
        }

        ret = drawTextOnCombineImageSimple(osd_img, osdConfig, _task_args->III_cap_stamp.cap_stamp,
                                           _task_args->vehicle_plate_source->veh_info_ptr);
        if (ret != 0)
        {
            setErrorMessage("_Run_Implement: in function: generate_stitch_image error:" + std::to_string(ret));
            return false;
        }

        std::vector<uchar> encodedData;
        std::vector<int> compression_params;
        compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
        compression_params.push_back(90);
        bool ret_encode = cv::imencode(".jpg", osd_img, encodedData, compression_params);
        if (ret_encode && encodedData.size() > 0)
        {
            int quality = int(90 * osdConfig.image_config.image_storage_size * 1024.0 / encodedData.size());
            if (quality > 90)
            {
                quality = std::min(
                    80 + int(10 * osdConfig.image_config.image_storage_size * 1024.0 / encodedData.size()), 100);
            }
            compression_params[1] = quality;
        }
        std::string filename_osd = getSaveTo() + "/" + "VI_" + std::to_string(getIndex()) + ".jpg";
        cv::imwrite(filename_osd, osd_img, compression_params);

        _already_save_image.emplace_back(filename_osd);
        _messageData->image1_path = filename_osd;
        _messageData->mage_cap_time1 = timestampToDataTime((long)frame_data_i.timestamp / 1000, formater);

        if (_task_args->III_cap_stamp.rule)
        {
            _messageData->space_code = _task_args->III_cap_stamp.rule->code;
        }
        _messageData->exception_time = _messageData->mage_cap_time3;
        _messageData->trigger_time = _messageData->mage_cap_time1;
        _messageData->exception_type = _task_args->illegal_type;
        return true;
    }
    catch (std::exception &error)
    {
        setErrorMessage(error.what());
        return false;
    }
    catch (...)
    {
        setErrorMessage("Unknown error.");
        return false;
    }

public:
    inline friend std::ostream &
    operator<<(std::ostream &os, const IllegalCaptureCropTaskNormal &task)
    {
        if (task._task_args)
        {
            os << " _task_args: " << *task._task_args;
        }
        return os;
    }
};

using T_IllegalCaptureCropTask = std::shared_ptr<IllegalCaptureCropBase>;

inline std::unique_ptr<IllegalCaptureCropBase>
IllegalCaptureCropBase::CreateTask(std::shared_ptr<IllegalParkingDetail> task_args)
{
    auto mode = IllegalConfigSL::GetInstance().Pick(task_args->channel)->CommonConfig()->mode;
    if (mode == 0)
    {
        return std::make_unique<IllegalCaptureCropTaskNormal>(task_args);
    }
    else if (mode == 1)
    {
        return std::make_unique<IllegalCaptureCropTask>(task_args);
    }
    else
    {
        throw std::invalid_argument("mode:" + std::to_string(mode) + " unsupported.");
    }
}

class IllegalEvidenceAddress : public ProducerConsumer<T_IllegalCaptureCropTask>
{
public:
    IllegalEvidenceAddress() : ProducerConsumer() {}
    ~IllegalEvidenceAddress() = default;

    void
    Producer(T_IllegalCaptureCropTask product) override
    {
        ProduceItem(std::forward<T_IllegalCaptureCropTask>(product));
    }

public:
    using IllegalUploadFunctionType = std::function<void(T_IllegalCaptureCropTask)>;
    void
    setIllegalUploadFunction(IllegalUploadFunctionType illegalUploadFunctionArg)
    {
        illegalUploadFunction = std::move(illegalUploadFunctionArg);
    }
    IllegalEvidenceAddress &
    setCameraManager(const std::shared_ptr<CameraManager> &CameraManager)
    {
        cameraManager = CameraManager;
        return *this;
    }

private:
    IllegalUploadFunctionType illegalUploadFunction;
    std::shared_ptr<CameraManager> cameraManager;

private:
    void
    ConsumeItem(T_IllegalCaptureCropTask taskUP) override
    {
        if (taskUP && !taskUP->getMessageData()->plate_number.empty())
        {
            auto runRet = taskUP->setCameraManagerWeakPtr(cameraManager.get()).Run();
            taskUP->getIllegalParkingDetail()->resetCache();
            if (!runRet)
            {
                // clean saved images when error
                const auto &saved_list = taskUP->getAlreadySaveImage();
                for (const auto &file_path : saved_list)
                {
                    deleteFile(file_path);
                }
                taskUP->getIllegalParkingDetail()->selfReduce();
            }
            if (illegalUploadFunction)
            {
                illegalUploadFunction(taskUP);
            }
        }
        else
        {
            taskUP->setErrorMessage("Plate number is empty.");
            if (illegalUploadFunction)
            {
                illegalUploadFunction(taskUP);
            }
        }
        taskUP->getIllegalParkingDetail()->resetCache();
    }
};

inline bool
IllegalParkingHistoryRecord::_Item::enough() const noexcept
{
    auto mode = IllegalConfigSL::GetInstance().Pick(channel)->CommonConfig()->mode;
    if (mode == 0)
    {
        if (count == 1)
        {
            return true;
        }
    }
    if (mode == 1)
    {
        if (count == 2)
        {
            return true;
        }
    }
    return false;
}

namespace algorithm
{
static inline double
crossProduct(const cv::Point &p1, const cv::Point &p2, const cv::Point &p3)
{
    return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
}

static inline double
crossProduct(const cv::Point &a, const cv::Point &b)
{
    return a.x * b.y - a.y * b.x;
}

static inline bool
isInsideByVector(const std::vector<cv::Point> &quadrilateral, const cv::Point &point)
{
    int n = quadrilateral.size();
    bool positive = false;
    bool negative = false;
    for (int i = 0; i < n; ++i)
    {
        int j = (i + 1) % n;
        cv::Point edge = cv::Point(quadrilateral[j].x - quadrilateral[i].x, quadrilateral[j].y - quadrilateral[i].y);
        cv::Point vec = cv::Point(point.x - quadrilateral[i].x, point.y - quadrilateral[i].y);
        double cp = crossProduct(edge, vec);
        if (cp > 0)
            positive = true;
        if (cp < 0)
            negative = true;
        if (positive && negative)
            return false;
    }
    return true;
}

static inline bool
isPointOnSegment(const cv::Point &p, const cv::Point &a, const cv::Point &b)
{
    return (p.x >= std::min(a.x, b.x)) && (p.x <= std::max(a.x, b.x)) && (p.y >= std::min(a.y, b.y)) &&
           (p.y <= std::max(a.y, b.y));
}

inline bool
areSegmentsIntersecting(const cv::Point &p1, const cv::Point &p2, const cv::Point &q1, const cv::Point &q2)
{
    cv::Rect rect1(std::min(p1.x, p2.x), std::min(p1.y, p2.y), abs(p1.x - p2.x), abs(p1.y - p2.y));
    cv::Rect rect2(std::min(q1.x, q2.x), std::min(q1.y, q2.y), abs(q1.x - q2.x), abs(q1.y - q2.y));
    if (!(rect1 & rect2).area())
        return false;

    double cp1 = crossProduct(p1, p2, q1);
    double cp2 = crossProduct(p1, p2, q2);
    double cp3 = crossProduct(q1, q2, p1);
    double cp4 = crossProduct(q1, q2, p2);

    if (cp1 * cp2 < 0 && cp3 * cp4 < 0)
        return true;

    if (cp1 == 0 && isPointOnSegment(q1, p1, p2))
        return true;
    if (cp2 == 0 && isPointOnSegment(q2, p1, p2))
        return true;
    if (cp3 == 0 && isPointOnSegment(p1, q1, q2))
        return true;
    if (cp4 == 0 && isPointOnSegment(p2, q1, q2))
        return true;

    return false;
}

} // namespace algorithm

} // namespace sonli::illegal
#endif // ILLEGAL_H

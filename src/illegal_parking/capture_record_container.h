#ifndef ACL_DET_YOLO_CAPTURE_RECORD_CONTAINER_H
#define ACL_DET_YOLO_CAPTURE_RECORD_CONTAINER_H
#include "data_type.h"
#include "parking_config.h"
#include "../interfaces/aih_type.h"
#include "../interfaces/aih_extern.h"
#include <memory>
#include <ostream>
#include <unordered_set>

namespace sonli
{

namespace illegal_parking
{

enum class EIllegalCase
{
    _PARALLEL_PARKING = 0, // 线外停车
    _BAN_EXTENT_PARKING, // 禁停区域停车
    _END_POINTS_PARKING, // 道路两端停车

    _PARKING_ONTO_LINE, // 平行压线停车
    _ANGLE_PARKING, // 斜停压线停车
    _VERTICAL_PARKING, // 垂直压线停车
    _OPPOSITE_PARKING, // 逆向停车

    _CROSS_PARKING, // 跨位停车
    _INTO_ILLEGAL_TIME, // 禁停时段停车
    _UNKNOWN // 未知
};


std::string
IllegalCaseStr(EIllegalCase eIllegalCase);


std::string
ILLEgalCaseCode(EIllegalCase eIllegalCase);


class IllegalCaptureDetail
{
public:
    aih::TimeInfo cap_stamp {};
    BerthEventConfig rule;
    VehInfo veh_info = {}; // 保存坐标信息
    std::string event;

    bool
    existCaptureEviData() const noexcept
    {
        return cap_stamp.pts_ != 0;
    }

    friend std::ostream &
    operator<<(std::ostream &os, const IllegalCaptureDetail &captureDetail)
    {
        os << "cap_stamp: " << captureDetail.cap_stamp.local_time_ << " veh_info_ptr: "
            << (captureDetail.veh_info.lp_info.lp_number.lp_str != "-" &&
                !captureDetail.veh_info.lp_info.lp_number.lp_str.empty()
                    ? captureDetail.veh_info.lp_info.lp_number.lp_str
                    : "track id:" + std::to_string(captureDetail.veh_info.veh_track_id));
        os << " event: <ctl:" << captureDetail.rule.control_id << ", evt:" << captureDetail.rule.event_id << ">";
        return os;
    }

    void
    resetCache()
    {
        cap_stamp.pts_ = 0;
        cap_stamp.local_time_ = 0;
        event.clear();
    }
};

struct IllegalParkingLogicKeepFrameEvent
{
    std::shared_ptr<IllegalCaptureDetail> cap_detail;
};

struct VehicleIllegalData final
{
    int ctrl_id = -1;
    int event_id = -1;
    std::string exception_type;
    aih::TimeInfo exception_time;
    aih::TimeInfo trigger_time;
    std::string data_type;
    EIllegalCase illegal_case = EIllegalCase::_UNKNOWN;

    std::string plate_number{"-"};
    SLLPColor_t plate_color;
    std::string plate_confidence;
    std::string vehicle_category;

    aih::TimeInfo mage_cap_time1;
    aih::TimeInfo mage_cap_time2;
    aih::TimeInfo mage_cap_time3;

    std::shared_ptr<IllegalCaptureDetail> I_cap_stamp = nullptr; // 第一次抓拍
    std::shared_ptr<IllegalCaptureDetail> II_cap_stamp = nullptr; // 第二次抓拍
    std::shared_ptr<IllegalCaptureDetail> III_cap_stamp = nullptr; // 第三次抓拍
    std::shared_ptr<IllegalCaptureDetail> vehicle_plate_source = nullptr;

    bool justUpdated = false;

    friend std::ostream &
    operator<<(std::ostream &os, const VehicleIllegalData &obj)
    {
         os
               << "ctrl_id: [" << obj.ctrl_id << "]"
               << " event_id: [" << obj.event_id << "]"
               << " exception_type: [" << obj.exception_type << "]"
               << " exception_time: [" << obj.exception_time.pts_  << "]"
               << " trigger_time: [" << obj.trigger_time.pts_ << "]"
               << " data_type: [" << obj.data_type << "]"
               << " illegal_case: [" << IllegalCaseStr(obj.illegal_case) << "]"
               << " plate_number: [" << obj.plate_number << "]"
               << " plate_color: [" << obj.plate_color << "]"
               << " plate_confidence: [" << obj.plate_confidence << "]"
               << " vehicle_category: [" << obj.vehicle_category << "]";
        if (obj.I_cap_stamp && obj.I_cap_stamp->existCaptureEviData())
        {
            os << " mage_cap_time1: [" << obj.mage_cap_time1.pts_ << "]";
            os << " I_cap_stamp: [" << *obj.I_cap_stamp << "]";
        }
        if (obj.II_cap_stamp && obj.II_cap_stamp->existCaptureEviData())
        {
            os << " mage_cap_time2: [" << obj.mage_cap_time2.pts_ << "]";
            os << " II_cap_stamp: [" << *obj.II_cap_stamp << "]";
        }
        if (obj.III_cap_stamp && obj.III_cap_stamp->existCaptureEviData())
        {
            os << " mage_cap_time3: [" << obj.mage_cap_time3.pts_ << "]";
            os << " III_cap_stamp: [" << *obj.III_cap_stamp << "]";
        }
        if (obj.vehicle_plate_source && obj.vehicle_plate_source->existCaptureEviData())
        {
            os << " vehicle_plate_source: [" << *obj.vehicle_plate_source << "]";
        }
        return os;
    }

    std::string
    to_string() const
    {
        std::ostringstream ss;
        ss << *this;
        return ss.str();
    }

};

class CaptureRecordContainer
{
protected:
    explicit
    CaptureRecordContainer(EIllegalCase illegal_case);

    virtual ~CaptureRecordContainer() = default;
public:
    virtual bool
    WasReported() const noexcept;

    CaptureRecordContainer &
    setNoReported() noexcept;

    CaptureRecordContainer &
    setReported() noexcept;

protected:
    int reported = 0; // 是否已经违停

public:
    EIllegalCase illegal_case; // 违停类型
    std::string illegal_type; // 违停类型
    std::shared_ptr<IllegalCaptureDetail> I_cap_stamp = nullptr; // 第一次抓拍
    std::shared_ptr<IllegalCaptureDetail> II_cap_stamp = nullptr; // 第二次抓拍
    std::shared_ptr<IllegalCaptureDetail> III_cap_stamp = nullptr; // 第三次抓拍
    std::shared_ptr<IllegalCaptureDetail> IIII_cap_stamp = nullptr; // 第四次抓拍
    std::shared_ptr<IllegalCaptureDetail> vehicle_plate_source = nullptr;
    unsigned long need_cd = 0; // 两次违规抓拍需要的cd
    bool justUpdated = false;

    virtual bool
    isSufficientEvidence() const noexcept;

    friend std::ostream &
    operator<<(std::ostream &os, const CaptureRecordContainer &parkingDetail);

    virtual void
    UpdateCapStamp(const BerthEventConfig &rule, const VehInfo &veh, const aih::TimeInfo &time_info);

    void
    resetCache();
};

class CaptureRecordContainerNormal : public CaptureRecordContainer
{
public:
    explicit
    CaptureRecordContainerNormal(EIllegalCase illegal_case);

    ~CaptureRecordContainerNormal() override = default;

    bool
    isSufficientEvidence() const noexcept override;

    void
    UpdateCapStamp(const BerthEventConfig &rule, const VehInfo &veh, const aih::TimeInfo &time_info) override;
};


struct TimeinfoCompare
{
    bool
    operator()(const aih::TimeInfo &a, const aih::TimeInfo &b) const
    {
        return a.pts_ == b.pts_;
    }
};

struct PersonHash
{
    size_t
    operator()(const aih::TimeInfo &p) const
    {
        size_t h1 = std::hash<uint64_t>{}(p.pts_);
        return h1;
    }
};

class MultipleIllegalCaptureDetail
{
public:
    std::vector<std::shared_ptr<CaptureRecordContainer> > multiple_illegal; // 多种违停共存
    std::shared_ptr<CaptureRecordContainer>
    Find(EIllegalCase eIllegalCase);

    std::unordered_set<aih::TimeInfo, PersonHash, TimeinfoCompare>
    GetUsedTimes();
};


}


}


#endif //ACL_DET_YOLO_CAPTURE_RECORD_CONTAINER_H
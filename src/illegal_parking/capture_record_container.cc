#include "capture_record_container.h"

#include "data_type.h"
#include "../utils/sl_logger.h"
#include "../utils/struct_2_string.h"

#include <algorithm> // for std::max
#include <memory>
#include <set>
#include <set>
#include <set>
#include <set>
#include <unordered_set>

namespace sonli
{
namespace illegal_parking
{
static std::string IllegalCaseArrayPreDef[9] = {
    "PARALLEL_PARKING", "BAN_EXTENT_PARKING", "END_POINTS_PARKING", "PARKING_ONTO_LINE", "ANGLE_PARKING",
    "VERTICAL_PARKING", "OPPOSITE_PARKING", "CROSS_PARKING", "INTO_ILLEGAL_TIME"};

static std::string IllegalCaseCode[9] = {"01", "03", "02", "06", "07", "05", "09", "08", "04"};

// class IllegalCaptureDetail
// {
// public:
//     aih::TimeInfo cap_stamp;
//     aih::SingleBerthAlarmEvent rule;
//     VehInfo veh_info = {}; // 保存坐标信息
//     std::string event;
//
//     bool
//     hasData() const noexcept
//     {
//         return cap_stamp.pts_ != 0;
//     }
//
//     friend std::ostream &
//     operator<<(std::ostream &os, const IllegalCaptureDetail &captureDetail)
//     {
//         os << "cap_stamp: " << captureDetail.cap_stamp.pts_ << " veh_info_ptr: "
//             << (captureDetail.veh_info.lp_info.lp_number.lp_str != "-" &&
//                 !captureDetail.veh_info.lp_info.lp_number.lp_str.empty()
//                     ? captureDetail.veh_info.lp_info.lp_number.lp_str
//                     : std::to_string(captureDetail.veh_info.veh_track_id));
//         os << " event: " << captureDetail.event;
//         return os;
//     }
//
//     void
//     resetCache()
//     {
//         cap_stamp.pts_ = 0;
//         cap_stamp.local_time_ = 0;
//         event.clear();
//     }
// };


std::string
IllegalCaseStr(EIllegalCase eIllegalCase)
{
    return IllegalCaseArrayPreDef[static_cast<int>(eIllegalCase)];
}

std::string
ILLEgalCaseCode(EIllegalCase eIllegalCase)
{
    return IllegalCaseCode[static_cast<int>(eIllegalCase)];
}

CaptureRecordContainer::CaptureRecordContainer(EIllegalCase illegal_case)
    : illegal_case(illegal_case),
      I_cap_stamp(std::make_shared<IllegalCaptureDetail>()),
      II_cap_stamp(std::make_shared<IllegalCaptureDetail>()),
      III_cap_stamp(std::make_shared<IllegalCaptureDetail>()),
      IIII_cap_stamp(std::make_shared<IllegalCaptureDetail>())
{

}

// IllegalParkingDetail 实现
bool
CaptureRecordContainer::WasReported() const noexcept
{
    return reported;
}

CaptureRecordContainer &
CaptureRecordContainer::setNoReported() noexcept
{
    reported = 0;
    return *this;
}

CaptureRecordContainer &
CaptureRecordContainer::setReported() noexcept
{
    reported = 1;
    return *this;
}

bool
CaptureRecordContainer::isSufficientEvidence() const noexcept
{
    return IIII_cap_stamp and IIII_cap_stamp->existCaptureEviData();
}

void
CaptureRecordContainer::UpdateCapStamp(const BerthEventConfig &rule, const VehInfo &veh, const aih::TimeInfo &time_info)
{
    // SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info("UpdateCapStamp: Is need judge again: {}", isNeedJudgeAgain());
    if (WasReported())
    {
        return;
    }
    auto waittime = rule.GetNextFrameCaptureTime();
    unsigned long latest_timestamp = 0; // 最后抓拍时间


    // SL_LOG_ROTATE_INFO( "current Time {}",time_info.pts_);

    if (I_cap_stamp->cap_stamp.pts_ != 0)
    {
        latest_timestamp = I_cap_stamp->cap_stamp.pts_;
        SL_LOG_ROTATE_INFO( "Illegal parking UpdateCapStamp I_cap_stamp: {}", I_cap_stamp->cap_stamp.pts_);
    }
    else
    {
        I_cap_stamp->cap_stamp = time_info;
        I_cap_stamp->rule = rule;
        I_cap_stamp->veh_info = veh;
        justUpdated = true;
       SL_LOG_ROTATE_INFO( "Illegal parking UpdateCapStamp I_cap_stamp: {}, and need wait time: {}, last time:{}",
            TO_STRING(*I_cap_stamp), waittime, latest_timestamp);
        return;
    }

    if (II_cap_stamp->cap_stamp.pts_ != 0)
    {
        latest_timestamp = II_cap_stamp->cap_stamp.pts_;
        SL_LOG_ROTATE_INFO( "Illegal parking UpdateCapStamp II_cap_stamp: {}", II_cap_stamp->cap_stamp.pts_);
    }
    else
    {
        if ((time_info.pts_ - latest_timestamp) > waittime)
        {
            II_cap_stamp->cap_stamp = time_info;
            II_cap_stamp->rule = rule;
            II_cap_stamp->veh_info = veh;
            justUpdated = true;
            SL_LOG_ROTATE_INFO("Illegal parking UpdateCapStamp II_cap_stamp: {}", TO_STRING(*II_cap_stamp));
        }
        return;
    }

    if (III_cap_stamp->cap_stamp.pts_ != 0)
    {
        latest_timestamp = III_cap_stamp->cap_stamp.pts_;
    }
    else
    {
        if ((time_info.pts_ - latest_timestamp)  > waittime)
        {
            III_cap_stamp->cap_stamp = time_info;
            III_cap_stamp->rule = rule;
            III_cap_stamp->veh_info = veh;
            justUpdated = true;
            SL_LOG_ROTATE_INFO("Illegal parking UpdateCapStamp III_cap_stamp: {}", TO_STRING(*III_cap_stamp));
        }
        return;
    }

    if (IIII_cap_stamp->cap_stamp.pts_ == 0)
    {
        if ((time_info.pts_ - latest_timestamp)  > waittime)
        {
            IIII_cap_stamp->cap_stamp = time_info;
            IIII_cap_stamp->rule = rule;
            IIII_cap_stamp->veh_info = veh;
            justUpdated = true;
            SL_LOG_ROTATE_INFO("Illegal parking UpdateCapStamp IIII_cap_stamp: {}", TO_STRING(*IIII_cap_stamp));
        }
    }
}

void
CaptureRecordContainer::resetCache()
{
    std::cout << "reset illegal parking Cache" << std::endl;
    vehicle_plate_source = nullptr;
    IIII_cap_stamp->resetCache();
    III_cap_stamp->resetCache();
    II_cap_stamp->resetCache();
    I_cap_stamp->resetCache();
}

CaptureRecordContainerNormal::CaptureRecordContainerNormal(EIllegalCase illegal_case)
    : CaptureRecordContainer(illegal_case)
{
    reported = 0;
}

std::ostream &
operator<<(std::ostream &os, const CaptureRecordContainer &parkingDetail)
{
    // 实现输出逻辑
    os << "IllegalCase: " << static_cast<int>(parkingDetail.illegal_case)
        << ", Type: " << parkingDetail.illegal_type;
    return os;
}

bool
CaptureRecordContainerNormal::isSufficientEvidence() const noexcept
{
    return III_cap_stamp and III_cap_stamp->existCaptureEviData();
}

void
CaptureRecordContainerNormal::UpdateCapStamp(const BerthEventConfig &rule, const VehInfo &veh,
                                             const aih::TimeInfo &time_info)
{
    // SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info("UpdateCapStamp: Is need judge again: {}", isNeedJudgeAgain());
    if (WasReported())
    {
        return;
    }
    auto waittime = rule.GetNextFrameCaptureTime();
    unsigned long latest_timestamp = 1; // 最后抓拍时间
    if (I_cap_stamp->cap_stamp.local_time_ != 0)
    {
        latest_timestamp = I_cap_stamp->cap_stamp.local_time_/1000;
    }
    else
    {
        I_cap_stamp->cap_stamp = time_info;
        I_cap_stamp->rule = rule;
        I_cap_stamp->veh_info = veh;
        justUpdated = true;
        SL_LOG_ROTATE_INFO("Illegal parking UpdateCapStamp I_cap_stamp: {}, wait time:{}, last time:{}",
            TO_STRING(*I_cap_stamp), waittime, latest_timestamp);
        return;
    }

    if (II_cap_stamp->cap_stamp.pts_ != 0)
    {
        latest_timestamp = II_cap_stamp->cap_stamp.local_time_ / 1000;
    }
    else
    {
        if ((time_info.local_time_/1000 - latest_timestamp) > waittime)
        {
            II_cap_stamp->cap_stamp = time_info;
            II_cap_stamp->rule = rule;
            II_cap_stamp->veh_info = veh;
            justUpdated = true;
            SL_LOG_ROTATE_INFO("Illegal parking UpdateCapStamp II_cap_stamp: {}, wait time:{}, last time:{}",
             TO_STRING(*II_cap_stamp), waittime, latest_timestamp);
        }
        return;
    }

    if (III_cap_stamp->cap_stamp.local_time_ == 0)
    {
        if ((time_info.local_time_/1000 - latest_timestamp)  > waittime)
        {
            III_cap_stamp->cap_stamp = time_info;
            III_cap_stamp->rule = rule;
            III_cap_stamp->veh_info = veh;
            justUpdated = true;
            SL_LOG_ROTATE_INFO("Illegal parking UpdateCapStamp III_cap_stamp: {}, wait time:{}, last time:{}",
            TO_STRING(*III_cap_stamp), waittime, latest_timestamp);
        }
    }
}

std::shared_ptr<CaptureRecordContainer>
MultipleIllegalCaptureDetail::Find(EIllegalCase eIllegalCase)
{
    for (auto &var : multiple_illegal)
    {
        if (var->illegal_case == eIllegalCase)
        {
            return var;
        }
    }
    std::shared_ptr<CaptureRecordContainer> tmp = std::make_shared<CaptureRecordContainerNormal>(eIllegalCase);
    tmp->illegal_type = ILLEgalCaseCode(eIllegalCase);
    tmp->illegal_case = eIllegalCase;
    multiple_illegal.emplace_back(tmp);
    return tmp;
}



std::unordered_set<aih::TimeInfo, PersonHash, TimeinfoCompare>
MultipleIllegalCaptureDetail::GetUsedTimes()
{
    std::unordered_set<aih::TimeInfo, PersonHash, TimeinfoCompare> result;
    for (auto &var : multiple_illegal)
    {
        if (var->WasReported())
        {
            continue;
        }
        if (var->I_cap_stamp->existCaptureEviData())
        {
            result.insert(var->I_cap_stamp->cap_stamp);
        }
        if (var->II_cap_stamp->existCaptureEviData())
        {
            result.insert(var->II_cap_stamp->cap_stamp);
        }
        if (var->III_cap_stamp->existCaptureEviData())
        {
            result.insert(var->III_cap_stamp->cap_stamp);
        }
        if (var->IIII_cap_stamp->existCaptureEviData())
        {
            result.insert(var->IIII_cap_stamp->cap_stamp);
        }
    }
    return result;
}

} // namespace illegal_parking
} // namespace sonli
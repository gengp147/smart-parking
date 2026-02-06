#include "illegal_parking_algorithm.h"

#include "../interfaces/aih_extern.h"
#include "../utils/sl_logger.h"
#include "capture_record_container.h"
#include "illegal_history_record.h"
#include <utility>

namespace sonli
{

std::shared_ptr<illegal_parking::MultipleIllegalCaptureDetail> &
CacheVehInfo::getMultipleIllegalCaptureDetail()
{
    if (multiple_illegal_capture_detail == nullptr)
    {
        multiple_illegal_capture_detail = std::make_shared<illegal_parking::MultipleIllegalCaptureDetail>();
    }
    return multiple_illegal_capture_detail;
}

namespace illegal_parking
{

extern void
CrossParkingDetectPerFrame(CacheVehInfo *vehicle_cache, const std::shared_ptr<ParkingInfoVec> &parking_info_ptr,
                           const BerthEventConfig *rules);

extern void
ParkingTimeCheck(CacheVehInfo *vehicle_cache, const std::shared_ptr<ParkingInfoVec> &parking_info_ptr,
                 const BerthEventConfig *rules);

extern void
AngleBetweenCarAndSpace(CacheVehInfo *vehicle_cache, const std::shared_ptr<ParkingInfoVec> &parking_info_ptr,
                        const BerthEventConfig *rules);

class IllegalCaptureCropBase
{
public:
    explicit IllegalCaptureCropBase(std::shared_ptr<CaptureRecordContainer> captureRecordContainer,
                                    std::shared_ptr<IllegalParkingHistoryRecord> parkingHistoryRecord)
        : _messageData(std::make_shared<VehicleIllegalData>()),
          _capture_record(std::move(captureRecordContainer)),
          _illegal_report_history(std::move(parkingHistoryRecord))
    {
    }

    virtual ~IllegalCaptureCropBase() = default;

    bool
    Run()
    {
        if (_messageData->plate_number.empty() or _messageData->plate_number == "-")
        {
            setErrorMessage("plate number is empty");
            return false;
        }
        if (!_capture_record)
        {
            setErrorMessage("Task args is nullptr, invalid arguments.");
            return false;
        }

        if (!_capture_record->III_cap_stamp->existCaptureEviData())
        {
            setErrorMessage("_capture_record->III_cap_stamp->hasData() return false");
            return false;
        }
        _capture_record->vehicle_plate_source = _capture_record->III_cap_stamp;

        if (!_capture_record->II_cap_stamp->existCaptureEviData())
        {
            setErrorMessage("_capture_record->II_cap_stamp->hasData() return false");
            return false;
        }
        if (_capture_record->II_cap_stamp->veh_info.lp_info.lp_number.vlp_rec_confidence >
            _capture_record->vehicle_plate_source->veh_info.lp_info.lp_number.vlp_rec_confidence)
        {
            _capture_record->vehicle_plate_source = _capture_record->II_cap_stamp;
        }
        if (!_capture_record->I_cap_stamp->existCaptureEviData())
        {
            setErrorMessage("_capture_record->I_cap_stamp->hasData() return false");
            return false;
        }
        if (_capture_record->I_cap_stamp->veh_info.lp_info.lp_number.vlp_rec_confidence >
            _capture_record->vehicle_plate_source->veh_info.lp_info.lp_number.vlp_rec_confidence)
        {
            _capture_record->vehicle_plate_source = _capture_record->I_cap_stamp;
        }
        auto f = filter();
        if (f)
        {
            setErrorMessage("have filter, continue");
            return false;
        }
        _messageData->mage_cap_time3 = _capture_record->III_cap_stamp->cap_stamp;
        _messageData->mage_cap_time2 = _capture_record->II_cap_stamp->cap_stamp;
        _messageData->mage_cap_time1 = _capture_record->I_cap_stamp->cap_stamp;

        _messageData->trigger_time = _messageData->mage_cap_time1;
        _messageData->exception_type = _capture_record->illegal_type;
        _messageData->ctrl_id = _capture_record->vehicle_plate_source->rule.control_id;
        _messageData->event_id = _capture_record->vehicle_plate_source->rule.event_id;
        _messageData->vehicle_plate_source = _capture_record->vehicle_plate_source;
        _messageData->illegal_case = _capture_record->illegal_case;

        _messageData->I_cap_stamp = _capture_record->I_cap_stamp;
        _messageData->II_cap_stamp = _capture_record->II_cap_stamp;
        _messageData->III_cap_stamp = _capture_record->III_cap_stamp;

        return true;
    }

    bool
    fill()
    {
        if (!_capture_record)
        {
            setErrorMessage("Task args is nullptr, invalid arguments.");
            return false;
        }

        if (_capture_record->I_cap_stamp)
        {
            if (_capture_record->I_cap_stamp->existCaptureEviData())
            {
                _messageData->mage_cap_time1 = _capture_record->I_cap_stamp->cap_stamp;
                _capture_record->vehicle_plate_source = _capture_record->I_cap_stamp;
                _messageData->I_cap_stamp = _capture_record->I_cap_stamp;
                // SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info("I captured");
            }
        }
        else
        {
            // SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info("I not captured");
            return false;
        }

        if (_capture_record->II_cap_stamp)
        {
            if (_capture_record->II_cap_stamp->existCaptureEviData())
            {
                _messageData->mage_cap_time2 = _capture_record->II_cap_stamp->cap_stamp;
                _capture_record->vehicle_plate_source = _capture_record->II_cap_stamp;
                _messageData->II_cap_stamp = _capture_record->II_cap_stamp;
                // SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info("II captured");
            }
        }

        if (_capture_record->III_cap_stamp)
        {
            if (_capture_record->III_cap_stamp->existCaptureEviData())
            {
                _messageData->mage_cap_time3 = _capture_record->III_cap_stamp->cap_stamp;
                _capture_record->vehicle_plate_source = _capture_record->III_cap_stamp;
                _messageData->III_cap_stamp = _capture_record->III_cap_stamp;
                SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info("III captured");
            }
        }

        if (_capture_record->vehicle_plate_source)
        {
            _messageData->trigger_time = _messageData->mage_cap_time1;
            _messageData->exception_type = _capture_record->illegal_type;
            _messageData->ctrl_id = _capture_record->vehicle_plate_source->rule.control_id;
            _messageData->event_id = _capture_record->vehicle_plate_source->rule.event_id;
            _messageData->vehicle_plate_source = _capture_record->vehicle_plate_source;
            _messageData->illegal_case = _capture_record->illegal_case;
        }
        else
        {
            SonLiLogger::getInstance()
                .getLogger(SonLiLogger::RotateLogger)
                ->info("_capture_record->vehicle_plate_source is nullptr");
            return false;
        }
        return true;
    }

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

    std::shared_ptr<VehicleIllegalData>
    getMessageData()
    {
        return _messageData;
    }

    bool
    filter()
    {
        std::unique_ptr<IllegalParkingHistoryRecord::_Item> opt(new IllegalParkingHistoryRecord::_Item(
            _messageData->plate_number, static_cast<int>(_capture_record->vehicle_plate_source->veh_info.veh_track_id),
            _capture_record->vehicle_plate_source->veh_info.veh_timestamp, _capture_record->illegal_type));

        auto imp_i = _illegal_report_history->find(opt);
        if (!imp_i)
        {
            _illegal_report_history->addNew(std::move(opt));
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

protected:
    static long
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

    std::shared_ptr<VehicleIllegalData> _messageData;
    std::string _error_message;

public:
    static long _index;

protected:
    std::shared_ptr<CaptureRecordContainer> _capture_record;
    std::shared_ptr<IllegalParkingHistoryRecord> _illegal_report_history;
};

long IllegalCaptureCropBase::_index = 0;

IllegalParkingAlgorithm::IllegalParkingAlgorithm()
{
    _illegal_parking_history_record = std::make_shared<IllegalParkingHistoryRecord>();
}

std::shared_ptr<IllegalParkingDetectionResult>
IllegalParkingAlgorithm::IllegalParkingEntry(const std::vector<CacheVehInfo *> &refreshed,
                                             const std::shared_ptr<ParkingInfoVec> &parking_info_ptr,
                                             const std::shared_ptr<NoParkingAreaConfigVec> &no_parking_area_configs)
{
    auto result = std::make_shared<IllegalParkingDetectionResult>();
    if (refreshed.empty())
    {
        return result;
    }

    for (const auto veh : refreshed)
    {
        for (const auto &illegal_item : veh->getMultipleIllegalCaptureDetail()->multiple_illegal)
        {
            if (illegal_item->isSufficientEvidence() && illegal_item->WasReported())
            {
                illegal_item->resetCache();
            }
        }

        if (no_parking_area_configs->this_frame_check_)
        {
            const BerthEventConfig *rules = nullptr;
            if (veh->veh_in_out_var.is_report_in && veh->veh_infos.back().veh_chassis.space_res == SPACE_IN)
            {
                if (veh->veh_infos.rbegin()->veh_chassis.space_id != -1)
                {
                    const auto &occupy_parking =
                        parking_info_ptr->parking_info[veh->veh_infos.rbegin()->veh_chassis.space_id];
                    rules = std::addressof(occupy_parking.berth_event_list);
                }
                if (!rules)
                {
                    continue;
                }
                ParkingTimeCheck(veh, parking_info_ptr, rules);
            }
            // 不规范停车
            if (veh->veh_infos.back().veh_chassis.space_res == SPACE_IN and no_parking_area_configs->this_frame_check_)
            {
                if (rules && veh->veh_in_out_var.is_standstill)
                {
                    try
                    {
                        // IllegalOppositeParkingCheck(vehicles, vehicle_cache, illegal_cache, illegal_fn);
                        if (rules->config_detail.alarm_events.bReportCrossBerthError)
                        {
                            CrossParkingDetectPerFrame(veh, parking_info_ptr, rules);
                        }

                        if (rules->config_detail.alarm_events.bReportCrossLineError ||
                            rules->config_detail.alarm_events.bReportVerticalParkingError ||
                            rules->config_detail.alarm_events.bReportDiagonalParkingError ||
                            rules->config_detail.alarm_events.bReportReverseParkingError)
                        {
                            AngleBetweenCarAndSpace(veh, parking_info_ptr, rules);
                        }
                    }
                    catch (std::exception &error)
                    {
                        SonLiLogger::getInstance()
                            .getLogger(SonLiLogger::RotateLogger)
                            ->warn("illegal parking code: {}", error.what());
                        return result;
                    }
                }
            }
            // 禁停区域停车
            else
            {
                if (no_parking_area_configs->this_frame_check_ &&
                    !no_parking_area_configs->no_parking_area_configs_.empty())
                {
                    JudgeIllegalNoParkingArea(no_parking_area_configs, *veh);
                }
            }
        }

        for (const auto &illegal_item : veh->getMultipleIllegalCaptureDetail()->multiple_illegal)
        {
            if (!illegal_item->WasReported() && illegal_item->isSufficientEvidence())
            {
                auto illegal_evidence =
                    std::make_shared<IllegalCaptureCropBase>(illegal_item, _illegal_parking_history_record);
                auto message = illegal_evidence->getMessageData();
                if (veh->lp_info.lp_number.lp_str.empty() or veh->lp_info.lp_number.lp_str == "-")
                {
                    // SonLiLogger::getInstance()
                    //     .getLogger(SonLiLogger::RotateLogger)
                    //     ->warn("Illegal record item continue because lp_number is empty");
                    continue;
                }
                message->plate_number = veh->lp_info.lp_number.lp_str;
                message->plate_color = veh->lp_info.lp_color;
                if (message->plate_color == 0)
                {
                    message->plate_color = SL_PLATE_COLOR_BLUE;
                }
                message->plate_confidence = std::to_string(veh->lp_info.lp_number.vlp_rec_confidence * 100);
                if (!illegal_evidence->Run())
                {
                    auto error_message = illegal_evidence->getErrorMessage();
                    continue;
                }
                illegal_item->setReported();
                result->vehicle_illegal_data.emplace_back(message);
            }
            else if (illegal_item->justUpdated)
            {
                SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->info("generate logic keep frame event");
                illegal_item->justUpdated = false;
                auto illegal_evidence =
                    std::make_shared<IllegalCaptureCropBase>(illegal_item, _illegal_parking_history_record);
                auto message = illegal_evidence->getMessageData();
                message->plate_number = veh->lp_info.lp_number.lp_str;
                message->plate_color = veh->lp_info.lp_color;
                message->plate_confidence = std::to_string(veh->lp_info.lp_number.vlp_rec_confidence * 100);
                if (!illegal_evidence->fill())
                {
                    continue;
                }
                result->logic_keep_frame_events.emplace_back(message);
            }
            auto used_times = veh->getMultipleIllegalCaptureDetail()->GetUsedTimes();
            result->illegal_parking_time_set.insert(used_times.begin(), used_times.end());
        }
    }

    return result;
}

} // namespace illegal_parking
} // namespace sonli
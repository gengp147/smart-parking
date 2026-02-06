#ifndef ACL_DET_YOLO_ILLEGAL_HISTORY_RECORD_H
#define ACL_DET_YOLO_ILLEGAL_HISTORY_RECORD_H
#include <memory>
#include <string>
#include <vector>


namespace sonli
{
namespace illegal_parking
{

class IllegalParkingHistoryRecord
{
public:
    struct _Item
    {
        _Item(std::string vehiclePlateNumber, int vehicleTrackId, unsigned long parkingTimestamp,
              const std::string &illegal_case_a)
            : vehicle_plate_number(std::move(vehiclePlateNumber)),
              vehicle_track_id(vehicleTrackId),
              parking_timestamp(parkingTimestamp),
              illegal_case(illegal_case_a)
        {
        }
        std::string vehicle_plate_number;
        int vehicle_track_id = -1;
        unsigned long parking_timestamp = 0;
        std::string illegal_case;

    public:
        _Item *
        selfAdd() noexcept;
        bool
        enough() const noexcept;
        bool
        operator==(const _Item &o) const noexcept;
        bool
        operator!=(const _Item &o) const noexcept;

    private:
        int count = 0;
    };

    _Item *
    find(const std::unique_ptr<_Item> &obj) const noexcept;

    IllegalParkingHistoryRecord *
    addNew(std::unique_ptr<_Item> &&new_obj);

private:
    std::vector<std::unique_ptr<_Item>> _history;
};

}
}

#endif //ACL_DET_YOLO_ILLEGAL_HISTORY_RECORD_H
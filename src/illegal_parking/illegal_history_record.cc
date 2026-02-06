#include "illegal_history_record.h"
#include <string>

sonli::illegal_parking::IllegalParkingHistoryRecord::_Item *
sonli::illegal_parking::IllegalParkingHistoryRecord::_Item::selfAdd() noexcept
{
    count++;
    return this;
}

bool
sonli::illegal_parking::IllegalParkingHistoryRecord::_Item::enough() const noexcept
{
    return count >= 1;
}

bool
sonli::illegal_parking::IllegalParkingHistoryRecord::_Item::operator==(const _Item &o) const noexcept
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
sonli::illegal_parking::IllegalParkingHistoryRecord::_Item::operator!=(const _Item &o) const noexcept
{
    return !(*this == o);
}

sonli::illegal_parking::IllegalParkingHistoryRecord::_Item *
sonli::illegal_parking::IllegalParkingHistoryRecord::find(const std::unique_ptr<_Item> &obj) const noexcept
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

sonli::illegal_parking::IllegalParkingHistoryRecord *
sonli::illegal_parking::IllegalParkingHistoryRecord::addNew(std::unique_ptr<_Item> &&new_obj)
{
    _history.emplace_back(std::forward<std::unique_ptr<_Item>>(new_obj));
    return this;
}
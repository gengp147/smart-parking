#ifndef _INOUT_VEHPARKING_INFO_H_
#define _INOUT_VEHPARKING_INFO_H_

#include "data_type.h"

namespace sonli
{

struct StatisticsVelInSpace
{
    explicit
    StatisticsVelInSpace(const VehInfo &vehinfoarg)
        : firstRefreshTime(vehinfoarg.veh_timestamp),
          nearRefreshTime(firstRefreshTime),
          space_rate_cache(121),
          counter(1),
          vehinfo(vehinfoarg),
          waitTimes(-1)
    {
        space_rate_cache.enqueue(vehinfoarg.veh_chassis.space_rat);
    }

    void
    Refresh(const VehInfo &newData, bool timeRefresh = true)
    {
        LPInfo lp_old;
        try
        {
            if (timeRefresh)
            {
                nearRefreshTime = newData.veh_timestamp;
            }
            counter++;
            lp_old = vehinfo.lp_info;
            vehinfo = newData;
            if (lp_old.lp_number.vlp_rec_confidence > newData.lp_info.lp_number.vlp_rec_confidence)
            {
                vehinfo.lp_info = lp_old;
            }
            space_rate_cache.enqueue(vehinfo.veh_chassis.space_rat);
        }
        catch (std::exception &e)
        {
            std::cerr << "[StatisticsVelInSpace] Refresh error: " << e.what() << std::endl;
            std::cerr << "[StatisticsVelInSpace] new lp_str:[" << newData.lp_info.lp_number.lp_str
                << "] confidence:[" << newData.lp_info.lp_number.vlp_rec_confidence << "]" << std::endl;
            std::cerr << "[StatisticsVelInSpace] old lp_str:[" << lp_old.lp_number.lp_str
                << "] confidence:[" << lp_old.lp_number.vlp_rec_confidence << "]" << std::endl;
            std::cerr << "[StatisticsVelInSpace] now lp_str:[" << vehinfo.lp_info.lp_number.lp_str
                << "] confidence:[" << vehinfo.lp_info.lp_number.vlp_rec_confidence << "]" << std::endl;
        }
    }

public:
    unsigned long long firstRefreshTime;
    unsigned long long nearRefreshTime;
    CircularArray<float> space_rate_cache;
    int counter;
    VehInfo vehinfo;
    int waitTimes = -1;
};


// 单个车位处理结果
struct ParkingMessage
{
    ParkingState_t parking_state = SL_PARKING_STATE_NOON; // 车位的状态
    unsigned long long refresh_time = 0; // 车位状态更新时间
    int veh_num = 0; // 车辆数量
    std::map<std::string, StatisticsVelInSpace> veh_info;
};

// 所有车位的处理结果
struct ParkingResult
{
    int parking_num = 0; //!< 车位的总数
    ParkingMessage parking_mess[park_num_max]; //!< 车位中的信息
    int camera_channel = -1;
    unsigned long long frame_index = 0;

    ParkingResult &
    operator=(const ParkingResult &other)
    {
        if (this == &other)
            return *this;
        parking_num = other.parking_num;
        camera_channel = other.camera_channel;
        frame_index = other.frame_index;
        for (int i = 0; i < parking_num && i < park_num_max; i++)
        {
            parking_mess[i] = other.parking_mess[i];
        }
        return *this;
    }
};

void
PrintParkingResultWhenChange(ParkingResult parking_result);

class VehParkingInfo
{
public:
    VehParkingInfo();

public:
    void
    UpdateParkingResult(const VehInOutInfoVec& veh_inout_info_vec, const VehInfoVec& vehicles);

    void
    InBranchDeal(const VehInOutInfo& order);

    void
    OutBranchDeal(const VehInOutInfo& order);

public:
    ParkingResult parking_result_;

#ifdef PARKING_LOG
public:
    inline bool
    PrintCurrentParking()
    {
        if (print)
        {
            print = false;
            return true;
        }
        else
        {
            return false;
        }
    };

private:
    bool print = false;
#endif
};
} // namespace sonli

#endif
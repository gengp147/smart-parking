#include "parking_info.h"
#include "config_content.h"
#include "../utils/sl_logger.h"
#include "../utils/file_utils.h"
#include "trend_acc.h"

std::ostream &
operator<<(std::ostream &os, const sonli::StatisticsVelInSpace &space)
{
    os << "firstRefreshTime: " << space.firstRefreshTime << " nearRefreshTime: " << space.nearRefreshTime
       << " counter: " << space.counter << " vehinfo: " << space.vehinfo.lp_info.lp_number.lp_str;
    return os;
}

namespace sonli
{
    namespace const_val
    {
        float PARKING_RATE_THRESHOLD = 0.25;
        float PARKING_RATE_FULL_SHARE = 0.7;
        float PARKING_MINIMUM_STABLE_VALUE = 0.35;
        float PARKING_DOWN_STABLE_VALUE = 0.06;
        float PARKING_VAR_STABLE_VALUE = 0.007;
        int PARKING_RATE_CACHE_MAX_COUNTER = 121;

        float UP_SLOPE_THRESHOLD = PARKING_RATE_THRESHOLD / PARKING_RATE_CACHE_MAX_COUNTER;
        float DOWN_SLOPE_THRESHOLD = -PARKING_RATE_THRESHOLD / PARKING_RATE_CACHE_MAX_COUNTER;
    } // namespace const_val

    VehParkingInfo::VehParkingInfo()
    {
    }

    // 更新车位信息
    void
    VehParkingInfo::UpdateParkingResult(const VehInOutInfoVec& veh_inout_info_vec, const VehInfoVec& vehicles)
    {
        parking_result_.camera_channel = vehicles.camera_channel;

        for (int i = 0; i < vehicles.veh_info.size(); i++)
        {
            auto occupy_a_parking_space_id = vehicles.veh_info[i].veh_chassis.space_id;
            auto track_id = vehicles.veh_info[i].veh_track_id;
            auto vehicle_lp = vehicles.veh_info[i].lp_info.lp_number.lp_str;
            auto vehicle_lp_rec_cr = vehicles.veh_info[i].lp_info.lp_number.vlp_rec_confidence;

            if (occupy_a_parking_space_id < 0)
            {
                for (int ind = 0; ind < park_num_max; ind++)
                {
                    for (auto &veh_in_p : parking_result_.parking_mess[ind].veh_info)
                    {
                        if ((veh_in_p.first == vehicle_lp && vehicle_lp != "-") ||
                            veh_in_p.second.vehinfo.veh_track_id == track_id)
                        {
                            veh_in_p.second.Refresh(vehicles.veh_info[i], false);
                        }
                    }
                }
                continue;
            }



            auto &parking_space = parking_result_.parking_mess[occupy_a_parking_space_id];
            if (parking_space.veh_info.empty())
            {
                parking_space.veh_info.emplace(vehicle_lp, vehicles.veh_info[i]);
            }
            else
            {
                if (parking_space.veh_info.count(vehicle_lp))
                {
                    parking_space.veh_info.at(vehicle_lp).Refresh(vehicles.veh_info[i]);
                }
                else
                {
                    bool insertedInto = false;
                    for (auto &v : parking_space.veh_info)
                    {
                        if (v.second.vehinfo.veh_track_id == track_id)
                        {
                            v.second.Refresh(vehicles.veh_info[i]);
                            insertedInto = true;
                            if (vehicle_lp_rec_cr > v.second.vehinfo.lp_info.lp_number.vlp_rec_confidence &&
                                vehicle_lp != "-")
                            {
                                parking_space.veh_info.emplace(vehicle_lp, v.second);
                                parking_space.veh_info.at(vehicle_lp).Refresh(vehicles.veh_info[i]);
                                parking_space.veh_info.erase(v.first);
                            }
                            break;
                        }
                    }
                    if (!insertedInto)
                    {
                        parking_space.veh_info.emplace(vehicle_lp, vehicles.veh_info[i]);
                    }
                }
            }
        }

        for (auto &parkingInfo : this->parking_result_.parking_mess)
        {
            // 车辆短时间停留后驶离，删除泊位内痕迹
            //			if(already.empty())
            {
                std::vector<std::string> removing;
                if (!parkingInfo.veh_info.empty())
                {
                    for (const auto &veh : parkingInfo.veh_info)
                    {
                        if ((vehicles.frame_timestamp - veh.second.nearRefreshTime) / 1000 > 15)
                        {
                            if (veh.second.counter < 12 * 15)
                            {
                                removing.push_back(veh.first);
                            }
                        }
                    }
                }

                for (const auto &pl : removing)
                {
                    parkingInfo.veh_info.erase(pl);
                }
            }

            // 根据驶入趋势，提前判断车辆进入了泊位
            if (!parkingInfo.veh_info.empty() && parkingInfo.parking_state != SL_PARKING_STATE_IN)
            {
                for (const auto &veh : parkingInfo.veh_info)
                {
                    if (veh.second.space_rate_cache.size() < const_val::PARKING_RATE_CACHE_MAX_COUNTER)
                        continue;
                    if (vehicles.frame_timestamp != veh.second.nearRefreshTime)
                        continue;
                    auto space_rate_vec = veh.second.space_rate_cache.readFirstNElements(-1);
                    if (veh.second.space_rate_cache[veh.second.space_rate_cache.size() - 1] >
                        const_val::PARKING_RATE_FULL_SHARE)
                    {
                        auto ma = basic_algorithm::MovingAverage(space_rate_vec, 5);
                        auto keyPoint = basic_algorithm::FindKeyPoint(ma, 5);

                        auto l_e = basic_algorithm::LinearRegression(ma, 0, keyPoint);
                        auto R2 = basic_algorithm::CalculateRSquared(ma, l_e.first, l_e.second, 0, keyPoint);
                        auto trend = basic_algorithm::DetermineTrend(
                            l_e.first, R2, 0.70, 0.70, const_val::UP_SLOPE_THRESHOLD, const_val::DOWN_SLOPE_THRESHOLD);

                        auto l_e1 = basic_algorithm::LinearRegression(ma, keyPoint, ma.size());
                        auto R21 = basic_algorithm::CalculateRSquared(ma, l_e1.first, l_e1.second, keyPoint, ma.size());
                        auto trend1 = basic_algorithm::DetermineTrend(
                            l_e1.first, R21, 0.75, 0.75, const_val::UP_SLOPE_THRESHOLD, const_val::DOWN_SLOPE_THRESHOLD);

                        if (trend.first == basic_algorithm::Trend::INCREASING &&
                            trend1.first != basic_algorithm::Trend::DECREASING)
                        {
                            if (parkingInfo.parking_state != SL_PARKING_STATE_IN)
                            {
                                parkingInfo.refresh_time = vehicles.frame_timestamp;
                            }
                            parkingInfo.parking_state = SL_PARKING_STATE_IN;

#ifdef PARKING_LOG

                            print = true;
#endif
                            continue;
                        }
                    }
                    if (veh.second.space_rate_cache[veh.second.space_rate_cache.size() - 1] >=
                        const_val::PARKING_MINIMUM_STABLE_VALUE)
                    {
                        auto var = basic_algorithm::CalculateStandardDeviation(space_rate_vec);
                        if (var.first > (const_val::PARKING_MINIMUM_STABLE_VALUE - const_val::PARKING_DOWN_STABLE_VALUE) &&
                            var.second < const_val::PARKING_VAR_STABLE_VALUE)
                        {
#ifdef PARKING_LOG
                            print = true;

#endif
                            if (parkingInfo.parking_state != SL_PARKING_STATE_IN)
                            {
                                parkingInfo.refresh_time = vehicles.frame_timestamp;
                            }
                            parkingInfo.parking_state = SL_PARKING_STATE_IN;
                        }
                    }
                }
            }
        }

        for (int index = 0; index < parking_result_.parking_num; index++)
        {
            //			this->parking_result_->parking_mess[index].parking_state = SL_PARKING_STATE_NOON;
            std::vector<std::string> need_del;
            for (auto &item : parking_result_.parking_mess[index].veh_info)
            {
                auto &lp = item.first;
                auto &detail = item.second;

                if (double(vehicles.frame_timestamp - detail.nearRefreshTime) / 1000.0 > PARKING_EMPTY_CONTINUE_TIME)
                {
                    need_del.push_back(lp);
                    continue;
                }

                if (vehicles.is_blind_camera)
                {
                    if (double(vehicles.frame_timestamp - detail.nearRefreshTime) > 120000)
                    {
                        need_del.push_back(lp);
                        continue;
                    }
                }

                if (detail.waitTimes == 0)
                {
                    auto space_rate_vec = item.second.space_rate_cache.readFirstNElements(120);
                    auto ma = basic_algorithm::MovingAverage(space_rate_vec, 12);
                    auto keyPoint = basic_algorithm::FindKeyPoint(ma, 12);

                    auto l_e1 = basic_algorithm::LinearRegression(ma, keyPoint, ma.size());
                    auto R21 = basic_algorithm::CalculateRSquared(ma, l_e1.first, l_e1.second, keyPoint, ma.size());
                    auto trend1 = basic_algorithm::DetermineTrend(
                        l_e1.first, R21, 0.72, 0.72, const_val::UP_SLOPE_THRESHOLD, const_val::DOWN_SLOPE_THRESHOLD);

                    if (trend1.first != basic_algorithm::Trend::INCREASING)
                    {
                        //	std::cout << "time comm... because is waitTimes:" << std::endl;
                        need_del.push_back(lp);
                        continue;
                    }
                    detail.waitTimes = -1;
                }

                if (detail.waitTimes > 0)
                {
                    detail.waitTimes--;
                    continue;
                }

                if (this->parking_result_.parking_mess[index].parking_state != SL_PARKING_STATE_EMPTY)
                {
                    if ((vehicles.frame_timestamp - detail.nearRefreshTime) / 1000 > 1)
                    {
                        if (detail.space_rate_cache.size() >= 120 &&
                            detail.space_rate_cache[detail.space_rate_cache.size() - 1] < const_val::PARKING_RATE_THRESHOLD)
                        {
                            auto space_rate_vec = item.second.space_rate_cache.readFirstNElements(120);
                            auto ma = basic_algorithm::MovingAverage(space_rate_vec, 12);
                            auto keyPoint = basic_algorithm::FindKeyPoint(ma, 12);

                            auto l_e1 = basic_algorithm::LinearRegression(ma, keyPoint, ma.size());
                            auto R21 = basic_algorithm::CalculateRSquared(ma, l_e1.first, l_e1.second, keyPoint, ma.size());
                            auto trend1 =
                                basic_algorithm::DetermineTrend(l_e1.first, R21, 0.72, 0.72, const_val::UP_SLOPE_THRESHOLD,
                                                                const_val::DOWN_SLOPE_THRESHOLD);

                            if (trend1.first == basic_algorithm::Trend::DECREASING)
                            {
                                //	std::cout << "Parking... because is DECREASING:" << keyPoint << std::endl;
                                detail.waitTimes = keyPoint;
                                continue;
                            }
                        }

                        // 均值0，标准差0.1
                        if (detail.space_rate_cache.size() >= 40)
                        {
                            auto l = detail.space_rate_cache.readLastNElements(40);
                            auto var = basic_algorithm::CalculateStandardDeviation(l);
                            if (var.first < 0.05 && var.second < 0.1)
                            {
                                //	std::cout << "Parking... because is 0.0:" << var.first << "\t" << var.second <<
                                // std::endl;
                                detail.waitTimes = 40;
                            }
                        }
                    }
                }
            }

            for (const auto &lp : need_del)
            {
                parking_result_.parking_mess[index].veh_info.erase(lp);
            }

            if (this->parking_result_.parking_mess[index].veh_info.empty())
            {
                //				if(this->parking_result_->parking_mess[index].parking_state !=
                // SL_PARKING_STATE_EMPTY) 					SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->debug( 							"space_id:
                //{}, status:[EMPTY]", 							index);

#ifdef PARKING_LOG
                if (this->parking_result_.parking_mess[index].parking_state != SL_PARKING_STATE_EMPTY)
                {
                    print = true;
                }
#endif
                if (this->parking_result_.parking_mess[index].parking_state != SL_PARKING_STATE_EMPTY)
                {
                    this->parking_result_.parking_mess[index].refresh_time = vehicles.frame_timestamp;
                }
                this->parking_result_.parking_mess[index].parking_state = SL_PARKING_STATE_EMPTY;
            }
            else
            {
                for (auto &singleVeh : this->parking_result_.parking_mess[index].veh_info)
                {
                    if ((singleVeh.second.nearRefreshTime - singleVeh.second.firstRefreshTime) / 1000 >
                        PARKING_IN_CONTINUE_TIME)
                    {
                        if (singleVeh.second.counter > 20)
                        {
#ifdef PARKING_LOG
                            if (this->parking_result_.parking_mess[index].parking_state != SL_PARKING_STATE_IN)
                            {
                                print = true;
                            }
#endif
                            if (this->parking_result_.parking_mess[index].parking_state != SL_PARKING_STATE_IN)
                            {
                                this->parking_result_.parking_mess[index].refresh_time = vehicles.frame_timestamp;
                            }
                            this->parking_result_.parking_mess[index].parking_state = SL_PARKING_STATE_IN;
                        }
                    }
                }
                if (this->parking_result_.parking_mess[index].parking_state != SL_PARKING_STATE_IN)
                {
#ifdef PARKING_LOG
                    if (this->parking_result_.parking_mess[index].parking_state != SL_PARKING_STATE_NOON)
                    {
                        print = true;
                    }
#endif
                    if (this->parking_result_.parking_mess[index].parking_state != SL_PARKING_STATE_NOON)
                    {
                        this->parking_result_.parking_mess[index].refresh_time = vehicles.frame_timestamp;
                    }
                    this->parking_result_.parking_mess[index].parking_state = SL_PARKING_STATE_NOON;
                }
            }
        }
    }

    void
    VehParkingInfo::InBranchDeal(const VehInOutInfo& order)
    {
        // const auto velDetail = order.pro_veh_info;
        if (order.pro_veh_info.veh_chassis.space_id < 0 || order.pro_veh_info.veh_chassis.space_id >= park_num_max)
        {
            // TODO error message
            return;
        }
        else
        {
            auto &spaceDetail = this->parking_result_.parking_mess[order.pro_veh_info.veh_chassis.space_id];
            if (spaceDetail.parking_state != ParkingState_t::SL_PARKING_STATE_IN)
            {

                if (spaceDetail.parking_state != SL_PARKING_STATE_IN)
                {
                    spaceDetail.refresh_time = order.send_timestamp;
                }
                spaceDetail.parking_state = ParkingState_t::SL_PARKING_STATE_IN;

#ifdef PARKING_LOG
                print = true;
#endif
            }

            spaceDetail.veh_info.clear();

            spaceDetail.veh_info.emplace(order.pro_veh_info.lp_info.lp_number.lp_str, order.pro_veh_info);
            spaceDetail.veh_info.at(order.pro_veh_info.lp_info.lp_number.lp_str).firstRefreshTime =
                order.pro_veh_info.veh_timestamp - PARKING_IN_CONTINUE_TIME * 1000 * 2 - 1000;
            spaceDetail.veh_info.at(order.pro_veh_info.lp_info.lp_number.lp_str).counter = 50;
            spaceDetail.veh_num = spaceDetail.veh_info.size();
        }
    }

    void
    VehParkingInfo::OutBranchDeal(const VehInOutInfo& order)
    {
        // auto velDetail = order.pro_veh_info;
        if (order.pro_veh_info.veh_chassis.space_id < 0 || order.pro_veh_info.veh_chassis.space_id >= park_num_max)
        {
            // TODO error message
            return;
        }
        else
        {
            auto &spaceDetail = this->parking_result_.parking_mess[order.pro_veh_info.veh_chassis.space_id];
            if (spaceDetail.veh_info.count(order.pro_veh_info.lp_info.lp_number.lp_str))
            {
                spaceDetail.veh_info.erase(order.pro_veh_info.lp_info.lp_number.lp_str);
                spaceDetail.veh_num--;
            }
            else if (spaceDetail.veh_info.size() == 1 &&
                     (order.pro_veh_info.lp_info.lp_number.lp_str == "-" || spaceDetail.veh_info.count("-")))
            {
                spaceDetail.veh_info.clear();
                spaceDetail.veh_num = 0;
            }
            else
            {
                // error message 未找到入场信息
            }

            spaceDetail.veh_num = spaceDetail.veh_info.size();
            if (spaceDetail.veh_num == 0)
            {

#ifdef PARKING_LOG
                if (spaceDetail.parking_state != ParkingState_t::SL_PARKING_STATE_EMPTY)
                {
                    print = true;
                }
#endif
                if (spaceDetail.parking_state != SL_PARKING_STATE_EMPTY)
                {
                    spaceDetail.refresh_time = order.send_timestamp;
                }
                spaceDetail.parking_state = ParkingState_t::SL_PARKING_STATE_EMPTY;
            }
        }
    }

} // namespace sonli

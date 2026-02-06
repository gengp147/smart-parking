#include "../veh_in_out/space_judge.h"
#include "capture_record_container.h"
#include "data_type.h"
#include "sl_data_types_base.h"
#include <tuple>
#include <vector>

namespace sonli
{
namespace illegal_parking
{
/**
 * @brief 计算两个向量之间的角度
 *
 * @param vector1 第一个向量
 * @param vector2 第二个向量
 * @return double 两个向量之间的角度（弧度）
 */
static double
_vector_angle(const std::vector<int> &vector1, const std::vector<int> &vector2)
{
    double dot_product = 0;
    double magnitude1 = 0;
    double magnitude2 = 0;

    for (size_t i = 0; i < vector1.size(); ++i)
    {
        dot_product += vector1[i] * vector2[i];
        magnitude1 += vector1[i] * vector1[i];
        magnitude2 += vector2[i] * vector2[i];
    }

    magnitude1 = std::sqrt(magnitude1);
    magnitude2 = std::sqrt(magnitude2);

    double cos_theta = dot_product / (magnitude1 * magnitude2);
    double theta = std::acos(cos_theta);
    return theta;
}

/**
 * @brief 计算两条线之间的角度
 *
 * @param start1 第一条线的起始点
 * @param end1 第一条线的结束点
 * @param start2 第二条线的起始点
 * @param end2 第二条线的结束点
 * @return double 两条线之间的角度（弧度）
 */
static double
_line_angle(const Point &start1, const Point &end1, const Point &start2, const Point &end2)
{
    auto _v1 = end1 - start1;
    auto _v2 = end2 - start2;
    return _vector_angle({_v1.x_, _v1.y_}, {_v2.x_, _v2.y_});
}

/**
 * @brief 计算车辆与车位之间的角度和视图角度
 *
 * @param vehicle_vector 车辆的两个关键点坐标
 * @param space_vector 车位的两个关键点坐标
 * @return std::tuple<float, float> 角度（弧度）和视图角度（度）
 */
auto
CalculateAngleFormation(const std::vector<Point> &vehicle_vector, const std::vector<Point> &space_vector)
    -> std::tuple<float, float>
{
    if (vehicle_vector.size() != 2 || space_vector.size() != 2)
    {
        throw std::invalid_argument("Invalid vehicle_vector or space_vector when CalculateAngleFormation.");
    }

    auto angle = _line_angle(vehicle_vector[0], vehicle_vector[1], space_vector[0], space_vector[1]);
    auto view = angle * 180 / std::acos(-1);
    return {angle, view};
}

Point
findTopLeft(const std::vector<Point> &points)
{
    Point top_left = points[0];
    for (const auto &p : points)
    {
        if (p.x_ < top_left.x_ || (p.x_ == top_left.x_ && p.y_ < top_left.y_))
        {
            top_left = p;
        }
    }
    return top_left;
}

double
crossProduct(const Point &P0, const Point &A, const Point &B)
{
    return (A.x_ - P0.x_) * (B.y_ - P0.y_) - (A.y_ - P0.y_) * (B.x_ - P0.x_);
}

std::vector<Point>
sortCounterClockwise(std::vector<Point> points)
{
    Point P0 = findTopLeft(points);
    std::sort(points.begin(), points.end(),
              [&P0](const Point &A, const Point &B)
              {
                  double cross = crossProduct(P0, A, B);
                  return cross > 0;
              });

    return points;
}

void
AngleBetweenCarAndSpace(CacheVehInfo *vehicle_cache, const std::shared_ptr<ParkingInfoVec> &parking_info_ptr,
                        const BerthEventConfig *rules)
{
    const auto &latest_frame_vehicle = vehicle_cache->veh_infos.back();
    const auto &keypoints = latest_frame_vehicle.veh_key_points;

    auto ori_sc = parking_info_ptr->parking_info.at(latest_frame_vehicle.veh_chassis.space_id).points;
    auto sc = sortCounterClockwise(ori_sc);
    auto a = (sc[0] + sc[3]) / 2;
    auto b = (sc[1] + sc[2]) / 2;

    float angle, view;
    std::tie(angle, view) =
        CalculateAngleFormation({Point(keypoints[0].x, keypoints[0].y), Point(keypoints[1].x, keypoints[1].y)}, {a, b});

    auto VerticalParkingParkingRecord =
        vehicle_cache->getMultipleIllegalCaptureDetail()->Find(EIllegalCase::_VERTICAL_PARKING);
    auto DiagonalParkingRecord = vehicle_cache->getMultipleIllegalCaptureDetail()->Find(EIllegalCase::_ANGLE_PARKING);
    auto OppositeParkingRecord =
        vehicle_cache->getMultipleIllegalCaptureDetail()->Find(EIllegalCase::_OPPOSITE_PARKING);
    auto ParkingOnTheLineRecord =
        vehicle_cache->getMultipleIllegalCaptureDetail()->Find(EIllegalCase::_PARKING_ONTO_LINE);

    bool check = false;
    if (rules->config_detail.alarm_events.bReportVerticalParkingError &&
        !VerticalParkingParkingRecord->WasReported() && 0)
    {
        auto p1 = (sc[0] + sc[1]) / 2;
        auto p2 = (sc[2] + sc[3]) / 2;
        float angle_t, view_t;
        std::tie(angle_t, view_t) = CalculateAngleFormation(
            {Point(keypoints[0].x, keypoints[0].y), Point(keypoints[1].x, keypoints[1].y)}, {p1, p2});
        if (view_t >= 0 && view_t <= 30)
        {
            check = true;
            VerticalParkingParkingRecord->UpdateCapStamp(*rules, latest_frame_vehicle, parking_info_ptr->time_info_);
        }
    }

    if (!check && rules->config_detail.alarm_events.bReportReverseParkingError &&
        !OppositeParkingRecord->WasReported())
    {

        auto first_point = keypoints[0];
        auto second_point = keypoints[1];
        int toward = 0;
        if (first_point.y < second_point.y)
        {
            toward = 2; // 看车尾
        }
        else
        {
            toward = 1; // 看车头
        }
        if (toward == rules->config_detail.alarm_events.reportReverseParkingType)
        {
            OppositeParkingRecord->UpdateCapStamp(*rules, latest_frame_vehicle, parking_info_ptr->time_info_);
        }
    }
    if (!check && rules->config_detail.alarm_events.bReportDiagonalParkingError &&
        !DiagonalParkingRecord->WasReported() && 0)
    {
        if ((view >= 30 && view <= 80)
            // || (view >= 100 && view <= 150)
        )
        {
            check = true;
            DiagonalParkingRecord->UpdateCapStamp(*rules, latest_frame_vehicle, parking_info_ptr->time_info_);
        }
    }

    if (!check && rules->config_detail.alarm_events.bReportCrossLineError &&
        !ParkingOnTheLineRecord->WasReported() && (view < 40 || view > 140) && 0)
    {
        auto car_asset_area = latest_frame_vehicle.veh_chassis.car_rat_vec[latest_frame_vehicle.veh_chassis.space_id];
        if (car_asset_area < 0.80)
        {
            auto vkp = latest_frame_vehicle.veh_chassis.chassis;
            int stride_w = (vkp[0].x_ - vkp[3].x_) * 0.20;
            int stride_h = (vkp[0].y_ - vkp[1].y_) * 0.20;
            vkp[0].x_ -= stride_w;
            vkp[0].y_ -= stride_h;
            vkp[1].x_ -= stride_w;
            vkp[1].y_ += stride_h;
            vkp[2].x_ += stride_w;
            vkp[2].y_ += stride_h;
            vkp[3].x_ += stride_w;
            vkp[3].y_ -= stride_h;
            float c1 = 0.0, c2 = 0.0;
            iou_poly(vkp, sc, c1, c2);
            if (c1 < 0.98)
            {
                ParkingOnTheLineRecord->UpdateCapStamp(*rules, latest_frame_vehicle, parking_info_ptr->time_info_);
            }
        }
    }
}

} // namespace illegal_parking
} // namespace sonli
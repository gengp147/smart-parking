/*
 * @Author: lwn
 * @Date: 2024-04-03 15:02:07
 * @LastEditors: lwn
 * @LastEditTime: 2024-06-26 17:49:12
 * @FilePath: \falconeye_debug\src\track\motion\calc_velocity.cc
 * @Description:
 */
#include "../track/motion/calc_velocity.h"
#include <assert.h>

#define PI 3.14159265358979323846

namespace sonli
{

cv::Mat
calcVelocities(const cv::Mat &pre_chassis_peaks, const cv::Mat &chassis_peaks)
{
    cv::Mat all_velocities(4, 2, CV_32F);
    cv::Mat speed = pre_chassis_peaks - chassis_peaks;
    for (int row = 0; row < all_velocities.rows; row++)
    {
        float norm = cv::norm(speed.at<cv::Vec2f>(row)) + 1e-6f;
        all_velocities.at<cv::Vec2f>(row) = speed.at<cv::Vec2f>(row) / norm;
    }
    return all_velocities;
}

float
velocity_cost_with_score(const cv::Mat &vel1, const cv::Mat &vel2, const float score, const float weight)
{

    if (vel1.empty() || vel2.empty())
    {
        return 0.0;
    }

    assert(vel1.rows == vel2.rows);
    assert(vel1.rows == 4);
    assert(vel1.cols == vel2.cols);
    assert(vel1.cols == 2);
    float cost = 0.f;
    for (int row = 0; row < vel1.rows; row++)
    {
        const cv::Vec2f &peak_a_velocity = vel1.at<cv::Vec2f>(row);
        const cv::Vec2f &peak_b_velocity = vel2.at<cv::Vec2f>(row);
        float diff_angle_cos = peak_a_velocity[0] * peak_b_velocity[0] + peak_a_velocity[1] * peak_b_velocity[1];
        diff_angle_cos = std::min(std::max(diff_angle_cos, -1.f), 1.f);
        float diff_angle = std::acos(diff_angle_cos);
        diff_angle = std::abs(PI / 2.f - std::abs(diff_angle)) / PI;
        cost += diff_angle;
    }
    return cost / vel1.rows * score * weight;
}

} // namespace sonli
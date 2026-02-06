/***
 * @Author: lwn
 * @Date: 2024-04-03 14:40:44
 * @LastEditors: lwn
 * @LastEditTime: 2024-04-03 16:04:08
 * @FilePath: /FalconEye/include/track/motion/calc_velocity.h
 * @Description:
 */
#ifndef _TRACK_MOTION_VEL_COST_H_
#define _TRACK_MOTION_VEL_COST_H_
#include <math.h>
#include <opencv2/opencv.hpp>

namespace sonli
{

/***
 * @description: Calculate velocty for chassis key points.
 * @param {Mat&} chassis_peaks : current chassis, Shape(4, 2)
 * @param {Mat&} pre_chassis_peaks: last_chassis, Shape(4, 2)
 * @return {Mat} all_velocities, Shape(4, 2)
 */
cv::Mat
calcVelocities(const cv::Mat &chassis_peaks, const cv::Mat &pre_chassis_peaks);

float
velocity_cost_with_score(const cv::Mat &vel1, const cv::Mat &vel2, const float score, const float weight = 1.f);
} // namespace sonli

#endif
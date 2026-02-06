/***
 * @Author: lwn
 * @Date: 2024-03-26 14:13:05
 * @LastEditors: lwn
 * @LastEditTime: 2024-04-24 10:24:57
 * @FilePath: \FalconEye\include\track\motion\improved_kalman_filter.h
 * @Description:
 */
#ifndef _TRACK_IMPROVED_KALMAN_FILTER_H_
#define _TRACK_IMPROVED_KALMAN_FILTER_H_

#include <vector>

#include <opencv2/core/core.hpp>
// #include <opencv2/highgui/highgui.hpp>
#include "opencv2/video/tracking.hpp"
#include <opencv2/imgproc/imgproc.hpp>

namespace sonli
{

class ImprovedKalmanFilter : public cv::KalmanFilter
{
public:
    ImprovedKalmanFilter();

    virtual void
    init(const cv::Mat &measurement);

    virtual ~ImprovedKalmanFilter() {}

    virtual cv::Mat
    predict();
    virtual const cv::Mat &
    correct(const cv::Mat &measurement);
    virtual void
    project(cv::Mat *mean, cv::Mat *covariance) const;

private:
    float std_weight_position;
    float std_weight_velocity;
};

} // namespace sonli

#endif
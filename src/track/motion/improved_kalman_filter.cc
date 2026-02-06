/*
 * @Author: lwn
 * @Date: 2024-04-10 09:31:19
 * @LastEditors: lwn
 * @LastEditTime: 2024-04-24 10:26:07
 * @FilePath: \FalconEye\src\track\motion\improved_kalman_filter.cc
 * @Description:
 */

#include "../track/motion/improved_kalman_filter.h"

namespace sonli
{
ImprovedKalmanFilter::ImprovedKalmanFilter(void) : cv::KalmanFilter(8, 4)
{
    cv::KalmanFilter::transitionMatrix = cv::Mat::eye(8, 8, CV_32F);
    for (int i = 0; i < 4; ++i)
        cv::KalmanFilter::transitionMatrix.at<float>(i, i + 4) = 1;
    cv::KalmanFilter::measurementMatrix = cv::Mat::eye(4, 8, CV_32F);
    std_weight_position = 1 / 40.f;
    std_weight_velocity = 1 / 160.f;
}

void
ImprovedKalmanFilter::init(const cv::Mat &measurement)
{
    measurement.copyTo(statePost(cv::Rect(0, 0, 1, 4)));
    statePost(cv::Rect(0, 4, 1, 4)).setTo(0);
    statePost.copyTo(statePre);

    float varpos = 2 * std_weight_position * (*measurement.ptr<float>(2));
    varpos *= varpos;
    float varvel = 10 * std_weight_velocity * (*measurement.ptr<float>(2));
    varvel *= varvel;

    errorCovPost.setTo(0);
    *errorCovPost.ptr<float>(0, 0) = varpos;
    *errorCovPost.ptr<float>(1, 1) = varpos;
    *errorCovPost.ptr<float>(2, 2) = varpos;
    *errorCovPost.ptr<float>(3, 3) = varpos;
    *errorCovPost.ptr<float>(4, 4) = varvel;
    *errorCovPost.ptr<float>(5, 5) = varvel;
    *errorCovPost.ptr<float>(6, 6) = 1e-10f;
    *errorCovPost.ptr<float>(7, 7) = varvel;
    errorCovPost.copyTo(errorCovPre);
}

cv::Mat
ImprovedKalmanFilter::predict()
{
    float varpos = std_weight_position * (*statePre.ptr<float>(2));
    varpos *= varpos;
    float varvel = std_weight_velocity * (*statePre.ptr<float>(2));
    varvel *= varvel;

    processNoiseCov.setTo(0);
    *processNoiseCov.ptr<float>(0, 0) = varpos;
    *processNoiseCov.ptr<float>(1, 1) = varpos;
    *processNoiseCov.ptr<float>(2, 2) = varpos;
    *processNoiseCov.ptr<float>(3, 3) = varpos;
    *processNoiseCov.ptr<float>(4, 4) = varvel;
    *processNoiseCov.ptr<float>(5, 5) = varvel;
    *processNoiseCov.ptr<float>(6, 6) = 1e-10f;
    *processNoiseCov.ptr<float>(7, 7) = varvel;

    return cv::KalmanFilter::predict();
}

const cv::Mat &
ImprovedKalmanFilter::correct(const cv::Mat &measurement)
{
    float varpos = std_weight_position * (*measurement.ptr<float>(2));
    varpos *= varpos;

    measurementNoiseCov.setTo(0);
    *measurementNoiseCov.ptr<float>(0, 0) = varpos;
    *measurementNoiseCov.ptr<float>(1, 1) = varpos;
    *measurementNoiseCov.ptr<float>(2, 2) = varpos;
    *measurementNoiseCov.ptr<float>(3, 3) = varpos;

    return cv::KalmanFilter::correct(measurement);
}

void
ImprovedKalmanFilter::project(cv::Mat *mean, cv::Mat *covariance) const
{
    float varpos = std_weight_position * (*statePost.ptr<float>(2));
    varpos *= varpos;

    cv::Mat measurementNoiseCov_ = cv::Mat::eye(4, 4, CV_32F);
    *measurementNoiseCov_.ptr<float>(0, 0) = varpos;
    *measurementNoiseCov_.ptr<float>(1, 1) = varpos;
    *measurementNoiseCov_.ptr<float>(2, 2) = varpos;
    *measurementNoiseCov_.ptr<float>(3, 3) = varpos;

    *mean = measurementMatrix * statePost;
    cv::Mat temp = measurementMatrix * errorCovPost;
    gemm(temp, measurementMatrix, 1, measurementNoiseCov_, 1, *covariance, cv::GEMM_2_T);
}

} // namespace sonli
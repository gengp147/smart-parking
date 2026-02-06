/***
 * @Author: lwn
 * @Date: 2024-04-03 10:44:35
 * @LastEditors: lwn
 * @LastEditTime: 2024-04-03 17:13:38
 * @FilePath: /FalconEye/include/track/motion/calc_distance.h
 * @Description:
 */
#ifndef _TRACK_MOTION_CALC_DISTANCE_H_
#define _TRACK_MOTION_CALC_DISTANCE_H_

#include <memory>
#include <opencv2/opencv.hpp>
#include <vector>

namespace sonli
{
inline cv::Mat
embeddingDistance(std::vector<cv::Mat> emb1, std::vector<cv::Mat> emb2)
{
    cv::Mat dists(emb1.size(), emb2.size(), CV_32F);
    for (size_t i = 0; i < emb1.size(); ++i)
    {
        float *distsi = dists.ptr<float>(i);
        for (size_t j = 0; j < emb2.size(); ++j)
        {
            cv::Mat u = emb1[i];
            cv::Mat v = emb2[j];
            double uv = u.dot(v);
            double uu = u.dot(u);
            double vv = v.dot(v);
            double dist = std::abs(1. - uv / std::sqrt(uu * vv));
            // double dist = cv::norm(a[i].smooth_embedding, b[j].smooth_embedding,
            // cv::NORM_L2);
            distsi[j] = static_cast<float>(std::max(std::min(dist, 2.), 0.));
        }
    }
    return dists;
}
} // namespace sonli

#endif
/***
 * @Author: lwn
 * @Date: 2024-04-03 17:19:31
 * @LastEditors: lwn
 * @LastEditTime: 2024-05-07 15:25:33
 * @FilePath: \FalconEye\include\track\matching\associate.h
 * @Description:
 */
#ifndef _TRACK_MATCHING_ASSOCIATE_H_
#define _TRACK_MATCHING_ASSOCIATE_H_
#include "../track/motion/calc_iou.h"
#include "../track/motion/calc_velocity.h"
#include <map>
#include <opencv2/opencv.hpp>
#include <vector>

#include "data_type.h"

namespace sonli
{
typedef std::map<int, int> Match;
typedef std::map<int, int>::iterator MatchIter;

void
linear_assignment(const cv::Mat &cost, float cost_limit, Match *matches, std::vector<int> *mismatch_row,
                  std::vector<int> *mismatch_col);
} // namespace sonli

#endif
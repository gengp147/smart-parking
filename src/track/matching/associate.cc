/*
 * @Author: lwn
 * @Date: 2024-04-03 17:23:25
 * @LastEditors: lwn
 * @LastEditTime: 2024-04-26 16:04:09
 * @FilePath: \FalconEye\src\track\matching\associate.cc
 * @Description:
 */

#include "../track/matching/associate.h"
#include "../track/matching/lapjv.h"
#include "../track/motion/calc_iou.h"
#include "../track/motion/calc_velocity.h"

namespace sonli
{

void
linear_assignment(const cv::Mat &cost, float cost_limit, Match *matches, std::vector<int> *mismatch_row,
                  std::vector<int> *mismatch_col)
{
    matches->clear();
    mismatch_row->clear();
    mismatch_col->clear();
    if (cost.empty())
    {
        for (int i = 0; i < cost.rows; ++i)
            mismatch_row->push_back(i);
        for (int i = 0; i < cost.cols; ++i)
            mismatch_col->push_back(i);
        return;
    }

    float opt = 0;
    cv::Mat x(cost.rows, 1, CV_32S);
    cv::Mat y(cost.cols, 1, CV_32S);

    lapjv_internal(cost, true, cost_limit, reinterpret_cast<int *>(x.data), reinterpret_cast<int *>(y.data));

    for (int i = 0; i < x.rows; ++i)
    {
        int j = *x.ptr<int>(i);
        if (j >= 0)
            matches->insert({i, j});
        else
            mismatch_row->push_back(i);
    }

    for (int i = 0; i < y.rows; ++i)
    {
        int j = *y.ptr<int>(i);
        if (j < 0)
            mismatch_col->push_back(i);
    }

    return;
}
} // namespace sonli
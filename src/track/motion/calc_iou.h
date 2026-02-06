/***
 * @Author: lwn
 * @Date: 2024-04-03 10:58:18
 * @LastEditors: lwn
 * @LastEditTime: 2024-05-07 15:24:07
 * @FilePath: \FalconEye\include\track\motion\calc_iou.h
 * @Description:
 */
#ifndef _TRACL_MOTION_CALC_IOU_H_
#define _TRACL_MOTION_CALC_IOU_H_

#include "data_type.h"

namespace sonli
{

template <class Tp>
Tp
iou(const BBox_<Tp> &rect1, const BBox_<Tp> &rect2)
{
    BBox_<Tp> inter_ = rect1 & rect2;
    if (inter_.empty())
    {
        return (Tp)0;
    }
    BBox_<Tp> union_ = rect1 | rect2;
    return inter_.area() / union_.area();
}
// cv::Mat width_modulated_iou(std::vector<TrackPtr> vec1, std::vector<TrackPtr> vec2);

} // namespace sonli

#endif
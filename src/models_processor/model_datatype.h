
#ifndef _DISC_POST_PROCESS_H_
#define _DISC_POST_PROCESS_H_

#include "data_type.h"
#include <algorithm>
#include <iostream>
#include <math.h>
#include <utility>
#include <vector>

namespace sonli
{
namespace model_datatype
{

template <typename _Tp> struct Anchor_
{
    _Tp w_;
    _Tp h_;
    Anchor_(_Tp w, _Tp h) : w_(w), h_(h) {}

    friend std::ostream &
    operator<<(std::ostream &out, Anchor_ &p)
    {
        out << "(w: " << p.w_ << " h: " << p.h_ << ")";
        return out;
    }
};
typedef Anchor_<int> Anchor;
typedef Anchor_<float> Anchor2f;


struct Object
{
    Rect2f rect_;
    std::vector<Point2f> landmark_;
    float score_ = 0;
    int32_t id_ = 0;
    int32_t label_ = 0;
    inline Object() {}
    inline Object(Rect2f xywh, std::vector<Point2f> landmark, float score, int32_t label, int32_t id = -1)
    {
        rect_ = xywh;
        score_ = score;
        label_ = label;
        id_ = id;
        landmark_ = landmark;
    }
    inline Object(float x, float y, float w, float h, std::vector<Point2f> landmark, float score, int32_t label,
                  int32_t id = -1)
    {
        rect_ = Rect2f(x, y, w, h);
        landmark_ = landmark;
        score_ = score;
        label_ = label;
        id_ = id;
    }
    friend bool
    operator<(const Object &o1, const Object &o2)
    {
        return o1.score_ < o2.score_;
    }
    friend bool
    operator>(const Object &o1, const Object &o2)
    {
        return o1.score_ > o2.score_;
    }
    friend bool
    operator==(const Object &o1, const Object &o2)
    {
        return o1.score_ == o2.score_;
    }
    friend bool
    operator!=(const Object &o1, const Object &o2)
    {
        return o1.score_ != o2.score_;
    }
    friend bool
    operator<=(const Object &o1, const Object &o2)
    {
        return o1.score_ <= o2.score_;
    }
    friend bool
    operator>=(const Object &o1, const Object &o2)
    {
        return o1.score_ >= o2.score_;
    }
};
struct Key_Point
{
    float x = 0;
    float y = 0;
    float vis_score = 0;
};

struct Kpt_Object
{
    Rect2f rect_;
    std::vector<Point_<float>> key_points_;
    std::vector<float> key_points_vis_;
    Key_Point veh_key_points_[16];
    float score_ = 0;
    int32_t id_ = 0;
    int32_t label_ = 0;
    float label_score_ = 0;
    Kpt_Object() = default;
    Kpt_Object(Rect2f xywh, std::vector<Point_<float>> key_points, std::vector<float> key_points_vis,
               float score, int32_t label, float label_score, int32_t id = -1)
    {
        rect_ = xywh;
        score_ = score;
        label_ = label;
        label_score_ = label_score;
        key_points_ = std::move(key_points);
        key_points_vis_ = std::move(key_points_vis);
        id_ = id;
    }
    Kpt_Object(float x, float y, float w, float h, std::vector<Point_<float>> key_points,
               std::vector<float> key_points_vis, float score, int32_t label, float label_score, int32_t id = -1)
    {
        rect_ = Rect2f(x, y, w, h);
        key_points_ = std::move(key_points);
        score_ = score;
        label_ = label;
        label_score_ = label_score;
        key_points_vis_ = std::move(key_points_vis);
        id_ = id;
    }
    friend bool
    operator<(const Kpt_Object &o1, const Kpt_Object &o2)
    {
        return o1.score_ < o2.score_;
    }
    friend bool
    operator>(const Kpt_Object &o1, const Kpt_Object &o2)
    {
        return o1.score_ > o2.score_;
    }
    friend bool
    operator==(const Kpt_Object &o1, const Kpt_Object &o2)
    {
        return o1.score_ == o2.score_;
    }
    friend bool
    operator!=(const Kpt_Object &o1, const Kpt_Object &o2)
    {
        return o1.score_ != o2.score_;
    }
    friend bool
    operator<=(const Kpt_Object &o1, const Kpt_Object &o2)
    {
        return o1.score_ <= o2.score_;
    }
    friend bool
    operator>=(const Kpt_Object &o1, const Kpt_Object &o2)
    {
        return o1.score_ >= o2.score_;
    }
};

struct GridAndStride
{
    int grid0;
    int grid1;
    int stride_w;
    int stride_h;
    GridAndStride() : grid0(0), grid1(0), stride_w(0), stride_h(0) {}
    GridAndStride(int g0, int g1, int s_w, int s_h)
    {
        grid0 = g0;
        grid1 = g1;
        stride_w = s_w;
        stride_h = s_h;
    }
};
}
}
#endif
#ifndef _SL_DATA_TYPES_BASE_H_
#define _SL_DATA_TYPES_BASE_H_

#include <algorithm>
#include <iostream>
#include <math.h>
#include <ostream>

namespace sonli
{
    template <typename _Tp>
    struct Point_
    {
        _Tp x_{};
        _Tp y_{};

        Point_() = default;
        Point_(_Tp x, _Tp y) : x_(x), y_(y) {}

        inline friend Point_ operator+(const Point_ &p1, const Point_ &p2)
        {
            return Point_(p1.x_ + p2.x_, p1.y_ + p2.y_);
        }

        inline friend Point_ operator-(const Point_ &p1, const Point_ &p2)
        {
            return Point_(p1.x_ - p2.x_, p1.y_ - p2.y_);
        }

        inline friend Point_ operator/(const Point_ &p1, _Tp s)
        {
            return Point_(p1.x_ / s, p1.y_ / s);
        }

        inline friend Point_ operator*(const Point_ &p1, _Tp s)
        {
            return Point_(p1.x_ * s, p1.y_ * s);
        }


    };

    typedef Point_<int> Point;
    typedef Point_<float> Point2f;

    template <typename _Tp>
    struct Rect_
    {
        _Tp x_{};
        _Tp y_{};
        _Tp width_{};
        _Tp height_{};
        Rect_() = default;
        Rect_(_Tp _x, _Tp _y, _Tp _w, _Tp _h)
        {
            x_ = _x;
            y_ = _y;
            width_ = _w;
            height_ = _h;
        }
        //  area (width*height) of the rectangle
        _Tp
        area() const
        {
            return width_ * height_;
        }
        /**
         * @brief   calculate rectangle intersection
         *
         * @param self
         * @param other
         * @return Rect_<_Tp>
         */
        friend Rect_<_Tp>
        operator&(const Rect_ &self, const Rect_ &other)
        {
            _Tp x1 = (_Tp)std::max(self.x_, other.x_);
            _Tp x2 = (_Tp)std::min(self.x_ + self.width_, other.x_ + other.width_);
            _Tp y1 = (_Tp)std::max(self.y_, other.y_);
            _Tp y2 = (_Tp)std::min(self.y_ + self.height_, other.y_ + other.height_);
            if (y2 <= y1 || x2 <= x1)
            {
                return Rect_<_Tp>(0, 0, 0, 0);
            }
            return Rect_<_Tp>(x1, y1, x2 - x1, y2 - y1);
        }

        friend std::ostream &
        operator<<(std::ostream &os, const Rect_ &obj)
        {
            return os
                   << "(x_: " << obj.x_
                   << " y_: " << obj.y_
                   << " width_: " << obj.width_
                   << " height_: " << obj.height_ << ")";
        }
    };

    typedef Rect_<float> Rect2f;
    typedef Rect_<int> Rect;

    template <typename _Tp>
    struct Line_
    {
        Point_<_Tp> start_point_;
        Point_<_Tp> end_point_;
        Line_() = default;
        Line_(Point_<_Tp> start, Point_<_Tp> end)
            : start_point_(start), end_point_(end) {}
    };

    typedef Line_<int> Line;
    typedef Line_<float> Line2f;

    template <typename _Tp>
    struct BBox_
    {
        _Tp x1_ = 0;
        _Tp x2_ = 0;
        _Tp y1_ = 0;
        _Tp y2_ = 0;
        BBox_() = default;
        BBox_(_Tp x1, _Tp y1, _Tp x2, _Tp y2)
        {
            x1_ = x1;
            y1_ = y1;
            x2_ = x2;
            y2_ = y2;
        }
        _Tp
        Width() const
        {
            return x2_ - x1_;
        }
        _Tp
        Height() const
        {
            return y2_ - y1_;
        }
        _Tp
        CenterX() const
        {
            return (x1_ + x2_) / 2;
        }
        _Tp
        CenterY() const
        {
            return (y1_ + y2_) / 2;
        }
        //  area (width*height) of the rectangle
        _Tp
        area() const
        {
            return (x2_ - x1_) * (y2_ - y1_);
        }
        /***
         * @description:  rectangle intersection
         * @param {*}
         * @return {*}
         */
        friend BBox_<_Tp>
        operator&(const BBox_ &self, const BBox_ &other)
        {
            _Tp x1 = (_Tp)std::max(self.x1_, other.x1_);
            _Tp x2 = (_Tp)std::min(self.x2_, other.x2_);
            _Tp y1 = (_Tp)std::max(self.y1_, other.y1_);
            _Tp y2 = (_Tp)std::min(self.y2_, other.y2_);
            return BBox_<_Tp>(x1, y1, x2, y2);
        }
    };
    typedef BBox_<float> BBox2f;
    typedef BBox_<int> BBox;

} // namespace sonli

#endif
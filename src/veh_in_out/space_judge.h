#ifndef _INOUT_SPACE_JUDGE_H_
#define _INOUT_SPACE_JUDGE_H_

#include "../utils/time_utils.h"
#include "opencv2/core/types.hpp"
#include "opencv2/imgproc.hpp"
#include "sl_data_types_base.h"

#include <vector>

namespace sonli
{

float inline
iou_poly(const std::vector<Point> &car, const std::vector<Point> &space, float &carIou, float &spaceIou)
{

    // 定义两个不规则四边形的顶点
    std::vector<cv::Point> polygon1;
    std::vector<cv::Point> polygon2;

    for (int i = 0; i < 4; i++)
    {
        polygon1.push_back(cv::Point(car[i].x_, car[i].y_));
        polygon2.push_back(cv::Point(space[i].x_, space[i].y_));
    }

    // 计算两个多边形的交集
    std::vector<cv::Point> intersectionPolygon;
    cv::intersectConvexConvex(polygon1, polygon2, intersectionPolygon);

    // 计算交集和并集的面积
    double intersectionArea = 0.0;
    if (intersectionPolygon.size() >= 3)
    {
        intersectionArea = cv::contourArea(intersectionPolygon);
    }
    else
    {
        carIou = 0;
        spaceIou = 0;
        return 0.0;
    }
    if (intersectionArea < 0.1)
    {
        carIou = 0;
        spaceIou = 0;
        return 0.0;
    }

    // 计算两个多边形的并集
    std::vector<cv::Point> unionPolygon;
    cv::convexHull(polygon1, unionPolygon);
    cv::convexHull(polygon2, unionPolygon);

    double unionArea = cv::contourArea(unionPolygon);

    double carArea = cv::contourArea(polygon1);
    double spaceArea = cv::contourArea(polygon2);

    // 计算 IoU
    double ioutt = intersectionArea / unionArea;

    if (carArea < 1e-3 || spaceArea < 1e-3)
    {
        carIou = 0;
        spaceIou = 0;
        return 0.0;
    }
    carIou = intersectionArea / carArea;
    spaceIou = intersectionArea / spaceArea;

    return ioutt;
}

std::string inline
JudgeSpace(const std::vector<std::vector<Point> > &cheWs, const std::vector<Point> &veh_chassis,
           float &spaceRat, std::vector<float> &spaceRatVec, float car_in_rat, float car_line_rat, int &space_id,
           std::vector<float> *carRatVec = nullptr)
{

    if (cheWs.size() < 1 || veh_chassis.size() != 4)
    {
        return "no";
    }

    // int spaceCount = cheWs.size();

    float carIou = 0.0;
    float spaceIou = 0.0;
    float iou = 0.0;
    float carIouSum = 0.0;
    float spaceIouSum = 0.0;

    float maxSpaceIou = 0.0;
    // float maxCarIou = 0.0;

    int car_space_count = 0; //车辆占几个车位

    for (long unsigned int  i = 0; i < cheWs.size(); i++)
    {
        carIou = 0.0;
        spaceIou = 0.0;
        (void)iou_poly(veh_chassis, cheWs[i], carIou, spaceIou);

        carIouSum = carIouSum + carIou;
        spaceIouSum = spaceIouSum + spaceIou;

        spaceRatVec.push_back(spaceIou);
        if (carRatVec)
        {
            carRatVec->push_back(carIou);
        }

        if (spaceIou > maxSpaceIou)
        {
            space_id = i;
            maxSpaceIou = spaceIou;
        }
        if (spaceIou > 0.05 || carIou > 0.1)
        {
            car_space_count++;
        }
    }

    // spaceRat = std::max(carIouSum, spaceIouSum);
    spaceRat = (carIouSum + spaceIouSum) / 2.0;

    if (car_space_count > 1)
    {
        spaceRat = std::fmax(spaceRat, car_in_rat);
        return "yes";
    }

    if (spaceRat < car_line_rat)
    {
        space_id = -1;
        return "no";
    }

    if (spaceRat < car_in_rat)
    {
        return "oh";
    }

    return "yes";
}
} // namespace sonli

#endif
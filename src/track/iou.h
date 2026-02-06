#ifndef FALCONEYE_IOU_H
#define FALCONEYE_IOU_H

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

namespace sonli
{
namespace basic_algorithm
{
inline double
crossProduct(double x1, double y1, double x2, double y2)
{
    return x1 * y2 - x2 * y1;
}

inline bool
isPointOnInnerSide(const std::vector<double> &point, const std::vector<double> &edgeStart,
                   const std::vector<double> &edgeEnd)
{
    double cross = crossProduct(edgeEnd[0] - edgeStart[0], edgeEnd[1] - edgeStart[1], point[0] - edgeStart[0],
                                point[1] - edgeStart[1]);
    return cross <= 1e-4;
}

inline std::vector<double>
lineSegmentIntersection(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
{
    double den = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    if (std::fabs(den) < 1e-4)
    {
        return {};
    }

    double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / den;
    double u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / den;

    if (t >= -1e-4 && t <= 1.0 + 1e-4 && u >= -1e-4 && u <= 1.0 + 1e-4)
    {
        return {x1 + t * (x2 - x1), y1 + t * (y2 - y1)};
    }
    return {};
}

inline std::vector<std::vector<double>>
clipConvexPolygon(const std::vector<std::vector<double>> &subjectPoly, const std::vector<std::vector<double>> &clipPoly)
{
    if (subjectPoly.size() < 3 || clipPoly.size() < 3)
        return {};

    std::vector<std::vector<double>> outputPoly = subjectPoly;
    for (size_t i = 0; i < clipPoly.size(); ++i)
    {
        const auto &edgeStart = clipPoly[i];
        const auto &edgeEnd = clipPoly[(i + 1) % clipPoly.size()];
        std::vector<std::vector<double>> inputPoly = outputPoly;
        outputPoly.clear();
        if (inputPoly.empty())
            break;

        std::vector<double> prevPoint = inputPoly.back();
        for (const auto &currPoint : inputPoly)
        {
            if (isPointOnInnerSide(currPoint, edgeStart, edgeEnd))
            {
                if (!isPointOnInnerSide(prevPoint, edgeStart, edgeEnd))
                {
                    auto intersect = lineSegmentIntersection(prevPoint[0], prevPoint[1], currPoint[0], currPoint[1],
                                                             edgeStart[0], edgeStart[1], edgeEnd[0], edgeEnd[1]);
                    if (!intersect.empty())
                        outputPoly.push_back(intersect);
                }
                outputPoly.push_back(currPoint);
            }
            else if (isPointOnInnerSide(prevPoint, edgeStart, edgeEnd))
            {
                auto intersect = lineSegmentIntersection(prevPoint[0], prevPoint[1], currPoint[0], currPoint[1],
                                                         edgeStart[0], edgeStart[1], edgeEnd[0], edgeEnd[1]);
                if (!intersect.empty())
                    outputPoly.push_back(intersect);
            }
            prevPoint = currPoint;
        }
    }
    return outputPoly;
}

inline double
polygonArea(const std::vector<std::vector<double>> &poly)
{
    if (poly.size() < 3)
        return 0.0;
    double area = 0.0;
    for (size_t i = 0; i < poly.size(); ++i)
    {
        size_t j = (i + 1) % poly.size();
        area += poly[i][0] * poly[j][1] - poly[j][0] * poly[i][1];
    }
    return std::fabs(area) / 2.0;
}

inline double
polygonArea(const std::vector<double> &points)
{
    if (points.size() < 6 || points.size() % 2 != 0)
        return 0.0;
    std::vector<std::vector<double>> poly;
    for (size_t i = 0; i < points.size(); i += 2)
    {
        poly.push_back({points[i], points[i + 1]});
    }
    return polygonArea(poly);
}

inline double
intersectionArea(const std::vector<double> &poly1, const std::vector<double> &poly2)
{
    if (poly1.size() < 6 || poly2.size() % 2 != 0)
        return 0.0;

    std::vector<std::vector<double>> poly1_vertices;
    for (size_t i = 0; i < poly1.size(); i += 2)
    {
        poly1_vertices.push_back({poly1[i], poly1[i + 1]});
    }
    std::vector<std::vector<double>> poly2_vertices;
    for (size_t i = 0; i < poly2.size(); i += 2)
    {
        poly2_vertices.push_back({poly2[i], poly2[i + 1]});
    }

    auto inter1 = clipConvexPolygon(poly1_vertices, poly2_vertices);
    auto inter2 = clipConvexPolygon(poly2_vertices, poly1_vertices);

    double area1 = polygonArea(inter1);
    double area2 = polygonArea(inter2);

    return std::max(area1, area2);
}

// 计算矩形框轨迹的IOU距离矩阵（1-IOU）
inline double
calculateIOU(const std::vector<double> &poly1, const std::vector<double> &poly2)
{
    double area1 = polygonArea(poly1);
    double area2 = polygonArea(poly2);

    if (area1 < 1e-4 || area2 < 1e-4)
        return 0.0;

    double intersection = intersectionArea(poly1, poly2);
    double union_area = area1 + area2 - intersection;

    return union_area < 1e-4 ? 0.0 : (intersection / union_area);
}
} // namespace basic_algorithm
} // namespace sonli

#endif // FALCONEYE_IOU_H

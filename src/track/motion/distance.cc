/*
 * @Author: lwn
 * @Date: 2024-04-24 13:27:44
 * @LastEditors: lwn
 * @LastEditTime: 2024-04-25 15:51:45
 * @FilePath: \FalconEye\src\track\motion\distance.cc
 * @Description:
 */
#include "../track/iou.h"
#include "../track/motion/levenshtein.h"
#include "../track/trajectory.h"
namespace sonli
{
cv::Mat
embedding_distance(const TrajectoryPool &a, const TrajectoryPool &b)
{
    cv::Mat dists(a.size(), b.size(), CV_32F);
    for (size_t i = 0; i < a.size(); ++i)
    {
        float *distsi = dists.ptr<float>(i);
        for (size_t j = 0; j < b.size(); ++j)
        {
            cv::Mat u = a[i].smooth_embedding_;
            cv::Mat v = b[j].smooth_embedding_;
            double uv = u.dot(v);
            double uu = u.dot(u);
            double vv = v.dot(v);
            double dist = std::abs(1. - uv / std::sqrt(uu * vv));
            distsi[j] = static_cast<float>(std::max(std::min(dist, 2.), 0.));
        }
    }
    return dists;
}

cv::Mat
embedding_distance(const TrajectoryPtrPool &a, const TrajectoryPtrPool &b)
{
    cv::Mat dists(a.size(), b.size(), CV_32F);
    for (size_t i = 0; i < a.size(); ++i)
    {
        float *distsi = dists.ptr<float>(i);
        for (size_t j = 0; j < b.size(); ++j)
        {
            cv::Mat u = a[i]->smooth_embedding_;
            cv::Mat v = b[j]->smooth_embedding_;
            double uv = u.dot(v);
            double uu = u.dot(u);
            double vv = v.dot(v);
            double dist = std::abs(1. - uv / std::sqrt(uu * vv));
            distsi[j] = static_cast<float>(std::max(std::min(dist, 2.), 0.));
        }
    }

    return dists;
}

cv::Mat
embedding_distance(const TrajectoryPtrPool &a, const TrajectoryPool &b)
{
    cv::Mat dists(a.size(), b.size(), CV_32F);
    for (size_t i = 0; i < a.size(); ++i)
    {
        float *distsi = dists.ptr<float>(i);
        for (size_t j = 0; j < b.size(); ++j)
        {
            cv::Mat u = a[i]->smooth_embedding_;
            cv::Mat v = b[j].smooth_embedding_;
            double uv = u.dot(v);
            double uu = u.dot(u);
            double vv = v.dot(v);
            double dist = std::abs(1. - uv / std::sqrt(uu * vv));
            distsi[j] = static_cast<float>(std::max(std::min(dist, 2.), 0.));
        }
    }

    return dists;
}

cv::Mat
mahalanobis_distance(const TrajectoryPool &a, const TrajectoryPool &b)
{
    std::vector<cv::Mat> means(a.size());
    std::vector<cv::Mat> icovariances(a.size());
    for (size_t i = 0; i < a.size(); ++i)
    {
        cv::Mat covariance;
        a[i].project(&means[i], &covariance);
        cv::invert(covariance, icovariances[i]);
    }

    cv::Mat dists(a.size(), b.size(), CV_32F);
    for (size_t i = 0; i < a.size(); ++i)
    {
        float *distsi = dists.ptr<float>(i);
        for (size_t j = 0; j < b.size(); ++j)
        {
            const cv::Mat x(b[j].xyah_);
            float dist = static_cast<float>(cv::Mahalanobis(x, means[i], icovariances[i]));
            distsi[j] = dist * dist;
        }
    }

    return dists;
}

cv::Mat
mahalanobis_distance(const TrajectoryPtrPool &a, const TrajectoryPtrPool &b)
{
    std::vector<cv::Mat> means(a.size());
    std::vector<cv::Mat> icovariances(a.size());
    for (size_t i = 0; i < a.size(); ++i)
    {
        cv::Mat covariance;
        a[i]->project(&means[i], &covariance);
        cv::invert(covariance, icovariances[i]);
    }

    cv::Mat dists(a.size(), b.size(), CV_32F);
    for (size_t i = 0; i < a.size(); ++i)
    {
        float *distsi = dists.ptr<float>(i);
        for (size_t j = 0; j < b.size(); ++j)
        {
            const cv::Mat x(b[j]->xyah_);
            float dist = static_cast<float>(cv::Mahalanobis(x, means[i], icovariances[i]));
            distsi[j] = dist * dist;
        }
    }

    return dists;
}

cv::Mat
mahalanobis_distance(const TrajectoryPtrPool &a, const TrajectoryPool &b)
{
    std::vector<cv::Mat> means(a.size());
    std::vector<cv::Mat> icovariances(a.size());

    for (size_t i = 0; i < a.size(); ++i)
    {
        cv::Mat covariance;
        a[i]->project(&means[i], &covariance);
        cv::invert(covariance, icovariances[i]);
    }

    cv::Mat dists(a.size(), b.size(), CV_32F);
    for (size_t i = 0; i < a.size(); ++i)
    {
        float *distsi = dists.ptr<float>(i);
        for (size_t j = 0; j < b.size(); ++j)
        {
            const cv::Mat x(b[j].xyah_);
            float dist = static_cast<float>(cv::Mahalanobis(x, means[i], icovariances[i]));
            distsi[j] = dist * dist;
        }
    }

    return dists;
}

static inline float
calc_inter_area(const cv::Vec4f &a, const cv::Vec4f &b)
{
    if (a[2] < b[0] || a[0] > b[2] || a[3] < b[1] || a[1] > b[3])
        return 0.f;

    float w = std::min(a[2], b[2]) - std::max(a[0], b[0]);
    float h = std::min(a[3], b[3]) - std::max(a[1], b[1]);
    return w * h;
}

static inline float
scale_short_long_edge(const cv::Vec4f &a, const cv::Vec4f &b, bool use_height = false)
{
    int offset_ = use_height ? 1 : 0;
    float short_ = std::min(a[2 + offset_], b[2 + offset_]) - std::max(a[0 + offset_], b[0 + offset_]);
    float long_ = std::max(a[2 + offset_], b[2 + offset_]) - std::min(a[0 + offset_], b[0 + offset_]);
    return short_ / long_;
}

cv::Mat
iou_distance(const TrajectoryPool &a, const TrajectoryPool &b)
{
    std::vector<float> areaa(a.size());
    for (size_t i = 0; i < a.size(); ++i)
    {
        float w = a[i].ltrb_[2] - a[i].ltrb_[0];
        float h = a[i].ltrb_[3] - a[i].ltrb_[1];
        areaa[i] = w * h;
    }

    std::vector<float> areab(b.size());
    for (size_t j = 0; j < b.size(); ++j)
    {
        float w = b[j].ltrb_[2] - b[j].ltrb_[0];
        float h = b[j].ltrb_[3] - b[j].ltrb_[1];
        areab[j] = w * h;
    }

    cv::Mat dists(a.size(), b.size(), CV_32F);
    for (size_t i = 0; i < a.size(); ++i)
    {
        const cv::Vec4f &boxa = a[i].ltrb_;
        float *distsi = dists.ptr<float>(i);
        for (size_t j = 0; j < b.size(); ++j)
        {
            const cv::Vec4f &boxb = b[j].ltrb_;
            float inters = calc_inter_area(boxa, boxb);
            distsi[j] = 1.f - inters / (areaa[i] + areab[j] - inters);
        }
    }

    return dists;
}

cv::Mat
iou_distance(const TrajectoryPtrPool &a, const TrajectoryPtrPool &b)
{
    std::vector<float> areaa(a.size());
    for (size_t i = 0; i < a.size(); ++i)
    {
        float w = a[i]->ltrb_[2] - a[i]->ltrb_[0];
        float h = a[i]->ltrb_[3] - a[i]->ltrb_[1];
        areaa[i] = w * h;
    }

    std::vector<float> areab(b.size());
    for (size_t j = 0; j < b.size(); ++j)
    {
        float w = b[j]->ltrb_[2] - b[j]->ltrb_[0];
        float h = b[j]->ltrb_[3] - b[j]->ltrb_[1];
        areab[j] = w * h;
    }

    cv::Mat dists(a.size(), b.size(), CV_32F);
    for (size_t i = 0; i < a.size(); ++i)
    {
        const cv::Vec4f &boxa = a[i]->ltrb_;
        float *distsi = dists.ptr<float>(i);
        for (size_t j = 0; j < b.size(); ++j)
        {
            const cv::Vec4f &boxb = b[j]->ltrb_;
            float inters = calc_inter_area(boxa, boxb);
            distsi[j] = 1.f - inters / (areaa[i] + areab[j] - inters);
        }
    }

    return dists;
}

cv::Mat
iou_distance(const TrajectoryPtrPool &a, const TrajectoryPool &b)
{
    std::vector<float> areaa(a.size());
    for (size_t i = 0; i < a.size(); ++i)
    {
        float w = a[i]->ltrb_[2] - a[i]->ltrb_[0];
        float h = a[i]->ltrb_[3] - a[i]->ltrb_[1];
        areaa[i] = w * h;
    }

    std::vector<float> areab(b.size());
    for (size_t j = 0; j < b.size(); ++j)
    {
        float w = b[j].ltrb_[2] - b[j].ltrb_[0];
        float h = b[j].ltrb_[3] - b[j].ltrb_[1];
        areab[j] = w * h;
    }

    cv::Mat dists(a.size(), b.size(), CV_32F);
    for (size_t i = 0; i < a.size(); ++i)
    {
        const cv::Vec4f &boxa = a[i]->ltrb_;
        float *distsi = dists.ptr<float>(i);
        for (size_t j = 0; j < b.size(); ++j)
        {
            const cv::Vec4f &boxb = b[j].ltrb_;
            float inters = calc_inter_area(boxa, boxb);
            distsi[j] = 1.f - inters / (areaa[i] + areab[j] - inters);
        }
    }

    return dists;
}

cv::Mat
miou_distance(const TrajectoryPool &a, const TrajectoryPool &b, bool use_height)
{
    std::vector<float> areaa(a.size());
    for (size_t i = 0; i < a.size(); ++i)
    {
        float w = a[i].ltrb_[2] - a[i].ltrb_[0];
        float h = a[i].ltrb_[3] - a[i].ltrb_[1];
        areaa[i] = w * h;
    }

    std::vector<float> areab(b.size());
    for (size_t j = 0; j < b.size(); ++j)
    {
        float w = b[j].ltrb_[2] - b[j].ltrb_[0];
        float h = b[j].ltrb_[3] - b[j].ltrb_[1];
        areab[j] = w * h;
    }

    cv::Mat dists(a.size(), b.size(), CV_32F);
    for (size_t i = 0; i < a.size(); ++i)
    {
        const cv::Vec4f &boxa = a[i].ltrb_;
        float *distsi = dists.ptr<float>(i);
        for (size_t j = 0; j < b.size(); ++j)
        {
            const cv::Vec4f &boxb = b[j].ltrb_;
            float scale = scale_short_long_edge(boxa, boxb, use_height);
            float inters = calc_inter_area(boxa, boxb);
            distsi[j] = 1.f - scale * inters / (areaa[i] + areab[j] - inters);
        }
    }

    return dists;
}

cv::Mat
miou_distance(const TrajectoryPtrPool &a, const TrajectoryPtrPool &b, bool use_height)
{
    std::vector<float> areaa(a.size());
    for (size_t i = 0; i < a.size(); ++i)
    {
        float w = a[i]->ltrb_[2] - a[i]->ltrb_[0];
        float h = a[i]->ltrb_[3] - a[i]->ltrb_[1];
        areaa[i] = w * h;
    }

    std::vector<float> areab(b.size());
    for (size_t j = 0; j < b.size(); ++j)
    {
        float w = b[j]->ltrb_[2] - b[j]->ltrb_[0];
        float h = b[j]->ltrb_[3] - b[j]->ltrb_[1];
        areab[j] = w * h;
    }

    cv::Mat dists(a.size(), b.size(), CV_32F);
    for (size_t i = 0; i < a.size(); ++i)
    {
        const cv::Vec4f &boxa = a[i]->ltrb_;
        float *distsi = dists.ptr<float>(i);
        for (size_t j = 0; j < b.size(); ++j)
        {
            const cv::Vec4f &boxb = b[j]->ltrb_;
            float scale = scale_short_long_edge(boxa, boxb, use_height);
            float inters = calc_inter_area(boxa, boxb);
            distsi[j] = 1.f - scale * inters / (areaa[i] + areab[j] - inters);
        }
    }

    return dists;
}

cv::Mat
miou_distance(const TrajectoryPtrPool &a, const TrajectoryPool &b, bool use_height)
{
    std::vector<float> areaa(a.size());
    for (size_t i = 0; i < a.size(); ++i)
    {
        float w = a[i]->ltrb_[2] - a[i]->ltrb_[0];
        float h = a[i]->ltrb_[3] - a[i]->ltrb_[1];
        areaa[i] = w * h;
    }

    std::vector<float> areab(b.size());
    for (size_t j = 0; j < b.size(); ++j)
    {
        float w = b[j].ltrb_[2] - b[j].ltrb_[0];
        float h = b[j].ltrb_[3] - b[j].ltrb_[1];
        areab[j] = w * h;
    }

    cv::Mat dists(a.size(), b.size(), CV_32F);
    for (size_t i = 0; i < a.size(); ++i)
    {
        const cv::Vec4f &boxa = a[i]->ltrb_;
        float *distsi = dists.ptr<float>(i);
        for (size_t j = 0; j < b.size(); ++j)
        {
            const cv::Vec4f &boxb = b[j].ltrb_;
            float scale = scale_short_long_edge(boxa, boxb, use_height);
            float inters = calc_inter_area(boxa, boxb);
            distsi[j] = 1.f - scale * inters / (areaa[i] + areab[j] - inters);
        }
    }

    return dists;
}

cv::Mat
levenshtein_distance(const TrajectoryPool &a, const TrajectoryPool &b)
{
    cv::Mat dists(a.size(), b.size(), CV_32F);

    for (size_t i = 0; i < a.size(); ++i)
    {
        std::string p1 = a[i].plate_;
        float *distsi = dists.ptr<float>(i);
        for (size_t j = 0; j < b.size(); ++j)
        {
            std::string p2 = b[j].plate_;
            distsi[j] = Levenshtein::normalized_distance(p1, p2);
        }
    }

    return dists;
}

cv::Mat
levenshtein_distance(const TrajectoryPtrPool &a, const TrajectoryPtrPool &b)
{
    cv::Mat dists(a.size(), b.size(), CV_32F);

    for (size_t i = 0; i < a.size(); ++i)
    {
        std::string p1 = a[i]->plate_;
        float *distsi = dists.ptr<float>(i);
        for (size_t j = 0; j < b.size(); ++j)
        {
            std::string p2 = b[j]->plate_;
            distsi[j] = Levenshtein::normalized_distance(p1, p2);
        }
    }

    return dists;
}

cv::Mat
levenshtein_distance(const TrajectoryPtrPool &a, const TrajectoryPool &b)
{
    cv::Mat dists(a.size(), b.size(), CV_32F);

    for (size_t i = 0; i < a.size(); ++i)
    {
        std::string p1 = a[i]->plate_;
        float *distsi = dists.ptr<float>(i);
        for (size_t j = 0; j < b.size(); ++j)
        {
            std::string p2 = b[j].plate_;
            distsi[j] = Levenshtein::normalized_distance(p1, p2);
        }
    }

    return dists;
}

cv::Mat
miou_distance_parallelogram(const TrajectoryPtrPool &a, const TrajectoryPool &b)
{
    cv::Mat rm = cv::Mat::ones(a.size(), b.size(), CV_32F);
    for (size_t i = 0; i < a.size(); ++i)
    {
        float *distsi = rm.ptr<float>(i);
        auto chassis_a = a[i]->chassis_.reshape(1, 1);
        std::vector<float> a_coord(chassis_a.begin<float>(), chassis_a.end<float>());
        for (size_t j = 0; j < b.size(); ++j)
        {
            if (b[j].score_ < 0.76 || b[j].chassis_.empty())
                continue;
            auto chassis_b = b[j].chassis_.reshape(1, 1);
            std::vector<float> b_coord(chassis_b.begin<float>(), chassis_b.end<float>());
            auto v = 1.0 - basic_algorithm::calculateIOU(chassis_a, chassis_b);
            distsi[j] = (float_t)v;
        }
    }
    return rm;
}

} // namespace sonli
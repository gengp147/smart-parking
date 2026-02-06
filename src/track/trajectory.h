/***
 * @Author: lwn
 * @Date: 2024-04-12 14:44:58
 * @LastEditors: lwn
 * @LastEditTime: 2024-06-27 09:51:35
 * @FilePath: \falconeye_debug\include\track\trajectory.h
 * @Description:
 */
#ifndef _TRACK_TRAJECTORY_H_
#define _TRACK_TRAJECTORY_H_

#include "opencv2/core.hpp"
#include <vector>
// #include "opencv2/highgui.hpp"
#include "../track/motion/improved_kalman_filter.h"
#include "data_type.h"

namespace sonli
{
// typedef enum {
//     New = 0,
//     Tracked = 1,
//     Lost = 2,
//     Removed = 3
// } TrajectoryState;
cv::Vec4f
ltrb2xyah(const cv::Vec4f &ltrb);
cv::Vec4f
ltrb2ltwh(const cv::Vec4f &ltrb);
class Trajectory;
typedef std::vector<Trajectory> TrajectoryPool;
typedef std::vector<Trajectory>::iterator TrajectoryPoolIterator;
typedef std::vector<Trajectory *> TrajectoryPtrPool;
typedef std::vector<Trajectory *>::iterator TrajectoryPtrPoolIterator;

class Trajectory : public ImprovedKalmanFilter
{
public:
    Trajectory();

    Trajectory(const cv::Vec4f &ltrb, float score, const cv::Mat &embedding, long timestamp,
               const cv::Mat &chassis = cv::Mat(), const std::string &plate = "");

    Trajectory(const Trajectory &other);

    Trajectory &
    operator=(const Trajectory &rhs);
    virtual ~Trajectory(void) {}

    virtual cv::Mat
    predict(void);
    virtual void
    update(Trajectory *traj, long timestamp, bool update_embedding_ = true);
    virtual void
    activate(long timestamp, bool enable_activate = false);
    virtual void reactivate(Trajectory *traj, long timestamp, bool update_embedding);
    virtual void
    mark_lost(void);
    virtual void
    mark_removed(void);

    void
    set_track_id(long id)
    {
        id_ = id;
    };

    friend TrajectoryPool
    operator+(const TrajectoryPool &a, const TrajectoryPool &b);
    friend TrajectoryPool
    operator+(const TrajectoryPool &a, const TrajectoryPtrPool &b);
    friend TrajectoryPool &
    operator+=(TrajectoryPool &a, // NOLINT
               const TrajectoryPtrPool &b);
    friend TrajectoryPool &
    operator+=(TrajectoryPool &a, // NOLINT
               const TrajectoryPool &b);
    friend TrajectoryPool
    operator-(const TrajectoryPool &a, const TrajectoryPool &b);
    friend TrajectoryPool &
    operator-=(TrajectoryPool &a, // NOLINT
               const TrajectoryPool &b);
    friend TrajectoryPool &
    operator-=(TrajectoryPool &a, // NOLINT
               const TrajectoryPtrPool &b);
    friend TrajectoryPtrPool
    operator+(const TrajectoryPtrPool &a, const TrajectoryPtrPool &b);
    friend TrajectoryPtrPool
    operator+(const TrajectoryPtrPool &a, TrajectoryPool *b);
    friend TrajectoryPtrPool
    operator-(const TrajectoryPtrPool &a, const TrajectoryPtrPool &b);

    friend cv::Mat
    embedding_distance(const TrajectoryPool &a, const TrajectoryPool &b);
    friend cv::Mat
    embedding_distance(const TrajectoryPtrPool &a, const TrajectoryPtrPool &b);
    friend cv::Mat
    embedding_distance(const TrajectoryPtrPool &a, const TrajectoryPool &b);

    friend cv::Mat
    mahalanobis_distance(const TrajectoryPool &a, const TrajectoryPool &b);
    friend cv::Mat
    mahalanobis_distance(const TrajectoryPtrPool &a, const TrajectoryPtrPool &b);
    friend cv::Mat
    mahalanobis_distance(const TrajectoryPtrPool &a, const TrajectoryPool &b);

    friend cv::Mat
    iou_distance(const TrajectoryPool &a, const TrajectoryPool &b);
    friend cv::Mat
    iou_distance(const TrajectoryPtrPool &a, const TrajectoryPtrPool &b);
    friend cv::Mat
    iou_distance(const TrajectoryPtrPool &a, const TrajectoryPool &b);

    friend cv::Mat
    miou_distance(const TrajectoryPool &a, const TrajectoryPool &b, bool use_height);
    friend cv::Mat
    miou_distance(const TrajectoryPtrPool &a, const TrajectoryPtrPool &b, bool use_height);
    friend cv::Mat
    miou_distance(const TrajectoryPtrPool &a, const TrajectoryPool &b, bool use_height);

    friend cv::Mat
    miou_distance_parallelogram(const TrajectoryPtrPool &a, const TrajectoryPool &b);

    friend cv::Mat
    levenshtein_distance(const TrajectoryPool &a, const TrajectoryPool &b);
    friend cv::Mat
    levenshtein_distance(const TrajectoryPtrPool &a, const TrajectoryPtrPool &b);
    friend cv::Mat
    levenshtein_distance(const TrajectoryPtrPool &a, const TrajectoryPool &b);

private:
    void
    update_embedding(const cv::Mat &embedding);
    void
    update_velocity(const cv::Mat &chassis);

public:
    TrackState state_ = TrackState::New;
    cv::Vec4f ltrb_ {};
    cv::Mat smooth_embedding_ {};
    long id_ = -1;
    bool is_activated = false;
    long timestamp_ = 0;
    long starttime_ = 0;
    float score_ = 0.0f;
    cv::Mat chassis_ {}; // shape [4, 2]
    cv::Mat velocities_ {};
    std::string plate_ {};
    // long length_;

private:
    cv::Vec4f xyah_ {};
    cv::Mat current_embedding_ {};
    float eta_ = 0.0f;
    float beta_ = 0.0f; // The coefficient of smooth velocity.
    int hit_streak_count_ = 0;
};

} // namespace sonli

#endif
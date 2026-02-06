/*
 * @Author: lwn
 * @Date: 2024-04-12 14:43:52
 * @LastEditors: lwn
 * @LastEditTime: 2024-06-27 09:53:42
 * @FilePath: \falconeye_debug\src\track\trajectory.cc
 * @Description:
 */
#include "../track/trajectory.h"
#include "../track/motion/calc_velocity.h"
// #include "common/config_content.h"

namespace sonli
{

// cv::Mat to cv::Vec4f
static inline cv::Vec4f
convertMatToVec4f(const cv::Mat &mat)
{
    assert(mat.type() == CV_32F && mat.rows == 4 && mat.cols == 1);
    cv::Vec4f vec;
    mat.copyTo(vec);
    return vec;
}

cv::Vec4f
ltrb2xyah(const cv::Vec4f &ltrb)
{
    cv::Vec4f xyah;
    xyah[0] = (ltrb[0] + ltrb[2]) * 0.5f;
    xyah[1] = (ltrb[1] + ltrb[3]) * 0.5f;
    xyah[3] = ltrb[3] - ltrb[1];
    xyah[2] = (ltrb[2] - ltrb[0]) / xyah[3]; // w / h
    return xyah;
}

cv::Vec4f
ltrb2ltwh(const cv::Vec4f &ltrb)
{
    cv::Vec4f xyah;
    xyah[0] = ltrb[0];
    xyah[1] = ltrb[1];
    xyah[3] = ltrb[3] - ltrb[1];
    xyah[2] = ltrb[2] - ltrb[0];
    return xyah;
}

cv::Vec4f
xyah2ltrb(const cv::Vec4f &xyah)
{
    cv::Vec4f ltrb;
    float w = xyah[3] * xyah[2];
    ltrb[0] = xyah[0] - w / 2;
    ltrb[2] = xyah[0] + w / 2;
    ltrb[1] = xyah[1] - xyah[3] / 2;
    ltrb[3] = xyah[1] + xyah[3] / 2;
    return ltrb;
}

Trajectory::Trajectory()
    : state_(TrackState::New),
      ltrb_(cv::Vec4f()),
      smooth_embedding_(cv::Mat()),
      id_(0),
      is_activated(false),
      timestamp_(0),
      starttime_(0),
      score_(0),
      chassis_(cv::Mat()),
      velocities_(cv::Mat()),
      plate_(),
      eta_(0.8),
      beta_(0.3),
      hit_streak_count_(0)
{
}

Trajectory::Trajectory(const cv::Vec4f &ltrb, float score, const cv::Mat &embedding, long timestamp,
                       const cv::Mat &chassis, const std::string &plate)
    : state_(TrackState::New),
      ltrb_(ltrb),
      smooth_embedding_(cv::Mat()),
      id_(0),
      is_activated(false),
      timestamp_(0),
      starttime_(0),
      score_(score),
      chassis_(chassis),
      velocities_(cv::Mat()),
      plate_(plate),
      eta_(0.8),
      beta_(0.3),
      hit_streak_count_(0)
{
    xyah_ = ltrb2xyah(ltrb);
    if (!embedding.empty())
    {
        update_embedding(embedding);
    }
}

Trajectory::Trajectory(const Trajectory &other)
    : state_(other.state_),
      ltrb_(other.ltrb_),
      id_(other.id_),
      is_activated(other.is_activated),
      timestamp_(other.timestamp_),
      starttime_(other.starttime_),
      score_(other.score_),
      plate_(other.plate_),
      xyah_(other.xyah_),
      eta_(other.eta_),
      beta_(other.beta_),
      hit_streak_count_(other.hit_streak_count_)
{
    other.smooth_embedding_.copyTo(smooth_embedding_);
    other.current_embedding_.copyTo(current_embedding_);
    // copy state in KalmanFilter

    other.chassis_.copyTo(chassis_);

    other.velocities_.copyTo(velocities_);

    other.statePre.copyTo(cv::KalmanFilter::statePre);
    other.statePost.copyTo(cv::KalmanFilter::statePost);
    other.errorCovPre.copyTo(cv::KalmanFilter::errorCovPre);
    other.errorCovPost.copyTo(cv::KalmanFilter::errorCovPost);
}

Trajectory &
Trajectory::operator=(const Trajectory &rhs)
{
    this->state_ = rhs.state_;
    this->ltrb_ = rhs.ltrb_;
    if (!rhs.smooth_embedding_.empty())
    {
        rhs.smooth_embedding_.copyTo(this->smooth_embedding_);
        rhs.current_embedding_.copyTo(this->current_embedding_);
    }
    rhs.chassis_.copyTo(this->chassis_);
    this->plate_ = rhs.plate_;
    this->id_ = rhs.id_;
    this->is_activated = rhs.is_activated;
    this->timestamp_ = rhs.timestamp_;
    this->starttime_ = rhs.starttime_;
    this->xyah_ = rhs.xyah_;
    this->score_ = rhs.score_;
    rhs.current_embedding_.copyTo(this->current_embedding_);
    rhs.velocities_.copyTo(this->velocities_);
    this->eta_ = rhs.eta_;
    this->beta_ = rhs.beta_;
    this->hit_streak_count_ = rhs.hit_streak_count_;

    // copy state in KalmanFilter

    rhs.statePre.copyTo(cv::KalmanFilter::statePre);
    rhs.statePost.copyTo(cv::KalmanFilter::statePost);
    rhs.errorCovPre.copyTo(cv::KalmanFilter::errorCovPre);
    rhs.errorCovPost.copyTo(cv::KalmanFilter::errorCovPost);

    return *this;
}

cv::Mat
Trajectory::predict()
{
    if (state_ != TrackState::Tracked)
        *cv::KalmanFilter::statePost.ptr<float>(7) = 0;
    cv::Mat rpred = ImprovedKalmanFilter::predict();
    // float vx = pred.at<float>(4);
    // float vy = pred.at<float>(5);
    // float vw = pred.at<float>(6);
    // float vh = pred.at<float>(7);
    // if (id_ == 4)
    // {
    //     std::cout << "v = (" << vx
    //     <<"," << vy
    //     <<"," << vw
    //     <<"," << vh << ")" <<  std::endl;
    // }
    xyah_ = convertMatToVec4f(rpred(cv::Rect(0, 0, 1, 4)));
    ltrb_ = xyah2ltrb(xyah_);
    // return ImprovedKalmanFilter::predict();
    return rpred;
}

void
Trajectory::update(Trajectory *traj, long timestamp, bool update_embedding_)
{
    timestamp_ = timestamp;
    ++hit_streak_count_;
    ltrb_ = traj->ltrb_;
    xyah_ = traj->xyah_;
    ImprovedKalmanFilter::correct(cv::Mat(traj->xyah_));

    if (hit_streak_count_ >= MIN_STREAK_COUNT)
    {
        state_ = TrackState::Tracked;
        is_activated = true;
    }
    score_ = traj->score_;
    velocities_ = calcVelocities(traj->chassis_, chassis_);
    plate_ = traj->plate_;
    traj->chassis_.copyTo(chassis_);
    if (update_embedding_ && !traj->current_embedding_.empty())
    {
        update_embedding(traj->current_embedding_);

    }
}

void
Trajectory::activate(long timestamp, bool enable_activate)
{
    ImprovedKalmanFilter::init(cv::Mat(xyah_));
    if (hit_streak_count_ >= MIN_STREAK_COUNT && enable_activate)
    {
        is_activated = true;
    }
    timestamp_ = timestamp;
    starttime_ = timestamp;
}

void Trajectory::reactivate(Trajectory *traj, long timestamp, bool is_update_embedding) {
    ImprovedKalmanFilter::correct(cv::Mat(traj->xyah_));
    if (is_update_embedding &&!traj->current_embedding_.empty() )
    {
        update_embedding(traj->current_embedding_);
    }
    chassis_ = traj->chassis_;
    plate_ = traj->plate_;

    hit_streak_count_= 0;
    state_ = TrackState::Tracked;
    is_activated = true;
    timestamp_ = timestamp;
}

void
Trajectory::mark_lost(void)
{
    state_ = TrackState::Lost;
}

void
Trajectory::mark_removed(void)
{
    state_ = TrackState::Removed;
}

TrajectoryPool
operator+(const TrajectoryPool &a, const TrajectoryPool &b)
{
    TrajectoryPool sum;
    sum.insert(sum.end(), a.begin(), a.end());

    std::vector<int> ids(a.size());
    for (size_t i = 0; i < a.size(); ++i)
        ids[i] = a[i].id_;

    for (size_t i = 0; i < b.size(); ++i)
    {
        auto iter = find(ids.begin(), ids.end(), b[i].id_);
        if (iter == ids.end())
        {
            sum.push_back(b[i]);
            ids.push_back(b[i].id_);
        }
    }

    return sum;
}

TrajectoryPool
operator+(const TrajectoryPool &a, const TrajectoryPtrPool &b)
{
    TrajectoryPool sum;
    sum.insert(sum.end(), a.begin(), a.end());

    std::vector<int> ids(a.size());
    for (size_t i = 0; i < a.size(); ++i)
        ids[i] = a[i].id_;

    for (size_t i = 0; i < b.size(); ++i)
    {
        auto iter = find(ids.begin(), ids.end(), b[i]->id_);
        if (iter == ids.end())
        {
            sum.push_back(*b[i]);
            ids.push_back(b[i]->id_);
        }
    }

    return sum;
}

TrajectoryPool &
operator+=(TrajectoryPool &a, // NOLINT
           const TrajectoryPtrPool &b)
{
    std::vector<int> ids(a.size());
    for (size_t i = 0; i < a.size(); ++i)
        ids[i] = a[i].id_;

    for (size_t i = 0; i < b.size(); ++i)
    {
        // if (b[i]->smooth_embedding_.empty())
        //     continue;
        auto iter = find(ids.begin(), ids.end(), b[i]->id_);
        if (iter == ids.end())
        {
            a.push_back(*b[i]);
            ids.push_back(b[i]->id_);
        }
    }

    return a;
}

TrajectoryPool &
operator+=(TrajectoryPool &a, const TrajectoryPool &b)
{
    std::vector<int> ids(a.size());
    for (size_t i = 0; i < a.size(); ++i)
        ids[i] = a[i].id_;

    for (size_t i = 0; i < b.size(); ++i)
    {
        // if (b[i].smooth_embedding_.empty())
        //     continue;
        auto iter = find(ids.begin(), ids.end(), b[i].id_);
        if (iter == ids.end())
        {
            a.push_back(b[i]);
            ids.push_back(b[i].id_);
        }
    }

    return a;
}

TrajectoryPool
operator-(const TrajectoryPool &a, const TrajectoryPool &b)
{
    TrajectoryPool dif;
    std::vector<int> ids(b.size());
    for (size_t i = 0; i < b.size(); ++i)
        ids[i] = b[i].id_;

    for (size_t i = 0; i < a.size(); ++i)
    {
        auto iter = find(ids.begin(), ids.end(), a[i].id_);
        if (iter == ids.end())
            dif.push_back(a[i]);
    }

    return dif;
}

TrajectoryPool &
operator-=(TrajectoryPool &a, // NOLINT
           const TrajectoryPool &b)
{
    std::vector<int> ids(b.size());
    for (size_t i = 0; i < b.size(); ++i)
        ids[i] = b[i].id_;

    TrajectoryPoolIterator piter;
    for (piter = a.begin(); piter != a.end();)
    {
        auto iter = find(ids.begin(), ids.end(), piter->id_);
        if (iter == ids.end())
            ++piter;
        else
            piter = a.erase(piter);
    }

    return a;
}

TrajectoryPool &
operator-=(TrajectoryPool &a, // NOLINT
           const TrajectoryPtrPool &b)
{
    std::vector<int> ids(b.size());
    for (size_t i = 0; i < b.size(); ++i)
        ids[i] = b[i]->id_;

    TrajectoryPoolIterator piter;
    for (piter = a.begin(); piter != a.end();)
    {
        auto iter = find(ids.begin(), ids.end(), piter->id_);
        if (iter == ids.end())
            ++piter;
        else
            piter = a.erase(piter);
    }

    return a;
}

TrajectoryPtrPool
operator+(const TrajectoryPtrPool &a, const TrajectoryPtrPool &b)
{
    TrajectoryPtrPool sum;
    sum.insert(sum.end(), a.begin(), a.end());

    std::vector<int> ids(a.size());
    for (size_t i = 0; i < a.size(); ++i)
        ids[i] = a[i]->id_;

    for (size_t i = 0; i < b.size(); ++i)
    {
        auto iter = find(ids.begin(), ids.end(), b[i]->id_);
        if (iter == ids.end())
        {
            sum.push_back(b[i]);
            ids.push_back(b[i]->id_);
        }
    }

    return sum;
}

TrajectoryPtrPool
operator+(const TrajectoryPtrPool &a, TrajectoryPool *b)
{
    TrajectoryPtrPool sum;
    sum.insert(sum.end(), a.begin(), a.end());

    std::vector<int> ids(a.size());
    for (size_t i = 0; i < a.size(); ++i)
        ids[i] = a[i]->id_;

    for (size_t i = 0; i < b->size(); ++i)
    {
        auto iter = find(ids.begin(), ids.end(), (*b)[i].id_);
        if (iter == ids.end())
        {
            sum.push_back(&(*b)[i]);
            ids.push_back((*b)[i].id_);
        }
    }

    return sum;
}

TrajectoryPtrPool
operator-(const TrajectoryPtrPool &a, const TrajectoryPtrPool &b)
{
    TrajectoryPtrPool dif;
    std::vector<int> ids(b.size());
    for (size_t i = 0; i < b.size(); ++i)
        ids[i] = b[i]->id_;

    for (size_t i = 0; i < a.size(); ++i)
    {
        auto iter = find(ids.begin(), ids.end(), a[i]->id_);
        if (iter == ids.end())
            dif.push_back(a[i]);
    }

    return dif;
}

void
Trajectory::update_embedding(const cv::Mat &embedding)
{
    if (embedding.empty())
    {
        return;
    }
    current_embedding_ = embedding;

    if (smooth_embedding_.empty())
    {
        smooth_embedding_ = current_embedding_;
        return;
    }
    else if (state_ == TrackState::Tracked)
    {
        smooth_embedding_ = eta_ * smooth_embedding_ + (1 - eta_) * current_embedding_;
    }
    else
    {
        smooth_embedding_ = eta_ * 0.5 * smooth_embedding_ + (1 - eta_ * 0.5) * current_embedding_;
    }
    smooth_embedding_ = smooth_embedding_ / cv::norm(smooth_embedding_);
}

void
Trajectory::update_velocity(const cv::Mat &chassis)
{
    if (velocities_.empty())
    {
        velocities_ = calcVelocities(chassis, chassis_);
    }
    else
    {
        velocities_ = beta_ * velocities_ + (1.0 - beta_) * calcVelocities(chassis, chassis_);
    }
}
} // namespace sonli
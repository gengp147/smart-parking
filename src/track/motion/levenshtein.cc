/*
 * @Author: lwn
 * @Date: 2024-04-24 17:33:37
 * @LastEditors: lwn
 * @LastEditTime: 2024-04-25 15:39:06
 * @FilePath: \FalconEye\src\track\motion\levenshtein.cc
 * @Description:
 */

#include "../track/motion/levenshtein.h"
#include <assert.h>
#include <map>

namespace sonli
{
namespace Levenshtein
{
static inline int
_levenshtein_maximum(const std::string &s1, const std::string &s2, std::vector<int> weights = {1, 1, 1})
{
    int len1 = s1.length();
    int len2 = s2.length();
    assert(weights.size() == 3);
    int insert_, delete_, replace_;
    insert_ = weights[0];
    delete_ = weights[1];
    replace_ = weights[2];

    int max_dist = len1 * delete_ + len2 * insert_;
    if (len1 >= len2)
    {
        max_dist = std::min(max_dist, len2 * replace_ + (len1 - len2) * delete_);
    }
    else
    {
        max_dist = std::min(max_dist, len1 * replace_ + (len2 - len1) * insert_);
    }

    return max_dist;
}

static inline int
_uniform_generic(const std::string &s1, const std::string &s2, std::vector<int> weights = {1, 1, 1})
{
    int len1 = s1.length();
    assert(weights.size() == 3);
    int insert_, delete_, replace_;
    insert_ = weights[0];
    delete_ = weights[1];
    replace_ = weights[2];

    std::vector<int> cache_(len1 + 1);
    for (int i = 0; i < len1 + 1; i++)
    {
        cache_.at(i) = i * delete_;
    }
    int temp, x;

    for (const char ch2 : s2)
    {
        temp = cache_[0];
        cache_[0] = cache_[0] + insert_;
        for (int i = 0; i < len1; i++)
        {
            x = temp;
            if (s1.at(i) != ch2)
            {
                x = std::min(std::min(cache_[i] + delete_, cache_[i + 1] + insert_), temp + replace_);
            }
            temp = cache_[i + 1];
            cache_[i + 1] = x;
        }
    }

    return *cache_.rbegin();
}

static int
_uniform_distance(const std::string &s1, const std::string &s2)
{
    if (s1.length() == 0)
    {
        return s2.length();
    }
    if (s2.length() == 0)
    {
        return s1.length();
    }

    int VP = (1 << s1.length()) - 1;
    int VN = 0;
    int currDist = s1.length();
    int mask = 1 << (s1.length() - 1);

    std::map<const char, int> block;
    int x = 1;
    for (const char ch1 : s1)
    {
        int temp = block.find(ch1) != block.end() ? block[ch1] : 0;
        block[ch1] = temp | x;
        x << 1;
    }

    for (const char ch2 : s2)
    {
        // Step 1: Computing D0
        int PM_j = block.find(ch2) != block.end() ? block[ch2] : 0;
        int X = PM_j;
        int D0 = (((X & VP) + VP) ^ VP) | X | VN;
        // Step 2: Computing HP and HN
        int HP = VN | ~(D0 | VP);
        int HN = D0 & VP;
        // Step 3: Computing the value D[m,j]
        currDist += (HP & mask) != 0;
        currDist -= (HN & mask) != 0;
        // Step 4: Computing Vp and VN
        HP = (HP << 1) | 1;
        HN = HN << 1;
        VP = HN | ~(D0 | HP);
        VN = HP & D0;
    }
    return currDist;
}

int
distance(const std::string &s1, const std::string &s2, const std::vector<int> weights, int score_cutoff)
{

    int dist;
    if (weights.empty() || weights == std::vector<int>(3, 1))
    {
        dist = _uniform_distance(s1, s2);
    }
    else
    {
        dist = _uniform_generic(s1, s2, weights);
    }

    return score_cutoff == 0 || dist <= score_cutoff ? dist : score_cutoff + 1;
}

int
similarity(const std::string &s1, const std::string &s2, const std::vector<int> weights, int score_cutoff)
{
    std::vector<int> weights_ = weights.empty() ? std::vector<int>(3, 1) : weights;
    int maximum = _levenshtein_maximum(s1, s2, weights_);
    int dist = distance(s1, s2, weights_);
    int sim = maximum - dist;
    return score_cutoff == 0 || sim >= score_cutoff ? sim : 0;
}

float
normalized_distance(const std::string &s1, const std::string &s2, const std::vector<int> weights, int score_cutoff)
{
    std::vector<int> weights_ = weights.empty() ? std::vector<int>(3, 1) : weights;
    int maximum = _levenshtein_maximum(s1, s2, weights_);
    int dist = distance(s1, s2, weights_);
    int norm_dist = maximum ? (float)dist / (float)maximum : 0.f;
    return score_cutoff == 0 || norm_dist <= score_cutoff ? norm_dist : 1.f;
}

float
normalized_similarity(const std::string &s1, const std::string &s2, const std::vector<int> weights, int score_cutoff)
{
    float norm_dist = normalized_distance(s1, s2, weights);
    float norm_sim = 1.0f - norm_dist;
    return score_cutoff == 0 || norm_sim >= score_cutoff ? norm_sim : 0.f;
}

} // namespace Levenshtein
} // namespace sonli
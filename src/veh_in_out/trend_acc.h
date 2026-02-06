#include <algorithm>
#include <numeric>
#include <string>
#include <vector>

namespace sonli
{
namespace basic_algorithm
{
enum class Trend
{
    INCREASING,
    DECREASING,
    CONSTANT,
    MIXED
};

inline std::string
TrendString(Trend trend)
{
    switch (trend)
    {
        case Trend::INCREASING:
            return "INCREASING";
        case Trend::DECREASING:
            return "DECREASING";
        case Trend::CONSTANT:
            return "CONSTANT";
        case Trend::MIXED:
            return "MIXED";
    }
    return "";
}

inline Trend
detectTrend(std::vector<float>::const_iterator begin, std::vector<float>::const_iterator end)
{
    if (end - begin < 2)
    {
        return Trend::CONSTANT;
    }

    std::vector<float> gradients;
    for (auto i = begin + 1; i != end; ++i)
    {
        gradients.push_back(*i - *(i - 1));
    }

    float sum = std::accumulate(gradients.begin(), gradients.end(), 0.0f);
    if (sum > 0)
    {
        return Trend::INCREASING;
    }
    else if (sum < 0)
    {
        return Trend::DECREASING;
    }
    else
    {
        return Trend::CONSTANT;
    }
}

inline Trend
detectTrend(const std::vector<float> &data)
{
    return detectTrend(data.begin(), data.end());
}

inline std::vector<float>
MovingAverage(const std::vector<float> &data, int windowSize)
{
    std::vector<float> smoothedData(data.size(), 0.0);
    for (size_t i = 0; i < data.size(); ++i)
    {
        int start = std::max(0, static_cast<int>(i) - windowSize / 2);
        int end = std::min(static_cast<int>(data.size()), static_cast<int>(i) + windowSize / 2 + 1);
        float sum = std::accumulate(data.begin() + start, data.begin() + end, 0.0);
        smoothedData[i] = sum / (end - start);
    }
    return smoothedData;
}

inline std::pair<float, float>
LinearRegression(const std::vector<float> &data, int start, int end)
{
    int n = end - start;
    float sumX = 0, sumY = 0, sumXY = 0, sumXX = 0;

    for (int i = start; i < end; ++i)
    {
        int x = i - start;
        sumX += x;
        sumY += data[i];
        sumXY += x * data[i];
        sumXX += x * x;
    }

    float slope = (n * sumXY - sumX * sumY) / (n * sumXX - sumX * sumX);
    float intercept = (sumY - slope * sumX) / n;

    return {slope, intercept};
}

inline int
FindLocalMaxima(const std::vector<float> &data, int start = 0)
{
    int maxI = 0;
    float maxV = 0.0;
    for (int i = start; i < data.size(); ++i)
    {
        if (maxV < data.at(i))
        {
            maxI = i;
            maxV = data.at(i);
        }
    }
    return maxI;
}

inline int
FindLocalMinima(const std::vector<float> &data, int start = 0)
{
    int minI = 0;
    float minV = 10000.0;
    for (int i = start; i < data.size(); ++i)
    {
        if (minV >= data.at(i))
        {
            minI = i;
            minV = data.at(i);
        }
    }
    return minI;
}

inline std::vector<float>
Diff(const std::vector<float> &vec)
{
    std::vector<float> result;
    for (size_t i = 1; i < vec.size(); ++i)
    {
        result.push_back(std::abs(vec[i] - vec[i - 1]));
    }
    return result;
}

inline int
FindKeyPoint(const std::vector<float> &data, int window_size = 5)
{
    std::vector<float> slopes;
    for (size_t i = 0; i <= data.size() - window_size; ++i)
    {
        auto slope_intercept = LinearRegression(data, i, i + window_size);
        slopes.emplace_back(slope_intercept.first);
    }

    auto diffv = Diff(slopes);
    return std::max_element(diffv.begin(), diffv.end()) - diffv.begin() + window_size / 2;
}

// 1 - ssr/sst
inline float
CalculateRSquared(const std::vector<float> &data, float slope, float intercept, int start, int end)
{
    int n = end - start;
    float sumY = std::accumulate(data.begin() + start, data.begin() + end, 0.0);
    float meanY = sumY / n;

    float ssTot = 0, ssRes = 0;
    for (int i = start; i < end; ++i)
    {
        float y = data[i];
        float yPred = slope * (i - start) + intercept;
        ssTot += (y - meanY) * (y - meanY);
        ssRes += (y - yPred) * (y - yPred);
    }

    float rSquared = 1 - (ssRes / ssTot);
    return rSquared;
}

/**
 * 根据给定数组判断趋势
 * @param slope  	 直线斜率
 * @param rSquared   直线R2，判断直线拟合程度
 * @param threshold1 上升直线拟合程度阈值
 * @param threshold2 下降直线拟合程度阈值
 * @param sk1        判断上升直线斜率阈值  Y/X=sk1
 * @param sk2        判断下降直线斜率阈值
 * @return           趋势以及置信度
 */
inline std::pair<Trend, float>
DetermineTrend(float slope, float rSquared, float threshold1 = 0.0, float threshold2 = -1.0, float sk1 = 0.0,
               float sk2 = -0.0)
{
    if (threshold2 == -1.0)
    {
        threshold2 = threshold1;
    }
    Trend trend;
    if (slope > sk1 && rSquared > threshold1)
    {
        trend = Trend::INCREASING;
    }
    else if (slope < sk2 && rSquared > threshold2)
    {
        trend = Trend::DECREASING;
    }
    else
    {
        trend = Trend::CONSTANT;
    }
    return {trend, rSquared};
}

template <typename ElementT>
inline ElementT
CalculateMean(const std::vector<ElementT> &data)
{
    ElementT startV = 0.0;
    ElementT sum = std::accumulate(data.begin(), data.end(), startV);
    return sum / data.size();
}

template <typename ElementT>
inline std::pair<ElementT, ElementT>
CalculateStandardDeviation(const std::vector<ElementT> &data)
{
    ElementT mean = CalculateMean(data);
    ElementT sumSquaredDiffs = 0.0;
    for (const auto &value : data)
    {
        sumSquaredDiffs += (value - mean) * (value - mean);
    }
    return {mean, std::sqrt(sumSquaredDiffs / data.size())};
}

} // namespace basic_algorithm
} // namespace sonli
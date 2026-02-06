
#ifndef ACL_DET_YOLO_FUNCTIONTIMER_H
#define ACL_DET_YOLO_FUNCTIONTIMER_H
#include <chrono>
#include <fstream>
#include <ostream>
#include <string>
#include <sys/time.h>
namespace sonli
{
class FunctionTimer
{
public:
    explicit FunctionTimer(const std::string &funcName) : functionName(funcName)
    {
        start = std::chrono::high_resolution_clock::now();
    }

    ~FunctionTimer()
    {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "function [" << functionName << "] run expensive time : "
                  << duration.count() << " us ("
                  << duration.count() / 1000.0 << " ms)" << std::endl;
    }

private:
    std::string functionName;
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
};

inline void
print_timestamp_ms(const std::string &prefix)
{
    struct timeval tv;
    // 获取当前时间（秒+微秒）
    if (gettimeofday(&tv, nullptr) == -1)
    {
        perror("gettimeofday failed");
        return;
    }

    // 计算总毫秒数：秒*1000 + 微秒/1000
    long long total_ms = (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
    // 单独提取毫秒部分（0-999）
    int ms = tv.tv_usec / 1000;

    // 格式化人类可读时间（线程安全：用 localtime_r 替代 localtime）
    struct tm tm;
    localtime_r(&tv.tv_sec, &tm); // localtime_r 线程安全，localtime 非线程安全
    std::cout << "[TimeStampRecord]<" << prefix << ">:" << total_ms << " ms" << std::endl;
}

inline long long
get_timestamp_ms()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    // 计算总毫秒数：秒*1000 + 微秒/1000
    long long total_ms = (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return total_ms;
}

class NumericColumnWriter
{
public:
    explicit NumericColumnWriter(const std::string &filePath = "/nfsroot/lib/TimeStampRecord.record")
        : file_path_(filePath), file_stream_()
    {
        // 以「追加+输出」模式打开文件（ios::app 确保后续写入不覆盖）
        file_stream_.open(file_path_, std::ios::out);
        if (!file_stream_.is_open())
        {
            throw std::runtime_error("[NumericColumnWriter]open file error：" + filePath);
        }
        std::cout << "[NumericColumnWriter]：writer ok " << filePath << std::endl;
    }

    // 析构函数：关闭文件流
    ~NumericColumnWriter()
    {
        if (file_stream_.is_open())
        {
            file_stream_.close();
        }
    }

    // 写入数值（模板函数，支持 int/long/float/double 等所有数值类型）
    template <typename T>
    bool
    write(const T &value)
    {
        if (!file_stream_.is_open())
        {
            std::cerr << "[NumericColumnWriter] file_stream_.is_open() error" << std::endl;
            return false;
        }

        // 写入数值（每行1个，单列格式），并刷新缓冲区确保数据实时写入
        file_stream_ << value << std::endl;
        file_stream_.flush();

        // 检查写入是否成功
        if (file_stream_.fail())
        {
            std::cerr << "[NumericColumnWriter]:" << value << " write error." << std::endl;
            return false;
        }
        return true;
    }

    // 获取当前文件路径（可选接口）
    std::string
    getFilePath() const
    {
        return file_path_;
    }

private:
    std::string file_path_;     // 文件路径
    std::ofstream file_stream_; // 文件输出流
};


}

#endif // ACL_DET_YOLO_FUNCTIONTIMER_H


#ifndef ACL_DET_YOLO_DETECTIONS_SAVE_H
#define ACL_DET_YOLO_DETECTIONS_SAVE_H
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
namespace sonli
{

struct DetectionsInformationItem
{
    int id = 0; //track id
    std::vector<int> objRect; // 四个值，车辆坐标，cx，cy，w，h
    std::vector<int> objRect_expend; // 四个值，车辆坐标，根据关键点求取的坐标，  cx，cy，w，h
    std::vector<int> objRect_pred; // 四个值，车辆坐标，卡尔曼预测的坐标，  cx，cy，w，h
    std::vector<int> key_points;//32个值，16个车辆关键坐标，其中前四个为车辆的底盘坐标，格式为x1，y1，x2，y2，...
    std::vector<float> key_points_confidence; //每个关键点的可见度，16个值
    std::vector<float> confidences;//共四个值，车辆检测置信度，车牌检测置信度，车牌字符检测平均置信度，车辆的可见度
    std::string lpstr;
};



struct DetectionsInformationS
{
    int frameNumber = 0;
    std::vector<DetectionsInformationItem> detections;
};

class CarDetectionCsvSaver
{
public:
    // 构造函数：初始化CSV文件（仅打开一次），单线程下创建对象时调用即可
    // 参数1：CSV文件保存路径（必填，如"./car_detections.csv"）
    // 参数2：浮点数输出精度（可选，默认保留4位小数）
    explicit CarDetectionCsvSaver(const std::string& csvPath, int floatPrecision = 4)
        : m_csvPath(csvPath), m_floatPrecision(floatPrecision), m_isHeaderWritten(false)
    {
        // 打开文件：不存在则创建，存在则追加，二进制模式避免跨平台换行符问题
        m_csvFout.open(csvPath, std::ios::out | std::ios::app | std::ios::binary);
        if (!m_csvFout.is_open())
        {
            std::cerr << "[CSV保存器错误] 无法打开文件：" << csvPath << "（检查路径/权限）" << std::endl;
            return;
        }


        // 一次性设置输出格式，永久有效（固定浮点数+指定精度）
        m_csvFout << std::fixed << std::setprecision(m_floatPrecision);
        std::cout << "[CSV保存器成功] 初始化完成，文件路径：" << csvPath
                  << "，浮点数精度：" << m_floatPrecision << "位" << std::endl;
    }

    // 析构函数：自动刷新缓冲区+关闭文件（RAII机制），对象销毁时自动调用，无需手动操作
    ~CarDetectionCsvSaver()
    {
        if (m_csvFout.is_open())
        {
            m_csvFout.flush(); // 最后一次刷新，保证所有缓存数据写入磁盘
            m_csvFout.close();
            std::cout << "[CSV保存器信息] 文件已自动关闭，路径：" << m_csvPath << std::endl;
        }
    }

    // 核心公有接口：逐帧保存检测结果（单线程下每帧调用一次即可）
    // 参数：本帧的检测结果结构体
    // 返回：bool - 写入成功返回true，失败返回false
    bool saveFrame(const DetectionsInformationS& frameData)
    {
        // 前置检查：文件流是否有效
        std::cout << "writeFrame: " << frameData.frameNumber << std::endl;
        if (!m_csvFout.is_open() || !m_csvFout.good())
        {
            std::cerr << "[CSV保存器错误] 文件流失效，无法写入帧" << frameData.frameNumber << std::endl;
            return false;
        }

        // 空帧跳过：本帧无检测目标，不写入
        if (frameData.detections.empty())
        {
            std::cout << "[CSV保存器信息] 帧" << frameData.frameNumber << "无检测目标，跳过写入" << std::endl;
            return true;
        }

        // 首次写入：自动生成并写入CSV表头（仅执行一次）
        if (!m_isHeaderWritten)
        {
            writeCsvHeader();
            m_isHeaderWritten = true;
        }

        // 遍历本帧所有目标，逐个写入CSV（一个目标一行）
        int validCount = 0;
        for (const auto& item : frameData.detections)
        {
            // 软校验：跳过vector长度非法的目标，仅打印警告
            if (!isItemValid(item))
            {
                std::cerr << "[CSV保存器警告] 帧" << frameData.frameNumber << "跟踪ID" << item.id << "数据非法，跳过" << std::endl;
                continue;
            }
            // 写入单个目标数据
            writeSingleItem(frameData.frameNumber, item);
            validCount++;
        }

        // 手动刷新缓冲区，保证数据实时写入磁盘
        m_csvFout.flush();
        std::cout << "[CSV保存器成功] 帧" << frameData.frameNumber << "写入完成，有效目标："
                  << validCount << "/" << frameData.detections.size() << std::endl;
        return true;
    }

    // 可选公有接口：手动关闭文件（一般无需调用，析构会自动关闭）
    void closeFile()
    {
        if (m_csvFout.is_open())
        {
            m_csvFout.flush();
            m_csvFout.close();
            std::cout << "[CSV保存器信息] 文件已手动关闭，路径：" << m_csvPath << std::endl;
        }
    }

private:
    // 私有成员变量：仅类内部访问，封装实现细节
    std::ofstream m_csvFout;       // 全局文件流，仅打开一次
    std::string m_csvPath;         // CSV文件路径
    int m_floatPrecision;          // 浮点数输出精度
    bool m_isHeaderWritten;        // 表头是否已写入标志位

    // 私有辅助函数：写入CSV表头（仅内部调用）
    void writeCsvHeader()
    {
        m_csvFout << "frameNumber,id,lpstr,"
             // 3组坐标：objRect/objRect_expend/objRect_pred（各4列）
             << "objRect_cx,objRect_cy,objRect_w,objRect_h,"
             << "objRect_expend_cx,objRect_expend_cy,objRect_expend_w,objRect_expend_h,"
             << "objRect_pred_cx,objRect_pred_cy,objRect_pred_w,objRect_pred_h,"
             // 32个关键点坐标（16个x/y）
             << "key_point_0_x,key_point_0_y,key_point_1_x,key_point_1_y,key_point_2_x,key_point_2_y,key_point_3_x,key_point_3_y,"
             << "key_point_4_x,key_point_4_y,key_point_5_x,key_point_5_y,key_point_6_x,key_point_6_y,key_point_7_x,key_point_7_y,"
             << "key_point_8_x,key_point_8_y,key_point_9_x,key_point_9_y,key_point_10_x,key_point_10_y,key_point_11_x,key_point_11_y,"
             << "key_point_12_x,key_point_12_y,key_point_13_x,key_point_13_y,key_point_14_x,key_point_14_y,key_point_15_x,key_point_15_y,"
             // 16个关键点置信度
             << "kp_conf_0,kp_conf_1,kp_conf_2,kp_conf_3,kp_conf_4,kp_conf_5,kp_conf_6,kp_conf_7,"
             << "kp_conf_8,kp_conf_9,kp_conf_10,kp_conf_11,kp_conf_12,kp_conf_13,kp_conf_14,kp_conf_15,"
             // 4个综合置信度
             << "car_conf,lp_conf,lp_char_avg_conf,car_visibility\n";
        std::cout << "[CSV保存器信息] CSV表头已成功写入" << std::endl;
    }

    // 私有辅助函数：校验单个目标数据是否合法（vector长度是否符合定义）
    bool isItemValid(const DetectionsInformationItem& item) const
    {
        return (item.objRect.size() == 4 && item.objRect_expend.size() == 4 &&
                item.objRect_pred.size() == 4 && item.key_points.size() == 32 &&
                item.key_points_confidence.size() == 16 && item.confidences.size() == 4);
    }

    // 私有辅助函数：CSV字符串转义（处理车牌含逗号/双引号/换行，避免列错位）
    std::string escapeString(const std::string& s) const
    {
        std::string res = s;
        if (res.find_first_of(",\"\n\r") != std::string::npos)
        {
            std::replace(res.begin(), res.end(), '"', '\"'); // 双引号转义为两个
            res = "\"" + res + "\""; // 双引号包裹整个字符串
        }
        return res;
    }

    // 私有辅助函数：写入单个检测目标的完整数据（一行）
    void writeSingleItem(int frameNum, const DetectionsInformationItem& item)
    {
        // 1. 基础字段：帧号、跟踪ID、转义后的车牌
        m_csvFout << frameNum << "," << item.id << "," << escapeString(item.lpstr) << ",";
        // 2. 3组车辆坐标（依次输出）
        for (int val : item.objRect) m_csvFout << val << ",";
        for (int val : item.objRect_expend) m_csvFout << val << ",";
        for (int val : item.objRect_pred) m_csvFout << val << ",";
        // 3. 32个关键点坐标
        for (int val : item.key_points) m_csvFout << val << ",";
        // 4. 16个关键点置信度
        for (float val : item.key_points_confidence) m_csvFout << val << ",";
        // 5. 4个综合置信度：最后一个无末尾逗号，避免列错位
        for (size_t i = 0; i < item.confidences.size(); ++i)
        {
            if (i == item.confidences.size() - 1)
                m_csvFout << item.confidences[i];
            else
                m_csvFout << item.confidences[i] << ",";
        }
        // 换行：一个目标占一行
        m_csvFout << "\n";
    }
};



}

#endif // ACL_DET_YOLO_DETECTIONS_SAVE_H

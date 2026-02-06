/***
 * @Author: lwn
 * @Date: 2024-01-31 10:48:57
 * @LastEditors: lwn
 * @LastEditTime: 2024-07-17 16:06:11
 * @FilePath: \FalconEye\include\fileio\utils.h
 * @Description:
 */
#ifndef _FILEIO_UTILS_H_
#define _FILEIO_UTILS_H_

#include <cstdio>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

namespace sonli
{
bool inline existLocalFile(const std::string &path)
{
    bool bRet = false;
    int iRet;
    iRet = access(path.c_str(), F_OK);
    if (iRet) // Not find src file
    {
    }
    else
    {
        bRet = true;
    }
    return bRet;
}

bool inline isExistFile(const std::string &file_name)
{
    std::ifstream infile;
    infile.open(file_name.c_str());
    auto ret = infile.is_open();
    if (ret)
    {
        infile.close();
    }
    return ret;
}

bool inline createDirectory(const std::string &folder)
{
    std::string folder_builder;
    std::string sub;
    sub.reserve(folder.size());
    for (auto it = folder.begin(); it != folder.end(); ++it)
    {
        const char c = *it;
        sub.push_back(c);
        if (c == '/' || it == folder.end() - 1)
        {
            folder_builder.append(sub);
            if (0 != access(folder_builder.c_str(), 0))
            {
                // this folder not exist
                if (0 != mkdir(folder_builder.c_str(), 0755))
                {
                    // create failed
                    return false;
                }
            }
            sub.clear();
        }
    }
    return true;
}

inline void
deleteFileLinux(const std::string &file_path)
{
    if (file_path.empty())
    {
        throw std::invalid_argument("File path cannot be empty!:" + file_path);
    }

    // 先判断路径是否为普通文件（避免误删目录）
    struct stat file_stat;
    if (stat(file_path.c_str(), &file_stat) == -1)
    {
        throw std::runtime_error("Failed to stat file [" + file_path + "]: " + strerror(errno));
    }

    // 检查是否为普通文件（S_ISREG），不是则抛出异常
    if (!S_ISREG(file_stat.st_mode))
    {
        throw std::runtime_error("[" + file_path + "] is not a regular file!");
    }

    // 调用 unlink 系统调用删除文件
    if (unlink(file_path.c_str()) == -1)
    {
        throw std::runtime_error("Failed to delete file [" + file_path + "]: " + strerror(errno));
    }
}

} // namespace sonli

#endif
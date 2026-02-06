/**
 * @file utils.h
 *
 * Copyright (C) 2021. Shenshu Technologies Co., Ltd. All rights reserved.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifndef UTILS_H
#define UTILS_H

#include "acl/svp_acl_mdl.h"
#include "error.h"
#include <string>
#include <vector>

#ifdef _WIN32
#define S_ISREG(m) (((m) & 0170000) == (0100000))
#endif

#define CHECK_EXPS_RETURN(exps, ret, msg, ...)                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((exps))                                                                                                    \
        {                                                                                                              \
            printf("CHECK_EXPS_RETURN failed, exps: %d, ret: %d, msg: %s\n", exps, ret, msg); \
            return (ret);                                                                                              \
        }                                                                                                              \
    } while (0)

class Utils
{
public:
    /**
     * @brief create device buffer of file
     * @param [in] fileName: file name
     * @param [out] fileSize: size of file
     * @return device buffer of file
     */
    static void *
    GetDeviceBufferOfFile(const std::string &fileName, const svp_acl_mdl_io_dims &dims, size_t stride, size_t dataSize);

    /**
     * @brief create buffer of file
     * @param [in] fileName: file name
     * @param [out] fileSize: size of file
     * @return buffer of pic
     */
    static void *
    ReadBinFile(const std::string &fileName, uint32_t &fileSize);

    static int
    ReadFloatFile(const std::string &fileName, std::vector<float> &detParas);

    static int
    GetFileSize(const std::string &fileName, uint32_t &fileSize);

    static void *
    ReadBinFileWithStride(const std::string &fileName, const svp_acl_mdl_io_dims &dims, size_t stride, size_t dataSize);

    static void
    InitData(int8_t *data, size_t dataSize);
};

#endif

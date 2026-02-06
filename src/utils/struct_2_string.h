//
// Created by gp147 on 2025/9/1.
//

#ifndef STRUCT_2_STRING_H
#define STRUCT_2_STRING_H

#include "../illegal_parking/illegal_parking_algorithm.h"
#include "data_type.h"
#include "struct_2_stream.h"

#include <string>

std::string
TO_STRING(const aih::ControlInfo &Obj);

std::string
TO_STRING(const aih::SingleEventCfg &Obj);

std::string
TO_STRING(const sonli::VehInfoVec &Obj);

std::string
TO_STRING(const sonli::ParkingInfo &Obj);

std::string
TO_STRING(const sonli::AccessConfig_t &Obj);

std::string
TO_STRING(const sonli::illegal_parking::NoParkingAreaConfig &Obj);

std::string
TO_STRING(const aih::EventInfo &ei);

template <typename T>
std::string
TO_STRING(const T &Obj)
{
    std::stringstream ss;
    ss << Obj;
    return ss.str();
}

#endif //STRUCT_2_STRING_H

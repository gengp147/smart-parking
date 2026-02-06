//
// Created by gp147 on 2025/9/1.
//

#include "struct_2_string.h"
#include "../illegal_parking/illegal_parking_algorithm.h"
#include "../interfaces/aih_extern.h"
std::string
TO_STRING(const aih::ControlInfo &Obj)
{
    std::stringstream ss;
    ss << Obj;
    return ss.str();
}

std::string
TO_STRING(const aih::SingleEventCfg &Obj)
{
    std::stringstream ss;
    ss << Obj;
    return ss.str();
}

std::string
TO_STRING(const sonli::VehInfoVec &Obj)
{
    std::stringstream ss;
    ss << Obj;
    return ss.str();
}

std::string
TO_STRING(const sonli::ParkingInfo &Obj)
{
    std::stringstream ss;
    ss << Obj;
    return ss.str();
}

std::string
TO_STRING(const sonli::AccessConfig_t &Obj)
{
    std::stringstream ss;
    ss << Obj;
    return ss.str();
}

std::string
TO_STRING(const sonli::illegal_parking::NoParkingAreaConfig &Obj)
{
    std::stringstream ss;
    ss << Obj;
    return ss.str();
}

std::string
TO_STRING(const aih::EventInfo &ei)
{
    std::stringstream ss;
    sonli::operator<<(ss, ei);
    return ss.str();
}


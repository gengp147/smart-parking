#ifndef _CHECK_PARAM_RANGE_H_
#define _CHECK_PARAM_RANGE_H_

#include <string>
#include "error.h"
#include "sl_logger.h"
#include "json/json.h"

namespace sonli
{
    static inline int check_json_value_int(Json::Value cfg_jv, const char *check_item, int min, int max)
    {
        if (cfg_jv.isMember(check_item))
		{
			auto param = cfg_jv[check_item];
			if (param.isNull() || !param.isInt()){
				SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error(
        			"Update config: {} failed! Value is null or not int tpye value !", check_item);
                return ERR_CONFIG_ILLEGAL;
			}
			if (param.asInt() < min || param.asInt() > max){
				SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error(
        			"Update config: {} failed! The value must in range [{}, {}], but got {} !", check_item, min, max, param.asInt());
                return ERR_CONFIG_INVALID_PARAM;
			}
            return SL_SUCCESS;
		}else{
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error(
        			"The given json value doesn't has the member: {} !", check_item);
            return ERR_CONFIG;
        }
    }

    static inline int check_json_value_bool(Json::Value cfg_jv, const char *check_item)
    {
        if (cfg_jv.isMember(check_item))
		{
			auto param = cfg_jv[check_item];
			if (param.isNull() || !param.isBool()){
				SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error(
        			"Update config: {} failed! Value is null or not bool tpye value !", check_item);
                return ERR_CONFIG_ILLEGAL;
			}
            return SL_SUCCESS;
		}else{
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error(
        			"The given json value doesn't has the member: {} !", check_item);
            return ERR_CONFIG;
        }
    }

    static inline int check_json_value_double(Json::Value cfg_jv, const char *check_item, double min, double max)
    {
        if (cfg_jv.isMember(check_item))
		{
			auto param = cfg_jv[check_item];
			if (param.isNull() || !param.isDouble()){
				SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error(
        			"Update config: {} failed! Value is null or not double tpye value !", check_item);
                return ERR_CONFIG_ILLEGAL;
			}
			if (param.isDouble() < min || param.isDouble() > max){
				SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error(
        			"Update config: {} failed! The value must in range [{}, {}], but got {} !", check_item, min, max, param.isDouble());
                return ERR_CONFIG_INVALID_PARAM;
			}
            return SL_SUCCESS;
		}else{
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error(
        			"The given json value doesn't has the member: {} !", check_item);
            return ERR_CONFIG;
        }
    }

    static inline bool check_int_value(int param, const char *check_item, int min, int max)
    {
        if (param >= min && param <= max) {
            return true;
        }
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error(
                "Update config: {} failed! The int value must in range [{}, {}], but got {} !", min, max, param);
        return false;
    }
    static inline bool check_double_value(double param, const char *check_item, double min, double max)
    {
        if (param >= min && param <= max) {
            return true;
        }
        SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error(
                "Update config: {} failed! The double value must in range [{}, {}], but got {} !", min, max, param);
        return false;
    }

} // namespace
#endif

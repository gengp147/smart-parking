/*
 * @Author: lwn
 * @Date: 2024-01-31 09:06:18
 * @LastEditors: lwn
 * @LastEditTime: 2024-07-03 14:42:58
 * @FilePath: \FalconEye\src\logging\logger.cc
 * @Description: 
 */
#include "sl_logger.h"
#include <fstream>
#include "json/json.h"
#include "../include/error.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "file_utils.h"
#include <chrono>
// #include "manager/log_manager.h"
namespace sonli {

    SonLiLogger::SonLiLogger() {
        type_name_map.clear();
        name_logger_map.clear();
    }

    SonLiLogger::~SonLiLogger() {
        type_name_map.clear();
        name_logger_map.clear();
    }

    int SonLiLogger::init(std::string& config) {

        if (inited)
        {
            SPDLOG_LOGGER_ERROR(spdlog::default_logger(),
                                "SonLiLogger is init twice!");
            return ERR_INIT_TWICE;
        }
        spdlog::flush_every(std::chrono::seconds(5));

        Json::Value logger_cfg;
        // get config from json str or json file
        if (isExistFile(config))
        {
            std::ifstream infile(config);
            if (!infile.good())
            {
                SPDLOG_LOGGER_ERROR(spdlog::default_logger(),
                                    "Can not log config file: {} ", config.c_str());
                return ERR_FILE_NOT_EXIST;
            }
            infile >> logger_cfg;
            infile.close();

        }
        else
        {
            Json::CharReaderBuilder builder;
            JSONCPP_STRING error;
            std::shared_ptr<Json::CharReader> json_reader(
                builder.newCharReader());

            try
            {
                bool ret = json_reader->parse(config.c_str(),
                                              config.c_str() + config.size(),
                                              &logger_cfg, &error);
                if (!ret)
                {
                    SPDLOG_LOGGER_ERROR(spdlog::default_logger(),
                                        "Parse log config failed! ");
                    return ERR_CONFIG;
                }
            }
            catch (const std::exception &e)
            {
                SPDLOG_LOGGER_ERROR(spdlog::default_logger(),
                                    "Can not parse log config, err: {} ", e.what());
                return ERR_CONFIG;
            }
        }

        // create a console sink

        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::debug);

        // create file sinks
        std::string log_path = ".";

        if (logger_cfg.isMember("logging_path") && logger_cfg["logging_path"].isString())
        {
            log_path = logger_cfg["logging_path"].asString();
        }

        if (logger_cfg.isMember("file_log"))
        {
            auto file_log_cfg = logger_cfg["file_log"];
            if (!file_log_cfg.isObject())
            {
                SPDLOG_LOGGER_ERROR(spdlog::default_logger(),
                                    "Setting basic file log err! ");
                return ERR_CONFIG;
            }

            if (file_log_cfg.isMember("enable") && file_log_cfg["enable"].isBool())
            {

                if (file_log_cfg["enable"].asBool())
                {
                    std::string filename;
                    int level = 3;
                    bool with_console = false;
                    if (file_log_cfg.isMember("filename") && file_log_cfg["filename"].isString())
                    {
                        filename = log_path + "/" + file_log_cfg["filename"].asString();
                    }
                    else
                    {
                        SPDLOG_LOGGER_ERROR(spdlog::default_logger(),
                                            "Getting filename of basic file log failed! ");
                        return ERR_CONFIG;
                    }
                    if (file_log_cfg.isMember("level") && file_log_cfg["level"].isInt())
                    {
                        level = file_log_cfg["level"].asInt();
                    }

                    if (file_log_cfg.isMember("stdout") && file_log_cfg["stdout"].isBool())
                    {
                        with_console = file_log_cfg["stdout"].asBool();
                    }

                    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename);
                    file_sink->set_level(spdlog::level::level_enum(level));
                    std::vector<spdlog::sink_ptr> sinks;
                    if (with_console)
                    {
                        sinks.push_back(console_sink);
                    }
                    sinks.push_back(file_sink);
                    auto logger = std::make_shared<spdlog::logger>("basic", begin(sinks), end(sinks));
                    logger->flush_on(spdlog::level::err);
                    type_name_map[BasicLogger] = "basic";
                    name_logger_map["basic"] = logger;

                    if (file_log_cfg.isMember("default") && file_log_cfg["default"].isBool() &&
                        file_log_cfg["default"].asBool() == true)
                    {
                        spdlog::set_default_logger(logger);
                    }
                }

            }
            else
            {
                SPDLOG_LOGGER_ERROR(spdlog::default_logger(),
                                    "Setting basic file log err! ");
                return ERR_CONFIG;
            }
        }

        if (logger_cfg.isMember("rotate_log"))
        {
            auto file_log_cfg = logger_cfg["rotate_log"];
            if (!file_log_cfg.isObject())
            {
                SPDLOG_LOGGER_ERROR(spdlog::default_logger(),
                                    "Setting rotate file log err! ");
                return ERR_CONFIG;
            }

            if (file_log_cfg.isMember("enable") && file_log_cfg["enable"].isBool())
            {

                if (file_log_cfg["enable"].asBool())
                {
                    std::string filename;
                    int level = 3;
                    bool with_console = false;
                    int file_size = 1024;
                    int file_num = 3;
                    if (file_log_cfg.isMember("filename") && file_log_cfg["filename"].isString())
                    {
                        filename = log_path + "/" + file_log_cfg["filename"].asString();
                    }
                    else
                    {
                        SPDLOG_LOGGER_ERROR(spdlog::default_logger(),
                                            "Getting filename of rotate file log failed! ");
                        return ERR_CONFIG;
                    }
                    if (file_log_cfg.isMember("level") && file_log_cfg["level"].isInt())
                    {
                        level = file_log_cfg["level"].asInt();
                    }

                    if (file_log_cfg.isMember("stdout") && file_log_cfg["stdout"].isBool())
                    {
                        with_console = file_log_cfg["stdout"].asBool();
                    }

                    if (file_log_cfg.isMember("file_size") && file_log_cfg["file_size"].isInt())
                    {
                        file_size = file_log_cfg["file_size"].asInt();
                    }

                    if (file_log_cfg.isMember("file_num") && file_log_cfg["file_num"].isInt())
                    {
                        file_num = file_log_cfg["file_num"].asInt();
                    }

                    auto rotate_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                        filename, file_size, file_num);
                    rotate_sink->set_level(spdlog::level::level_enum(level));

                    std::vector<spdlog::sink_ptr> sinks;
                    if (with_console)
                    {
                        sinks.push_back(console_sink);
                    }
                    sinks.push_back(rotate_sink);
                    auto logger = std::make_shared<spdlog::logger>("rotate", begin(sinks), end(sinks));
                    logger->flush_on(spdlog::level::err);
                    type_name_map[RotateLogger] = "rotate";
                    name_logger_map["rotate"] = logger;

                    if (file_log_cfg.isMember("default") && file_log_cfg["default"].isBool() &&
                        file_log_cfg["default"].asBool() == true)
                    {
                        spdlog::set_default_logger(logger);
                    }

                    //					if (file_log_cfg.isMember("data_ctl") && file_log_cfg["data_ctl"].isBool()) {
                    //						auto data_ctl = file_log_cfg["file_size"].isBool();
                    //						if(data_ctl)
                    //						{
                    //							if(file_log_cfg.isMember("data_ctl_stamp") && file_log_cfg["data_ctl_stamp"].isInt())
                    //							{
                    //								auto v= file_log_cfg["data_ctl_stamp"].isInt();
                    //								logManagerUP.reset(new LogManager(log_path, v));
                    //								logManagerUP->Rotate();
                    //							}
                    //							else
                    //							{
                    //								logManagerUP.reset(new LogManager(log_path));
                    //								logManagerUP->Rotate();
                    //							}
                    //						}
                    //					}
                }

            }
            else
            {
                SPDLOG_LOGGER_ERROR(spdlog::default_logger(),
                                    "Setting rotate file log err! ");
                return ERR_CONFIG;
            }
        }

        if (logger_cfg.isMember("daily_log"))
        {
            auto file_log_cfg = logger_cfg["daily_log"];
            if (!file_log_cfg.isObject())
            {
                SPDLOG_LOGGER_ERROR(spdlog::default_logger(),
                                    "Setting rotate file log err! ");
                return ERR_CONFIG;
            }

            if (file_log_cfg.isMember("enable") && file_log_cfg["enable"].isBool())
            {

                if (file_log_cfg["enable"].asBool())
                {
                    std::string filename;
                    int level = 3;
                    bool with_console = false;
                    int file_size = 1024;
                    int file_num = 3;
                    int hour = 0;
                    int minute = 0;
                    int max_files = 0;
                    if (file_log_cfg.isMember("filename") && file_log_cfg["filename"].isString())
                    {
                        filename = log_path + "/" + file_log_cfg["filename"].asString();
                    }
                    else
                    {
                        SPDLOG_LOGGER_ERROR(spdlog::default_logger(),
                                            "Getting filename of rotate file log failed! ");
                        return ERR_CONFIG;
                    }
                    if (file_log_cfg.isMember("level") && file_log_cfg["level"].isInt())
                    {
                        level = file_log_cfg["level"].asInt();
                    }

                    if (file_log_cfg.isMember("stdout") && file_log_cfg["stdout"].isBool())
                    {
                        with_console = file_log_cfg["stdout"].asBool();
                    }

                    if (file_log_cfg.isMember("hour") && file_log_cfg["hour"].isInt())
                    {
                        hour = file_log_cfg["hour"].asInt();
                    }

                    if (file_log_cfg.isMember("minute") && file_log_cfg["minute"].isInt())
                    {
                        minute = file_log_cfg["minute"].asInt();
                    }
                    if (file_log_cfg.isMember("max_files") && file_log_cfg["max_files"].isInt())
                    {
                        max_files = file_log_cfg["max_files"].asInt();
                    }

                    auto daily_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(
                        filename, hour, minute, false, max_files);
                    daily_sink->set_level(spdlog::level::level_enum(level));

                    std::vector<spdlog::sink_ptr> sinks;
                    if (with_console)
                    {
                        sinks.push_back(console_sink);
                    }
                    sinks.push_back(daily_sink);
                    auto logger = std::make_shared<spdlog::logger>("daily", begin(sinks), end(sinks));
                    logger->flush_on(spdlog::level::err);
                    type_name_map[DailyLogger] = "daily";
                    name_logger_map["daily"] = logger;

                    if (file_log_cfg.isMember("default") && file_log_cfg["default"].isBool() &&
                        file_log_cfg["default"].asBool() == true)
                    {
                        spdlog::set_default_logger(logger);
                    }
                }

            }
            else
            {
                SPDLOG_LOGGER_ERROR(spdlog::default_logger(),
                                    "Setting daily log err! ");
                return ERR_CONFIG;
            }
        }

        inited = true;
        auto logger = name_logger_map.find("rotate") == name_logger_map.end()
                          ? spdlog::default_logger()
                          : name_logger_map["rotate"];
        logger->info("Sonli logger init success!");
        return SL_SUCCESS;
        
    }

    std::shared_ptr<spdlog::logger> SonLiLogger::getLogger(LoggerType logger_type) {
        if (!inited) {
            
            SPDLOG_LOGGER_WARN(spdlog::default_logger(), 
                   "SonLi Logger not init, got default logger! ");            
            return spdlog::default_logger();
        }

        if (logger_type == DefaultLogger) {
            return spdlog::default_logger();
        }

        if (type_name_map.find(logger_type) != type_name_map.end()) {
            return getLogger(type_name_map.at(logger_type));
        } 
        SPDLOG_LOGGER_WARN(spdlog::default_logger(), 
                    "Get SonLi Logger not support type, got default logger! ");
            return spdlog::default_logger();
    }

    std::shared_ptr<spdlog::logger> SonLiLogger::getLogger(std::string logger_name) {
        if (!inited) {
            SPDLOG_LOGGER_WARN(spdlog::default_logger(), 
                    "SonLi Logger not init, got default logger! ");
            return spdlog::default_logger();
        }

        if (name_logger_map.find(logger_name) != name_logger_map.end()) {
            return name_logger_map.at(logger_name);
        } 
        SPDLOG_LOGGER_WARN(spdlog::default_logger(), 
                    " {} Logger not exist, got default logger! ", logger_name.c_str());
            return spdlog::default_logger();
    }

} // namespace sonli 

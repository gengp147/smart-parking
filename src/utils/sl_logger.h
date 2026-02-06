/*** 
 * @Author: lwn
 * @Date: 2024-01-30 11:11:48
 * @LastEditors: lwn
 * @LastEditTime: 2024-02-18 18:37:11
 * @FilePath: /FalconEye/include/logging/logger.h
 * @Description: 
 */
#ifndef LOGGING_LOGGER_H_
#define LOGGING_LOGGER_H_
#include <string>
#include <memory>
#include "spdlog/spdlog.h"
#include <unordered_map>
#include <atomic>

namespace sonli {
    class LogManager;
    /*** 
     * @description: Singleton of SonLi logger, manage all loggers.  
     */
    class SonLiLogger {

		// std::unique_ptr<LogManager> logManagerUP;

        public:
            /***
             * Type of logger
             */   
            enum LoggerType {
                ConsoleLogger,
                BasicLogger,
                RotateLogger,
                DailyLogger,
                DefaultLogger
            };

        public:
            ~SonLiLogger();
            SonLiLogger(const SonLiLogger&) = delete;
            SonLiLogger& operator=(const SonLiLogger&) = delete;

            /*** 
             * @description: get instance of SonliLogger, the instance is singleton.
             * other methods can called by the instance only.
             * @return {SonLiLogger&} The instance of SonliLogger
             */
            static SonLiLogger& getInstance() {
                static SonLiLogger instance;
                return instance;
            };

            /*** 
             * @description: Init loggers according to config params or config file; if 
             * not init, default spdlog logger is available only.
             * @param {string&} config
             * @return {*}
             */
            int init(std::string& config);

            /*** 
             * @description: Get logger pointer according to specified type
             * @param {LoggerType} logger_type
             * @return {*}
             */
            std::shared_ptr<spdlog::logger> getLogger(LoggerType logger_type);

            /*** 
             * @description: Get logger pointer according to specified name
             * @param {string} logger_name
             * @return {*}
             */
            std::shared_ptr<spdlog::logger> getLogger(std::string logger_name);
        private:
            /***
             * Convert creating function to privatization, to user can not create instance manually.
            */
            SonLiLogger();

            std::unordered_map<LoggerType, std::string> type_name_map;
            std::unordered_map<std::string, std::shared_ptr<spdlog::logger>> name_logger_map;

            std::atomic_bool inited{false};
            
    };


#define SL_LOG_ROTATE_INFO(...) \
    sonli::SonLiLogger::getInstance().getLogger(sonli::SonLiLogger::RotateLogger)->info(__VA_ARGS__);

#define SL_LOG_ROTATE_DEBUG(...) \
    sonli::SonLiLogger::getInstance().getLogger(sonli::SonLiLogger::RotateLogger)->debug(__VA_ARGS__);

#define SL_LOG_ROTATE_WARN(...) \
    sonli::SonLiLogger::getInstance().getLogger(sonli::SonLiLogger::RotateLogger)->warn(__VA_ARGS__);

#define SL_LOG_ROTATE_ERROR(...) \
    sonli::SonLiLogger::getInstance().getLogger(sonli::SonLiLogger::RotateLogger)->error(__VA_ARGS__);

#define SL_LOG_DAILY_INFO(...) \
    sonli::SonLiLogger::getInstance().getLogger(sonli::SonLiLogger::DailyLogger)->info(__VA_ARGS__);

#define SL_LOG_DAILY_DEBUG(...) \
sonli::SonLiLogger::getInstance().getLogger(sonli::SonLiLogger::DailyLogger)->debug(__VA_ARGS__);

#define SL_LOG_DAILY_WARN(...) \
sonli::SonLiLogger::getInstance().getLogger(sonli::SonLiLogger::DailyLogger)->warn(__VA_ARGS__);

#define SL_LOG_DAILY_ERROR(...) \
sonli::SonLiLogger::getInstance().getLogger(sonli::SonLiLogger::DailyLogger)->error(__VA_ARGS__);

} // namespace sonli 

#endif
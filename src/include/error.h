/***
 * @Author: lwn
 * @Date: 2024-01-24 16:28:32
 * @LastEditors: lwn
 * @LastEditTime: 2024-05-13 16:35:24
 * @FilePath: \FalconEye\include\common\error.h
 * @Description:
 */
#ifndef _COMMON_ERROR_H_
#define _COMMON_ERROR_H_

namespace sonli
{
#define SL_SUCCESS 0             /** 成功 */
#define ERR_SIGN_CERT_FAIL -1000 /** 签名证书失败 */
#define ERR_NOT_INIT -1001       /** 未初始化 */
#define ERR_INIT_TWICE -1002     /** 重复初始化 */
#define ERR_INITED -1003         /** 已初始化 */
#define ERR_NOT_START -1004      /** 未启动 */
#define ERR_IS_START -1005       /** 已启动 */
#define ERR_START_TWICE -1006    /** 重复启动 */
#define ERR_CONFIG -1007         /** 配置错误 */
#define ERR_MEM_FAILED -1008     /** 内存分配失败 */
#define ERR_STOP_FAIL -1009      /** 停止失败 */
#define ERR_INIT_FAIL -1010      /** 初始化失败 */
#define ERR_START_FAIL -1011     /** 启动失败 */

#define ERR_MODULE_INIT_TWICE -2001    /** 模块重复初始化 */
#define ERR_MODULE_NOT_INIT -2002      /** 模块未初始化 */
#define ERR_FILE_NOT_EXIST -3001       /** 文件不存在 */
#define ERR_CONFIG_ILLEGAL -3002       /** 配置非法 */
#define ERR_CONFIG_INVALID_PARAM -3003 /** 配置无效参数 */

#define ERR_START_CALIBRATION_FAILED -4001  /** 启动校准失败 */
#define ERR_STOP_CALIBRATION_FAILED -4002   /** 停止校准失败 */
#define ERR_FINISH_CALIBRATION_FAILED -4003 /** 完成校准失败 */
#define ERR_NOT_START_CALIBRATION -4004     /** 未启动校准 */

#define ERR_PROCESS_FAILED -4501 /** 处理失败 */

#define ERR_INVALID_IMAGE -5001        /** 无效图像 */
#define ERR_IMAGE_PROCESS_FAILED -5002 /** 图像处理失败 */

#define ERR_DATABASE_EXEC_ERR -7001           /** 数据库执行错误 */
#define ERR_DATABASE_TABLE_CREATE_TWICE -7002 /** 数据库表重复创建错误 */

#define ERR_CAPTURE_CONTROL_PICID_ABNORMAL -8001 /** 捕获控制图片ID异常 */
#define ERR_CAPTURE_CONTROL_PICID -8002          /** 捕获控制图片ID错误 */
#define ERR_CAPTURE_CONTROL_FRAMEID -8003        /** 捕获控制帧ID错误 */

#define ERR_STITCH_IMAGE_ERROR -10001 /** 拼接图像错误 */

#define ERR_STITCH_IMAGE_DATA_ERROR -10002 /** 拼接图像数据错误 */

#define ERROR_INVALID_ARGUMENTS -11001        /** 无效参数错误 */
#define ERR_CONTROL_CONTROLID_EXISTED -1102   /** 控制ID已存在错误 */
#define ERR_CONTROL_CONTROLID_NOT_FOUND -1103 /** 控制ID未找到错误 */
#define ERR_EVENT_ID_EXISTED -1104            /** 事件ID已存在错误 */
#define ERR_EVENT_ID_NOT_FOUND -1105          /** 事件ID未找到错误 */

#define ERR_RUNTIME_ERROR -11006 /** 运行时错误 */

} // namespace sonli

#endif
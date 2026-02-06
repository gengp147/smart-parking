#ifndef ACL_DET_YOLO_FRAME_INAGE_DATA_H
#define ACL_DET_YOLO_FRAME_INAGE_DATA_H
#include "../interfaces/aih_type.h"
#include "memory"
#include "vulcan/data_struct.h"

namespace sonli
{

struct FrameImageData final
{
    int width{};                     /** 图片宽度 */
    int height{};                    /** 图片高度 */
    aih::U64 stride[3] = {0};        /** 图片 行间距 */
    aih::U64 vir_[3] = {0};          /** 图片 虚拟地址 */
    aih::U64 phy_addr[3] = {0};      /** 图片 物理地址 */
    aih::TimeInfo time_info_;        /** 图片 时间信息 */
    aih::U64 frame_size{0};          /** 图片 大小 */
    aih::U64 frame_size_plate[4]{0}; /** 图片 平面大小 */
};

typedef std::shared_ptr<FrameImageData> FrameDataPtr;

struct FrameImageDataExt final : public vulcan::BaseData
{
    FrameImageDataExt() : vulcan::BaseData() { name_ = "image"; }

    FrameDataPtr _original_frame; /** 原始图片 */
    FrameDataPtr _thumbnail;      /** 缩略图 */
    long frame_id = 0;            /** 图片id */
    aih::TimeInfo time_info;      /** 图片 时间信息 */
};

} // namespace sonli

#endif // ACL_DET_YOLO_FRAME_INAGE_DATA_H
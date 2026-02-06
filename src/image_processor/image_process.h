#ifndef _IMAGE_PROCESS_H_
#define _IMAGE_PROCESS_H_

#include <atomic>
// #include "data_type.h"
#include "image_ive_common.h"
#include "frame_inage_data.h"

namespace sonli
{
    class ImageProcess
    {
    public:
        ImageProcess();
        ~ImageProcess();

    public:
        /// @brief create and start channel
        /// @param maxWidth max image width in channel
        /// @param maxHeight max image height  in channel
        /// @return success or not
        int init(int out_width, int out_height);

        int imageCrop(const FrameImageData* pInputData, ot_svp_img* pOutputData, Rect roi_rect, td_bool is_mmz_cached=TD_FALSE);
        int imageResizeInit(ot_svp_dst_img *dst_img, td_bool is_mmz_cached=TD_FALSE, int num=1);
        int imageResize(const ot_svp_src_img *src_img, ot_svp_dst_img *dst_img, int num=1);
        int imageCropResize(const FrameImageData *pInputData, ot_svp_dst_img *dst_img, Rect roi_rect, td_bool is_mmz_cached=TD_FALSE);

    private:
        int alignUp(int width, int mini_stride);
        int alignDown(int width, int mini_stride);

        std::atomic_bool inited_{false};
        int out_width_ = 0;
        int out_height_ = 0;
        int out_stride_ = 0;
    };

}

#endif // _IMAGE_PROCESS_H_

#include <string.h>
#include <sys/time.h>
#include "../utils/sl_logger.h"
#include "error.h"
#include "image_process.h"
#include "../utils/time_utils.h"


namespace sonli
{
    ImageProcess::ImageProcess() {}
    ImageProcess::~ImageProcess() {}

    int ImageProcess::init(int out_width, int out_height)
    {
        out_width_ = out_width;
        out_height_ = out_height;
        out_stride_ = alignUp(out_width_, 16);

        inited_ = true;
        return SL_SUCCESS;
    }

    int ImageProcess::alignUp(int width, int mini_stride)
    {
        return ((((width) + ((mini_stride)-1)) / (mini_stride)) * (mini_stride));
    }

    int ImageProcess::alignDown(int width, int mini_stride)
    {
        return ((((width)) / (mini_stride)) * (mini_stride));
    }

    int ImageProcess::imageResizeInit(ot_svp_dst_img *dst_img, td_bool is_mmz_cached, int num)
    {
        td_u32 size = out_stride_ * out_height_ * 3 / 2;
        td_s32 ret;
        td_void *virt_addr = TD_NULL;

        for (int img_idx = 0; img_idx < num; img_idx++)
        {
            dst_img[img_idx].type = OT_SVP_IMG_TYPE_YUV420SP;
            dst_img[img_idx].height = out_height_;
            dst_img[img_idx].width = out_width_;
            dst_img[img_idx].stride[0] = out_stride_;
            dst_img[img_idx].stride[1] = out_stride_;

            if (is_mmz_cached == TD_FALSE)
            {
                ret = ss_mpi_sys_mmz_alloc((td_phys_addr_t *)&(dst_img[img_idx].phys_addr[0]), (td_void **)&virt_addr,
                                           TD_NULL, TD_NULL, size);
                if (ret != TD_SUCCESS)
                {
                    SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error(
                        "imageResize ss_mpi_sys_mmz_alloc error! ret {}", ret);
                    
                    for (int i = 0; i < img_idx; i++)
                    {
                        sample_svp_mmz_free(dst_img[i].phys_addr[0], dst_img[i].virt_addr[0]);
                    }
                    return ret;
                }
            }
            else
            {
                ret = ss_mpi_sys_mmz_alloc_cached((td_phys_addr_t *)&(dst_img[img_idx].phys_addr[0]), (td_void **)&virt_addr,
                                                  TD_NULL, TD_NULL, size);
                if (ret != TD_SUCCESS)
                {
                    SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error(
                        "imageResize ss_mpi_sys_mmz_alloc_cached error! ret {}", ret);
                    
                    for (int i = 0; i < img_idx; i++)
                    {
                        sample_svp_mmz_free(dst_img[i].phys_addr[0], dst_img[i].virt_addr[0]);
                    }
                    return ret;
                }
            }

            dst_img[img_idx].virt_addr[0] = sample_svp_convert_ptr_to_addr(td_u64, virt_addr);
            dst_img[img_idx].phys_addr[1] = dst_img[img_idx].phys_addr[0] + out_stride_ * out_height_;
            dst_img[img_idx].virt_addr[1] = dst_img[img_idx].virt_addr[0] + out_stride_ * out_height_;
        }
        return ret;
    }

    // YUV420SP-NV21: YYYYVUVU
    int ImageProcess::imageResize(const ot_svp_src_img *src_img, ot_svp_dst_img *dst_img, int num)
    {
        td_s32 ret;

        ot_ive_resize_ctrl ctrl;
        ctrl.mode = OT_IVE_RESIZE_MODE_LINEAR;
        ctrl.num = num;
        ctrl.mem.size = sample_ive_resize_get_assist_buf_size(src_img, dst_img, ctrl.num);
        if (ctrl.mem.size == 0)
        {
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error(
                "imageResize sample_ive_resize_get_assist_buf_size 0!");
            return ERR_IMAGE_PROCESS_FAILED;
        }

        ret = sample_common_ive_create_mem_info(&ctrl.mem, ctrl.mem.size);
        if (ret != TD_SUCCESS)
        {
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error(
                "imageResize sample_common_ive_create_mem_info error! ret {}", ret);
            return ret;
        }

        td_bool is_instant = TD_TRUE;
        td_bool is_block = TD_TRUE;
        td_bool is_finish = TD_FALSE;
        ot_ive_handle handle;

        ret = ss_mpi_ive_resize(&handle, src_img, dst_img, &ctrl, is_instant);
        if (ret != TD_SUCCESS)
        {
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("imageResize ss_mpi_ive_resize error! ret {}", ret);
            sample_svp_mmz_free(ctrl.mem.phys_addr, ctrl.mem.virt_addr);
            return ret;
        }

        ret = ss_mpi_ive_query(handle, &is_finish, is_block);
        while (ret == OT_ERR_IVE_QUERY_TIMEOUT)
        {
            usleep(OT_SAMPLE_IVE_RESIZE_SLEEP);
            ret = ss_mpi_ive_query(handle, &is_finish, is_block);
        }

        if (ret != TD_SUCCESS)
        {
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("imageResize ss_mpi_ive_resize error! ret {}", ret);
        }

        sample_svp_mmz_free(ctrl.mem.phys_addr, ctrl.mem.virt_addr);

        return ret;
    }

    // YUV420SP-NV21: YYYYVUVU
    int ImageProcess::imageCrop(const FrameImageData *pInputData, ot_svp_img *pOutputData, Rect roi_rect, td_bool is_mmz_cached)
    {
        if (roi_rect.width_ < 32 || roi_rect.height_ < 4)
        {
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error(
                "imageCrop error! roi_rect->width {}, roi_rect->height {}", roi_rect.width_, roi_rect.height_);
            return ERR_IMAGE_PROCESS_FAILED;
        }

        roi_rect.x_ = alignDown(roi_rect.x_, 2);
        roi_rect.y_ = alignDown(roi_rect.y_, 2);
        roi_rect.height_ = alignDown(roi_rect.height_, 2);
        roi_rect.width_ = alignDown(roi_rect.width_, 2);

        td_s32 ret = TD_SUCCESS;
        td_u32 height = roi_rect.height_;
        td_u32 width = roi_rect.width_;
        td_u32 stride = alignUp(width, 16);
        td_u32 size = height * stride * 3 / 2;

        td_void *dst_virt_addr = TD_NULL;

        if (is_mmz_cached == TD_FALSE)
        {
            ret = ss_mpi_sys_mmz_alloc(&(pOutputData->phys_addr[0]),
                                       (td_void **)&dst_virt_addr, TD_NULL, TD_NULL, size);
        }
        else
        {
            ret = ss_mpi_sys_mmz_alloc_cached(&(pOutputData->phys_addr[0]),
                                              (td_void **)&dst_virt_addr, TD_NULL, TD_NULL, size);
        }

        if (ret != TD_SUCCESS)
        {
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error(
                "imageCrop ss_mpi_sys_mmz_alloc_cached error! ret {}", ret);
            return ret;
        }

        ot_ive_handle handle[2];
        ot_svp_data src[2];
        ot_svp_dst_data dst[2];
        // ot_ive_dma_ctrl ctrl = {OT_IVE_DMA_MODE_DIRECT_COPY};
        ot_ive_dma_ctrl ctrl = {OT_IVE_DMA_MODE_DIRECT_COPY, OT_IVE_DMA_MASK_MODE_32BIT, OT_IVE_DMA_MASK_MODE_32BIT, 0, 0, 0, 0, 0, 0};
        td_bool is_instant = TD_TRUE;

        for (int ch_i = 0; ch_i < 2; ch_i++)
        {
            src[ch_i].width = width;
            src[ch_i].stride = pInputData->stride[0];

            if (ch_i == 1)
            {
                src[ch_i].phys_addr = pInputData->phy_addr[ch_i] + roi_rect.y_ * pInputData->stride[0] / 2 + roi_rect.x_;
                src[ch_i].virt_addr = pInputData->vir_[ch_i] + roi_rect.y_ * pInputData->stride[0] / 2 + roi_rect.x_;
                src[ch_i].height = height / 2;

                dst[ch_i].phys_addr = pOutputData->phys_addr[0] + height * stride;
                dst[ch_i].virt_addr = (td_u64)(td_uintptr_t)((char *)dst_virt_addr + height * stride);
                dst[ch_i].height = height / 2;
            }
            else
            {

                src[ch_i].phys_addr = pInputData->phy_addr[ch_i] + roi_rect.y_ * pInputData->stride[0] + roi_rect.x_;
                src[ch_i].virt_addr = pInputData->vir_[ch_i] + roi_rect.y_ * pInputData->stride[0] + roi_rect.x_;
                src[ch_i].height = height;

                dst[ch_i].phys_addr = pOutputData->phys_addr[ch_i];
                dst[ch_i].virt_addr = (td_u64)(td_uintptr_t)dst_virt_addr;
                dst[ch_i].height = height;
            }
            dst[ch_i].stride = stride;
            dst[ch_i].width = width;

            ret = ss_mpi_ive_dma(&handle[ch_i], &src[ch_i], &dst[ch_i], &ctrl, is_instant);
            if (ret != TD_SUCCESS)
            {
                sample_svp_mmz_free(dst[ch_i].phys_addr, dst[ch_i].virt_addr);
                if (ch_i == 1)
                {
                    sample_svp_mmz_free(dst[0].phys_addr, dst[0].virt_addr);
                }

                SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error(
                    "imgCrop ss_mpi_ive_dma error! ret {}, ch_i {}", ret, ch_i);

                return ret;
            }

            td_bool is_block = TD_TRUE;
            td_bool is_finish = TD_FALSE;

            ret = ss_mpi_ive_query(handle[ch_i], &is_finish, is_block);
            while (ret == OT_ERR_IVE_QUERY_TIMEOUT)
            {
                usleep(OT_SAMPLE_IVE_RESIZE_SLEEP);
                ret = ss_mpi_ive_query(handle[ch_i], &is_finish, is_block);
            }

            if (ret != TD_SUCCESS)
            {
                sample_svp_mmz_free(dst[ch_i].phys_addr, dst[ch_i].virt_addr);
                if (ch_i == 1)
                {
                    sample_svp_mmz_free(dst[0].phys_addr, dst[0].virt_addr);
                }

                SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error(
                    "imageCrop ss_mpi_ive_query error! ret {}, ch_i {}", ret, ch_i);

                return ret;
            }
        }

        pOutputData->height = dst[0].height;
        pOutputData->width = dst[0].width;
        pOutputData->stride[0] = dst[0].stride;
        pOutputData->stride[1] = dst[0].stride;

        pOutputData->phys_addr[0] = dst[0].phys_addr;
        pOutputData->phys_addr[1] = dst[1].phys_addr;

        pOutputData->virt_addr[0] = dst[0].virt_addr;
        pOutputData->virt_addr[1] = dst[1].virt_addr;

        pOutputData->type = OT_SVP_IMG_TYPE_YUV420SP;

        return TD_SUCCESS;
    }

    // YUV420SP-NV21: YYYYVUVU
    int ImageProcess::imageCropResize(const FrameImageData *pInputData, ot_svp_dst_img *dst_img, Rect roi_rect, td_bool is_mmz_cached)
    {
        // time_t s_time = getSystemTimeUS();

        ot_svp_img pTempData;
        int ret = imageCrop(pInputData, &pTempData, roi_rect, is_mmz_cached);
        if (ret != TD_SUCCESS)
        {
            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("imageCropResize imageCrop error! ret {}", ret);
            return ret;
        }
        // std::cout << " ----------------imageCrop time: " << getSystemTimeUS() - s_time << std::endl;

        ret = imageResize(&pTempData, dst_img);

        // std::cout << " ----------------imageResize time: " << getSystemTimeUS() - s_time << std::endl;

        if (ret != TD_SUCCESS)
        {

            SonLiLogger::getInstance().getLogger(SonLiLogger::RotateLogger)->error("imageCropResize imageResize error! ret {}", ret);
        }
        sample_svp_mmz_free(pTempData.phys_addr[0], pTempData.virt_addr[0]);

        return ret;
    }
}

#ifndef _IMAGE_IVE_COMMON_H_
#define _IMAGE_IVE_COMMON_H_

#include <atomic>
#include "data_type.h"
#include "inc/ss_mpi_ive.h"
#include "inc/ss_mpi_sys_mem.h"


#define sample_svp_convert_ptr_to_addr(type, addr) ((type)(td_uintptr_t)(addr))

#define OT_SAMPLE_IVE_RESIZE_D1_WIDTH 720
#define OT_SAMPLE_IVE_RESIZE_D1_HEIGHT 576
#define OT_SAMPLE_IVE_RESIZE_NUM 8
#define OT_SAMPLE_IVE_RESIZE_SLEEP 30
#define OT_SAMPLE_RESIZE_MAX_TILE_METRIC_8BIT 2032
#define OT_SAMPLE_RESIZE_MAX_TILE_METRIC_16BIT 1016
#define OT_SAMPLE_RESIZE_TILE_ADJUST 2
#define OT_SAMPLE_RESIZE_ASSIST_UNIT_SIZE 48
#define OT_SAMPLE_RESIZE_CBCR_NUM_2 2
#define OT_SAMPLE_RESIZE_CBCR_NUM_3 3

#define OT_SAMPLE_IVE_PSP_QUERY_SLEEP           30

#define OT_SAMPLE_IVE_PSP_MAX_POINT_PAIR_NUM    68
#define OT_SAMPLE_IVE_PSP_POINT_PAIR_NUM        4
#define OT_SAMPLE_IVE_PSP_LEFT_SHIT             2
#define OT_SAMPLE_IVE_PSP_ROI_NUM               64


/* free mmz */
#ifndef sample_svp_mmz_free
#define sample_svp_mmz_free(phys, virt)                                                 \
do {                                                                                    \
    if (((phys) != 0) && ((virt) != 0)) {                                               \
        ss_mpi_sys_mmz_free((td_phys_addr_t)(phys), (td_void*)(td_uintptr_t)(virt));    \
        (phys) = 0;                                                                     \
        (virt) = 0;                                                                     \
    }                                                                                   \
} while (0)
#endif
namespace sonli
{
    td_u32 inline static sample_ive_resize_get_tile_metric(td_u32 src_metric, td_u32 dst_metric,
                                                    ot_svp_img_type img_type)
    {
        td_s32 tmp_metric = OT_SAMPLE_RESIZE_MAX_TILE_METRIC_8BIT;
        td_u32 src_tile_num, scale, dst_tmp_tile, src_tmp_tile, tile_remainder;
        switch (img_type)
        {
        case OT_SVP_IMG_TYPE_YUV420SP:
        case OT_SVP_IMG_TYPE_YUV422SP:
        case OT_SVP_IMG_TYPE_U8C3_PLANAR:
        case OT_SVP_IMG_TYPE_U8C1:
        case OT_SVP_IMG_TYPE_S8C1:
        {
            tmp_metric = OT_SAMPLE_RESIZE_MAX_TILE_METRIC_8BIT;
            break;
        }
        case OT_SVP_IMG_TYPE_U16C1:
        case OT_SVP_IMG_TYPE_S16C1:
        {
            tmp_metric = OT_SAMPLE_RESIZE_MAX_TILE_METRIC_16BIT;
            break;
        }
        default:
            break;
        }
        scale = (src_metric / dst_metric) + ((src_metric % dst_metric == 0) ? 0 : 1);
        if (dst_metric < src_metric)
        {
            src_tile_num = (src_metric / tmp_metric) + ((src_metric % tmp_metric == 0) ? 0 : 1);
            dst_tmp_tile = dst_metric / src_tile_num;
            src_tmp_tile = src_metric / src_tile_num + ((src_metric % src_tile_num == 0) ? 0 : 1);
            tile_remainder = (tmp_metric - src_tmp_tile) / scale;
            tmp_metric = (dst_tmp_tile + tile_remainder) & ~0x1;
            if (tile_remainder == 0)
            {
                tmp_metric -= OT_SAMPLE_RESIZE_TILE_ADJUST;
            }
        }
        else if (dst_metric == src_metric || scale == 1)
        {
            tmp_metric -= OT_SAMPLE_RESIZE_TILE_ADJUST;
        }
        return (td_u32)tmp_metric;
    }

td_u32 inline sample_ive_resize_get_tile_num(td_u32 width, td_u32 height, td_u32 tile_w, td_u32 tile_h)
    {
        td_u32 x_num, y_num;
        if ((tile_w == 0) || (tile_h == 0))
        {
            return 0;
        }
        x_num = (width / tile_w) + ((width % tile_w) == 0 ? 0 : 1);
        y_num = (height / tile_h) + ((height % tile_h) == 0 ? 0 : 1);
        return (x_num * y_num);
    }

    td_u32 inline sample_ive_resize_get_yuv420sp_tile_num(td_u32 src_width, td_u32 dst_width, td_u32 src_height,
                                                          td_u32 dst_height)
    {
        td_u32 j, tile_width, tile_height;
        td_u32 num = 0;
        tile_width = sample_ive_resize_get_tile_metric(src_width, dst_width, OT_SVP_IMG_TYPE_YUV420SP);
        for (j = 0; j < OT_SAMPLE_RESIZE_CBCR_NUM_2; j++)
        {
            tile_height = sample_ive_resize_get_tile_metric(src_height >> j, dst_height >> j, OT_SVP_IMG_TYPE_YUV420SP);
            num += sample_ive_resize_get_tile_num(dst_width, dst_height >> j, tile_width, tile_height);
        }
        return num;
    }



    td_u32 inline sample_ive_resize_get_assist_buf_size(const ot_svp_src_img src[], const ot_svp_dst_img dst[],
                                                        td_u32 resize_num)
    {
        td_u32 num = 0;
        for (unsigned int i = 0; i < resize_num; i++)
        {
            switch (src[i].type)
            {
            case OT_SVP_IMG_TYPE_YUV420SP:
            {
                num += sample_ive_resize_get_yuv420sp_tile_num(src[i].width, dst[i].width, src[i].height,
                                                               dst[i].height);
                break;
            }
            default:
                break;
            }
        }
        return num * OT_SAMPLE_RESIZE_ASSIST_UNIT_SIZE;
    }

    td_s32 inline sample_common_ive_create_mem_info(ot_svp_mem_info *mem_info, td_u32 size)
    {
        td_s32 ret = OT_ERR_IVE_NULL_PTR;
        td_void *virt_addr = TD_NULL;

        mem_info->size = size;
        ret = ss_mpi_sys_mmz_alloc((td_phys_addr_t *)&mem_info->phys_addr, (td_void **)&virt_addr, TD_NULL, TD_NULL, size);
        if(ret != TD_SUCCESS){
            return ret;
        }
        mem_info->virt_addr = sample_svp_convert_ptr_to_addr(td_u64, virt_addr);

        return TD_SUCCESS;
    }
}

#endif // _IMAGE_IVE_COMMON_H_

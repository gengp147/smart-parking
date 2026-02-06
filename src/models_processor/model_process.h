#ifndef _MODEL_PROCESS_H_
#define _MODEL_PROCESS_H_

#include "model_datatype.h"
#include "char_code.h"

#include <map>
#include <math.h>
#include <numeric>
#include <vector>
#define PI 3.141592653

namespace sonli
{

template <typename T>
void
qsort_descent_inplace(std::vector<T> &elements, int left, int right)
{
    int i = left;
    int j = right;
    T p = elements[(left + right) / 2];

    while (i <= j)
    {
        while (elements[i] > p)
            i++;

        while (elements[j] < p)
            j--;

        if (i <= j)
        {
            // swap
            std::swap(elements[i], elements[j]);

            i++;
            j--;
        }
    }

#pragma omp parallel sections
    {
#pragma omp section
        {
            if (left < j)
                qsort_descent_inplace<T>(elements, left, j);
        }
#pragma omp section
        {
            if (i < right)
                qsort_descent_inplace<T>(elements, i, right);
        }
    }
}

template <typename T, typename E>
void
qsort_descent_inplace_with_index(std::vector<T> &elements, int left, int right, std::vector<E> &indices)
{
    int i = left;
    int j = right;
    T p = elements[(left + right) / 2];

    while (i <= j)
    {
        while (elements[i] > p)
            i++;

        while (elements[j] < p)
            j--;

        if (i <= j)
        {
            // swap
            std::swap(elements[i], elements[j]);
            std::swap(indices[i], indices[j]);
            i++;
            j--;
        }
    }

#pragma omp parallel sections
    {
#pragma omp section
        {
            if (left < j)
                qsort_descent_inplace_with_index<T, E>(elements, left, j, indices);
        }
#pragma omp section
        {
            if (i < right)
                qsort_descent_inplace_with_index<T, E>(elements, i, right, indices);
        }
    }
}

template <typename T>
void
topK(T *feats, int k, std::vector<int> dims, std::vector<std::vector<T>> &top_value,
     std::vector<std::vector<int>> &indices, int stride = 0, int dim = -1)
{
    std::vector<int>::iterator iter;
    if (dim >= 0)
    {
        iter = dims.begin() + dim;
    }
    else
    {
        iter = dims.end() + dim;
    }

    int sample_len = *iter;

    // SONLI_CHECK_GE(sample_len, k) << "Length of sampling dimension is less than K";
    if (sample_len < k)
    {
        // TODO
        // SonLiLogger::getInstance()
        //     .getLogger(SonLiLogger::RotateLogger)
        //     ->warn("Length of sampling dimension mustn't be less than K!");
    }
    int prefix_len = 1;
    int postfix_len = 1;
    for (std::vector<int>::iterator i = dims.begin(); i != iter; i++)
    {
        int axis_len = *i;
        // TODO::
        // SONLI_CHECK_GE(axis_len, 1);
        if (axis_len < 1)
        {
            // TODO
            // SonLiLogger::getInstance()
            //     .getLogger(SonLiLogger::RotateLogger)
            //     ->warn("Length of axis_len mustn't be less than 1!");
        }
        prefix_len *= axis_len;
    }

    int valid_data_cardinal = 1;
    int valid_data_len = dims[dims.size() - 1];

    for (std::vector<int>::iterator j = iter + 1; j != dims.end(); j++)
    {
        int axis_len = *j;
        int valid_len = axis_len;
        if (j == dims.end() - 1)
        {

            valid_len = 1;
            if (stride != 0)
            {
                axis_len = stride;
            }
        }
        // SONLI_CHECK_GE(axis_len, 1);
        if (axis_len < 1)
        {
            // TODO
            // SonLiLogger::getInstance()
            //     .getLogger(SonLiLogger::RotateLogger)
            //     ->warn("Length of axis_len mustn't be less than 1!");
        }
        valid_data_cardinal *= valid_len;
        postfix_len *= axis_len;
    }

    int vec_num = prefix_len * postfix_len;
    int sample_algin_size = (iter == dims.end() - 1 && stride != 0) ? stride : sample_len;

    top_value.clear();
    indices.clear();
    top_value.resize(vec_num);
    indices.resize(vec_num);

    for (int i = 0; i < prefix_len; i++)
    {
        for (int j = 0; j < postfix_len; j++)
        {
            if (j % valid_data_cardinal >= valid_data_len)
            {
                continue;
            }
            int feats_idx = i * (sample_algin_size * postfix_len) + j;

            T value = feats[feats_idx];
            std::vector<T> each_K_value;
            std::vector<int> each_K_indix;
            for (int d = 0; d < k; d++)
            {

                each_K_value.emplace_back(feats[feats_idx + postfix_len * d]);
                each_K_indix.emplace_back(d);
            }
            if (k > 1)
            {
                qsort_descent_inplace_with_index<T, int>(each_K_value, 0, k - 1, each_K_indix);
            }

            for (int s = k; s < sample_len; s++)
            {
                // feats_idx += (postfix_len * s);
                int eindex = feats_idx + postfix_len * s;
                T value = feats[eindex];
                for (int m = 0; m < k; m++)
                {
                    if (value > each_K_value[m])
                    {
                        each_K_value.insert(each_K_value.begin() + m, value);
                        each_K_indix.insert(each_K_indix.begin() + m, s);
                        each_K_value.pop_back();
                        each_K_indix.pop_back();
                        break;
                    }
                }
            }
            top_value[i * postfix_len + j] = each_K_value;
            indices[i * postfix_len + j] = each_K_indix;
        }
    }
}

void inline generate_grids_and_stride(const int target_wsize, const int target_hsize, int stride,
                                      std::vector<model_datatype::GridAndStride> &grid_strides)
{
    int num_wgrid = target_wsize / stride;
    int num_hgrid = target_hsize / stride;
    for (int g1 = 0; g1 < num_hgrid; g1++)
    {
        for (int g0 = 0; g0 < num_wgrid; g0++)
        {
            grid_strides.push_back(model_datatype::GridAndStride(g0, g1, stride, stride));
        }
    }
}

void inline generate_grids_and_stride(const int target_wsize, const int target_hsize, int stride_w, int stride_h,
                                      std::vector<model_datatype::GridAndStride> &grid_strides)
{
    int num_wgrid = target_wsize / stride_w;
    int num_hgrid = target_hsize / stride_h;
    for (int g1 = 0; g1 < num_hgrid; g1++)
    {
        for (int g0 = 0; g0 < num_wgrid; g0++)
        {
            grid_strides.push_back(model_datatype::GridAndStride(g0, g1, stride_w, stride_h));
        }
    }
}

template <typename T>
inline float
intersection_area(const T &a, const T &b)
{

    auto inter = a.rect_ & b.rect_;
    return inter.area();
}

template <typename T> void inline coord_mp_2_ip(std::vector<T> &objects, int offset_w, int offset_h)
{
    for (int idx = 0; idx < objects.size(); idx++)
    {
        objects[idx].rect_.x += offset_w;
        objects[idx].rect_.y += offset_h;
    }
}

inline float
sigmoid(float x)
{
    return static_cast<float>(1.f / (1.f + exp(-x)));
}

template <typename T>
void inline decode_box(const T *out_tensor, const std::vector<int>& output_shape_list, std::vector<model_datatype::Object> &objects,
                       int in_net_height_, int in_net_width_, int num_class_, float prob_threshold_,
                       const std::vector<model_datatype::Anchor>& anchors_)
{
    // one output of net
    int batch_size = output_shape_list[0];
    int out_height = output_shape_list[1];
    int out_width = output_shape_list[2];
    int attr_len = output_shape_list[3];

    float stride_h = in_net_height_ / out_height;
    float stride_w = in_net_width_ / out_width;

    std::vector<std::vector<float>> scaled_anchors;
    for (int j = 0; j < anchors_.size(); j++)
    {
        float scaled_height = (float)anchors_[j].h_ / stride_h;
        float scaled_width = (float)anchors_[j].w_ / stride_w;
        std::vector<float> scaled{scaled_height, scaled_width};
        scaled_anchors.emplace_back(scaled);
    }
    // std::vector<std::vector<float>> bboxes;
    for (int h = 0; h < out_height; h++)
    {
        for (int w = 0; w < out_width; w++)
        {
            int base_pos = (h * out_width + w) * attr_len;
            for (int an = 0; an < scaled_anchors.size(); an++)
            {
                float box_objectness = out_tensor[base_pos + scaled_anchors.size() * 4 + an];
                for (int class_idx = 0; class_idx < num_class_; class_idx++)
                {
                    float box_cls_score = out_tensor[base_pos + scaled_anchors.size() * 5 + class_idx];
                    float box_prob = box_objectness * box_cls_score;

                    if (box_prob > prob_threshold_)
                    {
                        model_datatype::Object obj;
                        float x_center = (out_tensor[base_pos + an * 4 + 0] * 2. - 0.5) + w;
                        float y_center = (out_tensor[base_pos + an * 4 + 1] * 2. - 0.5) + h;
                        float box_w = std::pow((out_tensor[base_pos + an * 4 + 2] * 2.), 2) * scaled_anchors[an][1];
                        float box_h = std::pow((out_tensor[base_pos + an * 4 + 3] * 2.), 2) * scaled_anchors[an][0];
                        float x0 = x_center - box_w * 0.5f;
                        float y0 = y_center - box_h * 0.5f;
                        std::vector<Point2f> landmarks;
                        for (int lm_idx = 0; lm_idx < 8; lm_idx += 2)
                        {
                            float lm_x =
                                (out_tensor[base_pos + scaled_anchors.size() * 5 + num_class_ + an * 8 + lm_idx] * 8. -
                                 4.) *
                                    scaled_anchors[an][1] +
                                w;
                            float lm_y =
                                (out_tensor[base_pos + scaled_anchors.size() * 5 + num_class_ + an * 8 + lm_idx + 1] *
                                     8. -
                                 4.) *
                                    scaled_anchors[an][0] +
                                h;
                            landmarks.emplace_back(Point2f(lm_x / out_width, lm_y / out_height));
                        }
                        // normalization
                        obj.rect_.x_ = x0 / out_width;
                        obj.rect_.y_ = y0 / out_height;
                        obj.rect_.width_ = box_w / out_width;
                        obj.rect_.height_ = box_h / out_height;
                        obj.landmark_ = landmarks;
                        obj.score_ = box_prob;
                        obj.label_ = class_idx;

                        objects.push_back(obj);
                    }
                }
            }
        }
    }
    //}
}

void inline generate_grids_and_stride_yolox(int in_net_width_, int in_net_height_, int num_grid_x, int num_grid_y,
                                            std::vector<model_datatype::GridAndStride> &grid_strides)
{

    // int num_grid_y = INPUT_H / stride;
    // int num_grid_x = INPUT_W / stride;
    int stride_h = in_net_height_ / num_grid_y;
    int stride_w = in_net_width_ / num_grid_x;
    for (int g1 = 0; g1 < num_grid_y; g1++)
    {
        for (int g0 = 0; g0 < num_grid_x; g0++)
        {
            grid_strides.push_back(model_datatype::GridAndStride(g0, g1, stride_w, stride_h));
        }
    }
}

template <typename T>
void inline decode_lp_detect(const T *out_tensor, int out_num, std::vector<model_datatype::Object> &objects, int num_class_,
                             float prob_threshold_)
{
    std::map<int, int> useful_anchor_idx;

    for (int class_idx = 0; class_idx < num_class_; class_idx++)
    {
        int base = (4 + class_idx) * out_num;
        for (int anchor_idx = 0; anchor_idx < out_num; anchor_idx++)
        {
            if (out_tensor[anchor_idx + base] > prob_threshold_)
            {
                useful_anchor_idx[anchor_idx] = anchor_idx;
            }
        }
    }

    for (std::pair<int, int> anchor : useful_anchor_idx)
    {
        int label = -1;
        float label_conf = prob_threshold_;
        int anchor_idx = anchor.first;

        for (int class_idx = 0; class_idx < num_class_; class_idx++)
        {
            float cls_conf = out_tensor[anchor_idx + (4 + class_idx) * out_num];
            if (cls_conf > label_conf)
            {
                label = class_idx;
                label_conf = cls_conf;
            }
        }
        if (label < 0)
        {
            continue;
        }

        model_datatype::Object obj;
        obj.label_ = label;
        obj.score_ = label_conf;

        float x0 = out_tensor[anchor_idx];
        float y0 = out_tensor[anchor_idx + out_num];
        float box_w = out_tensor[anchor_idx + 2 * out_num];
        float box_h = out_tensor[anchor_idx + 3 * out_num];

        obj.rect_.x_ = x0 - box_w / 2.0;
        obj.rect_.y_ = y0 - box_h / 2.0;
        obj.rect_.width_ = box_w;
        obj.rect_.height_ = box_h;

        objects.push_back(obj);
    }


    // for (int anchor_idx = 0; anchor_idx < out_num; anchor_idx++)
    // {
    //     int label = -1;
    //     float label_conf = prob_threshold_;
    //
    //     for (int class_idx = 0; class_idx < num_class_; class_idx++)
    //     {
    //         float cls_conf = out_tensor[anchor_idx + (4 + class_idx) * out_num];
    //         if (cls_conf > label_conf)
    //         {
    //             label = class_idx;
    //             label_conf = cls_conf;
    //         }
    //     }
    //     if (label < 0)
    //     {
    //         continue;
    //     }
    //
    //     model_datatype::Object obj;
    //     obj.label_ = label;
    //     obj.score_ = label_conf;
    //
    //     float x0 = out_tensor[anchor_idx];
    //     float y0 = out_tensor[anchor_idx + out_num];
    //     float box_w = out_tensor[anchor_idx + 2 * out_num];
    //     float box_h = out_tensor[anchor_idx + 3 * out_num];
    //
    //     obj.rect_.x_ = x0 - box_w / 2.0;
    //     obj.rect_.y_ = y0 - box_h / 2.0;
    //     obj.rect_.width_ = box_w;
    //     obj.rect_.height_ = box_h;
    //
    //     objects.push_back(obj);
    // }
}

template <typename T>
void inline decode_lp_detect(const T *obj, const T *objIdx, const T *out_tensor, int out_num,
                             std::vector<model_datatype::Object> &objects, int num_class_,
                             float prob_threshold_)
{
    int outnum_base = 7056;

    for (int anchor_idx = 0; anchor_idx < out_num; anchor_idx++)
    {
        int idds = std::round(objIdx[anchor_idx]);
        float box_objectness = obj[anchor_idx];
// std::cout << "===================== anchor_idx: " << anchor_idx
// << " box_objectness "<< box_objectness 
// << " idds " << idds
// << " objIdx[anchor_idx] " << objIdx[anchor_idx]
// << std::endl;

        
// std::cout << "========lp=============" 
// << " label: " << out_tensor[idds + outnum_base * 4]
// << " score: " << out_tensor[idds + outnum_base * 5]
// << " box_x: " << out_tensor[idds + outnum_base * 0]
// << " box_y: " << out_tensor[idds + outnum_base * 1]
// << " box_w: " << out_tensor[idds + outnum_base * 2]
// << " box_h: " << out_tensor[idds + outnum_base * 3]
// << std::endl;

        int label = -1;
        float label_conf = prob_threshold_;

        int class_id = out_tensor[idds + outnum_base * 4];

        float cls_conf = box_objectness;
        if (cls_conf > label_conf)
        {
            label = class_id;
            label_conf = cls_conf;
            
        }
        else{
            break;
        }
        if (label < 0)
        {
            continue;
        }
// std::cout << " label " << label
// << " label_conf " << label_conf
// << " max index " << out_tensor[idds + 4 * outnum_base]
// << " max score " << out_tensor[idds + 5 * outnum_base]
// <<std::endl;
    
        model_datatype::Object obj;
        obj.label_ = label;
        obj.score_ = label_conf;
    
        float x0 = out_tensor[idds];
        float y0 = out_tensor[idds + outnum_base];
        float box_w = out_tensor[idds + 2 * outnum_base];
        float box_h = out_tensor[idds + 3 * outnum_base];
    
        obj.rect_.x_ = x0 - box_w / 2.0;
        obj.rect_.y_ = y0 - box_h / 2.0;
        obj.rect_.width_ = box_w;
        obj.rect_.height_ = box_h;
    
        objects.push_back(obj);
    }
}


template <typename T>
void inline decode_lp_detect(const T *out_tensor, int out_num, std::vector<model_datatype::Object> &objects, int num_class_,
                             float prob_threshold_, int kkk)
{
    for (int anchor_idx = 0; anchor_idx < out_num; anchor_idx++)
    {
        int label = -1;
        float label_conf = prob_threshold_;

        int class_id = out_tensor[anchor_idx + 4 * out_num];

        float cls_conf = out_tensor[anchor_idx + (5 + class_id) * out_num];
        if (cls_conf > prob_threshold_)
        {
            label = class_id;
            label_conf = cls_conf;
        }
        if (label < 0)
        {
            continue;
        }
// std::cout << " label " << label
// << " label_conf " << label_conf
// << " max index " << out_tensor[anchor_idx + 4 * out_num]
// << " max score " << out_tensor[anchor_idx + 5 * out_num]
// <<std::endl;
    
        model_datatype::Object obj;
        obj.label_ = label;
        obj.score_ = label_conf;
    
        float x0 = out_tensor[anchor_idx];
        float y0 = out_tensor[anchor_idx + out_num];
        float box_w = out_tensor[anchor_idx + 2 * out_num];
        float box_h = out_tensor[anchor_idx + 3 * out_num];
    
        obj.rect_.x_ = x0 - box_w / 2.0;
        obj.rect_.y_ = y0 - box_h / 2.0;
        obj.rect_.width_ = box_w;
        obj.rect_.height_ = box_h;
    
        objects.push_back(obj);
    }
}

template <typename T>
void inline decode_yolox_box_detect(const T *out_tensor, const std::vector<int>& output_shape_list,
                                    std::vector<model_datatype::Object> &objects, int in_net_height_, int in_net_width_, int num_class_,
                                    float prob_threshold_, const std::vector<model_datatype::GridAndStride> &grid_strides)
{
    // one output of net
    int batch_size = output_shape_list[0];
    int out_height = output_shape_list[1];
    int out_width = output_shape_list[2];
    int attr_len = output_shape_list[3];

    int stride = attr_len;

    float stride_h = in_net_height_ / out_height;
    float stride_w = in_net_width_ / out_width;

    int num_anchors = grid_strides.size();

    for (int anchor_idx = 0; anchor_idx < num_anchors; anchor_idx++)
    {
        int basic_pos = anchor_idx * stride;
        float box_objectness = sigmoid(out_tensor[basic_pos + 12]);

        if (box_objectness <= prob_threshold_)
        {
            continue;
        }

        int grid0 = grid_strides[anchor_idx].grid0;
        int grid1 = grid_strides[anchor_idx].grid1;
        int s_w = grid_strides[anchor_idx].stride_w;
        int s_h = grid_strides[anchor_idx].stride_h;

        for (int class_idx = 0; class_idx < num_class_; class_idx++)
        {
            if (box_objectness > prob_threshold_)
            {
                float box_prob = box_objectness;
                if (num_class_ > 1)
                {
                    float box_cls_score = sigmoid(out_tensor[basic_pos + 13 + class_idx]);
                    box_prob = box_prob * box_cls_score;
                }
                if (box_prob > prob_threshold_)
                {
                    model_datatype::Object obj;

                    float x_center = (out_tensor[basic_pos + 0] + grid0) * s_w;
                    float y_center = (out_tensor[basic_pos + 1] + grid1) * s_h;
                    float box_w = std::exp(out_tensor[basic_pos + 2]) * s_w;
                    float box_h = std::exp(out_tensor[basic_pos + 3]) * s_h;
                    float x0 = x_center - box_w * 0.5f;
                    float y0 = y_center - box_h * 0.5f;

                    std::vector<Point2f> landmarks;
                    for (int lm_idx = 4; lm_idx < 12; lm_idx += 2)
                    {
                        float lm_x = (out_tensor[basic_pos + lm_idx] + grid0) * s_w;
                        float lm_y = (out_tensor[basic_pos + lm_idx + 1] + grid1) * s_h;

                        landmarks.emplace_back(Point2f(lm_x / in_net_width_, lm_y / in_net_height_));
                    }

                    // normalization
                    obj.rect_.x_ = x0 / in_net_width_;
                    obj.rect_.y_ = y0 / in_net_height_;
                    obj.rect_.width_ = box_w / in_net_width_;
                    obj.rect_.height_ = box_h / in_net_height_;
                    obj.landmark_ = landmarks;
                    obj.score_ = box_prob;
                    obj.label_ = class_idx;
                    objects.push_back(obj);
                }
            }
        }
    }
}

template <typename T>
void inline decode_yolox_pose_detect(const T *cls_tensor, const T *obj_tensor, const T *bbox_tensor,
                                     const T *kpt_tensor, const T *vis_tensor, int out_num,
                                     std::vector<model_datatype::Kpt_Object> &objects, int num_class_, float prob_threshold_)
{
    for (int anchor_idx = 0; anchor_idx < out_num; anchor_idx++)
    {
        int basic_pos = anchor_idx;

        float box_objectness = obj_tensor[basic_pos];

        if (box_objectness < prob_threshold_)
        {
            break;
        }

        model_datatype::Kpt_Object obj;
        for (int class_idx = 0; class_idx < num_class_; class_idx++)
        {
            if (cls_tensor[basic_pos * 2 + class_idx] > obj.label_score_)
            {
                obj.label_ = class_idx;
                obj.label_score_ = cls_tensor[basic_pos * 2 + class_idx];
            }
        }

        float x0 = bbox_tensor[basic_pos * 4];
        float y0 = bbox_tensor[basic_pos * 4 + 1];
        float box_w = bbox_tensor[basic_pos * 4 + 2];
        float box_h = bbox_tensor[basic_pos * 4 + 3];

        obj.rect_.x_ = x0;
        obj.rect_.y_ = y0;
        obj.rect_.width_ = box_w;
        obj.rect_.height_ = box_h;

        for (int kpt_idx = 0; kpt_idx < 16; kpt_idx++)
        {
            obj.veh_key_points_[kpt_idx].x = kpt_tensor[basic_pos * 32 + (kpt_idx * 2)];
            obj.veh_key_points_[kpt_idx].y = kpt_tensor[basic_pos * 32 + (kpt_idx * 2 + 1)];
            obj.veh_key_points_[kpt_idx].vis_score = vis_tensor[basic_pos * 16 + kpt_idx];
        }

        obj.score_ = box_objectness;
        objects.push_back(obj);
    }
}

// inline Rect2f
// expandConvexHull(Key_Point *keypoints, float expandRatio = 0.05)
// {
//     std::vector<cv::Point2f> visiblePoints(10);
//     int a = 0;
//     for (int i = 0; i < 16; i++)
//     {
//         if (i == 0 || i == 1 || i == 2 || i == 3 || i == 8 || i == 9 || i == 10 || i == 11 || i == 14 || i == 15)
//         {
//             visiblePoints[a].x = keypoints[i].x, visiblePoints[a].y = keypoints[i].y;
//             a++;
//         }
//     }

//     std::vector<int> hullIndices;
//     cv::convexHull(cv::Mat(visiblePoints), hullIndices, false);

//     std::vector<cv::Point2f> hullPoints;
//     for (int idx : hullIndices)
//     {
//         hullPoints.push_back(visiblePoints[idx]);
//     }

//     cv::Point2f center(0, 0);
//     for (const auto &p : hullPoints)
//     {
//         center += p;
//     }
//     center /= static_cast<float>(hullPoints.size());

//     std::vector<cv::Point2f> expandedPoints;
//     for (const auto &p : hullPoints)
//     {
//         cv::Point2f dir = p - center;
//         expandedPoints.push_back(center + dir * (1 + expandRatio));
//     }

//     float xMin = FLT_MAX, yMin = FLT_MAX;
//     float xMax = -FLT_MAX, yMax = -FLT_MAX;

//     for (const auto &p : expandedPoints)
//     {
//         xMin = std::min(xMin, p.x);
//         yMin = std::min(yMin, p.y);
//         xMax = std::max(xMax, p.x);
//         yMax = std::max(yMax, p.y);
//     }

//     return {xMin, yMin, xMax - xMin, yMax - yMin};
// }

template <typename T>
void inline decode_yolox_pose_detect(const T *obj, const T *objIdx, const T *obj_tensor, int out_num,
                                     std::vector<model_datatype::Kpt_Object> &objects, int num_class_, 
                                     float prob_threshold_,
                                     const std::vector<model_datatype::GridAndStride> &grid_strides)
{
    int outnum_base = 7056;

    for (int anchor_idx = 0; anchor_idx < out_num; anchor_idx++)
    {
        int idds = std::round(objIdx[anchor_idx]);
        float box_objectness = obj[anchor_idx];

// std::cout << "===================== anchor_idx: " << anchor_idx
// << " box_objectness "<< box_objectness 
// << " idds " << idds
// << " objIdx[anchor_idx] " << objIdx[anchor_idx]
// << std::endl;

        
// std::cout << "========qwq============= box_objectness: " << obj_tensor[idds]
// << " cls1: " << obj_tensor[idds + outnum_base * 1]
// << " cls2: " << obj_tensor[idds + outnum_base * 2]
// << " box_x: " << obj_tensor[idds + outnum_base * 3]
// << " box_x: " << obj_tensor[idds + outnum_base * 4]
// << " box_w: " << obj_tensor[idds + outnum_base * 5]
// << " box_h: " << obj_tensor[idds + outnum_base * 6]

// << " lp_vis1: " << obj_tensor[idds + outnum_base * 53]
// << " lp_vis2: " << obj_tensor[idds + outnum_base * 54]

// << std::endl;

        box_objectness = sigmoid(box_objectness);
        if (box_objectness < prob_threshold_)
        {
            // break;
            continue;
        }

        int basic_pos = idds;

        model_datatype::Kpt_Object obj;
        for (int class_idx = 0; class_idx < num_class_; class_idx++)
        {
            if (obj_tensor[basic_pos + (class_idx + 1) * outnum_base] > obj.label_score_)
            {
                obj.label_ = class_idx;
                obj.label_score_ = obj_tensor[basic_pos + (class_idx + 1) * outnum_base];
            }
        }

        obj.label_score_ = sigmoid(obj.label_score_);


        int grid0 = grid_strides[idds].grid0;
        int grid1 = grid_strides[idds].grid1;
        int s_w = grid_strides[idds].stride_w;
        int s_h = grid_strides[idds].stride_h;

// std::cout 
// << "===================== idds: " << idds
// << " grid0 " << grid0
// << " grid1 "<< grid1 
// << " s_w "<< s_w 
// << " s_h "<< s_h 
// << std::endl;


        float x_center = (obj_tensor[basic_pos + outnum_base * 3] + grid0) * s_w;
        float y_center = (obj_tensor[basic_pos + outnum_base * 4] + grid1) * s_h;
        float box_w = std::exp(obj_tensor[basic_pos + outnum_base * 5]) * s_w;
        float box_h = std::exp(obj_tensor[basic_pos + outnum_base * 6]) * s_h;
        float x0 = x_center - box_w * 0.5f;
        float y0 = y_center - box_h * 0.5f;

        obj.rect_.x_ = x0;
        obj.rect_.y_ = y0;
        obj.rect_.width_ = box_w;
        obj.rect_.height_ = box_h;


        for (int kpt_idx = 0; kpt_idx < 16; kpt_idx++)
        {
            obj.veh_key_points_[kpt_idx].x = (obj_tensor[basic_pos + outnum_base * (kpt_idx * 2 + 7) ] + grid0) * s_w;
            obj.veh_key_points_[kpt_idx].y = (obj_tensor[basic_pos + outnum_base * (kpt_idx * 2 + 1 + 7)] + grid1) * s_h;
            obj.veh_key_points_[kpt_idx].vis_score = sigmoid(obj_tensor[basic_pos + outnum_base * (kpt_idx + 39)]);
        }

        obj.score_ = box_objectness;
        objects.push_back(obj);
    }
}


template <typename T>
void inline decode_yolox_pose_detect(const T *cls_tensor, const T *obj_tensor, const T *bbox_tensor,
                                     const T *kpt_tensor, const T *vis_tensor, std::vector<model_datatype::Kpt_Object> &objects,
                                     int num_class_, float prob_threshold_,
                                     const std::vector<model_datatype::GridAndStride> &grid_strides)
{
    int num_anchors = grid_strides.size();

    for (int anchor_idx = 0; anchor_idx < num_anchors; anchor_idx++)
    {
        int basic_pos = anchor_idx;

        float box_objectness = obj_tensor[basic_pos];
        if (box_objectness < prob_threshold_)
        {
            continue;
        }

        int grid0 = grid_strides[anchor_idx].grid0;
        int grid1 = grid_strides[anchor_idx].grid1;
        int s_w = grid_strides[anchor_idx].stride_w;
        int s_h = grid_strides[anchor_idx].stride_h;

        model_datatype::Kpt_Object obj;

        for (int class_idx = 0; class_idx < num_class_; class_idx++)
        {
            if (cls_tensor[basic_pos + num_anchors * class_idx] > obj.label_score_)
            {
                obj.label_ = class_idx;
                obj.label_score_ = cls_tensor[basic_pos + num_anchors * class_idx];
            }
        }



        float x_center = (bbox_tensor[basic_pos] + grid0) * s_w;
        float y_center = (bbox_tensor[basic_pos + num_anchors] + grid1) * s_h;
        float box_w = std::exp(bbox_tensor[basic_pos + num_anchors * 2]) * s_w;
        float box_h = std::exp(bbox_tensor[basic_pos + num_anchors * 3]) * s_h;
        float x0 = x_center - box_w * 0.5f;
        float y0 = y_center - box_h * 0.5f;

        for (int kpt_idx = 0; kpt_idx < 16; kpt_idx++)
        {
            float lm_x = (kpt_tensor[basic_pos + num_anchors * kpt_idx * 2] + grid0) * s_w;
            float lm_y = (kpt_tensor[basic_pos + num_anchors * (kpt_idx * 2 + 1)] + grid1) * s_h;

            obj.veh_key_points_[kpt_idx].x = lm_x;
            obj.veh_key_points_[kpt_idx].y = lm_y;
            obj.veh_key_points_[kpt_idx].vis_score = vis_tensor[basic_pos + num_anchors * kpt_idx];
        }

        obj.rect_.x_ = x0;
        obj.rect_.y_ = y0;
        obj.rect_.width_ = box_w;
        obj.rect_.height_ = box_h;

        obj.score_ = box_objectness;
        objects.push_back(obj);
    }
}

template <typename T>
void inline decode_box_detect_decoupling(const T *out_tensor, const std::vector<int>& output_shape_list,
                                         std::vector<model_datatype::Object> &objects, int in_net_height_, int in_net_width_,
                                         int num_class_, float prob_threshold_, const std::vector<model_datatype::Anchor>& anchors_,
                                         float *src_data_p, int *src_index_p)
{
    // one output of net
    int batch_size = output_shape_list[0];
    int out_height = output_shape_list[1];
    int out_width = output_shape_list[2];
    int attr_len = output_shape_list[3];
    int stride = attr_len;

    float stride_h = in_net_height_ / out_height;
    float stride_w = in_net_width_ / out_width;

    std::vector<std::vector<float>> scaled_anchors;
    for (int j = 0; j < anchors_.size(); j++)
    {
        float scaled_height = (float)anchors_[j].h_ / stride_h;
        float scaled_width = (float)anchors_[j].w_ / stride_w;
        std::vector<float> scaled{scaled_height, scaled_width};
        scaled_anchors.emplace_back(scaled);
    }

    int obj_cnt = objects.size();
    for (int h = 0; h < out_height; h++)
    {
        for (int w = 0; w < out_width; w++)
        {
            int base_pos = (h * out_width + w) * stride;
            for (int an = 0; an < scaled_anchors.size(); an++)
            {
                float box_objectness = sigmoid(out_tensor[base_pos + 12 + an]);

                if (box_objectness > prob_threshold_)
                {
                    for (int class_idx = 0; class_idx < num_class_; class_idx++)
                    {
                        // float box_cls_score = sigmoid(out_tensor[base_pos + 5 * scaled_anchors.size() + class_idx]);
                        //  float box_prob = box_objectness * box_cls_score;
                        float box_prob = box_objectness;
                        if (box_prob > prob_threshold_) // prob_threshold_
                        {
                            model_datatype::Object obj;
                            float x_center = sigmoid(out_tensor[base_pos + 0 + 4 * an]) * 2 - 0.5 + w;
                            float y_center = sigmoid(out_tensor[base_pos + 1 + 4 * an]) * 2 - 0.5 + h;
                            float box_w =
                                std::pow(sigmoid(out_tensor[base_pos + 2 + 4 * an]) * 2, 2) * scaled_anchors[an][1];
                            float box_h =
                                std::pow(sigmoid(out_tensor[base_pos + 3 + 4 * an]) * 2, 2) * scaled_anchors[an][0];
                            float x0 = x_center - box_w * 0.5f;
                            float y0 = y_center - box_h * 0.5f;
                            std::vector<Point2f> landmarks;
                            for (int lm_idx = 0; lm_idx < 8; lm_idx += 2)
                            {
                                float lm_x = (sigmoid(out_tensor[base_pos + 5 * scaled_anchors.size() + num_class_ +
                                                                 8 * an + lm_idx]) *
                                                  8 -
                                              4) *
                                                 scaled_anchors[an][1] +
                                             w;
                                float lm_y = (sigmoid(out_tensor[base_pos + 5 * scaled_anchors.size() + num_class_ +
                                                                 8 * an + lm_idx + 1]) *
                                                  8 -
                                              4) *
                                                 scaled_anchors[an][0] +
                                             h;
                                landmarks.emplace_back(Point2f(lm_x / out_width, lm_y / out_height));
                            }

                            // normalization
                            obj.rect_.x_ = x0 / out_width;
                            obj.rect_.y_ = y0 / out_height;
                            obj.rect_.width_ = box_w / out_width;
                            obj.rect_.height_ = box_h / out_height;
                            obj.landmark_ = landmarks;
                            obj.score_ = box_prob;
                            obj.label_ = class_idx;

                            objects.push_back(obj);

                            src_index_p[obj_cnt] = objects.size() - 1;
                            src_data_p[obj_cnt] = box_prob;
                            obj_cnt++;
                        }
                    }
                }
            }
        }
    }
}

template <typename T>
void inline decode_box_detect(const T *out_tensor, const std::vector<int>& output_shape_list,
                              std::vector<model_datatype::Object> &objects, int in_net_height_, int in_net_width_, int num_class_,
                              float prob_threshold_, const std::vector<model_datatype::Anchor>& anchors_, int stride = NULL)
{
    // one output of net
    int batch_size = output_shape_list[0];
    int out_height = output_shape_list[1];
    int out_width = output_shape_list[2];
    int attr_len = output_shape_list[3];
    if (stride == 0)
    {
        stride = attr_len;
    }

    float stride_h = in_net_height_ / out_height;
    float stride_w = in_net_width_ / out_width;

    std::vector<std::vector<float>> scaled_anchors;
    for (int j = 0; j < anchors_.size(); j++)
    {
        float scaled_height = (float)anchors_[j].h_ / stride_h;
        float scaled_width = (float)anchors_[j].w_ / stride_w;
        std::vector<float> scaled{scaled_height, scaled_width};
        scaled_anchors.emplace_back(scaled);
    }
    for (int h = 0; h < out_height; h++)
    {
        for (int w = 0; w < out_width; w++)
        {
            for (int an = 0; an < scaled_anchors.size(); an++)
            {
                int base_pos = (h * out_width + w) * stride + attr_len / scaled_anchors.size() * an;
                // int base_pos = (h * out_width + w) * attr_len + attr_len / scaled_anchors.size() * an;
                float box_objectness = sigmoid(out_tensor[base_pos + 12]);

                for (int class_idx = 0; class_idx < num_class_; class_idx++)
                {
                    float box_cls_score = sigmoid(out_tensor[base_pos + 13 + class_idx]);
                    float box_prob = box_objectness * box_cls_score;

                    if (box_prob > prob_threshold_) // prob_threshold_
                    {
                        model_datatype::Object obj;
                        float x_center = sigmoid(out_tensor[base_pos + 0]) + w;
                        float y_center = sigmoid(out_tensor[base_pos + 1]) + h;
                        float box_w = std::exp(out_tensor[base_pos + 2]) * scaled_anchors[an][1];
                        float box_h = std::exp(out_tensor[base_pos + 3]) * scaled_anchors[an][0];
                        float x0 = x_center - box_w * 0.5f;
                        float y0 = y_center - box_h * 0.5f;
                        std::vector<Point2f> landmarks;
                        for (int lm_idx = 4; lm_idx < 12; lm_idx += 2)
                        {
                            float lm_x = (sigmoid(out_tensor[base_pos + lm_idx]) * 2 - 1) * scaled_anchors[an][1] + w;
                            float lm_y =
                                (sigmoid(out_tensor[base_pos + lm_idx + 1]) * 2 - 1) * scaled_anchors[an][0] + h;
                            landmarks.emplace_back(Point2f(lm_x / out_width, lm_y / out_height));
                        }

                        // normalization
                        obj.rect_.x_ = x0 / out_width;
                        obj.rect_.y_ = y0 / out_height;
                        obj.rect_.width_ = box_w / out_width;
                        obj.rect_.height_ = box_h / out_height;
                        obj.landmark_ = landmarks;
                        obj.score_ = box_prob;
                        obj.label_ = class_idx;

                        objects.push_back(obj);
                    }
                }
            }
        }
    }
}

template <typename T>
void inline nms_sorted_bboxes(const std::vector<T> &objects, std::vector<int> &picked, float nms_threshold)
{
    picked.clear();

    const int n = objects.size();

    std::vector<float> areas(n);
    for (int i = 0; i < n; i++)
    {
        areas[i] = objects[i].rect_.area();
    }

    for (int i = 0; i < n; i++)
    {
        const T &a = objects[i];

        int keep = 1;
        for (int j = 0; j < (int)picked.size(); j++)
        {
            const T &b = objects[picked[j]];

            // intersection over union
            float inter_area = intersection_area(a, b);
            float union_area = areas[i] + areas[picked[j]] - inter_area;
            // float IoU = inter_area / union_area
            if (inter_area / union_area > nms_threshold)
                keep = 0;
        }

        if (keep)
            picked.push_back(i);
    }
}

template <typename T>
void inline diou_nms_sorted_bboxes(const std::vector<T> &objects, std::vector<int> &picked, float nms_threshold)
{
    picked.clear();

    const int n = objects.size();

    std::vector<float> areas(n);
    for (int i = 0; i < n; i++)
    {
        areas[i] = objects[i].rect_.area();
    }

    for (int i = 0; i < n; i++)
    {
        const T &a = objects[i];

        int keep = 1;
        for (int j = 0; j < (int)picked.size(); j++)
        {
            const T &b = objects[picked[j]];

            // intersection over union
            float inter_area = intersection_area(a, b);
            float union_area = areas[i] + areas[picked[j]] - inter_area;
            float iou = inter_area / union_area;

            float top_x = std::min(a.rect_.x, b.rect_.x);
            float top_y = std::min(a.rect_.y, b.rect_.y);
            float bot_x = std::max(a.rect_.x + a.rect_.width, b.rect_.x + b.rect_.width);
            float bot_y = std::max(a.rect_.y + a.rect_.height, b.rect_.y + b.rect_.height);
            float a_cen_x = a.rect_.x + a.rect_.width / 2;
            float a_cen_y = a.rect_.y + a.rect_.height / 2;
            float b_cen_x = b.rect_.x + b.rect_.width / 2;
            float b_cen_y = b.rect_.y + b.rect_.height / 2;

            float d_ratio =
                std::sqrt((a_cen_x - b_cen_x) * (a_cen_x - b_cen_x) + (a_cen_y - b_cen_y) * (a_cen_y - b_cen_y)) /
                (std::sqrt((top_x - bot_x) * (top_x - bot_x) + (top_y - bot_y) * (top_y - bot_y)) + 0.00001);

            float diou = iou - d_ratio;

            if (diou > nms_threshold)
                keep = 0;
        }

        if (keep)
            picked.push_back(i);
    }
}

bool inline time_compare(unsigned long long start_time, unsigned long long end_time, int conf)
{
    if (start_time == 0 || end_time == 0 || start_time > end_time)
    {
        return false;
    }
    int time_c = (int)(end_time - start_time) / 1000;
    if (time_c > conf)
    {
        return true;
    }
    return false;
}

template <typename RectType1, typename RectType2>
float inline cal_iou(RectType1 box1, RectType2 box2, float &rat1, float &rat2)
{
    float InterWidth = 0.0;
    float InterHeight = 0.0;
    float f64InterArea = 0.0;
    float f64Box1Area = 0.0;
    float f64Box2Area = 0.0;
    float f64UnionArea = 0.0;

    InterWidth = std::min(box1.x_ + box1.width_, box2.x_ + box2.width_) - std::max(box1.x_, box2.x_);
    InterHeight = std::min(box1.y_ + box1.height_, box2.y_ + box2.height_) - std::max(box1.y_, box2.y_);

    if (InterWidth <= 0 || InterHeight <= 0)
    {
        rat1 = 0.0;
        rat2 = 0.0;
        return 0;
    }

    f64InterArea = InterWidth * InterHeight;
    f64Box1Area = box1.width_ * box1.height_;
    f64Box2Area = box2.width_ * box2.height_;
    f64UnionArea = f64Box1Area + f64Box2Area - f64InterArea;

    rat1 = f64InterArea / f64Box1Area;
    rat2 = f64InterArea / f64Box2Area;

    return f64InterArea / f64UnionArea;
}

// 横线交并比
float inline cal_line_iou(float line_1_left, float line_1_right, float line_2_left, float line_2_right,
                          float &line_1_rat, float &line_2_rat)
{

    float inner = std::fmin(line_1_right, line_2_right) - std::fmax(line_1_left, line_2_left);
    float line_1 = line_1_right - line_1_left;
    float line_2 = line_2_right - line_2_left;

    if (line_1 > 0.0 && line_2 > 0.0)
    {
        line_1_rat = inner / line_1;
        line_2_rat = inner / line_2;
        float line = line_1 + line_2 - inner;
        if (line > 0.0 && inner > 0.0)
        {
            return inner / line;
        }
        else
        {
            return 0.0;
        }
    }
    line_1_rat = 0.0;
    line_2_rat = 0.0;

    return 0.0;
}

// 判断车辆是否停下
template <typename RectType1>
bool inline is_stand(const std::vector<RectType1>& boxes)
{
    // boxes xywh 左上点+wh
    int inter = 4;
    int n = inter * 4; // 取最新的n个框
    float rat = 0.85;

    bool isstand = false;
    if (boxes.size() < n)
    {
        isstand = false;
    }
    else
    {

        float pIOU = 0.0;
        int count = 0;
        float carRat1 = 11.0;
        float carRat2 = 11.0;

        for (int i = 0; i < inter; i++)
        {
            for (int j = i + inter; j < n; j++)
            {
                pIOU = cal_iou(boxes[j], boxes[i], carRat1, carRat2);
                if (pIOU < rat)
                {
                    count++;
                }
            }
        }

        if (count < 2)
        {
            isstand = true;
        }
        else
        {
            isstand = false;
        }
    }
    return isstand;
}

// 判断可能进场车辆是否在移动
template <typename RectType1>
bool inline is_move_in(const std::vector<RectType1>& boxes)
{
    // boxes xywh 左上点+wh
    int n = 30; // 取最新的n个框

    bool is_Move = false;
    if (boxes.size() >= n)
    {
        int group_count = 10;
        int group = boxes.size() / group_count;
        int avg_width = 0;
        int avg_height = 0;

        std::vector<Point2f> avg_center_points;
        Point2f avg_point;
        avg_point.x_ = 0;
        avg_point.y_ = 0;
        for (int i = 0; i < boxes.size(); i++)
        {
            avg_width = avg_width + boxes[i].width_;
            avg_height = avg_height + boxes[i].height_;

            Point2f point;
            point.x_ = boxes[i].x_ + boxes[i].width_ / 2;
            point.y_ = boxes[i].y_ + boxes[i].height_ / 2;

            if ((i + 1) % group_count != 0)
            {
                avg_point.x_ = avg_point.x_ + point.x_;
                avg_point.y_ = avg_point.y_ + point.y_;
            }
            else
            {
                avg_point.x_ = avg_point.x_ + point.x_;
                avg_point.y_ = avg_point.y_ + point.y_;
                avg_point.x_ = avg_point.x_ / group_count;
                avg_point.y_ = avg_point.y_ / group_count;
                avg_center_points.push_back(avg_point);

                avg_point.x_ = 0;
                avg_point.y_ = 0;
            }
        }
        avg_height = avg_height / boxes.size();
        avg_width = avg_width / boxes.size();

        if (avg_height > 1 && avg_width > 1)
        {
            int move_count = 0;
            for (int i = 0; i < avg_center_points.size() - 1; i++)
            {
                for (int j = i + 1; j < avg_center_points.size(); j++)
                {
                    if (std::abs(avg_center_points[i].x_ - avg_center_points[j].x_) >= avg_width)
                    {
                        move_count++;
                    }
                    if (std::abs(avg_center_points[i].y_ - avg_center_points[j].y_) >= avg_height)
                    {
                        move_count++;
                    }
                }
            }
            if (move_count >= group)
            {
                is_Move = true;
            }
        }
    }

    return is_Move;
}

// 判断车辆相较刚出现位置是否在移动
template <typename RectType1, typename RectType2>
bool inline is_move_in_loc(const std::vector<RectType1>& boxes, const std::vector<RectType2>& veh_locations)
{
    bool is_Move = false;
    float rat = 0.4;
    int count = 7;
    if (boxes.size() > count && veh_locations.size() > 0)
    {
        RectType2 avg_box;
        RectType2 veh_first_n_loc = veh_locations[veh_locations.size() - 1];

        avg_box.x_ = 0;
        avg_box.y_ = 0;
        avg_box.width_ = 0;
        avg_box.height_ = 0;

        for (int i = 0; i < count; i++)
        {
            avg_box.x_ += boxes[i].x_;
            avg_box.y_ += boxes[i].y_;
            avg_box.width_ += boxes[i].width_;
            avg_box.height_ += boxes[i].height_;
        }
        avg_box.x_ = avg_box.x_ / count;
        avg_box.y_ = avg_box.y_ / count;
        avg_box.width_ = avg_box.width_ / count;
        avg_box.height_ = avg_box.height_ / count;

        float pIOU = 1.0;
        float carRat1 = 11.0;
        float carRat2 = 11.0;

        pIOU = cal_iou(veh_first_n_loc, avg_box, carRat1, carRat2);
        if (pIOU < rat)
        {
            is_Move = true;
        }
    }

    return is_Move;
}

// 判断可能出场车辆是否在移动
template <typename RectType1>
bool inline is_move_out(const std::vector<RectType1>& boxes, float rat, float top_gap = 50)
{
    // boxes xywh 左上点+wh
    int inter = 4;
    int n = inter * 5; // 取最新的n个框

    bool is_Move = false;
    if (boxes.size() < n)
    {
        is_Move = false;
    }
    else
    {

        float pIOU = 0.0;
        int count = 0;
        float carRat1 = 11.0;
        float carRat2 = 11.0;

        for (int i = 0; i < inter * 2; i++)
        {
            pIOU = cal_iou(boxes[i % inter], boxes[boxes.size() - 1 - (i % inter)], carRat1, carRat2);
            if (pIOU < rat || carRat1 < rat || carRat2 < rat)
            {
                count++;
            }
        }

        if (count > inter)
        {
            is_Move = true;
        }
        else
        {
            is_Move = false;
        }

        if (top_gap > 10 && is_Move && boxes[0].y_ > top_gap)
        {
            count = 0;
            for (int i = 0; i < inter * 2; i++)
            {
                int move_h = std::abs(boxes[boxes.size() - 1 - i].y_ - boxes[0].y_);
                if (move_h < top_gap / 2)
                {
                    count++;
                }
            }
            if (count == inter * 2)
            {
                is_Move = false;
            }
        }
    }

    return is_Move;
}

// 判断车辆相较停稳位置是否在移动
template <typename RectType1, typename RectType2>
bool inline is_move_standstill_loc(const std::vector<RectType1> &boxes,
                                   const std::vector<RectType2> &veh_standstill_locations, 
                                   float rat)
{
    bool is_Move = false;
    unsigned int count = 3;
    if (boxes.size() > count && !veh_standstill_locations.empty())
    {
        RectType2 avg_box;
        RectType2 veh_standstill_loc = veh_standstill_locations[veh_standstill_locations.size() - 1];

        avg_box.x_ = 0;
        avg_box.y_ = 0;
        avg_box.width_ = 0;
        avg_box.height_ = 0;

        for (unsigned int i = 0; i < count; i++)
        {
            avg_box.x_ += boxes[i].x_;
            avg_box.y_ += boxes[i].y_;
            avg_box.width_ += boxes[i].width_;
            avg_box.height_ += boxes[i].height_;
        }
        avg_box.x_ = avg_box.x_ / count;
        avg_box.y_ = avg_box.y_ / count;
        avg_box.width_ = avg_box.width_ / count;
        avg_box.height_ = avg_box.height_ / count;

        float pIOU = 1.0;
        float carRat1 = 11.0;
        float carRat2 = 11.0;

        pIOU = cal_iou(veh_standstill_loc, avg_box, carRat1, carRat2);
        if (pIOU < rat)
        {
            is_Move = true;
        }
    }

    return is_Move;
}

// 计算两字符串相似性
int inline str_distance(const std::string& source, const std::string &target)
{
    // step 1

    int n = source.length();
    int m = target.length();
    if (m == 0)
        return n;
    if (n == 0)
        return m;
    // Construct a matrix
    typedef std::vector<std::vector<int>> Tmatrix;
    Tmatrix matrix(n + 1);
    for (int i = 0; i <= n; i++)
        matrix[i].resize(m + 1);

    // step 2 Initialize

    for (int i = 1; i <= n; i++)
        matrix[i][0] = i;
    for (int i = 1; i <= m; i++)
        matrix[0][i] = i;

    // step 3
    for (int i = 1; i <= n; i++)
    {
        const char si = source[i - 1];
        // step 4
        for (int j = 1; j <= m; j++)
        {

            const char dj = target[j - 1];
            // step 5
            int cost;
            if (si == dj)
            {
                cost = 0;
            }
            else
            {
                cost = 1;
            }
            // step 6
            const int above = matrix[i - 1][j] + 1;
            const int left = matrix[i][j - 1] + 1;
            const int diag = matrix[i - 1][j - 1] + cost;
            matrix[i][j] = std::min(above, std::min(left, diag));
        }
    } // step7
    return matrix[n][m];
}

// 计算两宽字符串相似性
int inline wstr_distance(const std::wstring &source, const std::wstring& target)
{
    // step 1

    int n = source.length();
    int m = target.length();
    if (m == 0)
        return n;
    if (n == 0)
        return m;
    // Construct a matrix
    typedef std::vector<std::vector<int>> Tmatrix;
    Tmatrix matrix(n + 1);
    for (int i = 0; i <= n; i++)
        matrix[i].resize(m + 1);

    // step 2 Initialize

    for (int i = 1; i <= n; i++)
        matrix[i][0] = i;
    for (int i = 1; i <= m; i++)
        matrix[0][i] = i;

    // step 3
    for (int i = 1; i <= n; i++)
    {
        const int si = source[i - 1];
        // step 4
        for (int j = 1; j <= m; j++)
        {

            const int dj = target[j - 1];
            // step 5
            int cost;
            if (si == dj)
            {
                cost = 0;
            }
            else
            {
                cost = 1;
            }
            // step 6
            const int above = matrix[i - 1][j] + 1;
            const int left = matrix[i][j - 1] + 1;
            const int diag = matrix[i - 1][j - 1] + cost;
            matrix[i][j] = std::min(above, std::min(left, diag));
            /*int temp = 1000;
            if (left < diag) {
                    temp = left;

            }
            else {
                    temp = diag;
            }
            if (above < temp) {
                    temp = above;
            }

            matrix[i][j] = temp;*/
        }
    } // step7
    return matrix[n][m];
}

// 根据first的值降序排序
bool inline cmp_first(std::pair<float, int> a, std::pair<float, int> b)
{
    return a.first > b.first;
}

// 根据second的值降序排序
bool inline cmp_second(std::pair<float, int> a, std::pair<float, int> b)
{
    return a.second > b.second;
}

/** 计算两个直线的夹角,
 * @param method:0为角度,1为弧度
 * @return 点1向量旋转到点2向量的角度,不考虑顺逆方向的夹角最小值;0~pi之间
 */
template <typename T>
float inline get_angle_diff(Point_<T> pt1_1, Point_<T> pt1_2, Point_<T> pt2_1, Point_<T> pt2_2, int method = 0)
{
    float theta1 = atan2(pt1_1.y_ - pt1_2.y_, pt1_1.x_ - pt1_2.x_); // 返回(-pi,pi)之间的反正切弧度值
    float theta2 = atan2(pt2_1.y_ - pt2_2.y_, pt2_1.x_ - pt2_2.x_);
    float result = std::abs(theta2 - theta1) > PI ? 2 * PI - std::abs(theta2 - theta1) : std::abs(theta2 - theta1);
    if (method == 1)
    {
        return result;
    }
    return result * 180 / PI;
}

std::map<std::string, std::pair<int, float>> inline LpStrCount(const std::vector<std::pair<std::string, float>>& lp_str_vec,
                                                               int &lp_count, float &lp_confidence, std::string &lp_str)
{
    std::map<std::string, std::pair<int, float>> lp_str_count;
    std::map<std::string, std::vector<float>> str_conf_vec;

    for (int veh_idx = lp_str_vec.size() - 1; veh_idx >= 0; veh_idx--)
    {
        if (lp_str_count.find(lp_str_vec[veh_idx].first) != lp_str_count.end())
        {
            std::pair<int, float> count_conf = lp_str_count[lp_str_vec[veh_idx].first];
            float conf_sum = count_conf.second * count_conf.first + lp_str_vec[veh_idx].second;
            count_conf.first++;
            count_conf.second = conf_sum / (float)count_conf.first;
            lp_str_count[lp_str_vec[veh_idx].first] = count_conf;

            str_conf_vec[lp_str_vec[veh_idx].first].push_back(lp_str_vec[veh_idx].second);
        }
        else
        {
            lp_str_count[lp_str_vec[veh_idx].first] = std::make_pair(1, lp_str_vec[veh_idx].second);
            str_conf_vec[lp_str_vec[veh_idx].first].push_back(lp_str_vec[veh_idx].second);
        }
    }

    if (!lp_str_count.empty())
    {
        float best_score = 0;
        for (auto & it : lp_str_count)
        {
            std::pair<int, float> count_conf = it.second;
            if (static_cast<float>(count_conf.first) *  count_conf.second > best_score && count_conf.first > 3)
            {
                best_score = static_cast<float>(count_conf.first) *  count_conf.second;
                lp_str = it.first;
                lp_count = count_conf.first;
                lp_confidence = count_conf.second;
            }
        }

        if (lp_str_count.find(lp_str) != lp_str_count.end())
        {
            std::pair<int, float> count_conf = lp_str_count[lp_str];

            std::vector<float> conf_vec = str_conf_vec[lp_str];
            std::sort(conf_vec.begin(), conf_vec.end());

            int use_c = 0;
            float conf_sum = 0.0;
            for (int i = conf_vec.size() - 1; i >= 0; i--)
            {
                conf_sum = conf_sum + conf_vec[i];
                use_c++;
                if (use_c >= 5)
                {
                    break;
                }
            }
            if (use_c > 0)
            {
                lp_confidence = conf_sum / (float)use_c;
                count_conf.second = lp_confidence;
                lp_str_count[lp_str] = count_conf;
            }
        }
    }

    return lp_str_count;
}

inline void lpColorTypeConfirm(LPInfo& lp_info)
{
    std::string lp_str_now = lp_info.lp_number.lp_str;
    if (lp_str_now == "-")
    {
        lp_info.lp_number.vlp_rec_confidence = 0.0;
        return;
    }
    std::wstring lp_wstr = utf8ToUnicode(lp_str_now);
    if (lp_wstr.size() > 6)
    {
        if (lp_wstr.size() == 8)
        {
            if (lp_info.lp_color != SL_PLATE_COLOR_GREENYELLOW &&
                lp_info.lp_color != SL_PLATE_COLOR_GREENBLACK)
            {
                lp_info.lp_color = SL_PLATE_COLOR_GREENBLACK;
            }

            if (lp_info.lp_color == SL_PLATE_COLOR_GREENYELLOW)
            {
                lp_info.lp_type = SL_PLATE_NEWENERGYBIG;
            }
            else{
                lp_info.lp_type = SL_PLATE_NEWENERGY;
            }
        }

        if (lp_wstr.size() == 7)
        {
            if (lp_info.lp_color != SL_PLATE_COLOR_YELLOW)
            {
                lp_info.lp_color = SL_PLATE_COLOR_BLUE;
                lp_info.lp_type = SL_PLATE_BLUE;
            }
            else
            {
                lp_info.lp_type = SL_PLATE_YELLOW;
            }

            if (lp_str_now.find("领") != std::string::npos ||
                lp_str_now.find("使") != std::string::npos)
            {
                lp_info.lp_color = SL_PLATE_COLOR_BLACK;
                lp_info.lp_type = SL_PLATE_EMBASSY;
            }

            if (lp_str_now.find("港") != std::string::npos ||
                lp_str_now.find("澳") != std::string::npos)
            {
                lp_info.lp_type = SL_PLATE_GANGAO;
                lp_info.lp_color = SL_PLATE_COLOR_BLACK;
            }

            if (lp_str_now.find("挂") != std::string::npos)
            {
                lp_info.lp_color = SL_PLATE_COLOR_YELLOW;
                lp_info.lp_type = SL_PLATE_YELLOW;
            }

            if (lp_str_now.find("学") != std::string::npos)
            {
                lp_info.lp_color = SL_PLATE_COLOR_YELLOW;
                lp_info.lp_type = SL_PLATE_INSTRUCTIONCAR;
            }

            if (lp_str_now.find("警") != std::string::npos)
            {
                lp_info.lp_color = SL_PLATE_COLOR_WHITE;
                lp_info.lp_type = SL_PLATE_POLICE;
            }
        }
    }
}

inline Rect2f expandConvexHull(const Veh_Key_Point*  keypoints, float expandRatio = 0.15) {
	    std::vector<cv::Point2f> visiblePoints(10);
	    int a = 0;
	    for (int i = 0; i < 16; i ++)
	    {
	        if (i == 0 || i == 1 || i == 2 || i == 3
	            || i == 8 || i == 9 || i == 10 || i == 11 || i == 14 || i == 15)
	        {
	        visiblePoints[a].x = keypoints[i].x,
	            visiblePoints[a].y = keypoints[i].y;
	            a++;
	        }
	    }

	    std::vector<int> hullIndices;
	    cv::convexHull(cv::Mat(visiblePoints), hullIndices, false);

	    std::vector<cv::Point2f> hullPoints;
	    for (int idx : hullIndices) {
	        hullPoints.push_back(visiblePoints[idx]);
	    }

	    cv::Point2f center(0, 0);
	    for (const auto& p : hullPoints) {
	        center += p;
	    }
	    center /= static_cast<float>(hullPoints.size());

	    std::vector<cv::Point2f> expandedPoints;
	    for (const auto& p : hullPoints) {
	        cv::Point2f dir = p - center;
	        expandedPoints.push_back(center + dir * (1 + expandRatio));
	    }

	    float xMin = FLT_MAX, yMin = FLT_MAX;
	    float xMax = -FLT_MAX, yMax = -FLT_MAX;

	    for (const auto& p : expandedPoints) {
	        xMin = std::min(xMin, p.x);
	        yMin = std::min(yMin, p.y);
	        xMax = std::max(xMax, p.x);
	        yMax = std::max(yMax, p.y);
	    }

	    return {xMin, yMin, xMax - xMin, yMax - yMin};
	}

} // namespace sonli


inline std::string extractFileNameWithoutExtension(const std::string& path) {
    std::string fileName = path;

    while (!fileName.empty() && (fileName.back() == '/' || fileName.back() == '\\')) {
        fileName.pop_back();
    }

    size_t sepPos = fileName.find_last_of("/\\");
    if (sepPos != std::string::npos) {
        fileName = fileName.substr(sepPos + 1);
    }

    size_t dotPos = fileName.find_last_of('.');
    if (dotPos != std::string::npos && dotPos > 0) {
        return fileName.substr(0, dotPos);
    }

    return fileName;

}



#endif 

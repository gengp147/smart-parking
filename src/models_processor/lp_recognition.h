
#ifndef om_model_test_lp_recogition_H
#define om_model_test_lp_recogition_H

#include "char_code.h"
#include "data_type.h"
#include "model_process_base.h"
#include <atomic>
#include <codecvt>
#include <regex>
#include "../image_processor/image_process.h"
#include "model_datatype.h"

#include "error.h"

#define vlp_max_width 500
#define vlp_min_width 50

class LPRecognitionConfig
{
    public:
    LPRecognitionConfig() {};
    ~LPRecognitionConfig() {};

    std::atomic<int> blank_idx_{0};
    std::atomic<float> vlpr_confi_thresh_{0.7};
    std::atomic<float> province_char_confi_thresh_{0.7};
    std::atomic<float> city_char_confi_thresh_{0.6};
    std::atomic<int> priorty_province_{0};  // 0 indicate all china, else, reference to "common/data_type.h"
    std::atomic<int> priorty_city_{0};      // 0 indicate all city in each province,

    std::atomic<int> vlp_min_width_{60};
    std::atomic<int> vlp_max_width_{500};
    std::atomic<int> vlp_max_height_{ 370 };

    std::atomic<int> vlp_net_height_{ 48 };
    std::atomic<int> vlp_net_width_{ 96 };
};

class LPRecognition : public ModelProcess
{
public:
    LPRecognition();
    ~LPRecognition() override;
    void InitLPRecognition();

    int32_t lpRecProcess(sonli::VehInfoVec &vehicles, sonli::FrameImageData * imgae_data, bool lp_qa_enable);

    bool checkPriorty(int priorty, const std::string &opt);
    bool checkConfidence(float confi);

    LPRecognitionConfig config_;

private:
    int32_t preProcess(void *data);
    int32_t inference();
    void lpColorPostprocess(sonli::LPInfo &lp_info);
    int32_t postProcess(sonli::LPNumber &lp_number_info);

    std::wregex zh_cn_lp_regix_reg = std::wregex(sonli::zh_cn_lp_regix);
    std::wregex dig_let_regix_reg = std::wregex(sonli::dig_let_regix);
    std::wregex letter_regix_reg = std::wregex(sonli::letter_regix);
    std::wregex digial_regix_reg = std::wregex(sonli::digial_regix);
    std::vector<int32_t> dims_;
    int stride_ = 0;



    std::unique_ptr<sonli::ImageProcess> proc_ptr_ = nullptr;
    ot_svp_dst_img dst_img_;
};

#endif

//om_model_test_lp_recogition_H

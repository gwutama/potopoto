#ifndef POTOPOTO_LAYERSATURATION_H
#define POTOPOTO_LAYERSATURATION_H

#include "LayerBase.h"

class LayerSaturation : public LayerBase {
public:
    LayerSaturation() {}
    ~LayerSaturation() {}

    void SetImage(const cv::UMat& in_image) override;
    void SetSaturation(float in_saturation) { saturation = in_saturation; }
    cv::UMat GetAdjustedImage() override;

protected:
    std::string GetName() override { return "Saturation"; }
    void Process() override;

private:
    float saturation;
    std::vector<cv::UMat> hsv_channels;
    std::vector<cv::UMat> hsv_channels_adjusted;
};


#endif //POTOPOTO_LAYERSATURATION_H

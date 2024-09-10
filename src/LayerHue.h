#ifndef POTOPOTO_LAYERHUE_H
#define POTOPOTO_LAYERHUE_H

#include "LayerBase.h"

class LayerHue : public LayerBase {
public:
    LayerHue() {}
    ~LayerHue() {}

    void SetImage(const cv::UMat& in_image) override;
    void SetHue(float in_hue) { hue = in_hue; }
    cv::UMat GetAdjustedImage() override;

protected:
    std::string GetName() override { return "Hue"; }
    void Process() override;

private:
    float hue;
    std::vector<cv::UMat> hsv_channels;
    std::vector<cv::UMat> hsv_channels_adjusted;
};


#endif //POTOPOTO_LAYERHUE_H

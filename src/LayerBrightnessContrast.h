#ifndef POTOPOTO_LAYERBRIGHTNESSCONTRAST_H
#define POTOPOTO_LAYERBRIGHTNESSCONTRAST_H

#include <opencv2/opencv.hpp>
#include "LayerBase.h"

class LayerBrightnessContrast : public LayerBase {
public:
    LayerBrightnessContrast();
    ~LayerBrightnessContrast() = default;

    void SetBrightness(float in_brightness);
    void SetContrast(float in_contrast);

    bool ParametersHaveChanged() override { return values_have_changed; }

    static const float DEFAULT_BRIGHTNESS;
    static const float DEFAULT_CONTRAST;

protected:
    std::string GetName() override { return "BrightnessContrast"; }
    bool Process(const cv::Rect& region) override;

private:
    float brightness;
    float contrast;
    bool values_have_changed;
};


#endif //POTOPOTO_LAYERBRIGHTNESSCONTRAST_H

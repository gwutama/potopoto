#ifndef POTOPOTO_LAYERLIGHTNESS_H
#define POTOPOTO_LAYERLIGHTNESS_H

#include <opencv2/opencv.hpp>
#include "LayerBase.h"

class LayerLightness : public LayerBase {
public:
    LayerLightness();
    ~LayerLightness() = default;

    void SetLightness(float in_lightness);

    bool ParametersHaveChanged() override { return values_have_changed; }

    static const float DEFAULT_LIGHTNESS;

protected:
    std::string GetName() override { return "Lightness"; }
    bool Process(const cv::Point& top_left, const cv::Point& bottom_right) override;

private:
    float lightness;
    bool values_have_changed;
};


#endif //POTOPOTO_LAYERLIGHTNESS_H

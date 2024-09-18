#ifndef POTOPOTO_LAYERWHITEBALANCE_H
#define POTOPOTO_LAYERWHITEBALANCE_H

#include <opencv2/opencv.hpp>
#include "LayerBase.h"

class LayerWhiteBalance : public LayerBase {
public:
    LayerWhiteBalance();
    ~LayerWhiteBalance() = default;

    bool ParametersHaveChanged() override { return values_have_changed; }

    void SetSaturationThreshold(float in_saturation_threshold);

    static const float DEFAULT_SATURATION_THRESHOLD;

private:
    std::string GetName() override { return "WhiteBalance"; }
    bool Process(const cv::Point& top_left, const cv::Point& bottom_right) override;

    void GrayworldWhiteBalance(cv::UMat& src, cv::UMat& dst, float threshold);
    void GpuWhiteBalance(cv::UMat& src, cv::UMat& dst, float threshold);

private:
    float saturation_threshold;
    bool values_have_changed;
};

#endif //POTOPOTO_LAYERWHITEBALANCE_H
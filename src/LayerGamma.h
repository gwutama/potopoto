#ifndef POTOPOTO_LAYERGAMMA_H
#define POTOPOTO_LAYERGAMMA_H

#include <opencv2/opencv.hpp>
#include "LayerBase.h"


class LayerGamma : public LayerBase {
public:
    LayerGamma();
    ~LayerGamma() = default;

    void SetGamma(float in_gamma);

    bool ParametersHaveChanged() override { return values_have_changed; }

    static const float DEFAULT_GAMMA;

protected:
    std::string GetName() override { return "Gamma"; }
    bool Process(const cv::Rect& region) override;

private:
    float gamma;
    bool values_have_changed;
    cv::UMat lookup_table; // Lookup table for gamma correction stored as UMat

    cv::UMat GenerateLookupTable(float gamma_value); // Helper to generate lookup table in UMat
};

#endif //POTOPOTO_LAYERGAMMA_H
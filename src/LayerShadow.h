#ifndef POTOPOTO_LAYERSHADOW_H
#define POTOPOTO_LAYERSHADOW_H

#include <opencv2/opencv.hpp>
#include "LayerBase.h"


class LayerShadow : public LayerBase {
public:
    LayerShadow();
    ~LayerShadow() = default;

    void SetShadow(float in_shadow);

    bool ParametersHaveChanged() override { return values_have_changed; }

    static const float DEFAULT_SHADOW;

protected:
    std::string GetName() override { return "Shadow"; }
    bool Process(const cv::Rect& region) override;

private:
    float shadow;
    bool values_have_changed;
};

#endif //POTOPOTO_LAYERSHADOW_H
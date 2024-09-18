#ifndef POTOPOTO_LAYERHIGHLIGHT_H
#define POTOPOTO_LAYERHIGHLIGHT_H

#include <opencv2/opencv.hpp>
#include "LayerBase.h"


class LayerHighlight : public LayerBase {
public:
    LayerHighlight();
    ~LayerHighlight() = default;

    void SetHighlight(float in_highlight);

    bool ParametersHaveChanged() override { return values_have_changed; }

    static const float DEFAULT_HIGHLIGHT;

protected:
    std::string GetName() override { return "Highlight"; }
    bool Process(const cv::Rect& region) override;

private:
    float highlight;
    bool values_have_changed;
};

#endif //POTOPOTO_LAYERHIGHLIGHT_H
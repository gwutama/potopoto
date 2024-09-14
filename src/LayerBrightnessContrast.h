#ifndef POTOPOTO_LAYERBRIGHTNESSCONTRAST_H
#define POTOPOTO_LAYERBRIGHTNESSCONTRAST_H

#include <imgui.h>
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
    bool Process(const ImVec2& top_left, const ImVec2& bottom_right) override;

private:
    float brightness;
    float contrast;
    bool values_have_changed;
};


#endif //POTOPOTO_LAYERBRIGHTNESSCONTRAST_H

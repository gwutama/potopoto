#ifndef POTOPOTO_LAYERHUESATURATIONVALUE_H
#define POTOPOTO_LAYERHUESATURATIONVALUE_H

#include "LayerBase.h"

class LayerHueSaturationValue : public LayerBase {
public:
    LayerHueSaturationValue();
    ~LayerHueSaturationValue() {}

    void SetHue(float in_hue) { hue = in_hue; }
    void SetSaturation(float in_saturation) { saturation = in_saturation; }
    void SetValue(float in_value) { value = in_value; }

    static const float DEFAULT_HUE;
    static const float DEFAULT_SATURATION;
    static const float DEFAULT_VALUE;

protected:
    std::string GetName() override { return "HueSaturationValue"; }
    void Process() override;

private:
    float hue;
    float saturation;
    float value;
};


#endif //POTOPOTO_LAYERHUESATURATIONVALUE_H

#ifndef POTOPOTO_LAYERHUESATURATIONVALUE_H
#define POTOPOTO_LAYERHUESATURATIONVALUE_H

#include "LayerBase.h"

class LayerHueSaturationValue : public LayerBase {
public:
    LayerHueSaturationValue();
    ~LayerHueSaturationValue() {}

    void SetHue(float in_hue);
    void SetSaturation(float in_saturation);
    void SetValue(float in_value);
    void SetHighPrecision(bool in_high_precision) { high_precision = in_high_precision; }

    bool ParametersHaveChanged() override { return values_have_changed; }

    static const float DEFAULT_HUE;
    static const float DEFAULT_SATURATION;
    static const float DEFAULT_VALUE;

protected:
    std::string GetName() override { return "HueSaturationValue"; }
    bool Process() override;

private:
    float hue;
    float saturation;
    float value;
    bool values_have_changed;
    bool high_precision;
};


#endif //POTOPOTO_LAYERHUESATURATIONVALUE_H

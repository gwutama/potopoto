#ifndef POTOPOTO_IMAGEADJUSTMENTSPARAMETERS_H
#define POTOPOTO_IMAGEADJUSTMENTSPARAMETERS_H

#include "LayerBrightnessContrast.h"
#include "LayerHueSaturationValue.h"

class ImageAdjustmentsParameters {
public:
    ImageAdjustmentsParameters() { Reset(); }

    void Reset() {
        brightness = LayerBrightnessContrast::DEFAULT_BRIGHTNESS;
        contrast = LayerBrightnessContrast::DEFAULT_CONTRAST;
        hue = LayerHueSaturationValue::DEFAULT_HUE;
        saturation = LayerHueSaturationValue::DEFAULT_SATURATION;
        value = LayerHueSaturationValue::DEFAULT_VALUE;
    }

    bool operator==(const ImageAdjustmentsParameters& other) const {
        return brightness == other.brightness &&
               contrast == other.contrast &&
               hue == other.hue &&
               saturation == other.saturation &&
               value == other.value;
    }

    float GetBrightness() const { return brightness; }
    void SetBrightness(float value) { brightness = value; }

    float GetContrast() const { return contrast; }
    void SetContrast(float value) { contrast = value; }

    float GetHue() const { return hue; }
    void SetHue(float value) { hue = value; }

    float GetSaturation() const { return saturation; }
    void SetSaturation(float value) { saturation = value; }

    float GetValue() const { return value; }
    void SetValue(float value) { this->value = value; }

private:
    float brightness;
    float contrast;
    float hue;
    float saturation;
    float value;
};

#endif //POTOPOTO_IMAGEADJUSTMENTSPARAMETERS_H

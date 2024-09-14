#ifndef POTOPOTO_ADJUSTMENTSPARAMETERS_H
#define POTOPOTO_ADJUSTMENTSPARAMETERS_H

#include "LayerBrightnessContrast.h"
#include "LayerHueSaturationValue.h"
#include "LayerCmyk.h"


class AdjustmentsParameters {
public:
    AdjustmentsParameters() { Reset(); }
    ~AdjustmentsParameters() = default;

    void Reset() {
        brightness = LayerBrightnessContrast::DEFAULT_BRIGHTNESS;
        contrast = LayerBrightnessContrast::DEFAULT_CONTRAST;

        hue = LayerHueSaturationValue::DEFAULT_HUE;
        saturation = LayerHueSaturationValue::DEFAULT_SATURATION;
        value = LayerHueSaturationValue::DEFAULT_VALUE;

        cyan = LayerCmyk::DEFAULT_CYAN;
        magenta = LayerCmyk::DEFAULT_MAGENTA;
        yellow = LayerCmyk::DEFAULT_YELLOW;
        black = LayerCmyk::DEFAULT_BLACK;
    }

    bool operator==(const AdjustmentsParameters& other) const {
        return brightness == other.brightness &&
               contrast == other.contrast &&
               hue == other.hue &&
               saturation == other.saturation &&
               value == other.value &&
               cyan == other.cyan &&
               magenta == other.magenta &&
               yellow == other.yellow &&
               black == other.black;
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

    float GetCyan() const { return cyan; }
    void SetCyan(float value) { cyan = value; }

    float GetMagenta() const { return magenta; }
    void SetMagenta(float value) { magenta = value; }

    float GetYellow() const { return yellow; }
    void SetYellow(float value) { yellow = value; }

    float GetBlack() const { return black; }
    void SetBlack(float value) { black = value; }

private:
    float brightness;
    float contrast;

    float hue;
    float saturation;
    float value;

    float cyan;
    float magenta;
    float yellow;
    float black;
};

#endif //POTOPOTO_ADJUSTMENTSPARAMETERS_H

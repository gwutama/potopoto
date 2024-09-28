#ifndef POTOPOTO_ADJUSTMENTSPARAMETERS_H
#define POTOPOTO_ADJUSTMENTSPARAMETERS_H

#include "LayerBrightnessContrast.h"
#include "LayerHueSaturationValue.h"
#include "LayerLightness.h"
#include "LayerWhiteBalance.h"
#include "LayerGamma.h"
#include "LayerShadow.h"
#include "LayerHighlight.h"
#include "LayerCmyk.h"
#include "IImageProcessingParameters.h"


class AdjustmentsParameters : public IImageProcessingParameters {
public:
    AdjustmentsParameters() { Reset(); }
    ~AdjustmentsParameters() = default;

    void Reset() override {
        brightness = LayerBrightnessContrast::DEFAULT_BRIGHTNESS;
        contrast = LayerBrightnessContrast::DEFAULT_CONTRAST;

        hue = LayerHueSaturationValue::DEFAULT_HUE;
        saturation = LayerHueSaturationValue::DEFAULT_SATURATION;
        value = LayerHueSaturationValue::DEFAULT_VALUE;

        lightness = LayerLightness::DEFAULT_LIGHTNESS;

        white_balance_saturation_threshold = LayerWhiteBalance::DEFAULT_SATURATION_THRESHOLD;

        gamma = LayerGamma::DEFAULT_GAMMA;

        shadow = LayerShadow::DEFAULT_SHADOW;
        highlight = LayerHighlight::DEFAULT_HIGHLIGHT;

        cyan = LayerCmyk::DEFAULT_CYAN;
        magenta = LayerCmyk::DEFAULT_MAGENTA;
        yellow = LayerCmyk::DEFAULT_YELLOW;
        black = LayerCmyk::DEFAULT_BLACK;
    }

    bool operator==(const IImageProcessingParameters& other) const override {
        const auto* otherParams = dynamic_cast<const AdjustmentsParameters*>(&other);
        return otherParams &&
               brightness == otherParams->brightness &&
               contrast == otherParams->contrast &&
               hue == otherParams->hue &&
               saturation == otherParams->saturation &&
               value == otherParams->value &&
               lightness == otherParams->lightness &&
               white_balance_saturation_threshold == otherParams->white_balance_saturation_threshold &&
               gamma == otherParams->gamma &&
               shadow == otherParams->shadow &&
               highlight == otherParams->highlight &&
               cyan == otherParams->cyan &&
               magenta == otherParams->magenta &&
               yellow == otherParams->yellow &&
               black == otherParams->black;
    }

    bool operator!=(const IImageProcessingParameters& other) const override {
        return !(*this == other);
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

    float GetLightness() const { return lightness; }
    void SetLightness(float value) { lightness = value; }

    float GetWhiteBalanceSaturationThreshold() const { return white_balance_saturation_threshold; }
    void SetWhiteBalanceSaturationThreshold(float value) { white_balance_saturation_threshold = value; }

    float GetGamma() const { return gamma; }
    void SetGamma(float value) { gamma = value; }

    float GetShadow() const { return shadow; }
    void SetShadow(float value) { shadow = value; }

    float GetHighlight() const { return highlight; }
    void SetHighlight(float value) { highlight = value; }

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

    float lightness;

    float white_balance_saturation_threshold;

    float gamma;

    float shadow;
    float highlight;

    float cyan;
    float magenta;
    float yellow;
    float black;
};

#endif //POTOPOTO_ADJUSTMENTSPARAMETERS_H

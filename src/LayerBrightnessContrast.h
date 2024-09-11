#ifndef POTOPOTO_LAYERBRIGHTNESSCONTRAST_H
#define POTOPOTO_LAYERBRIGHTNESSCONTRAST_H

#include "LayerBase.h"

class LayerBrightnessContrast : public LayerBase {
public:
    LayerBrightnessContrast();
    ~LayerBrightnessContrast() {}

    void SetBrightness(float in_brightness) { brightness = in_brightness; }
    void SetContrast(float in_contrast) { contrast = in_contrast; }

    static const float DEFAULT_BRIGHTNESS;
    static const float DEFAULT_CONTRAST;

protected:
    std::string GetName() override { return "BrightnessContrast"; }
    void Process() override;

private:
    float brightness;
    float contrast;
};


#endif //POTOPOTO_LAYERBRIGHTNESSCONTRAST_H

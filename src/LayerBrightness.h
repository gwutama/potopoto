#ifndef POTOPOTO_LAYERBRIGHTNESS_H
#define POTOPOTO_LAYERBRIGHTNESS_H

#include "LayerBase.h"

class LayerBrightness : public LayerBase {
public:
    LayerBrightness() {}
    ~LayerBrightness() {}

    void SetBrightness(float in_brightness) { brightness = in_brightness; }

protected:
    std::string GetName() override { return "Brightness"; }
    void Process() override;

private:
    float brightness;
};


#endif //POTOPOTO_LAYERBRIGHTNESS_H

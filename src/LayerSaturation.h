#ifndef POTOPOTO_LAYERSATURATION_H
#define POTOPOTO_LAYERSATURATION_H

#include "LayerBase.h"

class LayerSaturation : public LayerBase {
public:
    LayerSaturation() {}
    ~LayerSaturation() {}

    void SetSaturation(float in_saturation) { saturation = in_saturation; }

protected:
    std::string GetName() override { return "Saturation"; }
    void Process() override;

private:
    float saturation;
};


#endif //POTOPOTO_LAYERSATURATION_H

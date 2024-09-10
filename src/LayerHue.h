#ifndef POTOPOTO_LAYERHUE_H
#define POTOPOTO_LAYERHUE_H

#include "LayerBase.h"

class LayerHue : public LayerBase {
public:
    LayerHue() {}
    ~LayerHue() {}

    void SetHue(float in_hue) { hue = in_hue; }

protected:
    std::string GetName() override { return "Hue"; }
    void Process() override;

private:
    float hue;
};


#endif //POTOPOTO_LAYERHUE_H

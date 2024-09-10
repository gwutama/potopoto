#ifndef POTOPOTO_LAYERCONTRAST_H
#define POTOPOTO_LAYERCONTRAST_H

#include "LayerBase.h"

class LayerContrast : public LayerBase {
public:
    LayerContrast() {}
    ~LayerContrast() {}

    void SetContrast(float in_contrast) { contrast = in_contrast; }

protected:
    std::string GetName() override { return "Contrast"; }
    void Process() override;

private:
    float contrast;
};


#endif //POTOPOTO_LAYERCONTRAST_H

#ifndef POTOPOTO_IMAGEADJUSTMENTS_H
#define POTOPOTO_IMAGEADJUSTMENTS_H

#include <sigslot/signal.hpp>
#include "../ImageAdjustmentsParameters.h"

class ImageAdjustments {
public:
    ImageAdjustments() {}
    void Render();
    void Reset();

    sigslot::signal<const ImageAdjustmentsParameters&>& ParametersChanged() { return parameters_changed; }

private:
    sigslot::signal<const ImageAdjustmentsParameters&> parameters_changed;

    ImageAdjustmentsParameters adjustments_parameters;
};


#endif //POTOPOTO_IMAGEADJUSTMENTS_H

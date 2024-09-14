#ifndef POTOPOTO_IMAGEADJUSTMENTS_H
#define POTOPOTO_IMAGEADJUSTMENTS_H

#include <sigslot/signal.hpp>
#include "../AdjustmentsParameters.h"

class ImageAdjustments {
public:
    ImageAdjustments() = default;
    ~ImageAdjustments() = default;

    void Render();
    void Reset();

    sigslot::signal<const AdjustmentsParameters&>& ParametersChanged() { return parameters_changed; }

private:
    sigslot::signal<const AdjustmentsParameters&> parameters_changed;

    AdjustmentsParameters adjustments_parameters;
};


#endif //POTOPOTO_IMAGEADJUSTMENTS_H

#ifndef POTOPOTO_IIMAGEPROCESSINGPANEL_H
#define POTOPOTO_IIMAGEPROCESSINGPANEL_H

#include "../IImageProcessingParameters.h"
#include <memory>

class IImageProcessingPanel {
public:
    virtual void Reset() = 0;
    virtual std::shared_ptr<IImageProcessingParameters> GetImageProcessingParametersFromUiControls() const = 0;
};

#endif //POTOPOTO_IIMAGEPROCESSINGPANEL_H

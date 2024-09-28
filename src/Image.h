#ifndef POTOPOTO_IMAGE_H
#define POTOPOTO_IMAGE_H

#include <string>
#include <map>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <opencv2/opencv.hpp>

#include "AdjustmentsParameters.h"
#include "LayerBrightnessContrast.h"
#include "LayerHueSaturationValue.h"
#include "LayerLightness.h"
#include "LayerWhiteBalance.h"
#include "LayerGamma.h"
#include "LayerShadow.h"
#include "LayerHighlight.h"
#include "LayerCmyk.h"


class Image {
public:
    Image(const std::shared_ptr<cv::UMat>& in_image);
    ~Image();

    int GetWidth() const { return adjusted_image->cols; }
    int GetHeight() const { return adjusted_image->rows; }

    std::unordered_map<std::string, std::string> GetImageInfo() const { return image_info; }

    void AdjustParameters(std::shared_ptr<AdjustmentsParameters> parameters_in);

    virtual bool ApplyAdjustments();
    virtual bool ApplyAdjustmentsRegion(const cv::Rect& region);

    std::shared_ptr<cv::UMat> GetAdjustedImage() const { return adjusted_image; }

    std::shared_ptr<Image> Clone() const;

    std::chrono::time_point<std::chrono::system_clock> GetLastAdjustmentTime() const { return last_adjustment_time; }

protected:
    virtual void UpdateImageInfo();

protected:
    std::shared_ptr<cv::UMat> original_image;
    std::shared_ptr<cv::UMat> adjusted_image;
    std::unordered_map<std::string, std::string> image_info;

    std::shared_ptr<AdjustmentsParameters> parameters;
    bool parameters_changed;

    std::shared_ptr<LayerBrightnessContrast> brightness_contrast_adjustments_layer;
    std::shared_ptr<LayerHueSaturationValue> hsv_adjustments_layer;
    std::shared_ptr<LayerLightness> lightness_adjustments_layer;
    std::shared_ptr<LayerWhiteBalance> white_balance_adjustments_layer;
    std::shared_ptr<LayerGamma> gamma_adjustments_layer;
    std::shared_ptr<LayerShadow> shadow_adjustments_layer;
    std::shared_ptr<LayerHighlight> highlight_adjustments_layer;
    std::shared_ptr<LayerCmyk> cmyk_adjustments_layer;

    // adjustment timestamp
    std::chrono::time_point<std::chrono::system_clock> last_adjustment_time;
};

#endif //POTOPOTO_IMAGE_H
#ifndef POTOPOTO_IMAGE_H
#define POTOPOTO_IMAGE_H

#include <opencv2/opencv.hpp>
#include <string>
#include <map>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <imgui.h>

#include "AdjustmentsParameters.h"
#include "LayerBrightnessContrast.h"
#include "LayerHueSaturationValue.h"
#include "LayerCmyk.h"


class Image {
public:
    Image(const std::shared_ptr<cv::UMat>& in_image);
    ~Image();

    void LoadToTexture(GLuint& texture);

    int GetWidth() const { return adjusted_image->cols; }
    int GetHeight() const { return adjusted_image->rows; }

    std::unordered_map<std::string, std::string> GetImageInfo() const { return image_info; }

    void AdjustBrightness(float value);
    void AdjustContrast(float value);

    void AdjustHue(float value);
    void AdjustSaturation(float value);
    void AdjustValue(float value);

    void AdjustCyan(float value);
    void AdjustMagenta(float value);
    void AdjustYellow(float value);
    void AdjustBlack(float value);

    void AdjustParameters(const AdjustmentsParameters& parameters);

    virtual bool ApplyAdjustments();
    virtual bool ApplyAdjustmentsRegion(const ImVec2& top_left, const ImVec2& bottom_right);

protected:
    void UpdateImageInfo();

protected:
    std::shared_ptr<cv::UMat> original_image;
    std::shared_ptr<cv::UMat> adjusted_image;
    std::unordered_map<std::string, std::string> image_info;

    LayerBrightnessContrast brightness_contrast_adjustments_layer;
    LayerHueSaturationValue hsv_adjustments_layer;
    LayerCmyk cmyk_adjustments_layer;
};

#endif //POTOPOTO_IMAGE_H
#include "Image.h"
#include <opencv2/imgcodecs.hpp>
#include <sstream>


Image::Image(const std::shared_ptr<cv::UMat>& in_image) {
    original_image = in_image;
    adjusted_image = std::make_shared<cv::UMat>(original_image->clone());

    UpdateImageInfo();
}


Image::~Image() {
    original_image->release();
    adjusted_image->release();
}


void Image::LoadToTexture(GLuint& texture) {
    if (texture) {
        glDeleteTextures(1, &texture);
    }

    // Download the data from UMat to Mat
    cv::Mat imageMat = adjusted_image->getMat(cv::ACCESS_READ);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageMat.cols, imageMat.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageMat.data);
    glBindTexture(GL_TEXTURE_2D, 0);
}


void Image::AdjustBrightness(float value) {
    brightness_contrast_adjustments_layer.SetBrightness(value);
}


void Image::AdjustContrast(float value) {
    brightness_contrast_adjustments_layer.SetContrast(value);
}


void Image::AdjustHue(float value) {
    hsv_adjustments_layer.SetHue(value);
}


void Image::AdjustSaturation(float value) {
    hsv_adjustments_layer.SetSaturation(value);
}


void Image::AdjustValue(float value) {
    hsv_adjustments_layer.SetValue(value);
}


void Image::AdjustCyan(float value) {
    cmyk_adjustments_layer.SetCyan(value);
}


void Image::AdjustMagenta(float value) {
    cmyk_adjustments_layer.SetMagenta(value);
}


void Image::AdjustYellow(float value) {
    cmyk_adjustments_layer.SetYellow(value);
}


void Image::AdjustBlack(float value) {
    cmyk_adjustments_layer.SetBlack(value);
}


void Image::AdjustParameters(const AdjustmentsParameters &parameters) {
    AdjustBrightness(parameters.GetBrightness());
    AdjustContrast(parameters.GetContrast());
    AdjustHue(parameters.GetHue());
    AdjustSaturation(parameters.GetSaturation());
    AdjustValue(parameters.GetValue());
    AdjustCyan(parameters.GetCyan());
    AdjustMagenta(parameters.GetMagenta());
    AdjustYellow(parameters.GetYellow());
    AdjustBlack(parameters.GetBlack());
}


bool Image::ApplyAdjustments() {
    if (!brightness_contrast_adjustments_layer.ParametersHaveChanged() &&
        !hsv_adjustments_layer.ParametersHaveChanged() &&
        !cmyk_adjustments_layer.ParametersHaveChanged()) {
        return false;
    }

    bool image_changed = false;

    adjusted_image = std::make_shared<cv::UMat>(original_image->clone());
    brightness_contrast_adjustments_layer.SetImage(adjusted_image);
    image_changed = brightness_contrast_adjustments_layer.Apply() || image_changed;

    hsv_adjustments_layer.SetImage(adjusted_image);
    image_changed = hsv_adjustments_layer.Apply() || image_changed;

    cmyk_adjustments_layer.SetImage(adjusted_image);
    image_changed = cmyk_adjustments_layer.Apply() || image_changed;

    return image_changed;
}


bool Image::ApplyAdjustmentsRegion(const ImVec2 &top_left, const ImVec2 &bottom_right) {
    if (!brightness_contrast_adjustments_layer.ParametersHaveChanged() &&
        !hsv_adjustments_layer.ParametersHaveChanged() &&
        !cmyk_adjustments_layer.ParametersHaveChanged()) {
        return false;
    }

    bool image_changed = false;

    adjusted_image = std::make_shared<cv::UMat>(original_image->clone());
    brightness_contrast_adjustments_layer.SetImage(adjusted_image);
    image_changed = brightness_contrast_adjustments_layer.ApplyRegion(top_left, bottom_right) || image_changed;

    hsv_adjustments_layer.SetImage(adjusted_image);
    image_changed = hsv_adjustments_layer.ApplyRegion(top_left, bottom_right) || image_changed;

    cmyk_adjustments_layer.SetImage(adjusted_image);
    image_changed = cmyk_adjustments_layer.ApplyRegion(top_left, bottom_right) || image_changed;

    return image_changed;
}


void Image::UpdateImageInfo() {
    image_info.clear();

    image_info.insert(std::make_pair("Width", std::to_string(adjusted_image->cols) + " px"));
    image_info.insert(std::make_pair("Height", std::to_string(adjusted_image->rows) + " px"));
    image_info.insert(std::make_pair("Channels", std::to_string(adjusted_image->channels())));
    image_info.insert(std::make_pair("Number of Pixels", std::to_string(adjusted_image->total()) + " px"));
    image_info.insert(std::make_pair("Size", std::to_string(adjusted_image->total() * adjusted_image->elemSize()) + " b"));
}

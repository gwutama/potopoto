#include "Image.h"
#include <opencv2/imgcodecs.hpp>
#include <sstream>


Image::Image(const std::shared_ptr<cv::UMat>& in_image) {
    original_image = in_image;
    adjusted_image = std::make_shared<cv::UMat>(original_image->clone());

    brightness_contrast_adjustments_layer = std::make_shared<LayerBrightnessContrast>();
    hsv_adjustments_layer = std::make_shared<LayerHueSaturationValue>();
    cmyk_adjustments_layer = std::make_shared<LayerCmyk>();

    UpdateImageInfo();
}


Image::~Image() {
    original_image->release();
    adjusted_image->release();
}


void Image::AdjustParameters(const AdjustmentsParameters &parameters_in) {
    if (parameters_in == parameters) {
        parameters_changed = false;
        return;
    }

    brightness_contrast_adjustments_layer->SetBrightness(parameters_in.GetBrightness());
    brightness_contrast_adjustments_layer->SetContrast(parameters_in.GetContrast());

    hsv_adjustments_layer->SetHue(parameters_in.GetHue());
    hsv_adjustments_layer->SetSaturation(parameters_in.GetSaturation());
    hsv_adjustments_layer->SetValue(parameters_in.GetValue());

    cmyk_adjustments_layer->SetCyan(parameters_in.GetCyan());
    cmyk_adjustments_layer->SetMagenta(parameters_in.GetMagenta());
    cmyk_adjustments_layer->SetYellow(parameters_in.GetYellow());
    cmyk_adjustments_layer->SetBlack(parameters_in.GetBlack());

    parameters = parameters_in;
    parameters_changed = true;
}


bool Image::ApplyAdjustments() {
    return ApplyAdjustmentsRegion(cv::Point(0, 0), cv::Point(original_image->cols, original_image->rows));
}


bool Image::ApplyAdjustmentsRegion(const cv::Point& top_left, const cv::Point& bottom_right) {
    // Test whether the region is valid
    if (top_left.x < 0 || top_left.y < 0 || bottom_right.x > original_image->cols || bottom_right.y > original_image->rows) {
        return false;
    }

    // No need to run the pipeline if the parameters have not changed
    if (!parameters_changed) {
        return false;
    }

    bool image_changed = false;

    adjusted_image = std::make_shared<cv::UMat>(original_image->clone());
    brightness_contrast_adjustments_layer->SetImage(adjusted_image);
    image_changed = brightness_contrast_adjustments_layer->ApplyRegion(top_left, bottom_right) || image_changed;

    hsv_adjustments_layer->SetImage(adjusted_image);
    image_changed = hsv_adjustments_layer->ApplyRegion(top_left, bottom_right) || image_changed;

    cmyk_adjustments_layer->SetImage(adjusted_image);
    image_changed = cmyk_adjustments_layer->ApplyRegion(top_left, bottom_right) || image_changed;

    parameters_changed = false;

    last_adjustment_time = std::chrono::system_clock::now();

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


std::shared_ptr<Image> Image::Clone() const {
    auto original_image_copy = std::make_shared<cv::UMat>(original_image->clone());
    auto adjusted_image_copy = std::make_shared<cv::UMat>(adjusted_image->clone());

    auto cloned_image = std::make_shared<Image>(original_image_copy);
    cloned_image->AdjustParameters(parameters);
    cloned_image->adjusted_image = adjusted_image_copy; // instead of cloned_image->ApplyAdjustments() to avoid recalculating the adjustments
    return cloned_image;
}

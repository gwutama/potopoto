#include "Image.h"
#include <opencv2/imgcodecs.hpp>
#include <sstream>


Image::Image(const std::shared_ptr<cv::UMat>& in_image) {
    original_image = in_image;
    adjusted_image = std::make_shared<cv::UMat>(original_image->clone());

    brightness_contrast_adjustments_layer = std::make_shared<LayerBrightnessContrast>();
    hsv_adjustments_layer = std::make_shared<LayerHueSaturationValue>();
    lightness_adjustments_layer = std::make_shared<LayerLightness>();
    white_balance_adjustments_layer = std::make_shared<LayerWhiteBalance>();
    gamma_adjustments_layer = std::make_shared<LayerGamma>();
    shadow_adjustments_layer = std::make_shared<LayerShadow>();
    highlight_adjustments_layer = std::make_shared<LayerHighlight>();
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

    lightness_adjustments_layer->SetLightness(parameters_in.GetLightness());

    white_balance_adjustments_layer->SetSaturationThreshold(parameters_in.GetWhiteBalanceSaturationThreshold());

    gamma_adjustments_layer->SetGamma(parameters_in.GetGamma());

    shadow_adjustments_layer->SetShadow(parameters_in.GetShadow());
    highlight_adjustments_layer->SetHighlight(parameters_in.GetHighlight());

    cmyk_adjustments_layer->SetCyan(parameters_in.GetCyan());
    cmyk_adjustments_layer->SetMagenta(parameters_in.GetMagenta());
    cmyk_adjustments_layer->SetYellow(parameters_in.GetYellow());
    cmyk_adjustments_layer->SetBlack(parameters_in.GetBlack());

    parameters = parameters_in;
    parameters_changed = true;
}


bool Image::ApplyAdjustments() {
    return ApplyAdjustmentsRegion(cv::Rect(0, 0, adjusted_image->cols, adjusted_image->rows));
}


bool Image::ApplyAdjustmentsRegion(const cv::Rect& region) {
    // Check if the region is within the image bounds
    if (region.x < 0 || region.y < 0 || region.x + region.width > original_image->cols || region.y + region.height > original_image->rows) {
        std::cerr << "Image: Region is outside the image bounds." << std::endl;
        return false;
    }

    // No need to run the pipeline if the parameters have not changed
    if (!parameters_changed) {
        return false;
    }

    bool image_changed = false;

    // This pipeline operates on RGB color space. Input image is however, RGBA.
    // The alpha channel is ignored in this pipeline.
    // Convert the image to RGB color space
    auto rgb_image = std::make_shared<cv::UMat>();
    cv::cvtColor(*original_image, *rgb_image, cv::COLOR_BGRA2BGR);

    brightness_contrast_adjustments_layer->SetImage(rgb_image);
    image_changed = brightness_contrast_adjustments_layer->ApplyRegion(region) || image_changed;

    hsv_adjustments_layer->SetImage(rgb_image);
    image_changed = hsv_adjustments_layer->ApplyRegion(region) || image_changed;

    lightness_adjustments_layer->SetImage(rgb_image);
    image_changed = lightness_adjustments_layer->ApplyRegion(region) || image_changed;

    white_balance_adjustments_layer->SetImage(rgb_image);
    image_changed = white_balance_adjustments_layer->ApplyRegion(region) || image_changed;

    gamma_adjustments_layer->SetImage(rgb_image);
    image_changed = gamma_adjustments_layer->ApplyRegion(region) || image_changed;

    shadow_adjustments_layer->SetImage(rgb_image);
    image_changed = shadow_adjustments_layer->ApplyRegion(region) || image_changed;

    highlight_adjustments_layer->SetImage(rgb_image);
    image_changed = highlight_adjustments_layer->ApplyRegion(region) || image_changed;

    cmyk_adjustments_layer->SetImage(rgb_image);
    image_changed = cmyk_adjustments_layer->ApplyRegion(region) || image_changed;

    // Convert the image back to RGBA color space
    cv::cvtColor(*rgb_image, *adjusted_image, cv::COLOR_BGR2BGRA);

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

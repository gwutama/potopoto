#include "LayerLightness.h"
#include "ImageUtils.h"


const float LayerLightness::DEFAULT_LIGHTNESS = 0.0f;


LayerLightness::LayerLightness() :
    lightness(DEFAULT_LIGHTNESS),
    values_have_changed(false) {
}


void LayerLightness::SetLightness(float in_lightness) {
    if (lightness == in_lightness) {
        return;
    }

    lightness = in_lightness;
    values_have_changed = true;
}


bool LayerLightness::Process(const cv::Rect& region) {
    // Input image is RGB - Output image is RGB
    if (lightness == DEFAULT_LIGHTNESS) {
        return false; // No adjustment needed
    }

    // Crop the input image to the specified region before conversion
    cv::UMat cropped_rgb_image = (*image_adjusted)(region);

    // Convert the cropped RGB image to HLS (Hue, Lightness, Saturation)
    cv::UMat hls_image = ImageUtils::RgbToHls(cropped_rgb_image);

    // Split the HLS image into separate channels
    std::vector<cv::UMat> hls_channels;
    cv::split(hls_image, hls_channels);

    // Extract the region of interest from the Lightness (L) channel
    cv::UMat& lightness_roi = hls_channels[1]; // Lightness channel is at index 1

    // Adjust the lightness
    cv::add(lightness_roi, lightness, lightness_roi);
    cv::threshold(lightness_roi, lightness_roi, 255, 255, cv::THRESH_TRUNC); // Clamp to [0, 255]

    // Merge the channels back into the HLS image
    cv::merge(hls_channels, hls_image);

    // Convert the HLS image back to RGB
    cv::UMat rgb_image = ImageUtils::HlsToRgb(hls_image);

    // Copy the processed region back to the original adjusted image
    rgb_image.copyTo((*image_adjusted)(region));

    values_have_changed = false;
    return true;
}
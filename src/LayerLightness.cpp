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


bool LayerLightness::Process(const cv::Point &top_left, const cv::Point &bottom_right) {
    // Input image is RGBA - Output image is RGBA
    if (lightness == DEFAULT_LIGHTNESS) {
        return false; // No adjustment needed
    }

    // Convert RGBA image to HLS (Hue, Lightness, Saturation)
    cv::UMat hls_image = ImageUtils::RgbaToHls(*image_adjusted);

    // Split the HLS image into separate channels
    std::vector<cv::UMat> hls_channels;
    cv::split(hls_image, hls_channels);

    // Define region of interest (ROI) based on top-left and bottom-right coordinates
    int x_start = std::clamp(top_left.x, 0, image_adjusted->cols);
    int y_start = std::clamp(top_left.y, 0, image_adjusted->rows);
    int x_end = std::clamp(bottom_right.x, 0, image_adjusted->cols);
    int y_end = std::clamp(bottom_right.y, 0, image_adjusted->rows);

    cv::Rect region_of_interest(x_start, y_start, x_end - x_start, y_end - y_start);

    // Extract the region of interest from the Lightness (L) channel
    cv::UMat lightness_roi = hls_channels[1](region_of_interest); // Lightness channel is at index 1

    // Adjust the lightness
    cv::add(lightness_roi, lightness, lightness_roi);
    cv::threshold(lightness_roi, lightness_roi, 255, 255, cv::THRESH_TRUNC); // Clamp to [0, 255]

    // Merge the channels back into the HLS image
    cv::merge(hls_channels, hls_image);

    // Convert the HLS image back to RGBA
    cv::UMat rgba_image = ImageUtils::HlsToRgba(hls_image);

    // Copy the processed region back to the original adjusted image
    rgba_image(region_of_interest).copyTo((*image_adjusted)(region_of_interest));

    values_have_changed = false;
    return true;
}
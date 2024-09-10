#include "LayerHue.h"

void LayerHue::SetImage(const cv::UMat &in_image) {
    LayerBase::SetImage(in_image);

    // Split the HSV image into separate channels
    cv::split(image, hsv_channels);
    hsv_channels_adjusted = hsv_channels;
}


void LayerHue::Process() {
    // Input image is HSV - Output image is HSV
    if (hue == 0.0f) {
        return;
    }

    // Adjust the hue channel
    hsv_channels[0].convertTo(hsv_channels_adjusted[0], CV_32F); // Convert to float for precise adjustment

    // Add the hue adjustment value
    cv::add(hsv_channels_adjusted[0], hue, hsv_channels_adjusted[0]);

    // Correct hue values to be in range [0, 179] using modulo operation
    cv::UMat mod_180;
    cv::threshold(hsv_channels_adjusted[0], mod_180, 179, 0, cv::THRESH_TRUNC); // Threshold values to max of 179

    // Convert hue back to 8-bit
    hsv_channels_adjusted[0].convertTo(hsv_channels_adjusted[0], CV_8U);
}


cv::UMat LayerHue::GetAdjustedImage() {
    if (hue == 0.0f) {
        return image;
    }

    // Merge the channels back into the final HSV image
    cv::merge(hsv_channels_adjusted, adjusted_image);

    return adjusted_image;
}
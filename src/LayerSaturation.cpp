#include "LayerSaturation.h"

void LayerSaturation::SetImage(const cv::UMat &in_image) {
    LayerBase::SetImage(in_image);

    // Split the HSV image into separate channels
    cv::split(image, hsv_channels);
    hsv_channels_adjusted = hsv_channels;
}


void LayerSaturation::Process() {
    // Input image is HSV - Output image is HSV
    if (saturation == 0.0f) {
        return;
    }

    // Adjust the saturation channel
    hsv_channels[1].convertTo(hsv_channels_adjusted[1], CV_32F); // Convert to float for precise adjustment

    cv::add(hsv_channels_adjusted[1], saturation, hsv_channels_adjusted[1]);

    // Correct hue values to be in range [0, 255] using modulo operation
    cv::UMat mod_180;
    cv::threshold(hsv_channels_adjusted[1], mod_180, 255, 0, cv::THRESH_TRUNC); // Threshold values to max of 255

    hsv_channels_adjusted[1].convertTo(hsv_channels_adjusted[1], CV_8U); // Convert back to 8-bit
}


cv::UMat LayerSaturation::GetAdjustedImage() {
    if (saturation == 0.0f) {
        return image;
    }

    // Merge the channels back into the final HSV image
    cv::merge(hsv_channels_adjusted, adjusted_image);

    return adjusted_image;
}
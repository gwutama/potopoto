#include "LayerSaturation.h"

void LayerSaturation::Process() {
    // Input image is HSV - Output image is HSV
    if (saturation == 1.0f) {
        return;
    }

    // Split the HSV image into separate channels
    std::vector<cv::Mat> hsv_channels;
    cv::split(image, hsv_channels);

    // Adjust the saturation channel
    hsv_channels[1].convertTo(hsv_channels[1], CV_32F); // Convert to float for scaling
    hsv_channels[1] *= saturation; // Apply saturation scale

    // Clamp values to the range [0, 255]
    cv::threshold(hsv_channels[1], hsv_channels[1], 255, 255, cv::THRESH_TRUNC); // Max saturation is 255
    cv::threshold(hsv_channels[1], hsv_channels[1], 0, 0, cv::THRESH_TOZERO); // Min saturation is 0
    hsv_channels[1].convertTo(hsv_channels[1], CV_8U); // Convert back to 8-bit

    // Merge the channels back
    cv::merge(hsv_channels, adjusted_image);
}
#include "LayerHue.h"

void LayerHue::Process() {
    // Input image is HSV - Output image is HSV
    if (hue == 0.0f) {
        return;
    }

    // Split the HSV image into separate channels
    std::vector<cv::Mat> hsv_channels;
    cv::split(image, hsv_channels);

    // Adjust the hue channel
    hsv_channels[0].convertTo(hsv_channels[0], CV_32F); // Convert to float for adjustment
    hsv_channels[0] += hue; // Adjust the hue by the given value

    // Ensure hue values wrap around [0, 179]
    cv::threshold(hsv_channels[0], hsv_channels[0], 179, 0, cv::THRESH_TOZERO_INV);
    cv::threshold(hsv_channels[0], hsv_channels[0], 0, 179, cv::THRESH_TOZERO);

    // Convert hue back to 8-bit
    hsv_channels[0].convertTo(hsv_channels[0], CV_8U);

    // Merge the channels back
    cv::merge(hsv_channels, adjusted_image);
}
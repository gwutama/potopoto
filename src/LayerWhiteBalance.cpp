#include "LayerWhiteBalance.h"
#include <opencv2/xphoto/white_balance.hpp>

// Default saturation threshold for Grayworld white balance
const float LayerWhiteBalance::DEFAULT_SATURATION_THRESHOLD = 0.0f;


LayerWhiteBalance::LayerWhiteBalance() : saturation_threshold(DEFAULT_SATURATION_THRESHOLD), values_have_changed(false) {
}


void LayerWhiteBalance::SetSaturationThreshold(float in_saturation_threshold) {
    if (saturation_threshold == in_saturation_threshold) {
        return;
    }

    saturation_threshold = in_saturation_threshold;
    values_have_changed = true;
}


bool LayerWhiteBalance::Process(const cv::Rect& region) {
    // Input image is RGB - Output image is RGB
    if (saturation_threshold == DEFAULT_SATURATION_THRESHOLD) {
        return false;
    }

    // Crop the input image to the ROI (RGB format)
    cv::UMat cropped_rgb_image = (*image_adjusted)(region);

    // Apply the white balance adjustment
    cv::UMat balanced_rgb_image;
    GpuWhiteBalance(cropped_rgb_image, balanced_rgb_image, saturation_threshold);

    // Replace the adjusted region with the white-balanced result
    balanced_rgb_image.copyTo((*image_adjusted)(region));

    values_have_changed = false;
    return true;
}


void LayerWhiteBalance::GrayworldWhiteBalance(cv::UMat& src, cv::UMat& dst, float threshold) {
    // Input is RGB - Output is RGB
    cv::Ptr<cv::xphoto::GrayworldWB> wb = cv::xphoto::createGrayworldWB();
    wb->setSaturationThreshold(threshold);  // Set threshold for saturation

    // Apply the white balance adjustment
    wb->balanceWhite(src, dst);
}


void LayerWhiteBalance::GpuWhiteBalance(cv::UMat& src, cv::UMat& dst, float threshold) {
    // Split the image into RGB channels
    std::vector<cv::UMat> channels(3);
    cv::split(src, channels); // Split into RGB channels

    // Convert to HSV to calculate saturation, we reuse this conversion for the mask
    cv::UMat hsv;
    cv::cvtColor(src, hsv, cv::COLOR_BGR2HSV);

    std::vector<cv::UMat> hsv_channels(3);
    cv::split(hsv, hsv_channels); // Split HSV channels (we only need the Saturation channel)

    // Create mask based on the saturation threshold
    cv::UMat mask;
    cv::threshold(hsv_channels[1], mask, threshold * 255.0f, 255, cv::THRESH_BINARY_INV); // Mask for pixels below threshold

    // Compute the mean only on the unmasked regions (low-saturation pixels)
    cv::Scalar meanR = cv::mean(channels[2], mask); // Red channel mean
    cv::Scalar meanG = cv::mean(channels[1], mask); // Green channel mean
    cv::Scalar meanB = cv::mean(channels[0], mask); // Blue channel mean

    // Avoid division by zero if the image lacks sufficient variation
    float scaleR = (meanR[0] > 0) ? (meanG[0] / meanR[0]) : 1.0f;
    float scaleB = (meanB[0] > 0) ? (meanG[0] / meanB[0]) : 1.0f;

    // Ensure scaling factors are in a reasonable range to avoid oversaturation or underexposure
    scaleR = std::clamp(scaleR, 0.5f, 2.0f);
    scaleB = std::clamp(scaleB, 0.5f, 2.0f);

    // Scale the Red and Blue channels based on the calculated factors
    cv::multiply(channels[2], scaleR, channels[2]); // Adjust Red channel
    cv::multiply(channels[0], scaleB, channels[0]); // Adjust Blue channel

    // Merge the channels back into the output image (dst)
    cv::merge(channels, dst);
}
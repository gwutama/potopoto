#include "LayerShadow.h"
#include "ImageUtils.h"

const float LayerShadow::DEFAULT_SHADOW = 0.0f;


LayerShadow::LayerShadow() :
        shadow(DEFAULT_SHADOW),
        values_have_changed(false) {
}


void LayerShadow::SetShadow(float in_shadow) {
    if (shadow == in_shadow) {
        return;
    }

    shadow = in_shadow;
    values_have_changed = true;
}


bool LayerShadow::Process(const cv::Point &top_left, const cv::Point &bottom_right) {
    // Input image is RGBA - Output image is RGBA
    if (shadow == DEFAULT_SHADOW) {
        return false; // No adjustment needed
    }

    // Convert RGBA image to LAB (Luminance, A, B) to adjust shadow regions
    cv::UMat lab_image = ImageUtils::RgbaToLab(*image_adjusted);

    // Split the LAB image into separate channels
    std::vector<cv::UMat> lab_channels;
    cv::split(lab_image, lab_channels);

    // Define region of interest (ROI) based on top-left and bottom-right coordinates
    int x_start = std::clamp(top_left.x, 0, image_adjusted->cols);
    int y_start = std::clamp(top_left.y, 0, image_adjusted->rows);
    int x_end = std::clamp(bottom_right.x, 0, image_adjusted->cols);
    int y_end = std::clamp(bottom_right.y, 0, image_adjusted->rows);

    cv::Rect region_of_interest(x_start, y_start, x_end - x_start, y_end - y_start);

    // Extract the region of interest from the L (Luminance) channel
    cv::UMat luminance_roi = lab_channels[0](region_of_interest); // Luminance (Lightness) channel

    // Use a basic threshold to create the shadow mask
    cv::UMat shadow_mask;
    cv::threshold(luminance_roi, shadow_mask, 128, 255, cv::THRESH_BINARY_INV); // Mask for darker regions

    // Soften the mask by applying a Gaussian blur (faster than bilateral filtering)
    cv::UMat smoothed_shadow_mask;
    cv::GaussianBlur(shadow_mask, smoothed_shadow_mask, cv::Size(15, 15), 5, 5);

    // Create a UMat filled with the shadow value
    cv::UMat shadow_value_mat = cv::UMat::ones(luminance_roi.size(), luminance_roi.type());
    cv::multiply(shadow_value_mat, std::abs(shadow), shadow_value_mat); // Multiply with the absolute shadow value

    // Adjust luminance based on whether shadow is positive (lighten) or negative (darken)
    if (shadow > 0) {
        cv::add(luminance_roi, shadow_value_mat, luminance_roi, smoothed_shadow_mask);
    } else {
        cv::subtract(luminance_roi, shadow_value_mat, luminance_roi, smoothed_shadow_mask);
    }

    // Clamp to [0, 255] to ensure valid pixel values
    cv::threshold(luminance_roi, luminance_roi, 255, 255, cv::THRESH_TRUNC);

    // Merge the channels back into the LAB image
    cv::merge(lab_channels, lab_image);

    cv::UMat rgba_image = ImageUtils::LabToRgba(lab_image);

    rgba_image(region_of_interest).copyTo((*image_adjusted)(region_of_interest));

    values_have_changed = false;
    return true;
}
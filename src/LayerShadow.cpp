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


bool LayerShadow::Process(const cv::Rect& region) {
    // Input image is RGB - Output image is RGB
    if (shadow == DEFAULT_SHADOW) {
        return false; // No adjustment needed
    }

    // Crop the input image to the specified region before conversion
    cv::UMat cropped_rgb_image = (*image_adjusted)(region);

    // Convert the cropped RGB image to LAB (Luminance, A, B) to adjust shadow regions
    cv::UMat lab_image = ImageUtils::RgbToLab(cropped_rgb_image);

    // Split the LAB image into separate channels
    std::vector<cv::UMat> lab_channels;
    cv::split(lab_image, lab_channels);

    // Extract the region of interest from the L (Luminance) channel
    cv::UMat& luminance_roi = lab_channels[0]; // Luminance (Lightness) channel

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

    // Convert the LAB image back to RGB
    cv::UMat rgb_image = ImageUtils::LabToRgb(lab_image);

    // Copy the processed region back to the original adjusted image
    rgb_image.copyTo((*image_adjusted)(region));

    values_have_changed = false;
    return true;
}
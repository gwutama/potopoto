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

    cv::UMat cropped_rgb_image = (*image_adjusted)(region);
    cv::UMat lab_image = ImageUtils::RgbToLab(cropped_rgb_image);

    // Split the LAB image into separate channels
    std::vector<cv::UMat> lab_channels;
    cv::split(lab_image, lab_channels);

    // Extract the region of interest from the L (Luminance) channel
    cv::UMat& luminance_roi = lab_channels[0]; // Luminance (Lightness) channel

    // Use a basic threshold to create the shadow mask
    cv::UMat shadow_mask;
    // TODO: make the threshold value adjustable
    cv::threshold(luminance_roi, shadow_mask, 128, 255, cv::THRESH_BINARY_INV); // Mask for darker regions

    // Apply Gaussian blur to smooth the edges before the distance transform
    cv::UMat blurred_shadow_mask;
    cv::GaussianBlur(shadow_mask, blurred_shadow_mask, cv::Size(15, 15), 5, 5);

    // Compute the distance transform of the mask to get distances from the edges
    cv::UMat distance_transform;
    cv::distanceTransform(blurred_shadow_mask, distance_transform, cv::DIST_L2, 5);

    // Normalize the distance transform to [0, 1] for softening the mask's edges
    distance_transform.convertTo(distance_transform, CV_32F);
    cv::normalize(distance_transform, distance_transform, 0, 1.0, cv::NORM_MINMAX);

    // Scale the distance transform to a more subtle value (0.005 for less impact)
    cv::multiply(distance_transform, 2, distance_transform); // Adjust the scaling factor as needed

    // Ensure luminance_roi and distance_transform have the same type
    luminance_roi.convertTo(luminance_roi, CV_32F);

    // Create a UMat filled with the shadow value
    cv::UMat shadow_value_mat = cv::UMat::ones(luminance_roi.size(), luminance_roi.type());
    cv::multiply(shadow_value_mat, std::abs(shadow), shadow_value_mat); // Multiply with the absolute shadow value

    // Adjust luminance based on whether shadow is positive (lighten) or negative (darken)
    if (shadow > 0) {
        // Lighten the shadows
        cv::add(luminance_roi, shadow_value_mat.mul(distance_transform), luminance_roi, cv::noArray(), CV_32F);
    } else {
        // Darken the shadows
        cv::subtract(luminance_roi, shadow_value_mat.mul(distance_transform), luminance_roi, cv::noArray(), CV_32F);
    }

    // Clamp to [0, 255] to ensure valid pixel values
    cv::threshold(luminance_roi, luminance_roi, 255, 255, cv::THRESH_TRUNC);

    // Convert luminance_roi back to 8-bit for merging with other channels
    luminance_roi.convertTo(luminance_roi, CV_8U);
    cv::merge(lab_channels, lab_image);

    cv::UMat rgb_image = ImageUtils::LabToRgb(lab_image);
    rgb_image.copyTo((*image_adjusted)(region));

    values_have_changed = false;
    return true;
}
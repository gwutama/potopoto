#include "LayerHighlight.h"
#include "ImageUtils.h"

const float LayerHighlight::DEFAULT_HIGHLIGHT = 0.0f;


LayerHighlight::LayerHighlight() :
        highlight(DEFAULT_HIGHLIGHT),
        values_have_changed(false) {
}


void LayerHighlight::SetHighlight(float in_highlight) {
    if (highlight == in_highlight) {
        return;
    }

    highlight = in_highlight;
    values_have_changed = true;
}


bool LayerHighlight::Process(const cv::Rect& region) {
    // Input image is RGBA - Output image is RGBA
    if (highlight == DEFAULT_HIGHLIGHT) {
        return false; // No adjustment needed
    }

    // Crop the input image to the specified region before conversion
    cv::UMat cropped_rgba_image = (*image_adjusted)(region);

    // Convert the cropped RGBA image to LAB (Luminance, A, B) to adjust highlight regions
    cv::UMat lab_image = ImageUtils::RgbaToLab(cropped_rgba_image);

    // Split the LAB image into separate channels
    std::vector<cv::UMat> lab_channels;
    cv::split(lab_image, lab_channels);

    // Extract the region of interest from the L (Luminance) channel
    cv::UMat luminance_roi = lab_channels[0];

    // Use a basic threshold to create the highlight mask (focus on brighter regions)
    cv::UMat highlight_mask;
    cv::threshold(luminance_roi, highlight_mask, 128, 255, cv::THRESH_BINARY); // Mask for brighter regions

    // Soften the mask by applying a Gaussian blur
    cv::UMat smoothed_highlight_mask;
    cv::GaussianBlur(highlight_mask, smoothed_highlight_mask, cv::Size(15, 15), 5, 5);

    // Create a UMat filled with the highlight value
    cv::UMat highlight_value_mat = cv::UMat::ones(luminance_roi.size(), luminance_roi.type());
    cv::multiply(highlight_value_mat, std::abs(highlight), highlight_value_mat); // Multiply with the absolute highlight value

    // Adjust luminance based on whether highlight is positive (lighten) or negative (reduce brightness)
    if (highlight > 0) {
        cv::add(luminance_roi, highlight_value_mat, luminance_roi, smoothed_highlight_mask);
    } else {
        cv::subtract(luminance_roi, highlight_value_mat, luminance_roi, smoothed_highlight_mask);
    }

    // Clamp to [0, 255] to ensure valid pixel values
    cv::threshold(luminance_roi, luminance_roi, 255, 255, cv::THRESH_TRUNC);

    // Merge the channels back into the LAB image
    cv::merge(lab_channels, lab_image);

    // Convert the LAB image back to RGBA
    cv::UMat rgba_image = ImageUtils::LabToRgba(lab_image);

    // Copy the processed region back to the original adjusted image
    rgba_image.copyTo((*image_adjusted)(region));

    values_have_changed = false;
    return true;
}
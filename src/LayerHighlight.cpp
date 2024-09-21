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
    // Input image is RGB - Output image is RGB
    if (highlight == DEFAULT_HIGHLIGHT) {
        return false; // No adjustment needed
    }

    cv::UMat cropped_rgb_image = (*image_adjusted)(region);
    cv::UMat lab_image = ImageUtils::RgbToLab(cropped_rgb_image);

    // Split the LAB image into separate channels
    std::vector<cv::UMat> lab_channels;
    cv::split(lab_image, lab_channels);

    // Extract the region of interest from the L (Luminance) channel
    cv::UMat& luminance_roi = lab_channels[0];

    // Use a basic threshold to create the highlight mask (focus on brighter regions)
    cv::UMat highlight_mask;
    // TODO: make the threshold value adjustable
    cv::threshold(luminance_roi, highlight_mask, 128, 255, cv::THRESH_BINARY); // Mask for brighter regions

    // Apply Gaussian blur to smooth the edges before the distance transform
    cv::UMat blurred_highlight_mask;
    cv::GaussianBlur(highlight_mask, blurred_highlight_mask, cv::Size(15, 15), 5, 5);

    // Compute the distance transform of the blurred mask to get distances from the edges
    cv::UMat distance_transform;
    cv::distanceTransform(blurred_highlight_mask, distance_transform, cv::DIST_L2, 5);

    // Normalize the distance transform to [0, 1] for softening the mask's edges
    distance_transform.convertTo(distance_transform, CV_32F);
    cv::normalize(distance_transform, distance_transform, 0, 1.0, cv::NORM_MINMAX);

    // Scale the distance transform to control the impact (adjust the scaling factor as needed)
    cv::multiply(distance_transform, 2, distance_transform);

    // Ensure luminance_roi and distance_transform have the same type
    luminance_roi.convertTo(luminance_roi, CV_32F);

    // Create a UMat filled with the highlight value
    cv::UMat highlight_value_mat = cv::UMat::ones(luminance_roi.size(), luminance_roi.type());
    cv::multiply(highlight_value_mat, std::abs(highlight), highlight_value_mat); // Multiply with the absolute highlight value

    // Adjust luminance based on whether highlight is positive (lighten) or negative (reduce brightness)
    if (highlight > 0) {
        // Increase the brightness in highlight regions
        cv::add(luminance_roi, highlight_value_mat.mul(distance_transform), luminance_roi, cv::noArray(), CV_32F);
    } else {
        // Decrease the brightness in highlight regions
        cv::subtract(luminance_roi, highlight_value_mat.mul(distance_transform), luminance_roi, cv::noArray(), CV_32F);
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
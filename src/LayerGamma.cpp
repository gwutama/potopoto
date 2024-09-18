#include "LayerGamma.h"


// Default gamma value
const float LayerGamma::DEFAULT_GAMMA = 1.0f; // Default gamma correction (no change)


LayerGamma::LayerGamma() :
        gamma(DEFAULT_GAMMA),
        values_have_changed(false),
        lookup_table(GenerateLookupTable(DEFAULT_GAMMA)) // Initialize the lookup table with default gamma
{
}


void LayerGamma::SetGamma(float in_gamma) {
    if (gamma == in_gamma) {
        return;
    }

    gamma = in_gamma;
    values_have_changed = true;
    lookup_table = GenerateLookupTable(gamma); // Regenerate the lookup table when gamma changes
}


cv::UMat LayerGamma::GenerateLookupTable(float gamma_value) {
    // Create a lookup table for faster gamma correction (256 values for each intensity)
    cv::UMat table(1, 256, CV_8U);
    uchar* ptr = table.getMat(cv::ACCESS_WRITE).ptr();

    // Parallelize lookup table generation using OpenMP
#pragma omp parallel for
    for (int i = 0; i < 256; ++i) {
        ptr[i] = cv::saturate_cast<uchar>(pow(i / 255.0, gamma_value) * 255.0);
    }

    return table;
}


bool LayerGamma::Process(const cv::Point &top_left, const cv::Point &bottom_right) {
    // Input image is RGBA - Output image is RGBA
    if (gamma == DEFAULT_GAMMA) {
        return false; // No adjustment needed
    }

    // Define region of interest (ROI) based on top-left and bottom-right coordinates
    int x_start = std::clamp(top_left.x, 0, image_adjusted->cols);
    int y_start = std::clamp(top_left.y, 0, image_adjusted->rows);
    int x_end = std::clamp(bottom_right.x, 0, image_adjusted->cols);
    int y_end = std::clamp(bottom_right.y, 0, image_adjusted->rows);

    cv::Rect region_of_interest(x_start, y_start, x_end - x_start, y_end - y_start);

    // Extract the region of interest from the image
    cv::UMat roi_image = (*image_adjusted)(region_of_interest);

    // Parallelize the lookup table application using OpenMP (works with UMat if OpenCL is enabled)
    cv::LUT(roi_image, lookup_table, roi_image);

    // Replace the adjusted region back into the original image
    roi_image.copyTo((*image_adjusted)(region_of_interest));

    values_have_changed = false;
    return true;
}
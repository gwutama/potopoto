#include "LayerGamma.h"


const float LayerGamma::DEFAULT_GAMMA = 1.0f;


LayerGamma::LayerGamma() :
        gamma(DEFAULT_GAMMA),
        values_have_changed(false),
        lookup_table(GenerateLookupTable(DEFAULT_GAMMA))
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

#pragma omp parallel for
    for (int i = 0; i < 256; ++i) {
        ptr[i] = cv::saturate_cast<uchar>(pow(i / 255.0, gamma_value) * 255.0);
    }

    return table;
}


bool LayerGamma::Process(const cv::Rect& region) {
    // Input image is RGBA - Output image is RGBA
    if (gamma == DEFAULT_GAMMA) {
        return false; // No adjustment needed
    }

    // Extract the region of interest from the image
    cv::UMat roi_image = (*image_adjusted)(region);

    cv::LUT(roi_image, lookup_table, roi_image);

    // Replace the adjusted region back into the original image
    roi_image.copyTo((*image_adjusted)(region));

    values_have_changed = false;
    return true;
}
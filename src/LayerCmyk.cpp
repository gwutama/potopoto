#include "LayerCmyk.h"
#include "ImageUtils.h"


const float LayerCmyk::DEFAULT_CYAN = 0.0f;
const float LayerCmyk::DEFAULT_MAGENTA = 0.0f;
const float LayerCmyk::DEFAULT_YELLOW = 0.0f;
const float LayerCmyk::DEFAULT_BLACK = 0.0f;


LayerCmyk::LayerCmyk() :
    cyan(DEFAULT_CYAN),
    magenta(DEFAULT_MAGENTA),
    yellow(DEFAULT_YELLOW),
    black(DEFAULT_BLACK),
    values_have_changed(false) {
}


void LayerCmyk::SetCyan(float in_cyan) {
    if (cyan == in_cyan) {
        return;
    }

    cyan = in_cyan;
    values_have_changed = true;
}


void LayerCmyk::SetMagenta(float in_magenta) {
    if (magenta == in_magenta) {
        return;
    }

    magenta = in_magenta;
    values_have_changed = true;
}


void LayerCmyk::SetYellow(float in_yellow) {
    if (yellow == in_yellow) {
        return;
    }

    yellow = in_yellow;
    values_have_changed = true;
}


void LayerCmyk::SetBlack(float in_black) {
    if (black == in_black) {
        return;
    }

    black = in_black;
    values_have_changed = true;
}


bool LayerCmyk::Process(const cv::Rect& region) {
    // Input image is RGB - Output image is RGB
    if (cyan == DEFAULT_CYAN && magenta == DEFAULT_MAGENTA && yellow == DEFAULT_YELLOW && black == DEFAULT_BLACK) {
        return false; // No adjustment needed
    }

    // Crop the input RGB image to the specified region
    cv::UMat cropped_rgb_image = ImageUtils::CropImage(*image_adjusted, region);

    // Convert the cropped RGB image to CMYK
    cv::UMat cmyk_image = ImageUtils::RgbToCmyk(cropped_rgb_image);

    // Split the CMYK image into separate channels
    std::vector<cv::UMat> cmyk_channels;
    cv::split(cmyk_image, cmyk_channels);

    // Use OpenMP to parallelize adjustments for each channel
#pragma omp parallel sections
    {
#pragma omp section
        {
            if (cyan != DEFAULT_CYAN) {
                cv::add(cmyk_channels[0], cyan, cmyk_channels[0]);
            }
        }
#pragma omp section
        {
            if (magenta != DEFAULT_MAGENTA) {
                cv::add(cmyk_channels[1], magenta, cmyk_channels[1]);
            }
        }
#pragma omp section
        {
            if (yellow != DEFAULT_YELLOW) {
                cv::add(cmyk_channels[2], yellow, cmyk_channels[2]);
            }
        }
#pragma omp section
        {
            if (black != DEFAULT_BLACK) {
                cv::add(cmyk_channels[3], black, cmyk_channels[3]);
            }
        }
    }

    // Merge the adjusted CMYK channels back into a single image
    cv::merge(cmyk_channels, cmyk_image);

    // Convert the CMYK image back to RGB
    cv::UMat adjusted_rgb_image = ImageUtils::CmykToRgb(cmyk_image);

    // Copy the adjusted image back to the original image
    adjusted_rgb_image.copyTo((*image_adjusted)(region));

    values_have_changed = false;
    return true;
}
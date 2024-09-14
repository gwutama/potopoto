#include "LayerHueSaturationValue.h"
#include "ImageUtils.h"


const float LayerHueSaturationValue::DEFAULT_HUE = 0.0f;
const float LayerHueSaturationValue::DEFAULT_SATURATION = 0.0f;
const float LayerHueSaturationValue::DEFAULT_VALUE = 0.0f;


LayerHueSaturationValue::LayerHueSaturationValue() :
    hue(DEFAULT_HUE),
    saturation(DEFAULT_SATURATION),
    value(DEFAULT_VALUE),
    high_precision(false) {
}


void LayerHueSaturationValue::SetHue(float in_hue) {
    if (hue == in_hue) {
        return;
    }

    hue = in_hue;
    values_have_changed = true;
}


void LayerHueSaturationValue::SetSaturation(float in_saturation) {
    if (saturation == in_saturation) {
        return;
    }

    saturation = in_saturation;
    values_have_changed = true;
}


void LayerHueSaturationValue::SetValue(float in_value) {
    if (value == in_value) {
        return;
    }

    value = in_value;
    values_have_changed = true;
}


bool LayerHueSaturationValue::Process(const ImVec2& top_left, const ImVec2& bottom_right) {
    // Input image is RGBA - Output image is RGBA
    if (hue == DEFAULT_HUE && saturation == DEFAULT_SATURATION && value == DEFAULT_VALUE) {
        return false;
    }

    // Convert RGBA image to HSV
    cv::UMat hsv_image = ImageUtils::RgbaToHsv(*image_adjusted);

    // Split the HSV image into separate channels
    std::vector<cv::UMat> hsv_channels;
    cv::split(hsv_image, hsv_channels);

    // Define region of interest (ROI) based on top-left and bottom-right coordinates
    int x_start = static_cast<int>(std::clamp(top_left.x, 0.0f, static_cast<float>(image_adjusted->cols)));
    int y_start = static_cast<int>(std::clamp(top_left.y, 0.0f, static_cast<float>(image_adjusted->rows)));
    int x_end = static_cast<int>(std::clamp(bottom_right.x, 0.0f, static_cast<float>(image_adjusted->cols)));
    int y_end = static_cast<int>(std::clamp(bottom_right.y, 0.0f, static_cast<float>(image_adjusted->rows)));

    cv::Rect region_of_interest(x_start, y_start, x_end - x_start, y_end - y_start);

    // Extract the region of interest from each HSV channel
    cv::UMat hsv_roi_0 = hsv_channels[0](region_of_interest);
    cv::UMat hsv_roi_1 = hsv_channels[1](region_of_interest);
    cv::UMat hsv_roi_2 = hsv_channels[2](region_of_interest);

    // Convert channels to float if high precision adjustment is needed
    if (high_precision) {
#pragma omp parallel sections
        {
#pragma omp section
            hsv_roi_0.convertTo(hsv_roi_0, CV_32F); // Hue
#pragma omp section
            hsv_roi_1.convertTo(hsv_roi_1, CV_32F); // Saturation
#pragma omp section
            hsv_roi_2.convertTo(hsv_roi_2, CV_32F); // Value
        }
    }

    // Adjust Hue, Saturation, and Value in parallel
#pragma omp parallel sections
    {
#pragma omp section
        {
            if (hue != DEFAULT_HUE) {
                cv::add(hsv_roi_0, hue, hsv_roi_0); // Adjust hue
                cv::min(hsv_roi_0, 180, hsv_roi_0); // Clamp hue
            }
        }
#pragma omp section
        {
            if (saturation != DEFAULT_SATURATION) {
                cv::add(hsv_roi_1, saturation, hsv_roi_1); // Adjust saturation
                cv::min(hsv_roi_1, 255, hsv_roi_1); // Clamp saturation
            }
        }
#pragma omp section
        {
            if (value != DEFAULT_VALUE) {
                cv::add(hsv_roi_2, value, hsv_roi_2); // Adjust value
                cv::min(hsv_roi_2, 255, hsv_roi_2); // Clamp value
            }
        }
    }

    // Convert channels back to 8-bit, if high precision is enabled
    if (high_precision) {
#pragma omp parallel sections
        {
#pragma omp section
            hsv_roi_0.convertTo(hsv_roi_0, CV_8U);
#pragma omp section
            hsv_roi_1.convertTo(hsv_roi_1, CV_8U);
#pragma omp section
            hsv_roi_2.convertTo(hsv_roi_2, CV_8U);
        }
    }

    // Update the original HSV image with the processed ROI
    hsv_channels[0](region_of_interest) = hsv_roi_0;
    hsv_channels[1](region_of_interest) = hsv_roi_1;
    hsv_channels[2](region_of_interest) = hsv_roi_2;

    // Merge the channels back into the final HSV image
    cv::merge(hsv_channels, hsv_image);

    // Convert the HSV image back to RGBA
    cv::UMat new_image = ImageUtils::HsvToRgba(hsv_image);

    // Copy the processed image back to image_adjusted
    new_image.copyTo(*image_adjusted);

    values_have_changed = false;
    return true;
}

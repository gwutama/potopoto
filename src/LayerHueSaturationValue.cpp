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


bool LayerHueSaturationValue::Process() {
    // Input image is RGBA - Output image is RGBA
    if (hue == DEFAULT_HUE && saturation == DEFAULT_SATURATION && value == DEFAULT_VALUE) {
        return false;
    }

    cv::UMat hsv_image = ImageUtils::RgbaToHsv(*image_adjusted);

    // Split the HSV image into separate channels
    std::vector<cv::UMat> hsv_channels;
    cv::split(hsv_image, hsv_channels);

    // This is used for exporting, no need to parallelize this because the impact is minimal
    if (high_precision) {
        // Convert channels to float for precise adjustment
        hsv_channels[0].convertTo(hsv_channels[0], CV_32F); // Hue
        hsv_channels[1].convertTo(hsv_channels[1], CV_32F); // Saturation
        hsv_channels[2].convertTo(hsv_channels[2], CV_32F); // Value
    }

    if (hue != DEFAULT_HUE) {
        cv::add(hsv_channels[0], hue, hsv_channels[0]); // Adjust hue

        // Wrap hue values to be in the range [0, 180]
        cv::min(hsv_channels[0], 180, hsv_channels[0]); // Clamp hue
    }

    if (saturation != DEFAULT_SATURATION) {
        cv::add(hsv_channels[1], saturation, hsv_channels[1]); // Adjust saturation

        // Clamp value channel to the range [0, 255]
        // Not necessary because it's converted back to 8-bit later
        //cv::min(hsv_channels[1], 255, hsv_channels[1]); // Clamp saturation
    }

    if (value != DEFAULT_VALUE) {
        cv::add(hsv_channels[2], value, hsv_channels[2]); // Adjust value

        // Clamp value channel to the range [0, 255]
        // Not necessary because it's converted back to 8-bit later
        //cv::min(hsv_channels[2], 255, hsv_channels[2]); // Clamp value
    }

    // This is used for exporting, no need to parallelize this because the impact is minimal
    if (high_precision) {
        // Convert channels back to 8-bit
        hsv_channels[0].convertTo(hsv_channels[0], CV_8U);
        hsv_channels[1].convertTo(hsv_channels[1], CV_8U);
        hsv_channels[2].convertTo(hsv_channels[2], CV_8U);
    }

    // Merge the channels back into the final HSV image
    cv::merge(hsv_channels, hsv_image);

    // Convert the HSV image back to RGBA
    cv::UMat new_image = ImageUtils::HsvToRgba(hsv_image);

    // Copy the processed image back to image_adjusted
    new_image.copyTo(*image_adjusted);

    values_have_changed = false;
    return true;
}

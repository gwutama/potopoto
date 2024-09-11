#include "LayerHueSaturationValue.h"
#include "ImageUtils.h"


const float LayerHueSaturationValue::DEFAULT_HUE = 0.0f;
const float LayerHueSaturationValue::DEFAULT_SATURATION = 0.0f;
const float LayerHueSaturationValue::DEFAULT_VALUE = 0.0f;


LayerHueSaturationValue::LayerHueSaturationValue() : hue(DEFAULT_HUE), saturation(DEFAULT_SATURATION), value(DEFAULT_VALUE) {
}


void LayerHueSaturationValue::Process() {
    // Input image is RGBA - Output image is RGBA
    cv::UMat hsv_image = ImageUtils::RgbaToHsv(*image_adjusted);

    // Split the HSV image into separate channels
    std::vector<cv::UMat> hsv_channels;
    cv::split(hsv_image, hsv_channels);

    // Convert to float for precise adjustment
    hsv_channels[0].convertTo(hsv_channels[0], CV_32F); // Hue
    hsv_channels[1].convertTo(hsv_channels[1], CV_32F); // Saturation
    hsv_channels[2].convertTo(hsv_channels[2], CV_32F); // Value

    // Adjust channels using OpenCV functions
    cv::add(hsv_channels[0], hue, hsv_channels[0]); // Adjust hue
    cv::add(hsv_channels[1], saturation, hsv_channels[1]); // Adjust saturation
    cv::add(hsv_channels[2], value, hsv_channels[2]); // Adjust value

    // Wrap hue values to be in the range [0, 180]
    cv::min(hsv_channels[0], 180, hsv_channels[0]); // Clamp hue

    // Clamp saturation and value channels to the range [0, 255]
    cv::min(hsv_channels[1], 255, hsv_channels[1]); // Clamp saturation
    cv::min(hsv_channels[2], 255, hsv_channels[2]); // Clamp value

    // Convert channels back to 8-bit
    hsv_channels[0].convertTo(hsv_channels[0], CV_8U);
    hsv_channels[1].convertTo(hsv_channels[1], CV_8U);
    hsv_channels[2].convertTo(hsv_channels[2], CV_8U);

    // Merge the channels back into the final HSV image
    cv::merge(hsv_channels, hsv_image);

    // Convert the HSV image back to RGBA
    auto new_image = ImageUtils::HsvToRgba(hsv_image);

    // Copy the processed image back to image_adjusted
    new_image.copyTo(*image_adjusted);
}

#include "ImageUtils.h"
#include <iostream>


cv::UMat ImageUtils::RgbaToHsv(const cv::UMat& rgba_image) {
    // Convert RGBA to RGB color space
    cv::UMat rgb_image;
    cv::cvtColor(rgba_image, rgb_image, cv::COLOR_RGBA2RGB);

    // Convert RGB to HSV color space
    cv::UMat hsv_image;
    cv::cvtColor(rgb_image, hsv_image, cv::COLOR_RGB2HSV);

    return hsv_image;
}


cv::UMat ImageUtils::HsvToRgba(const cv::UMat& hsv_image) {
    // Convert HSV to RGB color space
    cv::UMat rgb_image;
    cv::cvtColor(hsv_image, rgb_image, cv::COLOR_HSV2RGB);

    // Convert RGB to RGBA
    cv::UMat rgba_image;
    cv::cvtColor(rgb_image, rgba_image, cv::COLOR_RGB2RGBA);

    return rgba_image;
}


cv::UMat ImageUtils::CombineHsvImages(const std::vector<cv::UMat>& hsv_images) {
    if (hsv_images.empty()) {
        std::cerr << "Error: No images provided." << std::endl;
        return cv::UMat(); // Return an empty image if no input is provided
    }

    // Check if all images have the same size and type (CV_8UC3 for HSV)
    cv::Size image_size = hsv_images[0].size();
    int image_type = hsv_images[0].type();

    for (const auto& img : hsv_images) {
        if (img.size() != image_size || img.type() != CV_8UC3) {
            std::cerr << "Error: All images must have the same dimensions and be of type CV_8UC3 (HSV format)." << std::endl;
            return cv::UMat();
        }
    }

    // Initialize accumulators for each HSV channel as CV_32F to store floating-point values
    cv::UMat hue_accumulator(image_size, CV_32F, cv::Scalar(0));
    cv::UMat saturation_accumulator(image_size, CV_32F, cv::Scalar(0));
    cv::UMat value_accumulator(image_size, CV_32F, cv::Scalar(0));

    int image_count = hsv_images.size();

    // Iterate through each image and accumulate channel values
    for (const auto& hsv_image : hsv_images) {
        std::vector<cv::UMat> channels;
        cv::split(hsv_image, channels); // Split the HSV image into its three channels (Hue, Saturation, Value)

        // Convert each channel to CV_32F before accumulation
        cv::UMat channel_hue, channel_saturation, channel_value;
        channels[0].convertTo(channel_hue, CV_32F);
        channels[1].convertTo(channel_saturation, CV_32F);
        channels[2].convertTo(channel_value, CV_32F);

        // Accumulate each channel separately
        cv::accumulate(channel_hue, hue_accumulator);
        cv::accumulate(channel_saturation, saturation_accumulator);
        cv::accumulate(channel_value, value_accumulator);
    }

    // Compute the average for each channel using cv::divide
    cv::divide(hue_accumulator, image_count, hue_accumulator);
    cv::divide(saturation_accumulator, image_count, saturation_accumulator);
    cv::divide(value_accumulator, image_count, value_accumulator);

    // Convert back to 8-bit format for merging
    hue_accumulator.convertTo(hue_accumulator, CV_8U);
    saturation_accumulator.convertTo(saturation_accumulator, CV_8U);
    value_accumulator.convertTo(value_accumulator, CV_8U);

    // Merge the averaged channels back into a single HSV image
    cv::UMat combined_hsv;
    std::vector<cv::UMat> merged_channels = {hue_accumulator, saturation_accumulator, value_accumulator};
    cv::merge(merged_channels, combined_hsv);

    return combined_hsv;
}


cv::UMat ImageUtils::CombineRgbaImages(const std::vector<cv::UMat>& rgba_images) {
    if (rgba_images.empty()) {
        std::cerr << "Error: No images provided." << std::endl;
        return cv::UMat(); // Return an empty image if no input is provided
    }

    // Check if all images have the same size and are of type CV_8UC4 (RGBA)
    cv::Size image_size = rgba_images[0].size();
    int channels = rgba_images[0].channels();

    for (const auto& img : rgba_images) {
        if (img.size() != image_size || img.channels() != 4) {
            std::cerr << "Error: All images must have the same dimensions and must be in RGBA format." << std::endl;
            return cv::UMat();
        }
    }

    // Initialize accumulators for each RGBA channel as CV_32F to store floating-point values
    cv::UMat red_accumulator(image_size, CV_32F, cv::Scalar(0));
    cv::UMat green_accumulator(image_size, CV_32F, cv::Scalar(0));
    cv::UMat blue_accumulator(image_size, CV_32F, cv::Scalar(0));
    cv::UMat alpha_accumulator(image_size, CV_32F, cv::Scalar(0));

    int image_count = rgba_images.size();

    // Iterate through each image and accumulate channel values
    for (const auto& rgba_image : rgba_images) {
        std::vector<cv::UMat> channels;
        cv::split(rgba_image, channels); // Split the RGBA image into its four channels (Red, Green, Blue, Alpha)

        // Convert each channel to CV_32F before accumulation
        cv::UMat channel_red, channel_green, channel_blue, channel_alpha;
        channels[0].convertTo(channel_red, CV_32F);
        channels[1].convertTo(channel_green, CV_32F);
        channels[2].convertTo(channel_blue, CV_32F);
        channels[3].convertTo(channel_alpha, CV_32F);

        // Accumulate each channel separately
        cv::accumulate(channel_red, red_accumulator);
        cv::accumulate(channel_green, green_accumulator);
        cv::accumulate(channel_blue, blue_accumulator);
        cv::accumulate(channel_alpha, alpha_accumulator);
    }

    // Compute the average for each channel using cv::divide
    cv::divide(red_accumulator, image_count, red_accumulator);
    cv::divide(green_accumulator, image_count, green_accumulator);
    cv::divide(blue_accumulator, image_count, blue_accumulator);
    cv::divide(alpha_accumulator, image_count, alpha_accumulator);

    // Convert back to 8-bit format for merging
    red_accumulator.convertTo(red_accumulator, CV_8U);
    green_accumulator.convertTo(green_accumulator, CV_8U);
    blue_accumulator.convertTo(blue_accumulator, CV_8U);
    alpha_accumulator.convertTo(alpha_accumulator, CV_8U);

    // Merge the averaged channels back into a single RGBA image
    cv::UMat combined_rgba;
    std::vector<cv::UMat> merged_channels = {red_accumulator, green_accumulator, blue_accumulator, alpha_accumulator};
    cv::merge(merged_channels, combined_rgba);

    return combined_rgba;
}

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


cv::UMat ImageUtils::RgbaToCmyk(const cv::UMat& rgba_image) {
    // Convert RGBA to RGB color space
    cv::UMat rgb_image;
    cv::cvtColor(rgba_image, rgb_image, cv::COLOR_RGBA2RGB);

    // Prepare the CMYK output image
    cv::UMat cmyk_image(rgb_image.size(), CV_32FC4); // CMYK has 4 channels (C, M, Y, K) in floating point

    // Create intermediate matrices to store the converted channels
    std::vector<cv::UMat> rgb_channels(3), cmyk_channels(4);
    cv::split(rgb_image, rgb_channels);

    // Normalize RGB channels to [0, 1]
    cv::UMat ones = cv::UMat::ones(rgb_channels[0].size(), CV_32F); // Matrix filled with 1.0

#pragma omp parallel sections
    {
#pragma omp section
        {
            rgb_channels[0].convertTo(rgb_channels[0], CV_32F, 1.0 / 255.0);
            cv::subtract(ones, rgb_channels[0], cmyk_channels[0]); // C = 1 - R
        }
#pragma omp section
        {
            rgb_channels[1].convertTo(rgb_channels[1], CV_32F, 1.0 / 255.0);
            cv::subtract(ones, rgb_channels[1], cmyk_channels[1]); // M = 1 - G
        }
#pragma omp section
        {
            rgb_channels[2].convertTo(rgb_channels[2], CV_32F, 1.0 / 255.0);
            cv::subtract(ones, rgb_channels[2], cmyk_channels[2]); // Y = 1 - B
        }
    }

    // Calculate K channel
    cv::min(cmyk_channels[0], cmyk_channels[1], cmyk_channels[3]);
    cv::min(cmyk_channels[3], cmyk_channels[2], cmyk_channels[3]); // K = min(C, M, Y)

    // Compute adjusted C, M, Y and handle edge cases where K = 1
    cv::UMat inverse_k, mask;
    cv::subtract(ones, cmyk_channels[3], inverse_k); // inverse_k = 1 - K
    cv::compare(cmyk_channels[3], ones, mask, cv::CMP_EQ); // Create mask where K == 1

#pragma omp parallel sections
    {
#pragma omp section
        {
            cv::subtract(cmyk_channels[0], cmyk_channels[3], cmyk_channels[0]); // C = C - K
            cv::divide(cmyk_channels[0], inverse_k, cmyk_channels[0], 1, -1); // C /= (1 - K)
            cmyk_channels[0].setTo(0, mask); // Set C to zero where K is 1
        }
#pragma omp section
        {
            cv::subtract(cmyk_channels[1], cmyk_channels[3], cmyk_channels[1]); // M = M - K
            cv::divide(cmyk_channels[1], inverse_k, cmyk_channels[1], 1, -1); // M /= (1 - K)
            cmyk_channels[1].setTo(0, mask); // Set M to zero where K is 1
        }
#pragma omp section
        {
            cv::subtract(cmyk_channels[2], cmyk_channels[3], cmyk_channels[2]); // Y = Y - K
            cv::divide(cmyk_channels[2], inverse_k, cmyk_channels[2], 1, -1); // Y /= (1 - K)
            cmyk_channels[2].setTo(0, mask); // Set Y to zero where K is 1
        }
    }

    // Merge CMYK channels
    cv::merge(cmyk_channels, cmyk_image);

    return cmyk_image;
}


cv::UMat ImageUtils::CmykToRgba(const cv::UMat& cmyk_image) {
    // Check if the input CMYK image has the correct number of channels
    if (cmyk_image.channels() != 4) {
        std::cerr << "Error: Input image must have 4 channels (C, M, Y, K) in floating point format." << std::endl;
        return cv::UMat(); // Return an empty UMat in case of error
    }

    // Split the CMYK image into individual channels
    std::vector<cv::UMat> cmyk_channels;
    cv::split(cmyk_image, cmyk_channels);

    // Prepare UMat containers for RGB channels
    cv::UMat r, g, b;

    // Create a matrix of ones for scalar operations
    cv::UMat ones = cv::UMat::ones(cmyk_channels[0].size(), CV_32F);

    // Convert CMYK to RGB in parallel
#pragma omp parallel sections
    {
#pragma omp section
        cv::subtract(ones, cmyk_channels[0], r); // R = 1 - C

#pragma omp section
        cv::subtract(ones, cmyk_channels[1], g); // G = 1 - M

#pragma omp section
        cv::subtract(ones, cmyk_channels[2], b); // B = 1 - Y
    }

    // Apply the black channel (K)
    cv::UMat inverse_k;
    cv::subtract(ones, cmyk_channels[3], inverse_k); // inverse_k = 1 - K

    // Multiply and convert values back to 8-bit color depth in parallel
#pragma omp parallel sections
    {
#pragma omp section
        {
            cv::multiply(r, inverse_k, r); // R *= (1 - K)
            r.convertTo(r, CV_8U, 255.0); // Convert to 8-bit
        }

#pragma omp section
        {
            cv::multiply(g, inverse_k, g); // G *= (1 - K)
            g.convertTo(g, CV_8U, 255.0); // Convert to 8-bit
        }

#pragma omp section
        {
            cv::multiply(b, inverse_k, b); // B *= (1 - K)
            b.convertTo(b, CV_8U, 255.0); // Convert to 8-bit
        }
    }

    // Create an alpha channel (fully opaque)
    cv::UMat a = cv::UMat::ones(r.size(), CV_8U);
    cv::multiply(a, 255, a); // Ensure alpha channel is fully opaque (255)

    // Merge RGB and Alpha channels into an RGBA image
    std::vector<cv::UMat> rgba_channels = {r, g, b, a};
    cv::UMat rgba_image;
    cv::merge(rgba_channels, rgba_image);

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


void ImageUtils::ResizeImageByWidth(const cv::UMat& inputImage, cv::UMat& outputImage, int newWidth) {
    // Get the original dimensions
    int originalWidth = inputImage.cols;
    int originalHeight = inputImage.rows;

    // Calculate the new height to keep the aspect ratio
    int newHeight = static_cast<int>(newWidth * originalHeight / originalWidth);

    // Resize the image
    cv::resize(inputImage, outputImage, cv::Size(newWidth, newHeight));
}


void ImageUtils::ResizeImageByHeight(const cv::UMat& inputImage, cv::UMat& outputImage, int newHeight) {
    // Get the original dimensions
    int originalWidth = inputImage.cols;
    int originalHeight = inputImage.rows;

    // Calculate the new width to keep the aspect ratio
    int newWidth = static_cast<int>(newHeight * originalWidth / originalHeight);

    // Resize the image
    cv::resize(inputImage, outputImage, cv::Size(newWidth, newHeight));
}


cv::UMat ImageUtils::CropImage(const cv::UMat& rgba_image, const cv::Rect& roi) {
    // Ensure the ROI is within the bounds of the original image
    cv::Rect safe_roi = roi & cv::Rect(0, 0, rgba_image.cols, rgba_image.rows);

    // Crop the image to the safe ROI
    return rgba_image(safe_roi).clone();
}

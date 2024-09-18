#include "ImageUtils.h"
#include <iostream>


cv::UMat ImageUtils::RgbToHsv(const cv::UMat& rgb_image) {
    cv::UMat hsv_image;
    cv::cvtColor(rgb_image, hsv_image, cv::COLOR_RGB2HSV);
    return hsv_image;
}


cv::UMat ImageUtils::HsvToRgb(const cv::UMat& hsv_image) {
    cv::UMat rgb_image;
    cv::cvtColor(hsv_image, rgb_image, cv::COLOR_HSV2RGB);
    return rgb_image;
}


cv::UMat ImageUtils::RgbToCmyk(const cv::UMat& rgb_image) {
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


cv::UMat ImageUtils::CmykToRgb(const cv::UMat& cmyk_image) {
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

    // Merge RGB channels into an RGB image
    std::vector<cv::UMat> rgb_channels = {r, g, b};
    cv::UMat rgb_image;
    cv::merge(rgb_channels, rgb_image);

    return rgb_image;
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


cv::UMat ImageUtils::RgbToHls(const cv::UMat& rgb_image) {
    cv::UMat hls_image;
    cv::cvtColor(rgb_image, hls_image, cv::COLOR_RGB2HLS);
    return hls_image;
}


cv::UMat ImageUtils::HlsToRgb(const cv::UMat &hls_image) {
    cv::UMat rgb_image;
    cv::cvtColor(hls_image, rgb_image, cv::COLOR_HLS2RGB);
    return rgb_image;
}


cv::UMat ImageUtils::RgbToLab(const cv::UMat& rgb_image) {
    cv::UMat lab_image;
    cv::cvtColor(rgb_image, lab_image, cv::COLOR_RGB2Lab);
    return lab_image;
}


cv::UMat ImageUtils::LabToRgb(const cv::UMat& lab_image) {
    cv::UMat rgb_image;
    cv::cvtColor(lab_image, rgb_image, cv::COLOR_Lab2RGB);
    return rgb_image;
}
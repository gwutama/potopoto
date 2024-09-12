#include "Image.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <sstream>
#include "ImageUtils.h"


Image::Image() {
    Close(); // = reset
}

Image::~Image() {}


bool Image::Open(const std::string& in_filename) {
    Close();

    cv::UMat image;
    cv::imread(in_filename, cv::IMREAD_UNCHANGED).copyTo(image);

    if (image.empty()) {
        std::cerr << "Error: Could not load image file: " << in_filename << std::endl;
        Close();
        return false;
    }

    original_image = std::make_shared<cv::UMat>(image.clone());
    image.release();

    // Convert to RGBA format if necessary
    if (original_image->channels() == 1) {
        cv::cvtColor(*original_image, *original_image, cv::COLOR_GRAY2RGBA);
    } else if (original_image->channels() == 3) {
        cv::cvtColor(*original_image, *original_image, cv::COLOR_BGR2RGBA);
    } else if (original_image->channels() != 4) {
        std::cerr << "Error: Unsupported image format with " << original_image->channels() << " channels." << std::endl;
        Close();
        return false;
    }

    adjusted_image = std::make_shared<cv::UMat>(original_image->clone());

    original_image_small = std::make_shared<cv::UMat>();

    // If width is larger than height, then resize by width. Otherwise, resize by height.
    if (original_image->cols > original_image->rows) {
        ImageUtils::ResizeImageByHeight(*original_image, *original_image_small, 128);
    } else {
        ImageUtils::ResizeImageByWidth(*original_image, *original_image_small, 128);
    }

    adjusted_image_histogram = std::make_shared<cv::UMat>(original_image_small->clone());

    UpdateHistogram();
    UpdateImageInfo();

    return true;
}


bool Image::IsOpen() const {
    return original_image != nullptr && !original_image->empty();
}


void Image::Close() {
    if (original_image != nullptr) {
        original_image->release();
    }

    if (adjusted_image != nullptr) {
        adjusted_image->release();
    }
}


bool Image::Save(const std::string& out_filename) const {
    if (adjusted_image == nullptr || adjusted_image->empty()) {
        return false;
    }

    return cv::imwrite(out_filename, *adjusted_image);
}


void Image::LoadToTexture(GLuint& texture) {
    if (adjusted_image == nullptr || adjusted_image->empty()) {
        return;
    }

    if (texture) {
        glDeleteTextures(1, &texture);
    }

    // Download the data from UMat to Mat
    cv::Mat imageMat = adjusted_image->getMat(cv::ACCESS_READ);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageMat.cols, imageMat.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageMat.data);
    glBindTexture(GL_TEXTURE_2D, 0);
}


void Image::AdjustBrightness(float value) {
    brightness_contrast_adjustments_layer.SetBrightness(value);
    brightness_contrast_adjustments_layer_hist.SetBrightness(value);
}


void Image::AdjustContrast(float value) {
    brightness_contrast_adjustments_layer.SetContrast(value);
    brightness_contrast_adjustments_layer_hist.SetContrast(value);
}


void Image::AdjustHue(float value) {
    hsv_adjustments_layer.SetHue(value);
    hsv_adjustments_layer_hist.SetHue(value);
}


void Image::AdjustSaturation(float value) {
    hsv_adjustments_layer.SetSaturation(value);
    hsv_adjustments_layer_hist.SetSaturation(value);
}


void Image::AdjustValue(float value) {
    hsv_adjustments_layer.SetValue(value);
    hsv_adjustments_layer_hist.SetValue(value);
}


bool Image::ApplyAdjustments() {
    if (adjusted_image == nullptr || adjusted_image->empty()) {
        return false;
    }

    if (!brightness_contrast_adjustments_layer.ParametersHaveChanged() &&
        !hsv_adjustments_layer.ParametersHaveChanged()) {
        return false;
    }

    bool image_changed = false;

#pragma omp parallel sections
    {
#pragma omp section
        {
            // Real image
            adjusted_image = std::make_shared<cv::UMat>(original_image->clone());
            brightness_contrast_adjustments_layer.SetImage(adjusted_image);
            image_changed = brightness_contrast_adjustments_layer.Apply() || image_changed;
            hsv_adjustments_layer.SetImage(adjusted_image);
            image_changed = hsv_adjustments_layer.Apply() || image_changed;
        }
#pragma omp section
        {
            // Histogram image
            adjusted_image_histogram = std::make_shared<cv::UMat>(original_image_small->clone());
            brightness_contrast_adjustments_layer_hist.SetImage(adjusted_image_histogram);
            brightness_contrast_adjustments_layer_hist.Apply();
            hsv_adjustments_layer_hist.SetImage(adjusted_image_histogram);
            hsv_adjustments_layer_hist.Apply();

            UpdateHistogram();
        }
    }

    return image_changed;
}


void Image::UpdateHistogram() {
    if (adjusted_image_histogram == nullptr || adjusted_image_histogram->empty()) {
        return;
    }

    bgr_histogram.clear();

    // Separate the image into B, G, R, A planes
    std::vector<cv::Mat> bgr_planes;
    cv::split(*adjusted_image_histogram, bgr_planes);

    // Set the number of bins and range
    int histSize = 256;
    float range[] = {0, 256};
    const float* histRange = {range};
    bool uniform = true;
    bool accumulate = false;

    cv::Mat b_hist, g_hist, r_hist;

    // Compute the histograms
#pragma omp parallel sections
    {
#pragma omp section
        {
            cv::calcHist(&bgr_planes[0], 1, 0, cv::UMat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
        }
#pragma omp section
        {
            cv::calcHist(&bgr_planes[1], 1, 0, cv::UMat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
        }
#pragma omp section
        {
            cv::calcHist(&bgr_planes[2], 1, 0, cv::UMat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);
        }
    }

    bgr_histogram.push_back(b_hist);
    bgr_histogram.push_back(g_hist);
    bgr_histogram.push_back(r_hist);
}


void Image::UpdateImageInfo() {
    if (adjusted_image == nullptr || adjusted_image->empty()) {
        return;
    }

    image_info.clear();

    image_info.insert(std::make_pair("Width", std::to_string(adjusted_image->cols) + " px"));
    image_info.insert(std::make_pair("Height", std::to_string(adjusted_image->rows) + " px"));
    image_info.insert(std::make_pair("Channels", std::to_string(adjusted_image->channels())));
    image_info.insert(std::make_pair("Number of Pixels", std::to_string(adjusted_image->total()) + " px"));
    image_info.insert(std::make_pair("Size", std::to_string(adjusted_image->total() * adjusted_image->elemSize()) + " b"));
}

#include "ImageHistogram.h"
#include "ImageUtils.h"


ImageHistogram::ImageHistogram(const std::shared_ptr<cv::UMat>& in_image) : Image(in_image) {
    original_image = std::make_shared<cv::UMat>(in_image->clone());

    // If width is larger than height, then resize by width. Otherwise, resize by height.
    if (original_image->cols > original_image->rows) {
        ImageUtils::ResizeImageByHeight(*original_image, *original_image, 100);
    } else {
        ImageUtils::ResizeImageByWidth(*original_image, *original_image, 100);
    }

    adjusted_image = std::make_shared<cv::UMat>(original_image->clone());

    UpdateImageInfo();
    UpdateHistogram();
}


ImageHistogram::~ImageHistogram() {
    original_image->release();
    adjusted_image->release();
}


void ImageHistogram::UpdateHistogram() {
    bgr_histogram.clear();

    // Separate the image into B, G, R, A planes
    std::vector<cv::Mat> bgr_planes;
    cv::split(*adjusted_image, bgr_planes);

    // Set the number of bins and range
    int histSize = 256;
    float range[] = {0, 256};
    const float* histRange = {range};
    bool uniform = true;
    bool accumulate = false;

    cv::Mat b_hist, g_hist, r_hist;

    // Compute the histograms
    cv::calcHist(&bgr_planes[0], 1, 0, cv::UMat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
    cv::normalize(b_hist, b_hist, 0, 1, cv::NORM_MINMAX);

    cv::calcHist(&bgr_planes[1], 1, 0, cv::UMat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
    cv::normalize(g_hist, g_hist, 0, 1, cv::NORM_MINMAX);

    cv::calcHist(&bgr_planes[2], 1, 0, cv::UMat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);
    cv::normalize(r_hist, r_hist, 0, 1, cv::NORM_MINMAX);

    bgr_histogram.push_back(b_hist);
    bgr_histogram.push_back(g_hist);
    bgr_histogram.push_back(r_hist);
}


bool ImageHistogram::ApplyAdjustments() {
    bool image_changed = Image::ApplyAdjustments();
    UpdateHistogram();
    return image_changed;
}
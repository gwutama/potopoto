#include "ImageReader.h"


bool ImageReader::Open(const std::string& in_filename, std::shared_ptr<cv::UMat> out_image) {
    cv::UMat image;
    cv::imread(in_filename, cv::IMREAD_UNCHANGED).copyTo(image);

    if (image.empty()) {
        std::cerr << "Error: Could not load image file: " << in_filename << std::endl;
        return false;
    }

    image.copyTo(*out_image);
    image.release();

    // Convert to RGBA format if necessary
    if (out_image->channels() == 1) {
        cv::cvtColor(*out_image, *out_image, cv::COLOR_GRAY2RGBA);
    } else if (out_image->channels() == 3) {
        cv::cvtColor(*out_image, *out_image, cv::COLOR_BGR2RGBA);
    } else if (out_image->channels() != 4) {
        std::cerr << "Error: Unsupported image format with " << out_image->channels() << " channels." << std::endl;
        return false;
    }

    return true;
}
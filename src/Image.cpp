#include "Image.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <exiv2/exiv2.hpp>
#include <sstream>
#include <sys/stat.h>

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

    LoadFileInfo(in_filename);
    LoadImageInfo();
    LoadExifMetadata(in_filename);

    adjusted_image = std::make_shared<cv::UMat>(original_image->clone());

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

    file_info.clear();
    image_info.clear();
    image_exif.clear();
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


void Image::LoadFileInfo(const std::string& filename) {
    struct stat file_stat{};

    if (stat(filename.c_str(), &file_stat) == 0) {
        // File size
        file_info.insert(std::make_pair("File Size", std::to_string(file_stat.st_size) + " b"));

        // File type (extension)
        std::string file_extension = filename.substr(filename.find_last_of(".") + 1);
        file_info.insert(std::make_pair("File Type", file_extension));

        // Creation time
        std::time_t creation_time = file_stat.st_ctime;
        std::stringstream creation_time_str;
        creation_time_str << std::asctime(std::localtime(&creation_time));
        file_info.insert(std::make_pair("Created", creation_time_str.str()));

        // Modification time
        std::time_t modification_time = file_stat.st_mtime;
        std::stringstream modification_time_str;
        modification_time_str << std::asctime(std::localtime(&modification_time));
        file_info.insert(std::make_pair("Modified", modification_time_str.str()));
    } else {
        std::cerr << "Error: Could not retrieve file information for: " << filename << std::endl;
    }
}


void Image::LoadImageInfo() {
    if (original_image == nullptr || original_image->empty()) {
        return;
    }

    image_info.clear();
    image_info.insert(std::make_pair("Width", std::to_string(original_image->cols) + " px"));
    image_info.insert(std::make_pair("Height", std::to_string(original_image->rows) + " px"));
    image_info.insert(std::make_pair("Channels", std::to_string(original_image->channels())));
    image_info.insert(std::make_pair("Number of Pixels", std::to_string(original_image->total()) + " px"));
    image_info.insert(std::make_pair("Size", std::to_string(original_image->total() * original_image->elemSize()) + " b"));
}


void Image::LoadExifMetadata(const std::string& filename) {
    try {
        Exiv2::Image::UniquePtr my_image = Exiv2::ImageFactory::open(filename);

        if (!my_image) {
            std::cerr << "Error: Could not open image file for metadata extraction: " << filename << std::endl;
            return;
        }

        my_image->readMetadata();

        Exiv2::ExifData &exifData = my_image->exifData();

        if (exifData.empty()) {
            std::cerr << "No Exif data found in the file: " << filename << std::endl;
            return;
        }

        for (Exiv2::ExifData::const_iterator i = exifData.begin(); i != exifData.end(); ++i) {
            std::string key = i->key();
            std::string value = i->value().toString();
            image_exif.insert(std::make_pair(key, value));
        }
    } catch (Exiv2::Error& e) {
        std::cerr << "Error reading EXIF data: " << e.what() << std::endl;
    }
}


std::vector<cv::Mat> Image::CalculateNormalizedHistogram() const {
    std::vector<cv::Mat> bgr_hist;

    if (adjusted_image == nullptr || adjusted_image->empty()) {
        return bgr_hist;
    }

    // Convert image to RGB if necessary
    cv::Mat imageRGB;
    if (adjusted_image->channels() == 4) {
        cv::cvtColor(*adjusted_image, imageRGB, cv::COLOR_RGBA2RGB);
    } else if (adjusted_image->channels() == 3) {
        adjusted_image->copyTo(imageRGB);
    } else {
        std::cerr << "Error: Unsupported image format with " << adjusted_image->channels() << " channels." << std::endl;
        return bgr_hist;
    }

    // Separate the image into B, G, R planes
    std::vector<cv::Mat> bgr_planes;
    cv::split(imageRGB, bgr_planes);

    // Set the number of bins and range
    int histSize = 256;
    float range[] = {0, 256};
    const float* histRange = {range};
    bool uniform = true, accumulate = false;

    cv::Mat b_hist, g_hist, r_hist;

    // Compute the histograms
#pragma omp parallel sections
    {
#pragma omp section
        {
            cv::calcHist(&bgr_planes[0], 1, 0, cv::UMat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
            cv::normalize(b_hist, b_hist, 0, 1, cv::NORM_MINMAX);
        }
#pragma omp section
        {
            cv::calcHist(&bgr_planes[1], 1, 0, cv::UMat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
            cv::normalize(g_hist, g_hist, 0, 1, cv::NORM_MINMAX);
        }
#pragma omp section
        {
            cv::calcHist(&bgr_planes[2], 1, 0, cv::UMat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);
            cv::normalize(r_hist, r_hist, 0, 1, cv::NORM_MINMAX);
        }
    }

    bgr_hist.push_back(b_hist);
    bgr_hist.push_back(g_hist);
    bgr_hist.push_back(r_hist);

    return bgr_hist;
}


void Image::AdjustBrightness(float value) {
    if (adjusted_image == nullptr || adjusted_image->empty()) {
        return;
    }

    brightness_contrast_adjustments_layer.SetBrightness(value);
}


void Image::AdjustContrast(float value) {
    if (adjusted_image == nullptr || adjusted_image->empty()) {
        return;
    }

    brightness_contrast_adjustments_layer.SetContrast(value);
}


void Image::AdjustHue(float value) {
    if (adjusted_image == nullptr || adjusted_image->empty()) {
        return;
    }

    hsv_adjustments_layer.SetHue(value);
}


void Image::AdjustSaturation(float value) {
    if (adjusted_image == nullptr || adjusted_image->empty()) {
        return;
    }

    hsv_adjustments_layer.SetSaturation(value);
}


void Image::AdjustValue(float value) {
    if (adjusted_image == nullptr || adjusted_image->empty()) {
        return;
    }

    hsv_adjustments_layer.SetValue(value);
}


void Image::ApplyAdjustments() {
    if (adjusted_image == nullptr || adjusted_image->empty()) {
        return;
    }

    adjusted_image = std::make_shared<cv::UMat>(original_image->clone());
    brightness_contrast_adjustments_layer.SetImage(adjusted_image);
    brightness_contrast_adjustments_layer.Apply();
    hsv_adjustments_layer.SetImage(adjusted_image);
    hsv_adjustments_layer.Apply();
}
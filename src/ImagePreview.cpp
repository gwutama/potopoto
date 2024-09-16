#include "ImagePreview.h"
#include "ImageUtils.h"


const int ImagePreview::TARGET_LOD_LOW_PIXELS = 4000000;
const int ImagePreview::TARGET_LOD_MEDIUM_PIXELS = 8000000;
const int ImagePreview::TARGET_LOD_HIGH_PIXELS = 12000000;


ImagePreview::ImagePreview(const std::shared_ptr<Image>& in_image) {
    GenerateLodImages(in_image);
}


ImagePreview::~ImagePreview() {
}


void ImagePreview::LoadCurrentLodToTexture(GLuint &texture) {
    // Only load if the task is not running otherwise it will be broken
    if (!apply_adjustments_tasks.at(current_lod_level)->IsRunning()) {
        LoadToTexture(current_lod_level, texture);
    }
}


void ImagePreview::LoadToTexture(LodLevel lod_level, GLuint& texture) {
    if (texture) {
        glDeleteTextures(1, &texture);
    }

    // Download the data from UMat to Mat
    auto lod_image = lod_images.at(lod_level);
    cv::Mat imageMat = lod_image->GetAdjustedImage()->getMat(cv::ACCESS_READ);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageMat.cols, imageMat.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageMat.data);
    glBindTexture(GL_TEXTURE_2D, 0);

    current_lod_level = lod_level;
}


void ImagePreview::GenerateLodImages(const std::shared_ptr<Image>& in_image) {
    lod_images.clear();

    std::shared_ptr<Image> lod_low = GenerateLodImage(in_image, LodLevel::LOW);
    lod_images.insert({LodLevel::LOW, lod_low});
    auto lod_low_task = std::make_shared<ImageApplyAdjustmentsTask>(lod_low, std::chrono::seconds (600));
    apply_adjustments_tasks.insert({LodLevel::LOW, lod_low_task});

    std::shared_ptr<Image> lod_medium = GenerateLodImage(in_image, LodLevel::MEDIUM);
    lod_images.insert({LodLevel::MEDIUM, lod_medium});
    auto lod_medium_task = std::make_shared<ImageApplyAdjustmentsTask>(lod_medium, std::chrono::seconds (600));
    apply_adjustments_tasks.insert({LodLevel::MEDIUM, lod_medium_task});

    std::shared_ptr<Image> lod_high = GenerateLodImage(in_image, LodLevel::HIGH);
    lod_images.insert({LodLevel::HIGH, lod_high});
    auto lod_high_task = std::make_shared<ImageApplyAdjustmentsTask>(lod_high, std::chrono::seconds (600));
    apply_adjustments_tasks.insert({LodLevel::HIGH, lod_high_task});
}


std::shared_ptr<Image> ImagePreview::GenerateLodImage(const std::shared_ptr<Image>& in_image, LodLevel lod_level) {
    std::cout << "Generating LOD image for level " << static_cast<int>(lod_level) << std::endl;
    auto image_copy = in_image->Clone();

    int target_px = 0;

    switch (lod_level) {
        case LodLevel::LOW:
            target_px = TARGET_LOD_LOW_PIXELS;
            break;
        case LodLevel::MEDIUM:
            target_px = TARGET_LOD_MEDIUM_PIXELS;
            break;
        case LodLevel::HIGH:
            target_px = TARGET_LOD_HIGH_PIXELS;
            break;
        default:
            target_px = TARGET_LOD_HIGH_PIXELS;
    }

    auto cv_lod_image = ResizeImageLod(image_copy->GetAdjustedImage(), target_px);

    auto lod_image = std::make_shared<Image>(cv_lod_image);
    lod_image->AdjustParameters(parameters);
    return lod_image;
}


std::shared_ptr<cv::UMat> ImagePreview::ResizeImageLod(const std::shared_ptr<cv::UMat>& in_image, int target_px) {
    auto out_image = std::make_shared<cv::UMat>(in_image->clone());

    // Calculate the total number of pixels in the original image
    int res = in_image->cols * in_image->rows;

    // Calculate the correct resize factor for linear dimensions
    float resize_factor = std::sqrt((float) target_px / (float) res);

    // Resize the image if the factor is less than 1 (i.e., target size is smaller than the original size)
    if (resize_factor < 1) {
        auto new_width = static_cast<int>(std::ceil(in_image->cols * resize_factor));
        ImageUtils::ResizeImageByWidth(*in_image, *out_image, new_width);

        std::cout << "Image resized for preview to " << out_image->cols << "x" << out_image->rows
                  << " with factor " << resize_factor << std::endl;
    }

    return out_image;
}


void ImagePreview::AdjustParameters(const AdjustmentsParameters& parameters_in) {
    parameters = parameters_in;
    lod_images.at(LodLevel::LOW)->AdjustParameters(parameters);
    lod_images.at(LodLevel::MEDIUM)->AdjustParameters(parameters);
    lod_images.at(LodLevel::HIGH)->AdjustParameters(parameters);
}


bool ImagePreview::ApplyAdjustmentsForCurrentLod() {
    return lod_images.at(current_lod_level)->ApplyAdjustments();
}


void ImagePreview::ApplyAdjustmentsForNonCurrentLodsAsync() {
    // For the rest, apply the adjustments in background tasks
    for (auto& task : apply_adjustments_tasks) {
        if (task.first != current_lod_level) {
            task.second->Stop();
            task.second->Run();
        }
    }
}
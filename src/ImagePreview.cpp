#include "ImagePreview.h"
#include "ImageUtils.h"


const int ImagePreview::TARGET_LOD_LOW_PIXELS = 4000000;
const int ImagePreview::TARGET_LOD_MEDIUM_PIXELS = 8000000;
const int ImagePreview::TARGET_LOD_HIGH_PIXELS = 12000000;


ImagePreview::ImagePreview(const std::shared_ptr<Image>& in_image) : current_lod_level(LodLevel::UNDEFINED) {
    GenerateLodImages(in_image);
}


ImagePreview::~ImagePreview() {
}


void ImagePreview::LoadCurrentLodToTexture(GLuint &texture) {
    if (current_lod_level == LodLevel::UNDEFINED) {
        return;
    }

    // Check if a task exists for the current LOD level
    auto task_iter = apply_adjustments_tasks.find(current_lod_level);
    bool task_exists = (task_iter != apply_adjustments_tasks.end());

    if (task_exists) {
        auto task = task_iter->second;

        if (task->IsRunning()) {
            // Task is still running, load the partial image to the texture
            LoadToTexture(partial_lod_image, texture);
        } else {
            // Task is completed, reset and erase it
            task_iter->second.reset();
            apply_adjustments_tasks.erase(task_iter);
            LoadToTexture(lod_images.at(current_lod_level), texture);
        }
    } else {
        // task does not exist (anymore?). Which one was the last one?
        auto lod_image = lod_images.at(current_lod_level);

        if (lod_image->GetLastAdjustmentTime() > partial_lod_image->GetLastAdjustmentTime()) {
            LoadToTexture(lod_image, texture);
        } else {
            LoadToTexture(partial_lod_image, texture);
        }
    }
}


void ImagePreview::LoadToTexture(const std::shared_ptr<Image>& lod_image, GLuint& texture) {
    if (texture) {
        glDeleteTextures(1, &texture);
    }

    // Download the data from UMat to Mat
    cv::Mat imageMat = lod_image->GetAdjustedImage()->getMat(cv::ACCESS_READ);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageMat.cols, imageMat.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageMat.data);
    glBindTexture(GL_TEXTURE_2D, 0);
}


void ImagePreview::GenerateLodImages(const std::shared_ptr<Image>& in_image) {
    lod_images.clear();

    std::shared_ptr<Image> lod_low = GenerateLodImage(in_image, LodLevel::LOW);
    lod_images.insert({LodLevel::LOW, lod_low});

    std::shared_ptr<Image> lod_medium = GenerateLodImage(in_image, LodLevel::MEDIUM);
    lod_images.insert({LodLevel::MEDIUM, lod_medium});

    std::shared_ptr<Image> lod_high = GenerateLodImage(in_image, LodLevel::HIGH);
    lod_images.insert({LodLevel::HIGH, lod_high});
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
    partial_lod_image->AdjustParameters(parameters);
}


bool ImagePreview::ApplyAdjustmentsForPreviewRegion(const cv::Rect& region) {
    if (partial_lod_image == nullptr || current_lod_level == LodLevel::UNDEFINED) {
        return false;
    }

    return partial_lod_image->ApplyAdjustmentsRegion(region);
}


void ImagePreview::ApplyAdjustmentsForAllLodsAsync() {
    // Stop and remove any existing tasks
    for (auto& task : apply_adjustments_tasks) {
        task.second->Stop();
    }

    apply_adjustments_tasks.clear();

    auto lod_low_task = std::make_shared<ImageApplyAdjustmentsTask>(lod_images.at(LodLevel::LOW),
                                                                    std::chrono::seconds (600));
    apply_adjustments_tasks.insert({LodLevel::LOW, lod_low_task});
    lod_low_task->Run();

    auto lod_medium_task = std::make_shared<ImageApplyAdjustmentsTask>(lod_images.at(LodLevel::MEDIUM),
                                                                       std::chrono::seconds (600));
    apply_adjustments_tasks.insert({LodLevel::MEDIUM, lod_medium_task});
    lod_medium_task->Run();

    auto lod_high_task = std::make_shared<ImageApplyAdjustmentsTask>(lod_images.at(LodLevel::HIGH),
                                                                     std::chrono::seconds (600));
    apply_adjustments_tasks.insert({LodLevel::HIGH, lod_high_task});
    lod_high_task->Run();
}


void ImagePreview::SetLodLevel(ImagePreview::LodLevel lod_level) {
    if (lod_level == current_lod_level || lod_level == LodLevel::UNDEFINED) {
        return;
    }

    current_lod_level = lod_level;
    partial_lod_image = lod_images.at(lod_level)->Clone();
}
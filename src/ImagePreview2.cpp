#include "ImagePreview2.h"


const int ImagePreview2::TARGET_LOD_LOW_PIXELS = 3000000;
const int ImagePreview2::TARGET_LOD_MEDIUM_PIXELS = 6000000;
const int ImagePreview2::TARGET_LOD_HIGH_PIXELS = 9000000;


ImagePreview2::ImagePreview2() {
    Reset();
}


ImagePreview2::~ImagePreview2() {
}


void ImagePreview2::LoadImage(const std::shared_ptr<Image> &in_image) {
    GenerateLodImages(in_image);
    SetLodLevel(LodLevel::LOW);
}


void ImagePreview2::Reset() {
    lod_images.clear();
    partial_lod_image.reset();
    current_lod_level = LodLevel::LOW;
    lod_sizes.clear();
    parameters.Reset();
    apply_adjustments_tasks.clear();
}


void ImagePreview2::GenerateLodImages(const std::shared_ptr<Image>& in_image) {
    lod_images.clear();
    lod_images.clear();

    std::shared_ptr<Image> lod_low = GenerateLodImage(in_image, LodLevel::LOW);
    lod_images.insert({LodLevel::LOW, lod_low});
    lod_sizes.insert({LodLevel::LOW, lod_low->GetAdjustedImage()->size()});

    std::shared_ptr<Image> lod_medium = GenerateLodImage(in_image, LodLevel::MEDIUM);
    lod_images.insert({LodLevel::MEDIUM, lod_medium});
    lod_sizes.insert({LodLevel::MEDIUM, lod_medium->GetAdjustedImage()->size()});

    std::shared_ptr<Image> lod_high = GenerateLodImage(in_image, LodLevel::HIGH);
    lod_images.insert({LodLevel::HIGH, lod_high});
    lod_sizes.insert({LodLevel::HIGH, lod_high->GetAdjustedImage()->size()});
}


std::shared_ptr<Image> ImagePreview2::GenerateLodImage(const std::shared_ptr<Image>& in_image, LodLevel lod_level) {
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


std::shared_ptr<cv::UMat> ImagePreview2::ResizeImageLod(const std::shared_ptr<cv::UMat>& in_image, int target_px) {
    auto out_image = std::make_shared<cv::UMat>(in_image->clone());

    // Calculate the total number of pixels in the original image
    int res = in_image->cols * in_image->rows;

    // Calculate the correct resize factor for linear dimensions
    float resize_factor = std::sqrt((float) target_px / (float) res);

    // Resize the image if the factor is less than 1 (i.e., target size is smaller than the original size)
    if (resize_factor < 1) {
        auto new_width = static_cast<int>(std::ceil(in_image->cols * resize_factor));
        ResizeImageByWidth(*in_image, *out_image, new_width);

        std::cout << "Image resized for preview to " << out_image->cols << "x" << out_image->rows
                  << " with factor " << resize_factor << std::endl;
    }

    return out_image;
}


void ImagePreview2::AdjustParameters(const AdjustmentsParameters& parameters_in) {
    parameters = parameters_in;
    lod_images.at(LodLevel::LOW)->AdjustParameters(parameters);
    lod_images.at(LodLevel::MEDIUM)->AdjustParameters(parameters);
    lod_images.at(LodLevel::HIGH)->AdjustParameters(parameters);
    partial_lod_image->AdjustParameters(parameters);
}


bool ImagePreview2::ApplyAdjustmentsForPreviewRegion(const cv::Rect& region) {
    return partial_lod_image->ApplyAdjustmentsRegion(region);
}


void ImagePreview2::ApplyAdjustmentsForAllLodsAsync() {
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


void ImagePreview2::SetLodLevel(ImagePreview2::LodLevel lod_level) {
    current_lod_level = lod_level;
    partial_lod_image = lod_images.at(lod_level)->Clone();
}


cv::Mat ImagePreview2::GetImage() {
    std::cout << "Getting image for LOD level " << static_cast<int>(current_lod_level) << std::endl;

    // Check if a task exists for the current LOD level
    auto task_iter = apply_adjustments_tasks.find(current_lod_level);
    bool task_exists = (task_iter != apply_adjustments_tasks.end());

    if (task_exists) {
        auto task = task_iter->second;

        if (task->IsRunning()) {
            // Task is still running, return the partial image
            return partial_lod_image->GetAdjustedImage()->getMat(cv::ACCESS_READ);
        } else {
            // Task is completed, reset and erase it. Return the full image of the current LOD level
            task_iter->second.reset();
            apply_adjustments_tasks.erase(task_iter);
            return lod_images.at(current_lod_level)->GetAdjustedImage()->getMat(cv::ACCESS_READ);
        }
    } else {
        // task does not exist (anymore?). Which one was the last one? Return the latest image
        auto lod_image = lod_images.at(current_lod_level);

        if (lod_image->GetLastAdjustmentTime() > partial_lod_image->GetLastAdjustmentTime()) {
            return lod_image->GetAdjustedImage()->getMat(cv::ACCESS_READ);
        } else {
            return partial_lod_image->GetAdjustedImage()->getMat(cv::ACCESS_READ);
        }
    }
}


void ImagePreview2::ResizeImageByWidth(const cv::UMat& inputImage, cv::UMat& outputImage, int newWidth) {
    // Get the original dimensions
    int originalWidth = inputImage.cols;
    int originalHeight = inputImage.rows;

    // Calculate the new height to maintain aspect ratio
    int newHeight = static_cast<int>(newWidth * (static_cast<float>(originalHeight) / originalWidth));

    // Use Lanczos interpolation for high-quality resizing
    cv::resize(inputImage, outputImage, cv::Size(newWidth, newHeight), 0, 0, cv::INTER_LANCZOS4);
}

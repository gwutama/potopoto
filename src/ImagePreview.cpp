#include "ImagePreview.h"


const int ImagePreview::TARGET_LOD_LOW_PIXELS = 2000000;
const int ImagePreview::TARGET_LOD_MEDIUM_PIXELS = 6000000;
const int ImagePreview::TARGET_LOD_HIGH_PIXELS = 9000000;


ImagePreview::ImagePreview() {
    Reset();
}


ImagePreview::~ImagePreview() {
}


void ImagePreview::LoadImage(const std::shared_ptr<Image> &in_image) {
    GenerateLodImages(in_image);
    SetLodLevel(LodLevel::LOW);
}


void ImagePreview::Reset() {
    lod_images.clear();
    partial_lod_image.reset();
    current_lod_level = LodLevel::LOW;
    lod_sizes.clear();
    parameters = std::make_shared<AdjustmentsParameters>();
    apply_adjustments_tasks.clear();
    completedTasks = 0;
}


void ImagePreview::GenerateLodImages(const std::shared_ptr<Image>& in_image) {
    lod_images.clear();
    lod_sizes.clear();

    std::shared_ptr<Image> lod_low = GenerateLodImage(in_image, LodLevel::LOW);
    lod_images.insert({LodLevel::LOW, lod_low});
    lod_sizes.insert({LodLevel::LOW, lod_low->GetAdjustedImage()->size()});

    std::shared_ptr<Image> lod_medium = GenerateLodImage(in_image, LodLevel::MEDIUM);
    lod_images.insert({LodLevel::MEDIUM, lod_medium});
    lod_sizes.insert({LodLevel::MEDIUM, lod_medium->GetAdjustedImage()->size()});

    std::shared_ptr<Image> lod_high = GenerateLodImage(in_image, LodLevel::HIGH);
    lod_images.insert({LodLevel::HIGH, lod_high});
    lod_sizes.insert({LodLevel::HIGH, lod_high->GetAdjustedImage()->size()});

    partial_lod_image = lod_images.at(current_lod_level)->Clone();
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
        ResizeImageByWidth(*in_image, *out_image, new_width);

        std::cout << "Image resized for preview to " << out_image->cols << "x" << out_image->rows
                  << " with factor " << resize_factor << std::endl;
    }

    return out_image;
}


void ImagePreview::AdjustParameters(std::shared_ptr<AdjustmentsParameters> parameters_in) {
    parameters = parameters_in;
    lod_images.at(LodLevel::LOW)->AdjustParameters(parameters);
    lod_images.at(LodLevel::MEDIUM)->AdjustParameters(parameters);
    lod_images.at(LodLevel::HIGH)->AdjustParameters(parameters);
    partial_lod_image->AdjustParameters(parameters);
}


bool ImagePreview::ApplyAdjustmentsForPreviewRegion(const cv::Rect& region) {
    return partial_lod_image->ApplyAdjustmentsRegion(region);
}


void ImagePreview::ApplyAdjustmentsForAllLodsAsync(std::function<void()> successCallback) {
    std::unique_lock<std::shared_mutex> lock(lodImageMutex);

    for (auto& task : apply_adjustments_tasks) {
        task.second->Stop();
    }

    apply_adjustments_tasks.clear();

    completedTasks = 0;
    const int totalTasks = 3;  // LOW, MEDIUM, HIGH

    auto onTaskCompleted = [this, totalTasks, successCallback]() {
        std::unique_lock<std::shared_mutex> lock(lodImageMutex);

        completedTasks++;
        if (completedTasks == totalTasks && successCallback) {
            successCallback();
        }
    };

    auto lod_low_task = std::make_shared<ImageApplyAdjustmentsTask>(lod_images.at(LodLevel::LOW), std::chrono::seconds(600));
    apply_adjustments_tasks.insert({LodLevel::LOW, lod_low_task});
    lod_low_task->Run([this, onTaskCompleted](TaskStatus status) {
        if (status == TaskStatus::SUCCESS) {
            onTaskCompleted();
        }
    });

    auto lod_medium_task = std::make_shared<ImageApplyAdjustmentsTask>(lod_images.at(LodLevel::MEDIUM), std::chrono::seconds(600));
    apply_adjustments_tasks.insert({LodLevel::MEDIUM, lod_medium_task});
    lod_medium_task->Run([this, onTaskCompleted](TaskStatus status) {
        if (status == TaskStatus::SUCCESS) {
            onTaskCompleted();
        }
    });

    auto lod_high_task = std::make_shared<ImageApplyAdjustmentsTask>(lod_images.at(LodLevel::HIGH), std::chrono::seconds(600));
    apply_adjustments_tasks.insert({LodLevel::HIGH, lod_high_task});
    lod_high_task->Run([this, onTaskCompleted](TaskStatus status) {
        if (status == TaskStatus::SUCCESS) {
            onTaskCompleted();
        }
    });
}


void ImagePreview::SetLodLevel(ImagePreview::LodLevel lod_level) {
    current_lod_level = lod_level;
    partial_lod_image = lod_images.at(lod_level)->Clone();
}


cv::Mat ImagePreview::GetImage() {
    std::shared_lock<std::shared_mutex> lock(lodImageMutex);

    std::cout << "Getting image for LOD level " << static_cast<int>(current_lod_level) << std::endl;

    auto lod_image = lod_images.at(current_lod_level);
    if (lod_image->GetLastAdjustmentTime() >= partial_lod_image->GetLastAdjustmentTime()) {
        std::cout << "Returning full LOD image." << std::endl;
        return lod_image->GetAdjustedImage()->getMat(cv::ACCESS_READ);
    } else if (partial_lod_image) {
        std::cout << "Returning partial LOD image." << std::endl;
        return partial_lod_image->GetAdjustedImage()->getMat(cv::ACCESS_READ);
    } else {
        std::cerr << "No image available!" << std::endl;
        return cv::Mat();  // Return an empty image
    }
}


void ImagePreview::ResizeImageByWidth(const cv::UMat& inputImage, cv::UMat& outputImage, int newWidth) {
    // Get the original dimensions
    int originalWidth = inputImage.cols;
    int originalHeight = inputImage.rows;

    // Calculate the new height to maintain aspect ratio
    int newHeight = static_cast<int>(newWidth * (static_cast<float>(originalHeight) / originalWidth));

    // Use Lanczos interpolation for high-quality resizing
    cv::resize(inputImage, outputImage, cv::Size(newWidth, newHeight), 0, 0, cv::INTER_LANCZOS4);
}

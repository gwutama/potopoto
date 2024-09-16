#include "ImagePreview.h"
#include "ImageUtils.h"


const int ImagePreview::TARGET_PREVIEW_PIXELS = 8000000; // 8 MP = 8,000,000 pixels


ImagePreview::ImagePreview(const std::shared_ptr<cv::UMat>& in_image) : Image(in_image) {
    ResizeOriginalImage(TARGET_PREVIEW_PIXELS);
}


ImagePreview::~ImagePreview() {
}


void ImagePreview::LoadToTexture(GLuint& texture) {
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


void ImagePreview::ResizeOriginalImage(int target_px) {
    std::cout << "Original image size is " << GetWidth() << "x" << GetHeight() << std::endl;

    // Resize original image to maximum width or height of max_px, keeping the aspect ratio
    // target pixel size = factor(original width * original height)
    // factor = target pixel size / (original width * original height)
    // Calculate the factor
    resize_factor = (float) target_px / (float)(original_image->cols * original_image->rows);

    // If the factor is less than 1, then the image is smaller than the target size
    // Resize the image to the target size
    // otherwise keep the image size
    if (resize_factor < 1) {
        auto new_width = static_cast<int>(std::ceil((float)original_image->cols * resize_factor));
        std::cout << "New width: " << new_width << std::endl;
        ImageUtils::ResizeImageByWidth(*original_image, *original_image, new_width);

        std::cout << "Image resized for preview to " << original_image->cols << "x" << original_image->rows
                  << " with factor " << resize_factor << std::endl;
    }

    adjusted_image = std::make_shared<cv::UMat>(original_image->clone());
}
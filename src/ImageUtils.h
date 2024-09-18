#ifndef POTOPOTO_IMAGEUTILS_H
#define POTOPOTO_IMAGEUTILS_H

#include <opencv2/opencv.hpp>
#include <vector>

class ImageUtils {
public:
    static cv::UMat RgbaToHsv(const cv::UMat& rgba_image);
    static cv::UMat HsvToRgba(const cv::UMat& hsv_image);
    static cv::UMat RgbaToCmyk(const cv::UMat& rgba_image);
    static cv::UMat CmykToRgba(const cv::UMat& cmyk_image);
    static cv::UMat CombineHsvImages(const std::vector<cv::UMat>& hsv_images);
    static cv::UMat CombineRgbaImages(const std::vector<cv::UMat>& rgba_images);
    static void ResizeImageByWidth(const cv::UMat& inputImage, cv::UMat& outputImage, int newWidth);
    static void ResizeImageByHeight(const cv::UMat& inputImage, cv::UMat& outputImage, int newHeight);
    static cv::UMat CropImage(const cv::UMat& rgba_image, const cv::Rect& roi);
    static cv::UMat RgbaToHls(const cv::UMat& rgba_image);
    static cv::UMat HlsToRgba(const cv::UMat& hls_image);
    static cv::UMat RgbaToLab(const cv::UMat& rgba_image);
    static cv::UMat LabToRgba(const cv::UMat& lab_image);
};


#endif //POTOPOTO_IMAGEUTILS_H

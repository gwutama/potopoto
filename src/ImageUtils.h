#ifndef POTOPOTO_IMAGEUTILS_H
#define POTOPOTO_IMAGEUTILS_H

#include <opencv2/opencv.hpp>
#include <vector>

class ImageUtils {
public:
    static cv::UMat RgbToHsv(const cv::UMat& rgb_image);
    static cv::UMat HsvToRgb(const cv::UMat& hsv_image);
    static cv::UMat RgbToCmyk(const cv::UMat& rgba_image);
    static cv::UMat CmykToRgb(const cv::UMat& cmyk_image);
    static void ResizeImageByWidth(const cv::UMat& inputImage, cv::UMat& outputImage, int newWidth);
    static void ResizeImageByHeight(const cv::UMat& inputImage, cv::UMat& outputImage, int newHeight);
    static cv::UMat CropImage(const cv::UMat& rgba_image, const cv::Rect& roi);
    static cv::UMat RgbToHls(const cv::UMat& rgb_image);
    static cv::UMat HlsToRgb(const cv::UMat& hls_image);
    static cv::UMat RgbToLab(const cv::UMat& rgb_image);
    static cv::UMat LabToRgb(const cv::UMat& lab_image);
};


#endif //POTOPOTO_IMAGEUTILS_H

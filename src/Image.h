#ifndef POTOPOTO_IMAGE_H
#define POTOPOTO_IMAGE_H

#include <opencv2/opencv.hpp>
#include <string>
#include <map>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "LayerBrightnessContrast.h"
#include "LayerHueSaturationValue.h"

class Image {
public:
    Image();
    ~Image();

    bool Open(const std::string& in_filename);
    bool IsOpen() const;
    void Close();
    bool Save(const std::string& out_filename) const;

    void LoadToTexture(GLuint& texture);

    int GetWidth() const { return adjusted_image->cols; }
    int GetHeight() const { return adjusted_image->rows; }

    std::unordered_map<std::string, std::string> GetImageInfo() const { return image_info; }

    std::vector<cv::Mat> GetHistogram() const { return bgr_histogram; }

    void AdjustBrightness(float value);
    void AdjustContrast(float value);
    void AdjustHue(float value);
    void AdjustSaturation(float value);
    void AdjustValue(float value);
    bool ApplyAdjustments();

private:
    void UpdateImageInfo();
    void UpdateHistogram();

private:
    std::shared_ptr<cv::UMat> original_image;
    std::shared_ptr<cv::UMat> adjusted_image;
    std::shared_ptr<cv::UMat> original_image_small;
    std::shared_ptr<cv::UMat> adjusted_image_histogram;
    std::vector<cv::Mat> bgr_histogram;
    std::unordered_map<std::string, std::string> image_info;

    LayerBrightnessContrast brightness_contrast_adjustments_layer;
    LayerHueSaturationValue hsv_adjustments_layer;

    LayerBrightnessContrast brightness_contrast_adjustments_layer_hist;
    LayerHueSaturationValue hsv_adjustments_layer_hist;
};

#endif //POTOPOTO_IMAGE_H
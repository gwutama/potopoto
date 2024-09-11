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
    std::map<std::string, std::string> GetFileInfo() const { return file_info; }
    std::map<std::string, std::string> GetImageInfo() const { return image_info; }
    std::map<std::string, std::string> GetExifMetadata() const { return image_exif; }

    std::vector<cv::Mat> CalculateNormalizedHistogram() const;

    void AdjustBrightness(float value);
    void AdjustContrast(float value);
    void AdjustHue(float value);
    void AdjustSaturation(float value);
    void AdjustValue(float value);
    void ApplyAdjustments();

private:
    void LoadFileInfo(const std::string& filename);
    void LoadImageInfo();
    void LoadExifMetadata(const std::string& filename);

private:
    std::shared_ptr<cv::UMat> original_image;
    std::shared_ptr<cv::UMat> adjusted_image;

    std::map<std::string, std::string> file_info;
    std::map<std::string, std::string> image_info;
    std::map<std::string, std::string> image_exif;

    LayerBrightnessContrast brightness_contrast_adjustments_layer;
    LayerHueSaturationValue hsv_adjustments_layer;
};

#endif //POTOPOTO_IMAGE_H
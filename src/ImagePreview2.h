#ifndef POTOPOTO_IMAGEPREVIEW_H
#define POTOPOTO_IMAGEPREVIEW_H

#include <map>
#include <OpenGL/gl.h>
#include <opencv2/opencv.hpp>

#include "Image.h"
#include "ImageApplyAdjustmentsTask.h"
#include "AdjustmentsParameters.h"


class ImagePreview2 {
public:
    enum class LodLevel {
        LOW,
        MEDIUM,
        HIGH,
    };

    ImagePreview2();
    ~ImagePreview2();

    void LoadImage(const std::shared_ptr<Image>& in_image);
    void Reset();

    void AdjustParameters(const AdjustmentsParameters& parameters_in);
    bool ApplyAdjustmentsForPreviewRegion(const cv::Rect& region);
    void ApplyAdjustmentsForAllLodsAsync();

    void SetLodLevel(LodLevel lod_level);
    std::map<LodLevel, cv::Size> GetLodSizes() const { return lod_sizes; }

    cv::Mat GetImage();
    cv::Size GetSize() const { return lod_sizes.at(current_lod_level); }
    cv::Size GetSize(LodLevel lodLevel) const { return lod_sizes.at(lodLevel); }

private:
    void GenerateLodImages(const std::shared_ptr<Image>& in_image);
    std::shared_ptr<Image> GenerateLodImage(const std::shared_ptr<Image>& in_image, LodLevel lod_level);
    std::shared_ptr<cv::UMat> ResizeImageLod(const std::shared_ptr<cv::UMat>& in_image, int target_px);
    void ResizeImageByWidth(const cv::UMat& inputImage, cv::UMat& outputImage, int newWidth);

private:
    static const int TARGET_LOD_LOW_PIXELS;
    static const int TARGET_LOD_MEDIUM_PIXELS;
    static const int TARGET_LOD_HIGH_PIXELS;
    std::map<LodLevel, std::shared_ptr<Image>> lod_images;
    std::shared_ptr<Image> partial_lod_image;
    LodLevel current_lod_level;
    std::map<LodLevel, cv::Size> lod_sizes;
    AdjustmentsParameters parameters;
    std::unordered_map<LodLevel, std::shared_ptr<ImageApplyAdjustmentsTask>> apply_adjustments_tasks;
};


#endif //POTOPOTO_IMAGEPREVIEW_H

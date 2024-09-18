#ifndef POTOPOTO_IMAGEPREVIEW_H
#define POTOPOTO_IMAGEPREVIEW_H

#include <map>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <opencv2/opencv.hpp>

#include "Image.h"
#include "ImageApplyAdjustmentsTask.h"


class ImagePreview {
public:
    enum class LodLevel {
        UNDEFINED,
        LOW,
        MEDIUM,
        HIGH,
    };

    ImagePreview(const std::shared_ptr<Image>& in_image);
    ~ImagePreview();

    void LoadCurrentLodToTexture(GLuint& texture);

    void AdjustParameters(const AdjustmentsParameters& parameters_in);
    bool ApplyAdjustmentsForPreviewRegion(const cv::Rect& region);
    void ApplyAdjustmentsForAllLodsAsync();

    void SetLodLevel(LodLevel lod_level);

    std::shared_ptr<Image> GetImageCurrentLod() const { return lod_images.at(current_lod_level); }

private:
    void LoadToTexture(const std::shared_ptr<Image>& lod_image, GLuint& texture);
    void GenerateLodImages(const std::shared_ptr<Image>& in_image);
    std::shared_ptr<Image> GenerateLodImage(const std::shared_ptr<Image>& in_image, LodLevel lod_level);
    std::shared_ptr<cv::UMat> ResizeImageLod(const std::shared_ptr<cv::UMat>& in_image, int target_px);

private:
    static const int TARGET_LOD_LOW_PIXELS;
    static const int TARGET_LOD_MEDIUM_PIXELS;
    static const int TARGET_LOD_HIGH_PIXELS;
    std::map<LodLevel, std::shared_ptr<Image>> lod_images;
    std::shared_ptr<Image> partial_lod_image;
    LodLevel current_lod_level;
    AdjustmentsParameters parameters;

    std::map<LodLevel, std::shared_ptr<ImageApplyAdjustmentsTask>> apply_adjustments_tasks;
};


#endif //POTOPOTO_IMAGEPREVIEW_H

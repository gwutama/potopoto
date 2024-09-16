#ifndef POTOPOTO_IMAGEPREVIEW_H
#define POTOPOTO_IMAGEPREVIEW_H

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <opencv2/opencv.hpp>

#include "Image.h"


class ImagePreview : public Image {
public:
    ImagePreview(const std::shared_ptr<cv::UMat>& in_image);
    ~ImagePreview();

    void LoadToTexture(GLuint& texture);

private:
    void ResizeOriginalImage(int target_px);

private:
    static const int TARGET_PREVIEW_PIXELS;
    float resize_factor;
};


#endif //POTOPOTO_IMAGEPREVIEW_H

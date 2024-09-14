#ifndef POTOPOTO_IMAGEHISTOGRAM_H
#define POTOPOTO_IMAGEHISTOGRAM_H

#include "Image.h"

class ImageHistogram : public Image {
public:
    ImageHistogram(const std::shared_ptr<cv::UMat>& in_image);
    ~ImageHistogram();

    std::vector<cv::Mat> GetHistogram() const { return bgr_histogram; }

    bool ApplyAdjustments() override;

private:
    void UpdateHistogram();

private:
    std::vector<cv::Mat> bgr_histogram;
};


#endif //POTOPOTO_IMAGEHISTOGRAM_H

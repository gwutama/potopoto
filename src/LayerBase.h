#ifndef POTOPOTO_LAYER_H
#define POTOPOTO_LAYER_H

#include <opencv2/opencv.hpp>
#include <iostream>

class LayerBase {
public:
    virtual void SetImage(std::shared_ptr<cv::UMat> in_image) {
        image_adjusted = in_image;
    }

    virtual std::shared_ptr<cv::UMat> GetAdjustedImage() { return image_adjusted; }

    virtual bool Apply() {
        return ApplyRegion(cv::Point(0, 0), cv::Point(image_adjusted->cols, image_adjusted->rows));
    }

    virtual bool ApplyRegion(const cv::Point& top_left, const cv::Point& bottom_right) {
        if (image_adjusted == nullptr || image_adjusted->empty()) {
            std::cerr << "Layer " << GetName() << " has no image to process." << std::endl;
            return false;
        }

        return Process(top_left, bottom_right);
    }

    virtual bool ParametersHaveChanged() = 0;

protected:
    virtual std::string GetName() = 0;
    virtual bool Process(const cv::Point& top_left, const cv::Point& bottom_right) = 0;

protected:
    std::shared_ptr<cv::UMat> image_adjusted;
};


#endif //POTOPOTO_LAYER_H

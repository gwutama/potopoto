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

    virtual void Apply() {
        if (image_adjusted == nullptr || image_adjusted->empty()) {
            std::cerr << "Layer " << GetName() << " has no image to process." << std::endl;
            return;
        }

        Process();
    }

protected:
    virtual std::string GetName() = 0;
    virtual void Process() = 0;

protected:
    std::shared_ptr<cv::UMat> image_adjusted;
};


#endif //POTOPOTO_LAYER_H

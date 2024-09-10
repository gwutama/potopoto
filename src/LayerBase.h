#ifndef POTOPOTO_LAYER_H
#define POTOPOTO_LAYER_H

#include <opencv2/opencv.hpp>
#include <iostream>

class LayerBase {
public:
    virtual void SetImage(const cv::UMat& in_image) {
        image = in_image.clone();
        adjusted_image = in_image.clone();
    }

    virtual cv::UMat GetAdjustedImage() { return adjusted_image; }

    virtual void Apply() {
        if (image.empty()) {
            std::cerr << "Layer " << GetName() << " has no image to process." << std::endl;
            return;
        }

        Process();
    }

protected:
    virtual std::string GetName() = 0;
    virtual void Process() = 0;

protected:
    cv::UMat image;
    cv::UMat adjusted_image;
};


#endif //POTOPOTO_LAYER_H

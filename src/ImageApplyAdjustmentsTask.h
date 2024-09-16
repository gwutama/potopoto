#ifndef POTOPOTO_IMAGEAPPLYADJUSTMENTSTASK_H
#define POTOPOTO_IMAGEAPPLYADJUSTMENTSTASK_H

#include <opencv2/opencv.hpp>
#include "BackgroundTask.h"
#include "Image.h"


class ImageApplyAdjustmentsTask : public BackgroundTask<bool> {
public:
    ImageApplyAdjustmentsTask(const std::shared_ptr<Image>& image,
                              std::chrono::milliseconds timeout_duration);

private:
    bool Execute() override;

private:
    std::shared_ptr<Image> image;
};


#endif //POTOPOTO_IMAGEAPPLYADJUSTMENTSTASK_H

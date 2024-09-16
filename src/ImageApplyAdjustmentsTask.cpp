#include "ImageApplyAdjustmentsTask.h"


ImageApplyAdjustmentsTask::ImageApplyAdjustmentsTask(const std::shared_ptr<Image>& my_image,
                                                     std::chrono::milliseconds timeout_duration) :
                                                     BackgroundTask<bool>(timeout_duration), image(my_image) {
}


bool ImageApplyAdjustmentsTask::Execute() {
    if (image == nullptr) {
        return false;
    }

    std::cout << "Running image adjustments task" << std::endl;
    bool ok = image->ApplyAdjustments();
    std::cout << "Image adjustments task completed" << std::endl;
    return ok;
}
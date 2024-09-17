#ifndef POTOPOTO_UIUTILS_H
#define POTOPOTO_UIUTILS_H

#include <opencv2/opencv.hpp>
#include <imgui.h>

class UiUtils {
public:
    static cv::Point ImVec2ToCvPoint(const ImVec2& in_point);
};


#endif //POTOPOTO_UIUTILS_H

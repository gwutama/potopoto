#include "UiUtils.h"

cv::Point UiUtils::ImVec2ToCvPoint(const ImVec2& in_point) {
    return {static_cast<int>(in_point.x), static_cast<int>(in_point.y)};
}
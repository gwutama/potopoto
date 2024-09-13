#ifndef POTOPOTO_HISTOGRAM_H
#define POTOPOTO_HISTOGRAM_H

#include <opencv2/opencv.hpp>
#include <vector>

class Histogram {
public:
    Histogram() {}
    void Render(const std::vector<cv::Mat>& histogram_data);
};


#endif //POTOPOTO_HISTOGRAM_H

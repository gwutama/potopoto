#ifndef POTOPOTO_IMAGEREADER_H
#define POTOPOTO_IMAGEREADER_H

#include <string>
#include <opencv2/opencv.hpp>

class ImageReader {
public:
    static bool Open(const std::string& in_filename, std::shared_ptr<cv::UMat> out_image);
};


#endif //POTOPOTO_IMAGEREADER_H

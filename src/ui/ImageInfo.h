#ifndef POTOPOTO_IMAGEINFO_H
#define POTOPOTO_IMAGEINFO_H

#include <map>
#include <string>
#include "../Image.h"

class ImageInfo {
public:
    ImageInfo() {}
    void Render(const std::unordered_map<std::string, std::string>& image_info);
};


#endif //POTOPOTO_IMAGEINFO_H

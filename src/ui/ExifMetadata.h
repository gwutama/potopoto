#ifndef POTOPOTO_EXIFMETADATA_H
#define POTOPOTO_EXIFMETADATA_H

#include <map>
#include <string>

class ExifMetadata {
public:
    ExifMetadata() {}
    void Render(const std::unordered_map<std::string, std::string>& metadata);
};


#endif //POTOPOTO_EXIFMETADATA_H

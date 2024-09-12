#ifndef POTOPOTO_METADATAREADER_H
#define POTOPOTO_METADATAREADER_H

#include <string>
#include <map>
#include <opencv2/opencv.hpp>

class MetadataReader {
public:
    MetadataReader() {}
    ~MetadataReader() {}

    void Load(const std::string& in_filename);
    std::unordered_map<std::string, std::string> GetFileInfo() const { return file_info; }
    std::unordered_map<std::string, std::string> GetExifMetadata() const { return image_exif; }

private:
    void LoadFileInfo(const std::string& filename);
    void LoadExifMetadata(const std::string& filename);

private:
    std::unordered_map<std::string, std::string> file_info;
    std::unordered_map<std::string, std::string> image_exif;
};


#endif //POTOPOTO_METADATAREADER_H

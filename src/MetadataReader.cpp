#include "MetadataReader.h"

#include <exiv2/exiv2.hpp>
#include <sys/stat.h>


void MetadataReader::Load(const std::string& in_filename) {
    LoadFileInfo(in_filename);
    LoadExifMetadata(in_filename);
}


void MetadataReader::LoadFileInfo(const std::string& filename) {
    file_info.clear();

    struct stat file_stat{};

    if (stat(filename.c_str(), &file_stat) == 0) {
        // File name
        file_info.insert(std::make_pair("File Name", filename));

        // File size
        file_info.insert(std::make_pair("File Size", std::to_string(file_stat.st_size) + " b"));

        // File type (extension)
        std::string file_extension = filename.substr(filename.find_last_of(".") + 1);
        file_info.insert(std::make_pair("File Type", file_extension));

        // Creation time
        std::time_t creation_time = file_stat.st_ctime;
        std::stringstream creation_time_str;
        creation_time_str << std::asctime(std::localtime(&creation_time));
        file_info.insert(std::make_pair("Created", creation_time_str.str()));

        // Modification time
        std::time_t modification_time = file_stat.st_mtime;
        std::stringstream modification_time_str;
        modification_time_str << std::asctime(std::localtime(&modification_time));
        file_info.insert(std::make_pair("Modified", modification_time_str.str()));
    } else {
        std::cerr << "Error: Could not retrieve file information for: " << filename << std::endl;
    }
}


void MetadataReader::LoadExifMetadata(const std::string& filename) {
    image_exif.clear();

    try {
        Exiv2::Image::UniquePtr my_image = Exiv2::ImageFactory::open(filename);

        if (!my_image) {
            std::cerr << "Error: Could not open image file for metadata extraction: " << filename << std::endl;
            return;
        }

        my_image->readMetadata();

        Exiv2::ExifData &exifData = my_image->exifData();

        if (exifData.empty()) {
            std::cerr << "No Exif data found in the file: " << filename << std::endl;
            return;
        }

        for (Exiv2::ExifData::const_iterator i = exifData.begin(); i != exifData.end(); ++i) {
            std::string key = i->key();
            std::string value = i->value().toString();
            image_exif.insert(std::make_pair(key, value));
        }
    } catch (Exiv2::Error& e) {
        std::cerr << "Error reading EXIF data: " << e.what() << std::endl;
    }
}

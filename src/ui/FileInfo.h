#ifndef POTOPOTO_FILEINFO_H
#define POTOPOTO_FILEINFO_H

#include <map>
#include <string>

class FileInfo {
public:
    FileInfo() {}
    void Render(const std::unordered_map<std::string, std::string>& metadata);
};


#endif //POTOPOTO_FILEINFO_H

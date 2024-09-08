#ifndef POTOPOTO_IMAGEEDITOR_H
#define POTOPOTO_IMAGEEDITOR_H

#pragma once

#ifdef __APPLE__
#include <OpenGL/gl.h> // Use the OpenGL framework for macOS
#else
#include <GL/gl.h>     // For other platforms, use the standard OpenGL header
#endif

#include "imgui.h"
#include <opencv2/opencv.hpp> // Include OpenCV

enum class ActiveTool { None, Zoom, Hand };

class ImageEditor {
public:
    ImageEditor();
    void Render();               // The main function to render the entire editor
    void LoadImage(const char* filename);
    void SaveImage();

private:
    void RenderImageEditor();    // Render the image editor GUI
    void HandleImageViewer();    // Handle the image viewer's logic
    void RenderToolbar();        // Render the toolbar
    void DisplayImageInfo();      // Display EXIF information
    void CalculateHistogram();   // Calculate and display histogram
    void GetImageInfo();         // Get image information
    void GetFileInfo(const char* filename); // Get file information
    void ExtractExifMetadata(const std::string& filename);
    void DisplayExifMetadata();

    GLuint image_texture;
    int image_width, image_height;
    float zoom;
    ImVec2 image_offset;
    ActiveTool active_tool;
    cv::Mat image; // OpenCV image
    std::vector<std::string> image_info;
    std::vector<std::string> image_exif;
};

#endif //POTOPOTO_IMAGEEDITOR_H

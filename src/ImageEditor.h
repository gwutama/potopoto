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
    void CalculateHistogram(const cv::Mat& image);   // Calculate and display histogram
    void GetImageInfo();         // Get image information
    void GetFileInfo(const char* filename); // Get file information
    void ExtractExifMetadata(const std::string& filename);
    void DisplayExifMetadata();
    void AdjustBrightness(cv::Mat& src, cv::Mat& dst, float brightness);
    void AdjustContrast(cv::Mat& src, cv::Mat& dst, float contrast);
    void UpdateTextureWithImage(const cv::Mat& image);

    GLuint image_texture;
    int image_width, image_height;
    float zoom;
    ActiveTool active_tool;
    cv::Mat original_image; // original image
    cv::Mat adjusted_image;
    std::vector<std::string> image_info;
    std::vector<std::string> image_exif;

    float brightness = 1.0f;
    float contrast = 1.0f;
};

#endif //POTOPOTO_IMAGEEDITOR_H

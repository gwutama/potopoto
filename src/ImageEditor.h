#ifndef POTOPOTO_IMAGEEDITOR_H
#define POTOPOTO_IMAGEEDITOR_H

#pragma once
#include "Image.h"

enum class ActiveTool { None, Zoom, Hand };

class ImageEditor {
public:
    ImageEditor();
    void Render();

private:
    void RenderImageEditor();    // Render the image editor GUI

    void RenderMenubar();
    void HandleLoadImage(const std::string& filename);
    void HandleSaveImage();

    void RenderToolbar();        // Render the toolbar

    void RenderImageViewer(float width, float height);
    void RenderImageAnalysisTabs();
    void RenderImageAdjustments();

    void HandleImageViewer();    // Handle the image viewer's logic
    void RenderImageInfo();      // Display EXIF information
    void RenderHistogram();   // Calculate and display histogram
    void RenderExifMetadata();

private:
    Image image;
    GLuint image_texture;
    float zoom;
    ActiveTool active_tool;
    std::vector<std::string> image_info;
    std::vector<std::string> image_exif;

    float brightness = 1.0f;
    float contrast = 1.0f;
};

#endif //POTOPOTO_IMAGEEDITOR_H

#ifndef POTOPOTO_IMAGEEDITOR_H
#define POTOPOTO_IMAGEEDITOR_H

#pragma once
#include "Image.h"

enum class ActiveTool { Zoom, Hand };

class ImageEditor {
public:
    ImageEditor();
    void Render();

private:
    void RenderMenubar();
    void HandleLoadImage(const std::string& filename);
    void HandleSaveImage();
    void HandleCloseImage();

    void RenderToolbar();

    void RenderImageViewer();
    void RenderImageAnalysisTabs();
    void RenderImageAdjustments();

    void RenderImageInfo();
    void RenderHistogram();
    void RenderExifMetadata();

private:
    Image image;
    GLuint image_texture;
    ActiveTool active_tool;
    float zoom;
    float brightness;
    float contrast;
    float hue;
    float saturation;
};

#endif //POTOPOTO_IMAGEEDITOR_H

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
    bool ImageAdjustmentsHaveChanged();

    void RenderImageInfo();
    void RenderHistogram();
    void RenderExifMetadata();

private:
    Image image;
    GLuint image_texture;
    ActiveTool active_tool;
    float zoom;
    float brightness;
    float last_brightness;
    float contrast;
    float last_contrast;
    float hue;
    float last_hue;
    float saturation;
    float last_saturation;
};

#endif //POTOPOTO_IMAGEEDITOR_H

#ifndef POTOPOTO_IMAGEEDITOR_H
#define POTOPOTO_IMAGEEDITOR_H

#pragma once
#include "Image.h"
#include "MetadataReader.h"

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

    void RenderHistogram();
    void RenderImageInfo();
    void RenderExifMetadata();
    void RenderFileInfo();

private:
    Image image;
    MetadataReader metadata_reader;
    GLuint image_texture;
    ActiveTool active_tool;
    float zoom;
    float brightness;
    float contrast;
    float hue;
    float saturation;
    float value;
};

#endif //POTOPOTO_IMAGEEDITOR_H

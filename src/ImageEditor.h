#ifndef POTOPOTO_IMAGEEDITOR_H
#define POTOPOTO_IMAGEEDITOR_H

#pragma once

#ifdef __APPLE__
#include <OpenGL/gl.h> // Use the OpenGL framework for macOS
#else
#include <GL/gl.h>     // For other platforms, use the standard OpenGL header
#endif

#include "imgui.h"

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
    void DisplayExifInfo();      // Display EXIF information

    GLuint image_texture;
    int image_width, image_height;
    float zoom;
    ImVec2 image_offset;
    ActiveTool active_tool;
};



#endif //POTOPOTO_IMAGEEDITOR_H

#ifndef POTOPOTO_CANVAS_H
#define POTOPOTO_CANVAS_H

#include <SDL_opengl.h>
#include <imgui.h>
#include "Enums.h"
#include "../Image.h"

class Canvas {
public:
    Canvas();
    void SetImage(std::shared_ptr<Image> my_image) { image = my_image; }
    void Reset();
    void Render();
    void SetActiveTool(ActiveTool tool) { active_tool = tool; }
    void UpdateTexture();

private:
    void HandleZoomTool(ImGuiIO& io);
    void HandleHandTool();

private:
    std::shared_ptr<Image> image;
    float zoom;
    GLuint image_texture;
    ActiveTool active_tool;
};


#endif //POTOPOTO_CANVAS_H

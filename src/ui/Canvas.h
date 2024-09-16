#ifndef POTOPOTO_CANVAS_H
#define POTOPOTO_CANVAS_H

#include <SDL_opengl.h>
#include <imgui.h>
#include "Enums.h"
#include "../ImagePreview.h"

class Canvas {
public:
    Canvas();
    ~Canvas() = default;

    void SetImage(std::shared_ptr<ImagePreview> my_image) { image = my_image; }
    void Reset();
    void Render();
    void SetActiveTool(ActiveTool tool) { active_tool = tool; }
    void UpdateTexture();

    std::pair<ImVec2, ImVec2> GetViewableRegion() const;

private:
    void HandleZoomTool(ImGuiIO& io);
    void HandleHandTool();
    ImagePreview::LodLevel GetLodLevelForZoom() const;

private:
    ImVec2 window_size;
    ImVec2 scroll_offset;
    std::shared_ptr<ImagePreview> image;
    float zoom;
    GLuint image_texture;
    ActiveTool active_tool;
    ImagePreview::LodLevel current_lod_level;
};


#endif //POTOPOTO_CANVAS_H

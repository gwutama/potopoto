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

    void SetImagePreview(std::shared_ptr<ImagePreview> my_image_preview) { image_preview = my_image_preview; }
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
    std::shared_ptr<ImagePreview> image_preview;
    float zoom;
    GLuint image_texture;
    ActiveTool active_tool;
    ImagePreview::LodLevel current_lod_level;
};


#endif //POTOPOTO_CANVAS_H

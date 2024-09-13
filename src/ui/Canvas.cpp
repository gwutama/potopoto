#include "Canvas.h"
#include <algorithm>
#include <imgui.h>
#include <limits>


Canvas::Canvas() {
    Reset();
}


void Canvas::Reset() {
    zoom = std::numeric_limits<float>::max();
    glDeleteTextures(1, &image_texture);
    image_texture = 0;
}


void Canvas::Render() {
    if (image == nullptr) {
        return;
    }

    ImGuiIO &io = ImGui::GetIO();

    // Image Viewer Scrollable Region
    ImVec2 image_size = ImVec2(static_cast<float>(image->GetWidth()), static_cast<float>(image->GetHeight()));
    ImVec2 window_size = ImGui::GetContentRegionAvail();

    // Set initial zoom level to fit the image in the view area if zoom is at default
    if (zoom == std::numeric_limits<float>::max()) {
        float zoom_x = (window_size.x - 50) / image_size.x;
        float zoom_y = (window_size.y - 50) / image_size.y;
        zoom = std::min(zoom_x, zoom_y);  // Set zoom to the minimum zoom level that fits the image
    }

    // Adjust image size according to the zoom level
    ImVec2 scaled_image_size = ImVec2(image_size.x * zoom, image_size.y * zoom);

    ImGui::BeginChild("Canvas", window_size, ImGuiChildFlags_Borders,
                      ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);

    // Handle zooming if the Zoom tool is active and the window is hovered
    if (active_tool == ActiveTool::Zoom && ImGui::IsWindowHovered()) {
        HandleZoomTool(io);
    }

    // Drag to pan if the hand tool is active
    if (active_tool == ActiveTool::Hand && ImGui::IsWindowHovered()) {
        HandleHandTool();
    }

    // Calculate offsets to center the image
    float offset_x = std::max(0.0f, (window_size.x - scaled_image_size.x) * 0.5f);
    float offset_y = std::max(0.0f, (window_size.y - scaled_image_size.y) * 0.5f);

    // Set the cursor position to center the image
    ImGui::SetCursorPos(ImVec2(offset_x, offset_y));

    // Render the image
    ImGui::Image((void *)(intptr_t)image_texture, scaled_image_size);

    ImGui::EndChild();
}


void Canvas::HandleZoomTool(ImGuiIO& io) {
    if (io.MouseWheel == 0.0f) {
        return;
    }

    // Calculate the current mouse position relative to the image
    ImVec2 mouse_pos = ImGui::GetMousePos();
    ImVec2 window_pos = ImGui::GetWindowPos();
    ImVec2 window_scroll = ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY());

    // Mouse position relative to the top-left corner of the image
    ImVec2 mouse_pos_relative_to_image = ImVec2(
            (mouse_pos.x - window_pos.x + window_scroll.x) / zoom,
            (mouse_pos.y - window_pos.y + window_scroll.y) / zoom
    );

    float zoom_factor = 1.02f; // Zoom speed factor
    zoom = (io.MouseWheel > 0.0f) ? zoom * zoom_factor : zoom / zoom_factor;
    zoom = std::clamp(zoom, 0.1f, 10.0f);
    io.WantCaptureMouse = true;

    // Calculate new scroll offsets to keep mouse-centered point at the same location
    ImVec2 new_scroll_offset;
    new_scroll_offset.x = (mouse_pos_relative_to_image.x * zoom) - (mouse_pos.x - window_pos.x);
    new_scroll_offset.y = (mouse_pos_relative_to_image.y * zoom) - (mouse_pos.y - window_pos.y);

    ImGui::SetScrollX(new_scroll_offset.x);
    ImGui::SetScrollY(new_scroll_offset.y);
}


void Canvas::HandleHandTool() {
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        ImVec2 drag_delta = ImGui::GetMouseDragDelta();
        ImGui::SetScrollX(ImGui::GetScrollX() - drag_delta.x);
        ImGui::SetScrollY(ImGui::GetScrollY() - drag_delta.y);
        ImGui::ResetMouseDragDelta();
    }
}


void Canvas::UpdateTexture() {
    if (image == nullptr) {
        return;
    }

    image->LoadToTexture(image_texture);
}

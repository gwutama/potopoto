#include "ImageEditor.h"
#include "ImageLoader.h"
#include "Toolbar.h"
#include "UiComponents.h"
#include "tinyfiledialogs.h"
#include <iostream>
#include <algorithm> // Include for std::clamp and std::max
#include <SDL.h>     // Include SDL for SDL_Event

ImageEditor::ImageEditor()
        : image_texture(0), image_width(0), image_height(0), zoom(1.0f), image_offset(ImVec2(0.0f, 0.0f)), active_tool(ActiveTool::Zoom) {}

void ImageEditor::Render() {
    RenderImageEditor();
}

void ImageEditor::RenderImageEditor() {
    static float brightness = 1.0f;
    static float contrast = 1.0f;
    static int filter = 0; // 0: None, 1: Blur, 2: Sharpen, etc.

    // Main Menu Bar
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::MenuItem("Open")) {
            const char* filename = tinyfd_openFileDialog(
                    "Open Image", "", 0, NULL, NULL, 0);
            if (filename) {
                LoadImage(filename);
            }
        }

        if (ImGui::MenuItem("Save")) {
            SaveImage();
        }

        if (ImGui::MenuItem("Close")) {
            SDL_Event quit_event;
            quit_event.type = SDL_QUIT;
            SDL_PushEvent(&quit_event);
        }

        ImGui::EndMainMenuBar();
    }

    // Adjust for the menu bar height
    ImVec2 main_window_pos = ImVec2(0, ImGui::GetFrameHeight());
    ImVec2 main_window_size = ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y - ImGui::GetFrameHeight());

    ImGui::SetNextWindowPos(main_window_pos);
    ImGui::SetNextWindowSize(main_window_size);

    ImGui::Begin("Main Layout", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

    RenderToolbar();

    ImGui::SameLine();

    ImVec2 available_size = ImGui::GetContentRegionAvail();
    float left_pane_width = available_size.x * 0.75f;
    float right_pane_width = available_size.x - left_pane_width;

    ImGui::BeginChild("LeftPane", ImVec2(left_pane_width, available_size.y), false);
    if (image_texture) {
        HandleImageViewer();
    } else {
        ImGui::Text("No image loaded");
    }
    ImGui::EndChild();

    ImGui::SameLine();
    ImGui::BeginChild("RightPane", ImVec2(right_pane_width, available_size.y), false);

    ImGui::BeginChild("Tabs", ImVec2(0, available_size.y * 0.3f), true);
    if (ImGui::BeginTabBar("TabBar")) {
        if (ImGui::BeginTabItem("Histogram")) {
            ImGui::Text("Histogram data goes here");
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Image Info")) {
            DisplayExifInfo();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::EndChild();

    ImGui::BeginChild("Adjustments", ImVec2(0, 0), true);
    ImGui::Text("Adjustments");
    ImGui::SliderFloat("Brightness", &brightness, 0.0f, 2.0f);
    ImGui::SliderFloat("Contrast", &contrast, 0.0f, 2.0f);

    const char* filters[] = { "None", "Blur", "Sharpen" };
    ImGui::Combo("Filter", &filter, filters, IM_ARRAYSIZE(filters));
    ImGui::EndChild();

    ImGui::EndChild();
    ImGui::End();
}

void ImageEditor::LoadImage(const char* filename) {
    image_texture = LoadImageFromFile(filename, &image_width, &image_height);
}

void ImageEditor::SaveImage() {
    // Implement save functionality
}

void ImageEditor::HandleImageViewer() {
    ImGuiIO& io = ImGui::GetIO();

    // Image Viewer Scrollable Region
    ImVec2 image_size = ImVec2(image_width * zoom, image_height * zoom);
    ImVec2 window_size = ImGui::GetContentRegionAvail();

    ImGui::BeginChild("Image Viewer", window_size, false, ImGuiWindowFlags_HorizontalScrollbar);

    // Handle zooming if the Zoom tool is active and the window is hovered
    if (active_tool == ActiveTool::Zoom && ImGui::IsWindowHovered()) {
        if (io.MouseWheel != 0.0f) {
            float zoom_factor = 1.1f; // Zoom speed factor
            zoom = (io.MouseWheel > 0.0f) ? zoom * zoom_factor : zoom / zoom_factor;
            zoom = std::clamp(zoom, 0.1f, 10.0f);
            io.WantCaptureMouse = true;
        }
    }

    // Drag to pan if the hand tool is active
    if (active_tool == ActiveTool::Hand && ImGui::IsWindowHovered()) {
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            ImVec2 drag_delta = ImGui::GetMouseDragDelta();
            ImGui::SetScrollX(ImGui::GetScrollX() - drag_delta.x);
            ImGui::SetScrollY(ImGui::GetScrollY() - drag_delta.y);
            ImGui::ResetMouseDragDelta();
        }
    }

    // Render the image
    if (image_texture) {
        ImGui::Image((void*)(intptr_t)image_texture, image_size);
    } else {
        ImGui::Text("No image loaded");
    }

    ImGui::EndChild();
}

void ImageEditor::RenderToolbar() {
    RenderToolbarUI(active_tool);
}

void ImageEditor::DisplayExifInfo() {
    ImGui::Text("Size: 1920x1080");
    ImGui::Text("Date Time: 2024-09-01 12:00:00");
    ImGui::Text("Camera: Canon EOS 5D Mark IV");
    ImGui::Text("Focal Length: 50mm");
    ImGui::Text("Aperture: f/1.8");
    ImGui::Text("ISO: 100");
}

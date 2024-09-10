#include "ImageEditor.h"
#include <tinyfiledialogs.h>
#include <imgui.h>
#include <iostream>
#include <algorithm>
#include <ctime>
#include <SDL.h>
#include <opencv2/opencv.hpp>
#include <implot.h>

ImageEditor::ImageEditor() {
    HandleCloseImage(); // = reset
}


void ImageEditor::Render() {
    RenderMenubar();

    // Adjust for the menu bar height
    ImVec2 main_window_pos = ImVec2(0, ImGui::GetFrameHeight());
    ImVec2 main_window_size = ImVec2(ImGui::GetIO().DisplaySize.x,ImGui::GetIO().DisplaySize.y - ImGui::GetFrameHeight());

    ImGui::SetNextWindowPos(main_window_pos);
    ImGui::SetNextWindowSize(main_window_size);

    ImGui::Begin("MainLayout", NULL,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                 ImGuiWindowFlags_NoTitleBar);

    if (!image.IsOpen()) {
        ImGui::BeginDisabled();
    }

    RenderToolbar();

    ImGui::SameLine();

    ImVec2 available_size = ImGui::GetContentRegionAvail();
    float right_pane_width = 400;
    float left_pane_width = available_size.x - right_pane_width;

    // Remove decoration and scrollbars from the LeftPane
    ImGui::BeginChild("LeftPane", ImVec2(left_pane_width, available_size.y), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    RenderImageViewer();
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("RightPane", ImVec2(right_pane_width, available_size.y), false);
    RenderImageAnalysisTabs();
    RenderImageAdjustments();
    ImGui::EndChild();

    if (!image.IsOpen()) {
        ImGui::EndDisabled();
    }

    ImGui::End();
}


void ImageEditor::RenderMenubar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::MenuItem("Open")) {
            const char *filename = tinyfd_openFileDialog("Open Image", "", 0,
                                                         NULL, NULL, 0);
            if (filename) {
                HandleLoadImage(filename);
            }
        }

        if (ImGui::MenuItem("Save")) {
            HandleSaveImage();
        }

        if (ImGui::MenuItem("Close")) {
            HandleCloseImage();
        }

        if (ImGui::MenuItem("Exit")) {
            SDL_Event quit_event;
            quit_event.type = SDL_QUIT;
            SDL_PushEvent(&quit_event);
        }

        ImGui::EndMainMenuBar();
    }
}


void ImageEditor::HandleLoadImage(const std::string &filename) {
    if (!image.Open(filename)) {
        std::cerr << "Error: Could not load image file: " << filename << std::endl;
        return;
    }

    image.LoadToTexture(image_texture);
}


void ImageEditor::HandleSaveImage() {
    // Implement save functionality
}


void ImageEditor::HandleCloseImage() {
    image.Close();
    glDeleteTextures(1, &image_texture);
    image_texture = 0;
    active_tool = ActiveTool::Hand;
    zoom = 1.0f;
    brightness = 1.0f;
    contrast = 1.0f;
    hue = 0.0f;
    saturation = 1.0f;
}


void ImageEditor::RenderToolbar() {
    ImGui::BeginChild("Toolbar", ImVec2(50, 0), false);

    if (ImGui::Selectable("Zoom", active_tool == ActiveTool::Zoom)) {
        active_tool = ActiveTool::Zoom;
    }

    if (ImGui::Selectable("Hand", active_tool == ActiveTool::Hand)) {
        active_tool = ActiveTool::Hand;
    }

    ImGui::EndChild();
}


void ImageEditor::RenderImageViewer() {
    if (!image.IsOpen()) {
        return;
    }

    ImGuiIO &io = ImGui::GetIO();

    // Image Viewer Scrollable Region
    ImVec2 image_size = ImVec2(static_cast<float>(image.GetWidth()) * zoom, static_cast<float>(image.GetHeight()) * zoom);
    ImVec2 window_size = ImGui::GetContentRegionAvail();

    ImGui::BeginChild("ImageViewer", window_size, ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);

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

    // Adjust brightness and contrast
    image.AdjustBrightness(brightness);
    image.AdjustContrast(contrast);
    image.AdjustHue(hue);
    image.AdjustSaturation(saturation);

    // Update the texture with the adjusted image
    image.LoadToTexture(image_texture);

    // Render the adjusted image
    ImGui::Image((void *) (intptr_t) image_texture, image_size);

    ImGui::EndChild();

    // Recalculate and display the histogram
    RenderHistogram();
}


void ImageEditor::RenderImageAnalysisTabs() {
    ImGui::BeginChild("Tabs", ImVec2(0, 200), true);

    if (ImGui::BeginTabBar("TabBar")) {
        if (ImGui::BeginTabItem("Histogram")) {
            // Disable mouse interactions
            ImGui::BeginDisabled();
            RenderHistogram();
            ImGui::EndDisabled();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Info")) {
            RenderImageInfo();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("EXIF")) {
            RenderExifMetadata();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::EndChild();
}


void ImageEditor::RenderImageAdjustments() {
    ImGui::BeginChild("Adjustments", ImVec2(0, 0), true);
    ImGui::Text("Adjustments");
    ImGui::SliderFloat("Brightness", &brightness, 0.0f, 2.0f);
    ImGui::SliderFloat("Contrast", &contrast, 0.0f, 2.0f);
    ImGui::SliderFloat("Hue", &hue, 0.0f, 360.0f);
    ImGui::SliderFloat("Saturation", &saturation, 0.0f, 2.0f);
    ImGui::EndChild();
}


void ImageEditor::RenderImageInfo() {
    if (ImGui::BeginTable("ImageInfoTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Property");
        ImGui::TableSetupColumn("Value");
        ImGui::TableHeadersRow();

        for (const auto &info: image.GetImageInfo()) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%s", info.first.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", info.second.c_str());
        }
        ImGui::EndTable();
    }
}


void ImageEditor::RenderHistogram() {
    std::vector<cv::Mat> hist = image.CalculateNormalizedHistogram();

    if (hist.size() != 3) { // BGR channels
        return;
    }

    int hist_size = 256;

    // Display the histogram using ImGui (ImPlot must be initialized before using this)
    ImVec2 plot_size = ImGui::GetContentRegionAvail();

    if (ImPlot::BeginPlot("HistogramPlot", nullptr, nullptr, plot_size, ImPlotFlags_CanvasOnly)) {
        ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_NoTickLabels);
        ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_NoTickLabels);

        ImPlot::PushStyleColor(ImPlotCol_Line, IM_COL32(0, 0, 255, 255)); // Blue
        ImPlot::PlotLine("Blue", hist.at(0).ptr<float>(), hist_size);
        ImPlot::PopStyleColor();

        ImPlot::PushStyleColor(ImPlotCol_Line, IM_COL32(0, 255, 0, 255)); // Green
        ImPlot::PlotLine("Green", hist.at(1).ptr<float>(), hist_size);
        ImPlot::PopStyleColor();

        ImPlot::PushStyleColor(ImPlotCol_Line, IM_COL32(255, 0, 0, 255)); // Red
        ImPlot::PlotLine("Red", hist.at(2).ptr<float>(), hist_size);
        ImPlot::PopStyleColor();

        ImPlot::EndPlot();
    }
}


void ImageEditor::RenderExifMetadata() {
    if (ImGui::BeginTable("ExifMetadataTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Property");
        ImGui::TableSetupColumn("Value");
        ImGui::TableHeadersRow();

        for (const auto &info: image.GetExifMetadata()) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%s", info.first.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", info.second.c_str());
        }
        ImGui::EndTable();
    }
}

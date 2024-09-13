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

    ImGui::Begin("MainLayout", nullptr, ImGuiWindowFlags_NoDecoration);

    if (!image.IsOpen()) {
        ImGui::BeginDisabled();
    }

    RenderToolbar();

    ImGui::SameLine();

    ImVec2 available_size = ImGui::GetContentRegionAvail();
    float right_pane_width = 400;
    float left_pane_width = available_size.x - right_pane_width - 8; // magic number

    // Remove decoration and scrollbars from the LeftPane
    ImGui::BeginChild("LeftPane", ImVec2(left_pane_width, available_size.y), ImGuiChildFlags_None,
                      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar);
    RenderImageViewer();
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("RightPane", ImVec2(right_pane_width, available_size.y));
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
                HandleOpenImage(filename);
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


void ImageEditor::HandleOpenImage(const std::string &filename) {
    HandleCloseImage();

    if (!image.Open(filename)) {
        std::cerr << "Error: Could not load image file: " << filename << std::endl;
        return;
    }

    metadata_reader.Load(filename);
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
    zoom = std::numeric_limits<float>::max();
    brightness = LayerBrightnessContrast::DEFAULT_BRIGHTNESS;
    contrast = LayerBrightnessContrast::DEFAULT_CONTRAST;
    hue = LayerHueSaturationValue::DEFAULT_HUE;
    saturation = LayerHueSaturationValue::DEFAULT_SATURATION;
    value = LayerHueSaturationValue::DEFAULT_VALUE;
}


void ImageEditor::RenderToolbar() {
    ImGui::BeginChild("Toolbar", ImVec2(50, 100), ImGuiChildFlags_Borders);

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
    ImVec2 image_size = ImVec2(static_cast<float>(image.GetWidth()), static_cast<float>(image.GetHeight()));
    ImVec2 window_size = ImGui::GetContentRegionAvail();

    // Set initial zoom level to fit the image in the view area if zoom is at default
    if (zoom == std::numeric_limits<float>::max()) {
        float zoom_x = (window_size.x - 50) / image_size.x;
        float zoom_y = (window_size.y - 50) / image_size.y;
        zoom = std::min(zoom_x, zoom_y);  // Set zoom to the minimum zoom level that fits the image
    }

    // Adjust image size according to the zoom level
    ImVec2 scaled_image_size = ImVec2(image_size.x * zoom, image_size.y * zoom);

    ImGui::BeginChild("ImageViewer", window_size, ImGuiChildFlags_Borders,
                      ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar);

    // Calculate the current mouse position relative to the image
    ImVec2 mouse_pos = ImGui::GetMousePos();
    ImVec2 window_pos = ImGui::GetWindowPos();
    ImVec2 window_scroll = ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY());

    // Mouse position relative to the top-left corner of the image
    ImVec2 mouse_pos_relative_to_image = ImVec2(
            (mouse_pos.x - window_pos.x + window_scroll.x) / zoom,
            (mouse_pos.y - window_pos.y + window_scroll.y) / zoom
    );

    // Handle zooming if the Zoom tool is active and the window is hovered
    if (active_tool == ActiveTool::Zoom && ImGui::IsWindowHovered()) {
        if (io.MouseWheel != 0.0f) {
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

    // Image adjustments
    image.AdjustBrightness(brightness);
    image.AdjustContrast(contrast);
    image.AdjustHue(hue);
    image.AdjustSaturation(saturation);
    image.AdjustValue(value);

    bool is_image_adjusted = image.ApplyAdjustments();

    if (is_image_adjusted) {
        // Update the texture with the adjusted image
        image.LoadToTexture(image_texture);
    }

    // Calculate offsets to center the image
    float offset_x = std::max(0.0f, (window_size.x - scaled_image_size.x) * 0.5f);
    float offset_y = std::max(0.0f, (window_size.y - scaled_image_size.y) * 0.5f);

    // Set the cursor position to center the image
    ImGui::SetCursorPos(ImVec2(offset_x, offset_y));

    // Render the image
    ImGui::Image((void *)(intptr_t)image_texture, scaled_image_size);

    ImGui::EndChild();

    if (is_image_adjusted) {
        RenderHistogram();
    }
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

        if (ImGui::BeginTabItem("Image")) {
            RenderImageInfo();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("EXIF")) {
            RenderExifMetadata();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("File")) {
            RenderFileInfo();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::EndChild();
}


void ImageEditor::RenderImageAdjustments() {
    ImGui::BeginChild("Adjustments", ImVec2(0, 0), true);
    ImGui::Text("Adjustments");
    ImGui::SliderFloat("Brightness", &brightness, 0.0f, 2.0f, "%.2f");
    ImGui::SliderFloat("Contrast", &contrast, 0.0f, 2.0f, "%.2f");
    ImGui::SliderFloat("Hue", &hue, 0.0f, 180.0f, "%.0f");
    ImGui::SliderFloat("Saturation", &saturation, -255.0f, 255.0f, "%.0f");
    ImGui::SliderFloat("Value", &value, 0.0f, 255.0f, "%.0f");
    ImGui::EndChild();
}


void ImageEditor::RenderHistogram() {
    std::vector<cv::Mat> hist = image.GetHistogram();

    if (hist.size() != 3) { // BGR channels
        return;
    }

    int hist_size = 256;

    // Display the histogram using ImGui (ImPlot must be initialized before using this)
    ImVec2 plot_size = ImGui::GetContentRegionAvail();

    if (ImPlot::BeginPlot("HistogramPlot", nullptr, nullptr, plot_size, ImPlotFlags_CanvasOnly | ImPlotFlags_NoFrame)) {
        ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_NoDecorations);
        ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_NoDecorations);

        ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(0.4, 0.4, 1, 1.0f)); // Blue
        ImPlot::PlotLine("Blue", hist.at(0).ptr<float>(), hist_size, 1, 0, ImPlotLineFlags_None);
        ImPlot::PopStyleColor();

        ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(0.4, 1, 0.4, 1.0f)); // Green
        ImPlot::PlotLine("Green", hist.at(1).ptr<float>(), hist_size, 1, 0, ImPlotLineFlags_None);
        ImPlot::PopStyleColor();

        ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(1, 0.4, 0.4, 1.0f)); // Red
        ImPlot::PlotLine("Red", hist.at(2).ptr<float>(), hist_size, 1, 0, ImPlotLineFlags_None);
        ImPlot::PopStyleColor();

        ImPlot::EndPlot();
    }
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


void ImageEditor::RenderExifMetadata() {
    if (ImGui::BeginTable("ExifMetadataTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Property");
        ImGui::TableSetupColumn("Value");
        ImGui::TableHeadersRow();

        for (const auto &info: metadata_reader.GetExifMetadata()) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%s", info.first.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", info.second.c_str());
        }

        ImGui::EndTable();
    }
}


void ImageEditor::RenderFileInfo() {
    if (ImGui::BeginTable("FileInfoTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Property");
        ImGui::TableSetupColumn("Value");
        ImGui::TableHeadersRow();

        for (const auto &info: metadata_reader.GetFileInfo()) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%s", info.first.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", info.second.c_str());
        }

        ImGui::EndTable();
    }
}

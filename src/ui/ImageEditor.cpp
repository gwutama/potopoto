#include "ImageEditor.h"
#include <imgui.h>
#include <iostream>
#include <algorithm>


ImageEditor::ImageEditor() {
    HandleCloseImage(); // = reset

    image = std::make_shared<Image>();
    canvas.SetImage(image);

    // connect signals to slots
    menubar.OpenSignal().connect(&ImageEditor::HandleOpenImage, this);
    menubar.SaveSignal().connect(&ImageEditor::HandleSaveImage, this);
    menubar.CloseSignal().connect(&ImageEditor::HandleCloseImage, this);

    image_adjustments.ParametersChanged().connect(&ImageEditor::OnImageAdjustmentsParametersChanged, this);
}


void ImageEditor::Render() {
    menubar.Render();

    // Adjust for the menu bar height
    ImVec2 main_window_pos = ImVec2(0, ImGui::GetFrameHeight());
    ImVec2 main_window_size = ImVec2(ImGui::GetIO().DisplaySize.x,ImGui::GetIO().DisplaySize.y - ImGui::GetFrameHeight());

    ImGui::SetNextWindowPos(main_window_pos);
    ImGui::SetNextWindowSize(main_window_size);

    ImGui::Begin("MainLayout", nullptr, ImGuiWindowFlags_NoDecoration);

    if (image == nullptr || !image->IsOpen()) {
        ImGui::BeginDisabled();
    }

    toolbar.Render();

    ImGui::SameLine();

    ImVec2 available_size = ImGui::GetContentRegionAvail();
    float right_pane_width = 400;
    float left_pane_width = available_size.x - right_pane_width - 8; // magic number

    // Remove decoration and scrollbars from the LeftPane
    ImGui::BeginChild("LeftPane", ImVec2(left_pane_width, available_size.y), ImGuiChildFlags_None,
                      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar);
    canvas.Render();
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("RightPane", ImVec2(right_pane_width, available_size.y));
    RenderImageAnalysisTabs();
    image_adjustments.Render();
    ImGui::EndChild();

    if (image == nullptr || !image->IsOpen()) {
        ImGui::EndDisabled();
    }

    ImGui::End();
}


void ImageEditor::HandleOpenImage(const std::string &filename) {
    HandleCloseImage();

    if (!image->Open(filename)) {
        std::cerr << "Error: Could not load image file: " << filename << std::endl;
        return;
    }

    metadata_reader.Load(filename);
    canvas.UpdateTexture();
}


void ImageEditor::HandleSaveImage() {
    // Implement save functionality
}


void ImageEditor::HandleCloseImage() {
    if (image != nullptr) {
        image->Close();
    }

    toolbar.Reset();
    image_adjustments.Reset();
    canvas.Reset();
}


void ImageEditor::OnImageAdjustmentsParametersChanged(const ImageAdjustmentsParameters &parameters) {
    image->AdjustBrightness(parameters.GetBrightness());
    image->AdjustContrast(parameters.GetContrast());
    image->AdjustHue(parameters.GetHue());
    image->AdjustSaturation(parameters.GetSaturation());
    image->AdjustValue(parameters.GetValue());
    image->ApplyAdjustments();
    canvas.UpdateTexture(); // Update the texture with the adjusted image
}


void ImageEditor::RenderImageAnalysisTabs() {
    ImGui::BeginChild("Tabs", ImVec2(0, 200), true);

    bool has_image = (image != nullptr && image->IsOpen());

    if (ImGui::BeginTabBar("TabBar")) {
        if (ImGui::BeginTabItem("Histogram")) {
            if (has_image) {
                // Disable mouse interactions
                ImGui::BeginDisabled();
                histogram.Render(image->GetHistogram());
                ImGui::EndDisabled();
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Image")) {
            if (has_image) {
                image_info.Render(image->GetImageInfo());
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("EXIF")) {
            if (has_image) {
                exif_metadata.Render(metadata_reader.GetExifMetadata());
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("File")) {
            if (has_image) {
                file_info.Render(metadata_reader.GetFileInfo());
            }

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::EndChild();
}

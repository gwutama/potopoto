#include "ImageEditor.h"
#include <imgui.h>
#include <iostream>
#include <algorithm>
#include "../ImageReader.h"
#include "UiUtils.h"


ImageEditor::ImageEditor() {
    OnCloseClicked(); // = reset

    canvas.SetActiveTool(ActiveTool::Zoom);

    // connect signals to slots
    menubar.OpenClicked().connect(&ImageEditor::OnOpenClicked, this);
    menubar.SaveClicked().connect(&ImageEditor::OnSaveClicked, this);
    menubar.CloseClicked().connect(&ImageEditor::OnCloseClicked, this);

    toolbar.ActiveToolChanged().connect(&Canvas::SetActiveTool, &canvas);

    image_adjustments.ParametersChanged().connect(&ImageEditor::OnImageAdjustmentsParametersChanged, this);
    image_adjustments.MouseReleased().connect(&ImageEditor::OnImageAdjustmentsMouseReleased, this);
}


ImageEditor::~ImageEditor() {
    OnCloseClicked();
}


void ImageEditor::Render() {
    menubar.Render();

    // Adjust for the menu bar height
    ImVec2 main_window_pos = ImVec2(0, ImGui::GetFrameHeight());
    ImVec2 main_window_size = ImVec2(ImGui::GetIO().DisplaySize.x,ImGui::GetIO().DisplaySize.y - ImGui::GetFrameHeight());

    ImGui::SetNextWindowPos(main_window_pos);
    ImGui::SetNextWindowSize(main_window_size);

    ImGui::Begin("MainLayout", nullptr, ImGuiWindowFlags_NoDecoration);

    if (image_preview == nullptr) {
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

    if (image_preview == nullptr) {
        ImGui::EndDisabled();
    }

    ImGui::End();
}


void ImageEditor::OnOpenClicked(const std::string &filename) {
    OnCloseClicked();

    std::shared_ptr<cv::UMat> my_image = std::make_shared<cv::UMat>();

    if (!ImageReader::Open(filename, my_image)) {
        std::cerr << "Error: Could not load image file: " << filename << std::endl;
        return;
    }

    auto image = std::make_shared<Image>(my_image);
    image_preview = std::make_shared<ImagePreview>(image);
    canvas.SetImagePreview(image_preview);

    metadata_reader.Load(filename);
    canvas.UpdateTexture();

    image_histogram = std::make_shared<ImageHistogram>(my_image);
}


void ImageEditor::OnSaveClicked() {
    // Implement save functionality
}


void ImageEditor::OnCloseClicked() {
    if (image_preview != nullptr) {
        image_preview.reset();
        image_preview = nullptr;
    }

    toolbar.Reset();
    image_adjustments.Reset();
    canvas.Reset();
}


void ImageEditor::OnImageAdjustmentsParametersChanged(const AdjustmentsParameters &parameters) {
#pragma omp parallel sections
    {
#pragma omp section
        {
            // Improve performance by only applying adjustments to the visible region
            auto region = canvas.GetViewableRegion();
            image_preview->AdjustParameters(parameters);

            cv::Point top_left = UiUtils::ImVec2ToCvPoint(region.first);
            cv::Point bottom_right = UiUtils::ImVec2ToCvPoint(region.second);
            cv::Rect region_cv(top_left, bottom_right);

            image_preview->ApplyAdjustmentsForPreviewRegion(region_cv);
            canvas.UpdateTexture(); // Update the texture with the adjusted image
        }
#pragma omp section
        {
            image_histogram->AdjustParameters(parameters);
            image_histogram->ApplyAdjustments();
        }
    }
}


void ImageEditor::RenderImageAnalysisTabs() {
    ImGui::BeginChild("Tabs", ImVec2(0, 200), true);

    if (ImGui::BeginTabBar("TabBar")) {
        if (ImGui::BeginTabItem("Histogram")) {
            if (image_preview != nullptr) {
                // Disable mouse interactions
                ImGui::BeginDisabled();
                histogram.Render(image_histogram->GetHistogram());
                ImGui::EndDisabled();
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Image")) {
            if (image_preview != nullptr) {
                image_info.Render(image_preview->GetImageCurrentLod()->GetImageInfo());
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("EXIF")) {
            if (image_preview != nullptr) {
                exif_metadata.Render(metadata_reader.GetExifMetadata());
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("File")) {
            if (image_preview != nullptr) {
                file_info.Render(metadata_reader.GetFileInfo());
            }

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::EndChild();
}


void ImageEditor::OnImageAdjustmentsMouseReleased() {
    image_preview->ApplyAdjustmentsForAllLodsAsync();
}
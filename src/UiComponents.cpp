#include "UiComponents.h"
#include "imgui.h"

void RenderImageInfo() {
    ImGui::BeginChild("Tabs", ImVec2(0, ImGui::GetContentRegionAvail().y * 0.3f), true);
    if (ImGui::BeginTabBar("TabBar")) {
        if (ImGui::BeginTabItem("Histogram")) {
            ImGui::Text("Histogram data goes here");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Image Info")) {
            ImGui::Text("Size: 1920x1080");
            ImGui::Text("Date Time: 2024-09-01 12:00:00");
            ImGui::Text("Camera: Canon EOS 5D Mark IV");
            ImGui::Text("Focal Length: 50mm");
            ImGui::Text("Aperture: f/1.8");
            ImGui::Text("ISO: 100");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::EndChild();
}

void RenderAdjustments() {
    static float brightness = 1.0f;
    static float contrast = 1.0f;
    static int filter = 0;

    ImGui::BeginChild("Adjustments", ImVec2(0, 0), true);
    ImGui::Text("Adjustments");
    ImGui::SliderFloat("Brightness", &brightness, 0.0f, 2.0f);
    ImGui::SliderFloat("Contrast", &contrast, 0.0f, 2.0f);
    const char* filters[] = { "None", "Blur", "Sharpen" };
    ImGui::Combo("Filter", &filter, filters, IM_ARRAYSIZE(filters));
    ImGui::EndChild();
}

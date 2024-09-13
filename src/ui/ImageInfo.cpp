#include "ImageInfo.h"
#include <imgui.h>

void ImageInfo::Render(const std::unordered_map<std::string, std::string>& image_info) {
    if (ImGui::BeginTable("ImageInfoTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Property");
        ImGui::TableSetupColumn("Value");
        ImGui::TableHeadersRow();

        for (const auto &info: image_info) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%s", info.first.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", info.second.c_str());
        }

        ImGui::EndTable();
    }
}
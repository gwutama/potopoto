#include "FileInfo.h"
#include <imgui.h>


void FileInfo::Render(const std::unordered_map<std::string, std::string>& metadata) {
    if (ImGui::BeginTable("FileInfoTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Property");
        ImGui::TableSetupColumn("Value");
        ImGui::TableHeadersRow();

        for (const auto &info: metadata) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%s", info.first.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", info.second.c_str());
        }

        ImGui::EndTable();
    }}
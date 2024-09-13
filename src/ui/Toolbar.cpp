#include "Toolbar.h"
#include <imgui.h>


Toolbar::Toolbar() : active_tool(ActiveTool::Zoom) {
}


void Toolbar::Render() {
    ImGui::BeginChild("Toolbar", ImVec2(50, 100), ImGuiChildFlags_Borders);

    if (ImGui::Selectable("Zoom", active_tool == ActiveTool::Zoom)) {
        active_tool = ActiveTool::Zoom;
        active_tool_changed(active_tool);
    }

    if (ImGui::Selectable("Hand", active_tool == ActiveTool::Hand)) {
        active_tool = ActiveTool::Hand;
        active_tool_changed(active_tool);
    }

    ImGui::EndChild();
}


void Toolbar::Reset() {
    active_tool = ActiveTool::Zoom;
}
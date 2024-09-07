#include "Toolbar.h"

void RenderToolbarUI(ActiveTool& active_tool) {
    ImGui::BeginChild("Toolbar", ImVec2(50, 0), false);
    if (ImGui::Selectable("Zoom", active_tool == ActiveTool::Zoom)) {
        active_tool = ActiveTool::Zoom;
    }
    if (ImGui::Selectable("Hand", active_tool == ActiveTool::Hand)) {
        active_tool = ActiveTool::Hand;
    }
    ImGui::EndChild();
}

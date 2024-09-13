#ifndef POTOPOTO_TOOLBAR_H
#define POTOPOTO_TOOLBAR_H

#include <sigslot/signal.hpp>
#include "Enums.h"

class Toolbar {
public:
    Toolbar();
    void Render();
    void Reset();

    ActiveTool GetActiveTool() const { return active_tool; }

    sigslot::signal<>& ZoomToolClicked() { return zoom_tool_clicked; }
    sigslot::signal<>& HandToolClicked() { return hand_tool_clicked; }

private:
    ActiveTool active_tool;
    sigslot::signal<> zoom_tool_clicked;
    sigslot::signal<> hand_tool_clicked;
};


#endif //POTOPOTO_TOOLBAR_H

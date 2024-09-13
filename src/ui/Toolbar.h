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
    sigslot::signal<ActiveTool>& ActiveToolChanged() { return active_tool_changed; }

private:
    ActiveTool active_tool;
    sigslot::signal<ActiveTool> active_tool_changed;
};


#endif //POTOPOTO_TOOLBAR_H

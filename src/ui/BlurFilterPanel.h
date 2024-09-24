#ifndef BLURFILTERPANEL_H
#define BLURFILTERPANEL_H

#include "AbstractFilterPanel.h"

class BlurFilterPanel : public AbstractFilterPanel {
public:
    BlurFilterPanel(wxWindow* parent);

protected:
    void AddFilterControls(wxWindow* window) override;
    std::string GetName() override { return "Blur"; }
};

#endif // BLURFILTERPANEL_H
#ifndef POTOPOTO_IMAGEADJUSTMENTSPANEL_H
#define POTOPOTO_IMAGEADJUSTMENTSPANEL_H

#include <wx/wx.h>
#include <wx/notebook.h>
#include "LayerAdjustmentsPanel.h"

class ImageAdjustmentsPanel : public wxPanel {
public:
    ImageAdjustmentsPanel(wxWindow* parent);
    void Reset();

private:
    wxNotebook* imageAdjustmentsTabs;
    LayerAdjustmentsPanel* layerAdjustmentsPanel;

    void CreateTabs();
};


#endif //POTOPOTO_IMAGEADJUSTMENTSPANEL_H

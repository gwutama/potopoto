#ifndef POTOPOTO_IMAGEADJUSTMENTSPANEL_H
#define POTOPOTO_IMAGEADJUSTMENTSPANEL_H

#include <wx/wx.h>
#include <wx/notebook.h>

class ImageAdjustmentsPanel : public wxPanel {
public:
    ImageAdjustmentsPanel(wxWindow* parent);

private:
    wxNotebook* imageAdjustmentsTabs;

    void CreateTabs();
};


#endif //POTOPOTO_IMAGEADJUSTMENTSPANEL_H

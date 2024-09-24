#ifndef FILTERADJUSTMENTSPANEL_H
#define FILTERADJUSTMENTSPANEL_H

#include <wx/wx.h>
#include "AbstractFilterPanel.h"

class FilterAdjustmentsPanel : public wxScrolledWindow {
public:
    FilterAdjustmentsPanel(wxWindow* parent);

private:
    wxComboBox* filterDropdown;
    wxBoxSizer* filterSectionSizer;

    void OnAddFilter(wxCommandEvent& event);
    void OnRemoveFilter(wxCommandEvent& event);
};

#endif //FILTERADJUSTMENTSPANEL_H
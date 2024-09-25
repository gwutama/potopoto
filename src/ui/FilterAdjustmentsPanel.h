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
    void OnRemoveButtonClicked(wxCommandEvent& event);
    void OnMoveUpButtonClicked(wxCommandEvent& event);
    void OnMoveDownButtonClicked(wxCommandEvent& event);
};

#endif //FILTERADJUSTMENTSPANEL_H
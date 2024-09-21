#ifndef POTOPOTO_TABLEDATAPANEL_H
#define POTOPOTO_TABLEDATAPANEL_H

#include <wx/wx.h>
#include <wx/grid.h>
#include <unordered_map>
#include <string>

class TableDataPanel : public wxPanel {
public:
    TableDataPanel(wxWindow* parent);
    virtual ~TableDataPanel();  // Ensure destructor is implemented

    void SetData(const std::unordered_map<std::string, std::string>& data);

private:
    wxGrid* infoGrid;

    void OnSize(wxSizeEvent& event);

wxDECLARE_EVENT_TABLE();
};

#endif // POTOPOTO_TABLEDATAPANEL_H

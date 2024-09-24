#ifndef ABSTRACTFILTERPANEL_H
#define ABSTRACTFILTERPANEL_H

#include <wx/wx.h>
#include <wx/collpane.h>  // Include for wxCollapsiblePane

class AbstractFilterPanel : public wxPanel {
public:
    // Constructor
    AbstractFilterPanel(wxWindow* parent);

    // Get the remove button to bind events externally
    wxButton* GetRemoveButton() const { return removeButton; }

protected:
    virtual void AddFilterControls(wxWindow* sizer) = 0;

    virtual std::string GetName() = 0;

    // Initialize common controls
    void InitializeControls();

protected:
    wxCollapsiblePane* collapsiblePane;  // Collapsible pane for filter content
    wxButton* removeButton;              // Button to remove the filter
};

#endif // ABSTRACTFILTERPANEL_H
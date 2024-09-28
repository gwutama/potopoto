#ifndef ABSTRACTFILTERPANEL_H
#define ABSTRACTFILTERPANEL_H

#include <wx/wx.h>
#include <wx/collpane.h>

class AbstractFilterPanel : public wxPanel {
public:
    AbstractFilterPanel(wxWindow* parent);

    // Buttons to bind events externally
    wxButton* GetRemoveButton() const { return removeButton; }
    wxButton* GetMoveUpButton() const { return moveUpButton; }
    wxButton* GetMoveDownButton() const { return moveDownButton; }

    void SetRemoveButtonClickCallback(const std::function<void(wxCommandEvent&)>& callback);
    void SetMoveUpButtonClickCallback(const std::function<void(wxCommandEvent&)>& callback);
    void SetMoveDownButtonClickCallback(const std::function<void(wxCommandEvent&)>& callback);

protected:
    virtual void AddFilterControls(wxWindow* sizer) = 0;

    wxBoxSizer* CreateButtonsSizer();

    virtual std::string GetName() = 0;

    // Initialize common controls
    void InitializeControls();

protected:
    wxCollapsiblePane* collapsiblePane;
    wxButton* removeButton;
    wxButton* moveUpButton;
    wxButton* moveDownButton;
};

#endif // ABSTRACTFILTERPANEL_H
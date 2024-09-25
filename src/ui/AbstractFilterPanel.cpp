#include "AbstractFilterPanel.h"

AbstractFilterPanel::AbstractFilterPanel(wxWindow* parent)
        : wxPanel(parent, wxID_ANY) {
}

void AbstractFilterPanel::InitializeControls() {
    // Create a static box to visually distinguish the panel
    wxStaticBox* staticBox = new wxStaticBox(this, wxID_ANY, "");  // Creates the box with the panel's name

    // Main vertical sizer for the static box
    wxStaticBoxSizer* staticBoxSizer = new wxStaticBoxSizer(staticBox, wxVERTICAL);

    // Collapsible pane to hold the filter controls
    collapsiblePane = new wxCollapsiblePane(this, wxID_ANY, GetName());

    // Add buttons inside the collapsible pane's content
    removeButton = new wxButton(collapsiblePane->GetPane(), wxID_ANY, "Remove");
    moveUpButton = new wxButton(collapsiblePane->GetPane(), wxID_ANY, "Move Up");
    moveDownButton = new wxButton(collapsiblePane->GetPane(), wxID_ANY, "Move Down");

    // Call derived class to add specific filter controls
    AddFilterControls(collapsiblePane->GetPane());

    // Add collapsible pane to the static box sizer
    staticBoxSizer->Add(collapsiblePane, 0, wxGROW | wxALL, 0);

    // Bind an event to handle when the collapsible pane changes state (collapse/expand)
    collapsiblePane->Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, [this](wxCollapsiblePaneEvent& event) {
        this->GetSizer()->Layout();
        this->GetParent()->Layout();  // Update the layout of the parent if necessary
    });

    // Set the sizer for the panel with the static box sizer
    SetSizer(staticBoxSizer);

    // Expand the collapsible pane by default
    collapsiblePane->Expand();
}


wxBoxSizer* AbstractFilterPanel::CreateButtonsSizer() {
    // Create a horizontal sizer for the buttons
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    // Add buttons to the sizer
    buttonSizer->Add(removeButton, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
    buttonSizer->Add(moveUpButton, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);
    buttonSizer->Add(moveDownButton, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 5);

    return buttonSizer;
}


void AbstractFilterPanel::SetRemoveButtonClickCallback(const std::function<void(wxCommandEvent&)>& callback) {
    removeButton->Bind(wxEVT_BUTTON, [callback](wxCommandEvent& event) {
        callback(event);
    });
}


void AbstractFilterPanel::SetMoveUpButtonClickCallback(const std::function<void(wxCommandEvent&)>& callback) {
    moveUpButton->Bind(wxEVT_BUTTON, [callback](wxCommandEvent& event) {
        callback(event);
    });
}


void AbstractFilterPanel::SetMoveDownButtonClickCallback(const std::function<void(wxCommandEvent&)>& callback) {
    moveDownButton->Bind(wxEVT_BUTTON, [callback](wxCommandEvent& event) {
        callback(event);
    });
}
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

    // Add the remove button inside the collapsible pane's content
    removeButton = new wxButton(collapsiblePane->GetPane(), wxID_ANY, "Remove");

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
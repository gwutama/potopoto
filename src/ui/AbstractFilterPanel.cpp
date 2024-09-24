#include "AbstractFilterPanel.h"

AbstractFilterPanel::AbstractFilterPanel(wxWindow* parent)
        : wxPanel(parent, wxID_ANY) {
}

void AbstractFilterPanel::InitializeControls() {
    // Main vertical sizer
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Collapsible pane to hold the filter controls
    collapsiblePane = new wxCollapsiblePane(this, wxID_ANY, GetName());

    removeButton = new wxButton(collapsiblePane->GetPane(), wxID_ANY, "Remove");

    AddFilterControls(collapsiblePane->GetPane());

    // Add collapsible pane to the main sizer
    mainSizer->Add(collapsiblePane, 0, wxGROW | wxALL, 0);

    // Bind an event to handle when the collapsible pane changes state (collapse/expand)
    collapsiblePane->Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, [this](wxCollapsiblePaneEvent& event) {
        // Trigger a layout update to adjust the size of the panel when collapsed or expanded
        this->GetSizer()->Layout();
        this->GetParent()->Layout();  // Update the layout of the parent, if necessary
    });

    // Set the sizer for the panel
    SetSizer(mainSizer);

    collapsiblePane->Expand();
}

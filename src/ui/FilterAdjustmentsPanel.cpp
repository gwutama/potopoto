#include "FilterAdjustmentsPanel.h"
#include "BlurFilterPanel.h"

FilterAdjustmentsPanel::FilterAdjustmentsPanel(wxWindow* parent)
        : wxScrolledWindow(parent, wxID_ANY) {  // Change wxPanel to wxScrolledWindow

    // Set scroll rate (pixels per scroll step)
    SetScrollRate(5, 5);

    // Create a vertical box sizer to hold the dropdown and filter panels
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Create a horizontal box sizer for the dropdown and add button
    wxBoxSizer* addSizer = new wxBoxSizer(wxHORIZONTAL);

    // Create the filter dropdown (combo box)
    filterDropdown = new wxComboBox(this, wxID_ANY);
    filterDropdown->Append("Blur");
    filterDropdown->Append("Sharpen"); // Add other filters here
    addSizer->Add(filterDropdown, 1, wxALL | wxEXPAND, 5);

    // Create the add button
    wxButton* addButton = new wxButton(this, wxID_ADD, "Add");
    addSizer->Add(addButton, 0, wxALL, 5);

    // Add the horizontal sizer to the main sizer
    mainSizer->Add(addSizer, 0, wxALL | wxEXPAND, 0);

    // Create a vertical sizer to hold the dynamically added filter panels
    filterSectionSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(filterSectionSizer, 1, wxALL | wxEXPAND, 0);

    // Set the panel's sizer
    SetSizer(mainSizer);

    // Bind events
    addButton->Bind(wxEVT_BUTTON, &FilterAdjustmentsPanel::OnAddFilter, this);
}

void FilterAdjustmentsPanel::OnAddFilter(wxCommandEvent& event) {
    wxString selectedFilter = filterDropdown->GetValue();

    if (selectedFilter.IsEmpty()) {
        wxMessageBox("Please select a filter", "Error", wxOK | wxICON_ERROR);
        return;
    }

    AbstractFilterPanel* filterPanel = nullptr;

    if (selectedFilter == "Blur") {
        filterPanel = new BlurFilterPanel(this);  // Create a blur filter panel
    }
    // Handle other filter types here (e.g., SharpenFilterPanel, etc.)

    if (filterPanel) {
        // Add the new filter panel to the filter section sizer
        filterSectionSizer->Add(filterPanel, 0, wxEXPAND | wxALL, 5);

        // Re-layout the sizer to reflect the changes
        filterSectionSizer->Layout();
        Layout();

        // Adjust the virtual size to fit the new contents
        FitInside();

        filterPanel->SetRemoveButtonClickCallback([this](wxCommandEvent& event) {
            OnRemoveButtonClicked(event);
        });

        filterPanel->SetMoveUpButtonClickCallback([this](wxCommandEvent& event) {
            OnMoveUpButtonClicked(event);
        });

        filterPanel->SetMoveDownButtonClickCallback([this](wxCommandEvent& event) {
            OnMoveDownButtonClicked(event);
        });
    } else {
        wxMessageBox("Invalid filter type selected", "Error", wxOK | wxICON_ERROR);
    }
}


void FilterAdjustmentsPanel::OnRemoveButtonClicked(wxCommandEvent& event) {
    // Get the button that triggered the event
    wxButton* removeButton = dynamic_cast<wxButton*>(event.GetEventObject());

    if (!removeButton) {
        return;
    }

    // Traverse through the filter panels and find which one contains the remove button
    for (auto child : filterSectionSizer->GetChildren()) {
        AbstractFilterPanel* filterPanel = dynamic_cast<AbstractFilterPanel*>(child->GetWindow());
        if (filterPanel && filterPanel->GetRemoveButton() == removeButton) {
            // Remove the panel from the sizer
            filterSectionSizer->Detach(filterPanel);
            filterPanel->Destroy();  // Destroy the panel

            // Re-layout the sizer to reflect the changes
            filterSectionSizer->Layout();
            Layout(); // Make sure the entire panel is updated
            FitInside();  // Adjust the virtual size after removal
            break;
        }
    }
}


void FilterAdjustmentsPanel::OnMoveUpButtonClicked(wxCommandEvent& event) {
    wxButton* moveUpButton = dynamic_cast<wxButton*>(event.GetEventObject());

    if (!moveUpButton) {
        return;
    }

    for (size_t i = 0; i < filterSectionSizer->GetChildren().size(); ++i) {
        AbstractFilterPanel* filterPanel = dynamic_cast<AbstractFilterPanel*>(filterSectionSizer->GetItem(i)->GetWindow());
        if (filterPanel && filterPanel->GetMoveUpButton() == moveUpButton && i > 0) {
            // Swap the current filter panel with the one above it
            filterSectionSizer->Detach(filterPanel);
            filterSectionSizer->Insert(i - 1, filterPanel, 0, wxEXPAND | wxALL, 5);

            filterSectionSizer->Layout();
            Layout();
            FitInside();
            break;
        }
    }
}


void FilterAdjustmentsPanel::OnMoveDownButtonClicked(wxCommandEvent& event) {
    wxButton* moveDownButton = dynamic_cast<wxButton*>(event.GetEventObject());

    if (!moveDownButton) {
        return;
    }

    size_t childCount = filterSectionSizer->GetChildren().size();
    for (size_t i = 0; i < childCount; ++i) {
        AbstractFilterPanel* filterPanel = dynamic_cast<AbstractFilterPanel*>(filterSectionSizer->GetItem(i)->GetWindow());
        if (filterPanel && filterPanel->GetMoveDownButton() == moveDownButton && i < childCount - 1) {
            // Swap the current filter panel with the one below it
            filterSectionSizer->Detach(filterPanel);
            filterSectionSizer->Insert(i + 1, filterPanel, 0, wxEXPAND | wxALL, 5);

            filterSectionSizer->Layout();
            Layout();
            FitInside();
            break;
        }
    }
}
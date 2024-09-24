#include "BlurFilterPanel.h"

BlurFilterPanel::BlurFilterPanel(wxWindow* parent)
        : AbstractFilterPanel(parent) {
    InitializeControls();
}

void BlurFilterPanel::AddFilterControls(wxWindow* window) {
    const int defaultValue = 0; // TODO: change me

    wxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // Add controls specific to the Blur filter (within the collapsible pane's content)
    wxBoxSizer* sliderSizer = new wxBoxSizer(wxHORIZONTAL);

    // Label
    wxStaticText* label = new wxStaticText(window, wxID_ANY, "Strength", wxDefaultPosition,
                                           wxSize(100, -1), wxALIGN_LEFT);
    sliderSizer->Add(label, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

    // Slider
    wxSlider* slider = new wxSlider(window, wxID_ANY, defaultValue, 0, 100, wxDefaultPosition,
                                    wxDefaultSize, wxSL_HORIZONTAL);
    sliderSizer->Add(slider, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

    // Value
    wxStaticText* valueLabel = new wxStaticText(window, wxID_ANY, wxString::Format("%d", defaultValue),
                                                wxDefaultPosition, wxSize(30, -1), wxALIGN_RIGHT);
    sliderSizer->Add(valueLabel, 0, wxALIGN_CENTER_VERTICAL);

    // Bind the slider event to update the value label
    slider->Bind(wxEVT_SLIDER, [valueLabel, slider](wxCommandEvent& event) {
        // Update the value label with the current slider value
        valueLabel->SetLabel(wxString::Format("%d", slider->GetValue()));
    });

    // Add the sliderSizer to the sizer (sizer is passed from AbstractFilterPanel's InitializeControls)
    sizer->Add(sliderSizer, 0, wxEXPAND | wxALL, 5);

    // Add remove button
    wxBoxSizer* removeButtonSizer = new wxBoxSizer(wxHORIZONTAL);
    removeButtonSizer->Add(removeButton, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 0);
    sizer->Add(removeButtonSizer, 0, wxEXPAND | wxALL, 5);

    window->SetSizer(sizer);
    sizer->SetSizeHints(window);
}
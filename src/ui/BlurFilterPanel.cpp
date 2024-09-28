#include "BlurFilterPanel.h"

wxDEFINE_EVENT(EVT_FILTER_PARAMETERS_CHANGED, wxCommandEvent);
wxDEFINE_EVENT(EVT_FILTER_MOUSE_RELEASED, wxCommandEvent);


BlurFilterPanel::BlurFilterPanel(wxWindow* parent)
        : AbstractFilterPanel(parent) {
    InitializeControls();
}


void BlurFilterPanel::Reset() {
    slider->SetValue(0); // TODO: change me
    valueLabel->SetLabel("0");

    filterParameters = std::make_shared<BlurFilterParameters>();
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
    slider = new wxSlider(window, wxID_ANY, defaultValue, 0, 100, wxDefaultPosition,
                          wxDefaultSize, wxSL_HORIZONTAL);
    sliderSizer->Add(slider, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

    // Value
    valueLabel = new wxStaticText(window, wxID_ANY, wxString::Format("%d", defaultValue),
                                  wxDefaultPosition, wxSize(30, -1), wxALIGN_RIGHT);
    sliderSizer->Add(valueLabel, 0, wxALIGN_CENTER_VERTICAL);


    slider->Bind(wxEVT_SLIDER, &BlurFilterPanel::OnSliderChanged, this);
    slider->Bind(wxEVT_LEFT_UP, &BlurFilterPanel::OnSliderMouseReleased, this);

    // Add the sliderSizer to the sizer (sizer is passed from AbstractFilterPanel's InitializeControls)
    sizer->Add(sliderSizer, 0, wxEXPAND | wxALL, 5);

    // Add buttons
    sizer->Add(CreateButtonsSizer(), 0, wxEXPAND | wxALL, 5);

    window->SetSizer(sizer);
    sizer->SetSizeHints(window);
}


void BlurFilterPanel::OnSliderChanged(wxCommandEvent &event) {
    auto changedSlider = dynamic_cast<wxSlider *>(event.GetEventObject());

    if (!changedSlider) {
        return;
    }

    // Update the value label with the current slider value
    valueLabel->SetLabel(wxString::Format("%d", changedSlider->GetValue()));

    auto newParams = std::dynamic_pointer_cast<BlurFilterParameters>(GetImageProcessingParametersFromUiControls());

    if (newParams != filterParameters) {
        filterParameters = newParams;
        wxCommandEvent newEvent(EVT_FILTER_PARAMETERS_CHANGED);
        newEvent.SetClientData(&filterParameters);
        ProcessEvent(newEvent);
    }
}


void BlurFilterPanel::OnSliderMouseReleased(wxMouseEvent &event) {
    wxCommandEvent evt(EVT_FILTER_MOUSE_RELEASED);
    wxPostEvent(this->GetParent(), evt);
}


std::shared_ptr<IImageProcessingParameters> BlurFilterPanel::GetImageProcessingParametersFromUiControls() const {
    BlurFilterParameters parameters;
    parameters.SetStrength(slider->GetValue());

    return std::make_shared<BlurFilterParameters>(parameters);
}
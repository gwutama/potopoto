#include "LayerAdjustmentsPanel.h"
#include <wx/stattext.h>
#include <wx/sizer.h>

wxBEGIN_EVENT_TABLE(LayerAdjustmentsPanel, wxPanel)
                // Your event mappings
wxEND_EVENT_TABLE()

LayerAdjustmentsPanel::LayerAdjustmentsPanel(wxWindow* parent)
        : wxPanel(parent, wxID_ANY) {

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Helper function to create sliders with labels
    auto addSliderWithLabel = [&](wxSizer* sizer, const wxString& labelText, wxSlider*& slider, wxStaticText*& valueLabel, int minValue, int maxValue, int defaultValue) {
        wxBoxSizer* sliderSizer = new wxBoxSizer(wxHORIZONTAL);

        // Label (fixed width, right-aligned)
        wxStaticText* label = new wxStaticText(this, wxID_ANY, labelText, wxDefaultPosition, wxSize(100, -1), wxALIGN_LEFT);
        sliderSizer->Add(label, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

        // Slider
        slider = new wxSlider(this, wxID_ANY, defaultValue, minValue, maxValue, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL);
        sliderSizer->Add(slider, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

        // Value display (fixed width, right-aligned)
        valueLabel = new wxStaticText(this, wxID_ANY, wxString::Format("%d", defaultValue), wxDefaultPosition, wxSize(30, -1), wxALIGN_RIGHT);
        sliderSizer->Add(valueLabel, 0, wxALIGN_CENTER_VERTICAL);

        sizer->Add(sliderSizer, 0, wxEXPAND | wxALL, 5);

        // Add to slider-to-value map
        sliderToValueMap[slider] = valueLabel;

        // Event handler to update the value display when the slider changes
        slider->Bind(wxEVT_SLIDER, &LayerAdjustmentsPanel::OnSliderChanged, this);
    };

    addSliderWithLabel(mainSizer, "Brightness", brightnessSlider, brightnessValueLabel, 0, 200, 100);
    addSliderWithLabel(mainSizer, "Contrast", contrastSlider, contrastValueLabel, 0, 200, 100);
    addSliderWithLabel(mainSizer, "Hue", hueSlider, hueValueLabel, 0, 180, 90);
    addSliderWithLabel(mainSizer, "Saturation", saturationSlider, saturationValueLabel, -128, 128, 0);
    addSliderWithLabel(mainSizer, "Value", valueSlider, valueValueLabel, -128, 128, 0);
    addSliderWithLabel(mainSizer, "Lightness", lightnessSlider, lightnessValueLabel, -128, 128, 0);
    addSliderWithLabel(mainSizer, "White Balance", whiteBalanceSlider, whiteBalanceValueLabel, 0, 100, 50);
    addSliderWithLabel(mainSizer, "Gamma", gammaSlider, gammaValueLabel, 0, 1000, 100);
    addSliderWithLabel(mainSizer, "Shadow", shadowSlider, shadowValueLabel, -50, 50, 0);
    addSliderWithLabel(mainSizer, "Highlight", highlightSlider, highlightValueLabel, -50, 50, 0);
    addSliderWithLabel(mainSizer, "Cyan", cyanSlider, cyanValueLabel, -100, 100, 0);
    addSliderWithLabel(mainSizer, "Magenta", magentaSlider, magentaValueLabel, -100, 100, 0);
    addSliderWithLabel(mainSizer, "Yellow", yellowSlider, yellowValueLabel, -100, 100, 0);
    addSliderWithLabel(mainSizer, "Black", blackSlider, blackValueLabel, -100, 100, 0);

    SetSizer(mainSizer);
}


void LayerAdjustmentsPanel::OnSliderChanged(wxCommandEvent& event)
{
    wxSlider* slider = dynamic_cast<wxSlider*>(event.GetEventObject());
    if (slider && sliderToValueMap.count(slider)) {
        // Update the corresponding value label
        int value = slider->GetValue();
        sliderToValueMap[slider]->SetLabel(wxString::Format("%d", value));
    }
}

void LayerAdjustmentsPanel::Reset()
{
    brightnessSlider->SetValue(100);
    contrastSlider->SetValue(100);
    hueSlider->SetValue(90);
    saturationSlider->SetValue(0);
    valueSlider->SetValue(0);
    lightnessSlider->SetValue(0);
    whiteBalanceSlider->SetValue(50);
    gammaSlider->SetValue(100);
    shadowSlider->SetValue(0);
    highlightSlider->SetValue(0);
    cyanSlider->SetValue(0);
    magentaSlider->SetValue(0);
    yellowSlider->SetValue(0);
    blackSlider->SetValue(0);
}

#include "LayerAdjustmentsPanel.h"
#include <wx/stattext.h>
#include <wx/sizer.h>

wxDEFINE_EVENT(EVT_ADJUSTMENT_SLIDER_VALUE_CHANGED, wxCommandEvent);
wxDEFINE_EVENT(EVT_ADJUSTMENT_SLIDER_MOUSE_RELEASED_VALUE_CHANGED, wxCommandEvent);

wxBEGIN_EVENT_TABLE(LayerAdjustmentsPanel, wxPanel)
                // Your event mappings
wxEND_EVENT_TABLE()


LayerAdjustmentsPanel::LayerAdjustmentsPanel(wxWindow *parent)
        : wxPanel(parent, wxID_ANY) {

    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);

    // Helper function to create sliders with labels
    auto addSliderWithLabel = [&](
            wxSizer *sizer, const wxString &labelText, wxSlider *&slider,
            wxStaticText *&valueLabel, int minValue, int maxValue, int defaultValue,
            std::function<void(wxCommandEvent &)> callback) {
        wxBoxSizer *sliderSizer = new wxBoxSizer(wxHORIZONTAL);

        // Label (fixed width, right-aligned)
        wxStaticText *label = new wxStaticText(this, wxID_ANY, labelText, wxDefaultPosition,
                                               wxSize(100, -1), wxALIGN_LEFT);
        sliderSizer->Add(label, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

        // Slider
        slider = new wxSlider(this, wxID_ANY, defaultValue, minValue, maxValue, wxDefaultPosition,
                              wxDefaultSize,wxSL_HORIZONTAL);
        sliderSizer->Add(slider, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);

        // Value display (fixed width, right-aligned)
        valueLabel = new wxStaticText(this, wxID_ANY, wxString::Format("%d", defaultValue),
                                      wxDefaultPosition,wxSize(30, -1), wxALIGN_RIGHT);
        sliderSizer->Add(valueLabel, 0, wxALIGN_CENTER_VERTICAL);

        sizer->Add(sliderSizer, 0, wxEXPAND | wxALL, 5);

        // Add to slider-to-value map
        sliderToValueMap[slider] = valueLabel;

        slider->Bind(wxEVT_SLIDER, [callback, this](wxCommandEvent &event) {
            this->OnSliderChanged(event);  // Update the value display
            if (callback) {
                callback(event);  // Call the provided callback function
            }
        });

        slider->Bind(wxEVT_LEFT_UP, [this](wxMouseEvent &event) {
            this->OnSliderMouseReleased(event);
        });
    };

    addSliderWithLabel(mainSizer, "Brightness", brightnessSlider, brightnessValueLabel,
                       -100, 100, 0, nullptr);
    addSliderWithLabel(mainSizer, "Contrast", contrastSlider, contrastValueLabel,
                       -100, 100, 0, nullptr);
    addSliderWithLabel(mainSizer, "Hue", hueSlider, hueValueLabel,
                       0, 180, 0, nullptr);
    addSliderWithLabel(mainSizer, "Saturation", saturationSlider, saturationValueLabel,
                       -100, 100, 0, nullptr);
    addSliderWithLabel(mainSizer, "Value", valueSlider, valueValueLabel,
                       -100, 100, 0, nullptr);
    addSliderWithLabel(mainSizer, "Lightness", lightnessSlider, lightnessValueLabel,
                       -100, 100, 0, nullptr);
    addSliderWithLabel(mainSizer, "White Balance", whiteBalanceSlider, whiteBalanceValueLabel,
                       0, 100, 0, nullptr);
    addSliderWithLabel(mainSizer, "Gamma", gammaSlider, gammaValueLabel,
                       -100, 100, 0, nullptr);
    addSliderWithLabel(mainSizer, "Shadow", shadowSlider, shadowValueLabel,
                       -100, 100, 0, nullptr);
    addSliderWithLabel(mainSizer, "Highlight", highlightSlider, highlightValueLabel,
                       -100, 100, 0, nullptr);
    addSliderWithLabel(mainSizer, "Cyan", cyanSlider, cyanValueLabel,
                       -100, 100, 0, nullptr);
    addSliderWithLabel(mainSizer, "Magenta", magentaSlider, magentaValueLabel,
                       -100, 100, 0, nullptr);
    addSliderWithLabel(mainSizer, "Yellow", yellowSlider, yellowValueLabel,
                       -100, 100, 0, nullptr);
    addSliderWithLabel(mainSizer, "Black", blackSlider, blackValueLabel,
                       -100, 100, 0, nullptr);

    SetSizer(mainSizer);
    Reset();
}


std::shared_ptr<IImageProcessingParameters> LayerAdjustmentsPanel::GetImageProcessingParametersFromUiControls() const {
    AdjustmentsParameters params;

    // Translate brightness slider value to actual brightness value
    // Slider range [-100, 100], default: 0
    // Actual range: [0.0, 2.0], default: 1.0, increment: 0.01
    float brightness = (brightnessSlider->GetValue() + 100) / 100.0f;
    params.SetBrightness(brightness);

    // Translate contrast slider value to actual contrast value
    // Slider range [-100, 100], default: 0
    // Actual range: [0.0, 2.0], default: 1.0, increment: 0.01
    float contrast = (contrastSlider->GetValue() + 100) / 100.0f;
    params.SetContrast(contrast);

    // Translate hue slider value to actual hue value
    // Slider range [0, 180], default: 0
    // Actual range: [0.0, 180.0], default: 0.0, increment: 1.0
    float hue = hueSlider->GetValue();
    params.SetHue(hue);

    // Translate saturation slider value to actual saturation value
    // Slider range [-100, 100], default: 0
    // Actual range: [-100.0, 100.0], default: 0.0, increment: 1.0
    float saturation = saturationSlider->GetValue();
    params.SetSaturation(saturation);

    // Translate value slider value to actual value value
    // Slider range [-100, 100], default: 0
    // Actual range: [-100.0, 100.0], default: 0.0, increment: 1.0
    float value = valueSlider->GetValue();
    params.SetValue(value);

    // Translate lightness slider value to actual lightness value
    // Slider range [-100, 100], default: 0
    // Actual range: [-100.0, 100.0], default: 0.0, increment: 1.0
    float lightness = lightnessSlider->GetValue();
    params.SetLightness(lightness);

    // Translate white balance slider value to actual white balance value
    // Slider range [0, 100], default: 0
    // Actual range: [0.0, 1.0], default: 0.0, increment: 0.01
    float whiteBalance = whiteBalanceSlider->GetValue() / 100.0f;
    params.SetWhiteBalanceSaturationThreshold(whiteBalance);

    // Translate gamma slider value to actual gamma value
    // Slider range [-100, 100], default: 0
    // Actual range: [0.0, 10.0], default: 1.0, increment: 0.01
    float gamma = (gammaSlider->GetValue() + 100.0f) / 100.0f;
    params.SetGamma(gamma);

    // Translate shadow slider value to actual shadow value
    // Slider range [-100, 100], default: 0
    // Actual range: [-100, 100], default: 0.0, increment: 1
    float shadow = shadowSlider->GetValue();
    params.SetShadow(shadow);

    // Translate highlight slider value to actual highlight value
    // Slider range [-100, 100], default: 0
    // Actual range: [-100, 100], default: 0.0, increment: 1
    float highlight = highlightSlider->GetValue();
    params.SetHighlight(highlight);

    // Translate cyan slider value to actual cyan value
    // Slider range [-100, 100], default: 0
    // Actual range: [-1.0, 1.0], default: 0.0, increment: 0.01
    float cyan = cyanSlider->GetValue() / 100.0f;
    params.SetCyan(cyan);

    // Translate magenta slider value to actual magenta value
    // Slider range [-100, 100], default: 0
    // Actual range: [-1.0, 1.0], default: 0.0, increment: 0.01
    float magenta = magentaSlider->GetValue() / 100.0f;
    params.SetMagenta(magenta);

    // Translate yellow slider value to actual yellow value
    // Slider range [-100, 100], default: 0
    // Actual range: [-1.0, 1.0], default: 0.0, increment: 0.01
    float yellow = yellowSlider->GetValue() / 100.0f;
    params.SetYellow(yellow);

    // Translate black slider value to actual black value
    // Slider range [-100, 100], default: 0
    // Actual range: [-1.0, 1.0], default: 0.0, increment: 0.01
    float black = blackSlider->GetValue() / 100.0f;
    params.SetBlack(black);

    return std::make_shared<AdjustmentsParameters>(params);
}


void LayerAdjustmentsPanel::OnSliderChanged(wxCommandEvent &event) {
    wxSlider *slider = dynamic_cast<wxSlider *>(event.GetEventObject());

    if (slider && sliderToValueMap.count(slider)) {
        // Update the corresponding value label
        int value = slider->GetValue();
        sliderToValueMap[slider]->SetLabel(wxString::Format("%d", value));
    }

    auto newParams = std::dynamic_pointer_cast<AdjustmentsParameters>(GetImageProcessingParametersFromUiControls());

    if (newParams != adjustments_parameters) {
        adjustments_parameters = newParams;

        // Fire custom event to notify external listeners about the adjustment change
        wxCommandEvent evt(EVT_ADJUSTMENT_SLIDER_VALUE_CHANGED);
        evt.SetClientData(&adjustments_parameters);
        wxPostEvent(this->GetParent(), evt); // Post the event to the parent (or other target)
    }
}


void LayerAdjustmentsPanel::OnSliderMouseReleased(wxMouseEvent &event) {
    // TODO: this is not good because it will always fire the event even if the values are the same
    // However adjustmentParameters has been set in OnSliderChanged. Find a way to avoid this.
    // Fire custom event to notify external listeners about the adjustment change
    wxCommandEvent evt(EVT_ADJUSTMENT_SLIDER_MOUSE_RELEASED_VALUE_CHANGED);
    evt.SetClientData(&adjustments_parameters);
    wxPostEvent(this->GetParent(), evt); // Post the event to the parent (or other target)
}


void LayerAdjustmentsPanel::Reset() {
    adjustments_parameters = std::make_shared<AdjustmentsParameters>();
    brightnessSlider->SetValue(0);
    contrastSlider->SetValue(0);
    hueSlider->SetValue(0);
    saturationSlider->SetValue(0);
    valueSlider->SetValue(0);
    lightnessSlider->SetValue(0);
    whiteBalanceSlider->SetValue(0);
    gammaSlider->SetValue(0);
    shadowSlider->SetValue(0);
    highlightSlider->SetValue(0);
    cyanSlider->SetValue(0);
    magentaSlider->SetValue(0);
    yellowSlider->SetValue(0);
    blackSlider->SetValue(0);
}

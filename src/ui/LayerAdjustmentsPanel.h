#ifndef IMAGE_ADJUSTMENTS_PANEL_H
#define IMAGE_ADJUSTMENTS_PANEL_H

#include <wx/wx.h>
#include <map>

class LayerAdjustmentsPanel : public wxPanel
{
public:
    LayerAdjustmentsPanel(wxWindow* parent);

    void Reset();  // Reset all sliders to default values

private:
    // Sliders and their associated value labels
    wxSlider* brightnessSlider;
    wxSlider* contrastSlider;
    wxSlider* hueSlider;
    wxSlider* saturationSlider;
    wxSlider* valueSlider;
    wxSlider* lightnessSlider;
    wxSlider* whiteBalanceSlider;
    wxSlider* gammaSlider;
    wxSlider* shadowSlider;
    wxSlider* highlightSlider;
    wxSlider* cyanSlider;
    wxSlider* magentaSlider;
    wxSlider* yellowSlider;
    wxSlider* blackSlider;

    wxStaticText* brightnessValueLabel;
    wxStaticText* contrastValueLabel;
    wxStaticText* hueValueLabel;
    wxStaticText* saturationValueLabel;
    wxStaticText* valueValueLabel;
    wxStaticText* lightnessValueLabel;
    wxStaticText* whiteBalanceValueLabel;
    wxStaticText* gammaValueLabel;
    wxStaticText* shadowValueLabel;
    wxStaticText* highlightValueLabel;
    wxStaticText* cyanValueLabel;
    wxStaticText* magentaValueLabel;
    wxStaticText* yellowValueLabel;
    wxStaticText* blackValueLabel;

    std::map<wxSlider*, wxStaticText*> sliderToValueMap;  // Maps sliders to their value labels

    void OnSliderChanged(wxCommandEvent& event);  // Event handler for when a slider changes

wxDECLARE_EVENT_TABLE();
};

#endif // IMAGE_ADJUSTMENTS_PANEL_H

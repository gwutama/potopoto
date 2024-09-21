#ifndef IMAGE_ADJUSTMENTS_PANEL_H
#define IMAGE_ADJUSTMENTS_PANEL_H

#include <wx/wx.h>
#include <map>
#include "../AdjustmentsParameters.h"

wxDECLARE_EVENT(EVT_ADJUSTMENT_SLIDER_VALUE_CHANGED, wxCommandEvent);
wxDECLARE_EVENT(EVT_ADJUSTMENT_SLIDER_MOUSE_RELEASED_VALUE_CHANGED, wxCommandEvent);

class LayerAdjustmentsPanel : public wxPanel
{
public:
    LayerAdjustmentsPanel(wxWindow* parent);

    void Reset();

private:
    AdjustmentsParameters GetAdjustmentsParametersFromUiValues() const;

    void OnSliderChanged(wxCommandEvent& event);
    void OnSliderMouseReleased(wxMouseEvent& event);

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

    AdjustmentsParameters adjustments_parameters;

wxDECLARE_EVENT_TABLE();
};

#endif // IMAGE_ADJUSTMENTS_PANEL_H

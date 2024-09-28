#ifndef BLURFILTERPANEL_H
#define BLURFILTERPANEL_H

#include <wx/wx.h>
#include "AbstractFilterPanel.h"
#include "../BlurFilterParameters.h"
#include "IImageProcessingPanel.h"

class BlurFilterPanel : public AbstractFilterPanel, public IImageProcessingPanel {
public:
    BlurFilterPanel(wxWindow* parent);
    ~BlurFilterPanel() = default;

    void Reset() override;

private:
    void AddFilterControls(wxWindow* window) override;
    std::string GetName() override { return "Blur"; }
    void OnSliderChanged(wxCommandEvent& event);
    void OnSliderMouseReleased(wxMouseEvent& event);
    std::shared_ptr<IImageProcessingParameters> GetImageProcessingParametersFromUiControls() const override;

private:
    wxSlider* slider;
    wxStaticText* valueLabel;
    std::shared_ptr<BlurFilterParameters> filterParameters;
};

#endif // BLURFILTERPANEL_H
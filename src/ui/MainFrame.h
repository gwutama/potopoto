#ifndef POTOPOTO_MAINFRAME_H
#define POTOPOTO_MAINFRAME_H

#include <wx/wx.h>
#include <wx/frame.h>

#include "../Image.h"
#include "../ImageHistogram.h"
#include "../ImagePreview.h"
#include "ImageEditor.h"
#include "ImageAnalysisPanel.h"
#include "ImageAdjustmentsPanel.h"


class MainFrame : public wxFrame {
public:
    MainFrame(const wxString &title);

private:
    void OnOpen(wxCommandEvent &event);
    void OnClose(wxCommandEvent &event);
    void OnAdjustmentSliderValueChanged(wxCommandEvent &event);
    void OnAdjustmentSliderMouseReleasedValueChanged(wxCommandEvent &event);

    void CreateMenuBar();
    void CreateRightPanel();

private:
    std::shared_ptr<ImagePreview> imagePreview;
    ImageEditor *editor;
    wxPanel *rightPanel;
    ImageAnalysisPanel *imageAnalysisPanel;
    ImageAdjustmentsPanel *imageAdjustmentsPanel;
    std::shared_ptr<Image> image;
    std::shared_ptr<ImageHistogram> imageHistogram;
};


#endif //POTOPOTO_MAINFRAME_H

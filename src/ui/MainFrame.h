#ifndef POTOPOTO_MAINFRAME_H
#define POTOPOTO_MAINFRAME_H

#include <wx/wx.h>
#include <wx/frame.h>

#include "../Image.h"
#include "../ImageHistogram.h"
#include "../ImagePreview2.h"
#include "ImageEditor2.h"
#include "ImageAnalysisPanel.h"
#include "ImageAdjustmentsPanel.h"


class MainFrame : public wxFrame {
public:
    MainFrame(const wxString &title);

private:
    void OnOpen(wxCommandEvent &event);
    void OnClose(wxCommandEvent &event);
    void CreateMenuBar();
    void CreateRightPanel();

private:
    std::shared_ptr<ImagePreview2> imagePreview;
    ImageEditor2 *editor;
    wxPanel *rightPanel;
    ImageAnalysisPanel *imageAnalysisPanel;
    ImageAdjustmentsPanel *imageAdjustmentsPanel;
    std::shared_ptr<Image> image;
    std::shared_ptr<ImageHistogram> imageHistogram;
};


#endif //POTOPOTO_MAINFRAME_H

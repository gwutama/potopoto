#ifndef POTOPOTO_IMAGEANALYSISPANEL_H
#define POTOPOTO_IMAGEANALYSISPANEL_H

#include <wx/wx.h>
#include <wx/notebook.h>

class ImageAnalysisPanel : public wxPanel {
public:
    ImageAnalysisPanel(wxWindow* parent);

private:
    wxNotebook* imageAnalysisTabs;

    void CreateTabs();
};


#endif //POTOPOTO_IMAGEANALYSISPANEL_H

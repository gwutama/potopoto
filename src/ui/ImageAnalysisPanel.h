#ifndef POTOPOTO_IMAGEANALYSISPANEL_H
#define POTOPOTO_IMAGEANALYSISPANEL_H

#include <wx/wx.h>
#include <wx/notebook.h>
#include "HistogramCanvas.h"

class ImageAnalysisPanel : public wxPanel {
public:
    ImageAnalysisPanel(wxWindow* parent);

    void Reset();

    HistogramCanvas* GetHistogramCanvas() const { return histogramCanvas; }

private:
    wxNotebook* imageAnalysisTabs;
    HistogramCanvas* histogramCanvas;

    void CreateTabs();
};


#endif //POTOPOTO_IMAGEANALYSISPANEL_H

#ifndef POTOPOTO_IMAGEANALYSISPANEL_H
#define POTOPOTO_IMAGEANALYSISPANEL_H

#include <wx/wx.h>
#include <wx/notebook.h>
#include "HistogramCanvas.h"
#include "TableDataPanel.h"

class ImageAnalysisPanel : public wxPanel {
public:
    ImageAnalysisPanel(wxWindow* parent);

    void Reset();

    HistogramCanvas* GetHistogramCanvas() const { return histogramCanvas; }
    TableDataPanel* GetImageInfoPanel() const { return imageInfoPanel; }
    TableDataPanel* GetExifMetadataPanel() const { return exifMetadataPanel; }
    TableDataPanel* GetFileInfoPanel() const { return fileInfoPanel; }

private:
    wxNotebook* imageAnalysisTabs;
    HistogramCanvas* histogramCanvas;
    TableDataPanel* imageInfoPanel;
    TableDataPanel* exifMetadataPanel;
    TableDataPanel* fileInfoPanel;

    void CreateTabs();
};


#endif //POTOPOTO_IMAGEANALYSISPANEL_H

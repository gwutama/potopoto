#ifndef HISTOGRAM_CANVAS_H
#define HISTOGRAM_CANVAS_H

#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <opencv2/opencv.hpp>
#include <vector>

class HistogramCanvas : public wxGLCanvas {
public:
    HistogramCanvas(wxWindow* parent);
    ~HistogramCanvas();

    void Reset();

    // Method to load histogram data
    void SetHistogramData(const std::vector<cv::Mat>& histogram_data);

protected:
    void OnPaint(wxPaintEvent& event);
    void OnResize(wxSizeEvent& event);
    void RenderHistogram();  // OpenGL rendering function

private:
    wxGLContext* glContext;
    std::vector<cv::Mat> histogramData;

wxDECLARE_EVENT_TABLE();
};

#endif // HISTOGRAM_CANVAS_H

#include "HistogramCanvas.h"


wxBEGIN_EVENT_TABLE(HistogramCanvas, wxGLCanvas)
                EVT_PAINT(HistogramCanvas::OnPaint)
                EVT_SIZE(HistogramCanvas::OnResize)
wxEND_EVENT_TABLE()


HistogramCanvas::HistogramCanvas(wxWindow* parent)
        : wxGLCanvas(parent, wxID_ANY, nullptr),
          glContext(new wxGLContext(this)) {
    SetBackgroundStyle(wxBG_STYLE_PAINT);
}


HistogramCanvas::~HistogramCanvas() {
    delete glContext;
}


void HistogramCanvas::Reset() {
    histogramData.clear();
    Refresh();  // Request a redraw when data is reset
}


void HistogramCanvas::SetHistogramData(const std::vector<cv::Mat>& data) {
    histogramData = data;
    Refresh();  // Request a redraw when new data is set
}


void HistogramCanvas::OnPaint(wxPaintEvent& event) {
    wxPaintDC dc(this);
    SetCurrent(*glContext);

    // Clear the screen with a dark background
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Set up the orthogonal projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    wxSize clientSize = GetClientSize();
    glOrtho(0, clientSize.GetWidth(), 0, clientSize.GetHeight(), -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Render the histogram
    RenderHistogram();

    SwapBuffers();
}


void HistogramCanvas::OnResize(wxSizeEvent& event) {
    wxSize size = GetClientSize();
    glViewport(0, 0, size.GetWidth(), size.GetHeight());  // Resize OpenGL viewport
    Refresh();  // Redraw when the window is resized
}


void HistogramCanvas::RenderHistogram() {
    if (histogramData.size() != 3) return;  // Ensure we have BGR data

    int hist_size = 256;
    wxSize clientSize = GetClientSize();
    float binWidth = static_cast<float>(clientSize.GetWidth()) / hist_size;

    std::vector<cv::Mat> channels = histogramData;  // BGR histogram data

    // Scaling factor to normalize the height of the graph
    float maxHeight = clientSize.GetHeight();
    float maxVal = 0;
    for (int i = 0; i < 3; ++i) {
        double minVal, max;
        cv::minMaxLoc(channels[i], &minVal, &max);
        maxVal = std::max(maxVal, static_cast<float>(max));
    }

    auto scaleValue = [&](float value) -> float {
        return value / maxVal * maxHeight;
    };

    // Render the line graph for each channel
    glBegin(GL_LINE_STRIP);

    // Blue Channel
    glColor3f(0.4f, 0.4f, 1.0f);  // Blue color
    for (int i = 0; i < hist_size; ++i) {
        float value = scaleValue(channels[0].at<float>(i));
        glVertex2f(i * binWidth, value);  // Set vertex for each bin
    }
    glEnd();

    glBegin(GL_LINE_STRIP);
    // Green Channel
    glColor3f(0.4f, 1.0f, 0.4f);  // Green color
    for (int i = 0; i < hist_size; ++i) {
        float value = scaleValue(channels[1].at<float>(i));
        glVertex2f(i * binWidth, value);  // Set vertex for each bin
    }
    glEnd();

    glBegin(GL_LINE_STRIP);
    // Red Channel
    glColor3f(1.0f, 0.4f, 0.4f);  // Red color
    for (int i = 0; i < hist_size; ++i) {
        float value = scaleValue(channels[2].at<float>(i));
        glVertex2f(i * binWidth, value);  // Set vertex for each bin
    }
    glEnd();
}

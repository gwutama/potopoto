#include "Histogram.h"
#include <implot.h>


void Histogram::Render(const std::vector<cv::Mat>& histogram_data) {
    if (histogram_data.size() != 3) { // BGR channels
        return;
    }

    int hist_size = 256;

    // Display the histogram using ImGui (ImPlot must be initialized before using this)
    ImVec2 plot_size = ImGui::GetContentRegionAvail();

    if (ImPlot::BeginPlot("HistogramPlot", nullptr, nullptr, plot_size, ImPlotFlags_CanvasOnly | ImPlotFlags_NoFrame)) {
        ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_NoDecorations);
        ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_NoDecorations);

        ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(0.4, 0.4, 1, 1.0f)); // Blue
        ImPlot::PlotLine("Blue", histogram_data.at(0).ptr<float>(), hist_size, 1, 0, ImPlotLineFlags_None);
        ImPlot::PopStyleColor();

        ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(0.4, 1, 0.4, 1.0f)); // Green
        ImPlot::PlotLine("Green", histogram_data.at(1).ptr<float>(), hist_size, 1, 0, ImPlotLineFlags_None);
        ImPlot::PopStyleColor();

        ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(1, 0.4, 0.4, 1.0f)); // Red
        ImPlot::PlotLine("Red", histogram_data.at(2).ptr<float>(), hist_size, 1, 0, ImPlotLineFlags_None);
        ImPlot::PopStyleColor();

        ImPlot::EndPlot();
    }
}

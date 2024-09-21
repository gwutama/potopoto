#include "ImageAnalysisPanel.h"


ImageAnalysisPanel::ImageAnalysisPanel(wxWindow* parent)
        : wxPanel(parent, wxID_ANY) {
    CreateTabs();

    // Set sizer for the panel to expand the tabs to the full size of this panel
    wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
    panelSizer->Add(imageAnalysisTabs, 1, wxEXPAND | wxALL, 5);
    SetSizer(panelSizer);
}


void ImageAnalysisPanel::Reset() {
    histogramCanvas->Reset();
}


void ImageAnalysisPanel::CreateTabs() {
    // Create a notebook for the right panel
    imageAnalysisTabs = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxSize(400, 200), wxNB_TOP);

    wxPanel* histogramTab = new wxPanel(imageAnalysisTabs);
    wxPanel* imageTab = new wxPanel(imageAnalysisTabs);
    wxPanel* exifTab = new wxPanel(imageAnalysisTabs);
    wxPanel* fileTab = new wxPanel(imageAnalysisTabs);

    imageAnalysisTabs->AddPage(histogramTab, "Histogram");
    imageAnalysisTabs->AddPage(imageTab, "Image");
    imageAnalysisTabs->AddPage(exifTab, "EXIF");
    imageAnalysisTabs->AddPage(fileTab, "File");

    // Create the histogram canvas and make it fill the tab
    histogramCanvas = new HistogramCanvas(histogramTab);

    // Set up a sizer for the histogramTab
    wxBoxSizer* histogramSizer = new wxBoxSizer(wxVERTICAL);

    // Add the canvas to the sizer, using 1 for proportion and wxEXPAND to fill the space
    histogramSizer->Add(histogramCanvas, 1, wxEXPAND | wxALL, 0);

    // Set the sizer for the tab
    histogramTab->SetSizer(histogramSizer);

    // Image tab content
    wxStaticText* imageLabel = new wxStaticText(imageTab, wxID_ANY, "Image Info Content");
    wxBoxSizer* imageSizer = new wxBoxSizer(wxVERTICAL);
    imageSizer->Add(imageLabel, 0, wxALIGN_CENTER | wxALL, 10);
    imageTab->SetSizer(imageSizer);

    // EXIF tab content
    wxStaticText* exifLabel = new wxStaticText(exifTab, wxID_ANY, "EXIF Metadata Content");
    wxBoxSizer* exifSizer = new wxBoxSizer(wxVERTICAL);
    exifSizer->Add(exifLabel, 0, wxALIGN_CENTER | wxALL, 10);
    exifTab->SetSizer(exifSizer);

    // File tab content
    wxStaticText* fileLabel = new wxStaticText(fileTab, wxID_ANY, "File Info Content");
    wxBoxSizer* fileSizer = new wxBoxSizer(wxVERTICAL);
    fileSizer->Add(fileLabel, 0, wxALIGN_CENTER | wxALL, 10);
    fileTab->SetSizer(fileSizer);
}

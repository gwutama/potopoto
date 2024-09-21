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
    imageAnalysisTabs->SetSelection(0);
    imageInfoPanel->SetData({});
    exifMetadataPanel->SetData({});
    fileInfoPanel->SetData({});
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
    wxBoxSizer* histogramSizer = new wxBoxSizer(wxVERTICAL);
    histogramSizer->Add(histogramCanvas, 1, wxEXPAND | wxALL, 0);
    histogramTab->SetSizer(histogramSizer);

    // Image tab content
    imageInfoPanel = new TableDataPanel(imageTab);
    wxBoxSizer* imageSizer = new wxBoxSizer(wxVERTICAL);
    imageSizer->Add(imageInfoPanel, 1, wxEXPAND | wxALL, 5); // Adjusted to expand
    imageTab->SetSizer(imageSizer);

    // EXIF tab content
    exifMetadataPanel = new TableDataPanel(exifTab);
    wxBoxSizer* exifSizer = new wxBoxSizer(wxVERTICAL);
    exifSizer->Add(exifMetadataPanel, 1, wxEXPAND | wxALL, 5);
    exifTab->SetSizer(exifSizer);

    // File tab content
    fileInfoPanel = new TableDataPanel(fileTab);
    wxBoxSizer* fileSizer = new wxBoxSizer(wxVERTICAL);
    fileSizer->Add(fileInfoPanel, 1, wxEXPAND | wxALL, 5);
    fileTab->SetSizer(fileSizer);
}


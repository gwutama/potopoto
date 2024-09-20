#include "ImageAnalysisPanel.h"

ImageAnalysisPanel::ImageAnalysisPanel(wxWindow* parent)
        : wxPanel(parent, wxID_ANY) {
    // Create the tabs for the image analysis
    CreateTabs();

    // Set sizer for the panel to expand the tabs to the full size of this panel
    wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);
    panelSizer->Add(imageAnalysisTabs, 1, wxEXPAND | wxALL, 5);
    SetSizer(panelSizer);
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

    wxStaticText* histogramLabel = new wxStaticText(histogramTab, wxID_ANY, "Histogram Content");
    wxBoxSizer* histogramSizer = new wxBoxSizer(wxVERTICAL);
    histogramSizer->Add(histogramLabel, 0, wxALIGN_CENTER | wxALL, 10);
    histogramTab->SetSizer(histogramSizer);

    wxStaticText* imageLabel = new wxStaticText(imageTab, wxID_ANY, "Image Info Content");
    wxBoxSizer* imageSizer = new wxBoxSizer(wxVERTICAL);
    imageSizer->Add(imageLabel, 0, wxALIGN_CENTER | wxALL, 10);
    imageTab->SetSizer(imageSizer);

    wxStaticText* exifLabel = new wxStaticText(exifTab, wxID_ANY, "EXIF Metadata Content");
    wxBoxSizer* exifSizer = new wxBoxSizer(wxVERTICAL);
    exifSizer->Add(exifLabel, 0, wxALIGN_CENTER | wxALL, 10);
    exifTab->SetSizer(exifSizer);

    wxStaticText* fileLabel = new wxStaticText(fileTab, wxID_ANY, "File Info Content");
    wxBoxSizer* fileSizer = new wxBoxSizer(wxVERTICAL);
    fileSizer->Add(fileLabel, 0, wxALIGN_CENTER | wxALL, 10);
    fileTab->SetSizer(fileSizer);
}

#include "ImageAdjustmentsPanel.h"
#include "LayerAdjustmentsPanel.h"

ImageAdjustmentsPanel::ImageAdjustmentsPanel(wxWindow* parent)
        : wxPanel(parent, wxID_ANY) {
    // Create the tabs for the image analysis
    CreateTabs();

    // Set sizer for the panel to expand the tabs to the full size of this panel
    wxBoxSizer* panelSizer = new wxBoxSizer(wxVERTICAL);

    // Remove or minimize padding by setting to 0
    panelSizer->Add(imageAdjustmentsTabs, 1, wxEXPAND | wxALL, 0);
    SetSizer(panelSizer);
}

void ImageAdjustmentsPanel::CreateTabs() {
    imageAdjustmentsTabs = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxSize(400, 200), wxNB_TOP);

    // Create a global tab
    wxPanel* globalTab = new wxPanel(imageAdjustmentsTabs);

    // Add the global tab to the notebook
    imageAdjustmentsTabs->AddPage(globalTab, "Global");

    // Add the LayerAdjustmentsPanel within the global tab
    LayerAdjustmentsPanel* layerAdjustmentsPanel = new LayerAdjustmentsPanel(globalTab);

    // Set the sizer for the global tab, minimizing padding
    wxBoxSizer* layerAdjustmentsSizer = new wxBoxSizer(wxVERTICAL);

    // Minimize padding on this layer as well
    layerAdjustmentsSizer->Add(layerAdjustmentsPanel, 1, wxEXPAND | wxALL, 10);

    // Apply the sizer to the globalTab
    globalTab->SetSizer(layerAdjustmentsSizer);
}

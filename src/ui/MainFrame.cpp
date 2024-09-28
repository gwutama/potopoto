#include "MainFrame.h"
#include "../ImageReader.h"
#include "../MetadataReader.h"
#include "LayerAdjustmentsPanel.h"


MainFrame::MainFrame(const wxString &title)
        : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)) {

    imagePreview = std::make_shared<ImagePreview>();
    editor = new ImageEditor(static_cast<wxWindow *>(this), imagePreview);
    editor->Disable();  // Disable the editor until an image is loaded

    CreateMenuBar();
    CreateRightPanel();

    rightPanel->Disable();  // Disable the right panel until an image is loaded

    wxBoxSizer *mainSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(editor, 1, wxEXPAND | wxALL, 5);
    mainSizer->Add(rightPanel, 0, wxEXPAND | wxALL, 5);
    SetSizer(mainSizer);

    Bind(EVT_ADJUSTMENT_SLIDER_VALUE_CHANGED, &MainFrame::OnAdjustmentSliderValueChanged, this);
    Bind(EVT_ADJUSTMENT_SLIDER_MOUSE_RELEASED_VALUE_CHANGED, &MainFrame::OnAdjustmentSliderMouseReleasedValueChanged, this);
}


void MainFrame::CreateRightPanel() {
    rightPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(400, -1));

    imageAnalysisPanel = new ImageAnalysisPanel(rightPanel);
    imageAdjustmentsPanel = new ImageAdjustmentsPanel(rightPanel);

    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(imageAnalysisPanel, 0, wxEXPAND | wxALL, 5);
    sizer->Add(imageAdjustmentsPanel, 1, wxEXPAND | wxALL, 5);

    rightPanel->SetSizer(sizer);
    rightPanel->Layout();  // Make sure the sizer gets applied
}


void MainFrame::OnOpen(wxCommandEvent &event) {
    wxFileDialog openFileDialog(this, _("Open Image file"), "", "",
                                "Image files (*.png;*.jpg;*.jpeg;*.bmp)|*.png;*.jpg;*.jpeg;*.bmp",
                                wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL) {
        return;
    }

    wxString filePath = openFileDialog.GetPath();
    std::string filename = filePath.ToStdString();

    auto imageUmat = std::make_shared<cv::UMat>();

    if (!ImageReader::Open(filename, imageUmat)) {
        wxMessageBox("Failed to load image", "Error", wxOK | wxICON_ERROR);
        return;
    }

    image = std::make_shared<Image>(imageUmat);
    imageHistogram = std::make_shared<ImageHistogram>(imageUmat);

    editor->LoadImage(image);
    imageAnalysisPanel->GetHistogramCanvas()->SetHistogramData(imageHistogram->GetHistogram());
    imageAnalysisPanel->GetImageInfoPanel()->SetData(image->GetImageInfo());

    MetadataReader metadataReader;
    metadataReader.Load(filename);
    imageAnalysisPanel->GetExifMetadataPanel()->SetData(metadataReader.GetExifMetadata());
    imageAnalysisPanel->GetFileInfoPanel()->SetData(metadataReader.GetFileInfo());

    editor->Enable();
    rightPanel->Enable();
}


void MainFrame::OnClose(wxCommandEvent &event) {
    editor->Disable();
    rightPanel->Disable();
    editor->Reset();
    imageAnalysisPanel->Reset();
    imageAdjustmentsPanel->Reset();
}


void MainFrame::OnAdjustmentSliderValueChanged(wxCommandEvent &event) {
    auto adjustments = static_cast<std::shared_ptr<AdjustmentsParameters>*>(event.GetClientData());

    // Update visible region of the image
    editor->GetImagePreview()->AdjustParameters(*adjustments);
    cv::Rect visibleRegion = editor->GetImageCanvas()->GetVisibleImageRegion();
    editor->GetImagePreview()->ApplyAdjustmentsForPreviewRegion(visibleRegion);
    editor->GetImageCanvas()->UpdateTexture();
    editor->GetImageCanvas()->Refresh();

    // Update histogram
    imageHistogram->AdjustParameters(*adjustments);
    imageHistogram->ApplyAdjustments();
    imageAnalysisPanel->GetHistogramCanvas()->SetHistogramData(imageHistogram->GetHistogram());
    imageAnalysisPanel->GetImageInfoPanel()->SetData(image->GetImageInfo());
}


void MainFrame::OnAdjustmentSliderMouseReleasedValueChanged(wxCommandEvent &event) {
    auto onSuccess = [this]() {
        std::cout << "All LOD adjustments have been successfully applied!" << std::endl;

        // Ensure UpdateTexture is called on the main thread
        this->CallAfter([this]() {
            editor->GetImageCanvas()->UpdateTexture();  // Update the texture after all adjustments are done
            editor->GetImageCanvas()->Refresh();        // Trigger a canvas refresh to see the changes
        });
    };

    // Update all LODs in the background
    editor->GetImagePreview()->ApplyAdjustmentsForAllLodsAsync(onSuccess);  // Apply async adjustments
}


void MainFrame::CreateMenuBar() {
    wxMenuBar *menuBar = new wxMenuBar();
    wxMenu *fileMenu = new wxMenu();

    fileMenu->Append(wxID_OPEN, "&Open\tCtrl-O", "Open an image file");
    fileMenu->Append(wxID_CLOSE, "&Close\tCtrl-W", "Close the current window");
    fileMenu->Append(wxID_EXIT, "&Quit\tCtrl-Q", "Quit the application");

    Bind(wxEVT_MENU, &MainFrame::OnOpen, this, wxID_OPEN);
    Bind(wxEVT_MENU, &MainFrame::OnClose, this, wxID_CLOSE);
    Bind(wxEVT_MENU, [this](wxCommandEvent &) { Close(true); }, wxID_EXIT);

    menuBar->Append(fileMenu, "&File");
    SetMenuBar(menuBar);
}

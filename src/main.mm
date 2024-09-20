#include <opencv2/opencv.hpp>
#include <wx/wx.h>
#include <wx/notebook.h>

#include "ui/ImageEditor2.h"
#include "ui/ImageAnalysisPanel.h"
#include "ui/ImageAdjustmentsPanel.h"

#include "ImagePreview2.h"
#include "ImageReader.h"
#include "Image.h"

#ifdef __WXMAC__
#import <AppKit/AppKit.h>
#endif

class MyApp : public wxApp {
public:
    virtual bool OnInit();
    void ForceDarkMode();
};

class MyFrame : public wxFrame {
public:
    MyFrame(const wxString &title);

private:
    std::shared_ptr<ImagePreview2> imagePreview;

    ImageEditor2 *editor;
    wxPanel *rightPanel;
    ImageAnalysisPanel *imageAnalysisPanel;
    ImageAdjustmentsPanel *imageAdjustmentsPanel;

    void OnOpen(wxCommandEvent &event);
    void OnClose(wxCommandEvent &event);
    void CreateMenuBar();
    void CreateRightPanel();
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
    // Force dark mode on macOS
    ForceDarkMode();

    MyFrame *frame = new MyFrame("potopoto");
    frame->Show(true);
    frame->Maximize(true);  // Maximize the window on start
    return true;
}

void MyApp::ForceDarkMode() {
#ifdef __WXMAC__
    [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"NSRequiresAquaSystemAppearance"];
    [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"AppleInterfaceStyleSwitchesAutomatically"];
    [[NSUserDefaults standardUserDefaults] setObject:@"Dark" forKey:@"AppleInterfaceStyle"];
    NSApp.appearance = [NSAppearance appearanceNamed:NSAppearanceNameDarkAqua];
#endif
}

MyFrame::MyFrame(const wxString &title)
        : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)) {

    imagePreview = std::make_shared<ImagePreview2>();
    editor = new ImageEditor2(static_cast<wxWindow*>(this), imagePreview);
    editor->Disable();  // Disable the editor until an image is loaded

    CreateMenuBar();
    CreateRightPanel();

    // Set up the layout using sizers
    wxBoxSizer *mainSizer = new wxBoxSizer(wxHORIZONTAL);

    mainSizer->Add(editor, 1, wxEXPAND | wxALL, 5);
    mainSizer->Add(rightPanel, 0, wxEXPAND | wxALL, 5);

    SetSizer(mainSizer);
}

void MyFrame::CreateRightPanel() {
    rightPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(400, -1));

    imageAnalysisPanel = new ImageAnalysisPanel(rightPanel);
    imageAdjustmentsPanel = new ImageAdjustmentsPanel(rightPanel);

    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(imageAnalysisPanel, 0, wxEXPAND | wxALL, 5);
    sizer->Add(imageAdjustmentsPanel, 1, wxEXPAND | wxALL, 5);

    rightPanel->SetSizer(sizer);
    rightPanel->Layout();  // Make sure the sizer gets applied
}

void MyFrame::OnOpen(wxCommandEvent &event) {
    wxFileDialog openFileDialog(this, _("Open Image file"), "", "",
                                "Image files (*.png;*.jpg;*.jpeg;*.bmp)|*.png;*.jpg;*.jpeg;*.bmp",
                                wxFD_OPEN | wxFD_FILE_MUST_EXIST);

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return;

    wxString filePath = openFileDialog.GetPath();
    std::string filename = filePath.ToStdString();

    auto myImage = std::make_shared<cv::UMat>();

    if (!ImageReader::Open(filename, myImage)) {
        wxMessageBox("Failed to load image", "Error", wxOK | wxICON_ERROR);
        return;
    }

    auto image = std::make_shared<Image>(myImage);
    editor->LoadImage(image);
    editor->Enable();
}

void MyFrame::OnClose(wxCommandEvent &event) {
    editor->Disable();
    editor->Reset();
}

void MyFrame::CreateMenuBar() {
    wxMenuBar *menuBar = new wxMenuBar();
    wxMenu *fileMenu = new wxMenu();

    fileMenu->Append(wxID_OPEN, "&Open\tCtrl-O", "Open an image file");
    fileMenu->Append(wxID_CLOSE, "&Close\tCtrl-W", "Close the current window");
    fileMenu->Append(wxID_EXIT, "&Quit\tCtrl-Q", "Quit the application");

    Bind(wxEVT_MENU, &MyFrame::OnOpen, this, wxID_OPEN);
    Bind(wxEVT_MENU, &MyFrame::OnClose, this, wxID_CLOSE);
    Bind(wxEVT_MENU, [this](wxCommandEvent &) { Close(true); }, wxID_EXIT);

    menuBar->Append(fileMenu, "&File");
    SetMenuBar(menuBar);
}

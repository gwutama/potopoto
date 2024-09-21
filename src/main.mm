#include <opencv2/opencv.hpp> // needs to be here otherwise it will cause conflict with the objc compiler (?)
#include <wx/wx.h>

#ifdef __WXMAC__
#import <AppKit/AppKit.h>
#endif

#import "ui/MainFrame.h"

class MyApp : public wxApp {
public:
    virtual bool OnInit();
    void ForceDarkMode();
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit() {
    // Force dark mode on macOS
    ForceDarkMode();

    MainFrame *frame = new MainFrame("potopoto");
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

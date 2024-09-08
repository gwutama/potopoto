#include "WindowManager.h"
#include "ImageEditor.h"
#include "exiv2.hpp"

int main(int argc, char* argv[]) {
    Exiv2::XmpParser::initialize();
    ::atexit(Exiv2::XmpParser::terminate);

    // Initialize window and OpenGL context
    if (!InitializeWindow()) {
        return -1;
    }

    ImageEditor editor;

    // Main Loop
    bool done = false;
    while (!done) {
        done = ProcessEvents();

        // Start ImGui Frame
        StartImGuiFrame();

        // Render the GUI
        editor.Render();

        // Rendering
        RenderFrame();
    }

    // Cleanup
    Cleanup();

    return 0;
}

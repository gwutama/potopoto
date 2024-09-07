#include "WindowManager.h"
#include "ImageEditor.h"

int main(int argc, char* argv[]) {
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

#include "WindowManager.h"
#include "ui/ImageEditor.h"
#include "exiv2.hpp"
#include <opencv2/core/ocl.hpp>
#include <omp.h>

int main(int argc, char* argv[]) {
    Exiv2::XmpParser::initialize();
    ::atexit(Exiv2::XmpParser::terminate);

    // Check for OpenCL support
    if (cv::ocl::haveOpenCL()) {
        cv::ocl::setUseOpenCL(true);  // Enable OpenCL
        std::cout << "OpenCL is available and enabled." << std::endl;
    } else {
        std::cout << "OpenCL is not available." << std::endl;
    }

    // Check for OpenMP support
    if (omp_get_max_threads() > 1) {
        std::cout << "OpenMP is available and enabled." << std::endl;
    } else {
        std::cout << "OpenMP is not available." << std::endl;
    }

    if (cv::useOptimized()) {
        std::cout << "OpenCV is using optimized code paths." << std::endl;
    } else {
        std::cout << "OpenCV is not using optimized code paths." << std::endl;
    }

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

        // limit to 30 fps
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }

    // Cleanup
    Cleanup();

    return 0;
}

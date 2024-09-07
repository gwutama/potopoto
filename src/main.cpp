#include <SDL.h>
#include <SDL_opengl.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include "tinyfiledialogs.h" // Include tinyfiledialogs

#include <algorithm> // Include for std::clamp
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// OpenGL texture handle
GLuint image_texture = 0;
int image_width = 0, image_height = 0;
float zoom = 1.0f; // Zoom factor
ImVec2 image_offset = ImVec2(0.0f, 0.0f); // Offset for image panning

// Enum to represent active tool
enum class ActiveTool { None, Zoom, Hand };
ActiveTool active_tool = ActiveTool::Zoom; // Default tool is Zoom

// Function to load an image using stb_image
bool LoadImage(const char* filename) {
    // Free existing texture if any
    if (image_texture) {
        glDeleteTextures(1, &image_texture);
        image_texture = 0;
    }

    // Load image using stb_image
    int channels;
    unsigned char* data = stbi_load(filename, &image_width, &image_height, &channels, 4); // Force RGBA
    if (data == nullptr) {
        printf("Failed to load image: %s\n", filename);
        return false;
    }

    // Generate OpenGL texture
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Free image data
    stbi_image_free(data);
    return true;
}

void SaveImage() {
    // TODO: Implement image saving logic here
}

// Function to display EXIF information (dummy example)
void DisplayExifInfo() {
    ImGui::Text("Size: 1920x1080");
    ImGui::Text("Date Time: 2024-09-01 12:00:00");
    ImGui::Text("Camera: Canon EOS 5D Mark IV");
    ImGui::Text("Focal Length: 50mm");
    ImGui::Text("Aperture: f/1.8");
    ImGui::Text("ISO: 100");
}

// Function to handle the image viewer behavior based on active tool
void HandleImageViewer() {
    ImGuiIO& io = ImGui::GetIO();

    // If the zoom tool is active and the window is hovered, handle zooming
    if (active_tool == ActiveTool::Zoom) {
        if (io.MouseWheel != 0.0f) {
            float zoom_factor = 1.1f; // Zoom speed factor
            if (io.MouseWheel > 0.0f) {
                zoom *= zoom_factor;  // Zoom in
            } else if (io.MouseWheel < 0.0f) {
                zoom /= zoom_factor;  // Zoom out
            }
            zoom = std::clamp(zoom, 0.1f, 10.0f); // Limit zoom between 10% and 1000%

            // Prevent default scrolling behavior
            io.WantCaptureMouse = true;
        }
    }

    // Image Viewer Scrollable Region
    ImVec2 image_size = ImVec2(image_width * zoom, image_height * zoom);
    ImVec2 window_size = ImGui::GetContentRegionAvail();

    // Begin the image viewer area with scrollbars
    ImGui::BeginChild("Image Viewer", window_size, false, ImGuiWindowFlags_HorizontalScrollbar);

    // Drag to pan if the hand tool is active
    if (active_tool == ActiveTool::Hand) {
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            ImVec2 drag_delta = ImGui::GetMouseDragDelta();

            // Adjust the scrollbars of the child window
            ImGui::SetScrollX(ImGui::GetScrollX() - drag_delta.x);
            ImGui::SetScrollY(ImGui::GetScrollY() - drag_delta.y);

            ImGui::ResetMouseDragDelta();   // Reset the drag delta after applying it
        }
    }

    // Calculate the new cursor position with offset
    ImVec2 cursor_pos = ImVec2((window_size.x - image_size.x) * 0.5f + image_offset.x, (window_size.y - image_size.y) * 0.5f + image_offset.y);
    cursor_pos.x = std::max(cursor_pos.x, 0.0f);
    cursor_pos.y = std::max(cursor_pos.y, 0.0f);

    // Apply the calculated cursor position and draw the image
    ImGui::SetCursorPos(cursor_pos);
    ImGui::Image((void*)(intptr_t)image_texture, image_size);
    ImGui::EndChild();
}


// Function to render the vertical toolbar
void RenderToolbar() {
    ImGui::BeginChild("Toolbar", ImVec2(50, 0), false); // Fixed width toolbar

    // Zoom tool button
    if (ImGui::Selectable("Zoom", active_tool == ActiveTool::Zoom)) {
        active_tool = ActiveTool::Zoom;
    }

    // Hand tool button
    if (ImGui::Selectable("Hand", active_tool == ActiveTool::Hand)) {
        active_tool = ActiveTool::Hand;
    }

    ImGui::EndChild();
}

// Function to render the image editor GUI
void RenderImageEditor() {
    static float brightness = 1.0f;
    static float contrast = 1.0f;
    static int filter = 0; // 0: None, 1: Blur, 2: Sharpen, etc.

    // Main Menu Bar
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::MenuItem("Open")) {
            // Use tinyfiledialogs to open a file browser dialog
            const char* filename = tinyfd_openFileDialog(
                    "Open Image",                // Title of the dialog
                    "",                          // Default path
                    0,                           // Number of filters
                    NULL,                        // Filters (e.g., "*.png")
                    NULL,                        // Filter description
                    0                            // Allow multiple selections
            );

            if (filename) {
                LoadImage(filename); // Load the selected image
            }
        }

        if (ImGui::MenuItem("Save")) { SaveImage(); }
        if (ImGui::MenuItem("Close")) { SDL_Event quit_event; quit_event.type = SDL_QUIT; SDL_PushEvent(&quit_event); }
        ImGui::EndMainMenuBar();
    }

    // Adjust for the menu bar height
    ImVec2 main_window_pos = ImVec2(0, ImGui::GetFrameHeight()); // Start position below the menu bar
    ImVec2 main_window_size = ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y - ImGui::GetFrameHeight());

    // Set the next window position and size to cover the entire screen below the menu bar
    ImGui::SetNextWindowPos(main_window_pos);
    ImGui::SetNextWindowSize(main_window_size);

    // Create the main window layout
    ImGui::Begin("Main Layout", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

    // Render vertical toolbar on the left
    RenderToolbar();

    ImGui::SameLine(); // Separate the toolbar and the image viewer

    // Calculate available size and split it into left and right parts
    ImVec2 available_size = ImGui::GetContentRegionAvail();
    float left_pane_width = available_size.x * 0.75f; // 75% for the left pane
    float right_pane_width = available_size.x - left_pane_width; // Remaining for the right pane

    // Left Pane - Image Viewer with zoom and scroll
    ImGui::BeginChild("LeftPane", ImVec2(left_pane_width, available_size.y), true);
    if (image_texture) {
        HandleImageViewer();
    } else {
        ImGui::Text("No image loaded");
    }
    ImGui::EndChild();

    // Right Pane
    ImGui::SameLine(); // Place the next child window beside the left pane
    ImGui::BeginChild("RightPane", ImVec2(right_pane_width, available_size.y), true);

    // Top: Tabs for Histogram and Image Info
    ImGui::BeginChild("Tabs", ImVec2(0, available_size.y * 0.3f), true); // Use 60% of height for the top section
    if (ImGui::BeginTabBar("TabBar")) {
        if (ImGui::BeginTabItem("Histogram")) {
            ImGui::Text("Histogram data goes here"); // Placeholder for histogram data
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Image Info")) {
            DisplayExifInfo();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::EndChild();

    // Bottom: Image Adjustments
    ImGui::BeginChild("Adjustments", ImVec2(0, 0), true); // Remaining height for the bottom section
    ImGui::Text("Adjustments");
    ImGui::SliderFloat("Brightness", &brightness, 0.0f, 2.0f);
    ImGui::SliderFloat("Contrast", &contrast, 0.0f, 2.0f);

    const char* filters[] = { "None", "Blur", "Sharpen" };
    ImGui::Combo("Filter", &filter, filters, IM_ARRAYSIZE(filters));
    ImGui::EndChild();

    ImGui::EndChild(); // End Right Pane
    ImGui::End(); // End Main Layout
}

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Create SDL Window with OpenGL context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_Window* window = SDL_CreateWindow("Image Editor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable VSync

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Main Loop
    bool done = false;
    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
        }

        // Start ImGui Frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Render the GUI
        RenderImageEditor();

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

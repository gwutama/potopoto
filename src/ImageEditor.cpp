#include "ImageEditor.h"
#include "ImageLoader.h"
#include "Toolbar.h"
#include "tinyfiledialogs.h"
#include <iostream>
#include <algorithm> // For std::clamp and std::max
#include <sys/stat.h>
#include <ctime>
#include <sstream>
#include <SDL.h>     // Include SDL for SDL_Event
#include <opencv2/opencv.hpp> // Include OpenCV for image processing
#include <implot.h>  // Include ImPlot for plotting histograms
#include <exiv2/exiv2.hpp>

// Global OpenCV image object to store the loaded image
cv::Mat image;

ImageEditor::ImageEditor()
        : image_texture(0), image_width(0), image_height(0), zoom(1.0f), image_offset(ImVec2(0.0f, 0.0f)), active_tool(ActiveTool::Zoom) {}

void ImageEditor::Render() {
    RenderImageEditor();
}

void ImageEditor::RenderImageEditor() {
    static float brightness = 1.0f;
    static float contrast = 1.0f;
    static int filter = 0; // 0: None, 1: Blur, 2: Sharpen, etc.

    // Main Menu Bar
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::MenuItem("Open")) {
            const char* filename = tinyfd_openFileDialog(
                    "Open Image", "", 0, NULL, NULL, 0);
            if (filename) {
                LoadImage(filename);
            }
        }

        if (ImGui::MenuItem("Save")) {
            SaveImage();
        }

        if (ImGui::MenuItem("Close")) {
            SDL_Event quit_event;
            quit_event.type = SDL_QUIT;
            SDL_PushEvent(&quit_event);
        }

        ImGui::EndMainMenuBar();
    }

    // Adjust for the menu bar height
    ImVec2 main_window_pos = ImVec2(0, ImGui::GetFrameHeight());
    ImVec2 main_window_size = ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y - ImGui::GetFrameHeight());

    ImGui::SetNextWindowPos(main_window_pos);
    ImGui::SetNextWindowSize(main_window_size);

    ImGui::Begin("Main Layout", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

    RenderToolbar();

    ImGui::SameLine();

    ImVec2 available_size = ImGui::GetContentRegionAvail();
    float left_pane_width = available_size.x * 0.75f;
    float right_pane_width = available_size.x - left_pane_width;

    ImGui::BeginChild("LeftPane", ImVec2(left_pane_width, available_size.y), false);
    if (image_texture) {
        HandleImageViewer();
    } else {
        ImGui::Text("No image loaded");
    }
    ImGui::EndChild();

    ImGui::SameLine();
    ImGui::BeginChild("RightPane", ImVec2(right_pane_width, available_size.y), false);

    ImGui::BeginChild("Tabs", ImVec2(0, available_size.y * 0.3f), true);
    if (ImGui::BeginTabBar("TabBar")) {
        if (ImGui::BeginTabItem("Histogram")) {
            // Disable mouse interactions
            ImGui::BeginDisabled();
            CalculateHistogram();
            ImGui::EndDisabled();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Info")) {
            DisplayImageInfo();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("EXIF")) {
            DisplayExifMetadata();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
    ImGui::EndChild();

    ImGui::BeginChild("Adjustments", ImVec2(0, 0), true);
    ImGui::Text("Adjustments");
    ImGui::SliderFloat("Brightness", &brightness, 0.0f, 2.0f);
    ImGui::SliderFloat("Contrast", &contrast, 0.0f, 2.0f);

    const char* filters[] = { "None", "Blur", "Sharpen" };
    ImGui::Combo("Filter", &filter, filters, IM_ARRAYSIZE(filters));
    ImGui::EndChild();

    ImGui::EndChild();
    ImGui::End();
}

void ImageEditor::LoadImage(const char* filename) {
    // clear the previous image data
    image.release();
    image_info.clear();
    image_exif.clear();

    // Load the image using OpenCV
    image = cv::imread(filename, cv::IMREAD_UNCHANGED);
    if (image.empty()) {
        std::cerr << "Error: Could not load image file: " << filename << std::endl;
        return;
    }

    // Convert to RGBA format if necessary
    if (image.channels() == 1) {
        cv::cvtColor(image, image, cv::COLOR_GRAY2RGBA);
    } else if (image.channels() == 3) {
        cv::cvtColor(image, image, cv::COLOR_BGR2RGBA);
    } else if (image.channels() != 4) {
        std::cerr << "Error: Unsupported image format with " << image.channels() << " channels." << std::endl;
        return;
    }

    image_width = image.cols;
    image_height = image.rows;

    // Load the image as OpenGL texture
    image_texture = LoadImageFromFile(filename, &image_width, &image_height);

    // Extract image information
    GetImageInfo();
    GetFileInfo(filename);
    ExtractExifMetadata(filename);
}

void ImageEditor::SaveImage() {
    // Implement save functionality
}

void ImageEditor::HandleImageViewer() {
    ImGuiIO& io = ImGui::GetIO();

    // Image Viewer Scrollable Region
    ImVec2 image_size = ImVec2(image_width * zoom, image_height * zoom);
    ImVec2 window_size = ImGui::GetContentRegionAvail();

    ImGui::BeginChild("Image Viewer", window_size, false, ImGuiWindowFlags_HorizontalScrollbar);

    // Handle zooming if the Zoom tool is active and the window is hovered
    if (active_tool == ActiveTool::Zoom && ImGui::IsWindowHovered()) {
        if (io.MouseWheel != 0.0f) {
            float zoom_factor = 1.1f; // Zoom speed factor
            zoom = (io.MouseWheel > 0.0f) ? zoom * zoom_factor : zoom / zoom_factor;
            zoom = std::clamp(zoom, 0.1f, 10.0f);
            io.WantCaptureMouse = true;
        }
    }

    // Drag to pan if the hand tool is active
    if (active_tool == ActiveTool::Hand && ImGui::IsWindowHovered()) {
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            ImVec2 drag_delta = ImGui::GetMouseDragDelta();
            ImGui::SetScrollX(ImGui::GetScrollX() - drag_delta.x);
            ImGui::SetScrollY(ImGui::GetScrollY() - drag_delta.y);
            ImGui::ResetMouseDragDelta();
        }
    }

    // Render the image
    if (image_texture) {
        ImGui::Image((void*)(intptr_t)image_texture, image_size);
    } else {
        ImGui::Text("No image loaded");
    }

    ImGui::EndChild();
}

void ImageEditor::RenderToolbar() {
    RenderToolbarUI(active_tool);
}

void ImageEditor::DisplayImageInfo() {
    if (image_info.empty()) {
        ImGui::Text("No image information available.");
        return;
    }

    for (const auto& info : image_info) {
        ImGui::Text("%s", info.c_str());
    }
}

void ImageEditor::CalculateHistogram() {
    if (image.empty()) {
        ImGui::Text("No image loaded.");
        return;
    }

    // Convert image to RGB if necessary
    cv::Mat imageRGB;
    if (image.channels() == 4) {
        cv::cvtColor(image, imageRGB, cv::COLOR_RGBA2RGB);
    } else if (image.channels() == 3) {
        imageRGB = image;
    } else {
        ImGui::Text("Unsupported image format for histogram.");
        return;
    }

    // Separate the image into B, G, R planes
    std::vector<cv::Mat> bgr_planes;
    cv::split(imageRGB, bgr_planes);

    // Set the number of bins and range
    int histSize = 256;
    float range[] = { 0, 256 };
    const float* histRange = { range };
    bool uniform = true, accumulate = false;

    cv::Mat b_hist, g_hist, r_hist;

    // Compute the histograms
    cv::calcHist(&bgr_planes[0], 1, 0, cv::Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
    cv::calcHist(&bgr_planes[1], 1, 0, cv::Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
    cv::calcHist(&bgr_planes[2], 1, 0, cv::Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);

    // Normalize the histograms to [0,1] for display
    cv::normalize(b_hist, b_hist, 0, 1, cv::NORM_MINMAX);
    cv::normalize(g_hist, g_hist, 0, 1, cv::NORM_MINMAX);
    cv::normalize(r_hist, r_hist, 0, 1, cv::NORM_MINMAX);

    // Display the histogram using ImGui (ImPlot must be initialized before using this)
    ImVec2 plot_size = ImGui::GetContentRegionAvail();

    if (ImPlot::BeginPlot("", nullptr, nullptr, plot_size, ImPlotFlags_NoLegend | ImPlotFlags_NoMouseText)) {
        ImPlot::SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_NoTickLabels);
        ImPlot::SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_NoTickLabels);

        ImPlot::PushStyleColor(ImPlotCol_Line, IM_COL32(0, 0, 255, 255)); // Blue
        ImPlot::PlotLine("Blue", b_hist.ptr<float>(), histSize);
        ImPlot::PopStyleColor();

        ImPlot::PushStyleColor(ImPlotCol_Line, IM_COL32(0, 255, 0, 255)); // Green
        ImPlot::PlotLine("Green", g_hist.ptr<float>(), histSize);
        ImPlot::PopStyleColor();

        ImPlot::PushStyleColor(ImPlotCol_Line, IM_COL32(255, 0, 0, 255)); // Red
        ImPlot::PlotLine("Red", r_hist.ptr<float>(), histSize);
        ImPlot::PopStyleColor();

        ImPlot::EndPlot();
    }
}

void ImageEditor::GetImageInfo() {
    if (image.empty()) {
        std::cerr << "No image loaded." << std::endl;
        return;
    }

    image_info.clear();
    image_info.push_back("Width: " + std::to_string(image.cols));
    image_info.push_back("Height: " + std::to_string(image.rows));
    image_info.push_back("Channels: " + std::to_string(image.channels()));
    image_info.push_back("Number of Pixels: " + std::to_string(image.total()));
    image_info.push_back("Image Size: " + std::to_string(image.total() * image.elemSize()) + " bytes");
}


void ImageEditor::GetFileInfo(const char* filename) {
    struct stat file_stat;
    if (stat(filename, &file_stat) == 0) {
        // File size
        image_info.push_back("File Size: " + std::to_string(file_stat.st_size) + " bytes");

        // File type (extension)
        std::string file_name_str(filename);
        std::string file_extension = file_name_str.substr(file_name_str.find_last_of(".") + 1);
        image_info.push_back("File Type: " + file_extension);

        // Creation time
        std::time_t creation_time = file_stat.st_ctime;
        std::stringstream creation_time_str;
        creation_time_str << std::asctime(std::localtime(&creation_time));
        image_info.push_back("Created: " + creation_time_str.str());

        // Modification time
        std::time_t modification_time = file_stat.st_mtime;
        std::stringstream modification_time_str;
        modification_time_str << std::asctime(std::localtime(&modification_time));
        image_info.push_back("Modified: " + modification_time_str.str());
    } else {
        std::cerr << "Error: Could not retrieve file information for: " << filename << std::endl;
    }
}

void ImageEditor::ExtractExifMetadata(const std::string& filename) {
    try {
        Exiv2::Image::UniquePtr image = Exiv2::ImageFactory::open(filename);
        if (!image) {
            std::cerr << "Error: Could not open image file for metadata extraction: " << filename << std::endl;
            return;
        }
        image->readMetadata();
        Exiv2::ExifData &exifData = image->exifData();
        if (exifData.empty()) {
            std::cerr << "No Exif data found in the file: " << filename << std::endl;
            return;
        }

        for (Exiv2::ExifData::const_iterator i = exifData.begin(); i != exifData.end(); ++i) {
            std::string key = i->key();
            std::string value = i->value().toString();
            image_exif.push_back(key + ": " + value);
        }
    } catch (Exiv2::Error& e) {
        std::cerr << "Error reading EXIF data: " << e.what() << std::endl;
    }
}

void ImageEditor::DisplayExifMetadata() {
    if (image_exif.empty()) {
        ImGui::Text("No EXIF metadata available.");
        return;
    }

    for (const auto& info : image_exif) {
        ImGui::Text("%s", info.c_str());
    }
}
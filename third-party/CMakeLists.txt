# Add ImGui library
# Add ImPlot library
set(IMPLOT_SOURCES
        implot/implot.cpp
        implot/implot_items.cpp
)

set(IMGUI_SOURCES
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_demo.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/backends/imgui_impl_sdl2.cpp
        imgui/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES} ${IMPLOT_SOURCES})
target_include_directories(imgui PUBLIC imgui imgui/backends)


# SDL2
option(SDL_SHARED "Build shared library" OFF)
option(SDL_STATIC "Build static library" ON)
add_subdirectory(SDL)
target_link_libraries(imgui PUBLIC SDL2::SDL2)

# Find OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(imgui PUBLIC ${OPENGL_LIBRARIES} OpenGL::GLU)

# tinyfiledialogs
set(TINYFILEDIALOGS_SOURCES
        tinyfiledialogs-code/tinyfiledialogs.c
)

add_library(tinyfiledialogs STATIC ${TINYFILEDIALOGS_SOURCES})
target_include_directories(tinyfiledialogs PUBLIC tinyfiledialogs)

# OpenCV
set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/third-party/opencv)
set(WITH_OBSENSOR OFF)

# Add OpenCV as a subdirectory
add_subdirectory(opencv)

# Link OpenCV to the target
target_link_libraries(imgui PUBLIC ${OpenCV_LIBS})

# Exiv2
set(BUILD_SHARED_LIBS OFF)
set(EXIV2_ENABLE_XMP OFF)
set(EXIV2_ENABLE_PNG OFF)
set(EXIV2_ENABLE_BROTLI OFF)
set(EXIV2_ENABLE_INIH OFF)
#set(EXIV2_ENABLE_FILESYSTEM_ACCESS OFF)
set(EXIV2_BUILD_EXIV2_COMMAND OFF)

add_subdirectory(exiv2)
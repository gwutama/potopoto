# Add ImGui library
set(IMGUI_SOURCES
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_demo.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/backends/imgui_impl_sdl2.cpp
        imgui/backends/imgui_impl_opengl3.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})
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
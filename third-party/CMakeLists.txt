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
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
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
set(OPENCV_EXTRA_MODULES_PATH "${CMAKE_SOURCE_DIR}/third-party/opencv_contrib/modules" CACHE STRING "" FORCE)
set(WITH_OPENMP ON CACHE BOOL "" FORCE)
set(WITH_OPENCL ON CACHE BOOL "" FORCE)
set(WITH_OBSENSOR OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_PERF_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_JAVA OFF CACHE BOOL "" FORCE)
set(BUILD_OBJC OFF CACHE BOOL "" FORCE)

# Add OpenCV as a subdirectory
add_subdirectory(opencv)

# Link OpenCV to the target
target_link_libraries(imgui PUBLIC ${OpenCV_LIBS})

# Exiv2
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(EXIV2_ENABLE_XMP OFF CACHE BOOL "" FORCE)
set(EXIV2_ENABLE_PNG OFF CACHE BOOL "" FORCE)
set(EXIV2_ENABLE_BROTLI OFF CACHE BOOL "" FORCE)
set(EXIV2_ENABLE_INIH OFF CACHE BOOL "" FORCE)
set(EXIV2_ENABLE_FILESYSTEM_ACCESS ON CACHE BOOL "" FORCE)
set(EXIV2_BUILD_EXIV2_COMMAND OFF CACHE BOOL "" FORCE)

add_subdirectory(exiv2)

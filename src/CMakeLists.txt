# Find OpenGL
find_package(OpenGL REQUIRED)

# Add definition to silence deprecation warnings on macOS
if(APPLE)
    add_definitions(-DGL_SILENCE_DEPRECATION)
endif()

# Define the source files
set(SRC_FILES
        ImageEditor.cpp
        ImageLoader.cpp
        main.cpp
        Toolbar.cpp
        UiComponents.cpp
        WindowManager.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/third-party/stb)
include_directories(${CMAKE_SOURCE_DIR}/third-party/tinyfiledialogs-code)

# Add executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 ${OPENGL_LIBRARIES} OpenGL::GLU imgui tinyfiledialogs)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Define the source files
set(SRC_FILES
        main.cpp
)

include_directories(${CMAKE_SOURCE_DIR}/third-party/stb)
include_directories(${CMAKE_SOURCE_DIR}/third-party/tinyfiledialogs-code)

# Add executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 ${OPENGL_LIBRARIES} OpenGL::GLU imgui tinyfiledialogs)

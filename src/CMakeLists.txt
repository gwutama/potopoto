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
include_directories(${CMAKE_SOURCE_DIR}/third-party/implot)

include_directories(${CMAKE_BINARY_DIR}) # for generated opencv_modules.hpp
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv/include)
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv/modules/calib3d/include)
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv/modules/core/include)
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv/modules/dnn/include)
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv/modules/features2d/include)
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv/modules/flann/include)
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv/modules/highgui/include)
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv/modules/imgcodecs/include)
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv/modules/imgproc/include)
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv/modules/ml/include)
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv/modules/objdetect/include)
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv/modules/photo/include)
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv/modules/stitching/include)
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv/modules/video/include)
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv/modules/videoio/include)

# Add executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 ${OPENGL_LIBRARIES} OpenGL::GLU imgui tinyfiledialogs opencv_ts)

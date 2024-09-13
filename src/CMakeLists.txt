# Find OpenGL
find_package(OpenGL REQUIRED)

# Add definition to silence deprecation warnings on macOS
if(APPLE)
    add_definitions(-DGL_SILENCE_DEPRECATION)

    # Manually set OpenMP flags for macOS
    find_package(OpenMP REQUIRED)
    if(OpenMP_FOUND)
        set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp -I/opt/homebrew/include")
        set(OpenMP_C_LIB_NAMES "omp")
        set(OpenMP_omp_LIBRARY "/opt/homebrew/opt/libomp/lib/libomp.a")

        # Add the OpenMP flags
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    else()
        message(FATAL_ERROR "OpenMP not found!")
    endif()

    # Find Iconv manually for macOS
    find_library(ICONV_LIBRARY iconv)
    if(NOT ICONV_LIBRARY)
        message(FATAL_ERROR "Iconv library not found!")
    endif()
else()
    # Non-Apple platforms: normal OpenMP
    find_package(OpenMP REQUIRED)
    if(OpenMP_CXX_FOUND)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    endif()

    # Use find_package for Iconv on non-Apple platforms
    find_package(Iconv REQUIRED)
endif()

# Define the source files
set(SRC_FILES
        ui/Canvas.cpp
        ui/Enums.h
        ui/ExifMetadata.cpp
        ui/FileInfo.cpp
        ui/Histogram.cpp
        ui/ImageAdjustments.cpp
        ui/ImageEditor.cpp
        ui/ImageInfo.cpp
        ui/Menubar.cpp
        ui/Toolbar.cpp
        Image.cpp
        ImageAdjustmentsParameters.h
        ImageReader.cpp
        ImageUtils.cpp
        LayerBase.h
        LayerBrightnessContrast.cpp
        LayerHueSaturationValue.cpp
        main.cpp
        MetadataReader.cpp
        WindowManager.cpp
)

# Include directories
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

include_directories(${CMAKE_SOURCE_DIR}/third-party/exiv2/include)

include_directories(${CMAKE_SOURCE_DIR}/third-party/sigslot/include)

# Add executable
add_executable(potopoto ${SRC_FILES})

# Link libraries
target_link_libraries(potopoto PRIVATE
        SDL2::SDL2 ${OPENGL_LIBRARIES} OpenGL::GLU
        imgui tinyfiledialogs opencv_core opencv_imgproc opencv_imgcodecs
        Exiv2::exiv2lib ${ICONV_LIBRARY} ${OpenMP_omp_LIBRARY}
)

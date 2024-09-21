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

# Enable wxWidgets static linking
set(wxWidgets_USE_STATIC 1)

# Find wxWidgets, including the OpenGL (gl) component
find_package(wxWidgets REQUIRED COMPONENTS core base gl)

if(wxWidgets_USE_FILE) # not defined in CONFIG mode
    include(${wxWidgets_USE_FILE})
endif()

# Define the source files
set(SRC_FILES
        ui/HistogramCanvas.cpp
        ui/ImageCanvas.cpp
        ui/ImageEditor.cpp
        ui/ImageAnalysisPanel.cpp
        ui/ImageAdjustmentsPanel.cpp
        ui/TableDataPanel.cpp
        ui/LayerAdjustmentsPanel.cpp
        ui/MainFrame.cpp

        AdjustmentsParameters.h
        BackgroundTask.h
        Image.cpp
        ImageApplyAdjustmentsTask.cpp
        ImageHistogram.cpp
        ImagePreview.cpp
        ImageReader.cpp
        ImageUtils.cpp
        LayerBase.h
        LayerBrightnessContrast.cpp
        LayerCmyk.cpp
        LayerGamma.cpp
        LayerHighlight.cpp
        LayerHueSaturationValue.cpp
        LayerLightness.cpp
        LayerShadow.cpp
        LayerWhiteBalance.cpp
        main.mm
        MetadataReader.cpp
)

# Include directories
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
include_directories(${CMAKE_SOURCE_DIR}/third-party/opencv_contrib/modules/xphoto/include)

include_directories(${CMAKE_SOURCE_DIR}/third-party/exiv2/include)

include_directories(${wxWidgets_INCLUDE_DIRS})

# Add executable
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
        opencv_core opencv_imgproc opencv_imgcodecs opencv_xphoto
        OpenGL::GL Exiv2::exiv2lib ${ICONV_LIBRARY} ${OpenMP_omp_LIBRARY} ${wxWidgets_LIBRARIES}
)

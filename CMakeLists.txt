cmake_minimum_required(VERSION 3.10)
project(potopoto)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# TODO: don't rely on locally installed libomp
# Set environment variable for openmp
# export OpenMP_ROOT=$(brew --prefix)/opt/libomp
set(OpenMP_ROOT "/opt/homebrew/opt/libomp")

include_directories(${OpenMP_ROOT}/include)


# Include third-party libraries and source directory
add_subdirectory(third-party)
add_subdirectory(src)

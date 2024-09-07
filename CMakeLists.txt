cmake_minimum_required(VERSION 3.10)
project(potopoto)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include third-party libraries and source directory
add_subdirectory(third-party)
add_subdirectory(src)

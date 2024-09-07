#ifndef POTOPOTO_IMAGELOADER_H
#define POTOPOTO_IMAGELOADER_H

#ifdef __APPLE__
#include <OpenGL/gl.h> // Use the OpenGL framework for macOS
#else
#include <GL/gl.h>     // For other platforms, use the standard OpenGL header
#endif

#include <opencv2/opencv.hpp>

// Function to load an image from a file and create an OpenGL texture
GLuint LoadImageFromFile(const char* filename, int* width, int* height);

#endif //POTOPOTO_IMAGELOADER_H

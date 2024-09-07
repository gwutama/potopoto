#ifndef POTOPOTO_IMAGELOADER_H
#define POTOPOTO_IMAGELOADER_H

#pragma once

#ifdef __APPLE__
#include <OpenGL/gl.h> // For macOS
#else
#include <GL/gl.h>     // For other platforms
#endif

GLuint LoadImageFromFile(const char* filename, int* width, int* height);
void SaveImageToFile(const char* filename);


#endif //POTOPOTO_IMAGELOADER_H

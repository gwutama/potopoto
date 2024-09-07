#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "ImageLoader.h"
#include <iostream>

#ifdef __APPLE__
#include <OpenGL/gl.h> // For macOS
#else
#include <GL/gl.h>     // For other platforms
#endif

GLuint LoadImageFromFile(const char* filename, int* width, int* height) {
    GLuint texture = 0;
    int channels;
    unsigned char* data = stbi_load(filename, width, height, &channels, 4); // Force RGBA
    if (data == nullptr) {
        std::cerr << "Failed to load image: " << filename << std::endl;
        return 0;
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Load the texture data into OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *width, *height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // Free image data
    stbi_image_free(data);

    return texture;
}

void SaveImageToFile(const char* filename) {
    // TODO: Implement image saving logic here
}

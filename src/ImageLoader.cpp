#include "ImageLoader.h"
#include <opencv2/imgcodecs.hpp>  // For cv::imread
#include <opencv2/imgproc.hpp>    // For cv::cvtColor
#include <iostream>

#ifdef __APPLE__
#include <OpenGL/gl.h> // Use the OpenGL framework for macOS
#else
#include <GL/gl.h>     // For other platforms, use the standard OpenGL header
#endif

GLuint LoadImageFromFile(const char* filename, int* width, int* height) {
    // Load the image using OpenCV
    cv::Mat image = cv::imread(filename, cv::IMREAD_UNCHANGED);
    if (image.empty()) {
        std::cerr << "Error: Could not load image file: " << filename << std::endl;
        return 0;
    }

    // Convert the image to RGBA format (if it's not already)
    cv::Mat imageRGBA;
    if (image.channels() == 1) {
        cv::cvtColor(image, imageRGBA, cv::COLOR_GRAY2RGBA);
    } else if (image.channels() == 3) {
        cv::cvtColor(image, imageRGBA, cv::COLOR_BGR2RGBA);
    } else if (image.channels() == 4) {
        imageRGBA = image;
    } else {
        std::cerr << "Error: Unsupported image format with " << image.channels() << " channels." << std::endl;
        return 0;
    }

    *width = imageRGBA.cols;
    *height = imageRGBA.rows;

    // Generate an OpenGL texture ID
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload the image data to the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *width, *height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageRGBA.data);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}

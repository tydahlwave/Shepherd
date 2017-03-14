//
//  ImageLoader.h
//  Shepherd
//
//  Created by Tyler Dahl on 3/10/17.
//
//

#ifndef ImageLoader_h
#define ImageLoader_h

#include <iostream>
#include <string>

#include "STB/stb_image.h"

#include "Logger.h"

struct ImageProperties {
    ImageProperties() {};
    ImageProperties(int w, int h, int c, GLenum i, GLenum o, GLenum d)
        :width(w), height(h), channels(c), inputFormat(i), outputFormat(o), dataType(d) {};
    
    int width = 0;
    int height = 0;
    int channels = 0;
    GLenum inputFormat = GL_RGB;
    GLenum outputFormat = GL_RGB;
    GLenum dataType = GL_UNSIGNED_BYTE;
};

static ImageProperties LoadImage(std::string imagePath, unsigned char **image,
                          int *width = 0, int *height = 0, int *channels = 0, int desiredChannels = 0) {
    int _width, _height, _channels;
    *image = stbi_load(imagePath.c_str(), &_width, &_height, &_channels, desiredChannels);
    
    if (!*image) {
        Log(ERROR, "Failed to load image", __FILE__, __LINE__);
        return {};
    }
    
    std::cout << "Image Loaded: " << _width << "x" << _height << " channels: " << _channels << std::endl;
    
    GLenum inputFormat = GL_RGB;
    GLenum outputFormat = GL_RGB;
    if (_channels == 1) {
        inputFormat = GL_RED;
        outputFormat = GL_RED;
    } else if (_channels == 2) {
        inputFormat = GL_RG;
        outputFormat = GL_RG;
    } else if (_channels == 4) {
        inputFormat = GL_RGBA;
        outputFormat = GL_RGBA;
    }
    
    if (width) *width = _width;
    if (height) *height = _height;
    if (channels) *channels = _channels;
    return { _width, _height, _channels, inputFormat, outputFormat, GL_UNSIGNED_BYTE };
}

static ImageProperties LoadImage(std::string imagePath, unsigned short **image,
                          int *width = 0, int *height = 0, int *channels = 0, int desiredChannels = 0) {
    int _width, _height, _channels;
    *image = stbi_load_16(imagePath.c_str(), &_width, &_height, &_channels, desiredChannels);
    
    if (!*image) {
        Log(ERROR, "Failed to load image", __FILE__, __LINE__);
        return {};
    }
    
    std::cout << "Image Loaded: " << _width << "x" << _height << " channels: " << _channels << std::endl;
    
    GLenum inputFormat = GL_RGB16;
    GLenum outputFormat = GL_RGB;
    if (_channels == 1) {
        inputFormat = GL_R16;
        outputFormat = GL_RED;
    } else if (_channels == 2) {
        inputFormat = GL_RG16;
        outputFormat = GL_RG;
    } else if (_channels == 4) {
        inputFormat = GL_RGBA16;
        outputFormat = GL_RGBA;
    }
    
    if (width) *width = _width;
    if (height) *height = _height;
    if (channels) *channels = _channels;
    return { _width, _height, _channels, inputFormat, outputFormat, GL_UNSIGNED_SHORT };
}

static ImageProperties LoadImage(std::string imagePath, float **image,
                          int *width = 0, int *height = 0, int *channels = 0, int desiredChannels = 0) {
    int _width, _height, _channels;
    *image = stbi_loadf(imagePath.c_str(), &_width, &_height, &_channels, desiredChannels);
    
    if (!*image) {
        Log(ERROR, "Failed to load image", __FILE__, __LINE__);
        return {};
    }
    
    std::cout << "Image Loaded: " << _width << "x" << _height << " channels: " << _channels << std::endl;
    
    GLenum inputFormat = GL_RGB16F;
    GLenum outputFormat = GL_RGB;
    if (_channels == 1) {
        inputFormat = GL_R16F;
        outputFormat = GL_RED;
    } else if (_channels == 2) {
        inputFormat = GL_RG16F;
        outputFormat = GL_RG;
    } else if (_channels == 4) {
        inputFormat = GL_RGBA16F;
        outputFormat = GL_RGBA;
    }
    
    if (width) *width = _width;
    if (height) *height = _height;
    if (channels) *channels = _channels;
    return { _width, _height, _channels, inputFormat, outputFormat, GL_FLOAT };
}

static void FreeImage(void *img) {
    stbi_image_free(img);
}

#endif /* ImageLoader_h */

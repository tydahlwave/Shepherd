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
#include <fstream>
#include <string>

#include "STB/stb_image.h"
#include "STB/stb_image_write.h"

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

static ImageProperties LoadImage(std::string imagePath, unsigned char **image, int *width = 0, int *height = 0, int *channels = 0, int desiredChannels = 0) {
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

static ImageProperties LoadImage(std::string imagePath, unsigned short **image, int *width = 0, int *height = 0, int *channels = 0, int desiredChannels = 0) {
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

static ImageProperties LoadImage(std::string imagePath, float **image, int *width = 0, int *height = 0, int *channels = 0, int desiredChannels = 0) {
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

static ImageProperties LoadImageCustom(std::string imagePath, unsigned short **image, int *width, int *height) {
    int _width = -1, _height = -1;
    int _channels = 1;
    
    std::string line;
    std::ifstream file(imagePath);
    if (file.is_open()) {
        int index = 0;
        
        while (getline(file, line)) {
            int value = std::stoi(line);
            
            if (_width == -1) {
                _width = value;
            } else if (_height == -1) {
                _height = value;
                // After reading width and height, allocate size for the image
                int dataSize = _width *  _height;
                *image = (unsigned short *)calloc(dataSize, sizeof(unsigned short));
            } else {
                (*image)[index++] = (unsigned short)value;
            }
            
//            std::cout << value << '\n';
        }
        file.close();
    } else {
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
    return { _width, _height, _channels, inputFormat, outputFormat, GL_FLOAT };
}

static void SaveImageCustom(std::string fileName, int width, int height, int channels, const unsigned short *data) {
    std::ofstream file;
    file.open(fileName);
    
    if (!file.is_open()) {
        Log(ERROR, "Failed to open file", __FILE__, __LINE__);
        return;
    }
    
    file << width << std::endl;
    file << height << std::endl;
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            file << data[row * width + col] << std::endl;
        }
    }
    file.close();
}

static void SaveImage(std::string fileName, int width, int height, int channels, const float *data) {
    std::cout << "Write png data" << std::endl;
    stbi_write_hdr(fileName.c_str(), width, height, channels, data);
}

static void SaveImage(std::string fileName, int width, int height, int channels, void *data, GLenum type) {
    int dataTypeLength = 4;
    if (type == GL_BYTE || type == GL_UNSIGNED_BYTE) {
        dataTypeLength = 1;
    } else if (type == GL_SHORT || type == GL_UNSIGNED_SHORT) {
        dataTypeLength = 2;
    } else {
        dataTypeLength = 4;
    }
    
    stbi_write_png(fileName.c_str(), width, height, channels, data, width*channels*dataTypeLength, dataTypeLength);
//    stbi_write_bmp(fileName.c_str(), width, height, channels, data);
}

static void FreeImage(void *img) {
    stbi_image_free(img);
}

#endif /* ImageLoader_h */

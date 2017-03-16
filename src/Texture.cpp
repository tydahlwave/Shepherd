//
//  Texture.cpp
//  Shepherd
//
//  Created by Tyler Dahl on 3/6/17.
//
//

#include "Texture.h"

#include "STB/stb_image.h"

#include "ImageLoader.h"

Texture::Texture(std::string name) :name(name) {
    // Generate the texture reference id
    glGenTextures(1, &texID);
}

void Texture::Load(std::string filePath) {
    // Load the image
    unsigned char *image = nullptr;
    ImageProperties imageProps = LoadImage(filePath.c_str(), &image, &width, &height);
    
    // Load the texture
    Load(image, imageProps);
    
    // Free the image
    FreeImage(image);
}

void Texture::Load(void *image, ImageProperties imageProps) {
    // Bind the texture
    glBindTexture(GL_TEXTURE_2D, texID);
    
    // Set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, properties.wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, properties.wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, properties.minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, properties.magFilter);
    
    // Fix pixel alignment
    if (imageProps.inputFormat != GL_RGBA) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    } else if (imageProps.inputFormat == GL_RED && imageProps.dataType == GL_UNSIGNED_SHORT) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
    } else {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    }
    
    // Load and generate the texture
    glTexImage2D(GL_TEXTURE_2D, 0, imageProps.inputFormat, imageProps.width, imageProps.height,
                 0, imageProps.outputFormat, imageProps.dataType, image);
    if (properties.mipmapped) glGenerateMipmap(GL_TEXTURE_2D);
    
    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // Log GL Error if it occurs
    LogGLError(__FILE__, __LINE__);
}

void Texture::bind(GLuint unit) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texID);
}

void Texture::unbind(GLuint unit) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, 0);
}

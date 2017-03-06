//
//  Texture.cpp
//  Shepherd
//
//  Created by Tyler Dahl on 3/6/17.
//
//

#include "Texture.h"

#include "SOIL/SOIL.h"

Texture::Texture(std::string filePath) {
    this->filePath = filePath;
    
    // Generate the texture reference id
    glGenTextures(1, &texID);
}

void Texture::Load() {
    // Bind the texture
    glBindTexture(GL_TEXTURE_2D, texID);
    
    // Set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, properties.wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, properties.wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, properties.minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, properties.magFilter);
    
    // Load and generate the texture
    unsigned char* image = SOIL_load_image(filePath.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    if (properties.mipmapped) glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    
    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

//
//  Texture.h
//  Shepherd
//
//  Created by Tyler Dahl on 3/6/17.
//
//

#ifndef Texture_h
#define Texture_h

#include <string>

#include <GL/glew.h>

#include "ImageLoader.h"

struct TextureProperties {
    GLint wrapS = GL_REPEAT;
    GLint wrapT = GL_REPEAT;
    GLint minFilter = GL_LINEAR_MIPMAP_LINEAR;
    GLint magFilter = GL_LINEAR;
    bool mipmapped = true;
};

class Texture {
public:
    Texture(std::string name);
    
    GLuint texID;
    int width, height;
    std::string name;
    TextureProperties properties;
    
    void Load(std::string filePath);
    void Load(void *buffer, ImageProperties imageProps);
    void bind(GLuint unit = 0);
    void unbind(GLuint unit = 0);
};

#endif /* Texture_h */

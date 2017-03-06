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

struct TextureProperties {
    GLint wrapS = GL_REPEAT;
    GLint wrapT = GL_REPEAT;
    GLint minFilter = GL_LINEAR_MIPMAP_LINEAR;
    GLint magFilter = GL_LINEAR;
    bool mipmapped = true;
};

class Texture {
public:
    Texture(std::string name, std::string filePath);
    
    GLuint texID;
    int width;
    int height;
    std::string name;
    std::string filePath;
    TextureProperties properties;
    
    void Load();
};

#endif /* Texture_h */

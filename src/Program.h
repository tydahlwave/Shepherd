#pragma  once
#ifndef __Program__
#define __Program__

#include <map>
#include <string>

#define GLEW_STATIC
#include <GL/glew.h>

class TextureLoader;

class Program
{
public:
    Program();
    virtual ~Program();
    
    void setVerbose(bool v) { verbose = v; }
    bool isVerbose() const { return verbose; }
    
    void setShaderNames(const std::string &v, const std::string &f);
    void setShaderNames(const std::string &v, const std::string &g, const std::string &f);
    void linkGeometryShader(GLint rc, GLuint pid);
    virtual bool init();
    virtual void bind();
    virtual void unbind();
    
    void addAttribute(const std::string &name);
    void addTexture(TextureLoader *texture);
    void addUniform(const std::string &name);
    
    bool hasAttribute(const std::string &name);
    bool hasUniform(const std::string &name);
    
    GLint getAttribute(const std::string &name) const;
    GLint getUniform(const std::string &name) const;
    
    TextureLoader *getTexture(const std::string &name) const;
    
    GLuint getPID() { return pid; }
    
protected:
    std::string vShaderName; // Vertex shader
    std::string gShaderName; // Geometry shader
    std::string fShaderName; // Fragment shader
    
private:
    GLuint pid;
    std::map<std::string,GLint> attributes;
    std::map<std::string,GLint> uniforms;
    std::map<std::string,TextureLoader*> textures;
    bool verbose;
};

#endif

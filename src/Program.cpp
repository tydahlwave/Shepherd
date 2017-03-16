#include "Program.h"

#include <iostream>
#include <cassert>

#include "GLSL.h"
#include "TextureLoader.h"


using namespace std;

Program::Program() :
vShaderName(""),
gShaderName(""),
fShaderName(""),
pid(0),
verbose(true)
{
    
}

Program::~Program()
{
    
}

void Program::setShaderNames(const string &v, const string &f)
{
    vShaderName = v;
    fShaderName = f;
}

void Program::setShaderNames(const string &v, const string &g, const string &f)
{
    vShaderName = v;
    gShaderName = g;
    fShaderName = f;
}

void Program::linkGeometryShader(GLint rc, GLuint pid) {
    GLuint GS = glCreateShader(GL_GEOMETRY_SHADER);
    const char *gshader = GLSL::textFileRead(gShaderName.c_str());
    glShaderSource(GS, 1, &gshader, NULL);
    // Compile geometry shader
    glCompileShader(GS);
    glGetShaderiv(GS, GL_COMPILE_STATUS, &rc);
    if(!rc) {
        if(isVerbose()) {
            GLSL::printShaderInfoLog(GS);
            cout << "Error compiling geometry shader " << gShaderName << endl;
        }
        return;
    }
    glAttachShader(pid, GS);
}

bool Program::init()
{
    GLint rc;
    
    // Create shader handles
    GLuint VS = glCreateShader(GL_VERTEX_SHADER);
    GLuint FS = glCreateShader(GL_FRAGMENT_SHADER);
    
    // Read shader sources
    const char *vshader = GLSL::textFileRead(vShaderName.c_str());
    const char *fshader = GLSL::textFileRead(fShaderName.c_str());
    glShaderSource(VS, 1, &vshader, NULL);
    glShaderSource(FS, 1, &fshader, NULL);
    
    // Compile vertex shader
    glCompileShader(VS);
    glGetShaderiv(VS, GL_COMPILE_STATUS, &rc);
    if(!rc) {
        if(isVerbose()) {
            GLSL::printShaderInfoLog(VS);
            cout << "Error compiling vertex shader " << vShaderName << endl;
        }
        return false;
    }
    
    // Compile fragment shader
    glCompileShader(FS);
    glGetShaderiv(FS, GL_COMPILE_STATUS, &rc);
    if(!rc) {
        if(isVerbose()) {
            GLSL::printShaderInfoLog(FS);
            cout << "Error compiling fragment shader " << fShaderName << endl;
        }
        return false;
    }
    
    // Create the program and link
    pid = glCreateProgram();
    glAttachShader(pid, VS);
    if (gShaderName.length() > 0) {
        linkGeometryShader(rc, pid);
    }
    glAttachShader(pid, FS);
    glLinkProgram(pid);
    glGetProgramiv(pid, GL_LINK_STATUS, &rc);
    if(!rc) {
        if(isVerbose()) {
            GLSL::printProgramInfoLog(pid);
            cout << "Error linking shaders " << vShaderName << " and " << gShaderName << " and " << fShaderName << endl;
        }
        return false;
    }
    
    GLSL::printError();
    assert(glGetError() == GL_NO_ERROR);
    return true;
}

void Program::bind()
{
    glUseProgram(pid);
    for(map<string,TextureLoader*>::iterator it = textures.begin(); it != textures.end(); ++it) {
        it->second->bind();
    }
}

void Program::unbind()
{
    for(map<string,TextureLoader*>::iterator it = textures.begin(); it != textures.end(); ++it) {
        it->second->unbind();
    }
    glUseProgram(0);
}

void Program::addAttribute(const string &name)
{
    attributes[name] = GLSL::getAttribLocation(pid, name.c_str(), isVerbose());
}

void Program::addUniform(const string &name)
{
    uniforms[name] = GLSL::getUniformLocation(pid, name.c_str(), isVerbose());
}

void Program::addTexture(TextureLoader *texture)
{
    const string &name = texture->getName();
    GLint handle = GLSL::getUniformLocation(pid, name.c_str());
    texture->setHandle(handle);
    textures[name] = texture;
}

bool Program::hasAttribute(const std::string &name) {
    map<string,GLint>::const_iterator attribute = attributes.find(name.c_str());
    if (attribute == attributes.end()) {
        return false;
    }
    return true;
}

bool Program::hasUniform(const std::string &name) {
    map<string,GLint>::const_iterator uniform = uniforms.find(name.c_str());
    if (uniform == uniforms.end()) {
        return false;
    }
    return true;
}

GLint Program::getAttribute(const string &name) const
{
    map<string,GLint>::const_iterator attribute = attributes.find(name.c_str());
    if(attribute == attributes.end()) {
        if(isVerbose()) {
            cout << name << " is not an attribute variable" << endl;
        }
        return 0;
    }
    return attribute->second;
}

GLint Program::getUniform(const string &name) const
{
    map<string,GLint>::const_iterator uniform = uniforms.find(name.c_str());
    if(uniform == uniforms.end()) {
        if(isVerbose()) {
            cout << name << " is not a uniform variable" << endl;
        }
        return 0;
    }
    return uniform->second;
}

TextureLoader *Program::getTexture(const string &name) const
{
    map<string,TextureLoader*>::const_iterator texture = textures.find(name.c_str());
    if(texture == textures.end()) {
        cout << name << " is not a texture" << endl;
        return 0;
    }
    return texture->second;
}


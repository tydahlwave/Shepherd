//
//  Material.cpp
//  SimpleGame
//
//  Created by Tyler Dahl on 1/23/17.
//
//

#include "Material.h"

Material *Material::shinyBluePlastic = nullptr;
Material *Material::pearl = nullptr;
Material *Material::brass = nullptr;
Material *Material::copper = nullptr;
Material *Material::bronze = nullptr;
Material *Material::ruby = nullptr;
Material *Material::emerald = nullptr;
Material *Material::polishedGold = nullptr;

void Material::InitializeMaterials() {
    // Shiny blue plastic
    glm::vec4 ambient = glm::vec4(0.02, 0.04, 0.2, 1);
    glm::vec4 diffuse = glm::vec4(0, 0.16, 0.9, 1);
    glm::vec4 specular = glm::vec4(0.14, 0.2, 0.8, 1);
    float shine = 120.0;
    shinyBluePlastic = new Material(new Color(ambient), new Color(diffuse), new Color(specular), shine);
    
    // Pearl
    ambient = glm::vec4(0.25, 0.20725, 0.20725, 1);
    diffuse = glm::vec4(1, 0.829, 0.829, 1);
    specular = glm::vec4(0.296648, 0.296648, 0.296648, 1);
    shine = 11.264;
    pearl = new Material(new Color(ambient), new Color(diffuse), new Color(specular), shine);
    
    // Brass
    ambient = glm::vec4(0.3294, 0.2235, 0.02745, 1);
    diffuse = glm::vec4(0.7804, 0.5686, 0.11373, 1);
    specular = glm::vec4(0.9922, 0.941176, 0.80784, 1);
    shine = 27.9;
    brass = new Material(new Color(ambient), new Color(diffuse), new Color(specular), shine);
    
    // Copper
    ambient = glm::vec4(0.1913, 0.0735, 0.0225, 1);
    diffuse = glm::vec4(0.7038, 0.27048, 0.0828, 1);
    specular = glm::vec4(0.257, 0.1376, 0.08601, 1);
    shine = 12.8;
    copper = new Material(new Color(ambient), new Color(diffuse), new Color(specular), shine);
    
    // Bronze
    ambient = glm::vec4(0.2125, 0.1275, 0.054, 1);
    diffuse = glm::vec4(0.714, 0.4284, 0.18144, 1);
    specular = glm::vec4(0.393548, 0.271906, 0.166721, 1);
    shine = 25.6;
    bronze = new Material(new Color(ambient), new Color(diffuse), new Color(specular), shine);
    
    // Ruby
    ambient = glm::vec4(0.1745, 0.01175, 0.01175, 1);
    diffuse = glm::vec4(0.61424, 0.04136, 0.04136, 1);
    specular = glm::vec4(0.727811, 0.626959, 0.626959, 1);
    shine = 76.8;
    ruby = new Material(new Color(ambient), new Color(diffuse), new Color(specular), shine);
    
    // Emerald
    ambient = glm::vec4(0.0215, 0.1745, 0.0215, 1);
    diffuse = glm::vec4(0.07568, 0.61424, 0.07568, 1);
    specular = glm::vec4(0.633, 0.727811, 0.633, 1);
    shine = 76.8;
    emerald = new Material(new Color(ambient), new Color(diffuse), new Color(specular), shine);
    
    // Polished gold
    ambient = glm::vec4(0.24725, 0.2245, 0.0645, 1);
    diffuse = glm::vec4(0.34615, 0.3143, 0.0903, 1);
    specular = glm::vec4(0.797357, 0.723991, 0.208006, 1);
    shine = 83.2;
    polishedGold = new Material(new Color(ambient), new Color(diffuse), new Color(specular), shine);
}

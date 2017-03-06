//
//  Transform.h
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#ifndef Transform_h
#define Transform_h

#include "Component.h"
#include "../MathDef.h"
#include "../MatrixStack.h"

class Transform : public Component {
public:
    Transform() { name = "Transform"; };
    Transform(glm::vec3 p, glm::vec3 r, glm::vec3 s) :Transform() { position = p; rotation = r; scale = s; }
    virtual ~Transform() {};
    
    glm::vec3 GetPosition() { return position; }
    glm::vec3 GetRotation() { return rotation; }
    glm::vec3 GetScale() { return scale; }
    void SetPosition(glm::vec3 pos);
    void SetRotation(glm::vec3 rot);
    void SetScale(glm::vec3 sc);
    
    const glm::mat4 &GetMatrix() {
        MatrixStack stack = MatrixStack();
        stack.loadIdentity();
        stack.translate(position);
        stack.rotate(rotation.x/180*M_PI, glm::vec3(1, 0, 0));
        stack.rotate(rotation.y/180*M_PI, glm::vec3(0, 1, 0));
        stack.rotate(rotation.z/180*M_PI, glm::vec3(0, 0, 1));
        stack.scale(scale);
        return stack.topMatrix();
    }
    
    void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer) {
        writer.Key("Transform");
        writer.StartObject();
        writer.Key("Position");
        writer.StartArray();
        writer.Double(position.x);
        writer.Double(position.y);
        writer.Double(position.z);
        writer.EndArray();
        writer.Key("Rotation");
        writer.StartArray();
        writer.Double(rotation.x);
        writer.Double(rotation.y);
        writer.Double(rotation.z);
        writer.EndArray();
        writer.Key("Scale");
        writer.StartArray();
        writer.Double(scale.x);
        writer.Double(scale.y);
        writer.Double(scale.z);
        writer.EndArray();
        writer.EndObject();
    }
    
private:
    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::vec3 rotation = glm::vec3(0, 0, 0);
    glm::vec3 scale = glm::vec3(1, 1, 1);
};


#endif /* Transform_h */

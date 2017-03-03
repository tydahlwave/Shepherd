//
//  Serializer.cpp
//  Shepherd
//
//  Created by Michael Cantrell on 3/2/17.
//
//

#include "Serializer.h"
#include "World.h"
#include "GameObject.h"
#include "EntityFactory.h"

#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <fstream>
#include <sstream>


void Serializer::SerializeWorld(World *world) {
    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    
    writer.StartObject();
    writer.Key("GameObjects");
    writer.StartArray();
    for (int i = 0; i < world->GetGameObjects().size(); i++) {
        if(world->GetGameObjects()[i]->isSerializable) {
            if(world->GetGameObjects()[i]->name == "Light")
                world->GetGameObjects()[i]->Serialize(writer);
        }
    }
    writer.EndArray();
    writer.EndObject();
    
    puts(sb.GetString());
    std::ofstream myfile;
    myfile.open ("../../resources/jsons/savedWorld.json");
    myfile << sb.GetString();
    myfile.close();
}

Transform *Serializer::DeserializeTransform(rapidjson::Value &v) {
    assert(v.HasMember("Position"));
    rapidjson::Value& pos = v["Position"];
    vec3 position = vec3(pos[0].GetDouble(),pos[1].GetDouble(),pos[2].GetDouble());
    assert(v.HasMember("Rotation"));
    rapidjson::Value& rot = v["Rotation"];
    vec3 rotation = vec3(rot[0].GetDouble(),rot[1].GetDouble(),rot[2].GetDouble());
    assert(v.HasMember("Scale"));
    rapidjson::Value& sc = v["Scale"];
    vec3 scale = (vec3(sc[0].GetDouble(),sc[1].GetDouble(),sc[2].GetDouble()));
    return new Transform(position,rotation,scale);
}

Light *Serializer::DeserializeLightComponent(rapidjson::Value &v) {
    assert(v.HasMember("IsDirectional"));
    float isDirectional = v["IsDirectional"].GetDouble();
    assert(v.HasMember("Intensities"));
    rapidjson::Value& intense = v["Intensities"];
    vec3 intensities = vec3(intense[0].GetDouble(),intense[1].GetDouble(),intense[2].GetDouble());
    assert(v.HasMember("Attenuation"));
    float attenuation = v["Attenuation"].GetDouble();
    assert(v.HasMember("AmbientCoefficient"));
    float ambientCoefficient = v["AmbientCoefficient"].GetDouble();
    assert(v.HasMember("ConeAngle"));
    float coneAngle = v["ConeAngle"].GetDouble();
    assert(v.HasMember("ConeDirection"));
    rapidjson::Value& cd = v["ConeDirection"];
    vec3 coneDirection = (vec3(cd[0].GetDouble(),cd[1].GetDouble(),cd[2].GetDouble()));
    return new Light(isDirectional, intensities, attenuation, ambientCoefficient, coneAngle, coneDirection);
}

GameObject *Serializer::DeserializeSphere(rapidjson::Value &v, World *world) {
    rapidjson::Value& v2 = v["Components"]["Transform"];
    Transform *t = DeserializeTransform(v2);
    GameObject *sphere = EntityFactory::createSphere(world, 2, t->GetPosition(), 2);
    sphere->transform->SetPosition(t->GetPosition());
    sphere->transform->SetRotation(t->GetRotation());
    sphere->transform->SetScale(t->GetScale());
    return sphere;
}

GameObject *Serializer::DeserializeBoulder(rapidjson::Value &v, World *world) {
    rapidjson::Value& v2 = v["Components"]["Transform"];
    Transform *t = DeserializeTransform(v2);
    int type = rand() % 3;
    GameObject *boulder = EntityFactory::createBoulder(world, type, 1,t->GetPosition());
    boulder->transform->SetPosition(t->GetPosition());
    boulder->transform->SetRotation(t->GetRotation());
    boulder->transform->SetScale(t->GetScale());
    return boulder;
}

GameObject *Serializer::DeserializeTree(rapidjson::Value &v, World *world) {
    rapidjson::Value& v2 = v["Components"]["Transform"];
    Transform *t = DeserializeTransform(v2);
    int type = (rand() % 2) + 1;
    GameObject *tree = EntityFactory::createTree(world, type,t->GetPosition());
    tree->transform->SetPosition(t->GetPosition());
    tree->transform->SetRotation(t->GetRotation());
    tree->transform->SetScale(t->GetScale());
    return tree;
}

GameObject *Serializer::DeserializeLightObject(rapidjson::Value &v, World *world) {
    rapidjson::Value& v2 = v["Components"]["Transform"];
    Transform *t = DeserializeTransform(v2);
    Light *lightComp = DeserializeLightComponent(v["Components"]["Light"]);
    GameObject *lightObj = EntityFactory::createLight(world, t->GetPosition(), lightComp->isDirectional, lightComp->intensities, lightComp->attenuation, lightComp->ambientCoefficient, lightComp->coneAngle, lightComp->coneDirection);
    lightObj->transform->SetPosition(t->GetPosition());
    lightObj->transform->SetRotation(t->GetRotation());
    lightObj->transform->SetScale(t->GetScale());
    
    return lightObj;
}

void Serializer::DeserializeWorld(World *world) {
    std::fstream inFile;
    inFile.open("../../resources/jsons/loadedWorld.json");//open the input file
    std::stringstream strStream;
    strStream << inFile.rdbuf();//read the file
    std::string json = strStream.str();//str holds the content of the file

    rapidjson::Document d;
    d.Parse(json.c_str());
    
    assert(d.IsObject());
    assert(d.HasMember("GameObjects"));
    assert(d["GameObjects"].IsArray());
    for (int i = 0; i < d["GameObjects"].Size(); i++) {// Uses SizeType instead of size_t
        rapidjson::Value& v = d["GameObjects"][i];
        // i have now gotten 1 game object
        assert(v.HasMember("ObjectType"));
        GameObject *go;
        if(v["ObjectType"] == "Sphere") go = DeserializeSphere(v, world);
        else if(v["ObjectType"] == "Boulder") go = DeserializeBoulder(v, world);
        else if(v["ObjectType"] == "Tree") go = DeserializeTree(v, world);
        else if(v["ObjectType"] == "Light") go = DeserializeLightObject(v, world);
        else cout << "DESERIALIZATION OF THIS OBJECT TYPE NOT SUPPORTED\n";
        assert(v.HasMember("Static"));
        go->SetIsStatic(v["Static"].GetBool());
    }
}

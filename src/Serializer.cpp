//
//  Serializer.cpp
//  Shepherd
//
//  Created by Michael Cantrell on 3/2/17.
//
//

#include "Serializer.h"


void Serializer::SerializeWorld(World *world) {
    std::vector<std::string> mylist{"Sphere"};//, "Cube", "Boulder", "Tree"}; // types of objects to serialize
    
    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    
    writer.StartObject();
    writer.Key("GameObjects");
    writer.StartArray();
    for (int i = 0; i < world->gameObjects.size(); i++)
        if(std::find(std::begin(mylist), std::end(mylist), world->gameObjects[i]->name) != std::end(mylist))
            world->gameObjects[i]->Serialize(writer);
    writer.EndArray();
    writer.EndObject();
    
    puts(sb.GetString());
    std::ofstream myfile;
    myfile.open ("../../resources/jsons/savedWorld.json");
    myfile << sb.GetString();
    myfile.close();
}

Transform *Serializer::DeserializeTransform(rapidjson::Value &v) {
    Transform *t = new Transform();
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
    assert(d["GameObjects"].isArray());
    for (int i = 0; i < d["GameObjects"].Size(); i++) {// Uses SizeType instead of size_t
        rapidjson::Value& v = d["GameObjects"][i];
        // i have now gotten 1 game object
        assert(v.HasMember("ObjectType"));
        assert(v["ObjectType"] == "Sphere");
        rapidjson::Value& v2 = v["Components"]["Transform"];//["Position"];
        Transform *t = DeserializeTransform(v2);
        EntityFactory::createSphere(world, 2, t->GetPosition(), 2);
    }
}

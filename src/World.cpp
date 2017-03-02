//
//  World.cpp
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#include "EntityFactory.h"

World::World() :gameObjects() {
    mainCamera = EntityFactory::createMainCamera(this);
    mainCamera->transform->SetPosition(glm::vec3(0, 0, 0));
    
    ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
    
    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    btCollisionDispatcher* dispatcher = new	btCollisionDispatcher(collisionConfiguration);
    
    ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
    
    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
    
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,overlappingPairCache,solver,collisionConfiguration);
    
    dynamicsWorld->setGravity(btVector3(0,-50,0));
    
}


std::vector<GameObject*> World::GetGameObjects() {
    return gameObjects;
}

GameObject *World::CreateGameObject(std::string name) {
    GameObject *gameObject = new GameObject(name);
    gameObject->world = this;
    gameObjects.push_back(gameObject);
    return gameObject;
}

void World::ClearGameObjects() {
	gameObjects.clear();
}

void World::SerializeWorld() {
    std::vector<std::string> mylist{"Sphere", "Boulder", "Tree"}; // types of objects to serialize

    rapidjson::StringBuffer sb;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
    
    writer.StartObject();
    writer.Key("GameObjects");
    writer.StartArray();
    for (int i = 0; i < gameObjects.size(); i++)
        if(std::find(std::begin(mylist), std::end(mylist), gameObjects[i]->name) != std::end(mylist))
            gameObjects[i]->Serialize(writer);
    writer.EndArray();
    writer.EndObject();

    puts(sb.GetString());
    ofstream myfile;
    myfile.open ("../../resources/jsons/savedWorld.json");
    myfile << sb.GetString();
    myfile.close();
}

Transform *World::DeserializeTransform(rapidjson::Value &v) {
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

void World::DeserializeSphere(rapidjson::Value &v) {
    rapidjson::Value& v2 = v["Components"]["Transform"];
    Transform *t = DeserializeTransform(v2);
    EntityFactory::createSphere(this, 2, t->GetPosition(), 2);
}

void World::DeserializeBoulder(rapidjson::Value &v) {
    rapidjson::Value& v2 = v["Components"]["Transform"];
    Transform *t = DeserializeTransform(v2);
    int type = rand() % 3;
    GameObject *boulder = EntityFactory::createBoulder(this, type, 1,t->GetPosition());
    float scale = rand() % 4 + 1;
    boulder->transform->SetRotation(glm::vec3(0, rand() % 360, 0));
    boulder->transform->SetScale(vec3(scale));
}

void World::DeserializeTree(rapidjson::Value &v) {
    rapidjson::Value& v2 = v["Components"]["Transform"];
    Transform *t = DeserializeTransform(v2);
    int type = (rand() % 2) + 1;
    GameObject *tree = EntityFactory::createTree(this, type,t->GetPosition());
    tree->transform->SetPosition(t->GetPosition());
    tree->transform->SetRotation(glm::vec3(0, rand() % 360, 0));
    tree->transform->SetScale(glm::vec3(10, 10, 10));
    
}

void World::DeserializeWorld() {
    fstream inFile;
    inFile.open("../../resources/jsons/loadedWorld.json");//open the input file
    stringstream strStream;
    strStream << inFile.rdbuf();//read the file
    string json = strStream.str();//str holds the content of the file
    
    rapidjson::Document d;
    d.Parse(json.c_str());

    assert(d.IsObject());
    assert(d.HasMember("GameObjects"));
    assert(d["GameObjects"].isArray());
    for (int i = 0; i < d["GameObjects"].Size(); i++) {// Uses SizeType instead of size_t
        rapidjson::Value& v = d["GameObjects"][i];
        // i have now gotten 1 game object
        assert(v.HasMember("ObjectType"));
        if(v["ObjectType"] == "Sphere") DeserializeSphere(v);
        else if(v["ObjectType"] == "Boulder") DeserializeBoulder(v);
        else if(v["ObjectType"] == "Tree") DeserializeTree(v);
    }
}

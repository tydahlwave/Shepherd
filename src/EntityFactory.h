//
//  EntityFactory.h
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#ifndef EntityFactory_h
#define EntityFactory_h

#include "GameObject.h"
#include "World.h"
#include <cstring>

class Terrain;
class Model;
class Shader;
class Material;

class EntityFactory {
public:
    static GameObject *createMainCamera(World *world);
    static GameObject *createBunny(World *world);
    static GameObject *createWolf(World *world);
    static GameObject *createHUD(World *world);
	static GameObject *createHUD2(World *world);
	static GameObject *createTitle(World *world);
    static GameObject *createChargeBar(World *world);
    static GameObject *createTexturedSphere(World *world);
    static GameObject *createGround(World *world);
    static GameObject *createBarrier(World *world, glm::vec3 position, float size);
    static GameObject *createCube(World *world, glm::vec3 dimensions, glm::vec3 position, float mass);
    static GameObject *createSphere(World *world, float radius, glm::vec3 position, float mass);
    static GameObject *createPhysicsGround(World *world);
    static GameObject *createBoulder(World *world, int boulderType, float radius, vec3 position);
    static GameObject *createTerrain(World *world, std::string resourceDir, int type, int size, glm::vec3 pos);
	static GameObject *createPath(World *world, GameObject *terrain, int nodes);
    static GameObject *createPath(World *world, GameObject *terrainObject, std::vector<glm::vec3> positions);
    static GameObject *createTree(World *world, int type, glm::vec3 pos);
	static GameObject *createSkybox(World *world, std::string resourceDir);
    static GameObject *createTestAnim(World *world);
    static GameObject *createLight(World *world, glm::vec3 position, bool isDirectional, glm::vec3 intensities, float attenuation,float ambientCoefficient,float coneAngle, glm::vec3 coneDirection);
    static GameObject *upgradeCharacter(World *world, GameObject *camera, glm::vec3 pos);
    static void UpdateTerrain(World *world, GameObject *terrainObj, Terrain *terrain);
    static GameObject *createStaticObject(World *world, std::string name, Model *model, Shader *shader);
    static GameObject *createStaticObject(World *world, std::string name, Model *model, Shader *shader, Material *material);
    static GameObject *createStartMenuTerrain(World *world, std::string resourceDir, int type, int size, glm::vec3 pos);
    static GameObject *createNodeSphere(World *world);
    static GameObject *createRing(World *world);
    static float getTerrainHeightForPosition(GameObject *terrainObject, Terrain *terrain, int x, int z);
};

#endif /* EntityFactory_h */

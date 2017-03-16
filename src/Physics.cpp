//
//  Physics.cpp
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#include <iostream>

#include "Physics.h"
#include "Components/RigidBody.h"
#include "Components/BoxCollider.h"
#include "Components/SphereCollider.h"
#include "Components/MeshRenderer.h"
#include "Components/TerrainRenderer.h"
#include "MaterialLibrary.h"
#include "Interpolation.h"
#include "Components/Death.h"
#include "Components/Force.h"
#include "Components/TextName.h"

void Physics::Update(float deltaTime, World &world) {
    if (!enabled) return;
    
    for (GameObject *gameObject : world.GetGameObjects()) {
        RigidBody *rigidBody = (RigidBody*)gameObject->GetComponent("RigidBody");
        if (rigidBody && rigidBody->useGravity && !rigidBody->isKinematic) {
//            glm::vec3 accel = rigidBody->acceleration * deltaTime;
            glm::vec3 vel = rigidBody->velocity * deltaTime;
//            rigidBody->acceleration += gravity * deltaTime;
//            rigidBody->velocity += accel;
            gameObject->transform->SetPosition(gameObject->transform->GetPosition()+vel);
        }
    }
    UpdateBulletPhysics(deltaTime, world);
    HandleTerrainCollisions(world);
    ComputeCollisions(world);
}

void Physics::UpdateBulletPhysics(float deltaTime, World &world) {
    btScalar fixedTimeStep = 1.f/60.f;
    world.dynamicsWorld->stepSimulation((deltaTime), 0, fixedTimeStep);
    //world.dynamicsWorld->stepSimulation(deltaTime);
    for(GameObject* go : world.GetGameObjects()) {
        RigidBody* rb = (RigidBody*)go->GetComponent("RigidBody");
        if(rb && rb->bulletRigidBody) {
            btTransform *form = new btTransform();
            rb->bulletRigidBody->getMotionState()->getWorldTransform(*form);
            go->transform->SetPosition(glm::vec3(form->getOrigin().x(), form->getOrigin().y(), form->getOrigin().z()));
            
//            btScalar yaw, pitch, roll;
//            btMatrix3x3 mat = form->getBasis();
//            mat.getEulerYPR(yaw, pitch, roll);

            if(go->name != "Camera") {
                btVector3 rot = form->getRotation().getAngle() * (form->getRotation().getAxis());
                go->transform->SetRotation(glm::vec3(glm::degrees(rot.x()),glm::degrees(rot.y()),glm::degrees(rot.z())));
            }
        }
        
    }
}

void Physics::ComputeCollisions(World &world) {
    std::vector<Collision> collisions;
    std::vector<GameObject*> gameObjects = world.GetGameObjects();
    
    // Only test for collision between 2 objects once
    for (int i = 0; i < gameObjects.size(); i++) {
        GameObject *gameObject1 = gameObjects[i];
        
        // Get bounds for first object
        Bounds bounds1 = gameObject1->getBounds();
        
        for (int j = i+1; j < gameObjects.size(); j++) {
            GameObject *gameObject2 = gameObjects[j];
            
            // Don't compute collision with self
            if (gameObject2 == gameObject1) continue;
            
            // Get bounds for second object
            Bounds bounds2 = gameObject2->getBounds();
            
            // Test for collision
            if (Bounds::Intersects(bounds1, bounds2)) {
//                std::cout << gameObject1->name << " hit " << gameObject2->name << std::endl;
                Collision collision = Collision(gameObject1, gameObject2, nullptr);
                collisions.push_back(collision);
            }
        }
    }
    ResolveCollisions(world, collisions);
}

void Physics::ResolveCollisions(World &world, std::vector<Collision> collisions) {
    for (Collision collision : collisions) {
        RigidBody *rigidBody1 = (RigidBody*)collision.gameObject1->GetComponent("RigidBody");
        RigidBody *rigidBody2 = (RigidBody*)collision.gameObject2->GetComponent("RigidBody");
        if(!rigidBody1 || !rigidBody2) continue;
        // Recompute bounds - they could have changed since last resolved collision
        Bounds bounds1 = collision.gameObject1->getBounds();
        Bounds bounds2 = collision.gameObject2->getBounds();
        // If there is no longer an intersection, continue
        if (!Bounds::Intersects(bounds1, bounds2)) continue;
        
        if (collision.gameObject1->name.compare("Barrier") == 0 && collision.gameObject2->name.compare("Bunny") == 0) {
            rigidBody2->velocity.x *= -1;
            rigidBody2->velocity.z *= -1;
            collision.gameObject2->transform->SetPosition(collision.gameObject2->transform->GetPosition() + rigidBody2->velocity / 5.0f);
            collision.gameObject2->transform->SetRotation(collision.gameObject2->transform->GetRotation() - glm::vec3(0, 180, 0));
        } else if (collision.gameObject2->name.compare("Barrier") == 0 && collision.gameObject1->name.compare("Bunny") == 0) {
            rigidBody1->velocity.x *= -1;
            rigidBody1->velocity.z *= -1;
            collision.gameObject1->transform->SetPosition(collision.gameObject1->transform->GetPosition() + rigidBody1->velocity / 5.0f);
            collision.gameObject1->transform->SetRotation(collision.gameObject1->transform->GetRotation() - glm::vec3(0, 180, 0));
        } else if (collision.gameObject1->name.compare("Wolf") == 0 && collision.gameObject2->name.compare("Bunny") == 0) {
//            collision.gameObject2->Destroy();
            collision.gameObject2->RemoveComponent("MeshRenderer");
            collision.gameObject2->RemoveComponent("BoxCollider");
            RigidBody *rb = (RigidBody*) collision.gameObject2->GetComponent("RigidBody");
            if (rb) {
                collision.gameObject2->RemoveComponent("RigidBody");
                if (rb->bulletRigidBody) {
                    world.dynamicsWorld->removeRigidBody(rb->bulletRigidBody);
                }
            }
        } else if (collision.gameObject2->name.compare("Wolf") == 0 && collision.gameObject1->name.compare("Bunny") == 0) {
//            collision.gameObject1->Destroy();
            collision.gameObject1->RemoveComponent("MeshRenderer");
            collision.gameObject1->RemoveComponent("BoxCollider");
            RigidBody *rb = (RigidBody*) collision.gameObject1->GetComponent("RigidBody");
            if (rb) {
                collision.gameObject1->RemoveComponent("RigidBody");
                if (rb->bulletRigidBody) {
                    world.dynamicsWorld->removeRigidBody(rb->bulletRigidBody);
                }
            }
            
            TextName* tn = (TextName*)collision.gameObject1->GetComponent("TextName");
            if(tn) {
                tn->color = vec3(1,0,0);
            }
        } else if (collision.gameObject2->name.compare("Wolf") == 0 && collision.gameObject1->name.compare("Boulder") == 0) {
            RigidBody *body = (RigidBody*)collision.gameObject1->GetComponent("RigidBody");
            std::cout << "Hit Wolf with Boulder" << std::endl;
            std::cout << "Boulder velocity: " << body->bulletRigidBody->getLinearVelocity().length() << std::endl;
            Force *force = (Force*)collision.gameObject1->GetComponent("Force");
            if (force) {
                std::cout << "Boulder force: " << glm::length(force->dir) << std::endl;
            }
            if(body && force && glm::length(force->dir) > 0) {
                Death* gD = (Death*) collision.gameObject2->GetComponent("Death");
                if(gD){
                    CAudioEngine::instance()->PlaySound("wolfHurt.wav");
                    collision.gameObject2->RemoveComponent("MeshRenderer");
                    collision.gameObject2->RemoveComponent("BoxCollider");
                    RigidBody *rb = (RigidBody*) collision.gameObject2->GetComponent("RigidBody");
                    if (rb) {
                        collision.gameObject2->RemoveComponent("RigidBody");
                        if (rb->bulletRigidBody) {
                            world.dynamicsWorld->removeRigidBody(rb->bulletRigidBody);
                        }
                    }
                }
            }
        } else if (collision.gameObject1->name.compare("Wolf") == 0 && collision.gameObject2->name.compare("Boulder") == 0) {
            RigidBody *body = (RigidBody*)collision.gameObject2->GetComponent("RigidBody");
            std::cout << "Hit Wolf with Boulder" << std::endl;
            std::cout << "Boulder velocity: " << body->bulletRigidBody->getLinearVelocity().length() << std::endl;
            Force *force = (Force*)collision.gameObject2->GetComponent("Force");
            if (force) {
                std::cout << "Boulder force: " << glm::length(force->dir) << std::endl;
            }
            if(body && force && glm::length(force->dir) > 0) {
                Death* gD = (Death*) collision.gameObject1->GetComponent("Death");
                if(gD){
                    CAudioEngine::instance()->PlaySound("wolfHurt.wav");
                    collision.gameObject1->RemoveComponent("MeshRenderer");
                    collision.gameObject1->RemoveComponent("BoxCollider");
                    RigidBody *rb = (RigidBody*) collision.gameObject1->GetComponent("RigidBody");
                    if (rb) {
                        collision.gameObject1->RemoveComponent("RigidBody");
                        if (rb->bulletRigidBody) {
                            world.dynamicsWorld->removeRigidBody(rb->bulletRigidBody);
                        }
                    }
                }
            }
        }
    }
}

void Physics::HandleTerrainCollisions(World &world) {
    std::vector<Collision> collisions;
    std::vector<GameObject*> gameObjects = world.GetGameObjects();
    GameObject *terrainObject = nullptr;
    
    // Find the terrain
    for (GameObject *obj : world.GetGameObjects()) {
        if (obj->name.compare("Terrain") == 0) {
            terrainObject = obj;
            break;
        }
    }
    if (!terrainObject) return;
    
    // Get terrain component
    TerrainRenderer *terrainRenderer = (TerrainRenderer*) terrainObject->GetComponent("TerrainRenderer");
    Terrain *terrain = terrainRenderer->terrain;
    glm::vec3 terrainSize = terrainObject->transform->GetScale() * (float)terrain->size;//glm::vec3(terrain->size, 1, terrain->size);
    glm::vec3 terrainPos = terrainObject->transform->GetPosition();
    glm::vec3 terrainMin = terrainObject->transform->GetPosition() - terrainSize/2.0f;
    terrainMin.y = terrainRenderer->terrain->min;
    glm::vec3 terrainMax = terrainObject->transform->GetPosition() + terrainSize/2.0f;
    terrainMax.y = terrainRenderer->terrain->max;
    
//    std::cout << "\nTerrain Size: " << terrainRenderer->terrain->size << std::endl;
//    std::cout << "Terrain Pos: (" << terrainPos.x << "," << terrainPos.y << "," << terrainPos.z << ")" << std::endl;
//    std::cout << "Terrain Scale: (" << terrainObject->transform->GetScale().x << "," << terrainObject->transform->GetScale().y << "," << terrainObject->transform->GetScale().z << ")" << std::endl;
//    std::cout << "Terrain min/max: (" << terrainMin.x << "," << terrainMin.y << "," << terrainMin.z << "), (" << terrainMax.x << "," << terrainMax.y << "," << terrainMax.z << ")" << std::endl;
    
    // Compare objects for collisions
    for (GameObject *obj : world.GetGameObjects()) {
        // If it's not terrain
        if (obj->name.compare("Terrain") != 0) {
            Bounds bounds = obj->getBounds();
            
            // If the object is within XZ bounds of terrain
            glm::vec3 pos = obj->transform->GetPosition();
            if (pos.x >= terrainMin.x && pos.x < terrainMax.x - terrainObject->transform->GetScale().x &&
                pos.z >= terrainMin.z && pos.z < terrainMax.z - terrainObject->transform->GetScale().z) {
                float fColIndex = (pos.z - terrainMin.z) / terrainObject->transform->GetScale().z;
                float fRowIndex = (pos.x - terrainMin.x) / terrainObject->transform->GetScale().x;
                int rowIndex = (int)fRowIndex;// / (terrainSize.z/2.0f);
                int colIndex = (int)fColIndex;// / (terrainSize.x/2.0f);
                float neighbors[2][2] = {
                    {terrain->getHeight(rowIndex, colIndex), terrain->getHeight(rowIndex+1, colIndex)},
                    {terrain->getHeight(rowIndex, colIndex+1), terrain->getHeight(rowIndex+1, colIndex+1)}
                };
                
//                std::cout << "Height[" << rowIndex << "][" << colIndex << "] = " << terrain->getHeight(rowIndex, colIndex) << std::endl;
                float interpolatedHeight = BilinearInterpolate(neighbors, fColIndex-colIndex, fRowIndex-rowIndex);
//                std::cout << "Interpolated Height: " << interpolatedHeight << std::endl;
                
//                if(obj->name.compare("Wolf") != 0)
//                obj->transform->SetPosition(glm::vec3(pos.x, terrainPos.y + interpolatedHeight * terrainObject->transform->GetScale().y + obj->transform->GetScale().y / 2.0f, pos.z));
                
                if(obj->name.compare("Wolf") == 0 && obj->transform->GetPosition().y <= terrainPos.y + interpolatedHeight * terrainObject->transform->GetScale().y + obj->transform->GetScale().y / 2.0f + 1){
                    RigidBody *body = (RigidBody*)obj->GetComponent("RigidBody");
                    if(body && body->bulletRigidBody->getLinearVelocity().y() < 0)
                    {
                    Death* gD = (Death*) obj->GetComponent("Death");
//                    std::cout<<"got in d loop";
                    if(gD){
//                        std::cout<<"found collision";
                        if(gD->shouldDie){
//                            std::cout<<"should die true";
                            CAudioEngine::instance()->PlaySound("wolfHurt.wav");
                            obj->RemoveComponent("MeshRenderer");
                            obj->RemoveComponent("BoxCollider");
                            RigidBody *rb = (RigidBody*) obj->GetComponent("RigidBody");
                            if (rb) {
                                obj->RemoveComponent("RigidBody");
                                if (rb->bulletRigidBody) {
                                    world.dynamicsWorld->removeRigidBody(rb->bulletRigidBody);
                                }
                            }
                        }
                    }
                    }
                }
                
                // Move any objects that are under the terrain to above it
                MeshRenderer *meshRenderer = (MeshRenderer*)obj->GetComponent("MeshRenderer");
                if (!meshRenderer) continue;
                float terrainHeight = interpolatedHeight * terrainObject->transform->GetScale().y;
                float halfObjectHeight = meshRenderer->model->bounds.halfwidths.y * obj->transform->GetScale().y;
                float newPosY = terrainPos.y + terrainHeight + halfObjectHeight;
                
                if (obj->name.compare("Bunny") == 0 || obj->name.compare("Wolf") == 0 || obj->name.compare("Boulder") == 0 || obj->name.compare("Camera") == 0 || obj->name.compare("FollowSphere") == 0) {
                    if (pos.y < newPosY) {
                        obj->transform->SetPosition(glm::vec3(pos.x, newPosY, pos.z));
                        RigidBody *rigidBody = (RigidBody*)obj->GetComponent("RigidBody");
                        if (rigidBody && rigidBody->bulletRigidBody) {
                            rigidBody->bulletRigidBody->setLinearVelocity(btVector3(0, 0, 0));
                            if (obj->name.compare("Boulder") == 0) {
                                Force *force = (Force*)obj->GetComponent("Force");
                                if (force && Time::Now() - force->time > 1000) {
                                    force->dir = glm::vec3(0);
                                }
                            }
                        }
                    }
                } else if (obj->name.compare("Tree") == 0) {
                    // Always place this object directly on the terrain (cannot fly above terrain)
                    obj->transform->SetPosition(glm::vec3(pos.x, newPosY, pos.z));
                }
            }
        }
    }
}

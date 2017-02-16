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

void Physics::Update(float deltaTime, World &world) {
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
    ComputeCollisions(world);
    HandleTerrainCollisions(world);
    UpdateBulletPhysics(deltaTime, world);
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

            btVector3 rot = form->getRotation().getAngle() * (form->getRotation().getAxis());
            
            go->transform->SetRotation(glm::vec3(glm::degrees(rot.x()),glm::degrees(rot.y()),glm::degrees(rot.z())));
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
        
        if (collision.gameObject1->name.compare("Camera") == 0 && collision.gameObject2->name.compare("Bunny") == 0) {
            /*MeshRenderer *meshRenderer = (MeshRenderer*)collision.gameObject2->GetComponent("MeshRenderer");
            if (meshRenderer->material != MaterialLibrary::polishedGold) {
                meshRenderer->material = MaterialLibrary::polishedGold;
                rigidBody2->isKinematic = true;
                bunniesCollected += 1;
//                std::cout << "Bunnies Collected: " << bunniesCollected << std::endl;
            }*/
        } else if (collision.gameObject2->name.compare("Camera") == 0 && collision.gameObject1->name.compare("Bunny") == 0) {
            /*
            MeshRenderer *meshRenderer = (MeshRenderer*)collision.gameObject1->GetComponent("MeshRenderer");
            if (meshRenderer->material != MaterialLibrary::polishedGold) {
                meshRenderer->material = MaterialLibrary::polishedGold;
                rigidBody2->isKinematic = true;
                bunniesCollected += 1;
//                std::cout << "Bunnies Collected: " << bunniesCollected << std::endl;
            }*/
        } else if (collision.gameObject1->name.compare("Barrier") == 0 && collision.gameObject2->name.compare("Bunny") == 0) {
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
        }
        else if (collision.gameObject2->name.compare("Terrain") == 0 && collision.gameObject1->name.compare("Wolf") == 0) {
            //            collision.gameObject1->Destroy();
            
            Death* gD = (Death*) collision.gameObject1->GetComponent("Death");
//            std::cout<<"got in d loop";
            if(gD){
//                std::cout<<"found collision";
                if(gD->shouldDie){
//                    std::cout<<"should die true";
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
            
            
        }else if (collision.gameObject2->name.compare("Wolf") == 0 && collision.gameObject1->name.compare("Terrain") == 0) {
            //            collision.gameObject1->Destroy();
            
            Death* gD = (Death*) collision.gameObject2->GetComponent("Death");
//            std::cout<<"got in d loop";
            if(gD){
//                std::cout<<"found collision";
                if(gD->shouldDie){
//                    std::cout<<"should die true";
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
            
            
        }else {
        
        if (rigidBody1 && rigidBody2 && !rigidBody1->isKinematic && !rigidBody2->isKinematic) {
            // TODO: don't just move up
            float dy;
            if (bounds2.getMax().y > bounds1.getMax().y) {
                dy = bounds1.getMax().y - bounds2.getMin().y;
                collision.gameObject2->transform->SetRotation(collision.gameObject2->transform->GetRotation() + glm::vec3(0, dy, 0));
                rigidBody1->velocity.y = rigidBody1->velocity.y/2;
                rigidBody1->acceleration = glm::vec3(0, 0, 0);
                rigidBody2->velocity.y = 0;
                rigidBody2->acceleration = glm::vec3(0, 0, 0);
            } else {
                dy = bounds2.getMax().y - bounds1.getMin().y;
                collision.gameObject1->transform->SetRotation(collision.gameObject1->transform->GetRotation() + glm::vec3(0, dy, 0));
                rigidBody2->velocity.y = -rigidBody2->velocity.y/2;
                rigidBody2->acceleration = glm::vec3(0, 0, 0);
                rigidBody1->velocity.y = 0;
                rigidBody1->acceleration = glm::vec3(0, 0, 0);
            }
//            collision.gameObject1->transform->position += glm::vec3(0, dy/2, 0);
//            rigidBody1->velocity.y = 0;
//            rigidBody1->acceleration = glm::vec3(0, 0, 0);
//            rigidBody1->acceleration = rigidBody1->acceleration / glm::vec3(2, 2, 2);
//            rigidBody1->acceleration.y = -rigidBody1->acceleration.y;
//            collision.gameObject2->transform->position += glm::vec3(0, -dy/2, 0);
//            rigidBody2->velocity.y = 0;
//            rigidBody2->acceleration = glm::vec3(0, 0, 0);
//            rigidBody2->acceleration = rigidBody2->acceleration / glm::vec3(2, 2, 2);
//            rigidBody2->acceleration.y = -rigidBody2->acceleration.y;
        } else if (rigidBody1 && !rigidBody1->isKinematic) {
            // TODO: don't just move up
            float dy = bounds2.getMax().y - bounds1.getMin().y;
            collision.gameObject1->transform->SetRotation(collision.gameObject1->transform->GetRotation() + glm::vec3(0, dy, 0));
            rigidBody1->velocity.y = 0;
            rigidBody1->acceleration = glm::vec3(0, 0, 0);
//            rigidBody1->acceleration = rigidBody1->acceleration / glm::vec3(2, 2, 2);
//            rigidBody1->acceleration.y = -rigidBody1->acceleration.y;
        } else if (rigidBody2 && !rigidBody2->isKinematic) {
            // TODO: don't just move up
            float dy = bounds1.getMax().y - bounds2.getMin().y;
            collision.gameObject2->transform->SetRotation(collision.gameObject2->transform->GetRotation() + glm::vec3(0, dy, 0));
            rigidBody2->velocity.y = 0;
            rigidBody2->acceleration = glm::vec3(0, 0, 0);
//            rigidBody2->acceleration = rigidBody2->acceleration / glm::vec3(2, 2, 2);
//            rigidBody2->acceleration.y = -rigidBody2->acceleration.y;
        }
        }
    }
}

// Get the bounds for a game object if it is collidable, otherwise return null
//Bounds *Physics::BoundsForGameObject(GameObject *gameObject) {
//    MeshRenderer *meshRenderer = (MeshRenderer*)gameObject->GetComponent("MeshRenderer");
//    BoxCollider *boxCollider = (BoxCollider*)gameObject->GetComponent("BoxCollider");
//    SphereCollider *sphereCollider = (SphereCollider*)gameObject->GetComponent("SphereCollider");
//    if (boxCollider || sphereCollider) {
//        if (meshRenderer) {
//            return meshRenderer->model->bounds.TransformedBounds(gameObject->transform);
//        }
//        Bounds *bounds = new Bounds();
//        return bounds->TransformedBounds(gameObject->transform);
//    }
//    return new Bounds();
//}

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
    glm::vec3 terrainSize = terrainObject->transform->GetScale() * glm::vec3(terrain->size, 1, terrain->size);
    glm::vec3 terrainPos = terrainObject->transform->GetPosition();
    glm::vec3 terrainMin = terrainObject->transform->GetPosition() - terrainSize/2.0f;
    glm::vec3 terrainMax = terrainObject->transform->GetPosition() + terrainSize/2.0f;
    
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
                    {terrain->getHeight(rowIndex+1, colIndex+1), terrain->getHeight(rowIndex+1, colIndex)},
                    {terrain->getHeight(rowIndex, colIndex+1), terrain->getHeight(rowIndex, colIndex)}
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
                
                if (obj->name.compare("Camera") == 0 || obj->name.compare("Bunny") == 0 || obj->name.compare("Boulder") == 0) {
                    if (pos.y < terrainPos.y + interpolatedHeight * terrainObject->transform->GetScale().y + obj->transform->GetScale().y / 2.0f)
                        obj->transform->SetPosition(glm::vec3(pos.x, terrainPos.y + interpolatedHeight * terrainObject->transform->GetScale().y + obj->transform->GetScale().y / 2.0f + 1, pos.z));
                } else {
                    if (pos.y < terrainPos.y + interpolatedHeight * terrainObject->transform->GetScale().y + obj->transform->GetScale().y / 2.0f)
                    obj->transform->SetPosition(glm::vec3(pos.x, terrainPos.y + interpolatedHeight * terrainObject->transform->GetScale().y + obj->transform->GetScale().y / 2.0f + 1, pos.z));
                }
            }
        }
    }
}

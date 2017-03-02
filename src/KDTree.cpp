//
//  KDTree.cpp
//  Shepherd
//
//  Created by Michael Cantrell on 2/5/17.
//
//

#include <stdio.h>
#include <algorithm>    // std::sort
#include "KDTree.h"
#include "Components/Camera.h"

bool intersectFrustumAABB2(Camera *cam, vec3 min, vec3 max) {
    // Indexed for the 'index trick' later
    vec3 box[] = {min, max};
    
    // We only need to do 6 point-plane tests
    for (int i = 0; i < 6; ++i)
    {
        // This is the current plane
        vec4 plane = cam->planes[i];
        
        // p-vertex selection (with the index trick)
        // According to the plane normal we can know the
        // indices of the positive vertex
        const int px = static_cast<int>(plane.x > 0.0f);
        const int py = static_cast<int>(plane.y > 0.0f);
        const int pz = static_cast<int>(plane.z > 0.0f);
        
        // Dot product
        // project p-vertex on plane normal
        // (How far is p-vertex from the origin)
        const float dp =
        (plane.x*box[px].x) +
        (plane.y*box[py].y) +
        (plane.z*box[pz].z);
        
        // Doesn't intersect if it is behind the plane
        if (dp < -plane.w) { return false; }
    }
    return true;
}

std::vector<GameObject*> KDTree::getStaticObjectsInViewFrustrumRec(Camera *camera, Node *root) {
    std::vector<GameObject*> objects;
    if(intersectFrustumAABB2(camera, root->minVals, root->maxVals)) {
        std::vector<GameObject*> leftObjects = getStaticObjectsInViewFrustrumRec(camera, root->left);
        std::vector<GameObject*> rightObjects = getStaticObjectsInViewFrustrumRec(camera, root->right);
        objects.insert(objects.end(), leftObjects.begin(), leftObjects.end());
        objects.insert(objects.end(), rightObjects.begin(), rightObjects.end());
    }
    return objects;
}

std::vector<GameObject*> KDTree::getStaticObjectsInViewFrustrum(Camera *camera) {
    getStaticObjectsInViewFrustrumRec(camera, root);
}

bool KDTree::gameObjectDimensionCmp(GameObject *obj1, GameObject *obj2, int dimension) {
    return obj1->transform->GetPosition()[dimension] < obj2->transform->GetPosition()[dimension];
}

bool KDTree::gameObjectXCmp (GameObject *obj1, GameObject *obj2) {
    return gameObjectDimensionCmp(obj1, obj2, 0);
}

bool KDTree::gameObjectYCmp (GameObject *obj1, GameObject *obj2) {
    return gameObjectDimensionCmp(obj1, obj2, 1);
}

bool KDTree::gameObjectZCmp (GameObject *obj1, GameObject *obj2) {
    return gameObjectDimensionCmp(obj1, obj2, 2);
}

float KDTree::putIntoRightAndLeft(std::vector<GameObject*> gameObjects, std::vector<GameObject*> &left, std::vector<GameObject*> &right, int dimension) {
    if(dimension == 0)
        std::sort (gameObjects.begin(), gameObjects.end(), gameObjectXCmp);
    else if(dimension == 1)
        std::sort (gameObjects.begin(), gameObjects.end(), gameObjectYCmp);
    else
        std::sort (gameObjects.begin(), gameObjects.end(), gameObjectZCmp);
    
    float halfWayPoint;
    if(gameObjects.size() % 2 == 0) {
        float pos1 = gameObjects[gameObjects.size()/2 - 1]->transform->GetPosition()[dimension];
        float pos2 = gameObjects[gameObjects.size()/2]->transform->GetPosition()[dimension];
        halfWayPoint = (pos1 + pos2)/2.0f;
    }
    else {
        halfWayPoint = gameObjects[gameObjects.size()/2 - 1]->transform->GetPosition()[dimension];
    }
    for(int i = 0; i < gameObjects.size(); i++) {
        Bounds b = gameObjects[i]->getBounds();
        if(b.getMin()[dimension] < halfWayPoint) {
            left.push_back(gameObjects[i]);
        }
        if(b.getMax()[dimension] > halfWayPoint) {
            right.push_back(gameObjects[i]);
        }
    }
    return halfWayPoint;
}

Node *KDTree::createTree(std::vector<GameObject*> gameObjects, int dimension, glm::vec3 parentMinVals, glm::vec3 parentMaxVals) {
    struct Node* n = new Node;
    n->gameObjects = gameObjects;
    n->dimension = dimension;
    if(gameObjects.size() == 1) {
        n->left = n->right = nullptr;
        n->halfWayPoint = gameObjects[0]->transform->GetPosition()[dimension];
    }
    else {
        std::vector<GameObject*> leftObjects;
        std::vector<GameObject*> rightObjects;
        n->halfWayPoint = putIntoRightAndLeft(gameObjects, leftObjects, rightObjects, dimension);
        n->left->minVals = parentMinVals;
        n->left->maxVals = parentMaxVals;
        n->left->maxVals[dimension] = n->halfWayPoint;
        n->left = createTree(leftObjects, (dimension + 1) % 3, n->left->minVals, n->left->maxVals);
        n->right->minVals = parentMinVals;
        n->right->minVals[dimension] = n->halfWayPoint;
        n->right->maxVals = parentMaxVals;
        n->right = createTree(rightObjects, (dimension + 1) % 3, n->right->minVals, n->right->maxVals);
    }
    return n;
}






//
//  KDTree.h (thanks to http://www.geeksforgeeks.org/k-dimensional-tree/ )
//  Shepherd
//
//  Created by Michael Cantrell on 2/4/17.
//
//

#ifndef KDTree_h
#define KDTree_h

#include <vector>
#include "GameObject.h"

typedef struct Node {
    float halfWayPoint;
    int dimension; // 0 - x, 1 - y, 2 - z
    Node *left, *right;
    std::vector<GameObject*> gameObjects;
    glm::vec3 maxVals; // contains mixX maxY and maxZ
    glm::vec3 minVals; // contains minX minY and minZ
} Node;

class KDTree {
public:
    KDTree(std::vector<GameObject*> gameObjects) { root = createTree(gameObjects, 0, glm::vec3(-INFINITY), glm::vec3(INFINITY));}
    virtual ~KDTree() {};
    void insert(glm::vec3 point);
    bool search(glm::vec3 point);

private:
    Node *root = nullptr;
    Node *createTree(std::vector<GameObject*> gameObjects, int dimension, glm::vec3 parentMinVals, glm::vec3 parentMaxVals);
    float putIntoRightAndLeft(std::vector<GameObject*> gameObjects, std::vector<GameObject*> &left, std::vector<GameObject*> &right, int dimension);
    static bool gameObjectDimensionCmp(GameObject *obj1, GameObject *obj2, int dimension);
    static bool gameObjectXCmp (GameObject *obj1, GameObject *obj2);
    static bool gameObjectYCmp (GameObject *obj1, GameObject *obj2);
    static bool gameObjectZCmp (GameObject *obj1, GameObject *obj2);
};

#endif /* KDTree_h */

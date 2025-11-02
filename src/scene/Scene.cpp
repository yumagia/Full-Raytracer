#include "Scene.h"

Scene::~Scene() {
    delete bvh;
    delete[] vertexPool;
    delete[] normalPool; 
}
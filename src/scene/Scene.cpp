#include "Scene.h"

Scene::~Scene() {
    delete[] vertexPool;
    delete[] normalPool; 
}
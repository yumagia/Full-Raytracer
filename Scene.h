#ifndef SCENE_INCLUDED
#define SCENE_INCLUDED

#include "ScenePrimitives.h"

#include <vector>


// Stores the entire scene
class Scene {
public:
	Scene() {}
	~Scene();

	int imageWidth = 640;
	int imageHeight = 480;

	const char *fileName = "raytraced.bmp";

	int maxDepth; // Maximum recursion depth for reflected and refracted rays

	Camera camera;    
	Background background;
	AmbientLight ambient;

	std::vector<Sphere> spheres;

	std::vector<DirectionalLight>	directionalLights;
	std::vector<PointLight>			pointLights;
	std::vector<SpotLight>			spotLights;
};

#endif
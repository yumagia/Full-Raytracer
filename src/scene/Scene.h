#ifndef SCENE_INCLUDED
#define SCENE_INCLUDED

#include "ScenePrimitives.h"

#include <vector>
#include <string>

// Stores the entire scene
class Scene {
public:
	Scene() {}

	int imageWidth;
	int imageHeight;

	std::string outputImage;

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
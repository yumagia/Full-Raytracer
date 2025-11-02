#ifndef SCENE_INCLUDED
#define SCENE_INCLUDED

#include "ScenePrimitives.h"
#include "Bvh.h"

#include <vector>
#include <string>

// Stores the entire scene
class Scene {
public:
	Scene() {}
	~Scene();



	int imageWidth;
	int imageHeight;

	std::string outputImage;

	Camera camera;    
	Background background;
	AmbientLight ambient;

	std::vector<Triangle> triangles;
	std::vector<NormalTriangle> normalTriangles;
	std::vector<Sphere> spheres;

	std::vector<DirectionalLight>	directionalLights;
	std::vector<PointLight>			pointLights;
	std::vector<SpotLight>			spotLights;

	int maxDepth; // Maximum recursion depth for reflected and refracted rays

	// These set the size of the vertex and normal pools
	int maxVertices;
	int maxNormals;

	// These will be used to access vertices and normals via index
	Vertex* vertexPool = NULL;
	Normal* normalPool = NULL;

	int vertexCount = 0;
	int normalCount = 0;

	// Lets go
	SceneBvh *bvh;
	bool accelerate;
};

#endif
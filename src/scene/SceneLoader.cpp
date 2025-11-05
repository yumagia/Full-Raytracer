#include "SceneLoader.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

std::vector<std::string> SceneLoader::ParseArgsFromLine(std::string line) {
	std::vector<std::string> args;
	args.reserve(MAX_ARGS);

	std::istringstream iss(line);
	std::string arg;
	while(iss >> arg) {
		args.push_back(arg);

		if(args.size() > MAX_ARGS) {		// Not supposed to happen
			std::cerr << "Exceeded MAX_ARGS!" << std::endl;
			abort();
		}
	}

	return args;
}


Scene *SceneLoader::ParseSceneFile(const char *fileName) {
	std::ifstream file(fileName);

	if(!file.is_open()) {
		std::cerr << "Error opening file!" << std::endl;
		abort();
	}

	// Create the new scene
	Scene *raytracerScene = new Scene();

	// Fill the default fields
	raytracerScene->outputImage = "raytraced.bmp"; // memory leak
	raytracerScene->imageWidth = 640;
	raytracerScene->imageHeight = 480;
	raytracerScene->maxDepth = 5;

	// Default is no ambient
	AmbientLight sceneAmbient;
	sceneAmbient = Color(0, 0, 0);

	// Default is black
	Background sceneBackground;
	sceneBackground = Color(0, 0, 0);

	Camera sceneCamera;
	sceneCamera.eye = Vec3f(0, 0, 0);
	sceneCamera.fwd = Vec3f(0, 0, -1);
	sceneCamera.up = Vec3f(0, 1, 0);
	sceneCamera.right = Vec3f(1, 0, 0);
	sceneCamera.halfAngleFov = 45;

	// Default is a matte white surface
	Material currentMaterial;
	currentMaterial.ambient = Color(0, 0, 0);
	currentMaterial.diffuse = Color(1, 1, 1);
	currentMaterial.specular = Color(0, 0, 0);
	currentMaterial.specularCoeff = 5;
	currentMaterial.transmissive = Color(0, 0, 0);
	currentMaterial.refractionCoeff = 1;


	std::string line;
	while(std::getline(file, line)) {		// Process all of the arguments
		std::vector<std::string> args;

		args = ParseArgsFromLine(line);

		if(!(args.empty() || args[0][0] == '#')) {    // Otherwise, skip it. It's a comment
			if(args[0] == "camera_pos:") {
				sceneCamera.eye = Vec3f(stof(args[1]), stof(args[2]), stof(args[3]));
			}
			else if(args[0] == "camera_fwd:") {
				sceneCamera.fwd = Vec3f(stof(args[1]), stof(args[2]), stof(args[3]));
			}
			else if(args[0] == "camera_up:") {
				sceneCamera.up = Vec3f(stof(args[1]), stof(args[2]), stof(args[3]));
			}
			else if(args[0] == "camera_fov_ha:") {
				sceneCamera.halfAngleFov = stof(args[1]);
			}
			else if(args[0] == "film_resolution:") {
				raytracerScene->imageWidth = stoi(args[1]);
				raytracerScene->imageHeight = stoi(args[2]);				
			}
			else if(args[0] == "output_image:") {
				raytracerScene->outputImage = args[1].c_str();
			}
			else if(args[0] == "max_vertices:") {
				raytracerScene->maxVertices = stoi(args[1]);
				raytracerScene->vertexPool = new Vertex[raytracerScene->maxVertices];
				std::cout << "Max vertices: " << raytracerScene->maxVertices << std::endl;
				raytracerScene->vertexCount = 0;
			}
			else if(args[0] == "max_normals:") {
				raytracerScene->maxNormals = stoi(args[1]);
				raytracerScene->normalPool = new Normal[raytracerScene->maxNormals];
				raytracerScene->normalCount = 0;
				std::cout << "Max normals: " << raytracerScene->maxNormals << std::endl;
			}
			else if(args[0] == "vertex:") {
				Vertex vertex = Vertex(stof(args[1]), stof(args[2]), stof(args[3]));
				raytracerScene->vertexPool[raytracerScene->vertexCount] = vertex;
				raytracerScene->vertexCount++;
			}
			else if(args[0] == "normal:") {
				Normal normal = Normal(stof(args[1]), stof(args[2]), stof(args[3]));
				raytracerScene->normalPool[raytracerScene->normalCount] = normal;
				raytracerScene->normalCount++;
			}
			else if(args[0] == "triangle:") {
				Triangle triangle;
				triangle.v1 = raytracerScene->vertexPool[stoi(args[1])];
				triangle.v2 = raytracerScene->vertexPool[stoi(args[2])];
				triangle.v3 = raytracerScene->vertexPool[stoi(args[3])];
				triangle.material = currentMaterial;

				// Pre-process the plane
				triangle.CreatePlane();

				raytracerScene->triangles.push_back(triangle);
			}
			else if(args[0] == "normal_triangle:") {
				Triangle normalTriangle;

				normalTriangle.v1 = raytracerScene->vertexPool[stoi(args[1])];
				normalTriangle.v2 = raytracerScene->vertexPool[stoi(args[2])];
				normalTriangle.v3 = raytracerScene->vertexPool[stoi(args[3])];

				normalTriangle.n1 = raytracerScene->normalPool[stoi(args[4])];
				normalTriangle.n2 = raytracerScene->normalPool[stoi(args[5])];
				normalTriangle.n3 = raytracerScene->normalPool[stoi(args[6])];
				normalTriangle.material = currentMaterial;

				normalTriangle.useNormals = true;

				// Pre-process the plane
				normalTriangle.CreatePlane();

				raytracerScene->triangles.push_back(normalTriangle);

			}
			else if(args[0] == "sphere:") {
				Sphere sphere;
				sphere.origin = Vec3f(stof(args[1]), stof(args[2]), stof(args[3]));
				sphere.r = stof(args[4]);
				sphere.material = currentMaterial;

				raytracerScene->spheres.push_back(sphere);
			}
			else if(args[0] == "background:") {
				sceneBackground = Color(stof(args[1]), stof(args[2]), stof(args[3]));
			}
			else if(args[0] == "material:") {
				currentMaterial.ambient = Color(stof(args[1]), stof(args[2]), stof(args[3]));
				currentMaterial.diffuse = Color(stof(args[4]), stof(args[5]), stof(args[6]));
				currentMaterial.specular = Color(stof(args[7]), stof(args[8]), stof(args[9]));
				currentMaterial.specularCoeff = stof(args[10]);
				currentMaterial.transmissive = Color(stof(args[11]), stof(args[12]), stof(args[13]));
				currentMaterial.refractionCoeff = stof(args[14]);
			}
			else if(args[0] == "directional_light:") {
				DirectionalLight directionalLight;
				directionalLight.intensity = Color(stof(args[1]), stof(args[2]), stof(args[3]));
				directionalLight.direction = Vec3f(stof(args[4]), stof(args[5]), stof(args[6]));

				raytracerScene->directionalLights.push_back(directionalLight);
			}
			else if(args[0] == "point_light:") {
				PointLight pointLight;
				pointLight.intensity = Color(stof(args[1]), stof(args[2]), stof(args[3]));
				pointLight.origin = Vec3f(stof(args[4]), stof(args[5]), stof(args[6]));

				raytracerScene->pointLights.push_back(pointLight);
			}
			else if(args[0] == "spot_light:") {
				SpotLight spotLight;
				spotLight.intensity = Color(stof(args[1]), stof(args[2]), stof(args[3]));
				spotLight.origin = Vec3f(stof(args[4]), stof(args[5]), stof(args[6]));
				spotLight.direction = Vec3f(stof(args[7]), stof(args[8]), stof(args[9]));
				spotLight.angle1 = stof(args[10]);
				spotLight.angle2 = stof(args[11]);

				raytracerScene->spotLights.push_back(spotLight);
			}
			else if(args[0] == "ambient_light:") {
				sceneAmbient = Color(stof(args[1]), stof(args[2]), stof(args[3]));
			}
			else if(args[0] == "max_depth:") {
				raytracerScene->maxDepth = stoi(args[1]);
			}
		}
	}

	// Apply Ambient and Background to the raytracer scene
	raytracerScene->ambient = sceneAmbient;
	raytracerScene->background = sceneBackground;

	// Create an orthonormal camera basis based on the provided up and forward
	sceneCamera.right = (sceneCamera.up).Cross(sceneCamera.fwd);
	(sceneCamera.right).Normalize();
	sceneCamera.up = (sceneCamera.fwd).Cross(sceneCamera.right);
	(sceneCamera.up).Normalize();
	(sceneCamera.fwd).Normalize();

	raytracerScene->camera = sceneCamera;

	raytracerScene->bvh = new SceneBvh(raytracerScene->triangles);
	raytracerScene->hasBvh = (*raytracerScene->bvh).BuildBvh();

	file.close();

	std::cout << "Number of triangles: " << raytracerScene->triangles.size() << std::endl;
	std::cout << "File Parsing Success" << std::endl;

	return raytracerScene;
}
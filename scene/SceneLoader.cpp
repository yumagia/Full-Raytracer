#include "SceneLoader.h"

#include <iostream>
#include <fstream>

Scene *SceneLoader::ParseSceneFile(const char *fileName) {
	std::ifstream file(fileName);

	if(!file.is_open()) {
		std::cerr << "Error opening file!" << std::endl;
		return;
	}

	// Create the new scene
	Scene *raytracerScene = new Scene;

	// Fill the default fields
	raytracerScene->fileName = "raytraced.bmp";
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
	sceneCamera.cameraPos = Vec3f(0, 0, 0);
	sceneCamera.cameraFwd = Vec3f(0, 0, -1);
	sceneCamera.cameraUp = Vec3f(0, 1, 0);
	sceneCamera.cameraRight = Vec3f(1, 0, 0);
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
		std::string arg = line.substr(0, line.find(" "));

		if(!(arg == "#")) {    // Otherwise, skip it. It's a comment
			std::string s = line;
			if(arg == "camera_pos:") {
				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float x = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float y = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				float z = stof(s);
				sceneCamera.cameraPos = Vec3f(x, y, z);
			} else
			if(arg == "camera_fwd:") {
				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float x = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float y = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				float z = stof(s);
				sceneCamera.cameraFwd = Vec3f(x, y, z);
			} else
			if(arg == "camera_up:") {
				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float x = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float y = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				float z = stof(s);
				sceneCamera.cameraUp = Vec3f(x, y, z);
			} else
			if(arg == "camera_fov_ha:") {
			std::string s = line;

			s.erase(0, arg.length());
			sceneCamera.halfAngleFov = stof(s);
			}
			if(arg == "film_resolution:") {
				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				raytracerScene->imageWidth = stoi(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				raytracerScene->imageHeight = stoi(arg);
			} else
			if(arg == "output_image") {
				s.erase(0, arg.length());
      			raytracerScene->fileName = s.c_str();
			} else
			if(arg == "sphere:") {
				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float x = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float y = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float z = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s;

				Sphere newSphere;
				newSphere.origin = Vec3f(x, y, z);
				newSphere.r = stof(arg);
				newSphere.material = currentMaterial;

				raytracerScene->spheres.push_back(newSphere);
			} else
			if(arg == "background") {
				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float r = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float g = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				float b = stof(s);
				sceneBackground = Color(r, g, b);
			} else
			if(arg == "material") {
				// First triple are ambient
				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float r = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float g = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float b = stof(arg);
				currentMaterial.ambient = Color(r, g, b);

				// Next is diffuse
				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				r = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				g = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				b = stof(arg);
				currentMaterial.diffuse = Color(r, g, b);

				// Next is specular
				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				r = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				g = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				b = stof(arg);
				currentMaterial.specular = Color(r, g, b);

				// Specular Coefficient
				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				currentMaterial.specularCoeff = stof(arg);

				// Transmissive value
				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				r = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				g = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				b = stof(arg);
				currentMaterial.transmissive = Color(r, g, b);

				s.erase(0, arg.length());
				removeLeading(s);
				currentMaterial.refractionCoeff = stof(s);
			} else
			if(arg == "directional_light:") {
				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float r = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float g = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float b = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float x = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float y = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				float z = stof(s);

				DirectionalLight newDirectionalLight;
				newDirectionalLight.direction = Vec3f(x, y, z);
				newDirectionalLight.intensity = Color(r, g, b);

				raytracerScene->directionalLights.push_back(newDirectionalLight);
			} else
			if(arg == "point_light:") {
				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float r = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float g = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float b = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float x = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float y = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				float z = stof(s);

				PointLight newPointLight;
				newPointLight.origin = Vec3f(x, y, z);
				newPointLight.intensity = Color(r, g, b);

				raytracerScene->pointLights.push_back(newPointLight);
			} else
			if(arg == "spot_light:") {
				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float r = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float g = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float b = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float px = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float py = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float pz = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float dx = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float dy = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float dz = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float angle1 = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				float angle2 = stof(s);

				SpotLight newSpotLight;
				newSpotLight.origin = Vec3f(px, py, pz);
				newSpotLight.direction = Vec3f(dx, dy, dz);
				newSpotLight.intensity = Color(r, g, b);
				newSpotLight.angle1 = angle1;
				newSpotLight.angle2 = angle2;

				raytracerScene->spotLights.push_back(newSpotLight);
			} else
			if(arg == "ambient_light") {
				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float r = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				arg = s.substr(0, s.find(" "));
				float g = stof(arg);

				s.erase(0, arg.length());
				removeLeading(s);
				float b = stof(s);
				sceneAmbient = Color(r, g, b);
			} else
			if(arg == "max_depth") {
				s.erase(0, arg.length());
      			raytracerScene->maxDepth = stoi(s);
			}
		}
	}

	// Apply Ambient and Background to the raytracer scene
	raytracerScene->ambient = sceneAmbient;
	raytracerScene->background = sceneBackground;

	// Create an orthonormal camera basis based on the provided up and forward
	sceneCamera.cameraRight = (sceneCamera.cameraUp).Cross(sceneCamera.cameraFwd);
	(sceneCamera.cameraRight).Normalize();
	sceneCamera.cameraUp = (sceneCamera.cameraFwd).Cross(sceneCamera.cameraRight);
	(sceneCamera.cameraUp).Normalize();
	(sceneCamera.cameraFwd).Normalize();

	raytracerScene->camera = sceneCamera;

	file.close();

	return raytracerScene;
}
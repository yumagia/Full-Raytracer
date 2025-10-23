#include "Image.h"
#include "Math.h"
#include "scene/SceneLoader.h"

#include <iostream>

#define MAX_T 1000
#define RAY_EPS 0.0001


// Returns a boolean hit check,
// Passes the t value where a strike occurs
bool HitCheckSphere(Vec3f start, Vec3f dir, float tMax, Vec3f spherePos, float r, float &tHit) {
	float a = dir.Dot(dir);
	Vec3f toStart = start - spherePos;
	float b = 2 * dir.Dot(toStart);
	float c = toStart.Dot(toStart) - r * r;

	float discr = b * b - 4 * a * c;
	if(discr < 0) {
		return false;
	}

	float t0 = (-b + sqrtf(discr)) / (2 * a);
	float t1 = (-b - sqrtf(discr)) / (2 * a);
	if (t0 > 0 || t1 > 0)  {
		tHit = (t0 < t1) ? t0 : t1;

		if(tHit > tMax) {		// Segment terminates before it strikes
			return false;
		}

		return true;
	}

	return false;
}

Color Shade(Vec3f l, Vec3f n, Material material, Scene scene) {
	std::vector<DirectionalLight> directionalLights = scene.directionalLights;
	std::vector<PointLight> pointLights = scene.pointLights;
	std::vector<SpotLight> spotLights = scene.spotLights;
	Color shade = scene.ambient;

	for(DirectionalLight directionalLight : directionalLights) {
		Vec3f dir = directionalLight.direction;
		dir.Normalize();
		float lambert = n.Dot(dir);
		Color diffuse = material.ambient * lambert;
		shade = shade + diffuse;
	}

	return shade;
}

Color RayTraceScene(Vec3f start, Vec3f dir, Scene scene) {
	std::vector<Sphere> sphereList = scene.spheres;

	float tMax = MAX_T;
	bool hit = false;
	Vec3f l, n;			// For shading
	Material material;	// Material, also for shading

	for(Sphere sphere : sphereList) {
		Color hitColor;
		float tHit;
		if(HitCheckSphere(start, dir, tMax, sphere.origin, sphere.r, tHit)) {			
			l = tHit * dir;						// Vector from eye to hit point
			n = sphere.origin - (start + l);	// Surface normal for point on sphere
			material = sphere.material;

			tMax = tHit;	// Truncate the ray. This helps with performance
			hit = true;
		}
	}

	if(!hit) {
		return scene.background;
	}
	
	// Since we got the closest hit data, we can now shade
	
	n.Normalize();
	return Shade(l, n, material, scene);	// Costly, so it's good to do once per ray
}

int main(int argc, char** argv) {
	if(argc != 2) {
		std::cerr << "Usage: ./a.out scenefile" << std::endl;
		return 0;
	}

	const char *fileName = argv[1];
	SceneLoader loader;
	Scene *raytracerScene = loader.ParseSceneFile(fileName);

	Camera camera = raytracerScene->camera;

	float imgW = raytracerScene->imageWidth;
	float imgH = raytracerScene->imageHeight;
	float halfW = imgW/2;
	float halfH = imgH/2;
	float d = halfH / tanf(camera.halfAngleFov * (M_PI / 180.0f));

	Image outputImage = Image(raytracerScene->imageWidth, raytracerScene->imageHeight);
	for(int j = 0; j < raytracerScene->imageHeight; j++) {
		for(int i = 0; i < raytracerScene->imageWidth; i++) {
			float u = (halfW - imgW * ((i + 0.5) / imgW));
			float v = (halfH - imgH * ((j + 0.5) / imgH));
			Vec3f p = camera.eye - d * camera.fwd + u * camera.right + v * camera.up;
			Vec3f rayDir = (p - camera.eye);
			rayDir.Normalize();
			
			Color color = RayTraceScene(camera.eye, rayDir, *raytracerScene);

			outputImage.SetPixel(i, j, color);
		}
	}

	outputImage.Write(raytracerScene->outputImage.c_str());

	delete raytracerScene;

	return 0;
}
#include "Image.h"
#include "Math.h"
#include "scene/SceneLoader.h"

#include <iostream>

bool RaySphereIntersect(Vec3f start, Vec3f dir, Vec3f spherePos, float r) {
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
		return true;
	}

	return false;
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
			float v = (halfH - imgH * ((i + 0.5) / imgH));
			Vec3f p = camera.eye - d * camera.fwd + u * camera.right + v * camera.up;
			Vec3f rayDir = (p - camera.eye);
			rayDir.Normalize();
			
			bool hit = RaySphereIntersect(camera.eye, rayDir, Vec3f(0, 0, 0), 1);
			Color color = Color(0, 0, 0);
			if (hit) {
				color = Color(1, 1, 1);
			}
			outputImage.SetPixel(i, j, color);
		}
	}

	outputImage.Write(raytracerScene->outputImage);
	return 0;
}
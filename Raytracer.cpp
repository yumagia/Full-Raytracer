#include "Raytracer.h"
#include "Image.h"
#include "Math.h"
#include "scene/SceneLoader.h"

#include <iostream>
#include <algorithm>

#define SAMPLE_COUNT 9

#define KC 0.02
#define KL 0.03
#define KQ 0.04

#define MAX_T 1000
#define RAY_EPS 0.0001		// Prevents acne


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

bool HitCheckScene(Vec3f start, Vec3f dir, float tMax, Scene *scene) {
	std::vector<Sphere> sphereList = scene->spheres;
	float tHit;

	for(Sphere sphere : sphereList) {
		if(HitCheckSphere(start, dir, tMax, sphere.origin, sphere.r, tHit)) {
			if(!(tHit < RAY_EPS)) {
				return true;
			}
		}
	}

	return false;
}

float GetFresnelFactor(float refractionCoeff1, float refractionCoeff2, Vec3f v, Vec3f n) {
	float reflectionCoeff = (refractionCoeff1 - refractionCoeff2) / (refractionCoeff1 + refractionCoeff2);
	reflectionCoeff *= reflectionCoeff;
	float cosine = -v.Dot(n);
	if(refractionCoeff1 > refractionCoeff2) {
		float refractiveRatio = refractionCoeff1 / refractionCoeff2;
		float sine = refractiveRatio * refractiveRatio * (1 - cosine * cosine);
		if(sine > 1.f) {	// Total internal reflection
			return 1.f;
		}
		cosine = sqrt(1 - sine);
	}

	float fresnelFactor = reflectionCoeff + (1 - reflectionCoeff) * powf(1 - cosine, 5);

	return fresnelFactor;
}

Color Shade(Vec3f v, Vec3f n, Vec3f p, Material material, Scene *scene, bool hitFlag, int depth) {
	std::vector<DirectionalLight> directionalLights = scene->directionalLights;
	std::vector<PointLight> pointLights = scene->pointLights;
	std::vector<SpotLight> spotLights = scene->spotLights;
	Color shade = scene->ambient * material.ambient;

	for(DirectionalLight directionalLight : directionalLights) {
		Vec3f lightDir = directionalLight.direction;
		lightDir.Normalize();
		Vec3f toLight = lightDir;
		toLight.Negate();
		if(!HitCheckScene(p, toLight, MAX_T, scene)) {		// Cast another ray for shadowing
			float diffuse = std::clamp(n.Dot(toLight), 0.f, 1.f);
		
			Vec3f rayReflected = ((2 * lightDir.Dot(n)) * n) - lightDir;
			float specular = std::clamp(rayReflected.Dot(v), 0.f, 1.f);

			shade = shade + material.diffuse * diffuse + material.specular * specular;
		}
	}

	for(PointLight pointLight : pointLights) {
		Vec3f lightDir = p - pointLight.origin;
		float dist = lightDir.Normalize();
		Vec3f toLight = lightDir;
		toLight.Negate();
		if(!HitCheckScene(p, toLight, dist, scene)) {		// Cast another ray for shadowing
			float diffuse = std::clamp(n.Dot(toLight), 0.f, 1.f);
			
			Vec3f rayReflected = ((2 * lightDir.Dot(n)) * n) - lightDir;
			float specular = powf(std::clamp(rayReflected.Dot(v), 0.f, 1.f), material.specularCoeff);

			float falloff = 1 / (KC + KL * dist + KQ * (dist * dist));
			shade = shade + falloff * (material.diffuse * diffuse + material.specular * specular);
		}
	}


	if(!(depth > scene->maxDepth)) {
		float fresnelFactor;

		if(hitFlag) {	// In a dielectric
			fresnelFactor = GetFresnelFactor(material.refractionCoeff, 1, v, n);
		} 
		else {			// In air
			fresnelFactor = GetFresnelFactor(1, material.refractionCoeff, v, n);
		}

		Vec3f rayReflected = (-2 * v.Dot(n) * n) + v;
		Color reflection = fresnelFactor * RayTraceScene(p, rayReflected, scene, hitFlag, depth + 1);
		shade = shade + reflection;

		if(!(fresnelFactor == 1.f)) {	// Make sure the ray is not being hyper-reflected
			Vec3f refractedPerp = (material.refractionCoeff) * (v + -v.Dot(n) * n);
			Vec3f refractedParallel = sqrtf(fabs(1.0 - refractedPerp.Dot(refractedPerp))) * n;
			refractedParallel.Negate();
			Vec3f rayRefracted = refractedPerp + refractedParallel;
			Color refraction = (1 - fresnelFactor) * RayTraceScene(p, rayRefracted, scene, hitFlag^1, depth + 1);

			shade = shade + (material.transmissive * refraction);
		}

	}

	return shade;
}

Color RayTraceScene(Vec3f start, Vec3f dir, Scene *scene, bool hitFlag, int depth) {
	std::vector<Sphere> sphereList = scene->spheres;

	float tMax = MAX_T;
	bool hit = false;
	Vec3f v, n, p;			// For shading
	Material material;	// Material, also for shading

	for(Sphere sphere : sphereList) {
		float tHit;
		if(HitCheckSphere(start, dir, tMax, sphere.origin, sphere.r, tHit)) {			
			if(!(tHit < RAY_EPS)) {
				v = tHit * dir;				// Vector from eye to hit point
				p = start + v;				// Point on sphere
				n = p - sphere.origin;		// Surface normal
				material = sphere.material;

				tMax = tHit;	// Truncate the ray. This helps with performance
				hit = true;
			}
		}
	}

	if(!hit) {
		return scene->background;
	}
	
	// Since we got the closest hit data, we can now shade
	
	n.Normalize();
	v.Normalize();
	return Shade(v, n, p, material, scene, hitFlag, depth);	// Costly, so it's good to do once per ray
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

			Color color = Color(0, 0, 0);
			for(int samples = 0; samples < SAMPLE_COUNT; samples++) {		// Do a few samples to beat aliasing
				float u = (halfW - imgW * ((i + (rand()/(float) RAND_MAX)) / imgW));
				float v = (halfH - imgH * ((j + (rand()/(float) RAND_MAX)) / imgH));
				Vec3f p = camera.eye - d * camera.fwd + u * camera.right + v * camera.up;
				Vec3f rayDir = (p - camera.eye);
				rayDir.Normalize();

				color = color + RayTraceScene(camera.eye, rayDir, raytracerScene, false, 1);
			}

			color = color / 9;

			outputImage.SetPixel(i, j, color);
		}
	}

	outputImage.Write(raytracerScene->outputImage.c_str());

	delete raytracerScene;

	return 0;
}
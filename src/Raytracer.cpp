#include "Raytracer.h"
#include "Image.h"
#include "Math.h"
#include "scene/SceneLoader.h"

#include <omp.h>		// Parallel processing

#include <iostream>
#include <algorithm>

#define SAMPLE_COUNT 1

// Fall-off constants 
#define KC 2
#define KL 2
#define KQ 0.3

#define MAX_T 5000
#define RAY_EPS 0.003		// Prevents acne
#define PLANE_EQUALS_EPS 0.0000001		// For parallel rays

// Return d for convenient checks
Vec3f RayPlaneIntersection(Vec3f start, Vec3f dir, Plane plane, float &d) {
	d = -(plane.normal.Dot(start) + plane.dist) / plane.normal.Dot(dir);

	return start + d * dir;
}

// Boolean hit check against triangle,
// Passes a Vec3f intersection point
bool HitCheckTriangle(Vec3f start, Vec3f dir, Triangle triangle, float tMax, float &tHit, float &u, float &v) {

	// Möller-Trumbore test
	Vec3f e1 = triangle.v2 - triangle.v1;
	Vec3f e2 = triangle.v3 - triangle.v1;
	Vec3f cross = dir.Cross(e2);
	float det = e1.Dot(cross);

	if(fabs(det) < PLANE_EQUALS_EPS) {		// Parallel
		return false;
	}

	float detInverse = 1.f / det;

	// Find u
	Vec3f s = start - triangle.v1;
	u = detInverse * s.Dot(cross);
	if(u < 0 || u > 1) {
		return false;
	}

	// Find v
	Vec3f cross2 = s.Cross(e1);
	v = detInverse * dir.Dot(cross2);
	if(v < 0 || u + v > 1) {
		return false;
	}

	tHit = detInverse * e2.Dot(cross2);
	
	if(tHit > tMax) {	// We went too far
		return false;
	}

	return true;
}

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
	float tHit = tMax;

	if(scene->accelerate && scene->hasBvh) {
		Triangle hitTriangle;
		float u, v;
		if(scene->bvh->RayBvh(start, dir, 0, tMax, tHit, hitTriangle, u, v)) {
			if(!(tHit < RAY_EPS)) {
				return true;
			}
		}
	}
	else {
		for(Triangle triangle : scene->triangles) {
			float u, v;
			if(HitCheckTriangle(start, dir, triangle, tMax, tHit, u, v)) {
				if(!(tHit < RAY_EPS)) {
					return true;
				}
			}
		}
	}

	for(Sphere sphere : scene->spheres) {
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

Color Shade(Vec3f v, Vec3f n, Vec3f p, Material material, Scene *scene, bool noRefract, int depth) {
	Color shade = scene->ambient * material.ambient;

	for(DirectionalLight directionalLight : scene->directionalLights) {
		Vec3f lightDir = directionalLight.direction;
		lightDir.Normalize();
		Vec3f toLight = lightDir;
		toLight.Negate();
			
		if((n.Dot(toLight) > PLANE_EQUALS_EPS) && !HitCheckScene(p, toLight, MAX_T, scene)) {		// Cast another ray for shadowing
			float diffuse = std::clamp(n.Dot(toLight), 0.f, 1.f);
		
			Vec3f rayReflected = ((2 * lightDir.Dot(n)) * n) - lightDir;
			float specular = std::clamp(rayReflected.Dot(v), 0.f, 1.f);
			specular = powf(specular, material.specularCoeff);

			shade = shade + (directionalLight.intensity * material.diffuse * diffuse) + (directionalLight.intensity * material.specular * specular); 
		}
	}

	for(PointLight pointLight : scene->pointLights) {
		Vec3f lightDir = p - pointLight.origin;
		float dist = lightDir.Normalize();
		Vec3f toLight = lightDir;
		toLight.Negate();
		
		if((n.Dot(toLight) > PLANE_EQUALS_EPS) && !HitCheckScene(p, toLight, dist, scene)) {		// Cast another ray for shadowing
			float diffuse = std::clamp(n.Dot(toLight), 0.f, 1.f);
			
			Vec3f rayReflected = ((2 * lightDir.Dot(n)) * n) - lightDir;
			float specular = powf(std::clamp(rayReflected.Dot(v), 0.f, 1.f), material.specularCoeff);
			specular = powf(specular, material.specularCoeff);

			float falloff = 1 / (KC + KL * dist + KQ * (dist * dist));
			shade = shade + falloff * (material.diffuse * pointLight.intensity * diffuse + material.specular * pointLight.intensity * specular);
		}
	}

	if(!(depth > scene->maxDepth)) {
		float fresnelFactor;

		if(n.Dot(v) > 0) {	// In a dielectric
			fresnelFactor = GetFresnelFactor(material.refractionCoeff, 1, v, n);
		} 
		else {			// In air
			fresnelFactor = GetFresnelFactor(1, material.refractionCoeff, v, n);
		}

		if(!(material.specular.Length() < 0.001)) {
			Vec3f rayReflected = (-2 * v.Dot(n) * n) + v;
			Color reflection = material.specular * RayTraceScene(p, rayReflected, scene, depth + 1);
			shade = shade + reflection;
		}


		if((!(fresnelFactor == 1.f)) && !(material.transmissive.Length() < 0.001)) {	// Make sure the ray is not being hyper-reflected	
				Vec3f rayRefracted;
				Vec3f refractedPerp = (material.refractionCoeff) * (v + -v.Dot(n) * n);
				Vec3f refractedParallel = sqrtf(fabs(1.0 - refractedPerp.Dot(refractedPerp))) * n;
				refractedParallel.Negate();
				rayRefracted = refractedPerp + refractedParallel;
				Color refraction = (1 - fresnelFactor) * RayTraceScene(p, rayRefracted, scene, depth + 1);

				shade = shade + (material.transmissive * refraction);
		}

	}

	shade = Color(std::clamp(shade.r, 0.f, 1.f), std::clamp(shade.g, 0.f, 1.f), std::clamp(shade.b, 0.f, 1.f)); 

	return shade;
}

Color RayTraceScene(Vec3f start, Vec3f dir, Scene *scene, int depth) {
	bool noRefract;
	float tMax = MAX_T;
	bool hit = false;
	Vec3f v, n, p;			// For shading
	Material material;	// Material, also for shading
	float tHit = tMax;

	if(scene->accelerate && scene->hasBvh) {
		Triangle hitTriangle;
		float uCoord, vCoord;
		if(scene->bvh->RayBvh(start, dir, 0, tMax, tHit, hitTriangle, uCoord, vCoord)) {
			if(!(tHit < RAY_EPS)) {
				v = tHit * dir;					// Vector from eye to hit point
				p = start + v;					// Hit point
				if(hitTriangle.useNormals) {
					n = (uCoord * hitTriangle.n1) + (vCoord * hitTriangle.n2) + ((1 - uCoord - vCoord) * hitTriangle.n3);
					n.Normalize();
				}
				else {
					n = hitTriangle.plane.normal;	// Triangle normal
					if((n.Dot(start) - hitTriangle.plane.dist) < 0) {			// Flip if facing away from ray
						n.Negate();
					}
				}
				material = hitTriangle.material;

				tMax = tHit;
				hit = true;
				noRefract = true;
			}
		}

	}
	else {
		for(Triangle triangle : scene->triangles) {
			float uCoord, vCoord;
			if(HitCheckTriangle(start, dir, triangle, tMax, tHit, uCoord, vCoord)) {
				if(tHit > RAY_EPS) {
					v = tHit * dir;				// Vector from eye to hit point
					p = start + v;				// Hit point

					if(triangle.useNormals) {
						n = (uCoord * triangle.n1) + (vCoord * triangle.n2) + ((1 - uCoord - vCoord) * triangle.n3);
						n.Normalize();
					}
					else {
						n = triangle.plane.normal;	// Triangle normal
						if((n.Dot(start) - triangle.plane.dist) < 0) {			// Flip if facing away from ray
							n.Negate();
						}
					}

					material = triangle.material;

					tMax = tHit;
					hit = true;
					noRefract = true;
				}
			}
		}

	}


	for(Sphere sphere : scene->spheres) {
		if(HitCheckSphere(start, dir, tMax, sphere.origin, sphere.r, tHit)) {
			if(!(tHit < RAY_EPS)) {
				v = tHit * dir;				// Vector from eye to hit point
				p = start + v;				// Point on sphere
				n = p - sphere.origin;		// Surface normal
				material = sphere.material;

				tMax = tHit;	// Truncate the ray. This helps with performance
				hit = true;
				noRefract = false;
			}
		}
	}

	if(!hit) {
		return scene->background;
	}

	// Since we got the closest hit data, we can now shade
	
	n.Normalize();
	v.Normalize();
	Color c = Shade(v, n, p, material, scene, noRefract, depth);

	//std::cout << c.r << " " << c.g << " "  << c.b << std::endl;

	return c;	// Costly, so it's good to do once per ray
}

// Returns time taken for the process
// Accelerated with OpenMP
double RayTracePixel(int i, int j, Camera camera, int imgW, int imgH, double halfW, double halfH, float d, Scene *raytracerScene, Image *outputImage) {
	Color color = Color(0, 0, 0);
	int samples;
	double start = omp_get_wtime();
	//#pragma omp parallel for schedule(static, 1) num_threads(SAMPLE_COUNT)
	for(samples = 0; samples < SAMPLE_COUNT; samples++) {		// Do a few samples to beat aliasing
		float u = (halfW - imgW * (i / ((double) imgW)));
		float v = (halfH - imgH * (j / ((double) imgH)));
		Vec3f p = camera.eye - d * camera.fwd + u * camera.right + v * camera.up;
		Vec3f rayDir = (p - camera.eye);
		rayDir.Normalize();

		color = color + RayTraceScene(camera.eye, rayDir, raytracerScene, 1);
	}
	double end = omp_get_wtime();
	
	color = color / SAMPLE_COUNT;

	(*outputImage).SetPixel(i, j, color);

	double time = end - start;
	//std::cout << time << std::endl;
	return time;
}

int main(int argc, char** argv) {
	if(argc < 2) {
		std::cerr << "Usage: ./a.out scenefile" << std::endl;
		return 0;
	}

	const char *fileName = argv[1];
	SceneLoader loader;
	Scene *raytracerScene = loader.ParseSceneFile(fileName);

	std::cout << "--- RAYTRACING SCENE ---" << std::endl;

	if(argv[2]) {
		std::string optimize = argv[2];
		if(optimize == "-accelerate") {
			std::cout << "Using triangle BVH" << std::endl;
			raytracerScene->accelerate = true;
		}
	}


	Camera camera = raytracerScene->camera;

	int imgW = raytracerScene->imageWidth;
	int imgH = raytracerScene->imageHeight;
	double halfW = imgW/2;
	double halfH = imgH/2;
	float d = halfH / tanf(camera.halfAngleFov * (M_PI / 180.0f));

	double start = omp_get_wtime();
	double totalTime = 0;
	Image outputImage = Image(raytracerScene->imageWidth, raytracerScene->imageHeight);
	#pragma omp parallel for num_threads(12)
	for(int j = 0; j < raytracerScene->imageHeight; j++) {
		for(int i = 0; i < raytracerScene->imageWidth; i++) {
			totalTime += RayTracePixel(i, j, camera, imgH, imgW, halfW, halfH, d, raytracerScene, &outputImage);
		}
		if(j%32 == 0) {
			double elapsed =  round((j / (double) imgH) * 100);
			std::cout << elapsed << "%" << std::endl;
		}
	}
	double end = omp_get_wtime();
	
	std::cout << "Done!" << std::endl;
	//std::cout << "Raytracing took: " << totalTime << " seconds" << std::endl;
	std::cout << "Raytracing took: " << end - start << " seconds" << std::endl;

	outputImage.Write(raytracerScene->outputImage.c_str());

	delete raytracerScene;

	return 0;
}
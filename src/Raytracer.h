#ifndef RAYTRACER_INCLUDED
#define RAYTRACER_INCLUDED

#include "Math.h"
#include "scene/SceneLoader.h"

bool HitCheckTriangle(Vec3f start, Vec3f dir, Triangle triangle, float tMax, float &tHit);
bool HitCheckSphere(Vec3f start, Vec3f dir, float tMax, Vec3f spherePos, float r, float &tHit);
bool HitCheckScene(Vec3f start, Vec3f dir, float tMax, Scene *scene);
float GetFresnelFactor(float refractionCoeff1, float refractionCoeff2, Vec3f v, Vec3f n);
Color Shade(Vec3f v, Vec3f n, Vec3f p, Material material, Scene *scene, bool noRefract, int depth);
Color RayTraceScene(Vec3f start, Vec3f dir, Scene *scene, int depth);
#endif
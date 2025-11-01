#ifndef MAP_INCLUDED
#define MAP_INCLUDED

#include "Math.h"

struct Camera {
	Vec3f eye;
	Vec3f fwd;
	Vec3f up;
    Vec3f right;
	float halfAngleFov;
};

typedef Color Background;

struct Material {
    Color ambient, diffuse, specular, transmissive;
    float specularCoeff, refractionCoeff;
};

struct Sphere {
    Vec3f origin;
    float r;
    Material material;
};

struct DirectionalLight {
    Vec3f direction;
    Color intensity;
};

struct PointLight {
    Vec3f origin;
    Color intensity;
};

struct SpotLight {
    Vec3f origin, direction;
    Color intensity;
    float angle1, angle2;
};

typedef Color AmbientLight;

#endif
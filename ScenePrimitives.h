#ifndef MAP_INCLUDED
#define MAP_INCLUDED

#include "Math.h"

struct Camera {
	Vec3f cameraPos;
	Vec3f cameraFwd;
	Vec3f cameraUp;
	float halfAngleFov;
};

struct Background {
    Color background;
};

struct Material {
    Color ambient, diffuse, specular, transmissive;
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
    float angle1, angle2;
};

struct AmbientLight {
    Color ambient;
};

#endif
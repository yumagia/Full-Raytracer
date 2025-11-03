#ifndef MAP_INCLUDED
#define MAP_INCLUDED

#include "Math.h"

// Note: A lot of these should be classes?

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

// A Vec3f representing a vertex
typedef Vec3f Vertex;

// A Vec3f representing a normal
typedef Vec3f Normal;

struct Plane {
	Normal	normal;
	float	dist;
};

// NOTE: Should be a class?
struct Triangle {
	// NOTE: Write a c++ file for this?
	void CreatePlane() {
		plane.normal = (v2 - v1).Cross(v3 - v1);
		plane.normal.Normalize();
		area = (plane.normal.Normalize()) / 2;
	}
	bool useNormals = false;
	
	Vertex v1, v2, v3;
	Normal n1, n2, n3;
	Plane plane;
	float area;
	Material material;
};

// NOTE: Should inherit from triangle?
struct NormalTriangle {
	void CreatePlane() {
		plane.normal = (v2 - v1).Cross(v3 - v1);
		plane.normal.Normalize();
		area = (plane.normal.Normalize()) / 2;
	}

	Vertex v1, v2, v3;
	Normal n1, n2, n3;
	Plane plane;
	float area;
	Material material;
};

// A color representing scene ambient
typedef Color AmbientLight;

#endif
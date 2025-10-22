#ifndef MATH_INCLUDED
#define MATH_INCLUDED

#include <math.h>

class Vec3f {
public:
	Vec3f() {}
	Vec3f(float x, float y, float z) : m_x(x), m_y(y), m_z(z) {}
	Vec3f(const Vec3f &v2) : m_x(v2.m_x), m_y(v2.m_y), m_z(v2.m_z) {}

	float Length() {
		return sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
	}

	float Normalize();
	Vec3f Cross(const Vec3f &v2) const;
	float Dot(const Vec3f &v2) const;

	float m_x = 0.f;
	float m_y = 0.f;
	float m_z = 0.f;

	Vec3f operator+(const Vec3f &v2) const;
	Vec3f operator-(const Vec3f &v2) const;
	Vec3f operator*(float s) const;
	Vec3f operator/(float s) const;
};


// Floating-point quaternion in (w, x, y, z) convention
class Quaternion {
public:
	Quaternion() {}		// Will be the indentity quaternion
    Quaternion(float w, float x, float y, float z) : m_w(w), m_x(x), m_y(y), m_z(z) {}
	Quaternion(const Quaternion &q2) : m_w(q2.m_w), m_x(q2.m_x), m_y(q2.m_y), m_z(q2.m_z) {}

	Quaternion(const Vec3f &axis, float angle); // Create from an axis-angle

	Quaternion GetConjugate();
	void Normalize();
	Vec3f RotateVector(const Vec3f &v) const;

	float m_w = 1.f;
	float m_x = 0.f;
	float m_y = 0.f;
	float m_z = 0.f;
	
	Quaternion operator*(const Quaternion &q2) const;
};

// R, G, and B float values between 0-1
class Color3f {
public:
	Color3f() {}
	Color3f(float r, float g, float b) : m_r(r), m_g(g), m_b(b) {}
	Color3f(const Color3f &c2) : m_r(c2.m_r), m_g(c2.m_g), m_b(c2.m_b) {}

	float m_r = 0.0;
	float m_g = 0.0;
	float m_b = 0.0;

	Color3f operator+(const Color3f &c2) const;
	Color3f operator-(const Color3f &c2) const;
	Color3f operator*(float s) const;
	Color3f operator/(float s) const;
};

#endif
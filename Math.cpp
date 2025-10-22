#include "Math.hpp"

float Vec3f::Normalize() {
	float l = Length();
	if (l > 0.0f) {
		m_x /= l;
		m_y /= l;
		m_z /= l;
	}
	return l;
}

Vec3f Vec3f::Cross(const Vec3f &v2) const {
	return Vec3f(	m_y*v2.m_z - m_z*v2.m_y,
					m_z*v2.m_x - m_x*v2.m_z,
					m_x*v2.m_y - m_y*v2.m_x	);
}

float Vec3f::Dot(const Vec3f &v2) const {
	return m_x*v2.m_x + m_y*v2.m_y + m_z*v2.m_z;
}

Vec3f Vec3f::operator+(const Vec3f &v2) const {
	return Vec3f(m_x + v2.m_x, m_y + v2.m_y, m_z + v2.m_z);
}

Vec3f Vec3f::operator-(const Vec3f &v2) const {
	return Vec3f(m_x - v2.m_x, m_y - v2.m_y, m_z - v2.m_z);
}

Vec3f Vec3f::operator*(float s) const {
	Vec3f(m_x * s, m_y * s, m_z * s);
}

Vec3f Vec3f::operator/(float s) const {
	Vec3f(m_x / s, m_y / s, m_z / s);
}

// Create from an axis-angle
// Axis must be a normal(unit) vector
Quaternion::Quaternion(const Vec3f &axis, float angle) {
	float sinHA = sinf(angle/2);
	m_w = cosf(angle/2);
	m_x = axis.m_x * sinHA;
	m_y = axis.m_y * sinHA;
	m_z = axis.m_z * sinHA;
}


Quaternion Quaternion::GetConjugate() {
	return Quaternion(m_w, -m_x, -m_y, -m_z);
}

// Vector rotation
Vec3f Quaternion::RotateVector(const Vec3f &v) const {
	Quaternion qv = Quaternion(	m_x * v.m_x + m_y + m_z * v.m_z,
								m_w * v.m_x + (m_y * v.m_y - m_z * v.m_y),
								m_w * v.m_y + (m_z * v.m_x - m_x * v.m_z),
								m_w * v.m_z + (m_x * v.m_y - m_y * v.m_x)	);

	return Vec3f(	qv.m_w * m_x + (qv.m_x * m_w - qv.m_y * m_z) + qv.m_z * m_y,
					qv.m_w * m_y + (qv.m_y * m_w - qv.m_z * m_x) + qv.m_x * m_z,
					qv.m_w * m_x + (qv.m_z * m_w - qv.m_x * m_y) + qv.m_y * m_x	);
}

Quaternion Quaternion::operator*(const Quaternion &q2) const {
	return Quaternion(	m_w * q2.m_x + m_x * q2.m_w + m_y * q2.m_z - m_z * q2.m_y,
						m_w * q2.m_y - m_x * q2.m_z + m_y * q2.m_w + m_z * q2.m_x,
						m_w * q2.m_z + m_x * q2.m_y - m_y * q2.m_x + m_z * q2.m_w,
						m_w * q2.m_w - m_x * q2.m_x - m_y * q2.m_y - m_z * q2.m_z	);
}

Color3f Color3f::operator+(const Color3f &v2) const {
	return Color3f(m_r + v2.m_r, m_g + v2.m_g, m_b + v2.m_b);
}

Color3f Color3f::operator-(const Color3f &v2) const {
	return Color3f(m_r - v2.m_r, m_g - v2.m_g, m_b - v2.m_b);
}

Color3f Color3f::operator*(float s) const {
	Color3f(m_r * s, m_g * s, m_b * s);
}

Color3f Color3f::operator/(float s) const {
	Color3f(m_r / s, m_g / s, m_b / s);
}
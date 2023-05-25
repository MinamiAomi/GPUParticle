#pragma once
#ifndef MATHUTILS_INLINE_H_
#define MATHUTILS_INLINE_H_
#include "MathUtils.h"

namespace Math {
	inline float ToRad(float deg) { return deg * Pi / 180.0f; }
	inline float ToDeg(float rad) { return rad * 180.0f / Pi; }

#pragma region Vector2
	inline Vector2::Vector2() : x(0.0f), y(0.0f) {}
	inline Vector2::Vector2(float x, float y) : x(x), y(y) {}
	inline Vector2::Vector2(float xy) : x(xy), y(xy) {}
	inline Vector2 operator+(const Vector2& v) {
		return { +v.x, +v.y };
	}
	inline Vector2 operator-(const Vector2& v) {
		return { -v.x, -v.y };
	}
	inline Vector2 operator+(const Vector2& v1, const Vector2& v2) {
		return { v1.x + v2.x, v1.y + v2.y };
	}
	inline Vector2 operator-(const Vector2& v1, const Vector2& v2) {
		return { v1.x - v2.x, v1.y - v2.y };
	}
	inline Vector2 operator*(const Vector2& v, float s) {
		return { v.x * s, v.y * s };
	}
	inline Vector2 operator*(float s, const Vector2& v) {
		return { s * v.x, s * v.y };
	}
	inline Vector2& operator+=(Vector2& v1, const Vector2& v2) {
		v1.x += v2.x;
		v1.y += v2.y;
		return v1;
	}
	inline Vector2& operator-=(Vector2& v1, const Vector2& v2) {
		v1.x -= v2.x;
		v1.y -= v2.y;
		return v1;
	}
	inline Vector2& operator*=(Vector2& v, float s) {
		v.x *= s;
		v.y *= s;
		return v;
	}
	inline bool operator==(const Vector2& v1, const Vector2& v2) {
		return v1.x == v2.x && v1.y == v2.y;
	}
	inline bool operator!=(const Vector2& v1, const Vector2& v2) {
		return v1.x != v2.x || v1.y != v2.y;
	}
	inline float Dot(const Vector2& v1, const Vector2& v2) {
		return v1.x * v2.x + v1.y * v2.y;
	}
	inline float Cross(const Vector2& v1, const Vector2& v2) {
		return v1.x * v2.y - v1.y * v2.x;
	}
	inline float LengthSquare(const Vector2& v) {
		return Dot(v, v);
	}
	inline float Length(const Vector2& v) {
		return std::sqrt(LengthSquare(v));
	}
	inline Vector2 Normalize(const Vector2& v) {
		return v * (1.0f / Length(v));
	}
	inline Vector2 Lerp(float t, const Vector2& start, const Vector2& end) {
		return start * t + end * (1.0f - t);
	}
	inline Vector2 Project(const Vector2& v1, const Vector2& v2) {
		Vector2 normV2 = Normalize(v2);
		return Dot(v1, normV2) * normV2;
	}
	inline Vector2 Mid(const Vector2& v1, const Vector2& v2) {
		return (v1 + v2) * 0.5f;
	}
	inline Vector2 Reflected(const Vector2& v, const Vector2& n) {
		return Dot(n, -v) * 2.0f * n + v;
	}
#pragma endregion
#pragma region Vector3
	inline Vector3::Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
	inline Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
	inline Vector3::Vector3(float xyz) : x(xyz), y(xyz), z(xyz) {}
	inline Vector3::Vector3(const Vector2& xy, float z) : x(xy.x), y(xy.y), z(z) {}
	inline Vector3& Vector3::xy(const Vector2& xy) {
		x = xy.x;
		y = xy.y;
		return *this;
	}
	inline Vector2 Vector3::xy() const {
		return{ x, y };
	}
	inline Vector3& Vector3::xz(const Vector2& xz) {
		x = xz.x;
		z = xz.y;
		return *this;
	}
	inline Vector2 Vector3::xz() const {
		return { x, z };
	}
	inline Vector3& Vector3::yz(const Vector2& yz) {
		y = yz.x;
		z = yz.y;
		return *this;
	}
	inline Vector2 Vector3::yz() const {
		return { y, z };
	}
	inline Vector3 operator+(const Vector3& v) {
		return { +v.x, +v.y, +v.z };
	}
	inline Vector3 operator-(const Vector3& v) {
		return { -v.x, -v.y, -v.z };
	}
	inline Vector3 operator+(const Vector3& v1, const Vector3& v2) {
		return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
	}
	inline Vector3 operator-(const Vector3& v1, const Vector3& v2) {
		return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
	}
	inline Vector3 operator*(const Vector3& v, float s) {
		return { v.x * s, v.y * s, v.z * s };
	}
	inline Vector3 operator*(float s, const Vector3& v) {
		return { s * v.x, s * v.y, s * v.z };
	}
	inline Vector3& operator+=(Vector3& v1, const Vector3& v2) {
		v1.x += v2.x;
		v1.y += v2.y;
		v1.z += v2.z;
		return v1;
	}
	inline Vector3& operator-=(Vector3& v1, const Vector3& v2) {
		v1.x -= v2.x;
		v1.y -= v2.y;
		v1.z -= v2.z;
		return v1;
	}
	inline Vector3& operator*=(Vector3& v, float s) {
		v.x *= s;
		v.y *= s;
		v.z *= s;
		return v;
	}
	inline bool operator==(const Vector3& v1, const Vector3& v2) {
		return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
	}
	inline bool operator!=(const Vector3& v1, const Vector3& v2) {
		return v1.x != v2.x || v1.y != v2.y || v1.z != v2.z;
	}
	inline float Dot(const Vector3& v1, const Vector3& v2) {
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}
	inline Vector3 Cross(const Vector3& v1, const Vector3& v2) {
		return {
			v1.y * v2.z - v1.z * v2.y,
			v1.z * v2.x - v1.x * v2.z,
			v1.x * v2.y - v1.y * v2.x };
	}
	inline float LengthSquare(const Vector3& v) {
		return Dot(v, v);
	}
	inline float Length(const Vector3& v) {
		return std::sqrt(LengthSquare(v));
	}
	inline Vector3 Normalize(const Vector3& v) {
		return v * (1.0f / Length(v));
	}
	inline Vector3 Lerp(float t, const Vector3& start, const Vector3& end) {
		return start * t + end * (1.0f - t);
	}
	inline Vector3 Project(const Vector3& v1, const Vector3& v2) {
		Vector3 normV2 = Normalize(v2);
		return Dot(v1, normV2) * normV2;
	}

	inline Vector3 Mid(const Vector3& v1, const Vector3& v2) {
		return (v1 + v2) * 0.5f;
	}
	inline Vector3 Reflected(const Vector3& v, const Vector3& n) {
		return Dot(n, -v) * 2.0f * n + v;
	}
#pragma endregion
#pragma region Vector4
	inline Vector4::Vector4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
	inline Vector4::Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
	inline Vector4::Vector4(float xyzw) : x(xyzw), y(xyzw), z(xyzw), w(xyzw) {}
	inline Vector4::Vector4(const Vector3& xyz, float w) : x(xyz.x), y(xyz.y), z(xyz.z), w(w) {}
	inline Vector4& Vector4::xyz(const Vector3& xyz) {
		x = xyz.x;
		y = xyz.y;
		z = xyz.z;
		return *this;
	}
	inline Vector3 Vector4::xyz() const {
		return { x, y, z };
	}

	inline Vector4 operator+(const Vector4& v) {
		return { +v.x, +v.y, +v.z, +v.w };
	}
	inline Vector4 operator-(const Vector4& v) {
		return { -v.x, -v.y, -v.z, -v.z };
	}
	inline Vector4 operator+(const Vector4& v1, const Vector4& v2) {
		return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w };
	}
	inline Vector4 operator-(const Vector4& v1, const Vector4& v2) {
		return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w };
	}
	inline Vector4 operator*(const Vector4& v, float s) {
		return { v.x * s, v.y * s, v.z * s, v.w * s };
	}
	inline Vector4 operator*(float s, const Vector4& v) {
		return { s * v.x, s * v.y, s * v.z, s * v.w };
	}
	inline Vector4& operator+=(Vector4& v1, const Vector4& v2) {
		v1.x += v2.x;
		v1.y += v2.y;
		v1.z += v2.z;
		v1.w += v2.w;
		return v1;
	}
	inline Vector4& operator-=(Vector4& v1, const Vector4& v2) {
		v1.x -= v2.x;
		v1.y -= v2.y;
		v1.z -= v2.z;
		v1.w -= v2.w;
		return v1;
	}
	inline Vector4& operator*=(Vector4& v, float s) {
		v.x *= s;
		v.y *= s;
		v.z *= s;
		v.w *= s;
		return v;
	}
	inline bool operator==(const Vector4& v1, const Vector4& v2) {
		return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w;
	}
	inline bool operator!=(const Vector4& v1, const Vector4& v2) {
		return v1.x != v2.x || v1.y != v2.y || v1.z != v2.z || v1.w != v2.w;
	}
	inline float Dot(const Vector4& v1, const Vector4& v2) {
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
	}
	inline float LengthSquare(const Vector4& v) {
		return Dot(v, v);
	}
	inline float Length(const Vector4& v) {
		return std::sqrt(LengthSquare(v));
	}
	inline Vector4 Normalize(const Vector4& v) {
		return v * (1.0f / Length(v));
	}
	inline Vector4 Lerp(float t, const Vector4& start, const Vector4& end) {
		return start * t + end * (1.0f - t);
	}
	Vector4 Project(const Vector4& v1, const Vector4& v2) {
		Vector4 normV2 = Normalize(v2);
		return Dot(v1, normV2) * normV2;
	}
	inline Vector4 Mid(const Vector4& v1, const Vector4& v2) {
		return (v1 + v2) * 0.5f;
	}
#pragma endregion
#pragma region Quaternion
	inline Quaternion::Quaternion() : Vector4(0.0f, 0.0f, 0.0f, 1.0f) {}
	inline Quaternion::Quaternion(float x, float y, float z, float w) : Vector4(x, y, z, w) {}
	inline float Quaternion::GetAngle() const {
		return std::acos(w) * 2.0f;
	}
	inline Vector3 Quaternion::GetAxis() const {
		return xyz() * (1.0f / std::sin(std::acos(w)));
	}
	inline Vector3 Quaternion::GetEuler() const {
		Vector3 result;
		result.x = std::asin(2.0f * y * z + 2.0f * x * w);
		if (result.x != 0.0f) {
			result.y = std::atan(-((2.0f * x * z + -2.0f * y * w) / (2.0f * w * w + 2.0f * z * z - 1)));
			result.z = std::atan(-((2.0f * x * y + -2.0f * z * w) / (2.0f * w * w + 2.0f * y * y - 1)));
			return result;
		}
		result.y = 0.0f;
		result.z = std::atan((2.0f * x * y + 2.0f * z * w) / (2.0f * w * w + 2.0f * x * x - 1));
		return result;
	}
	inline Quaternion Quaternion::MakeFromAngleAxis(float angle, const Vector3& axis) {
		float sin2 = std::sin(angle * 0.5f);
		return Quaternion{
			axis.x * sin2,
			axis.y * sin2,
			axis.z * sin2,
			std::cos(angle * 0.5f)
		};
	}
	inline Quaternion Quaternion::MakeFromEulerAngle(const Vector3& euler) {
		// ピッチ ヨー ロールの順
		Vector3 s = Vector3(std::sin(euler.x * 0.5f), std::sin(euler.y * 0.5f), std::sin(euler.z * 0.5f));
		Vector3 c = Vector3(std::cos(euler.x * 0.5f), std::cos(euler.y * 0.5f), std::cos(euler.z * 0.5f));
		return Quaternion{
			s.x * c.y * c.z - c.x * s.y * s.z,
			c.x * s.y * c.z + s.x * c.y * s.z,
			c.x * c.y * s.z - s.x * s.y * c.z,
			c.x * c.y * c.z + s.x * s.y * s.z
		};
	}
	inline Quaternion Quaternion::MakeForXAxis(float angle) {
		return Quaternion(std::sin(angle / 2.0f), 0.0f, 0.0f, std::cos(angle / 2.0f));
	}
	inline Quaternion Quaternion::MakeForYAxis(float angle) {
		return Quaternion(0.0f, std::sin(angle / 2.0f), 0.0f, std::cos(angle / 2.0f));
	}
	inline Quaternion Quaternion::MakeForZAxis(float angle) {
		return Quaternion(0.0f, 0.0f, std::sin(angle / 2.0f), std::cos(angle / 2.0f));
	}
	inline Quaternion Quaternion::MakeFromTwoVector(const Vector3& v1, const Vector3& v2) {
		Vector3 axis = Normalize(Cross(v1, v2));
		float angle = std::acos(Dot(v1, v2));
		return MakeFromAngleAxis(angle, axis);
	}
	inline Quaternion Quaternion::MakeFromOrthonormal(const Vector3& x, const Vector3& y, const Vector3& z) {
		float trace = x.x + y.y + z.z;

		if (trace > 0.0f) {
			float s = std::sqrt(trace + 1.0f) * 0.5f;
			Quaternion result{};
			result.w = s;
			s = 0.25f / s;
			result.x = (y.z - z.y) * s;
			result.y = (z.x - x.z) * s;
			result.z = (x.y - y.x) * s;
			return result;
		}
		else if (x.x > y.y && x.x > z.z) {
			float s = std::sqrt(1.0f + x.x - y.y - z.z) * 0.5f;
			Quaternion result{};
			result.x = s;
			s = 0.25f / s;
			result.y = (x.y + y.x) * s;
			result.z = (z.x + x.z) * s;
			result.w = (y.z - z.y) * s;
			return result;
		}
		else if (y.y > z.z) {
			float s = std::sqrt(1.0f - x.x + y.y - z.z) * 0.5f;
			Quaternion result{};
			result.y = s;
			s = 0.25f / s;
			result.x = (x.y + y.x) * s;
			result.z = (y.z + z.y) * s;
			result.w = (z.x - x.z) * s;
			return result;
		}
		Quaternion result{};
		float s = std::sqrt(1.0f - x.x - y.y + z.z) * 0.5f;
		result.z = s;
		s = 0.25f / s;
		result.x = (z.x + x.z) * s;
		result.y = (y.z + z.y) * s;
		result.w = (x.y - y.x) * s;
		return result;
	}
	inline Quaternion Quaternion::MakeLookRotation(const Vector3& direction, const Vector3& up) {
		Vector3 z = Normalize(direction);
		Vector3 x = Normalize(Cross(up, z));
		Vector3 y = Cross(z, x);
		return MakeFromOrthonormal(x, y, z);
	}
	inline Quaternion Quaternion::MakeFromMatrix(const Matrix44& m) {
		Vector3 x{ m.m[0][0],m.m[0][1],m.m[0][2] };
		Vector3 y{ m.m[1][0],m.m[1][1],m.m[1][2] };
		Vector3 z{ m.m[2][0],m.m[2][1],m.m[2][2] };
		return MakeFromOrthonormal(x, y, z);
	}
	inline Quaternion operator+(const Quaternion& q1, const Quaternion& q2) {
		return Quaternion{ q1.x + q2.x, q1.y + q2.y, q1.z + q2.z, q1.w + q2.w };
	}
	inline Quaternion operator*(float s, const Quaternion& q) {
		return Quaternion{ s * q.x, s * q.y, s * q.z, s * q.w };
	}
	inline Quaternion operator*(const Quaternion& q, float s) {
		return Quaternion{ q.x * s, q.y * s, q.z * s, q.w * s };
	}
	inline Quaternion operator*(const Quaternion& q1, const Quaternion& q2) {
		Vector3 qv = q1.xyz();
		Vector3 pv = q2.xyz();
		Vector3 v = q1.w * pv + q2.w * qv + Cross(qv, pv);
		float w = q1.w * q2.w - Dot(qv, pv);
		return Quaternion{ v.x, v.y, v.z, w };
	}
	inline Vector3 operator*(const Quaternion& q, const Vector3& v) {
		Vector3 qv = q.xyz();
		return v + 2.0f * Cross(qv, Cross(qv, v) + q.w * v);
	}
	inline Quaternion Conjugate(const Quaternion& q) {
		return Quaternion{ -q.x, -q.y, -q.z, q.w };
	}
	inline Quaternion Inverse(const Quaternion& q) {
		return Conjugate(q) * (1.0f / LengthSquare(q));
	}
#pragma endregion
#pragma region Matrix44
	inline Matrix44::Matrix44() {
		*this = Identity;
	}
	inline Matrix44::Matrix44(
		float _00, float _01, float _02, float _03,
		float _10, float _11, float _12, float _13,
		float _20, float _21, float _22, float _23,
		float _30, float _31, float _32, float _33) {
		m[0][0] = _00, m[0][1] = _01, m[0][2] = _02, m[0][3] = _03;
		m[1][0] = _10, m[1][1] = _11, m[1][2] = _12, m[1][3] = _13;
		m[2][0] = _20, m[2][1] = _21, m[2][2] = _22, m[2][3] = _23;
		m[3][0] = _30, m[3][1] = _31, m[3][2] = _32, m[3][3] = _33;
	}
	inline Vector3  Matrix44::GetXAxis() const {
		return { m[0][0], m[0][1], m[0][2] };
	}
	inline Vector3  Matrix44::GetYAxis() const {
		return { m[1][0], m[1][1], m[1][2] };
	}
	inline Vector3  Matrix44::GetZAxis() const {
		return { m[2][0], m[2][1], m[2][2] };
	}
	inline Vector3  Matrix44::GetTranslate() const {
		return { m[3][0], m[3][1], m[3][2] };
	}
	inline Matrix44 Matrix44::MakeScaling(const Vector3& scale) {
		return {
			scale.x, 0.0f, 0.0f, 0.0f,
			0.0f, scale.y, 0.0f, 0.0f,
			0.0f, 0.0f, scale.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f };
	}
	inline Matrix44 Matrix44::MakeRotationX(float rotate) {
		float c = std::cos(rotate);
		float s = std::sin(rotate);
		return {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, c, s, 0.0f,
			0.0f, -s, c, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f };
	}
	inline Matrix44 Matrix44::MakeRotationY(float rotate) {
		float s = std::sin(rotate);
		float c = std::cos(rotate);
		return {
			c,		0.0f,	-s,		0.0f,
			0.0f,	1.0f,	0.0f,	0.0f,
			s,		0.0f,	c,		0.0f,
			0.0f,	0.0f,	0.0f,	1.0f };
	}
	inline Matrix44 Matrix44::MakeRotationZ(float rotate) {
		float s = std::sin(rotate);
		float c = std::cos(rotate);
		return {
			c,		s,		0.0f,	0.0f,
			-s,		c,		0.0f,	0.0f,
			0.0f,	0.0f,	1.0f,	0.0f,
			0.0f,	0.0f,	0.0f,	1.0f };
	}
	inline Matrix44 Matrix44::MakeRotationXYZ(const Vector3& rotate) {
		Vector3 s = { std::sin(rotate.x), std::sin(rotate.y), std::sin(rotate.z) };
		Vector3 c = { std::cos(rotate.x), std::cos(rotate.y), std::cos(rotate.z) };
		return {
			c.y * c.z,						c.y * s.z,						-s.y,		0.0f,
			s.x * s.y * c.z - c.x * s.z,	s.x * s.y * s.z + c.x * c.z,	s.x * c.y,	0.0f,
			c.x * s.y * c.z + s.x * s.z,	c.x * s.y * s.z - s.x * c.z,	c.x * c.y,	0.0f,
			0.0f,	0.0f,	0.0f,	1.0f };
	}
	inline Matrix44 Matrix44::MakeRotationFromQuaternion(const Quaternion& q) {
		float w2 = q.w * q.w, x2 = q.x * q.x, y2 = q.y * q.y, z2 = q.z * q.z;
		float wx = q.w * q.x, wy = q.w * q.y, wz = q.w * q.z;
		float xy = q.x * q.y, xz = q.x * q.z, yz = q.y * q.z;

		return {
			w2 + x2 - y2 - z2,	2.0f * (wz + xy),	2.0f * (xz - wy),	0.0f,
			2.0f * (xy - wz),	w2 - x2 + y2 - z2,	2.0f * (yz + wx),	0.0f,
			2.0f * (wy + xz),	2.0f * (-wx + yz),	w2 - x2 - y2 + z2,	0.0f,
			0.0f,				0.0f,				0.0f,				1.0f };
	}
	inline Matrix44 Matrix44::MakeTranslation(const Vector3& translate) {
		return {
		1.0f,		0.0f,		0.0f,		0.0f,
		0.0f,		1.0f,		0.0f,		0.0f,
		0.0f,		0.0f,		1.0f,		0.0f,
		translate.x,	translate.y,	translate.z,	1.0f };
	}
	inline Matrix44 Matrix44::MakeAffine(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
		Vector3 s = { std::sin(rotate.x), std::sin(rotate.y), std::sin(rotate.z) };
		Vector3 c = { std::cos(rotate.x), std::cos(rotate.y), std::cos(rotate.z) };
		return {
				scale.x * (c.y * c.z),
				scale.x * (c.y * s.z),
				scale.x * (-s.y),
				0.0f,

				scale.y * (s.x * s.y * c.z - c.x * s.z),
				scale.y * (s.x * s.y * s.z + c.x * c.z),
				scale.y * (s.x * c.y),
				0.0f,

				scale.z * (c.x * s.y * c.z + s.x * s.z),
				scale.z * (c.x * s.y * s.z - s.x * c.z),
				scale.z * (c.x * c.y),
				0.0f,

				translate.x,
				translate.y,
				translate.z,
				1.0f
		};
	}
	inline Matrix44 Matrix44::MakeAffine(const Vector3& scale, const Quaternion& rotate, const Vector3& translate) {
		float w2 = rotate.w * rotate.w, x2 = rotate.x * rotate.x, y2 = rotate.y * rotate.y, z2 = rotate.z * rotate.z;
		float wx = rotate.w * rotate.x, wy = rotate.w * rotate.y, wz = rotate.w * rotate.z;
		float xy = rotate.x * rotate.y, xz = rotate.x * rotate.z, yz = rotate.y * rotate.z;
		return {
				scale.x * (w2 + x2 - y2 - z2),
				scale.x * (2.0f * (wz + xy)),
				scale.x * (2.0f * (xz - wy)),
				0.0f,

				scale.y * (2.0f * (xy - wz)),
				scale.y * (w2 - x2 + y2 - z2),
				scale.y * (2.0f * (yz + wx)),
				0.0f,

				scale.z * (2.0f * (wy + xz)),
				scale.z * (2.0f * (-wx + yz)),
				scale.z * (w2 - x2 - y2 + z2),
				0.0f,

				translate.x,
				translate.y,
				translate.z,
				1.0f
		};
	}
	inline Matrix44 Matrix44::MakeLookRotation(const Vector3& eye, const Vector3& target, const Vector3& up) {
		Vector3 z = Normalize(target - eye);
		Vector3 x = Normalize(Cross(up, z));
		Vector3 y = Cross(z, x);
		return {
			x.x, x.y, x.z, 0.0f,
			y.x, y.y, y.z, 0.0f,
			z.x, z.y, z.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f };
	}
	inline Matrix44 Matrix44::MakeBillBoard(const Vector3& eye, const Vector3& target, const Vector3& up) {
		return MakeLookRotation(eye, target, up);
	}
	inline Matrix44 Matrix44::MakeBillBoardYAxis(const Vector3& eye, const Vector3& target, const Vector3& up) {
		Vector3 z = Normalize(target - eye);
		Vector3 x = Normalize(Cross(up, z));
		Vector3 y = Normalize(up);
		z = Cross(x, y);	// zを再計算
		return {
			x.x, x.y, x.z, 0.0f,
			y.z, y.y, y.z, 0.0f,
			z.x, z.y, z.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f };
	}
	inline Matrix44 Matrix44::MakeView(const Vector3& eye, const Vector3& target, const Vector3& up) {
		Vector3 z = Normalize(target - eye);
		Vector3 x = Normalize(Cross(up, z));
		Vector3 y = Cross(z, x);
		Vector3 w = { -Dot(x, eye), -Dot(y, eye), -Dot(z, eye) };
		return {
			x.x, y.x, z.x, 0.0f,
			x.y, y.y, z.y, 0.0f,
			x.z, y.z, z.z, 0.0f,
			w.x, w.y, w.z, 1.0f };
	}
	inline Matrix44 Matrix44::MakePerspectiveProjection(float fovY, float aspect, float nearZ, float farZ) {
		float s = 1.0f / std::tan(fovY * 0.5f);
		float a = farZ / (farZ - nearZ);
		return {
			s / aspect, 0.0f, 0.0f, 0.0f,
			0.0f, s, 0.0f, 0.0f,
			0.0f, 0.0f, a, 1.0f,
			0.0f, 0.0f, a * -nearZ, 0.0f };
	}
	inline Matrix44 Matrix44::MakeOrthographicProjection(float width, float height, float nearZ, float farZ) {
		float zRange = farZ - nearZ;
		return {
			2.0f / width, 0.0f, 0.0f, 0.0f,
			0.0f, 2.0f / height, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f / zRange, 0.0f,
			0.0f, 0.0f, nearZ / -zRange, 1.0f };
	}
	inline Matrix44 Matrix44::MakeViewport(float left, float top, float width, float height, float nearZ, float farZ) {
		float halfW = width / 2.0f;
		float halfh = height / 2.0f;
		return {
			halfW, 0.0f, 0.0f, 0.0f,
			0.0f, -halfh, 0.0f, 0.0f,
			0.0f, 0.0f, farZ - nearZ, 0.0f,
			left + halfW, top + halfh, nearZ, 1.0f };
	}
	inline Matrix44 operator*(const Matrix44& mat1, const Matrix44& mat2) {
		return {
			mat1.m[0][0] * mat2.m[0][0] + mat1.m[0][1] * mat2.m[1][0] + mat1.m[0][2] * mat2.m[2][0] + mat1.m[0][3] * mat2.m[3][0],
			mat1.m[0][0] * mat2.m[0][1] + mat1.m[0][1] * mat2.m[1][1] + mat1.m[0][2] * mat2.m[2][1] + mat1.m[0][3] * mat2.m[3][1],
			mat1.m[0][0] * mat2.m[0][2] + mat1.m[0][1] * mat2.m[1][2] + mat1.m[0][2] * mat2.m[2][2] + mat1.m[0][3] * mat2.m[3][2],
			mat1.m[0][0] * mat2.m[0][3] + mat1.m[0][1] * mat2.m[1][3] + mat1.m[0][2] * mat2.m[2][3] + mat1.m[0][3] * mat2.m[3][3],

			mat1.m[1][0] * mat2.m[0][0] + mat1.m[1][1] * mat2.m[1][0] + mat1.m[1][2] * mat2.m[2][0] + mat1.m[1][3] * mat2.m[3][0],
			mat1.m[1][0] * mat2.m[0][1] + mat1.m[1][1] * mat2.m[1][1] + mat1.m[1][2] * mat2.m[2][1] + mat1.m[1][3] * mat2.m[3][1],
			mat1.m[1][0] * mat2.m[0][2] + mat1.m[1][1] * mat2.m[1][2] + mat1.m[1][2] * mat2.m[2][2] + mat1.m[1][3] * mat2.m[3][2],
			mat1.m[1][0] * mat2.m[0][3] + mat1.m[1][1] * mat2.m[1][3] + mat1.m[1][2] * mat2.m[2][3] + mat1.m[1][3] * mat2.m[3][3],

			mat1.m[2][0] * mat2.m[0][0] + mat1.m[2][1] * mat2.m[1][0] + mat1.m[2][2] * mat2.m[2][0] + mat1.m[2][3] * mat2.m[3][0],
			mat1.m[2][0] * mat2.m[0][1] + mat1.m[2][1] * mat2.m[1][1] + mat1.m[2][2] * mat2.m[2][1] + mat1.m[2][3] * mat2.m[3][1],
			mat1.m[2][0] * mat2.m[0][2] + mat1.m[2][1] * mat2.m[1][2] + mat1.m[2][2] * mat2.m[2][2] + mat1.m[2][3] * mat2.m[3][2],
			mat1.m[2][0] * mat2.m[0][3] + mat1.m[2][1] * mat2.m[1][3] + mat1.m[2][2] * mat2.m[2][3] + mat1.m[2][3] * mat2.m[3][3],

			mat1.m[3][0] * mat2.m[0][0] + mat1.m[3][1] * mat2.m[1][0] + mat1.m[3][2] * mat2.m[2][0] + mat1.m[3][3] * mat2.m[3][0],
			mat1.m[3][0] * mat2.m[0][1] + mat1.m[3][1] * mat2.m[1][1] + mat1.m[3][2] * mat2.m[2][1] + mat1.m[3][3] * mat2.m[3][1],
			mat1.m[3][0] * mat2.m[0][2] + mat1.m[3][1] * mat2.m[1][2] + mat1.m[3][2] * mat2.m[2][2] + mat1.m[3][3] * mat2.m[3][2],
			mat1.m[3][0] * mat2.m[0][3] + mat1.m[3][1] * mat2.m[1][3] + mat1.m[3][2] * mat2.m[2][3] + mat1.m[3][3] * mat2.m[3][3] };
	}
	inline Matrix44& operator*=(Matrix44& mat1, const Matrix44& mat2) {
		mat1 = mat1 * mat2;
		return mat1;
	}
	inline Vector3 operator*(const Vector3& vec, const Matrix44& mat) {
		return {
			vec.x * mat.m[0][0] + vec.y * mat.m[1][0] + vec.z * mat.m[2][0] + mat.m[3][0],
			vec.x * mat.m[0][1] + vec.y * mat.m[1][1] + vec.z * mat.m[2][1] + mat.m[3][1],
			vec.x * mat.m[0][2] + vec.y * mat.m[1][2] + vec.z * mat.m[2][2] + mat.m[3][2] };
	}
	inline Vector4 operator*(const Vector4& vec, const Matrix44& mat) {
		return {
			vec.x * mat.m[0][0] + vec.y * mat.m[1][0] + vec.z * mat.m[2][0] + vec.w * mat.m[3][0],
			vec.x * mat.m[0][1] + vec.y * mat.m[1][1] + vec.z * mat.m[2][1] + vec.w * mat.m[3][1],
			vec.x * mat.m[0][2] + vec.y * mat.m[1][2] + vec.z * mat.m[2][2] + vec.w * mat.m[3][2],
			vec.x * mat.m[0][3] + vec.y * mat.m[1][3] + vec.z * mat.m[2][3] + vec.w * mat.m[3][3] };
	}
	inline Matrix44 operator*(float s, const Matrix44& mat) {
		return {
			s * mat.m[0][0], s * mat.m[0][1], s * mat.m[0][2], s * mat.m[0][3],
			s * mat.m[1][0], s * mat.m[1][1], s * mat.m[1][2], s * mat.m[1][3],
			s * mat.m[2][0], s * mat.m[2][1], s * mat.m[2][2], s * mat.m[2][3],
			s * mat.m[3][0], s * mat.m[3][1], s * mat.m[3][2], s * mat.m[3][3] };
	}
	inline Matrix44 operator*(const Matrix44& mat, float s) {
		return {
			mat.m[0][0] * s, mat.m[0][1] * s, mat.m[0][2] * s, mat.m[0][3] * s,
			mat.m[1][0] * s, mat.m[1][1] * s, mat.m[1][2] * s, mat.m[1][3] * s,
			mat.m[2][0] * s, mat.m[2][1] * s, mat.m[2][2] * s, mat.m[2][3] * s,
			mat.m[3][0] * s, mat.m[3][1] * s, mat.m[3][2] * s, mat.m[3][3] * s };
	}
	inline Matrix44 Inverse(const Matrix44& m) {
		return 1.0f / Determinant(m) * Adjugate(m);
	}
	inline Matrix44 Transpose(const Matrix44& m) {
		return {
			m.m[0][0], m.m[1][0], m.m[2][0], m.m[3][0],
			m.m[0][1], m.m[1][1], m.m[2][1], m.m[3][1],
			m.m[0][2], m.m[1][2], m.m[2][2], m.m[3][2],
			m.m[0][3], m.m[1][3], m.m[2][3], m.m[3][3] };
	}
#pragma endregion
}

#endif

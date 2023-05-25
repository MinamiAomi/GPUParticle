#include "MathUtils.h"
#include "MathUtils_inline.h"

namespace Math {
	const Vector2 Vector2::UnitX{ 1.0f, 0.0f };
	const Vector2 Vector2::UnitY{ 0.0f, 1.0f };
	const Vector2 Vector2::Zero{ 0.0f, 0.0f };

	const Vector3 Vector3::UnitX{ 1.0f, 0.0f, 0.0f };
	const Vector3 Vector3::UnitY{ 0.0f, 1.0f, 0.0f };
	const Vector3 Vector3::UnitZ{ 0.0f, 0.0f, 1.0f };
	const Vector3 Vector3::Zero{ 0.0f, 0.0f, 0.0f };

	const Vector4 Vector4::Zero{ 0.0f, 0.0f, 0.0f, 0.0f };

	const Quaternion Quaternion::Identity{ 0.0f,0.0f,0.0f,1.0f };

	const Matrix44 Matrix44::Identity{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f };

	Vector2 Slerp(float t, const Vector2& s, const Vector2& e) {
		float dot = Dot(Normalize(s), Normalize(e));
		if (std::abs(dot) > 0.999f) {
			return Lerp(t, s, e);
		}
		float theta = std::acos(dot);
		float sinTheta = std::sin(theta);
		float t1 = std::sin((1.0f - t) * theta) / sinTheta;
		float t2 = std::sin(t * theta) / sinTheta;

		return t1 * s + t2 * e;
	}

	Vector2 CatmullRomSpline(float t, const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3) {
		return
			0.5f * ((-p0 + 3.0f * p1 - 3.0f * p2 + p3) * std::pow(t, 3.0f)
				+ (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * std::pow(t, 2.0f)
				+ (-p0 + p2) * t
				+ 2.0f * p1);
	}
	Vector2 QuadraticBezierCurve(float t, const Vector2& p0, const Vector2& p1, const Vector2& p2) {
		float s = 1.0f - t;
		return
			s * s * p0 +
			2.0f * t * s * p1 +
			t * t * p2;

	}
	Vector2 CubicBezierCurve(float t, const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3) {
		float s = 1.0f - t;
		return
			(s * s * s * p0) +
			(3.0f * s * s * t * p1) +
			(3.0f * s * t * t * p2) +
			t * t * t * p3;
	}
	Vector3 Slerp(float t, const Vector3& s, const Vector3& e) {
		float dot = Dot(Normalize(s), Normalize(e));
		if (std::abs(dot) > 0.999f) {
			return Lerp(t, s, e);
		}
		float theta = std::acos(dot);
		float sinTheta = std::sin(theta);
		float t1 = std::sin((1.0f - t) * theta) / sinTheta;
		float t2 = std::sin(t * theta) / sinTheta;

		return t1 * s + t2 * e;
	}
	Vector3 CatmullRomSpline(float t, const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3) {
		return
			0.5f * ((-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t * t * t
				+ (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t * t
				+ (-p0 + p2) * t
				+ 2.0f * p1);
	}
	Vector3 QuadraticBezierCurve(float t, const Vector3& p0, const Vector3& p1, const Vector3& p2) {
		float s = 1.0f - t;
		return
			s * s * p0 +
			2.0f * t * s * p1 +
			t * t * p2;

	}
	Vector3 CubicBezierCurve(float t, const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3) {
		float s = 1.0f - t;
		return
			(s * s * s * p0) +
			(3.0f * s * s * t * p1) +
			(3.0f * s * t * t * p2) +
			t * t * t * p3;
	}


	Quaternion Slerp(float t, const Quaternion& start, const Quaternion& end) {
		Quaternion s = start;
		float dot_val = Dot(start, end);
		// q1, q2が反対向きの場合
		if (dot_val < 0) {
			s.w = -s.w;
			s.x = -s.x;
			s.y = -s.y;
			s.z = -s.z;
			dot_val = -dot_val;
		}
		// 球面線形補間の計算
		float theta = std::acos(dot_val);
		return (std::sin((1.0f - t) * theta) * s + std::sin(t * theta) * end) * (1.0f / std::sin(theta));
	}

	float Determinant(const Matrix44& m) {
		float result = 0.0f;

		result += m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3]; // +11,22,33,44
		result += m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1]; // +11,23,34,42
		result += m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2]; // +11,24,32,43

		result -= m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1]; // -11,24,33,42
		result -= m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3]; // -11,23,32,44
		result -= m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2]; // -11,22,34,43

		result -= m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3]; // -12,21,33,44
		result -= m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1]; // -13,21,34,42
		result -= m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2]; // -14,21,32,43

		result += m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1]; // +14,21,33,42
		result += m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3]; // +13,21,32,44
		result += m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2]; // +12,21,34,43

		result += m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3]; // +12,23,31,44
		result += m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1]; // +13,24,31,42
		result += m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2]; // +14,22,31,43

		result -= m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1]; // -14,23,31,42
		result -= m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3]; // -13,22,31,44
		result -= m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2]; // -12,24,31,43

		result -= m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0]; // -12,23,34,41
		result -= m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0]; // -13,24,32,41
		result -= m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0]; // -14,22,33,41

		result += m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0]; // +14,23,32,41
		result += m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0]; // +13,22,34,41
		result += m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0]; // +12,24,33,41

		return result;
	}

	Matrix44 Adjugate(const Matrix44& m) {
		Matrix44 result;
		// 1行目
		result.m[0][0] = 0.0f;							// 11
		result.m[0][0] += m.m[1][1] * m.m[2][2] * m.m[3][3];	// +22,33,44
		result.m[0][0] += m.m[1][2] * m.m[2][3] * m.m[3][1];	// +23,34,42
		result.m[0][0] += m.m[1][3] * m.m[2][1] * m.m[3][2];	// +24,32,43
		result.m[0][0] -= m.m[1][3] * m.m[2][2] * m.m[3][1];	// -24,33,42
		result.m[0][0] -= m.m[1][2] * m.m[2][1] * m.m[3][3];	// -23,32,44
		result.m[0][0] -= m.m[1][1] * m.m[2][3] * m.m[3][2];	// -22,34,43

		result.m[0][1] = 0.0f;							// 12
		result.m[0][1] -= m.m[0][1] * m.m[2][2] * m.m[3][3];	// -12,33,44
		result.m[0][1] -= m.m[0][2] * m.m[2][3] * m.m[3][1];	// -13,34,42
		result.m[0][1] -= m.m[0][3] * m.m[2][1] * m.m[3][2];	// -14,32,43
		result.m[0][1] += m.m[0][3] * m.m[2][2] * m.m[3][1];	// +14,33,42
		result.m[0][1] += m.m[0][2] * m.m[2][1] * m.m[3][3];	// +13,32,44
		result.m[0][1] += m.m[0][1] * m.m[2][3] * m.m[3][2];	// +12,34,43

		result.m[0][2] = 0.0f;							// 13
		result.m[0][2] += m.m[0][1] * m.m[1][2] * m.m[3][3];	// +12,23,44
		result.m[0][2] += m.m[0][2] * m.m[1][3] * m.m[3][1];	// +13,24,42
		result.m[0][2] += m.m[0][3] * m.m[1][1] * m.m[3][2];	// +14,22,43
		result.m[0][2] -= m.m[0][3] * m.m[1][2] * m.m[3][1];	// -14,23,42
		result.m[0][2] -= m.m[0][2] * m.m[1][1] * m.m[3][3];	// -13,22,44
		result.m[0][2] -= m.m[0][1] * m.m[1][3] * m.m[3][2];	// -12,24,43

		result.m[0][3] = 0.0f;							// 14
		result.m[0][3] -= m.m[0][1] * m.m[1][2] * m.m[2][3];	// -12,23,34
		result.m[0][3] -= m.m[0][2] * m.m[1][3] * m.m[2][1];	// -13,24,32
		result.m[0][3] -= m.m[0][3] * m.m[1][1] * m.m[2][2];	// -14,22,33
		result.m[0][3] += m.m[0][3] * m.m[1][2] * m.m[2][1];	// +14,23,32
		result.m[0][3] += m.m[0][2] * m.m[1][1] * m.m[2][3];	// +13,22,34
		result.m[0][3] += m.m[0][1] * m.m[1][3] * m.m[2][2];	// +12,24,33


		// 2行目
		result.m[1][0] = 0.0f;							// 21
		result.m[1][0] -= m.m[1][0] * m.m[2][2] * m.m[3][3];	// -21,33,44
		result.m[1][0] -= m.m[1][2] * m.m[2][3] * m.m[3][0];	// -23,34,41
		result.m[1][0] -= m.m[1][3] * m.m[2][0] * m.m[3][2];	// -24,31,43
		result.m[1][0] += m.m[1][3] * m.m[2][2] * m.m[3][0];	// +24,33,41
		result.m[1][0] += m.m[1][2] * m.m[2][0] * m.m[3][3];	// +23,31,44
		result.m[1][0] += m.m[1][0] * m.m[2][3] * m.m[3][2];	// +21,34,43

		result.m[1][1] = 0.0f;							// 22
		result.m[1][1] += m.m[0][0] * m.m[2][2] * m.m[3][3];	// +11,33,44
		result.m[1][1] += m.m[0][2] * m.m[2][3] * m.m[3][0];	// +13,34,41
		result.m[1][1] += m.m[0][3] * m.m[2][0] * m.m[3][2];	// +14,31,43
		result.m[1][1] -= m.m[0][3] * m.m[2][2] * m.m[3][0];	// -14,33,41
		result.m[1][1] -= m.m[0][2] * m.m[2][0] * m.m[3][3];	// -13,31,44
		result.m[1][1] -= m.m[0][0] * m.m[2][3] * m.m[3][2];	// -11,34,43

		result.m[1][2] = 0.0f;							// 23
		result.m[1][2] -= m.m[0][0] * m.m[1][2] * m.m[3][3];	// -11,23,44
		result.m[1][2] -= m.m[0][2] * m.m[1][3] * m.m[3][0];	// -13,24,41
		result.m[1][2] -= m.m[0][3] * m.m[1][0] * m.m[3][2];	// -14,21,43
		result.m[1][2] += m.m[0][3] * m.m[1][2] * m.m[3][0];	// +14,23,41
		result.m[1][2] += m.m[0][2] * m.m[1][0] * m.m[3][3];	// +13,21,44
		result.m[1][2] += m.m[0][0] * m.m[1][3] * m.m[3][2];	// +11,24,43

		result.m[1][3] = 0.0f;							// 24
		result.m[1][3] += m.m[0][0] * m.m[1][2] * m.m[2][3];	// +11,23,34
		result.m[1][3] += m.m[0][2] * m.m[1][3] * m.m[2][0];	// +13,24,31
		result.m[1][3] += m.m[0][3] * m.m[1][0] * m.m[2][2];	// +14,21,33
		result.m[1][3] -= m.m[0][3] * m.m[1][2] * m.m[2][0];	// -14,23,31
		result.m[1][3] -= m.m[0][2] * m.m[1][0] * m.m[2][3];	// -13,21,34
		result.m[1][3] -= m.m[0][0] * m.m[1][3] * m.m[2][2];	// -11,24,33


		// 3行目
		result.m[2][0] = 0.0f;							// 31
		result.m[2][0] += m.m[1][0] * m.m[2][1] * m.m[3][3];	// +21,32,44
		result.m[2][0] += m.m[1][1] * m.m[2][3] * m.m[3][0];	// +22,34,41
		result.m[2][0] += m.m[1][3] * m.m[2][0] * m.m[3][1];	// +24,31,42
		result.m[2][0] -= m.m[1][3] * m.m[2][1] * m.m[3][0];	// -24,32,41
		result.m[2][0] -= m.m[1][1] * m.m[2][0] * m.m[3][3];	// -22,31,44
		result.m[2][0] -= m.m[1][0] * m.m[2][3] * m.m[3][1];	// -21,34,42

		result.m[2][1] = 0.0f;							// 32
		result.m[2][1] -= m.m[0][0] * m.m[2][1] * m.m[3][3];	// -11,32,44
		result.m[2][1] -= m.m[0][1] * m.m[2][3] * m.m[3][0];	// -12,34,41
		result.m[2][1] -= m.m[0][3] * m.m[2][0] * m.m[3][1];	// -14,31,42
		result.m[2][1] += m.m[0][3] * m.m[2][1] * m.m[3][0];	// +14,32,41
		result.m[2][1] += m.m[0][1] * m.m[2][0] * m.m[3][3];	// +12,31,44
		result.m[2][1] += m.m[0][0] * m.m[2][3] * m.m[3][1];	// +11,34,42

		result.m[2][2] = 0.0f;							// 33
		result.m[2][2] += m.m[0][0] * m.m[1][1] * m.m[3][3];	// +11,22,44
		result.m[2][2] += m.m[0][1] * m.m[1][3] * m.m[3][0];	// +12,24,41
		result.m[2][2] += m.m[0][3] * m.m[1][0] * m.m[3][1];	// +14,21,42
		result.m[2][2] -= m.m[0][3] * m.m[1][1] * m.m[3][0];	// -14,22,41
		result.m[2][2] -= m.m[0][1] * m.m[1][0] * m.m[3][3];	// -12,21,44
		result.m[2][2] -= m.m[0][0] * m.m[1][3] * m.m[3][1];	// -11,24,42

		result.m[2][3] = 0.0f;							// 34
		result.m[2][3] -= m.m[0][0] * m.m[1][1] * m.m[2][3];	// -11,22,34
		result.m[2][3] -= m.m[0][1] * m.m[1][3] * m.m[2][0];	// -12,24,31
		result.m[2][3] -= m.m[0][3] * m.m[1][0] * m.m[2][1];	// -14,21,32
		result.m[2][3] += m.m[0][3] * m.m[1][1] * m.m[2][0];	// +14,22,31
		result.m[2][3] += m.m[0][1] * m.m[1][0] * m.m[2][3];	// +12,21,34
		result.m[2][3] += m.m[0][0] * m.m[1][3] * m.m[2][1];	// +11,24,32


		// 4行目
		result.m[3][0] = 0.0f;							// 41
		result.m[3][0] -= m.m[1][0] * m.m[2][1] * m.m[3][2];	// -21,32,43
		result.m[3][0] -= m.m[1][1] * m.m[2][2] * m.m[3][0];	// -22,33,41
		result.m[3][0] -= m.m[1][2] * m.m[2][0] * m.m[3][1];	// -23,31,42
		result.m[3][0] += m.m[1][2] * m.m[2][1] * m.m[3][0];	// +23,32,41
		result.m[3][0] += m.m[1][1] * m.m[2][0] * m.m[3][2];	// +22,31,43
		result.m[3][0] += m.m[1][0] * m.m[2][2] * m.m[3][1];	// +21,33,42

		result.m[3][1] = 0.0f;							// 42
		result.m[3][1] += m.m[0][0] * m.m[2][1] * m.m[3][2];	// +11,32,43
		result.m[3][1] += m.m[0][1] * m.m[2][2] * m.m[3][0];	// +12,33,41
		result.m[3][1] += m.m[0][2] * m.m[2][0] * m.m[3][1];	// +13,31,42
		result.m[3][1] -= m.m[0][2] * m.m[2][1] * m.m[3][0];	// -13,32,41
		result.m[3][1] -= m.m[0][1] * m.m[2][0] * m.m[3][2];	// -12,31,43
		result.m[3][1] -= m.m[0][0] * m.m[2][2] * m.m[3][1];	// -11,33,42

		result.m[3][2] = 0.0f;							// 43
		result.m[3][2] -= m.m[0][0] * m.m[1][1] * m.m[3][2];	// -11,22,43
		result.m[3][2] -= m.m[0][1] * m.m[1][2] * m.m[3][0];	// -12,23,41
		result.m[3][2] -= m.m[0][2] * m.m[1][0] * m.m[3][1];	// -13,21,42
		result.m[3][2] += m.m[0][2] * m.m[1][1] * m.m[3][0];	// +13,22,41
		result.m[3][2] += m.m[0][1] * m.m[1][0] * m.m[3][2];	// +12,21,43
		result.m[3][2] += m.m[0][0] * m.m[1][2] * m.m[3][1];	// +11,23,42

		result.m[3][3] = 0.0f;							// 44
		result.m[3][3] += m.m[0][0] * m.m[1][1] * m.m[2][2];	// +11,22,33
		result.m[3][3] += m.m[0][1] * m.m[1][2] * m.m[2][0];	// +12,23,31
		result.m[3][3] += m.m[0][2] * m.m[1][0] * m.m[2][1];	// +13,21,32
		result.m[3][3] -= m.m[0][2] * m.m[1][1] * m.m[2][0];	// -13,22,31
		result.m[3][3] -= m.m[0][1] * m.m[1][0] * m.m[2][2];	// -12,21,33
		result.m[3][3] -= m.m[0][0] * m.m[1][2] * m.m[2][1];	// -11,23,32

		return result;
	}
}

#pragma once
#ifndef MATHUTILS_H_
#define MATHUTILS_H_

namespace Math {
	constexpr float Pi = 3.141592653589793f;
	constexpr float TwoPi = Pi * 2.0f;
	constexpr float HalfPi = Pi * 0.5f;

	inline float ToRad(float deg);
	inline float ToDeg(float rad);

	struct Vector2;
	struct Vector3;
	struct Vector4;
	struct Quaternion;
	struct Matrix44;

#pragma region Vector2
	// 二次元ベクトル
	struct Vector2 {
		static const Vector2 UnitX;
		static const Vector2 UnitY;
		static const Vector2 Zero;

		inline Vector2();
		inline Vector2(float x, float y);
		inline explicit Vector2(float xy);

		float x, y;
	};

	inline Vector2 operator+(const Vector2& v);
	inline Vector2 operator-(const Vector2& v);
	inline Vector2 operator+(const Vector2& v1, const Vector2& v2);
	inline Vector2 operator-(const Vector2& v1, const Vector2& v2);
	inline Vector2 operator*(const Vector2& v, float s);
	inline Vector2 operator*(float s, const Vector2& v);
	inline Vector2& operator+=(Vector2& v1, const Vector2& v2);
	inline Vector2& operator-=(Vector2& v1, const Vector2& v2);
	inline Vector2& operator*=(Vector2& v, float s);
	inline bool operator==(const Vector2& v1, const Vector2& v2);
	inline bool operator!=(const Vector2& v1, const Vector2& v2);
	inline float Dot(const Vector2& v1, const Vector2& v2);
	inline float Cross(const Vector2& v1, const Vector2& v2);
	inline float LengthSquare(const Vector2& v);
	inline float Length(const Vector2& v);
	inline Vector2 Normalize(const Vector2& v);

	inline Vector2 Lerp(float t, const Vector2& start, const Vector2& end);
	inline Vector2 Project(const Vector2& v1, const Vector2& v2);
	inline Vector2 Mid(const Vector2& v1, const Vector2& v2);
	inline Vector2 Reflected(const Vector2& v, const Vector2& n);

	Vector2 Slerp(float t, const Vector2& s, const Vector2& e);
	Vector2 CatmullRomSpline(float t, const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3);
	Vector2 QuadraticBezierCurve(float t, const Vector2& p0, const Vector2& p1, const Vector2& p2);
	Vector2 CubicBezierCurve(float t, const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3);
#pragma endregion
#pragma region Vector3
	// 三次元ベクトル
	struct Vector3 {
		static const Vector3 UnitX;
		static const Vector3 UnitY;
		static const Vector3 UnitZ;
		static const Vector3 Zero;

		inline Vector3();
		inline Vector3(float x, float y, float z);
		inline explicit Vector3(float xyz);
		inline explicit Vector3(const Vector2& xy, float z);
		inline Vector3& xy(const Vector2& xy);
		inline Vector2 xy() const;
		inline Vector3& xz(const Vector2& xz);
		inline Vector2 xz() const;
		inline Vector3& yz(const Vector2& yz);
		inline Vector2 yz() const;

		float x, y, z;
	};

	inline Vector3 operator+(const Vector3& v);
	inline Vector3 operator-(const Vector3& v);
	inline Vector3 operator+(const Vector3& v1, const Vector3& v2);
	inline Vector3 operator-(const Vector3& v1, const Vector3& v2);
	inline Vector3 operator*(const Vector3& v, float s);
	inline Vector3 operator*(float s, const Vector3& v);
	inline Vector3& operator+=(Vector3& v1, const Vector3& v2);
	inline Vector3& operator-=(Vector3& v1, const Vector3& v2);
	inline Vector3& operator*=(Vector3& v, float s);
	inline bool operator==(const Vector3& v1, const Vector3& v2);
	inline bool operator!=(const Vector3& v1, const Vector3& v2);
	inline float Dot(const Vector3& v1, const Vector3& v2);
	inline Vector3 Cross(const Vector3& v1, const Vector3& v2);
	inline float LengthSquare(const Vector3& v);
	inline float Length(const Vector3& v);
	inline Vector3 Normalize(const Vector3& v);
	inline Vector3 Lerp(float t, const Vector3& start, const Vector3& end);
	inline Vector3 Project(const Vector3& v1, const Vector3& v2);
	inline Vector3 Mid(const Vector3& v1, const Vector3& v2);
	inline Vector3 Reflected(const Vector3& v, const Vector3& n);
	Vector3 Slerp(float t, const Vector3& s, const Vector3& e);
	Vector3 CatmullRomSpline(float t, const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3);
	Vector3 QuadraticBezierCurve(float t, const Vector3& p0, const Vector3& p1, const Vector3& p2);
	Vector3 CubicBezierCurve(float t, const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3);
#pragma endregion
#pragma region Vector4
	// 四次元ベクトル
	struct Vector4 {
		static const Vector4 Zero;

		inline Vector4();
		inline Vector4(float x, float y, float z, float w);
		inline explicit Vector4(float xyzw);
		inline explicit Vector4(const Vector3& xyz, float w);
		inline Vector4& xyz(const Vector3& xyz);
		inline Vector3 xyz() const;

		float x, y, z, w;
	};

	inline Vector4 operator+(const Vector4& v);
	inline Vector4 operator-(const Vector4& v);
	inline Vector4 operator+(const Vector4& v1, const Vector4& v2);
	inline Vector4 operator-(const Vector4& v1, const Vector4& v2);
	inline Vector4 operator*(const Vector4& v, float s);
	inline Vector4 operator*(float s, const Vector4& v);
	inline Vector4& operator+=(Vector4& v1, const Vector4& v2);
	inline Vector4& operator-=(Vector4& v1, const Vector4& v2);
	inline Vector4& operator*=(Vector4& v, float s);
	inline bool operator==(const Vector4& v1, const Vector4& v2);
	inline bool operator!=(const Vector4& v1, const Vector4& v2);
	inline float Dot(const Vector4& v1, const Vector4& v2);
	inline float LengthSquare(const Vector4& v);
	inline float Length(const Vector4& v);
	inline Vector4 Normalize(const Vector4& v);
	inline Vector4 Lerp(float t, const Vector4& start, const Vector4& end);
	inline Vector4 Project(const Vector4& v1, const Vector4& v2);
	inline Vector4 Mid(const Vector4& v1, const Vector4& v2);
#pragma endregion
#pragma region Quaternion
	// 四元数
	struct Quaternion : public Vector4 {
	public:
		static const Quaternion Identity;

		inline Quaternion();
		inline explicit Quaternion(float x, float y, float z, float w);
		/// <summary>
		/// 軸周りの角度を取得
		/// </summary>
		/// <returns></returns>
		inline float GetAngle() const;
		/// <summary>
		/// 回転軸を取得
		/// </summary>
		/// <returns></returns>
		inline Vector3 GetAxis() const;
		/// <summary>
		/// オイラー角を取得
		/// </summary>
		/// <returns></returns>
		inline Vector3 GetEuler() const;
		/// <summary>
		/// 回転角と回転軸からクォータニオンを作成
		/// </summary>
		/// <param name="angle"></param>
		/// <param name="axis"></param>
		/// <returns></returns>
		static inline Quaternion MakeFromAngleAxis(float angle, const Vector3& axis);
		/// <summary>
		/// オイラー角からクォータニオンを作成
		/// </summary>
		/// /// <param name="euler"></param>
		/// <returns></returns>
		static inline Quaternion MakeFromEulerAngle(const Vector3& euler);
		/// <summary>
		/// X軸を回転軸にクォータニオンを作成
		/// </summary>
		/// <param name="angle"></param>
		/// <returns></returns>
		static inline Quaternion MakeForXAxis(float angle);
		/// <summary>
		/// Y軸を回転軸にクォータニオンを作成
		/// </summary>
		/// <param name="angle"></param>
		/// <returns></returns>
		static inline Quaternion MakeForYAxis(float angle);
		/// <summary>
		/// Z軸を回転軸にクォータニオンを作成
		/// </summary>
		/// <param name="angle"></param>
		/// <returns></returns>
		static inline Quaternion MakeForZAxis(float angle);
		/// <summary>
		/// 二つのベクトルからクォータニオンを作成
		/// </summary>
		/// <param name="v1"></param>
		/// <param name="v2"></param>
		/// <returns></returns>
		static inline Quaternion MakeFromTwoVector(const Vector3& v1, const Vector3& v2);
		/// <summary>
		/// 正規直交軸からクォータニオンを作成
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <param name="z"></param>
		/// <returns></returns>
		static inline Quaternion MakeFromOrthonormal(const Vector3& x, const Vector3& y, const Vector3& z);
		/// <summary>
		/// Directionの方向を向くクォータニオンを作成
		/// </summary>
		/// <param name="direction">方向</param>
		/// <param name="up">上ベクトル</param>
		/// <returns></returns>
		static inline Quaternion MakeLookRotation(const Vector3& direction, const Vector3& up = Vector3::UnitY);
		/// <summary>
		/// 正規直交行列からクォータニオンを作成
		/// </summary>
		/// <param name="m">正規直交行列</param>
		/// <returns></returns>
		static inline Quaternion MakeFromMatrix(const Matrix44& m);
	};

	inline Quaternion operator+(const Quaternion& q1, const Quaternion& q2);
	inline Quaternion operator*(float s, const Quaternion& q);
	inline Quaternion operator*(const Quaternion& q, float s);
	inline Quaternion operator*(const Quaternion& q1, const Quaternion& q2);
	inline Vector3 operator*(const Quaternion& q, const Vector3& v);
	inline Quaternion Conjugate(const Quaternion& q);
	inline Quaternion Inverse(const Quaternion& q);
	inline Quaternion Slerp(float t, const Quaternion& start, const Quaternion& end);
#pragma endregion
#pragma region Matrix44
	// 4行4列行列
	struct Matrix44 {
		static const Matrix44 Identity;

		inline Matrix44();
		inline Matrix44(
			float _00, float _01, float _02, float _03,
			float _10, float _11, float _12, float _13,
			float _20, float _21, float _22, float _23,
			float _30, float _31, float _32, float _33);

		inline Vector3 GetXAxis() const;
		inline Vector3 GetYAxis() const;
		inline Vector3 GetZAxis() const;
		inline Vector3 GetTranslate() const;

		/// <summary>
		/// 拡大縮小行列を作成
		/// </summary>
		/// <param name="scale"></param>
		/// <returns></returns>
		static inline Matrix44 MakeScaling(const Vector3& scale);
		/// <summary>
		/// x軸回りの回転行列を作成
		/// </summary>
		/// <param name="rotate"></param>
		/// <returns></returns>
		static inline Matrix44 MakeRotationX(float rotate);
		/// <summary>
		/// y軸回りの回転行列を作成
		/// </summary>
		/// <param name="rotate"></param>
		/// <returns></returns>
		static inline Matrix44 MakeRotationY(float rotate);
		/// <summary>
		/// z軸回りの回転行列を作成
		/// </summary>
		/// <param name="rotate"></param>
		/// <returns></returns>
		static inline Matrix44 MakeRotationZ(float rotate);
		/// <summary>
		/// 回転行列を作成
		/// </summary>
		/// <param name="rotate"></param>
		/// <returns></returns>
		static inline Matrix44 MakeRotationXYZ(const Vector3& rotate);
		/// <summary>
		/// クォータニオン回転行列を作成
		/// </summary>
		/// <param name="q"></param>
		/// <returns></returns>
		static inline Matrix44 MakeRotationFromQuaternion(const Quaternion& q);
		/// <summary>
		/// 平行移動行列
		/// </summary>
		/// <param name="translate"></param>
		/// <returns></returns>
		static inline Matrix44 MakeTranslation(const Vector3& translate);
		/// <summary>
		/// アフィン変換行列を作成
		/// </summary>
		/// <param name="scale"></param>
		/// <param name="rotate">オイラー角</param>
		/// <param name="trans"></param>
		/// <returns></returns>
		static inline Matrix44 MakeAffine(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
		/// <summary>
		/// アフィン変換行列を作成
		/// </summary>
		/// <param name="scale"></param>
		/// <param name="rotate">クォータニオン</param>
		/// <param name="translate"></param>
		/// <returns></returns>
		static inline Matrix44 MakeAffine(const Vector3& scale, const Quaternion& rotate, const Vector3& translate);
		/// <summary>
		/// ルックアット行列を作成
		/// </summary>
		/// <param name="eye">座標</param>
		/// <param name="target">向ける座標</param>
		/// <param name="up">上ベクトル</param>
		/// <returns></returns>
		static inline Matrix44 MakeLookRotation(const Vector3& eye, const Vector3& target, const Vector3& up);
		/// <summary>
		/// 全方向ビルボード行列を作成
		/// </summary>
		/// <param name="eye">カメラの座標</param>
		/// <param name="target">注視点座標</param>
		/// <param name="up">上ベクトル</param>
		/// <returns></returns>
		static inline Matrix44 MakeBillBoard(const Vector3& eye, const Vector3& target, const Vector3& up);
		/// <summary>
		/// y軸ビルボード行列を作成
		/// </summary>
		/// <param name="eye">カメラの座標</param>
		/// <param name="target">注視点座標</param>
		/// <param name="up">上ベクトル</param>
		/// <returns></returns>
		static inline Matrix44 MakeBillBoardYAxis(const Vector3& eye, const Vector3& target, const Vector3& up);
		/// <summary>
		/// ビュー行列を作成
		/// </summary>
		/// <param name="eye">カメラの座標</param>
		/// <param name="target">注視点座標</param>
		/// <param name="up">上ベクトル</param>
		/// <returns></returns>
		static inline Matrix44 MakeView(const Vector3& eye, const Vector3& target, const Vector3& up);
		/// <summary>
		/// 透視投影行列を作成
		/// </summary>
		/// <param name="fovY">垂直画角</param>
		/// <param name="aspect">アスペクト比 幅 / 高</param>
		/// <param name="nearZ">クリッピング距離(近)</param>
		/// <param name="farZ">クリッピング距離(遠)</param>
		/// <returns></returns>
		static inline Matrix44 MakePerspectiveProjection(float fovY, float aspect, float nearZ, float farZ);
		/// <summary>
		/// 平行投影行列を作成
		/// </summary>
		/// <param name="left">左</param>
		/// <param name="top">上</param>
		/// <param name="right">右</param>
		/// <param name="bottom">下</param>
		/// <param name="nearZ">クリッピング距離(近)</param>
		/// <param name="farZ">クリッピング距離(遠)</param>
		/// <returns></returns>
		static inline Matrix44 MakeOrthographicProjection(float width, float height, float nearZ, float farZ);
		/// <summary>
		/// ビューポート行列を作成
		/// </summary>
		/// <param name="left">左</param>
		/// <param name="top">上</param>
		/// <param name="width">幅</param>
		/// <param name="height">高</param>
		/// <param name="nearZ">クリッピング距離(近)</param>
		/// <param name="farZ">クリッピング距離(遠)</param>
		/// <returns></returns>
		static inline Matrix44 MakeViewport(float left, float top, float width, float height, float nearZ, float farZ);


		float m[4][4];
	};

	inline Matrix44 operator*(const Matrix44& mat1, const Matrix44& mat2);
	inline Matrix44& operator*=(Matrix44& mat1, const Matrix44& mat2);
	inline Vector3 operator*(const Vector3& vec, const Matrix44& mat);
	inline Vector4 operator*(const Vector4& vec, const Matrix44& mat);
	inline Matrix44 operator*(float s, const Matrix44& mat);
	inline Matrix44 operator*(const Matrix44& mat, float s);
	/// <summary>
	/// 行列式
	/// </summary>
	/// <returns></returns>
	float Determinant(const Matrix44& m);
	/// <summary>
	/// 余因子行列
	/// </summary>
	/// <returns></returns>
	Matrix44 Adjugate(const Matrix44& m);
	/// <summary>
	/// 逆行列(処理多)
	/// </summary>
	/// <returns></returns>
	inline Matrix44 Inverse(const Matrix44& m);
	/// <summary>
	/// 転置行列
	/// </summary>
	/// <returns></returns>
	inline Matrix44 Transpose(const Matrix44& m);

#pragma endregion
}

#include "MathUtils_inline.h"

#endif
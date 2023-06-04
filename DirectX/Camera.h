#pragma once
#include "MathUtils.h"

class Camera {
public:
	void UpdateMatrix();

	void SetPosition(const Vector3& position) { position_ = position; }
	void SetRotate(const Quaternion& rotate) { rotate_ = rotate; }
	void SetProjectionParameter(float fovY, float aspect, float nearZ, float farZ) { fovY_ = fovY, aspect_ = aspect, nearZ_ = nearZ, farZ_ = farZ; }
	void SetViewMatrix(const Matrix4x4& viewMatrix) { viewMatrix_ = viewMatrix; }
	void SetProjectionMatrix(const Matrix4x4& projectionMatrix) { projectionMatrix_ = projectionMatrix; }

	const Matrix4x4& GetViewMatrix() const { return viewMatrix_; }
	const Matrix4x4& GetProjectionMatrix() const { return projectionMatrix_; }
	const Matrix4x4& GetViewProjectionMatrix() const { return viewProjectionMatrix_; }

private:
	Vector3 position_ = { 0.0f, 1.5f, -5.0f };
	Quaternion rotate_ = Quaternion::MakeLookRotation(-position_);

	float fovY_{ 45.0f * Math::ToRadian };
	float aspect_{ 1280.0f / 720.0f };
	float nearZ_{ 0.1f };
	float farZ_{ 1000.0f };

	Matrix4x4 viewMatrix_;
	Matrix4x4 projectionMatrix_;
	Matrix4x4 viewProjectionMatrix_;
};


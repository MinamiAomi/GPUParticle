#pragma once
#include "MathUtils.h"

class Camera {
public:
	void UpdateMatrix();

	void SetPosition(const Math::Vector3& position) { position_ = position; }
	void SetRotate(const Math::Quaternion& rotate) { rotate_ = rotate; }
	void SetProjectionParameter(float fovY, float aspect, float nearZ, float farZ) { fovY_ = fovY, aspect_ = aspect, nearZ_ = nearZ, farZ_ = farZ; }
	void SetViewMatrix(const Math::Matrix44& viewMatrix) { viewMatrix_ = viewMatrix; }
	void SetProjectionMatrix(const Math::Matrix44& projectionMatrix) { projectionMatrix_ = projectionMatrix; }

	const Math::Matrix44& GetViewMatrix() const { return viewMatrix_; }
	const Math::Matrix44& GetProjectionMatrix() const { return projectionMatrix_; }
	const Math::Matrix44& GetViewProjectionMatrix() const { return viewProjectionMatrix_; }

private:
	Math::Vector3 position_ = { 0.0f, 1.5f, -5.0f };
	Math::Quaternion rotate_ = Math::Quaternion::MakeLookRotation(-position_);

	float fovY_{ Math::ToRad(45.0f) };
	float aspect_{ 1280.0f / 720.0f };
	float nearZ_{ 0.1f };
	float farZ_{ 1000.0f };

	Math::Matrix44 viewMatrix_;
	Math::Matrix44 projectionMatrix_;
	Math::Matrix44 viewProjectionMatrix_;
};


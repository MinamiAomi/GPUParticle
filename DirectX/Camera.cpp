#include "stdafx.h"
#include "Camera.h"

void Camera::UpdateMatrix() {
	viewMatrix_ = Matrix4x4::MakeAffine({ 1.0f,1.0f,1.0f }, rotate_, position_).inverse;
	projectionMatrix_ = Matrix4x4::MakePerspectiveProjection(fovY_, aspect_, nearZ_, farZ_);
	viewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;
}

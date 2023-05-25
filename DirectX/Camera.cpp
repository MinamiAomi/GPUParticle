#include "stdafx.h"
#include "Camera.h"

void Camera::UpdateMatrix() {
	viewMatrix_ = Math::Inverse(Math::Matrix44::MakeAffine({ 1.0f,1.0f,1.0f }, rotate_, position_));
	projectionMatrix_ = Math::Matrix44::MakePerspectiveProjection(fovY_, aspect_, nearZ_, farZ_);
	viewProjectionMatrix_ = viewMatrix_ * projectionMatrix_;
}

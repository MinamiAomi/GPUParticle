#pragma once
#ifdef HLSL
typedef float32_t2 Vector2;
typedef float32_t3 Vector3;
typedef float32_t4 Vector4;
typedef float32_t4x4 Matrix44;
#else
#include "MathUtils.h"
using Vector2 = Math::Vector2;
using Vector3 = Math::Vector3;
using Vector4 = Math::Vector4;
using Matrix44 = Math::Matrix44;
#endif

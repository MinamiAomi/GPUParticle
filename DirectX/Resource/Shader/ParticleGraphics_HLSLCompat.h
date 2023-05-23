#pragma once
#include "HLSLCompat.h"

namespace ParticleShader {
	struct Scene {
		Matrix44 viewMatrix;
		Matrix44 projectionMatrix;
	};
}
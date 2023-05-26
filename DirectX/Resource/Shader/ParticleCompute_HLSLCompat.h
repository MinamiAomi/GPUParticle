#pragma once
#include "HLSLCompat.h"

namespace ParticleShader {
	struct Particle {
		Vector4 position;
		Vector3 velocity;
		Vector3 acceleration;
	};

	struct Target {
		Vector3 position;
	};
}
#pragma once
#include "Engine/Core/Common/Header.h"
#include "Engine/Core/Common.h"

#include <vector>

struct alignas(16) Particle {
    glm::vec3 position;
    float pad1;

    glm::vec3 velocity;
    float pad2;

    float lifetime;
    float pad3[3];
};

struct ParticleGenerator {
};

namespace ParticleSystem {
	extern GLuint particleBuffers[2];
	void init();
	void Simulate(float dt);
	void RenderParticles();
}
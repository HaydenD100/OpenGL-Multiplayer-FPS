#include "Particle.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Core/Camera.h"

namespace ParticleSystem {
	GLuint particleBuffers[2];
	GLuint quadVBO;
	unsigned int frame = 0;


	void init() {
		float quadVerts[] = {
			-0.5f, -0.5f,
			 0.5f, -0.5f,
			-0.5f,  0.5f,
			 0.5f,  0.5f
		};

		std::vector<Particle> particles;

		for (int i = 0; i < MAX_PARTICLES; i++) {
			Particle p;
			p.position = glm::vec3(0.0f);
			p.velocity = glm::vec3(float(rand()) / RAND_MAX - 0.5f,float(rand()) / RAND_MAX,float(rand()) / RAND_MAX - 0.5f) * 2.0f;
			p.lifetime = 5.0f;
			particles.push_back(p);
		}

		glGenBuffers(1, &quadVBO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
		glGenBuffers(2, particleBuffers);

		for (int i = 0; i < 2; ++i) {
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleBuffers[i]);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Particle) * MAX_PARTICLES, particles.data(), GL_DYNAMIC_COPY);
		}
	}

	void Simulate(float dt) {
		frame++;
		int readIndex = frame % 2;
		int writeIndex = (frame + 1) % 2;

		Renderer::cs_sim_particle.Use();
		Renderer::cs_sim_particle.SetFloat("dt", dt);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleBuffers[readIndex]);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, particleBuffers[writeIndex]);
		//TODO figure out the amount of particles
		glDispatchCompute(MAX_PARTICLES,0,0);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}
	void RenderParticles() {
		Renderer::s_particle.Use();
		Renderer::s_particle.SetMat4("V", Camera::getViewMatrix());
		Renderer::s_particle.SetMat4("P", Camera::getProjectionMatrix());

		// Bind quad vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);

		int readIndex = frame % 2;

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleBuffers[readIndex]);
		// Draw
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, MAX_PARTICLES);
	}
}
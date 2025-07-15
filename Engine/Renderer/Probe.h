#pragma once
#include <vector>
#include <iostream>
#include "Engine/Core/Common/RenderCommon.h"
#include "Engine/Core/Common.h"
#include "Engine/Core/Lights/Light.h"
class Probe
{
public:
	Probe(glm::vec3 postion);
	void Bake();
	void Irradiance();
	void ReLight();
	void CreateBindless();
	Transform GetTransform();

	GLuint GetCubeAlbedo();
	GLuint GetCubeNormal();
	GLuint GetCubePosition();


	unsigned int ProbeID();

	//used for DDGI
	GLuint64 h_gAlbedo = 0;
	GLuint64 h_gNormal = 0;
	GLuint64 h_gPosition = 0;

private:
	Transform transform;
	GLuint probeAlbedo = 0;
	GLuint probeNormal = 0;
	GLuint probePosition = 0;

	GLuint gbufferFBO = 0;
	GLuint Depth = 0;

	GLuint probeTextureBuffer = 0;

	unsigned int probeID;
	static unsigned int probeCount;
	

	glm::mat4 captureProjection;
	glm::mat4 captureViews[6];


	std::vector<glm::vec3> rayDirs;
};


struct ProbeGrid {
	std::vector<Probe> probes;
	int doneConfigure = 0;
	int updatedIndex = 0;
	GLuint b_handles;
	GLuint b_probePosition;
	std::vector<GLuint64> handels;
	std::vector<glm::vec3> positions;

	//Generate all the probes on another thread while the assets are loading


	void Configure(float Width, float Height, float Depth, glm::vec3 spacing, glm::vec3 start) {
		// Pre-calculate inverse spacing to avoid repeated division
		const glm::vec3 invSpacing = 1.0f / spacing;

		// Convert dimensions to integer counts using proper rounding
		const int width = static_cast<int>(std::round(Width * invSpacing.x));
		const int height = static_cast<int>(std::round(Height * invSpacing.y));
		const int depth = static_cast<int>(std::round(Depth * invSpacing.z));

		volume = glm::vec3(width, height, depth);
		postion = start;
		this->spacing = spacing;

		// Pre-calculate total number of probes to avoid multiple calculations
		const size_t totalProbes = static_cast<size_t>(width) * height * depth;

		// Reserve memory upfront to prevent reallocations
		probes.reserve(totalProbes);
		positions.reserve(totalProbes);

		std::cout << "Starting configure \n";

		const glm::vec3 scaledStart = start * invSpacing;

		for (int z = 0; z < depth; ++z) { // Outermost loop: corresponds to gl_WorkGroupID.z (slowest changing)
			const float zPos = static_cast<float>(z) + scaledStart.z;
			for (int y = 0; y < height; ++y) { // Middle loop: corresponds to start_index (gl_WorkGroupID.y)
				const float yPos = static_cast<float>(y) + scaledStart.y;
				for (int x = 0; x < width; ++x) { // Innermost loop: corresponds to gl_WorkGroupID.x (fastest changing)
					const float xPos = static_cast<float>(x) + scaledStart.x;

					// Create position vector once and reuse
					const glm::vec3 pos(xPos, yPos, zPos);
					//std::cout << "Pos: " << (pos * spacing).x << " " << (pos * spacing).y << " " << (pos * spacing).z << "\n";
					probes.emplace_back(pos * spacing);
					positions.push_back(pos * spacing);
				}
			}
		}
		glNamedBufferStorage(
			b_probePosition,
			sizeof(glm::vec3) * positions.size(),
			positions.data(),
			GL_DYNAMIC_STORAGE_BIT
		);

		std::cout << "Done configure Probe Count: " << probes.size() << "\n";
		doneConfigure = 1;
	}
	void FillBuffer();

	void Bind(int index) {
		//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, textureBuffer);
	}
	void BindCubeMapArray(int index) {
		glBindImageTexture(index, CubeMapArrayBuffer, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
	}


	void AddProbe(glm::vec3 postion) {
		probes.push_back(Probe(postion));
	}

	void Bake();
	void ReLight(int probeRelightCount);
	void ShowProbes();


	glm::vec3 postion;
	glm::vec3 volume;
	glm::vec3 spacing;

	std::vector<glm::uvec2> probeTextureHandles;
	std::vector< glm::uvec2> probeDepthHandles;
	GLuint textureBuffer = 0;
	GLuint CubeMapArrayBuffer = 0;

};

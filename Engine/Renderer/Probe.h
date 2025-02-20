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
	Transform GetTransform();
	GLuint GetCubeAlbedo();
	GLuint GetCubeNormal();
	GLuint GetCubePosition();
	GLuint GetCubeLighting();


	GLuint GetDepthCubeMap();
	GLuint GetIrradianceCubeMap();

	unsigned int ProbeID();

	//used for DDGI
	GLuint64 probeCubemapBindlessHandle = 0;
	GLuint64 DepthBindlessHandle = 0;
private:
	Transform transform;
	GLuint probeLighting = 0;
	GLuint probeAlbedo = 0;
	GLuint probeNormal = 0;
	GLuint probePosition = 0;

	GLuint probeIrradianceCubemap = 0;
	GLuint probeFBO = 0;
	GLuint gbufferFBO = 0;
	GLuint Depth = 0;
	GLuint m_depth = 0;

	GLuint probeTextureBuffer = 0;


	GLuint probeCubemapBindless = 0;
	GLuint DepthBindless = 0;

	unsigned int probeID;
	static unsigned int probeCount;

	glm::mat4 captureProjection;
	glm::mat4 captureViews[6];


	std::vector<glm::vec3> rayDirs;
};


struct ProbeGrid {
	std::vector<Probe> probes;
	int doneConfigure = 0;
	//Generate all the probes on another thread while the assets are loading


	void Configure(float width, float height, float depth, float spacing, glm::vec3 start) {

		volume = glm::vec3(width, height, depth);
		postion = start;
		this->spacing = spacing;
		glGenTextures(1, &CubeMapArrayBuffer);
		glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, CubeMapArrayBuffer);
		glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 1, GL_RGBA16F, DDGIPROBESIZE, DDGIPROBESIZE, width/spacing * height/spacing * depth/spacing * 6);

		std::cout << "Starting configure \n";
		std::vector<GLubyte> blackData(width * height * 4, 0); // RGBA all zeros (black)

		for (float x = 0; x < width; x += spacing) {
			for (float y = 0; y < height; y += spacing) {
				for (float z = 0; z < depth; z += spacing) {
					probes.push_back(Probe(glm::vec3(x, y, z) + start));
					//redudant right now
					GLuint64 handle = probes[probes.size() - 1].probeCubemapBindlessHandle;
					probeTextureHandles.push_back(glm::uvec2(static_cast<uint32_t>(handle & 0xFFFFFFFF), static_cast<uint32_t>(handle >> 32)));

					glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, CubeMapArrayBuffer);
					for (int face = 0; face < 6; ++face) {
						glTexSubImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, 0, 0, probes[probes.size()-1].ProbeID() * 6 + face,
							width, height, 1, GL_RGBA16F, GL_UNSIGNED_BYTE, blackData.data());
					}
				}
			}
		}
		glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, CubeMapArrayBuffer);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);



		glCreateBuffers(1, &textureBuffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, probeTextureHandles.size() * sizeof(glm::uvec2), &probeTextureHandles[0], GL_STATIC_DRAW);
		std::cout << "Done configure \n";
		doneConfigure = 1;
	}

	void Bind(int index) {
		//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, textureBuffer);
	}
	void BindCubeMapArray(int index) {
		glBindImageTexture(index, CubeMapArrayBuffer, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA16F);
	}


	void AddProbe(glm::vec3 postion) {
		probes.push_back(Probe(postion));
	}

	void Bake(std::vector<Light> lights);
	void ShowProbes();


	glm::vec3 postion;
	glm::vec3 volume;
	float spacing;

	std::vector<glm::uvec2> probeTextureHandles;
	std::vector< glm::uvec2> probeDepthHandles;
	GLuint textureBuffer = 0;
	GLuint CubeMapArrayBuffer = 0;

};

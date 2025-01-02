#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Engine/Core/Common.h"
#include "Engine/Core/Lights/Light.h"
#include <vector>
const int PROBESIZE = 64;

class Probe
{
public:
	Probe(glm::vec3 postion);
	void Bake();
	void Irradiance();
	Transform GetTransform();
	GLuint GetCubeMap();
	GLuint GetDepthCubeMap();
	GLuint GetIrradianceCubeMap();

	unsigned int ProbeID();


private:
	Transform transform;
	GLuint probeCubemap = 0;
	GLuint probeIrradianceCubemap = 0;
	GLuint probeFBO = 0;
	GLuint Depth = 0;

	unsigned int probeID;
	static unsigned int probeCount;

	glm::mat4 captureProjection;
	glm::mat4 captureViews[6];
};


struct ProbeGrid {
	std::vector<Probe> probes;

	void Configure(float width, float height, float depth, float spacing, glm::vec3 start) {

		volume = glm::vec3(width, height, depth);
		postion = start;
		this->spacing = spacing;
		std::cout << "Starting configure \n";

		for (float x = 0; x < width; x += spacing) {
			for (float y = 0; y < height; y += spacing) {
				for (float z = 0; z < depth; z += spacing) {
					probes.push_back(Probe(glm::vec3(x, y, z) + start));
				}
			}
		}
		std::cout << "Done configure \n";
	}

	void AddProbe(glm::vec3 postion) {
		probes.push_back(Probe(postion));
	}

	void Bake(std::vector<Light> lights);
	void ShowProbes();


	glm::vec3 postion;
	glm::vec3 volume;
	float spacing;


};

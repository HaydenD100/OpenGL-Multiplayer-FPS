#pragma once
#include "Engine/Core/Common.h"
#include "glm/gtx/intersect.hpp"


namespace Raycaster
{
	void Init();
	void CleanUp();
	void FillBuffers();
	void Bind();
	void Compute();
	void queueRay(glm::vec3 rayDirection, glm::vec3 rayOrigin, float lenght, int probeID);
	int GetIndicesSize();
};

//having alot of issues figuring out raycasting using the GPU Compute so im going to write a CPU raycaster to figure out so stuff
namespace SoftwareRaycaster
{
	void Init();
	void Compute();
	void FillBuffers();
	void queueRay(glm::vec3 rayDirection, glm::vec3 rayOrigin, float lenght);
	float TriangleIntersectionTest(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int rayIndex);
	float TriangleIntersectionTest2(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int rayIndex);

	void RenderVerticies();


	

};

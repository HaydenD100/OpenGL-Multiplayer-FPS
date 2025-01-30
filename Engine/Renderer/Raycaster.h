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
	void queueRay(glm::vec3 rayOrigin, glm::vec3 rayDirection, float length);
	int GetIndicesSize();
	void RenderVerticies();

};

//having alot of issues figuring out raycasting using the GPU Compute so im going to write a CPU raycaster to figure out so stuff
namespace SoftwareRaycaster
{
	void Init();
	void Compute();
	void FillBuffers();
	void queueRay(glm::vec3 rayOrigin, glm::vec3 rayDirection, float length);
	float TriangleIntersectionTest(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int rayIndex);
	float TriangleIntersectionTest2(const glm::vec3& orig, const glm::vec3& dir,
		const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
		glm::vec3& baryPosition);

	void RenderVerticies();


	

};

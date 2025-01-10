#pragma once
#include "Engine/Core/Common.h"


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


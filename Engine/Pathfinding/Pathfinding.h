#pragma once
#include "Engine/Core/Common.h"
#include "Engine/Core/GameObject.h"


struct AiAgent {
	GameObject* gameobject_ptr = nullptr;
	glm::vec3 targetPosition = glm::vec3(0);
};

namespace PathFinding {
	void Init();

}
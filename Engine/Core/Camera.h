#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Input.h"
#include "Physics/Physics.h"

namespace Camera
{
	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();

	glm::vec3 GetPosition();
	void SetHorizontalAngle(float angle);
	void SetVerticalAngle(float angle);
	void SetPosition(glm::vec3 pos);
	glm::vec3 GetDirection();
	void Update(float dt);

	glm::vec3 ComputeRay();
	btCollisionWorld::ClosestRayResultCallback GetRayHit();
}

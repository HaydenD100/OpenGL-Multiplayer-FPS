#pragma once
#include "Engine/Core/Common/RenderCommon.h"
#include "Common.h"



namespace Camera
{
	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();
	void RecalcuteProjectionMatrix();

	glm::vec3 GetPosition();
	void SetHorizontalAngle(float angle);
	void SetVerticalAngle(float angle);
	void SetPosition(glm::vec3 pos);
	glm::vec3 GetDirection();
	glm::vec3 GetRight();
	glm::vec3 GetUp();
	Frustum GetFrustum();

	float GetHorizontalAngle();
	float GetVerticalAngle();

	

	void Update(float dt);

	glm::vec3 GetRotation();

	glm::vec3 ComputeRay();
	btCollisionWorld::ClosestRayResultCallback GetRayHit(float Maxoffset = 0);
	glm::vec3 GetMouseRay(glm::mat4 projection, glm::mat4 view, int windowWidth, int windowHeight, int mouseX, int mouseY);

}

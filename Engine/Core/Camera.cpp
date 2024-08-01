#include "Camera.h"
#include "AssetManager.h"
#include "Engine/Core/Common.h"
#include "Engine/Physics/BulletPhysics.h"



namespace Camera {

	glm::vec3 position = glm::vec3(0, 0, 5);
	glm::vec3 direction = glm::vec3(0, 0, 0);

	// horizontal angle : toward -Z
	float horizontalAngle = 3.14f;
	float verticalAngle = 0.0f;
	float initialFoV = 45.0f;
	float maxAngle = 1.5;
	float mouseSpeed = 0.005f;


	
	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;

	glm::vec3 Camera::GetPosition() {
		return position;
	}
	glm::mat4 Camera::getViewMatrix() {
		return ViewMatrix;
	}
	glm::mat4 Camera::getProjectionMatrix() {
		return ProjectionMatrix;
	}
	void Camera::SetHorizontalAngle(float angle) {
		horizontalAngle = angle;
	}
	void Camera::SetVerticalAngle(float angle) {
		verticalAngle = angle;
	}
	void Camera::SetPosition(glm::vec3 pos) {
		position = pos;
	}
	glm::vec3 Camera::ComputeRay() {
		glm::vec4 lRayStart_NDC(
			((float)Input::GetMouseX() / (float)SCREENWIDTH - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
			((float)Input::GetMouseY() / (float)SCREENHEIGHT - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
			-1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
			1.0f
		);
		glm::vec4 lRayEnd_NDC(
			((float)Input::GetMouseX() / (float)SCREENWIDTH - 0.5f) * 2.0f,
			((float)Input::GetMouseY() / (float)SCREENHEIGHT - 0.5f) * 2.0f,
			0.0,
			1.0f
		);

		glm::mat4 InverseProjectionMatrix = glm::inverse(ProjectionMatrix);

		// The View Matrix goes from World Space to Camera Space.
		// So inverse(ViewMatrix) goes from Camera Space to World Space.
		glm::mat4 InverseViewMatrix = glm::inverse(ViewMatrix);

		glm::vec4 lRayStart_camera = InverseProjectionMatrix * lRayStart_NDC;    lRayStart_camera /= lRayStart_camera.w;
		glm::vec4 lRayStart_world = InverseViewMatrix * lRayStart_camera; lRayStart_world /= lRayStart_world.w;
		glm::vec4 lRayEnd_camera = InverseProjectionMatrix * lRayEnd_NDC;      lRayEnd_camera /= lRayEnd_camera.w;
		glm::vec4 lRayEnd_world = InverseViewMatrix * lRayEnd_camera;   lRayEnd_world /= lRayEnd_world.w;

		glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
		

		return glm::normalize(lRayDir_world);
	}
	btCollisionWorld::ClosestRayResultCallback Camera::GetRayHit() {
		glm::vec3 out_end = Camera::position + ComputeRay() * 1000.0f;

		btCollisionWorld::ClosestRayResultCallback RayCallback(
			btVector3(Camera::position.x, Camera::position.y, Camera::position.z),
			btVector3(out_end.x, out_end.y, out_end.z)
		);
		RayCallback.m_collisionFilterGroup = GROUP_PLAYER;
		RayCallback.m_collisionFilterMask = GROUP_STATIC | GROUP_DYNAMIC;
		PhysicsManagerBullet::GetDynamicWorld()->rayTest(btVector3(Camera::position.x, Camera::position.y, Camera::position.z),btVector3(out_end.x, out_end.y, out_end.z),RayCallback);
		return RayCallback;
	}
	

	void Camera::Update(float dt) {

		if (verticalAngle <= maxAngle && verticalAngle >= -maxAngle)
			verticalAngle += mouseSpeed * float(768 / 2 - Input::GetMouseY());

		else if (verticalAngle > maxAngle)
			verticalAngle = maxAngle;

		else if (verticalAngle < -maxAngle)
			verticalAngle = -maxAngle;

		// Direction : Spherical coordinates to Cartesian coordinates conversion
		direction = glm::vec3(
			cos(verticalAngle) * sin(horizontalAngle),
			sin(verticalAngle),
			cos(verticalAngle) * cos(horizontalAngle)
		);
		// Right vector
		glm::vec3 right = glm::vec3(
			sin(horizontalAngle - 3.14f / 2.0f),
			0,
			cos(horizontalAngle - 3.14f / 2.0f)
		);
		// Up vector
		glm::vec3 up = glm::cross(right, direction);

		// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
		ProjectionMatrix = glm::perspective(initialFoV, 4.0f / 3.0f, 0.1f, 100.0f);
		// Camera matrix
		ViewMatrix = glm::lookAt(
			position,           // Camera is here
			position + direction, // and looks here : at the same position, plus "direction"
			up                  // Head is up (set to 0,-1,0 to look upside-down)
		);
	}
	glm::vec3 Camera::GetDirection() {
		return direction;
	}

}
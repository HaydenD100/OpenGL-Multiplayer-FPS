#pragma once
#include "bullet/btBulletDynamicsCommon.h"
#include "bullet/LinearMath/btVector3.h"
#include "bullet/LinearMath/btAlignedObjectArray.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include "Engine/Core/GameObject.h"

enum ColliderShape
{
	Box,
	Sphere,
	Cylinder,
	Capsule,
	Convex,
	Concave,
	None
};

enum CollisionGroups
{
	GROUP_PLAYER = 1,
	GROUP_STATIC = 2,
	GROUP_DYNAMIC = 4,
	GROUP_NONE = 8
	// Add more groups as needed
};

namespace PhysicsManagerBullet
{
	void Init();
	void Update(float deltaTime);
	void AddColliderShape(btCollisionShape* collider);
	void AddRigidBody(btRigidBody* body);
	btDiscreteDynamicsWorld* GetDynamicWorld();
}

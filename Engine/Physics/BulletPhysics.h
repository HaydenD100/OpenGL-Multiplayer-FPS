#pragma once
#include "bullet/btBulletDynamicsCommon.h"
#include "bullet/LinearMath/btVector3.h"
#include "bullet/LinearMath/btAlignedObjectArray.h"
#include "Engine/Core/Debug/BulletDebug.h"
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>

#include "Engine/Core/Common/Header.h"
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
	GROUP_NONE = 8,
	GROUP_TRIGGER = 16
	// Add more groups as needed
};

namespace PhysicsManagerBullet
{
	void Init();
	void Delete();
	void Update(float deltaTime);
	void AddColliderShape(btCollisionShape* collider);
	void AddRigidBody(btRigidBody* body);
	btDiscreteDynamicsWorld* GetDynamicWorld();
}

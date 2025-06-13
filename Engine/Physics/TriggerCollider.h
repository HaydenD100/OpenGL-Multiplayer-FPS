#pragma once
#include "Engine/Core/Common/Header.h"
#include "Engine/Physics/BulletPhysics.h"
#include <memory>
#include <iostream>

class TriggerCollider
{
public:
	TriggerCollider() = default;
	~TriggerCollider();
	TriggerCollider(glm::vec3 position, glm::vec3 scale);

	bool IsObjectInside(const btCollisionObject* target);

	glm::vec3 CheckOverlap(const btCollisionObject* playerObj) const;

	glm::vec3 m_pos = glm::vec3(0);
	glm::vec3 m_scale = glm::vec3(0);

private:
	std::unique_ptr<btBoxShape> m_triggerShape;
	std::unique_ptr<btGhostObject> m_trigger;
};


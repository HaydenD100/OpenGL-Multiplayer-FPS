#include "TriggerCollider.h"


TriggerCollider::~TriggerCollider() {

}
TriggerCollider::TriggerCollider(glm::vec3 position, glm::vec3 scale) {
    m_triggerShape = std::make_unique<btBoxShape>(btVector3(scale.x * 0.5f, scale.y * 0.5f, scale.z * 0.5f));

    m_pos = position;
    m_scale = scale;
    // Use btGhostObject instead of btCollisionObject
    m_trigger = std::make_unique<btGhostObject>();
    m_trigger->setCollisionShape(m_triggerShape.get());

    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(btVector3(position.x, position.y, position.z));
    m_trigger->setWorldTransform(transform);

    // Set collision groups
    short TRIGGER_GROUP = 1 << 10;
    short TRIGGER_MASK = GROUP_DYNAMIC | GROUP_PLAYER | GROUP_STATIC | GROUP_TRIGGER;

    PhysicsManagerBullet::GetDynamicWorld()->addCollisionObject(
        m_trigger.get(),
        TRIGGER_GROUP,
        TRIGGER_MASK
    );
}

bool TriggerCollider::IsObjectInside(const btCollisionObject* target) {
    for (int i = 0; i < m_trigger->getNumOverlappingObjects(); ++i) {
        if (m_trigger->getOverlappingObject(i) == target) {
            return true;
        }
    }
    return false;
}

glm::vec3 TriggerCollider::CheckOverlap(const btCollisionObject* playerObj) const {
    if (!m_trigger || !playerObj) {
        return glm::vec3(0.0f);
    }

    // Get AABBs for both objects
    btVector3 tMin, tMax;
    m_trigger->getCollisionShape()->getAabb(m_trigger->getWorldTransform(), tMin, tMax);

    btVector3 pMin, pMax;
    playerObj->getCollisionShape()->getAabb(playerObj->getWorldTransform(), pMin, pMax);

    // Calculate overlap amounts
    float overlapX = std::max(0.0f, std::min(tMax.x(), pMax.x()) - std::max(tMin.x(), pMin.x()));
    float overlapY = std::max(0.0f, std::min(tMax.y(), pMax.y()) - std::max(tMin.y(), pMin.y()));
    float overlapZ = std::max(0.0f, std::min(tMax.z(), pMax.z()) - std::max(tMin.z(), pMin.z()));

    // Return 0 if no overlap on any axis
    if (overlapX <= 0.0f && overlapY <= 0.0f && overlapZ <= 0.0f) {
        return glm::vec3(0.0f);
    }
    return glm::vec3(overlapX, overlapY, overlapZ);
}
#include "DecalInstance.h"
#include "Engine/Core/Decal.h"


DecalInstance::DecalInstance(glm::vec3 position, glm::vec3 normal, Decal* decal, GameObject* Parent) {
	this->decal = decal;
	this->normal = normal;
	parent = Parent;

	glm::vec3 up(0, 1, 0);
	glm::vec3 rotationAxis = glm::cross(normal, up);
	float angle = acos(glm::dot(normal, up));

	transform.position = position + (normal * 0.01f);
	transform.rotation = (rotationAxis * -angle);

	transform.scale = decal->GetSize();

	aabb = generateAABB();
}

bool DecalInstance::CheckParentIsNull() {
	if (parent == nullptr)
		return true;
	return false;
}

glm::mat4 DecalInstance::GetModel() {
	return parent->GetModelMatrix() * transform.to_mat4();
}

glm::vec3 DecalInstance::GetNormal() {
	return normal;
}

glm::vec3 DecalInstance::GetScale() {
	return transform.scale;
}
Transform DecalInstance::getTransform() {
	return transform;
}

AABB* DecalInstance::GetAABB() {
	return &aabb;
}

Decal* DecalInstance::GetDecal() {
	return decal;
}




AABB DecalInstance::generateAABB()
{
	glm::vec3 minAABB = glm::vec3(std::numeric_limits<float>::max());
	glm::vec3 maxAABB = glm::vec3(std::numeric_limits<float>::min());
	for (int i = 0; i < decal->getVerticesSize(); i++)
	{
		glm::vec3 vertexpos = decal->GetVertex(i);
		minAABB.x = std::min(minAABB.x, vertexpos.x);
		minAABB.y = std::min(minAABB.y, vertexpos.y);
		minAABB.z = std::min(minAABB.z, vertexpos.z);

		maxAABB.x = std::max(maxAABB.x, vertexpos.x);
		maxAABB.y = std::max(maxAABB.y, vertexpos.y);
		maxAABB.z = std::max(maxAABB.z, vertexpos.z);
	}
	return AABB(minAABB, maxAABB);
}
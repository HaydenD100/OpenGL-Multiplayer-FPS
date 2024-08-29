#include "Crate.h"
#include "Engine/Core/Common.h"

Crate::Crate(glm::vec3 position, std::string name, Model* model) {
	this->name = name;
	AssetManager::AddGameObject(name, model, position, false, 4, Box);
	AssetManager::GetGameObject(name)->GetRigidBody()->setCcdMotionThreshold(0.1);
	AssetManager::GetGameObject(name)->GetRigidBody()->setCcdSweptSphereRadius(0.2); // Set the radius for CCD
}

void Crate::Update() {
	if (health <= 0) {
		Break();
	}
}

std::string Crate::GetName() {
	return name;
}

void Crate::DealDamage(int damage,glm::vec3 position, glm::vec3 force) {
	health -= damage;
}

void Crate::Break() {
	AssetManager::RemoveGameObject(name);
}

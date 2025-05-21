#include "Crate.h"

#include "Engine/Core/Scene/SceneManager.h"


Crate::Crate(glm::vec3 position, std::string name, Model* model) {
	this->name = name;
	SceneManager::GetCurrentScene()->AddGameObject(name, model, position, false, 4, Box);
	SceneManager::GetCurrentScene()->GetGameObject(name)->GetRigidBody()->setCcdMotionThreshold(0.1);
	SceneManager::GetCurrentScene()->GetGameObject(name)->GetRigidBody()->setCcdSweptSphereRadius(0.2); // Set the radius for CCD
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
	SceneManager::GetCurrentScene()->RemoveGameObject(name);
}

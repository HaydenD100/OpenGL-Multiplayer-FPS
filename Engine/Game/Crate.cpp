#include "Crate.h"

#include "Engine/Core/Scene/World.h"

Crate::Crate(glm::vec3 position, std::string name, Model* model) {
	this->name = name;
	World::AddGameObject(name, model, position, false, 4, Box);
	World::GetGameObject(name)->GetRigidBody()->setCcdMotionThreshold(0.1);
	World::GetGameObject(name)->GetRigidBody()->setCcdSweptSphereRadius(0.2); // Set the radius for CCD
	
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
	World::RemoveGameObject(name);
}

#include "Door.h"

Door::Door(std::string Name, Model* door, Model* frame, glm::vec3 position) {
	name = Name;
	AssetManager::AddGameObject(GameObject(name + "_frame", frame, position, false, 0, Box, 0, 0, 0));
	AssetManager::AddGameObject(name + "_door", door, position, false, 0, Convex);
	GameObject* gameobject = AssetManager::GetGameObject(name + "_door");
	gameobject->GetRigidBody()->setCcdMotionThreshold(0.1);
	gameobject->GetRigidBody()->setCcdSweptSphereRadius(0.2); // Set the radius for CCD
	door_rotation = gameobject->getRotation();
	opened = false;
	opening = false;
	door_position = position;
}

void Door::Interact() {
	if (Player::GetInteractingWithName() == name + "_door" && opening == false) {
		opening = true;
		if (opened) {
			rotation = 1.5;
			AudioManager::PlaySound("door_close", door_position);
		}
		else {
			rotation = 0;
			AudioManager::PlaySound("door_open", door_position);
		}
	}
}

void Door::Update(float deltaTime) {
	if (!opening)
		return;
	if ((rotation >= 1.5f && !opened) || (rotation <= 0 && opened)) {
		opening = false;
		opened = !opened;
	}
		
	if (!opened)
		rotation += openingSpeed;
	else
		rotation -= openingSpeed;
	AssetManager::GetGameObject(name + "_door")->SetRotationY(door_rotation.y + rotation);
}

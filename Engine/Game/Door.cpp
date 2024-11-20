#include "Door.h"
#include "Engine/Animation/Animation.h"

Door::Door(std::string Name, Model* door, Model* frame, glm::vec3 position, glm::vec3 GameObjectRotation, bool inWards) {
	name = Name;
	int doorFrameIndex = AssetManager::AddGameObject(GameObject(name + "_frame", frame, position, false, 0, Concave)) ;
	int doorIndex = AssetManager::AddGameObject(name + "_door", door, position, false, 0, Concave);
	GameObject* gameobject = AssetManager::GetGameObject(name + "_door");
	GameObject* gameobjectFrame = AssetManager::GetGameObject(name + "_frame");

	gameobject->setRotation(GameObjectRotation);
	gameobjectFrame->setRotation(GameObjectRotation);
	gameobject->GetRigidBody()->setCcdMotionThreshold(0.1);
	gameobject->GetRigidBody()->setCcdSweptSphereRadius(0.2); // Set the radius for CCD

	door_rotation = GameObjectRotation;
	door_position = position;
	direction = inWards ? 1.0 : -1.0;
	rotation = 0; 

	opened = false;
	opening = false;
}

void Door::Open() {
	if (opening == false) {
		opening = true;
		if (!opened) 
			AudioManager::PlaySound("door_open", door_position);
		else 
			AudioManager::PlaySound("door_close", door_position);
	}
}

void Door::Interact() {
	if ((Player::GetInteractingWithName() == name + "_door"  || PlayerTwo::GetInteractingWithName() == name + "_door")  && opening == false) {
		opening = true;
		if (!opened) {
			//AnimationManager::Play("door_open", name + "_door");
			AudioManager::PlaySound("door_open", door_position);
			//opened = true;
		}
			
		else {
			//AnimationManager::Play("door_close", name + "_door");	
			AudioManager::PlaySound("door_close", door_position);
			//opened = false;
		}
		
	}
}

void Door::Update(float deltaTime) {
	if (!opening)
		return;
	//door is opening
	if (!opened) {
		rotation += openingSpeed;
	}
	//door is closing
	if (opened) {
		rotation -= openingSpeed;
	}
	
	if (rotation >= maxRotation) {
		opening = false;
		opened = true;
		AssetManager::GetGameObject(name + "_door")->setRotation(door_rotation + direction * glm::vec3(0, maxRotation, 0));
		rotation = maxRotation;
		return;
	}
	if (rotation <= 0) {
		opening = false;
		opened = false;
		AssetManager::GetGameObject(name + "_door")->setRotation(door_rotation);
		rotation = 0;
		return;
	}
	AssetManager::GetGameObject(name + "_door")->setRotation(door_rotation + direction * glm::vec3(0,rotation,0));
}
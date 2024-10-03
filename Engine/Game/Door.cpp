#include "Door.h"
#include "Animation.h"

Door::Door(std::string Name, Model* door, Model* frame, glm::vec3 position) {
	name = Name;
	AssetManager::AddGameObject(GameObject(name + "_frame", frame, position, false, 0, Concave));
	AssetManager::AddGameObject(name + "_door", door, position, false, 0, Concave);
	GameObject* gameobject = AssetManager::GetGameObject(name + "_door");
	gameobject->GetRigidBody()->setCcdMotionThreshold(0.1);
	gameobject->GetRigidBody()->setCcdSweptSphereRadius(0.2); // Set the radius for CCD
	door_rotation = gameobject->getRotation();
	door_position = position;
	rotation = 0; 

	opened = false;
	opening = false;
}

void Door::Interact() {
	if (Player::GetInteractingWithName() == name + "_door" && opening == false) {
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
	//if (!AnimationManager::IsAnimationPlaying("door_open") && !AnimationManager::IsAnimationPlaying("door_close"))
		//opening = false;
	
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
		AssetManager::GetGameObject(name + "_door")->SetRotationY(door_rotation.y + maxRotation);
		rotation = maxRotation;

		return;
	}
	if (rotation <= 0) {
		opening = false;
		opened = false;
		AssetManager::GetGameObject(name + "_door")->SetRotationY(door_rotation.y);
		rotation = 0;
		return;
	}
	AssetManager::GetGameObject(name + "_door")->setRotation(door_rotation + glm::vec3(0,rotation,0));
}
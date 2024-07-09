#include "Door.h"
#include "Engine/Core/AssetManager.h"



Door::Door(const char* frame, Texture* frameTexture, const char* door, Texture* doorTexture, glm::vec3 position, std::string name) {
	this->name = name;
	this->frame = AssetManager::GetGameObject(AssetManager::AddGameObject(std::string("test"), frame, frameTexture, position));

	this->door = AssetManager::GetGameObject(AssetManager::AddGameObject(std::string("best"), door, doorTexture, position));
	this->door->SetParentName(name);

	PhysicsManager::AddCube(this->door, name);
	collider = PhysicsManager::GetColider(name);
	collider->SetStatic(false);

}
void Door::Interact() {
	if (Player::GetInteractingWithName() == this->name && !Opening)
		Opening = true;	
}

void Door::Update() {
	if (Opening)
	{
		std::cout <<  "Before: " << door->getRotation().y << std::endl;
		if (!isOpened)
			door->SetRotationY(door->getRotation().y + doorOpenSpeed);
		else 
			door->SetRotationY(door->getRotation().y - doorOpenSpeed);
		std::cout << "Adter: " << door->getRotation().y << std::endl;


		if (door->getRotation().y >= 1.5708 || door->getRotation().y <= 0)
		{
			std::cout << "test1" << std::endl;
			Opening = false;
			isOpened = !isOpened;
			//collider->Regenerate(door);

		}

	}
}
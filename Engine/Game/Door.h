#pragma once
#include "Engine/Core/GameObject.h"
#include "string"
#include "Engine/Core/AssetManager.h"
//#include "Engine/Physics/Physics.h"
#include "Engine/Game/Player.h"
#include "Engine/Audio/Audio.h"

class Door
{
public:
	Door(std::string Name, Model* foor, Model* frame, glm::vec3 position);
	void Interact();
	void Update(float deltaTime);

private:
	std::string name;
	bool opened;
	bool opening;
	glm::vec3 door_position;
	glm::vec3 door_rotation;
	float rotation = 0.0f;
	float openingSpeed = 0.08f;
};

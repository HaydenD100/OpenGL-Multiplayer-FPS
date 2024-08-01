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
	Door(std::string Name, const char* doorPath, const char* framePath, Texture* doorTexture, Texture* frameTexture, glm::vec3 position);
	void Interact();
	void Update(float deltaTime);

private:
	std::string name;
	bool opened;
	bool opening;
	glm::vec3 door_position;
	float rotaion = 0.0f;
	float openingSpeed = 0.02f;
};

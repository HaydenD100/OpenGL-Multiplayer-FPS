#pragma once
#include <string>
#include "Engine/Core/Common/Defines.h"
#include "Engine/Renderer/Model.h"

//TODO:: redo this

class Door
{
public:
	Door(std::string Name, Model* foor, Model* frame, glm::vec3 position, glm::vec3  GameObjectRotation, bool inWards = true);
	void Interact();
	void Open();
	void Update(float deltaTime);

private:
	float direction;
	std::string name;
	bool opened;
	bool opening;
	glm::vec3 door_position;
	glm::vec3 door_rotation;
	double rotation = 0;
	double openingSpeed = 0.08f;
	double maxRotation = 1.5f;
};

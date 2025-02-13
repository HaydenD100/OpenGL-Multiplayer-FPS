#pragma once
#include <string>
#include <vector>
#include "Engine/Physics/BulletPhysics.h"
#include "Engine/Core/AssetManager.h"

//old stuff, not in use no more

class Crate
{
public:
	Crate(glm::vec3 position, std::string name, Model* model);
	void Update();
	void Break();
	void DealDamage(int damage, glm::vec3 position, glm::vec3 force);
	std::string GetName();
private:
	std::string name = "None";
	int health = 500;
};
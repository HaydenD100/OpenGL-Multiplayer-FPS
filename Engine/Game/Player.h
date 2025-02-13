#pragma once
#include "Engine/Core/Common/Defines.h"
#include "Engine/Core/Common/GameCommon.h"
#include "Engine/Core/GameObject.h"
#include <string>
#include "Engine/Game/Gun.h"

namespace Player
{
	extern GameObject* playerModel;
	extern GameObject* gun;
	glm::vec3 getPosition();
	glm::vec3 getForward();
	void setPosition(glm::vec3 pos);
	void Update(float deltaTime);
	void Init();
	std::string GetInteractingWithName();
	std::string getCurrentGun();

	bool SelectWeapon(std::string weaponName);
	bool OnGround();

	void SwitchWeapons(int index);
	void Shoot();
	void Graffite();

	void TakeDamage(int amount);
	void SetHealth(int NewHealth);
	int GetHealth();
	void Respawn();
	void AddToKill();
	int GetKills();
	int GetDeaths();
	int IsDead();

}

namespace PlayerTwo
{
	void Init();
	void SetData(std::string interact, std::string gunname, glm::vec3 position, glm::vec3 rotation);
	void SetIneractingWith(std::string interact);
	void Update();

	int GetKills();

	std::string GetInteractingWithName();
	std::string GetCurrentWeapon();
}
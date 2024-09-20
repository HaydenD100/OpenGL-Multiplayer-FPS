#pragma once
#include <string>
#include <vector>

#include "Engine/Core/Camera.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Game/Player.h"
#include "Engine/Core/AssetManager.h"

enum GunType
{
	Semi,
	Auto,
	ShotGun
};

struct Gun
{

	static glm::vec3 swayPosition;

	std::string name;
	int ammo;
	double firerate;
	int currentammo;
	double reloadtime;
	int damage;
	float recoil;
	float recoilY;
	float kickback;
	double lastTimeShot = glfwGetTime();
	std::string gunModel;
	std::string gunsShotName;
	GunType type;

	glm::vec3 weaponOffSet = glm::vec3(-0.3, -0.25, 0.5);
	glm::vec3 aimingPosition = glm::vec3(0, -0.2, 0.5);

	float kickbackOffset = 0;
	int down = 1;
	int firesounds = 4;
	int bulletsPerShot = 1;
	float spread = 0;
	void Update(float deltaTime, bool isReloading, bool aiming);
	void Shoot();

	//startingPos
};

namespace WeaponManager
{
	void Init();
	Gun* GetGunByName(std::string name);
}

class GunPickUp
{
public:
	static int GunPickUpCount;
	GunPickUp(std::string GunName, std::string ObjectName, Model* model, glm::vec3 position);
	GunPickUp(std::string GunName, glm::vec3 position, glm::vec3 force);
	void Update();
	bool Interact();
private:
	std::string gunName;
	std::string objectName;
};

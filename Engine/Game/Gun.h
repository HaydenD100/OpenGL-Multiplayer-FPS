#pragma once
#include <string>
#include <vector>

#include "Engine/Core/Camera.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Game/Player.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Core/SkinnedAnimatior.h"

#include "Engine/Core/Networking/NetworkManager.h"

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


	//animations
	bool hasAnimations = false;
	SkinnedAnimation shootAnim;
	SkinnedAnimation reloadAnim;
	SkinnedAnimation equipAnim;

	bool reloadPlaying = false;

	glm::vec3 weaponOffSet = glm::vec3(-0.3, -0.25, 0.5);
	glm::vec3 aimingPosition = glm::vec3(0, -0.2, 0.5);

	float kickbackOffset = 0;
	int down = 1;
	int firesounds = 4;
	int bulletsPerShot = 1;
	float spread = 0;
	void Update(float deltaTime, bool isReloading, bool aiming);
	void Shoot();
	void Reload();
	void Equip();


	//startingPos
};

namespace WeaponManager
{
	void Init();
	Gun* GetGunByName(std::string name);
}

class GunSpawner {
public:
	GunSpawner(std::string GunType, std::string spawnerName, glm::vec3 postion);
	void CheckForSpawn();

private:
	std::string gunType;
	std::string spawnerName;
	glm::vec3 postion;

	float timeSincePickUp = 0;
	float spawnTime = 5;
	bool needsSpawning = 1;
};

class GunPickUp
{
public:
	static int GunPickUpCount;
	GunPickUp(std::string GunName, std::string ObjectName, Model* model, glm::vec3 position);
	GunPickUp(std::string GunName, glm::vec3 position, glm::vec3 force);
	void Update();
	bool Interact();
	std::string GetName();
private:
	std::string gunName = "";
	std::string objectName = "Unkown";

};

#pragma once
#include <string>
#include <vector>
#include "Engine/Animation/SkinnedAnimatior.h"

enum GunType
{
	Semi,
	Auto,
	ShotGun,
	Melee
};

struct Gun
{
	static glm::vec3 swayPosition;

	std::string name;
	int ammo = 1;
	double firerate = 100;
	int currentammo = 1;
	double reloadtime = 0;
	int damage = 0;
	float recoil = 0;
	float recoilY = 0;
	float kickback = 0;
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

class GunPickUp
{
public:
	static int GunPickUpCount;
	GunPickUp(std::string GunName, Model* model, glm::vec3 position);
	void Update();
	bool Interact();
private:
	std::string objectName = "";
	std::string gunName;
};

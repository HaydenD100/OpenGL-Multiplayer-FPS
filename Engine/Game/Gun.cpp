#include "Gun.h"
#include "Engine/Core/Scene/SceneManager.h"
#include <sstream>

void Gun::Update(float deltaTime, bool isReloading, bool aiming) {
	GameObject* gun = AssetManager::GetGameObject(gunModel);
	gun->GetRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
	gun->GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
	kickbackOffset *= 0.96f;
	if (kickbackOffset < 0.01)
		kickbackOffset = 0;

	float verticalAngle = -gun->getRotation().x;
	float horizontalAngle = gun->getRotation().y;

	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
	
	if (isReloading) {
		if(!AnimationManager::IsAnimationPlaying("ak47_reload"))
			AnimationManager::Play("ak47_reload", Player::getCurrentGun());
	}
	else if (aiming) {
		AssetManager::GetGameObject(gunModel)->setPosition(aimingPosition);
	}
	else {
		gun->setPosition(weaponOffSet + (direction * -kickbackOffset * deltaTime));

		//TokyoSpliffs code
		float xSwayTarget = 0.0f;
		float ySwayTarget = 0.0f;


		if (Input::GetMouseOffsetX() < 0) {
			xSwayTarget = 2.5f;
		}
		if (Input::GetMouseOffsetX() > 0) {
			xSwayTarget = -2.5f;
		}
		if (Input::GetMouseOffsetY() < 0) {
			ySwayTarget = 0.5f;
		}
		if (Input::GetMouseOffsetY() > 0) {
			ySwayTarget = -0.5f;
		}

		float speed = 2.0f;
		swayPosition.x = finlerpTo(swayPosition.x, xSwayTarget, 1/60, speed);
		swayPosition.y = finlerpTo(swayPosition.y, ySwayTarget, 1/60, speed);
		gun->addPosition(swayPosition);
	}
	

	for (int i = 1; i <= firesounds; i++) {
		AudioManager::GetSound(gunsShotName + std::to_string(i))->SetPosition(Player::getPosition());
	}
}

void Gun::Shoot() {
	int randomnum = (rand() % firesounds) + 1;
	AudioManager::PlaySound(gunsShotName + std::to_string(randomnum));
	kickbackOffset += kickback;
}

namespace WeaponManager
{
	std::vector<Gun> guns;

	void WeaponManager::Init() {
		AssetManager::AddGameObject(GameObject("glock", AssetManager::GetModel("glock"), glm::vec3(0.2, -0.25, 0.2), false, 0, Convex));
		AssetManager::GetGameObject("glock")->SetRender(false);
		AssetManager::GetGameObject("glock")->SetParentName("player_head");
		AssetManager::GetGameObject("glock")->SetShaderType("Overlay");


		AssetManager::AddGameObject(GameObject("ak47", AssetManager::GetModel("ak47"), glm::vec3(0.2, -0.25, -0.2), false, 0, Convex));
		AssetManager::GetGameObject("ak47")->SetRender(false);
		AssetManager::GetGameObject("ak47")->SetParentName("player_head");
		AssetManager::GetGameObject("ak47")->SetShaderType("Overlay"); 


		AssetManager::AddGameObject("shotgun", AssetManager::GetModel("shotgun"), glm::vec3(-3, 2, 3), true, 0, Convex);
		AssetManager::GetGameObject("shotgun")->SetRender(false);
		AssetManager::GetGameObject("shotgun")->SetParentName("player_head");
		AssetManager::GetGameObject("shotgun")->SetShaderType("Overlay");


		
		AudioManager::AddSound("Assets/Audio/shotgun_fire.wav", "shotgun_fire1", AssetManager::GetGameObject("shotgun")->getPosition(), 5, 0.4f);
		AudioManager::AddSound("Assets/Audio/ak47_fire1.wav", "ak47_fire1", AssetManager::GetGameObject("ak47")->getPosition(), 5,0.5f);
		AudioManager::AddSound("Assets/Audio/ak47_fire2.wav", "ak47_fire2", AssetManager::GetGameObject("ak47")->getPosition(), 5, 0.5f);
		AudioManager::AddSound("Assets/Audio/ak47_fire3.wav", "ak47_fire3", AssetManager::GetGameObject("ak47")->getPosition(), 5, 0.5f);
		AudioManager::AddSound("Assets/Audio/ak47_fire4.wav", "ak47_fire4", AssetManager::GetGameObject("ak47")->getPosition(), 5, 0.5f);
		AudioManager::AddSound("Assets/Audio/glock_fire1.wav", "glock_fire1", AssetManager::GetGameObject("glock")->getPosition(), 5, 0.5f);
		AudioManager::AddSound("Assets/Audio/glock_fire2.wav", "glock_fire2", AssetManager::GetGameObject("glock")->getPosition(), 5, 0.5f);
		AudioManager::AddSound("Assets/Audio/glock_fire3.wav", "glock_fire3", AssetManager::GetGameObject("glock")->getPosition(), 5, 0.5f);
		AudioManager::AddSound("Assets/Audio/glock_fire4.wav", "glock_fire4", AssetManager::GetGameObject("glock")->getPosition(), 5, 0.5f);
		AudioManager::AddSound("Assets/Audio/dry_fire.wav", "dry_fire", AssetManager::GetGameObject("glock")->getPosition(), 5, 0.2f);
		
		Gun glock;
		glock.name = "glock";
		glock.ammo = 18;
		glock.reloadtime = 1.5;
		glock.firerate = 250;
		glock.currentammo = 18;
		glock.damage = 10;
		glock.type = Semi;
		glock.recoil = 0.01f;
		glock.recoilY = 100;
		glock.kickback = 3;
		glock.weaponOffSet = glm::vec3(-0.3, -0.2f, 0.9);
		glock.aimingPosition = glm::vec3(0,-0.2, 0.7);
		glock.gunModel = "glock"; 
		glock.gunsShotName = "glock_fire";
		guns.emplace_back(glock);

		Gun ak47;
		ak47.name = "ak47";
		ak47.ammo = 30;
		ak47.reloadtime = 2.5;
		ak47.firerate = 600;
		ak47.currentammo = 30;
		ak47.damage = 25;
		ak47.type = Auto;
		ak47.recoil = 0.03f;
		ak47.recoilY = 175;
		ak47.kickback = 2;
		ak47.gunModel = "ak47";
		ak47.gunsShotName = "ak47_fire";
		ak47.weaponOffSet = glm::vec3(-0.3, -0.25, 0.9);
		ak47.aimingPosition = glm::vec3(0, -0.2, 0.7);
		guns.emplace_back(ak47);

		Gun shotgun;
		shotgun.name = "shotgun";
		shotgun.ammo = 6;
		shotgun.reloadtime = 2.5;
		shotgun.firerate = 150;
		shotgun.currentammo = 6;
		shotgun.damage = 45;
		shotgun.type = Semi;
		shotgun.recoil = 0.05f;
		shotgun.recoilY = 200;
		shotgun.kickback = 2;
		shotgun.firesounds = 1;
		shotgun.bulletsPerShot = 12;
		shotgun.spread = 0.10f;
		shotgun.gunModel = "shotgun";
		shotgun.gunsShotName = "shotgun_fire";
		shotgun.weaponOffSet = glm::vec3(-0.3, -0.25, 0.9);
		shotgun.aimingPosition = glm::vec3(0, -0.2, 0.7);
		guns.emplace_back(shotgun);
	}
	
	Gun* WeaponManager::GetGunByName(std::string name) {
		for (int i = 0; i < guns.size(); i++)
			if (guns[i].name == name)
				return &guns[i];
		return nullptr;
	}
}

GunPickUp::GunPickUp(std::string GunName, std::string ObjectName, Model* model, glm::vec3 position) {
	gunName = GunName;
	objectName = ObjectName;
	AssetManager::AddGameObject(objectName, model, position, false,1,Convex);
	GunPickUpCount++;
}

// TODO: fix throwing weapon
GunPickUp::GunPickUp(std::string GunName, glm::vec3 position, glm::vec3 force) {
	std::string temp = std::to_string(GunPickUpCount);
	std::cout << temp << "\n";
	objectName = temp;
	gunName = GunName;
	int index = AssetManager::AddGameObject(objectName, AssetManager::GetModel(GunName), position, false, 1, Convex);
	GunPickUpCount++;

	AssetManager::GetGameObject(index)->GetRigidBody()->applyCentralImpulse(glmToBtVector3(force));
}

glm::vec3 Gun::swayPosition = glm::vec3(0);

void GunPickUp::Update() {
	
}

bool GunPickUp::Interact() {
	if (Player::GetInteractingWithName() != objectName || !Player::SelectWeapon(gunName))
		return false;
	
	GameObject* object = AssetManager::GetGameObject(objectName);
	PhysicsManagerBullet::GetDynamicWorld()->removeRigidBody(object->GetRigidBody());
	object->SetRender(false);
		
	WeaponManager::GetGunByName(gunName)->currentammo = WeaponManager::GetGunByName(gunName)->ammo;
	AudioManager::PlaySound("item_pickup", Player::getPosition());
	return true;
}

int GunPickUp::GunPickUpCount = 0;

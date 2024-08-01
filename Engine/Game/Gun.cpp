#include "Gun.h"
#include "Engine/Core/Scene/SceneManager.h"



void Gun::Update(float deltaTime, bool isReloading, bool aiming) {
	GameObject* gun = AssetManager::GetGameObject(gunModel);
	gun->GetRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
	gun->GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
	kickbackOffset = kickbackOffset * 0.96;
	if (kickbackOffset < 0.01) kickbackOffset = 0;

	float verticalAngle = -gun->getRotation().x;
	float horizontalAngle = gun->getRotation().y;

	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
	if (isReloading)
	{
		float currentXRotation = AssetManager::GetGameObject(gunModel)->getRotation().x;
		if (currentXRotation > 1.6 / 2)
			down = -1;			
		float incerment = (1.6 / reloadtime) * down * deltaTime;
		gun->SetRotationX(currentXRotation + incerment);
		gun->addPosition(glm::vec3(0,-incerment/3,0));
	}
	else if (aiming) {
		AssetManager::GetGameObject(gunModel)->setPosition(aimingPosition);
	}
	else
	{
		gun->setPosition(weaponOffSet + (direction * -kickbackOffset * deltaTime));
		gun->SetRotationX(0);
	}
	AudioManager::GetSound(gunsShotName + std::to_string(1))->SetPosition(Player::getPosition());
	AudioManager::GetSound(gunsShotName + std::to_string(2))->SetPosition(Player::getPosition());
	AudioManager::GetSound(gunsShotName + std::to_string(3))->SetPosition(Player::getPosition());
	AudioManager::GetSound(gunsShotName + std::to_string(4))->SetPosition(Player::getPosition());

}
void Gun::Shoot() {
	int randomnum = (rand() % 4) + 1;
	AudioManager::PlaySound(gunsShotName + std::to_string(randomnum));
	kickbackOffset += kickback;
}

namespace WeaponManager {
	std::vector<Gun> guns;

	void WeaponManager::Init() {

		AssetManager::AddGameObject(GameObject("glock", "Assets/Objects/glock_17.obj", AssetManager::GetTexture("glock"), glm::vec3(0.2, -0.25, 0.2), false,0,Box,0,0,0));
		AssetManager::GetGameObject("glock")->SetRender(false);
		AssetManager::GetGameObject("glock")->SetParentName("player_head");

		AssetManager::AddTexture("ak47", "Assets/Textures/ak47.png","Assets/Normals/ak47_normal.png");
		AssetManager::AddGameObject(GameObject("ak47", "Assets/Objects/ak47.obj", AssetManager::GetTexture("ak47"), glm::vec3(0.2, -0.25, -0.2), false,0,Box, 0, 0, 0));
		AssetManager::GetGameObject("ak47")->SetRender(false);
		AssetManager::GetGameObject("ak47")->SetParentName("player_head");


		AudioManager::AddSound("Assets/Audio/ak47_fire1.wav", "ak47_fire1", AssetManager::GetGameObject("ak47")->getPosition(), 5,0.5);
		AudioManager::AddSound("Assets/Audio/ak47_fire2.wav", "ak47_fire2", AssetManager::GetGameObject("ak47")->getPosition(), 5, 0.5);
		AudioManager::AddSound("Assets/Audio/ak47_fire3.wav", "ak47_fire3", AssetManager::GetGameObject("ak47")->getPosition(), 5, 0.5);
		AudioManager::AddSound("Assets/Audio/ak47_fire4.wav", "ak47_fire4", AssetManager::GetGameObject("ak47")->getPosition(), 5, 0.5);
		AudioManager::AddSound("Assets/Audio/glock_fire1.wav", "glock_fire1", AssetManager::GetGameObject("glock")->getPosition(), 5, 0.5);
		AudioManager::AddSound("Assets/Audio/glock_fire2.wav", "glock_fire2", AssetManager::GetGameObject("glock")->getPosition(), 5, 0.5);
		AudioManager::AddSound("Assets/Audio/glock_fire3.wav", "glock_fire3", AssetManager::GetGameObject("glock")->getPosition(), 5, 0.5);
		AudioManager::AddSound("Assets/Audio/glock_fire4.wav", "glock_fire4", AssetManager::GetGameObject("glock")->getPosition(), 5, 0.5);
		AudioManager::AddSound("Assets/Audio/dry_fire.wav", "dry_fire", AssetManager::GetGameObject("glock")->getPosition(), 5, 0.2);



		Gun glock;
		glock.name = "glock";
		glock.ammo = 18;
		glock.reloadtime = 1.5;
		glock.firerate = 250;
		glock.currentammo = 18;
		glock.damage = 10;
		glock.type = Semi;
		glock.recoil = 0.01;
		glock.recoilY = 100;
		glock.kickback = 3;
		glock.weaponOffSet = glm::vec3(-0.3, -0.2, 0.9);
		glock.aimingPosition = glm::vec3(0,-0.16, 0.7);

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
		ak47.recoil = 0.03;
		ak47.recoilY = 175;
		ak47.kickback = 2;
		ak47.gunModel = "ak47";
		ak47.gunsShotName = "ak47_fire";
		ak47.weaponOffSet = glm::vec3(-0.3, -0.25, 0.9);
		ak47.aimingPosition = glm::vec3(0, -0.2, 0.7);
		guns.emplace_back(ak47);

	}
	Gun* WeaponManager::GetGunByName(std::string name) {
		for (int i = 0; i < guns.size(); i++)
			if (guns[i].name == name)
				return &guns[i];
		return NULL;
	}
}


GunPickUp::GunPickUp(std::string GunName, std::string ObjectName, const char* objectModel, Texture* texture, glm::vec3 position) {
	gunName = GunName;
	objectName = ObjectName;
	AssetManager::AddGameObject(objectName, objectModel, texture, position, false,1,Convex);
}
// TODO: Doesn't work
GunPickUp::GunPickUp(std::string GunName, std::string GunObject, glm::vec3 position) {
	gunName = GunName;
	objectName = GunObject + std::to_string(SceneManager::GetCurrentScene()->GetGunPickUpSize());
	std::cout << "here" << std::endl;
	AssetManager::GetGameObject(GunObject)->Copy(objectName);
	std::cout << "here1" << std::endl;
	AssetManager::GetGameObject(objectName)->SetRender(true);
	AssetManager::GetGameObject(objectName)->setPosition(position);

}
void GunPickUp::Update() {
}

bool GunPickUp::Interact() {
	if (Player::GetInteractingWithName() == objectName && Player::SelectWeapon(gunName)) {
		GameObject* object = AssetManager::GetGameObject(objectName);

		PhysicsManagerBullet::GetDynamicWorld()->removeRigidBody(object->GetRigidBody());
		object->SetRender(false);
		
		WeaponManager::GetGunByName(gunName)->currentammo = WeaponManager::GetGunByName(gunName)->ammo;
		AudioManager::PlaySound("item_pickup", Player::getPosition());
		return true;
	}
	return false;
}
#include "Gun.h"
#include "Engine/Core/Scene/SceneManager.h"
#include <sstream>



void Gun::Update(float deltaTime, bool isReloading, bool aiming) {
	GameObject* gun = AssetManager::GetGameObject(gunModel);
	gun->GetRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
	gun->GetRigidBody()->setLinearVelocity(btVector3(0, 0, 0));
	if (! hasAnimations) {
		kickbackOffset *= 0.96f;
		if (kickbackOffset < 0.01)
			kickbackOffset = 0;
	}
	

	float verticalAngle = -gun->getRotation().x;
	float horizontalAngle = gun->getRotation().y;

	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
	

	if (aiming) {
		AssetManager::GetGameObject(gunModel)->setPosition(aimingPosition);
	}
	else {
		gun->setPosition(weaponOffSet + (direction * -kickbackOffset * 1.0f/60.0f));

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

void Gun::Equip() {
	if (hasAnimations) {
		Animator::PlayAnimation(&equipAnim, name, false);
		NetworkManager::SendAnimation(equipAnim.GetName(), name + "_PlayerTwo");
	}
		
}

void Gun::Reload() {
	if (hasAnimations) {
		Animator::PlayAnimation(&reloadAnim, name, false);
		NetworkManager::SendAnimation(reloadAnim.GetName(), name + "_PlayerTwo");
	}
	else 
		AnimationManager::Play("ak47_reload", Player::getCurrentGun());
		
	

}

void Gun::Shoot(){
	int randomnum = (rand() % firesounds) + 1;
	AudioManager::PlaySound(gunsShotName + std::to_string(randomnum));
	NetworkManager::SendSound(gunsShotName + std::to_string(randomnum), Player::getPosition());

	if (hasAnimations) {
		Animator::PlayAnimation(&shootAnim, name, false);
		//tell the other instance to play animation
		NetworkManager::SendAnimation(shootAnim.GetName(), name + "_PlayerTwo");
		
	}
	else 
		kickbackOffset += kickback;
}

namespace WeaponManager
{
	std::vector<Gun> guns;

	void WeaponManager::Init() {
		AssetManager::AddGameObject(GameObject("glock", AssetManager::GetModel("glockhand"), glm::vec3(5, 0, -5), false, 0, Convex));
		AssetManager::GetGameObject("glock")->SetRender(false);
		AssetManager::GetGameObject("glock")->SetParentName("player_head");
		AssetManager::GetGameObject("glock")->SetShaderType("Overlay");


		AssetManager::AddGameObject(GameObject("ak47", AssetManager::GetModel("ak47hand"), glm::vec3(0.2, -0.25, -0.2), false, 0, Convex));
		AssetManager::GetGameObject("ak47")->SetRender(false);
		AssetManager::GetGameObject("ak47")->SetParentName("player_head");
		AssetManager::GetGameObject("ak47")->SetShaderType("Overlay"); 


		AssetManager::AddGameObject("shotgun", AssetManager::GetModel("shotgun"), glm::vec3(-3, 2, 3), false, 0, Convex);
		AssetManager::GetGameObject("shotgun")->SetRender(false);
		AssetManager::GetGameObject("shotgun")->SetParentName("player_head");
		AssetManager::GetGameObject("shotgun")->SetShaderType("Overlay");

		AssetManager::AddGameObject("double_barrel", AssetManager::GetModel("double_barrel_hand"), glm::vec3(-3, 2, 3), false, 0, Convex);
		AssetManager::GetGameObject("double_barrel")->SetRender(false);
		AssetManager::GetGameObject("double_barrel")->SetParentName("player_head");
		AssetManager::GetGameObject("double_barrel")->SetShaderType("Overlay");


		
		AudioManager::AddSound("Assets/Audio/shotgun_fire.wav", "shotgun_fire1", AssetManager::GetGameObject("shotgun")->getPosition(), 1, 0.4f);
		AudioManager::AddSound("Assets/Audio/ak47_fire1.wav", "ak47_fire1", AssetManager::GetGameObject("ak47")->getPosition(), 10,0.5f);
		AudioManager::AddSound("Assets/Audio/ak47_fire2.wav", "ak47_fire2", AssetManager::GetGameObject("ak47")->getPosition(), 10, 0.5f);
		AudioManager::AddSound("Assets/Audio/ak47_fire3.wav", "ak47_fire3", AssetManager::GetGameObject("ak47")->getPosition(), 10, 0.5f);
		AudioManager::AddSound("Assets/Audio/ak47_fire4.wav", "ak47_fire4", AssetManager::GetGameObject("ak47")->getPosition(), 10, 0.5f);
		AudioManager::AddSound("Assets/Audio/glock_fire1.wav", "glock_fire1", AssetManager::GetGameObject("glock")->getPosition(), 10, 0.5f);
		AudioManager::AddSound("Assets/Audio/glock_fire2.wav", "glock_fire2", AssetManager::GetGameObject("glock")->getPosition(), 10, 0.5f);
		AudioManager::AddSound("Assets/Audio/glock_fire3.wav", "glock_fire3", AssetManager::GetGameObject("glock")->getPosition(), 10, 0.5f);
		AudioManager::AddSound("Assets/Audio/glock_fire4.wav", "glock_fire4", AssetManager::GetGameObject("glock")->getPosition(), 10, 0.5f);
		AudioManager::AddSound("Assets/Audio/dry_fire.wav", "dry_fire", AssetManager::GetGameObject("glock")->getPosition(), 5, 0.2f);
	
		Gun glock;
		glock.name = "glock";
		glock.ammo = 18;
		glock.reloadtime = 1.5;
		glock.firerate = 250; 
		glock.shootAnim = SkinnedAnimation("Assets/Objects/FBX/glock17_shoot1.dae", AssetManager::GetModel("glockhand"),0, "glock17_shoot");
		glock.reloadAnim = SkinnedAnimation("Assets/Objects/FBX/glock17_reload.dae", AssetManager::GetModel("glockhand"), 0, "glock17_reload");
		glock.equipAnim = SkinnedAnimation("Assets/Objects/FBX/glock17_equip.dae", AssetManager::GetModel("glockhand"), 0, "glock17_equip");

		glock.hasAnimations = true;
		glock.currentammo = 18;
		glock.damage = 14;
		glock.type = Semi;
		glock.recoil = 0.01f;
		glock.recoilY = 100;
		glock.kickback = 3;
		glock.weaponOffSet = glm::vec3(-0.3, -0.2f, 0.9);
		glock.aimingPosition = glm::vec3(0.15,-0.2, 0.7);
		glock.gunModel = "glock"; 
		glock.gunsShotName = "glock_fire";
		guns.emplace_back(glock);

		Gun ak47;
		ak47.name = "ak47";
		ak47.ammo = 30;
		ak47.reloadtime = 2.5;
		ak47.firerate = 600;
		ak47.currentammo = 30;
		ak47.damage = 18;
		ak47.type = Auto;
		ak47.recoil = 0.03f;
		ak47.recoilY = 175;
		ak47.kickback = 2;

		ak47.shootAnim = SkinnedAnimation("Assets/Objects/FBX/ak47_shoot.dae", AssetManager::GetModel("ak47hand"), 0, "ak47_shoot");
		ak47.reloadAnim = SkinnedAnimation("Assets/Objects/FBX/ak47_reload.dae", AssetManager::GetModel("ak47hand"), 0, "ak47_reload");
		ak47.equipAnim = SkinnedAnimation("Assets/Objects/FBX/ak47_equip.dae", AssetManager::GetModel("ak47hand"), 0, "ak47_equip");
		ak47.hasAnimations = true;

		ak47.gunModel = "ak47";
		ak47.gunsShotName = "ak47_fire";
		ak47.weaponOffSet = glm::vec3(-0.3, -0.25, 0.9);
		ak47.aimingPosition = glm::vec3(0, -0.32, 0.7);
		guns.emplace_back(ak47);


		//removing this for now
		Gun shotgun;
		shotgun.name = "shotgun";
		shotgun.ammo = 6;
		shotgun.reloadtime = 2.5;
		shotgun.firerate = 150;
		shotgun.currentammo = 6;
		shotgun.damage = 10;
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

		Gun doublebarrel;
		doublebarrel.name = "double_barrel";
		doublebarrel.ammo = 2;
		doublebarrel.reloadtime = 2;
		doublebarrel.firerate = 100;
		doublebarrel.currentammo = 2;
		doublebarrel.damage = 10;
		doublebarrel.type = Semi;
		doublebarrel.recoil = 0.07f;
		doublebarrel.recoilY = 250;
		doublebarrel.kickback = 2;

		doublebarrel.shootAnim = SkinnedAnimation("Assets/Objects/FBX/db_shoot.dae", AssetManager::GetModel("double_barrel_hand"), 1, "db_shoot");
		doublebarrel.reloadAnim = SkinnedAnimation("Assets/Objects/FBX/db_reload.dae", AssetManager::GetModel("double_barrel_hand"),0, "db_reload");
		doublebarrel.equipAnim = SkinnedAnimation("Assets/Objects/FBX/db_equip.dae", AssetManager::GetModel("double_barrel_hand"), 1, "db_equip");
		doublebarrel.hasAnimations = true;

		doublebarrel.firesounds = 1;
		doublebarrel.bulletsPerShot = 12;
		doublebarrel.spread = 0.15f;
		doublebarrel.gunModel = "double_barrel";
		doublebarrel.gunsShotName = "shotgun_fire";
		doublebarrel.weaponOffSet = glm::vec3(-0.27, -0.2f, 1.5);
		doublebarrel.aimingPosition = glm::vec3(0, -0.2, 0.7);
		guns.emplace_back(doublebarrel);
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

GunPickUp::GunPickUp(std::string GunName, glm::vec3 position, glm::vec3 force) {

	//STILL GIVING ERROR STING TO LONG ERROR NOT SURE WHY NOT SURE HOW TO FIX
	objectName.reserve(100);

	std::stringstream ss;
	ss << Player::getPosition().x;
	ss << Camera::GetDirection().y;
	ss << GunName;
	objectName = ss.str();
	gunName = GunName;
	AssetManager::AddGameObject(objectName, AssetManager::GetModel(GunName), position, false, 1, Convex);
	GunPickUpCount++;
	AssetManager::GetGameObject(objectName)->GetRigidBody()->applyCentralImpulse(glmToBtVector3(force));
}

glm::vec3 Gun::swayPosition = glm::vec3(0);

void GunPickUp::Update() {
	
}
std::string GunPickUp::GetName() {
	return objectName;
}


bool GunPickUp::Interact() {
	if (PlayerTwo::GetInteractingWithName() == objectName && PlayerTwo::GetCurrentWeapon() != gunName) {
		GameObject* object = AssetManager::GetGameObject(objectName);
		PhysicsManagerBullet::GetDynamicWorld()->removeRigidBody(object->GetRigidBody());
		object->SetRender(false);
		AssetManager::RemoveGameObject(objectName);

		AudioManager::PlaySound("item_pickup", Player::getPosition());
		return true;
	}
	if (Player::GetInteractingWithName() != objectName || !Player::SelectWeapon(gunName))
		return false;
	
	GameObject* object = AssetManager::GetGameObject(objectName);
	PhysicsManagerBullet::GetDynamicWorld()->removeRigidBody(object->GetRigidBody());
	object->SetRender(false);
		
	WeaponManager::GetGunByName(gunName)->currentammo = WeaponManager::GetGunByName(gunName)->ammo;
	AudioManager::PlaySound("item_pickup", Player::getPosition());
	return true;


}
GunSpawner::GunSpawner(std::string GunType, std::string spawnerName, glm::vec3 postion) {
	gunType = GunType;
	this->spawnerName = spawnerName;
	this->postion = postion;
}

void GunSpawner::CheckForSpawn() {
	if (needsSpawning && timeSincePickUp < glfwGetTime() - spawnTime) {
		std::cout << "Spawning Gun \n";
		SceneManager::GetCurrentScene()->AddGunPickUp(GunPickUp(gunType, spawnerName, AssetManager::GetModel(gunType), postion));
		needsSpawning = 0;
	}
	else if(needsSpawning == 0) {
		needsSpawning = !SceneManager::GetCurrentScene()->DoesGunPickUpExsit(spawnerName);
		if (needsSpawning)
			timeSincePickUp = glfwGetTime();
		std::cout << "needs spawning: " << needsSpawning << "\n";
	}
}

int GunPickUp::GunPickUpCount = 0;

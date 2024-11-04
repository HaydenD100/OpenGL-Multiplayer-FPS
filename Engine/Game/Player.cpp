#include "Player.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Core/Scene/SceneManager.h"
#include "Engine/Physics/BulletPhysics.h"
#include <random>


namespace Player
{
	glm::vec3 forward;
	float horizontalAngle = 3.14f;
	float verticalAngle = 0.0f;
	float initialFoV = 45.0f;
	float maxAngle = 1.5;
	std::string gunName = "nothing";
	std::string interactingWithName = "nothing";
	float interactDistance = 3;

	float swayIntensity = 0.02f;
	float swaySpeed = 2.0f;
	float smoothFactor = 0.1f;
	float mouseSpeed = 0.005f;
	float speed = 5000;

	const float runningSpeed = 6000;
	const float walkingSpeed = 4000;

	float airSpeed = 1000;
	float MaxSpeed = 6;
	float jumpforce = 9;

	// States
	bool reloading = false;
	bool aiming = false;

	double reloadingTime = 0;
	double footstepTime = 0;
	double footstep_interval = 0.8;

	const double walkingfootstep_interval = 0.6;
	const double runningfootstep_interval = 0.3;


	std::string inv[4] = {"ak47","glock","double_barrel"};
	const int decal_count = 5;
	std::string decal_inv[decal_count] = { "flower_decal","panda_decal","pizza_decal","tank_decal", "freaky_decal"};
	int decal_index = 0;

	//Game Logic
	int Health = 100;
	int isDead = 0;
	float timeSinceDeath = 0;
	float animationDeathTime = 0.5f;


	void Player::Init() {
		timeSinceDeath = glfwGetTime();
		srand((unsigned int)time(nullptr));
		AssetManager::AddGameObject(GameObject("player", AssetManager::GetModel("player") , glm::vec3(0, 10, 5), false, 1, Capsule, 0.5, 2.2, 0.5));
		AssetManager::AddGameObject(GameObject("player_head", AssetManager::GetModel("player"), glm::vec3(0, 10, 5), false, 0, Sphere, 0, 0, 0));
		GameObject* player_head = AssetManager::GetGameObject("player_head");
		GameObject* player_body = AssetManager::GetGameObject("player");

		player_head->SetRender(false);
		btBroadphaseProxy*  proxy = player_head->GetRigidBody()->getBroadphaseHandle();
		if (proxy) {
			proxy->m_collisionFilterGroup = GROUP_PLAYER;
			proxy->m_collisionFilterMask = GROUP_STATIC | GROUP_DYNAMIC;
			// TODO: Add the constraint to the world
		}
		btBroadphaseProxy* otherProxy = player_body->GetRigidBody()->getBroadphaseHandle();
		if (otherProxy) {
			// Set the collision filter group and mask for the other object
			otherProxy->m_collisionFilterGroup = GROUP_DYNAMIC; // Group for dynamic objects
			otherProxy->m_collisionFilterMask = GROUP_STATIC | GROUP_DYNAMIC; // Collide with static and dynamic, but not the player
		}

		proxy->m_collisionFilterMask &= ~GROUP_DYNAMIC; // Remove dynamic objects from the player head's collision mask
		otherProxy->m_collisionFilterMask &= ~GROUP_PLAYER; // Remove the player group from the other object's collision mask


		btRigidBody* playerHeadRigidBody = player_head->GetRigidBody(); // Assuming GetRigidBody() returns btRigidBody*
		playerHeadRigidBody->setActivationState(DISABLE_SIMULATION);

		btRigidBody* body = AssetManager::GetGameObject("player")->GetRigidBody();
		AssetManager::GetGameObject("player")->SetRender(false);
		body->setFriction(0.0f);
		body->setRestitution(0.0f);
		body->setCcdMotionThreshold(0.05f);
		body->setCcdSweptSphereRadius(0.2f); // Set the radius for CCD

		body->setGravity(btVector3(0, -10 * 3.0f, 0));
		player_head->GetRigidBody()->setFriction(0.0f);
		player_head->GetRigidBody()->setRestitution(0.0f);
		player_head->GetRigidBody()->setActivationState(false);


		proxy = body->getBroadphaseHandle();
		if (proxy) {
			proxy->m_collisionFilterGroup = GROUP_PLAYER;
			proxy->m_collisionFilterMask = GROUP_STATIC | GROUP_DYNAMIC;
			// TODO: Add the constraint to the world
		}
		
		//std::cout << "loading player model" << std::endl;
		//gunName = "ak47";
	}
	void Player::Shoot() {
		if (gunName == "nothing")
			return;

		if (WeaponManager::GetGunByName(gunName)->currentammo > 0) {
			WeaponManager::GetGunByName(gunName)->currentammo--;
			WeaponManager::GetGunByName(gunName)->Shoot();
			verticalAngle += WeaponManager::GetGunByName(gunName)->recoil;
			horizontalAngle += (float)rand() / RAND_MAX / WeaponManager::GetGunByName(gunName)->recoilY;

			for (int i = 0; i < WeaponManager::GetGunByName(gunName)->bulletsPerShot; i++) {
				float maxSpread = WeaponManager::GetGunByName(gunName)->spread;
				btCollisionWorld::ClosestRayResultCallback hit = Camera::GetRayHit(maxSpread);
				if (hit.m_collisionObject != nullptr) {
					GameObject* gameobject = AssetManager::GetGameObject(hit.m_collisionObject->getUserIndex());
					if (gameobject != nullptr)
					{
						btRigidBody* body = gameobject->GetRigidBody();
						btVector3 localForcePos = body->getWorldTransform().inverse() * hit.m_hitPointWorld;
						body->applyImpulse(2 * glmToBtVector3(Camera::ComputeRay()), localForcePos);
						glm::vec4 worldPositionHomogeneous(glm::vec3(hit.m_hitPointWorld.getX(), hit.m_hitPointWorld.getY(), hit.m_hitPointWorld.getZ()), 1.0f);
						glm::vec4 localPositionHomogeneous = glm::inverse(gameobject->GetModelMatrix()) * worldPositionHomogeneous;
						glm::vec3 vec3local = glm::vec3(localPositionHomogeneous.x, localPositionHomogeneous.y, localPositionHomogeneous.z);
						glm::vec3 normal = glm::vec3(hit.m_hitNormalWorld.getX(), hit.m_hitNormalWorld.getY(), hit.m_hitNormalWorld.getZ());
						glm::mat4 rotation_matrix = glm::mat4_cast(glm::quat(gameobject->getRotation()));
						normal = glm::vec3(glm::inverse(rotation_matrix) * glm::vec4(normal, 0));
						AssetManager::AddDecalInstance(vec3local, normal, AssetManager::GetDecal("bullet_hole"), gameobject);

						NetworkManager::SendGunShotData(gameobject->GetName(), "bullet_hole", vec3local, normal, btToGlmVector3(localForcePos), WeaponManager::GetGunByName(gunName)->damage, btToGlmVector3(2 * glmToBtVector3(Camera::ComputeRay())));
					}
				}
			}
		}
		else {
			// Click click
			AudioManager::PlaySound("dry_fire", Player::getPosition());
			NetworkManager::SendSound("dry_fire", Player::getPosition());

		}
		
		WeaponManager::GetGunByName(gunName)->lastTimeShot = glfwGetTime();
	}
	void Player::Graffite() {
		btCollisionWorld::ClosestRayResultCallback hit = Camera::GetRayHit();
		if (hit.m_collisionObject != nullptr) {
			GameObject* gameobject = AssetManager::GetGameObject(hit.m_collisionObject->getUserIndex());
			if (gameobject != nullptr)
			{
				btRigidBody* body = gameobject->GetRigidBody();
				glm::vec4 worldPositionHomogeneous(glm::vec3(hit.m_hitPointWorld.getX(), hit.m_hitPointWorld.getY(), hit.m_hitPointWorld.getZ()), 1.0f);
				glm::vec4 localPositionHomogeneous = glm::inverse(gameobject->GetModelMatrix()) * worldPositionHomogeneous;
				glm::vec3 vec3local = glm::vec3(localPositionHomogeneous.x, localPositionHomogeneous.y, localPositionHomogeneous.z);
				glm::vec3 normal = glm::vec3(hit.m_hitNormalWorld.getX(), hit.m_hitNormalWorld.getY(), hit.m_hitNormalWorld.getZ());
				glm::mat4 rotation_matrix = glm::mat4_cast(glm::quat(gameobject->getRotation()));
				normal = glm::vec3(glm::inverse(rotation_matrix) * glm::vec4(normal, 0));

				AssetManager::AddDecalInstance(vec3local, normal, AssetManager::GetDecal(decal_inv[decal_index]), gameobject);

			}
		}
	}
	
	bool Player::OnGround() {
		GameObject* player = AssetManager::GetGameObject("player");
		glm::vec3 out_end = player->getPosition() + glm::vec3(0,-1.1,0);

		btCollisionWorld::ClosestRayResultCallback RayCallback(
			btVector3(player->getPosition().x, player->getPosition().y, player->getPosition().z),
			btVector3(out_end.x, out_end.y, out_end.z)
		);
		
		PhysicsManagerBullet::GetDynamicWorld()->rayTest(
			btVector3(player->getPosition().x, player->getPosition().y, player->getPosition().z),
			btVector3(out_end.x, out_end.y, out_end.z),
			RayCallback
		);
		
		if (RayCallback.hasHit())
			return true;
		
		return false;
	}


	//TODO :: This is a mess I should clean this up at somepoint
	void Player::Update(float deltaTime) {
		if (isDead && timeSinceDeath < glfwGetTime() - animationDeathTime) {
			Respawn();
		}

		if (isDead) {
			return;
		}
			


		GameObject* player =  AssetManager::GetGameObject("player");

		GameObject* head = AssetManager::GetGameObject("player_head");
		player->setRotation(glm::vec3(horizontalAngle,0, 0));
		head->setRotation(glm::vec3(-verticalAngle, horizontalAngle, 0));
		head->setPosition(player->getPosition() + glm::vec3(0, 1, 0));

		if(verticalAngle < 1.5f && verticalAngle > -1.5f)
			Camera::SetVerticalAngle(verticalAngle);
		Camera::SetHorizontalAngle(horizontalAngle);
		Camera::SetPosition(head->getPosition());

		if (gunName != "nothing")
			WeaponManager::GetGunByName(gunName)->Update(deltaTime, reloading, aiming);

		bool IsGrounded = OnGround();
		if (IsGrounded) {
			player->GetRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
			player->GetRigidBody()->setLinearVelocity(btVector3(player->GetRigidBody()->getLinearVelocity().x() * 0.0, 0, player->GetRigidBody()->getLinearVelocity().z() * 0.0));
		}
		player->GetRigidBody()->setLinearVelocity(btVector3(player->GetRigidBody()->getLinearVelocity().x() * 0.0, player->GetRigidBody()->getLinearVelocity().y(), player->GetRigidBody()->getLinearVelocity().z() * 0.0));
			
		btQuaternion quat;
		quat.setEuler(0, player->getRotation().y, 0);
		player->GetRigidBody()->getWorldTransform().setRotation(quat);

		interactingWithName = "nothing";

		if (verticalAngle <= maxAngle && verticalAngle >= -maxAngle)
			verticalAngle += mouseSpeed * float(SCREENHEIGHT / 2 - Input::GetMouseY());
		
		else if (verticalAngle > maxAngle)
			verticalAngle = maxAngle;
		
		else if (verticalAngle < -maxAngle)
			verticalAngle = -maxAngle;
		
		forward = Camera::GetRotation();
		forward.y = 0;
		forward = glm::normalize(forward);
		
		// Right vector
		glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0,1,0)));

		
		// Move forward
		glm::vec3 movement = glm::vec3(0, player->GetRigidBody()->getLinearVelocity().y(), 0);
		if (Input::KeyDown('w')) {
			movement += forward;
		}
		
		// Move backward
		if (Input::KeyDown('s')) {
			movement += -forward;
		}
		
		// Strafe right
		if (Input::KeyDown('d')) {
			movement += right;
		}
		
		// Strafe left
		if (Input::KeyDown('a')) {
			movement += -right;
		}
		
		// Jump
		if (Input::KeyDown(' ') && IsGrounded) {
			movement.y = 1 * jumpforce;
		}
		if (Input::LeftShiftDown()) {
			speed = runningSpeed;
			footstep_interval = runningfootstep_interval;
		}
			
		else {
			speed = walkingSpeed;
			footstep_interval = walkingfootstep_interval;
		}
			
		
		//movement = glm::normalize(movement);
		float lenght = glm::vec3(movement.x, 0, movement.z).length();
		movement.x = movement.x / lenght;
		movement.z = movement.z / lenght;

		movement.x = movement.x * speed;
		movement.z = movement.z * speed;

		//fallout4 did it so why not? TODO:: fix delta time
		movement.x = movement.x * 0.003f;
		movement.z = movement.z * 0.003f;

		/*
		if (movement.x > MaxSpeed) movement.x = MaxSpeed;
		if (movement.x < -MaxSpeed) movement.x = -MaxSpeed;
		if (movement.z > MaxSpeed) movement.z = MaxSpeed;
		if (movement.z < -MaxSpeed) movement.z = -MaxSpeed;
		*/
		
		player->GetRigidBody()->setLinearVelocity(glmToBtVector3(movement));
		if (Input::KeyPressed('e')) {
			btCollisionWorld::ClosestRayResultCallback hit = Camera::GetRayHit();
			if (hit.m_collisionObject != nullptr) {
				GameObject* gameobject = AssetManager::GetGameObject(hit.m_collisionObject->getUserIndex());
				if (gameobject != nullptr && glm::distance(gameobject->getPosition(), getPosition()) <= interactDistance)
					interactingWithName = gameobject->GetName();
			}
		}
		if (Input::KeyPressed('g')) {
			Graffite();
		}
		if (Input::KeyPressed('t')) {
			if (decal_index == decal_count -1)
				decal_index = 0;
			else
				decal_index++;
		}
		
		if (Input::KeyPressed('r') && !reloading && !aiming && gunName != "nothing") {
			reloading = true;
			reloadingTime = glfwGetTime();
			WeaponManager::GetGunByName(gunName)->Reload();

		}

		if (Input::RightMouseDown() && !reloading) {
			aiming = true;
		}
		else {
			aiming = false;
		}
		
		if (gunName != "nothing") {
			if (glfwGetTime() - reloadingTime > WeaponManager::GetGunByName(gunName)->reloadtime && reloading)
			{
				reloading = false;
				WeaponManager::GetGunByName(gunName)->currentammo = WeaponManager::GetGunByName(gunName)->ammo;
				WeaponManager::GetGunByName(gunName)->down = 1;
			}
			
			// Get ray details
			if (Input::LeftMousePressed() && WeaponManager::GetGunByName(gunName)->type == Semi && glfwGetTime() - WeaponManager::GetGunByName(gunName)->lastTimeShot > 60.0f / WeaponManager::GetGunByName(gunName)->firerate && !reloading) {
				Shoot();
			}
			else if (Input::LeftMouseDown() && WeaponManager::GetGunByName(gunName)->type == Auto && glfwGetTime() - WeaponManager::GetGunByName(gunName)->lastTimeShot > 60.0f / WeaponManager::GetGunByName(gunName)->firerate && !reloading) {
				Shoot();
			}
			
			//TODO:: FIX bad alloc error
			/*
			if (Input::KeyPressed('q') && !reloading) {
				SceneManager::GetCurrentScene()->AddGunPickUp(GunPickUp(getCurrentGun(), getPosition() + glm::vec3(0, 1, 0) + Camera::GetDirection(), Camera::GetDirection() * 7.0f));
				AssetManager::GetGameObject(gunName)->SetRender(false);
				gunName = "nothing";
			}
			*/
		}
		
		//optimize this
		if (Input::KeyPressed('1')) {
			SelectWeapon(inv[0]);
		}

		if (Input::KeyPressed('2')) {
			SelectWeapon(inv[1]);
		}
		if (Input::KeyPressed('3')) {
			SelectWeapon(inv[2]);
		}
		
		horizontalAngle += mouseSpeed * float(SCREENWIDTH / 2 - Input::GetMouseX());

		if ((Input::KeyDown('w') || Input::KeyDown('a') || Input::KeyDown('s') || Input::KeyDown('d')) && footstepTime + footstep_interval < glfwGetTime() ) {
			AudioManager::PlaySound("foot_step" + std::to_string((rand() % 4) + 1), player->getPosition());
			NetworkManager::SendSound("foot_step" + std::to_string((rand() % 4) + 1), player->getPosition());
			footstepTime = glfwGetTime();
		}

		if(GetInteractingWithName() != "nothing")
			std::cout << GetInteractingWithName() << "\n";

		//AudioManager::UpdateListener(player->getPosition(), Camera::GetDirection(), btToGlmVector3(player->GetRigidBody()->getLinearVelocity()));

		//sometimes you fall through floor
		if (getPosition().y < -100)
			Respawn();

	}
	
	glm::vec3 Player::getPosition() {
		return AssetManager::GetGameObject("player")->getPosition();
	}
	
	glm::vec3 Player::getForward() {
		return forward;
	}
	
	void Player::setPosition(glm::vec3 pos) {
		AssetManager::GetGameObject("player")->setPosition(pos);
		Camera::SetPosition(AssetManager::GetGameObject("player")->getPosition());
	}
	
	std::string Player::GetInteractingWithName() {
		return interactingWithName;
	}
	
	std::string Player::getCurrentGun() {
		return gunName;
	}
	
	bool Player::SelectWeapon(std::string weaponName) {
		if (reloading || weaponName == gunName)
			return false;
		if (gunName != "nothing")
			AssetManager::GetGameObject(WeaponManager::GetGunByName(gunName)->name)->SetRender(false);
		gunName = weaponName;
		AssetManager::GetGameObject(WeaponManager::GetGunByName(gunName)->name)->SetRender(true);
		AudioManager::PlaySound("item_pickup", getPosition());
		WeaponManager::GetGunByName(weaponName)->Equip();
		return true;
	}
	
	void Player::SwitchWeapons(int index) {
		AssetManager::GetGameObject(inv[index])->SetRender(false);
		gunName = inv[index];
	}

	void Player::TakeDamage(int amount) {
		//packets get evaluated after respawn leading to take damage after respawning
		if (timeSinceDeath + animationDeathTime < 2.0f) {
			Health = 100;
			return;
		}
			
		Health -= amount;
		if (Health <= 0 && !isDead) {
			//death
			Health = 0;
			isDead = 1;
			timeSinceDeath = glfwGetTime();
			NetworkManager::SendAnimation("bean_death", "PlayerTwo");
			if(gunName != "nothing")
				AssetManager::GetGameObject(gunName)->SetRender(false);
			gunName = "nothing";
			

		}
	}

	void Player::SetHealth(int NewHealth) {
		Health = NewHealth;
	}
	int Player::GetHealth() {
		return Health;
	}
	void Player::Respawn() {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> distrib(0, spawnpointsize);
		int spawnpointindex = distrib(gen);

		Health = 100;
		if(gunName != "nothing")
			AssetManager::GetGameObject(gunName)->SetRender(false);
		gunName = "nothing";
		setPosition(spawnpoints[spawnpointindex]);
		isDead = 0;

	}
}


namespace PlayerTwo
{
	std::string objectName = "PlayerTwo";
	std::string currentGun = "nothing";
	std::string interactingWithName = "nothing";

	void PlayerTwo::Init() {
		AssetManager::AddGameObject("PlayerTwo", AssetManager::GetModel("playertwo"), glm::vec3(0, 2, 0), false, 0, Convex);
		if (NetworkManager::IsServer()) {
			GameObject* otherPlayer = AssetManager::GetGameObject("PlayerTwo");
			if (otherPlayer != nullptr) {
				otherPlayer->SetRender(false);
				otherPlayer->GetRigidBody()->setActivationState(DISABLE_SIMULATION);
			}
		}
		

		AssetManager::AddGameObject(GameObject("glock_PlayerTwo", AssetManager::GetModel("glockhand"), glm::vec3(-0.3, 0.25f, 0.9), false, 0, Convex));
		AssetManager::GetGameObject("glock_PlayerTwo")->SetRender(false);
		AssetManager::GetGameObject("glock_PlayerTwo")->SetParentName("PlayerTwo");
		AssetManager::GetGameObject("glock_PlayerTwo")->SetScale(0.3);
		AssetManager::GetGameObject("glock_PlayerTwo")->SetDontCull(true);

		AssetManager::AddGameObject(GameObject("ak47_PlayerTwo", AssetManager::GetModel("ak47hand"), glm::vec3(-0.3, 0.25, 0.9), false, 0, Convex));
		AssetManager::GetGameObject("ak47_PlayerTwo")->SetRender(false);
		AssetManager::GetGameObject("ak47_PlayerTwo")->SetParentName("PlayerTwo");
		AssetManager::GetGameObject("ak47_PlayerTwo")->SetScale(0.3);
		AssetManager::GetGameObject("ak47_PlayerTwo")->SetDontCull(true);

		AssetManager::AddGameObject("shotgun_PlayerTwo", AssetManager::GetModel("shotgun"), glm::vec3(-0.3, 0.25, 0.9), false, 0, Convex);
		AssetManager::GetGameObject("shotgun_PlayerTwo")->SetRender(false);
		AssetManager::GetGameObject("shotgun_PlayerTwo")->SetParentName("PlayerTwo");
		AssetManager::GetGameObject("shotgun_PlayerTwo")->SetScale(0.3);
		AssetManager::GetGameObject("shotgun_PlayerTwo")->SetDontCull(true);

		AssetManager::AddGameObject("double_barrel_PlayerTwo", AssetManager::GetModel("double_barrel_hand"), glm::vec3(-0.3, 0.25f, 0.9), false, 0, Convex);
		AssetManager::GetGameObject("double_barrel_PlayerTwo")->SetRender(false);
		AssetManager::GetGameObject("double_barrel_PlayerTwo")->SetParentName("PlayerTwo");
		AssetManager::GetGameObject("double_barrel_PlayerTwo")->SetScale(0.3);
		AssetManager::GetGameObject("double_barrel_PlayerTwo")->SetDontCull(true);

		AssetManager::AddSkinnedAnimation(SkinnedAnimation("Assets/Objects/FBX/glock17_shoot1.dae", AssetManager::GetModel("glockhand"), 0, "glock17_shoot"));
		AssetManager::AddSkinnedAnimation(SkinnedAnimation("Assets/Objects/FBX/glock17_reload.dae", AssetManager::GetModel("glockhand"), 0, "glock17_reload"));
		AssetManager::AddSkinnedAnimation(SkinnedAnimation("Assets/Objects/FBX/glock17_equip.dae", AssetManager::GetModel("glockhand"), 0, "glock17_equip"));

		AssetManager::AddSkinnedAnimation(SkinnedAnimation("Assets/Objects/FBX/ak47_shoot.dae", AssetManager::GetModel("ak47hand"), 0, "ak47_shoot"));
		AssetManager::AddSkinnedAnimation(SkinnedAnimation("Assets/Objects/FBX/ak47_reload.dae", AssetManager::GetModel("ak47hand"), 0, "ak47_reload"));
		AssetManager::AddSkinnedAnimation(SkinnedAnimation("Assets/Objects/FBX/ak47_equip.dae", AssetManager::GetModel("ak47hand"), 0, "ak47_equip"));

		AssetManager::AddSkinnedAnimation(SkinnedAnimation("Assets/Objects/FBX/db_shoot.dae", AssetManager::GetModel("double_barrel_hand"), 1, "db_shoot"));
		AssetManager::AddSkinnedAnimation(SkinnedAnimation("Assets/Objects/FBX/db_reload.dae", AssetManager::GetModel("double_barrel_hand"), 0, "db_reload"));
		AssetManager::AddSkinnedAnimation(SkinnedAnimation("Assets/Objects/FBX/db_equip.dae", AssetManager::GetModel("double_barrel_hand"), 1, "db_equip"));

		AssetManager::AddSkinnedAnimation(SkinnedAnimation("Assets/Objects/FBX/bean_death.dae", AssetManager::GetModel("playertwo"), 0, "bean_death"));



	}
	void PlayerTwo::SetData(std::string interact, std::string gunname, glm::vec3 position, glm::vec3 rotation) {
		interactingWithName = interact;
		if (gunname != currentGun) {
			if(currentGun != "nothing")
				AssetManager::GetGameObject(currentGun + "_PlayerTwo")->SetRender(false);
			currentGun = gunname;
			if (currentGun != "nothing")
				AssetManager::GetGameObject(currentGun + "_PlayerTwo")->SetRender(true);

			std::cout << "Player_two changed weapon to: " << currentGun << "\n";

		}
			

		AssetManager::GetGameObject("PlayerTwo")->setPosition(position);
		AssetManager::GetGameObject("PlayerTwo")->setRotation(rotation);

	}
	void PlayerTwo::Update() {

	}

	std::string PlayerTwo::GetInteractingWithName() {
		return interactingWithName;
	}
	void PlayerTwo::SetIneractingWith(std::string interact) {
		interactingWithName = interact;
	}
	std::string PlayerTwo::GetCurrentWeapon() {
		return currentGun;
	}

	
}
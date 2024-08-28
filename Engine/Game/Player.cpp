#include "Player.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Core/Scene/SceneManager.h"
#include "Engine/Physics/BulletPhysics.h"

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
	float airSpeed = 1000;
	float MaxSpeed = 6;
	float jumpforce = 9;

	// States
	bool reloading = false;
	bool aiming = false;

	double reloadingTime = 0;
	double footstepTime = 0;
	double footstep_interval = 0.5;

	std::string inv[2] = { "ak47","glock" };

	void Player::Init() {
		srand((unsigned int)time(nullptr));
		AssetManager::AddGameObject(GameObject("player", SceneManager::GetCurrentScene()->GetModel("player"), glm::vec3(0, 10, 5), false, 1, Capsule, 0.5, 1, 0.5));
		AssetManager::AddGameObject(GameObject("player_head", SceneManager::GetCurrentScene()->GetModel("player"), glm::vec3(0, 10, 5), false, 0, Sphere, 1, 2, 1));
		GameObject* player_head = AssetManager::GetGameObject("player_head");
		player_head->SetRender(false);
		btBroadphaseProxy*  proxy = player_head->GetRigidBody()->getBroadphaseHandle();
		if (proxy) {
			proxy->m_collisionFilterGroup = GROUP_PLAYER;
			proxy->m_collisionFilterMask = GROUP_STATIC | GROUP_DYNAMIC;
			// TODO: Add the constraint to the world
		}

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
			btCollisionWorld::ClosestRayResultCallback hit = Camera::GetRayHit();
			if (hit.m_collisionObject != nullptr) {
				GameObject* gameobject = AssetManager::GetGameObject(hit.m_collisionObject->getUserIndex());
				if (gameobject != nullptr)
				{
					btRigidBody* body = gameobject->GetRigidBody();
					body->applyImpulse(2 * glmToBtVector3(Camera::ComputeRay()),  body->getWorldTransform().inverse() * hit.m_hitPointWorld );
					glm::vec4 worldPositionHomogeneous(glm::vec3(hit.m_hitPointWorld.getX(), hit.m_hitPointWorld.getY(), hit.m_hitPointWorld.getZ()), 1.0f);
					glm::vec4 localPositionHomogeneous = glm::inverse(gameobject->GetModelMatrix()) * worldPositionHomogeneous;
					glm::vec3 vec3local = glm::vec3(localPositionHomogeneous.x, localPositionHomogeneous.y, localPositionHomogeneous.z);

					if (body->getBroadphaseHandle()->m_collisionFilterGroup == GROUP_STATIC)
						AssetManager::AddDecal(vec3local, glm::vec3(hit.m_hitNormalWorld.getX(), hit.m_hitNormalWorld.getY(), hit.m_hitNormalWorld.getZ()), glm::vec3(0.025, 0.025, 0.025), AssetManager::GetTexture("bullet_hole"),gameobject);
				}
			}
		}
		else {
			// Click click
			AudioManager::PlaySound("dry_fire", AssetManager::GetGameObject("player")->getPosition());
		}
		
		WeaponManager::GetGunByName(gunName)->lastTimeShot = glfwGetTime();
	}
	
	bool Player::OnGround() {
		GameObject* player = AssetManager::GetGameObject("player");
		glm::vec3 out_end = player->getPosition() + glm::vec3(0,-0.65,0);

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

	void Player::Update(float deltaTime) {
		GameObject* player =  AssetManager::GetGameObject("player");

		GameObject* head = AssetManager::GetGameObject("player_head");
		player->setRotation(glm::vec3(0, horizontalAngle, 0));
		head->setRotation(glm::vec3(-verticalAngle, horizontalAngle, 0));
		head->setPosition(player->getPosition() + glm::vec3(0, 1.5, 0));

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

		interactingWithName = "Nothing";

		if (verticalAngle <= maxAngle && verticalAngle >= -maxAngle)
			verticalAngle += mouseSpeed * float(SCREENHEIGHT / 2 - Input::GetMouseY());
		
		else if (verticalAngle > maxAngle)
			verticalAngle = maxAngle;
		
		else if (verticalAngle < -maxAngle)
			verticalAngle = -maxAngle;
		
		forward = Camera::GetRotation();
		forward.y = 0;
		
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
		
		
		//movement = glm::normalize(movement);
		float lenght = glm::vec3(movement.x, 0, movement.z).length();
		movement.x = movement.x / lenght;
		movement.z = movement.z / lenght;

		movement.x = movement.x * speed;
		movement.z = movement.z * speed;

		//fallout4 did it so why not?? TODO:: fix delta time
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
		
		if (Input::KeyPressed('r') && !reloading && !aiming) {
			reloading = true;
			reloadingTime = glfwGetTime();

			//AnimationManager::Play("ak47_reload", "ak47");

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
			
			if (Input::KeyPressed('q') && !reloading) {
				SceneManager::GetCurrentScene()->AddGunPickUp(GunPickUp(getCurrentGun(),getPosition() + glm::vec3(0,1,0) + Camera::GetDirection(), Camera::GetDirection() * 7.0f));
				AssetManager::GetGameObject(gunName)->SetRender(false);
				gunName = "nothing";
			}
		}
		
		if (Input::KeyPressed('1')) {
			SelectWeapon(inv[0]);
		}

		if (Input::KeyPressed('2')) {
			SelectWeapon(inv[1]);
		}
		
		horizontalAngle += mouseSpeed * float(SCREENWIDTH / 2 - Input::GetMouseX());

		if ((Input::KeyDown('w') || Input::KeyDown('a') || Input::KeyDown('s') || Input::KeyDown('d')) && footstepTime + footstep_interval < glfwGetTime() ) {
			AudioManager::PlaySound("foot_step" + std::to_string((rand() % 4) + 1), AssetManager::GetGameObject("player")->getPosition());
			footstepTime = glfwGetTime();
		}

		if(GetInteractingWithName() != "Nothing")
			std::cout << GetInteractingWithName() << "\n";

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
			AssetManager::GetGameObject(WeaponManager::GetGunByName(gunName)->gunModel)->SetRender(false);
		gunName = weaponName;
		AssetManager::GetGameObject(WeaponManager::GetGunByName(gunName)->gunModel)->SetRender(true);
		AudioManager::PlaySound("item_pickup", getPosition());
		return true;
	}
	
	void Player::SwitchWeapons(int index) {
		AssetManager::GetGameObject(inv[index])->SetRender(false);
		gunName = inv[index];
	}
}

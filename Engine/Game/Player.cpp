#include "Player.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Core/Scene/SceneManager.h"
#include <random>

#include "Engine/Networking/NetworkManager.h"
#include "Engine/Renderer/Raycaster.h"
#include "Engine/Physics/BulletPhysics.h"
#include "Engine/Core/Camera.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/Common/GameCommon.h"



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

	float swayIntensity = 0.0f; 
	float swaySpeed = 0.0f;
	float smoothFactor = 0.1f;
	float speed = 4000;
	glm::vec2 totalRecoil = glm::vec2(0);



	float airSpeed = 800;
	float MaxSpeed = 6;
	float jumpforce = 6;

	// States
	bool reloading = false;
	bool aiming = false;

	double reloadingTime = 0;
	double footstepTime = 0;
	double footstep_interval = 0.8;

	const double walkingfootstep_interval = 0.5;
	const double runningfootstep_interval = 0.3;

	
	const int weapon_size = 4;
	std::string inv[weapon_size] = {"ak47","glock","double_barrel","knife"};
	const int decal_count = 5;
	std::string decal_inv[decal_count] = { "flower_decal","panda_decal","pizza_decal","tank_decal", "freaky_decal"};
	int decal_index = 0;

	//Game Logic
	int Health = 100;
	int isDead = 0;
	float timeSinceDeath = 0;
	float animationDeathTime = 0.5f;
	int kills = 0;
	int deaths = 0;

	void AddToKill() {
		kills++;
		std::cout << kills << "\n";
	}


	void Player::Init() {
		timeSinceDeath = glfwGetTime();
		srand((unsigned int)time(nullptr));
		SceneManager::GetCurrentScene()->g_objects.push_back(std::make_unique<GameObject>("player", AssetManager::GetModel("player") , glm::vec3(0, 10, 5), false, 1, Capsule, 0.5, 2.2, 0.5));
		SceneManager::GetCurrentScene()->g_objects.push_back(std::make_unique<GameObject>("player_head", AssetManager::GetModel("player"), glm::vec3(0, 10, 5), false, 0, Sphere, 0, 0, 0));
		GameObject* player_head = SceneManager::GetCurrentScene()->GetGameObject("player_head");
		GameObject* player_body = SceneManager::GetCurrentScene()->GetGameObject("player");

		player_head->SetRender(false);
		btBroadphaseProxy*  proxy = player_head->GetRigidBody()->getBroadphaseHandle();
		if (proxy) {
			proxy->m_collisionFilterGroup = GROUP_PLAYER;
			proxy->m_collisionFilterMask = GROUP_STATIC | GROUP_DYNAMIC;
		}
		btBroadphaseProxy* otherProxy = player_body->GetRigidBody()->getBroadphaseHandle();
		if (otherProxy) {
			// Set the collision filter group and mask for the other object
			otherProxy->m_collisionFilterGroup = GROUP_DYNAMIC; // Group for dynamic objects
			otherProxy->m_collisionFilterMask = GROUP_STATIC | GROUP_DYNAMIC; // Collide with static and dynamic, but not the player
		}

		proxy->m_collisionFilterMask &= ~GROUP_DYNAMIC; // Remove dynamic objects from the player head's collision mask
		otherProxy->m_collisionFilterMask &= ~GROUP_PLAYER; // Remove the player group from the other object's collision mask


		std::shared_ptr<btRigidBody> playerHeadRigidBody = player_head->GetRigidBody(); // Assuming GetRigidBody() returns btRigidBody*
		playerHeadRigidBody->setActivationState(DISABLE_SIMULATION);

		std::shared_ptr<btRigidBody> body = SceneManager::GetCurrentScene()->GetGameObject("player")->GetRigidBody();
		SceneManager::GetCurrentScene()->GetGameObject("player")->SetRender(false);
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
		}
		
		//std::cout << "loading player model" << std::endl;
		//gunName = "ak47";
	}
	void Player::Shoot() {
		if (gunName == "nothing")
			return;
		WeaponManager::GetGunByName(gunName)->lastTimeShot = glfwGetTime();


		if (WeaponManager::GetGunByName(gunName)->type == Melee) {
			WeaponManager::GetGunByName(gunName)->Shoot();
			btCollisionWorld::ClosestRayResultCallback hit = Camera::GetRayHit(0);
			if (hit.m_collisionObject != nullptr) {
				ObjectType type = static_cast<ObjectType>(reinterpret_cast<uintptr_t>(hit.m_collisionObject->getUserPointer()));
				GameObject* gameobject = nullptr;
				if (type == ObjectType::GLASS) {
					gameobject = SceneManager::GetCurrentScene()->g_glass[hit.m_collisionObject->getUserIndex()].get();
				}
				else if(type == ObjectType::DEFAULT) {
					gameobject = SceneManager::GetCurrentScene()->g_objects[hit.m_collisionObject->getUserIndex()].get();
				}

				if (gameobject != nullptr)
				{
					btVector3 start = hit.m_rayFromWorld; // Ray origin
					btVector3 end = hit.m_hitPointWorld; // Hit point
					float distance = (end - start).length();
					if (distance > 2)
						return;


					std::shared_ptr<btRigidBody> body = gameobject->GetRigidBody();
					if (body) {
						btVector3 localForcePos = body->getWorldTransform().inverse() * hit.m_hitPointWorld;
						body->applyImpulse(2 * glmToBtVector3(Camera::ComputeRay()), localForcePos);
						glm::vec4 worldPositionHomogeneous(glm::vec3(hit.m_hitPointWorld.getX(), hit.m_hitPointWorld.getY(), hit.m_hitPointWorld.getZ()), 1.0f);
						glm::vec4 localPositionHomogeneous = glm::inverse(gameobject->GetModelMatrix()) * worldPositionHomogeneous;
						glm::vec3 vec3local = glm::vec3(localPositionHomogeneous.x, localPositionHomogeneous.y, localPositionHomogeneous.z);
						glm::vec3 normal = glm::vec3(hit.m_hitNormalWorld.getX(), hit.m_hitNormalWorld.getY(), hit.m_hitNormalWorld.getZ());
						glm::mat4 rotation_matrix = glm::mat4_cast(glm::quat(gameobject->getRotation()));
						normal = glm::vec3(glm::inverse(rotation_matrix) * glm::vec4(normal, 0));
						AssetManager::AddDecalInstance(vec3local, normal, AssetManager::GetDecal("bullet_hole"), gameobject);
					}
					//NetworkManager::SendGunShotData(gameobject->GetName(), "bullet_hole", vec3local, normal, btToGlmVector3(localForcePos), WeaponManager::GetGunByName(gunName)->damage, btToGlmVector3(2 * glmToBtVector3(Camera::ComputeRay())));
					int randomnum = (rand() % 3) + 1;
					//if (type == DEFAULT)
						//AudioManager::PlaySound("bullet_impact_" + std::to_string(randomnum), gameobject->GetPosition());
					if (type == GLASS)
						AudioManager::PlaySound("bullet_impact_glass_" + std::to_string(randomnum), gameobject->GetPosition());

				}
			}
			return;
		}

		if (WeaponManager::GetGunByName(gunName)->currentammo >= 0) {

			if (WeaponManager::GetGunByName(gunName)->currentammo == 0) {
				AudioManager::PlaySound("dry_fire", Player::getPosition());
				WeaponManager::GetGunByName(gunName)->currentammo = -1;
				return;
			}
			
			WeaponManager::GetGunByName(gunName)->currentammo--;
			WeaponManager::GetGunByName(gunName)->Shoot();
			totalRecoil.x += WeaponManager::GetGunByName(gunName)->recoil * 0.01;
			totalRecoil.y += WeaponManager::GetGunByName(gunName)->recoilY * 0.1;

			for (int i = 0; i < WeaponManager::GetGunByName(gunName)->bulletsPerShot; i++) {
				float maxSpread = WeaponManager::GetGunByName(gunName)->spread;
				btCollisionWorld::ClosestRayResultCallback hit = Camera::GetRayHit(maxSpread);
				if (hit.m_collisionObject != nullptr) {
					ObjectType type = static_cast<ObjectType>(reinterpret_cast<uintptr_t>(hit.m_collisionObject->getUserPointer()));
					GameObject* gameobject = nullptr;
					if (type == ObjectType::GLASS) {
						int randomnum = (rand() % 3) + 1;
						AudioManager::PlaySound("bullet_impact_glass_" + std::to_string(randomnum), glm::vec3(hit.m_hitPointWorld.getX(), hit.m_hitPointWorld.getY(), hit.m_hitPointWorld.getZ()));
						return;
					}
					else if (type == ObjectType::DEFAULT) {
						gameobject = SceneManager::GetCurrentScene()->g_objects[hit.m_collisionObject->getUserIndex()].get();
					}
					else if (type == ObjectType::DESTORYABLE) {
						gameobject = SceneManager::GetCurrentScene()->g_objects[hit.m_collisionObject->getUserIndex()].get();
						if (gameobject->destructable.m_destoryed_object != "" && !gameobject->destructable.m_destoryed) {
							Model* model = AssetManager::GetModel(gameobject->destructable.m_destoryed_object);
							if (model) {
								
								gameobject->destructable.m_destoryed = 1;
								gameobject->SetModel(model);
								gameobject->GetRigidBody()->setCollisionShape(gameobject->destructable.convexHullShape);
								AudioManager::PlaySound(gameobject->destructable.m_destoryed_sound, gameobject->GetPosition());
								std::cout << gameobject->destructable.m_destoryed_object << "\n";
							}
								
						}
					}
					else {
						return;
					}
					
					if (gameobject != nullptr )
					{
						std::cout << "Object Name: " << gameobject->GetName() << "\n";
						std::shared_ptr<btRigidBody> body = gameobject->GetRigidBody();
						if (!body) {
							return; // Early exit if no rigid body
						}

						btTransform transform = body->getWorldTransform();
						btVector3 origin = transform.getOrigin();

						// Check for NaN/Inf in the transform
						if (std::isnan(origin.x()) || std::isinf(origin.x()) ||
							std::isnan(origin.y()) || std::isinf(origin.y()) ||
							std::isnan(origin.z()) || std::isinf(origin.z())) {
							std::cerr << "Invalid transform (NaN/Inf detected)!" << std::endl;
							return;
						}

						// Compute local force position safely
						btVector3 hitPointWorld = hit.m_hitPointWorld;
						btVector3 localForcePos = transform.inverse() * hitPointWorld;

						// Check for NaN/Inf in the impulse calculation
						btVector3 impulse = 2 * glmToBtVector3(Camera::ComputeRay());
						if (std::isnan(impulse.x()) || std::isinf(impulse.x()) ||
							std::isnan(impulse.y()) || std::isinf(impulse.y()) ||
							std::isnan(impulse.z()) || std::isinf(impulse.z())) {
							std::cerr << "Invalid impulse (NaN/Inf detected)!" << std::endl;
							return;
						}

						// Apply impulse
						body->applyImpulse(impulse, localForcePos);

						// Convert hit point to glm
						glm::vec4 worldPositionHomogeneous(
							hitPointWorld.x(), hitPointWorld.y(), hitPointWorld.z(),
							1.0f
						);

						// Get model matrix once
						glm::mat4 modelMatrix = gameobject->GetModelMatrix();
						glm::vec4 localPositionHomogeneous = glm::inverse(modelMatrix) * worldPositionHomogeneous;
						glm::vec3 vec3local(localPositionHomogeneous);

						// Convert normal and rotate into local space
						btVector3 btNormal = hit.m_hitNormalWorld;
						glm::vec3 normal(btNormal.x(), btNormal.y(), btNormal.z());
						glm::quat rotation = gameobject->getRotation(); // Ensure this exists!
						glm::mat4 rotationMatrix = glm::mat4_cast(rotation);
						normal = glm::vec3(glm::inverse(rotationMatrix) * glm::vec4(normal, 0.0f));

						// Ensure decal asset exists before adding
						Decal* decal = AssetManager::GetDecal("bullet_hole");
						if (decal) {
							AssetManager::AddDecalInstance(vec3local, normal, decal, gameobject);
						}
						//NetworkManager::SendGunShotData(gameobject->GetName(), "bullet_hole", vec3local, normal, btToGlmVector3(localForcePos), WeaponManager::GetGunByName(gunName)->damage, btToGlmVector3(2 * glmToBtVector3(Camera::ComputeRay())));
						//if (type == DEFAULT)
							//AudioManager::PlaySound("bullet_impact_" + std::to_string(randomnum), gameobject->GetPosition());
						
					}
				}
			}
		}
		
	}
	void Player::Graffite() {
		btCollisionWorld::ClosestRayResultCallback hit = Camera::GetRayHit();
		if (hit.m_collisionObject != nullptr) {
			GameObject* gameobject = SceneManager::GetCurrentScene()->g_objects[hit.m_collisionObject->getUserIndex()].get();
			if (gameobject != nullptr)
			{
				btVector3 start = hit.m_rayFromWorld; // Ray origin
				btVector3 end = hit.m_hitPointWorld; // Hit point
				float distance = (end - start).length();
				if (distance > 4)
					return;
				std::shared_ptr<btRigidBody> body = gameobject->GetRigidBody();
				glm::vec4 worldPositionHomogeneous(glm::vec3(hit.m_hitPointWorld.getX(), hit.m_hitPointWorld.getY(), hit.m_hitPointWorld.getZ()), 1.0f);
				glm::vec4 localPositionHomogeneous = glm::inverse(gameobject->GetModelMatrix()) * worldPositionHomogeneous;
				glm::vec3 vec3local = glm::vec3(localPositionHomogeneous.x, localPositionHomogeneous.y, localPositionHomogeneous.z);
				glm::vec3 normal = glm::vec3(hit.m_hitNormalWorld.getX(), hit.m_hitNormalWorld.getY(), hit.m_hitNormalWorld.getZ());
				glm::mat4 rotation_matrix = glm::mat4_cast(glm::quat(gameobject->getRotation()));
				normal = glm::vec3(glm::inverse(rotation_matrix) * glm::vec4(normal, 0));

				//Stops z fighting by offesting the decal by a random amount
				std::srand(std::time(0));
				double randNum = 0.2 + static_cast<double>(std::rand()) / RAND_MAX * (0.2 - 0.01);

				AssetManager::AddDecalInstance(vec3local + normal * glm::vec3(randNum), normal, AssetManager::GetDecal(decal_inv[decal_index]), gameobject);
				AudioManager::PlaySound("spray_paint");

			}
		}
	}
	
	bool Player::OnGround() {
		GameObject* player = SceneManager::GetCurrentScene()->GetGameObject("player");
		glm::vec3 out_end = player->getPosition() + glm::vec3(0,-1.2,0);

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


	//TODO :: This is a mess I should clean this up at some point
	void Player::Update(float deltaTime) {
		if (isDead && timeSinceDeath < glfwGetTime() - animationDeathTime) {
			Respawn();
		}

		if (isDead) 
			return;

		horizontalAngle += totalRecoil.x * 0.001;
		verticalAngle += totalRecoil.y * 0.001;

		totalRecoil -= totalRecoil * 0.2f;

		if (glm::length(totalRecoil) < 0.001f)
			totalRecoil = glm::vec2(0.0f);

		//float recoilDamping = 5.0f; // adjust this for how fast it fades
		//totalRecoil -= totalRecoil * recoilDamping * deltaTime;
			

		GameObject* player =  SceneManager::GetCurrentScene()->GetGameObject("player");
		GameObject* head = SceneManager::GetCurrentScene()->GetGameObject("player_head");
		player->setRotation(glm::vec3(horizontalAngle,0, 0));
		head->setRotation(glm::vec3(-verticalAngle, horizontalAngle, 0));
		head->setPosition(player->getPosition() + glm::vec3(0, 1, 0));

		if(verticalAngle < 1.5f && verticalAngle > -1.5f)
			Camera::SetVerticalAngle(verticalAngle);
		Camera::SetHorizontalAngle(horizontalAngle);
		Camera::SetPosition(head->getPosition());

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
			verticalAngle += Input::GetSensitivity() * float(Backend::GetHeight() / 2 - Input::GetMouseY());
		
			
		
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
		if (Input::KeyDown(FORWARD)) {
			movement += forward;
		}
		
		// Move backward
		if (Input::KeyDown(BACKWARD)) {
			movement += -forward;
		}
		
		// Strafe right
		if (Input::KeyDown(RIGHT)) {
			movement += right;
		}
		
		// Strafe left
		if (Input::KeyDown(LEFT)) {
			movement += -right;
		}
		
		// Jump
		if (Input::KeyDown(JUMP) && IsGrounded) {
			movement.y = 1 * jumpforce;
		}
		if (Input::LeftShiftDown()) {
			speed = RUNNINGSPEED;
			footstep_interval = runningfootstep_interval;
		}
		else if (Input::KeyDown(CROUCH)) {
			speed = CRROUCHINGSPEED;
			footstep_interval = walkingfootstep_interval * 2;
		}
			
		else {
			speed = WALKINGSPEED;
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
		
		player->GetRigidBody()->setLinearVelocity(glmToBtVector3(movement));
		if (Input::KeyPressed(INTERACT)) {
			btCollisionWorld::ClosestRayResultCallback hit = Camera::GetRayHit();
			if (hit.m_collisionObject != nullptr) {
				GameObject* gameobject = SceneManager::GetCurrentScene()->g_objects[hit.m_collisionObject->getUserIndex()].get();
				if (gameobject != nullptr && glm::distance(gameobject->getPosition(), getPosition()) <= interactDistance)
					interactingWithName = gameobject->GetName();
			}
		}
		if (Input::KeyPressed(SPRAYPAINT)) {
			Graffite();
		}
		if (Input::KeyPressed(CYCLESPRAY)) {
			if (decal_index == decal_count -1)
				decal_index = 0;
			else
				decal_index++;
		}
		
		if (Input::KeyPressed(RELOAD) && !reloading && !aiming && gunName != "nothing" && WeaponManager::GetGunByName(getCurrentGun())->type != Melee) {
			reloading = true;
			reloadingTime = glfwGetTime();
			WeaponManager::GetGunByName(gunName)->Reload();

		}
		if (Input::KeyPressed(DROPWEAPON) && gunName != "nothing" && WeaponManager::GetGunByName(getCurrentGun())->type != Melee) {
			Gun* gun = WeaponManager::GetGunByName(gunName);
			Model* gunModel = AssetManager::GetModel(gun->gunModel);
			GunPickUp temp_pickup = GunPickUp(gunName,gunModel,Camera::GetPosition() + Camera::GetDirection() * 1.5f);
			SceneManager::GetCurrentScene()->m_gunPickups.push_back(temp_pickup);
		}

		if (Input::RightMouseDown() && !reloading && WeaponManager::GetGunByName(gunName)->type != Melee && gunName != "nothing") {
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
			if (Input::LeftMousePressed() && (WeaponManager::GetGunByName(gunName)->type == Semi || WeaponManager::GetGunByName(gunName)->type == Melee) && glfwGetTime() - WeaponManager::GetGunByName(gunName)->lastTimeShot > 60.0f / WeaponManager::GetGunByName(gunName)->firerate && !reloading) {
				Shoot();
			}
			else if (Input::LeftMouseDown() && WeaponManager::GetGunByName(gunName)->type == Auto && glfwGetTime() - WeaponManager::GetGunByName(gunName)->lastTimeShot > 60.0f / WeaponManager::GetGunByName(gunName)->firerate && !reloading) {
				Shoot();
			}
			WeaponManager::GetGunByName(gunName)->Update(deltaTime, reloading, aiming);	
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
		if (Input::KeyPressed('4')) {
			SelectWeapon(inv[3]);
		}
		
		horizontalAngle += Input::GetSensitivity() * float(Backend::GetWidth() / 2 - Input::GetMouseX());

		if ((Input::KeyDown(FORWARD) || Input::KeyDown(LEFT) || Input::KeyDown(BACKWARD) || Input::KeyDown(RIGHT)) && footstepTime + footstep_interval < glfwGetTime()  && IsGrounded) {
			AudioManager::PlaySound("foot_step" + std::to_string((rand() % 4) + 1), player->getPosition());
			footstepTime = glfwGetTime();
		}

		//AudioManager::UpdateListener(player->getPosition(), Camera::GetDirection(), btToGlmVector3(player->GetRigidBody()->getLinearVelocity()));

		//sometimes you fall through floor
		if (getPosition().y < -100)
			Respawn();

		if(Input::KeyPressed(RESPAWN))
			Respawn();
		if (Input::KeyPressed(PLAYERINFO)) {
			std::cout << "========================== PLAYER INFO ======================= \n";
			std::cout << "Current Weapon: " << getCurrentGun() << "\n";
			std::cout << "death state: " << isDead << "\n";
			std::cout << "time since death: " << timeSinceDeath << "\n";
			std::cout << "============================================================== \n";



		}
	}
	
	glm::vec3 Player::getPosition() {
		return SceneManager::GetCurrentScene()->GetGameObject("player")->getPosition();
	}
	
	glm::vec3 Player::getForward() {
		return forward;
	}
	
	void Player::setPosition(glm::vec3 pos) {
		SceneManager::GetCurrentScene()->GetGameObject("player")->setPosition(pos);
		Camera::SetPosition(SceneManager::GetCurrentScene()->GetGameObject("player")->getPosition());
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
			SceneManager::GetCurrentScene()->GetGameObject(WeaponManager::GetGunByName(gunName)->name)->SetRender(false);
		gunName = weaponName;
		SceneManager::GetCurrentScene()->GetGameObject(WeaponManager::GetGunByName(gunName)->name)->SetRender(true);
		AudioManager::PlaySound("item_pickup", getPosition());
		WeaponManager::GetGunByName(weaponName)->Equip();
		return true;
	}
	
	void Player::SwitchWeapons(int index) {
		SceneManager::GetCurrentScene()->GetGameObject(inv[index])->SetRender(false);
		gunName = inv[index];
	}

	void Player::TakeDamage(int amount) {
			
		//packets get evaluated after respawn leading to take damage after respawning
		if (timeSinceDeath + animationDeathTime < 2.0f) {
			Health = 100;
			return;
		}

		Health -= amount;
		if (Health < 0)
			Health = 0;

		if (Health <= 0 && !isDead) {
			//death
			Health = 0;
			isDead = 1;
			timeSinceDeath = glfwGetTime();
			if(gunName != "nothing")
				SceneManager::GetCurrentScene()->GetGameObject(gunName)->SetRender(false);
			gunName = "nothing";

			deaths++;

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
			SceneManager::GetCurrentScene()->GetGameObject(gunName)->SetRender(false);
		gunName = "nothing";
		setPosition(spawnpoints[spawnpointindex]);
		isDead = 0;

		for (int i = 0; i < weapon_size; i++) {
			Gun* gun = WeaponManager::GetGunByName(inv[i]);
			gun->currentammo = gun->ammo;
		}
	}

	int GetKills() {
		return kills;
	}
	int GetDeaths() {
		return deaths;
	}
	int IsDead() {
		return isDead;
	}

}

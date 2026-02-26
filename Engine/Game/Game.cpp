#include "Game.h"
#include "Engine/Game/Player.h"

#include "Engine/Core/Scene/Scene.h"
#include "Engine/Core/Camera.h"
#include "Engine/Networking/NetworkManager.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Game/Player.h"
#include "Engine/Core/Common/GameCommon.h"
#include "Engine.h"
#include "Engine/Core/Scene/World.h"
#include "Engine/Networking/NetworkManager.h"


namespace Game {

	GameNetworkingMode m_multiPlayerMode;

	int m_playerKills = 0;
	int m_enemyKills = 0;

	std::vector<PlayerN> players;

	void Init(GameNetworkingMode multiPlayerMode) {
		m_multiPlayerMode = multiPlayerMode;
		float startLoadTime = glfwGetTime();

		if (multiPlayerMode == MultiPlayer) {
			//TODO :: myGui ConnectMenu
			std::cout << "==================================CONNECT/HOST=========================================================================\n";
			std::cout << "ENTER the IP of the server to join:";
			char temp[256];
			std::cin.getline(temp, sizeof(temp));
			//Client::Init(temp);

			Client::Init("127.0.0.1");
		}

		AssetManager::Init();
		World::Load();
		WeaponManager::Init();
		//AssetManager::LoadAssets();

		Player::Init();
		Player::setPosition(glm::vec3(0, 10, 0));
		Animator::Init();

		PathFinding::Init();
		std::cout << "Scene and Asset Load took " << (glfwGetTime() - startLoadTime) << "s \n";
	}
	void Update(float dt) {
		CheckDebugPress();

		World::Update(dt);
		Player::Update(dt);
		Camera::Update(dt);
			
		if (Input::KeyPressed(SPAWNFLOATIES)) {
			Destructible desruct;
			desruct.m_destoryed_object = "uni_float_defalated";
			desruct.m_destoryed_sound = "pop";
			std::string random = generateRandomString(16);
			World::AddGameObject(random, AssetManager::GetModel("uni_float"), glm::vec3(3, 3, 0), true, 0.5, Convex);
			World::GetGameObject(random)->GetRigidBody()->setUserPointer((void*)ObjectType::DESTORYABLE);
			World::GetGameObject(random)->destructable = desruct;
			World::GetGameObject(random)->m_buoyancy = 6.0;

		}
		
		Animator::UpdateAnimation(dt);
		AudioManager::UpdateListener(Player::getPosition(), Player::getForward(), Player::getForward());

		if (m_multiPlayerMode == MultiPlayer) {
			Client::SendWorldPosition(Player::getPosition(), Player::getForward());
		}

		//Host keeps track of all the physics objects 
		
	}
	void CheckDebugPress() {
		if (Input::KeyDown(RELOADSHADERS))
			Renderer::LoadAllShaders();
		if (Input::KeyDown(BAKEGI))
			Renderer::probeGrid.Bake();
	}
	void CleanUp() {
		Client::CleanUp();
	}
}
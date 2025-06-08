#include "Game.h"
#include "Engine/Game/Player.h"
#include "Engine/Core/Scene/SceneManager.h"
#include "Engine/Core/Scene/Scene.h"
#include "Engine/Core/Camera.h"
#include "Engine/Networking/NetworkManager.h"
#include "Engine/Core/Input.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Game/Player.h"
#include "Engine/Core/Common/GameCommon.h"
#include "Engine.h"

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
			//std::cout << "==================================CONNECT/HOST=========================================================================\n";
			//std::cout << "ENTER the IP of the server to join:";
			//char temp[256];
			//std::cin.getline(temp, sizeof(temp));
			//Client::Init(temp);

			Client::Init("127.0.0.1");
		}

		AssetManager::Init();
		SceneManager::Init();
		SceneManager::CreateScene(std::move(Scene()));
		SceneManager::LoadScene(0);
		WeaponManager::Init();
		AssetManager::LoadAssets();

		Player::Init();
		Player::setPosition(glm::vec3(0, 10, 0));
		Animator::Init();

		//PathFinding::Init();
		std::cout << "Scene and Asset Load took " << (glfwGetTime() - startLoadTime) << "s \n";
	}
	void Update(float dt) {
		CheckDebugPress();

		SceneManager::Update(dt);
		Player::Update(dt);
		Camera::Update(dt);
			
		
		Animator::UpdateAnimation(dt);
		AudioManager::UpdateListener(Player::getPosition(), Player::getForward(), Player::getForward());

		//Host keeps track of all the physics objects 
		
	}
	void CheckDebugPress() {
		if (Input::KeyDown(RELOADSHADERS))
			Renderer::LoadAllShaders();
		if (Input::KeyDown(BAKEGI))
			Renderer::probeGrid.Bake(SceneManager::GetCurrentScene()->g_lights);
	}
	void CleanUp() {
		Client::CleanUp();
	}
}
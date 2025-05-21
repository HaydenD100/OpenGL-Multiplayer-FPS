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


	void Init(GameNetworkingMode multiPlayerMode) {
		m_multiPlayerMode = multiPlayerMode;
		float startLoadTime = glfwGetTime();

		if (multiPlayerMode == MultiPlayer) {
			NetworkManager::Init();
			//TODO :: myGui ConnectMenu
			std::cout << "==================================CONNECT/HOST=========================================================================\n";
			std::cout << "ENTER 0 to create/host a Game OR Type the IP of the server to join:";
			char temp[255];
			std::cin.getline(temp, sizeof(temp));
			std::cout << "If you are hosting/creating a game for others outside of your network to join you remember to port forward PORT: " << DEFAULT_PORT << "\n";
			std::cout << "=======================================================================================================================\n";

			if (temp[0] == '0')
				NetworkManager::InitServer();
			else
				NetworkManager::InitClient(temp);
		}

		AssetManager::Init();
		SceneManager::Init();
		SceneManager::CreateScene(Scene());
		SceneManager::LoadScene(0);
		WeaponManager::Init();
		AssetManager::LoadAssets();

		Player::Init();
		Player::setPosition(glm::vec3(0, 10, 0));
		Animator::Init();

		if (multiPlayerMode == MultiPlayer) {
			NetworkManager::LoadedIn();
			NetworkManager::SendControl(CONNECTED);
			NetworkManager::SendPackets();
			PlayerTwo::Init();
		}
		std::cout << "Scene and Asset Load took " << (glfwGetTime() - startLoadTime) << "s \n";
	}
	void Update(float dt) {
		CheckDebugPress();
	
		if(m_multiPlayerMode == MultiPlayer)
			NetworkManager::EvaulatePackets();

		SceneManager::Update(dt);
		Player::Update(dt);
		Camera::Update(dt);
			
		
		Animator::UpdateAnimation(dt);
		AudioManager::UpdateListener(Player::getPosition(), Player::getForward(), Player::getForward());

		if (m_multiPlayerMode == MultiPlayer)
			NetworkManager::SendPlayerData(Player::getPosition(), glm::vec3(-Camera::GetVerticalAngle(), Camera::GetHorizontalAngle(), 0), Player::getCurrentGun(), Player::GetInteractingWithName());
		//Host keeps track of all the physics objects 
		if (NetworkManager::IsServer && m_multiPlayerMode == MultiPlayer) {
			for (int i = 0; i < SceneManager::GetCurrentScene()->GetGameObjectsSize(); i++) {
				GameObject* gameobject = SceneManager::GetCurrentScene()->GetGameObject(i);
				if (!gameobject->IsDynamic() || gameobject->GetName() == "PlayerTwo" || gameobject->GetName() == "player")
					continue;

				NetworkManager::SendDyanmicObjectData(gameobject->GetName(), gameobject->GetPosition(), gameobject->getRotation(), btToGlmVector3(gameobject->GetRigidBody()->getLinearVelocity()));
			}
		}

		if (m_multiPlayerMode == MultiPlayer)
			NetworkManager::SendPackets();
	}
	void CheckDebugPress() {
		if (Input::KeyDown(RELOADSHADERS))
			Renderer::LoadAllShaders();
		if (Input::KeyDown(BAKEGI))
			Renderer::probeGrid.Bake(SceneManager::GetCurrentScene()->getLights());
	}
	void CleanUp() {
		NetworkManager::SendControl(DISCONNECTED);
		NetworkManager::CleanUp();
	}
}
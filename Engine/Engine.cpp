#include "Engine.h"
#include "Engine/Physics/BulletPhysics.h"
#include <ctime>
#include <iostream>
#include "Engine/Core/Networking/NetworkManager.h"




//rewrite of my first 3D Engine
//Not sure what im going to call it yet 
//this Engine is fueled by coffee and sleep deprivation
//https://github.com/HaydenD100

//Just a little note that some of the textures/models are from livinamuk

namespace Engine
{

	bool Editing = false;

	int Engine::Run() {


		std::filesystem::path cwd = std::filesystem::current_path();
		std::ofstream file(cwd.string());
		file.close();


		std::cout << "Currently in: " << cwd << "\n";

		NetworkManager::Init();
		std::cout << "==================================CONNECT/HOST==============================================================================\n";
		std::cout << "ENTER 0 to create/host a Game OR Type the IP of the server to join:";
		char temp[255];
		std::cin.getline(temp, sizeof(temp));
		std::cout << "If you are hosting/creating a game for others outside of your network to join you remember to port forward PORT: " << DEFAULT_PORT << "\n";
		std::cout << "============================================================================================================================\n";

		
		if (temp[0] == '0')
			NetworkManager::InitServer();
		else
			NetworkManager::InitClient(temp);


		

		//init Engine comps
		AssetManager::Init();

		Input::Init();
		Input::HideCursor();
		Text2D::initText2D("Assets/Fonts/Holstein.DDS");
		AudioManager::Init();
		PhysicsManagerBullet::Init();
		Scene basicScene = Scene();
		SceneManager::Init();
		SceneManager::CreateScene(basicScene);
		float startLoadTime = glfwGetTime();
		SceneManager::LoadScene(0);
		float endLoadTime = glfwGetTime() - startLoadTime;
		std::cout << "Load took " << endLoadTime << "s \n";
		//EditorManager::Init();

		// For speed computation
		double lastTimeDT = glfwGetTime();
		double previousTime = glfwGetTime();
		int frameCount = 0;
		int FPS = 0;

		PlayerTwo::Init();
		NetworkManager::LoadedIn();
		NetworkManager::SendControl(CONNECTED);
		NetworkManager::SendPackets();

		while (Backend::IsWindowOpen()) {

			// Delta time stuff
			double currentTime = glfwGetTime();
			double dt = currentTime - lastTimeDT;
			lastTimeDT = currentTime;
			frameCount++;

			if (currentTime - previousTime >= 1.0) { 
				FPS = frameCount;
				frameCount = 0;
				previousTime = currentTime;
			}
			// Update Managers
			Input::Update();

			//Reloads Shaders
			if (Input::KeyDown('h'))
				Renderer::LoadAllShaders();
				
			NetworkManager::EvaulatePackets();
				
			Input::CenterMouse();
			SceneManager::Update(dt);
			AnimationManager::Update(dt);
			Camera::Update(dt);
			//SceneManager::Render();
			Renderer::RenderScene();
			AudioManager::Update();
			PhysicsManagerBullet::Update(dt);


			//TODO :: put this in the scnene or somthing idk why its still here in ENGINE.cpp
			std::ostringstream oss;
			oss << "FPS: " << FPS;
			Renderer::RenderText(oss.str().c_str(), 660, 585, 15);
			oss.str(""); oss.clear();
			oss.precision(3);
			oss << Player::getPosition().x << " y:" << Player::getPosition().y << " z:" << Player::getPosition().z << "\n";
			Renderer::RenderText(oss.str().c_str(), 0, 560, 15);
			if (Player::getCurrentGun() != "nothing") {
				oss.str(""); oss.clear();
				oss.precision(4);
				oss << WeaponManager::GetGunByName(Player::getCurrentGun())->currentammo << "/" << WeaponManager::GetGunByName(Player::getCurrentGun())->ammo << "\n";
				Renderer::RenderText(oss.str().c_str(), 700, 60, 15);
			}

			oss.str(""); oss.clear();
			oss.precision(4);
			oss << Player::GetHealth() << "\n";
			Renderer::RenderText(oss.str().c_str(), 5, 20, 30);

			oss.str(""); oss.clear();
			oss.precision(4);
			oss << "P1:" << Player::GetKills() << " P2:" << Player::GetDeaths();
			Renderer::RenderText(oss.str().c_str(), 350, 560, 20);

			//AssetManager::CleanUp();

			
			Renderer::SwapBuffers(Backend::GetWindowPointer());
			
			//send Network Info
			NetworkManager::SendPlayerData(Player::getPosition(), glm::vec3(-Camera::GetVerticalAngle(), Camera::GetHorizontalAngle(), 0), Player::getCurrentGun(), Player::GetInteractingWithName());

			//Host keeps track of all the physics objects 
			if (NetworkManager::IsServer) {
				for (int i = 0; i < AssetManager::GetGameObjectsSize(); i++) {
					GameObject* gameobject = AssetManager::GetGameObject(i);
					if (!gameobject->IsDynamic() || gameobject->GetName() == "PlayerTwo" || gameobject->GetName() == "player")
						continue;

					NetworkManager::SendDyanmicObjectData(gameobject->GetName(), gameobject->GetPosition(), gameobject->getRotation(),btToGlmVector3(gameobject->GetRigidBody()->getLinearVelocity()));

				}
			}
			
			NetworkManager::SendPackets();


			
			
		}
		NetworkManager::SendControl(DISCONNECTED);
		NetworkManager::CleanUp();
		return 0;
	}
}

#include "Engine.h"
#include "Engine/Physics/BulletPhysics.h"
#include "Editor/EditorManager.h"


//rewrite of my first 3D Engine
//Not sure what im going to call it yet 
//this Engine is fueled by coffee and sleep deprivation
//https://github.com/HaydenD100

//Just a little note that some of the textures/models are from livinamuk

namespace Engine
{

	bool Editing = false;

	int Engine::Run() {

		//Im g
		/*
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(Backend::GetWindowPointer(), true);
		ImGui_ImplOpenGL3_Init("#version 330");
		*/

		//init Engine comps
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
		EditorManager::Init();

		// For speed computation
		double lastTimeDT = glfwGetTime();
		double previousTime = glfwGetTime();
		int frameCount = 0;
		int FPS = 0;

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
			//Renderer::ClearScreen();
			if (Editing)
			{
				EditorManager::Update();
			}
			else {
				
				Input::CenterMouse();
				SceneManager::Update(dt);
				AnimationManager::Update(dt);
				Camera::Update(dt);
				//SceneManager::Render();
				Renderer::RenderScene();
				AudioManager::Update();
				PhysicsManagerBullet::Update(dt);
				std::ostringstream oss;
				oss << "FPS: " << FPS;
				Renderer::RenderText(oss.str().c_str(), 660, 585, 15);
				oss.str(""); oss.clear();
				oss.clear();
				
				oss << "Position: " << Player::getPosition().x << " y:" << Player::getPosition().y << " z:" << Player::getPosition().z << "\n";
				Renderer::RenderText(oss.str().c_str(), 0, 560, 15);
				//AssetManager::CleanUp();
			}
			Renderer::SwapBuffers(Backend::GetWindowPointer());

			
		}

		return 0;
	}
}

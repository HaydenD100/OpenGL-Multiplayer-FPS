#include "Engine.h"
#include "Engine/Physics/BulletPhysics.h"

//rewrite of my first 3D Engine
//Not sure what im going to call it yet 
//this Engine is fueled by coffee and sleep deprivation
//https://github.com/HaydenD100

namespace Engine
{
	int Engine::Run() {
		//init Engine comps
		Input::Init();
		Input::HideCursor();
		Text2D::initText2D("Assets/Fonts/Holstein.DDS");
		AudioManager::Init();
		PhysicsManagerBullet::Init();

		Scene basicScene = Scene();
		SceneManager::Init();
		SceneManager::CreateScene(basicScene);
		SceneManager::LoadScene(0);

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
			Input::CenterMouse();
			Renderer::ClearScreen();
			SceneManager::Update(dt);
			AnimationManager::Update(dt);
			Camera::Update(dt); 
			SceneManager::Render();
			AudioManager::Update();
			PhysicsManagerBullet::Update(dt);
			std::ostringstream oss;
			oss << "FPS: " << FPS;
			Renderer::RenderText(oss.str().c_str(), 660, 585, 15);
			Renderer::SwapBuffers(Backend::GetWindowPointer());
			//AssetManager::CleanUp();
		}

		return 0;
	}
}

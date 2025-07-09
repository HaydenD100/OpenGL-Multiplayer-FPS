#include "Engine.h"
#include <ctime>
#include <iostream>

#include "Backend.h"
#include "Engine/Renderer/Raycaster.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/Particle.h"
#include "Engine/Audio/Audio.h"
#include "Engine/Core/UI/Text2D.h"
#include "Engine/Core/Input.h"
#include "Engine/Physics/BulletPhysics.h"
#include "Engine/Game/Game.h"
#include "Engine/Game/Player.h"
#include "Engine/Core/UI/UI.h"
#include "Engine/Editor/Editor.h"


//rewrite of my first 3D Engine
//Not sure what im going to call it yet 
//this Engine is fueled by coffee and sleep deprivation
//https://github.com/HaydenD100

namespace Engine
{

	bool Editing = false;

	int Engine::Run() {

		Backend::init();
		Input::Init();
		Text2D::initText2D("Assets/Fonts/Holstein.DDS");
		AudioManager::Init();
		PhysicsManagerBullet::Init();
		Game::Init(Game::SinglePlayer);
		Input::HideCursor();


		double lastTimeDT = glfwGetTime();
		double previousTime = glfwGetTime();
		int frameCount = 0;
		int FPS = 0;

		//SoftwareRaycaster::Init();
		//SoftwareRaycaster::FillBuffers();

		Renderer::BeforeRender();

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
			if (Input::KeyPressed(EDITORTOGGLE)) {
				Editing = !Editing;
				ImGuiIO& io = ImGui::GetIO();
				
				if (Editing) 
					Input::ShowCursor();
				else 
					Input::HideCursor();
			}
				
			// Update Managers
			ImGuiUI::NewFrame();

			Input::Update();
			
			AudioManager::Update();
			if (!Editing) {
				Input::CenterMouse();
				PhysicsManagerBullet::Update(dt);
				Game::Update(dt);
			}
			else {
				Editor::Update();
				Editor::RenderUI();
			}

			Renderer::RenderScene(dt);

			//TODO :: put this in the scene or somthing idk why its still here in ENGINE.cpp
			// TDOO :: acctualy I want to make a UI class, and also change the UI shader this is really old when i was first learning
			// OpenGL, so it needs to be updated, and maybe just replaced with myGUI

			//Dont use this its not working rightnow
			

			
			if ((Renderer::DebugState & NoGUi) != NoGUi) {

				std::ostringstream oss;
				oss << "FPS: " << FPS;
				Renderer::RenderText(oss.str().c_str(), 660, 585, 15);
				oss.str(""); oss.clear();
				oss.precision(4);
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
				oss << "Player Kills " << Player::GetKills();
				Renderer::RenderText(oss.str().c_str(), 0, 530, 15);
				oss.str(""); oss.clear();
				oss.precision(4);
				oss << "Enemy Kills " << Player::GetDeaths();
				Renderer::RenderText(oss.str().c_str(), 0, 500, 15);
			}
			
			
			
			ImGuiUI::DrawUI();

			Renderer::SwapBuffers(Backend::GetWindowPointer());
		}

		PhysicsManagerBullet::Delete();
		return 0;
	}
}

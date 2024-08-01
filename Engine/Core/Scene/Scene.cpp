#pragma once
#include "Scene.h"

Scene::Scene() {
	
}

void Scene::Load() {
	AssetManager::AddTexture("uvmap", "Assets/Textures/uvmap.png");
	AssetManager::AddTexture("crate", "Assets/Textures/crate.png");

	AssetManager::AddTexture("target", "Assets/Textures/target.jpeg");
	AssetManager::AddTexture("container", "Assets/Textures/Container.png", "Assets/Normals/container_normal.png");
	AssetManager::AddTexture("bullet_hole", "Assets/Textures/bullet_hole.png");
	AssetManager::AddTexture("bullet_hole_glass", "Assets/Textures/bullet_hole_glass.png");
	AssetManager::AddTexture("sand", "Assets/Textures/sandyGround.png","Assets/Normals/sand_normal.png");

	AssetManager::AddTexture("concrete", "Assets/Textures/fence.png","Assets/Normals/fence_normal.png");
	AssetManager::AddTexture("ak47_lowpoly", "Assets/Textures/ak47_lowpoly.png", "Assets/Normals/ak47_lowpoly_normal.png");
	AssetManager::AddTexture("crosshair", "Assets/Sprites/CrossHair.png", "Assets/Normals/ak47_lowpoly_normal.png");
	AssetManager::AddTexture("window", "Assets/Textures/window.png");
	
	AssetManager::AddTexture("glock", "Assets/Textures/glock_17.png", "Assets/Normals/glock_17_normal.png");
	AssetManager::AddTexture("door2", "Assets/Textures/Door_C.jpg");

	// TODO: not currently working
	//AssetManager::LoadAssets("Assets/Saves/mainScene.json");

	WeaponManager::Init();

	AssetManager::AddGameObject("fence1", "Assets/Objects/fence3.obj", AssetManager::GetTexture("concrete"), glm::vec3(5, 1.3, 2), true, 0, Box);
	AssetManager::AddGameObject("fence2", "Assets/Objects/fence1.obj", AssetManager::GetTexture("concrete"), glm::vec3(-9, 1.3, 2), true, 0, Box);
	AssetManager::AddGameObject("fence3", "Assets/Objects/fence2.obj", AssetManager::GetTexture("concrete"), glm::vec3(2, 1.3, -9), true, 0, Box);
	AssetManager::AddGameObject("fence4", "Assets/Objects/fence2.obj", AssetManager::GetTexture("concrete"), glm::vec3(-1, 1.3, 5), true, 0, Box);
	AssetManager::AddGameObject("floor", "Assets/Objects/test_platform.obj", AssetManager::GetTexture("sand"), glm::vec3(0, -2, 0), true, 0, Box);
	AssetManager::AddGameObject("floor", "Assets/Objects/slope.obj", AssetManager::GetTexture("sand"), glm::vec3(-1, 2, -7), true, 0, Convex);

	crates.push_back(Crate(glm::vec3(1, 25, 1), "crate2", "Assets/Objects/Crate.obj", AssetManager::GetTexture("crate")));
	crates.push_back(Crate(glm::vec3(1, 30, 0.5), "crate2", "Assets/Objects/Crate.obj", AssetManager::GetTexture("crate")));
	crates.push_back(Crate(glm::vec3(0.5, 20, 1), "crate2", "Assets/Objects/Crate.obj", AssetManager::GetTexture("crate")));



	gunPickUps.push_back(GunPickUp("ak47", "ak47_pickup", "Assets/Objects/ak47.obj", AssetManager::GetTexture("ak47"), glm::vec3(1, 30, 1)));


	gunPickUps.push_back(GunPickUp("glock", "glock_pickup1", "Assets/Objects/glock_17.obj", AssetManager::GetTexture("glock"), glm::vec3(1,25, 0)));

	doors.push_back(Door("door1", "Assets/Objects/door2.obj", "Assets/Objects/frame2.obj", AssetManager::GetTexture("door2"), AssetManager::GetTexture("door2"), glm::vec3(-3, 0, -3)));

	//gunPickUps.push_back(GunPickUp("ak47", "ak47_pickup", "Assets/Objects/ak47_lowpoly.obj", AssetManager::GetTexture("ak47_lowpoly"), glm::vec3(8, -12, -5)));
	//AssetManager::GetGameObject("ak47_pickup")->SetRender(false);
	//gunPickUps.push_back(GunPickUp("glock", "glock_pickup", "Assets/Objects/glock.obj", AssetManager::GetTexture("uvmap"), glm::vec3(8, -13, -6)));
	//AssetManager::GetGameObject("glock_pickup")->SetRender(false);

	// Sets renderer
	Renderer::UseProgram(Renderer::GetProgramID("Texture"));
	std::vector<std::string> faces{ 
		"Assets/Skybox/daylight/right.png",
			"Assets/Skybox/daylight/left.png",
			"Assets/Skybox/daylight/top.png",
			"Assets/Skybox/daylight/bottom.png",
			"Assets/Skybox/daylight/front.png",
			"Assets/Skybox/daylight/back.png"
	};
	sky = SkyBox(faces);


	// MAX LIGHTS BY DEFAULT IS 10 if you want more lights go to FragmentShader.frag and VertexShader.vert and change MAXLIGHTS
	{
		Light light(glm::vec3(-2.5, 4, -5), glm::vec3(1, 0.25, 0), 30);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(-6, 2, -2), glm::vec3(1, 0, 1), 30);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(-1, 2, -1), glm::vec3(0, 1, 1), 30);
		lights.push_back(light);
	}

	Player::Init();
	Player::setPosition(glm::vec3(3, 10, 0));

	// TODO: not currently working
	//AssetManager::SaveAssets("Assets/Saves/mainScene.json");
}

void Scene::Update(float deltaTime) {
	Player::Update(deltaTime);

	for (int i = 0; i < AssetManager::GetGameObjectsSize(); i++) {

		AssetManager::GetGameObject(i)->Update();
	}
	for (int door = 0; door < doors.size(); door++) {
		doors[door].Interact();
		doors[door].Update(deltaTime);
	}
	for (int gun = 0; gun < gunPickUps.size(); gun++) {
		gunPickUps[gun].Update();
		if (gunPickUps[gun].Interact() && Player::getCurrentGun() == "nothing")
			gunPickUps.erase(gunPickUps.begin() + gun);
	}
	for (int crate = 0; crate < crates.size(); crate++) {
		crates[crate].Update();
	}

	
	AudioManager::UpdateListener(Player::getPosition(),Player::getForward(),PhysicsManager::GetRigidbody("PlayerRB")->GetForce());
	AudioManager::Update();
}

void Scene::RenderObjects() {
	glm::mat4 ProjectionMatrix = Camera::getProjectionMatrix();
	glm::mat4 ViewMatrix = Camera::getViewMatrix();
	glm::mat4 PV = ProjectionMatrix * ViewMatrix;

	Renderer::RendererSkyBox(ViewMatrix, ProjectionMatrix, sky);
	Renderer::UseProgram(Renderer::GetProgramID("Texture"));
	
	GLuint programid = Renderer::GetCurrentProgramID();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Renderer::SetLights(lights);

	for (int i = 0; i < AssetManager::GetGameObjectsSize(); i++) {

		GameObject* gameobjectRender = AssetManager::GetGameObject(i);

		if (!gameobjectRender->ShouldRender())
			continue;

		glm::mat4 ModelMatrix = gameobjectRender->GetModelMatrix();
		glm::mat4 MVP = PV * ModelMatrix;
		glm::mat3 ModelView3x3Matrix = glm::mat3(ViewMatrix * ModelMatrix); // Take the upper-left part of ModelViewMatrix

		Renderer::SetTextureShader(MVP, ModelMatrix, ViewMatrix, ModelView3x3Matrix);
		gameobjectRender->RenderObject(programid);
	}

	for (int i = 0; i < windows.size(); i++)
	{
		windows[i].Render(Renderer::GetProgramID("Texture"), ViewMatrix, ProjectionMatrix);
	}

	for (int i = 0; i < AssetManager::GetDecalsSize(); i++)
	{
		Decal* decal = AssetManager::GetDecal(i);
		if (decal->CheckParentIsNull())
			continue;
		// Do some pre-normal calculations
		glm::mat4 ModelMatrix = decal->GetModel();
		glm::mat4 MVP = PV * ModelMatrix;
		glm::mat3 ModelView3x3Matrix = glm::mat3(ViewMatrix * ModelMatrix); // Take the upper-left part of ModelViewMatrix
		Renderer::SetTextureShader(MVP, ModelMatrix, ViewMatrix, ModelView3x3Matrix);
		decal->RenderDecal(programid);
	}

	glDisable(GL_BLEND);
	std::ostringstream oss;
	oss.precision(2);
	glm::vec3 pos = btToGlmVector3(AssetManager::GetGameObject("player")->GetRigidBody()->getWorldTransform().getOrigin());
	oss << "Pos x:" << pos.x << " y:" << pos.y << " z:" << pos.z;
	Renderer::RenderText(oss.str().c_str(), 0, 570, 15);
	glm::vec3 vel = btToGlmVector3(AssetManager::GetGameObject("player")->GetRigidBody()->getLinearVelocity());
	oss.str(""); oss.clear();
	oss.precision(2);
	oss << "Vel x:" << vel.x << " y:" << vel.y << " z:" << vel.z;
	Renderer::RenderText(oss.str().c_str(), 0, 540, 15);
	if (Player::getCurrentGun() != "nothing")
	{
		oss.str(""); oss.clear();
		oss << WeaponManager::GetGunByName(Player::getCurrentGun())->currentammo << "/" << WeaponManager::GetGunByName(Player::getCurrentGun())->ammo;
		Renderer::RenderText(oss.str().c_str(), 660, 0, 15);
	}
	oss.str(""); oss.clear();
	oss << Player::getCurrentGun();
	Renderer::RenderText(oss.str().c_str(), 0, 500, 15);
	
}
void Scene::AddGunPickUp(GunPickUp gunpickup) {
	gunPickUps.push_back(gunpickup);
}
void Scene::AddGunPickUp(std::string gunName, std::string gunObject, glm::vec3 Position) {
	GunPickUp pickup = GunPickUp(gunName, gunObject, Position);
	std::cout << " testing";
	gunPickUps.push_back(pickup);
}
void Scene::RenderObjects(const char* shaderName) {
	glm::mat4 ProjectionMatrix = Camera::getProjectionMatrix();
	glm::mat4 ViewMatrix = Camera::getViewMatrix();
	glm::mat4 PV = ProjectionMatrix * ViewMatrix;

	Renderer::RendererSkyBox(ViewMatrix, ProjectionMatrix, sky);
	GLuint programid = Renderer::GetProgramID(shaderName);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Renderer::SetLights(lights);

	for (int i = 0; i < AssetManager::GetGameObjectsSize(); i++) {

		GameObject* gameobjectRender = AssetManager::GetGameObject(i);

		if (!gameobjectRender->ShouldRender())
			continue;

		glm::mat4 ModelMatrix = gameobjectRender->GetModelMatrix();
		glm::mat4 MVP = PV * ModelMatrix;
		glm::mat3 ModelView3x3Matrix = glm::mat3(ViewMatrix * ModelMatrix); // Take the upper-left part of ModelViewMatrix

		Renderer::SetTextureShader(MVP, ModelMatrix, ViewMatrix, ModelView3x3Matrix);
		gameobjectRender->RenderObject(programid);
	}
}


int Scene::GetGunPickUpSize() {
	return gunPickUps.size();
}
Crate* Scene::GetCrate(std::string name) {
	for (int i = 0; i < crates.size(); i++) {
		if (crates[i].GetName() == name)
			return &crates[i];
	}
	return nullptr;
}




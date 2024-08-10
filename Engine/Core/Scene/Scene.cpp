#include "Scene.h"

Scene::Scene() {

}

void Scene::Load() {
	AssetManager::AddTexture("uvmap", "Assets/Textures/uvmap.png");
	AssetManager::AddTexture("crate", "Assets/Textures/crate.png");
	AssetManager::AddTexture("bullet_hole", "Assets/Textures/bullet_hole.png");
	AssetManager::AddTexture("sand", "Assets/Textures/sandyGround.png", "Assets/Normals/sand_normal.png");
	AssetManager::AddTexture("concrete", "Assets/Textures/fence.png", "Assets/Normals/fence_normal.png");
	AssetManager::AddTexture("glock", "Assets/Textures/glock_17.png", "Assets/Normals/glock_17_normal.png");
	AssetManager::AddTexture("door2", "Assets/Textures/Door_C.jpg");
	AssetManager::AddTexture("ak47", "Assets/Textures/ak47.png", "Assets/Normals/ak47_normal.png");
	AssetManager::AddTexture("drawer", "Assets/Textures/drawerred.png", "Assets/Normals/drawer_normal.png");
	AssetManager::AddTexture("lamp", "Assets/Textures/lamp.png", "Assets/Normals/lamp_normal.png");


	Animation anim = Animation("Assets/Animations/cube_test_anim.fbx", "cube");
	AnimationManager::AddAnimation(anim);


	// TODO: not currently working
	//AssetManager::LoadAssets("Assets/Saves/mainScene.json");

	//Loads Models that then can 
	models["fence1"] = Model("Assets/Objects/fence1.fbx", AssetManager::GetTexture("concrete"));
	models["fence2"] = Model("Assets/Objects/fence2.fbx", AssetManager::GetTexture("concrete"));
	models["fence3"] = Model("Assets/Objects/fence3.fbx", AssetManager::GetTexture("concrete"));
	models["floor"] = Model("Assets/Objects/FBX/floor.fbx", AssetManager::GetTexture("sand"));
	models["slope"] = Model("Assets/Objects/FBX/slope.fbx", AssetManager::GetTexture("sand"));
	models["crate"] = Model("Assets/Objects/FBX/crate.fbx", AssetManager::GetTexture("crate"));
	models["glock"] = Model("Assets/Objects/FBX/glock17.fbx", "Assets/Objects/glock17_convex.obj", AssetManager::GetTexture("glock"));
	models["ak47"] = Model("Assets/Objects/FBX/ak47.fbx", "Assets/Objects/ak47_convex.obj", AssetManager::GetTexture("ak47"));
	models["door"] = Model("Assets/Objects/FBX/door2.fbx", AssetManager::GetTexture("door2"));
	models["door_frame"] = Model("Assets/Objects/FBX/frame2.fbx", AssetManager::GetTexture("door2"));
	models["player"] = Model("Assets/Objects/FBX/player.fbx", AssetManager::GetTexture("uvmap"));
	//models["lamp"] = Model("Assets/Objects/FBX/lamp.fbx", "Assets/Objects/lamp_convex.obj", AssetManager::GetTexture("lamp"));


	models["wooden_floor"] = Model("Assets/Objects/Map1/floor.fbx", AssetManager::GetTexture("uvmap"));
	models["map"] = Model("Assets/Objects/Map1/map1.fbx", AssetManager::GetTexture("uvmap"));
	//models["map1"] = Model("Assets/Objects/Map1/map1.fbx", AssetManager::GetTexture("uvmap"));


	//models["drawer"] = Model(Mesh("Assets/Objects/drawer.obj"), AssetManager::GetTexture("drawer"));

	WeaponManager::Init();


	//AssetManager::AddGameObject("fence1", &models["fence3"], glm::vec3(5, 1.3, 2), true, 0, Box);
	//AssetManager::AddGameObject("fence2", &models["fence1"], glm::vec3(-9, 1.3, 2), true, 0, Box);
	//AssetManager::AddGameObject("fence3", &models["fence2"], glm::vec3(2, 1.3, -9), true, 0, Box);
	//AssetManager::AddGameObject("fence4", &models["fence2"], glm::vec3(-1, 1.3, 5), true, 0, Box);
	AssetManager::AddGameObject("map1", &models["map"], glm::vec3(0, 1.85, 0), true, 0, Concave);

	AssetManager::AddGameObject("floor", &models["floor"], glm::vec3(0, 0, 0), true, 0, Convex);

	//AssetManager::AddGameObject("lamp", &models["lamp"], glm::vec3(1.6, 2, 2), true, 2.0f, Convex);
	/*
	{
		Light light(glm::vec3(-2.5, 4, -5), glm::vec3(1, 0.779, 0.529), 1.0, 0.7, 1.8);
		lights.push_back(light);
	}
	*/

	//AssetManager::AddGameObject("Map1", &models["map1"], glm::vec3(0, 2, 0), true, 0.0f, Convex);
	//AssetManager::AddGameObject("drawer", &models["drawer"], glm::vec3(0.3, 1, 4.4), true, 0.0f, Box);
	//AssetManager::GetGameObject("drawer")->SetRotationY(1.5708f);




	crates.push_back(Crate(glm::vec3(1, 25, 1), "crate1", &models["crate"]));
	crates.push_back(Crate(glm::vec3(1, 30, 0.5), "crate2", &models["crate"]));
	crates.push_back(Crate(glm::vec3(0.5, 20, 1), "crate3", &models["crate"]));


	gunPickUps.push_back(GunPickUp("ak47", "ak47_pickup", &models["ak47"], glm::vec3(1, 30, 1)));
	gunPickUps.push_back(GunPickUp("glock", "glock_pickup", &models["glock"], glm::vec3(1, 25, 0)));

	doors.push_back(Door("door1", &models["door"], &models["door_frame"], glm::vec3(-3, 0, -3)));



	// Sets renderer
	Renderer::UseProgram(Renderer::GetProgramID("Texture"));
	std::vector<std::string> faces{
		"Assets/Skybox/Space/right.png",
			"Assets/Skybox/Space/left.png",
			"Assets/Skybox/Space/top.png",
			"Assets/Skybox/Space/bottom.png",
			"Assets/Skybox/Space/front.png",
			"Assets/Skybox/Space/back.png"
	};
	sky = SkyBox(faces);

	// MAX LIGHTS BY DEFAULT IS 10 if you want more lights go to FragmentShader.frag and VertexShader.vert and change MAXLIGHTS
	{
		Light light(glm::vec3(-2.5, 4, -5), glm::vec3(1, 0.25, 0), 1, 0.22, 0.20);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(-6, 2, -2), glm::vec3(1, 0, 1), 1, 0.22, 0.20);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(-1, 2, -1), glm::vec3(0, 1, 1), 1, 0.22, 0.20);
		lights.push_back(light);
	}

	Player::Init();
	Player::setPosition(glm::vec3(0, 10, 0));

	// TODO: not currently working
	//AssetManager::SaveAssets("Assets/Saves/mainScene.json");


}

void Scene::Update(float deltaTime) {
	//just a little hack to test out lamps
	//lights[0].position = AssetManager::GetGameObject("lamp")->getPosition();
	Player::Update(deltaTime);

	if (Input::KeyDown('k')) {
		AnimationManager::Play("cube", "cube1");
	}

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

	AudioManager::UpdateListener(Player::getPosition(), Player::getForward(), PhysicsManager::GetRigidbody("PlayerRB")->GetForce());
	AudioManager::Update();
}

void Scene::RenderObjects() {
	glm::mat4 ProjectionMatrix = Camera::getProjectionMatrix();
	glm::mat4 ViewMatrix = Camera::getViewMatrix();
	glm::mat4 PV = ProjectionMatrix * ViewMatrix;

	Renderer::RendererSkyBox(ViewMatrix, ProjectionMatrix, sky);
	Renderer::UseProgram(Renderer::GetProgramID("Texture"));

	GLuint programid = Renderer::GetCurrentProgramID();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
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

	for (int i = 0; i < AssetManager::GetDecalsSize(); i++) {
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
	oss.str("");
	oss.clear();
	oss.precision(2);
	oss << "Vel x:" << vel.x << " y:" << vel.y << " z:" << vel.z;
	Renderer::RenderText(oss.str().c_str(), 0, 540, 15);

	oss.str(""); oss.clear();
	oss << "Current Weapon: " << Player::getCurrentGun();
	Renderer::RenderText(oss.str().c_str(), 0, 500, 15);
}

void Scene::AddGunPickUp(GunPickUp gunpickup) {
	gunPickUps.push_back(gunpickup);
}


void Scene::RenderObjects(const char* shaderName) {
	std::cout << "red";
	glm::mat4 ProjectionMatrix = Camera::getProjectionMatrix();
	glm::mat4 ViewMatrix = Camera::getViewMatrix();
	glm::mat4 PV = ProjectionMatrix * ViewMatrix;

	Renderer::RendererSkyBox(ViewMatrix, ProjectionMatrix, sky);
	GLuint programid = Renderer::GetProgramID(shaderName);

	Renderer::SetLights(lights);

	for (int i = 0; i < AssetManager::GetGameObjectsSize(); i++) {
		GameObject* gameobjectRender = AssetManager::GetGameObject(i);

		if (!gameobjectRender->ShouldRender())
			continue;

		glm::mat4 ModelMatrix = gameobjectRender->GetModelMatrix();

		Renderer::setMat4(glGetUniformLocation(Renderer::GetCurrentProgramID(), "model"), ModelMatrix);
		//Renderers model
		gameobjectRender->RenderObject(programid);
	}
}

//had to change back to int instead of size_t as it was giving me errors with string sizes
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
Model* Scene::GetModel(std::string name) {
	return &models[name];
}
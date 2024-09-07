#include "Scene.h"

Scene::Scene() {

}

void Scene::Load() { 
	AssetManager::AddTexture("uvmap", "Assets/Textures/uvmap.png",0,0);
	AssetManager::AddTexture("red_glass", "Assets/Textures/red_glass.png", 0, 0);

	AssetManager::AddTexture("crate", "Assets/Textures/crate.png",0.7,0);
	AssetManager::AddTexture("bullet_hole", "Assets/Textures/bullet_hole.png",0.5,0);
	AssetManager::AddTexture("sand", "Assets/Textures/sandyGround.png", "Assets/Normals/sand_normal.png",0.9,0);
	AssetManager::AddTexture("concrete", "Assets/Textures/fence.png", "Assets/Normals/fence_normal.png",0.9,0);
	AssetManager::AddTexture("glock", "Assets/Textures/glock_17.png", "Assets/Normals/glock_17_normal.png",0.5,0.5);
	AssetManager::AddTexture("door", "Assets/Textures/door.png","Assets/Normals/door_normal.png",0.6,0);
	AssetManager::AddTexture("ak47", "Assets/Textures/ak47.png", "Assets/Normals/ak47_normal.png",0.4,0.4);
	AssetManager::AddTexture("drawer", "Assets/Textures/drawerred.png", "Assets/Normals/drawer_normal.png",0.7,0);
	AssetManager::AddTexture("lamp", "Assets/Textures/lamp.png", "Assets/Normals/lamp_normal.png",0.7,0.1);
	AssetManager::AddTexture("beige_wall", "Assets/Textures/beige_wall.jpg", "Assets/Normals/beige_wall_normal.jpg",0.7,0);
	AssetManager::AddTexture("wooden_floor", "Assets/Textures/wooden_floor.jpg", "Assets/Normals/wooden_floor_normal.jpg",0.2,0.2);

	// TODO: not currently working
	//AssetManager::LoadAssets("Assets/Saves/mainScene.json");

	//Loads Mode
	models["fence1"] = Model("Assets/Objects/fence1.fbx", AssetManager::GetTexture("concrete"));
	models["fence2"] = Model("Assets/Objects/fence2.fbx", AssetManager::GetTexture("concrete"));
	models["fence3"] = Model("Assets/Objects/fence3.fbx", AssetManager::GetTexture("concrete"));
	models["floor"] = Model("Assets/Objects/FBX/floor.fbx", AssetManager::GetTexture("sand"));
	models["slope"] = Model("Assets/Objects/FBX/slope.fbx", AssetManager::GetTexture("sand"));
	models["crate"] = Model("Assets/Objects/FBX/crate.fbx", AssetManager::GetTexture("crate"));
	models["cube"] = Model("Assets/Objects/FBX/cube.fbx", AssetManager::GetTexture("red_glass"));

	models["glock"] = Model("Assets/Objects/FBX/glock17.fbx", "Assets/Objects/glock17_convex.obj", AssetManager::GetTexture("glock"));
	models["ak47"] = Model("Assets/Objects/FBX/ak47.fbx", "Assets/Objects/ak47_convex.obj", AssetManager::GetTexture("ak47"));
	models["door"] = Model(Mesh("Assets/Objects/door.obj"), AssetManager::GetTexture("door"));
	models["door_frame"] = Model(Mesh("Assets/Objects/door_frame.obj"), AssetManager::GetTexture("door"));
	models["player"] = Model("Assets/Objects/FBX/player.fbx", AssetManager::GetTexture("uvmap"));
	models["map_floor"] = Model("Assets/Objects/Map1/floors.fbx", AssetManager::GetTexture("wooden_floor"));
	models["map_walls"] = Model("Assets/Objects/Map1/walls.fbx", AssetManager::GetTexture("beige_wall"));
	models["map_ceiling"] = Model("Assets/Objects/Map1/ceiling.fbx", AssetManager::GetTexture("beige_wall"));


	AnimationManager::AddAnimation(Animation("Assets/Objects/FBX/ak47.fbx", "ak47_reload"));
	AnimationManager::AddAnimation(Animation("Assets/Animations/door_open.fbx", "door_open"));
	AnimationManager::AddAnimation(Animation("Assets/Animations/door_close.fbx", "door_close"));


	WeaponManager::Init();

	AssetManager::AddGameObject("map1_floor", &models["map_floor"], glm::vec3(0, 1.6, 0), true, 0, Concave);
	AssetManager::AddGameObject("map1_walls", &models["map_walls"], glm::vec3(0, 1.6, 0), true, 0, Concave);
	AssetManager::AddGameObject("map1_ceiling", &models["map_ceiling"], glm::vec3(0, 1.6, 0), true, 0, Convex);

	AssetManager::AddGameObject("red_glass", &models["cube"], glm::vec3(-7, 2, 6), true, 0, Convex);
	AssetManager::GetGameObject("red_glass")->SetShaderType("Transparent");
	AssetManager::AddGameObject("floor", &models["floor"], glm::vec3(0, 0, 0), true, 0, Box);
	

	crates.push_back(Crate(glm::vec3(1, 10, 1), "crate1", &models["crate"]));
	crates.push_back(Crate(glm::vec3(1, 12, 0.5), "crate2", &models["crate"])); 
	crates.push_back(Crate(glm::vec3(-10, 2, 14), "crate3", &models["crate"]));


	gunPickUps.push_back(GunPickUp("ak47", "ak47_pickup", &models["ak47"], glm::vec3(1, 30, 1)));
	gunPickUps.push_back(GunPickUp("glock", "glock_pickup", &models["glock"], glm::vec3(1, 25, 0)));

	doors.push_back(Door("door1", &models["door"], &models["door_frame"], glm::vec3(-10.6, 0, 0.05)));
	doors.push_back(Door("door2", &models["door"], &models["door_frame"], glm::vec3(-10.6, 0, 9.95)));

	doors.push_back(Door("door3", &models["door"], &models["door_frame"], glm::vec3(-12.3, 0, 4.6)));
	AssetManager::GetGameObject("door3_door")->SetRotationY(1.5708f);
	AssetManager::GetGameObject("door3_frame")->SetRotationY(1.5708f);

	doors.push_back(Door("door4", &models["door"], &models["door_frame"], glm::vec3(-10, 0, 4.6)));
	AssetManager::GetGameObject("door4_door")->SetRotationY(1.5708f);
	AssetManager::GetGameObject("door4_frame")->SetRotationY(1.5708f);



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

	// MAX LIGHTS BY DEFAULT IS 60 if you want more lights go to lighting.frag and change MAXLIGHTS
	{
		Light light(glm::vec3(-11, 3, 15), glm::vec3(1, 0.779, 0.529) * 5.0f, 0.09, 0.032);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(-11, 3, 6), glm::vec3(1, 0.779, 0.529) * 5.0f, 0.09, 0.032);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(-15, 3, 5), glm::vec3(1, 0.25, 0) * 5.0f, 0.09, 0.032);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(-7.5, 3, 5), glm::vec3(1, 0.25, 0) * 5.0f, 0.09, 0.032);
		lights.push_back(light);
	}

	{
		Light light(glm::vec3(-2.5, 3, -5), glm::vec3(1, 0.25, 0) * 5.0f, 0.09, 0.0320);
		lights.push_back(light);
	}
	
	{
		Light light(glm::vec3(-6, 2, -2), glm::vec3(1, 0, 1) * 4.0f, 0.09, 0.0320);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(-1, 2, -1), glm::vec3(0, 1, 1) * 4.0f, 0.09, 0.0320);
		lights.push_back(light);
	}
	
	Player::Init();
	Player::setPosition(glm::vec3(0, 10, 0));

	// TODO: not currently working
	//AssetManager::SaveAssets("Assets/Saves/mainScene.json");


	ModelMatrixId = glGetUniformLocation(Renderer::GetCurrentProgramID(), "model");


	glBindVertexArray(sky.GetSkyBoxVAO());
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

	AudioManager::UpdateListener(Player::getPosition(), Player::getForward(), PhysicsManager::GetRigidbody("PlayerRB")->GetForce());
	AudioManager::Update();
}

void Scene::RenderObjects(GLuint programid) {
	NeedRendering.clear();

	glm::mat4 ViewMatrix = Camera::getViewMatrix();
	for (int i = 0; i < AssetManager::GetGameObjectsSize(); i++) {
		GameObject* gameobjectRender = AssetManager::GetGameObject(i);
		if (!gameobjectRender->ShouldRender())
			continue;

		if (gameobjectRender->GetShaderType() != "Default") {
			NeedRendering.push_back(gameobjectRender);
			continue;
		}

		glm::mat4 ModelMatrix = gameobjectRender->GetModelMatrix();
		glm::mat4 modelViewMatrix = ViewMatrix * ModelMatrix;
		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelViewMatrix)));

		glUniformMatrix3fv(glGetUniformLocation(programid, "normalMatrix3"), 1, GL_FALSE, &normalMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(programid, "M"), 1, GL_FALSE, &ModelMatrix[0][0]);

		gameobjectRender->RenderObject(programid);
	}
	/*
	
	for (int i = 0; i < AssetManager::GetDecalsSize(); i++) {
		Decal* decal = AssetManager::GetDecal(i);
		if (decal->CheckParentIsNull())
			continue;
		glm::mat4 ModelMatrix = decal->GetModel();
		Renderer::setMat4(glGetUniformLocation(Renderer::GetCurrentProgramID(), "M"), ModelMatrix);
		decal->RenderDecal(programid);
	}*/
}

void Scene::AddGunPickUp(GunPickUp gunpickup) {
	gunPickUps.push_back(gunpickup);
}
void Scene::RenderObjects(const char* shaderName) {
	glm::mat4 ProjectionMatrix = Camera::getProjectionMatrix();
	glm::mat4 ViewMatrix = Camera::getViewMatrix();
	glm::mat4 PV = ProjectionMatrix * ViewMatrix;

	GLuint programid = Renderer::GetProgramID(shaderName);

	Renderer::SetLights(lights);

	for (int i = 0; i < AssetManager::GetGameObjectsSize(); i++) {
		GameObject* gameobjectRender = AssetManager::GetGameObject(i);

		if (!gameobjectRender->ShouldRender())
			continue;

		glm::mat4 ModelMatrix = gameobjectRender->GetModelMatrix();

		Renderer::setMat4(ModelMatrixId, ModelMatrix);
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
std::vector<Light> Scene::getLights() {
	return lights;
}

SkyBox Scene::GetSkyBox() {
	return sky;
}
std::vector<GameObject*> Scene::NeedRenderingObjects() {
	return NeedRendering;
}

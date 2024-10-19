#include "Scene.h"

Scene::Scene() {

}

void Scene::LoadAssets() {
	AssetManager::ClearAssets();
	AnimationManager::ClearAnimations();
	//Thank you to tokyo splif for some of the models and sounds

	AssetManager::AddTexture("uvmap", "Assets/Textures/uvmap.png", 0, 0);
	AssetManager::AddTexture("red_glass", "Assets/Textures/red_glass.png", 0.1, 0.9);
	AssetManager::AddTexture("green_glass", "Assets/Textures/green_glass.png", 0.1, 0.9);
	AssetManager::AddTexture("panda", "Assets/Textures/panda_decal.png", 0, 0);
	AssetManager::AddTexture("flower", "Assets/Textures/flower_decal.png", 0, 0);
	AssetManager::AddTexture("freaky", "Assets/Textures/freaky_decal.png", 0, 0);
	AssetManager::AddTexture("pizza", "Assets/Textures/pizza_decal.png", 0, 0);
	AssetManager::AddTexture("tank", "Assets/Textures/tank_decal.png", 0, 0);
	AssetManager::AddTexture("crate", "Assets/Textures/crate.png", 0.7, 0);
	AssetManager::AddTexture("window", "Assets/Textures/window.png", 0.7, 0);
	AssetManager::AddTexture("bullet_hole", "Assets/Textures/bullet_hole.png","Assets/Normals/window_normal.png", 0.5, 0);
	AssetManager::AddTexture("sand", "Assets/Textures/sandyGround.png", "Assets/Normals/sand_normal.png", 0.9, 0);
	AssetManager::AddTexture("concrete", "Assets/Textures/fence.png", "Assets/Normals/fence_normal.png", 0.9, 0);
	AssetManager::AddTexture("glock", "Assets/Textures/glock_17.png", "Assets/Normals/glock_17_normal.png", 0.5, 0.5);
	AssetManager::AddTexture("door", "Assets/Textures/door.png", "Assets/Normals/door_normal.png", 0.6, 0);
	AssetManager::AddTexture("ak47", "Assets/Textures/ak47.png", "Assets/Normals/ak47_normal.png", 0.4, 0.4);
	AssetManager::AddTexture("drawer", "Assets/Textures/drawerred.png", "Assets/Normals/drawer_normal.png", 0.7, 0);
	AssetManager::AddTexture("lamp", "Assets/Textures/lamp.png", "Assets/Normals/lamp_normal.png", 0.7, 0.1);
	AssetManager::AddTexture("beige_wall", "Assets/Textures/beige_wall.jpg", "Assets/Normals/beige_wall_normal.jpg", 0.7, 0);
	AssetManager::AddTexture("wooden_floor", "Assets/Textures/wooden_floor.jpg", "Assets/Normals/wooden_floor_normal.jpg", 0.2, 0.2);
	AssetManager::AddTexture("shotgun", "Assets/Textures/remington.png", "Assets/Normals/remington_normal.png", "Assets/Roughness/remington_roughness.png","Assets/Metalic/remington_metallic.png");
	AssetManager::AddTexture("glass", "Assets/Textures/glass.png", 0.1, 1);


	// TODO: not currently working
	//AssetManager::LoadAssets("Assets/Saves/mainScene.json");
	//Loads Mode
	AssetManager::AddModel("anim_test", Model("Assets/Objects/FBX/glock17_shoot.dae", AssetManager::GetTexture("uvmap")));

	AssetManager::AddModel("window", Model("Assets/Objects/FBX/window.fbx", AssetManager::GetTexture("window")));
	AssetManager::AddModel("window_glass", Model("Assets/Objects/FBX/window_glass.fbx", AssetManager::GetTexture("glass")));


	AssetManager::AddModel("fence1", Model("Assets/Objects/fence1.fbx", AssetManager::GetTexture("concrete")));
	AssetManager::AddModel("fence2", Model("Assets/Objects/fence2.fbx", AssetManager::GetTexture("concrete")));
	AssetManager::AddModel("fence3", Model("Assets/Objects/fence3.fbx", AssetManager::GetTexture("concrete")));
	AssetManager::AddModel("floor", Model("Assets/Objects/FBX/floor.fbx", AssetManager::GetTexture("sand")));
	AssetManager::AddModel("slope", Model("Assets/Objects/FBX/slope.fbx", AssetManager::GetTexture("sand")));
	AssetManager::AddModel("crate", Model("Assets/Objects/FBX/crate.fbx", AssetManager::GetTexture("crate")));
	AssetManager::AddModel("cube", Model("Assets/Objects/FBX/cube.fbx", AssetManager::GetTexture("red_glass")));
	AssetManager::AddModel("cube1", Model("Assets/Objects/FBX/cube.fbx", AssetManager::GetTexture("green_glass")));

	AssetManager::AddModel("glock", Model("Assets/Objects/FBX/glock17.fbx", "Assets/Objects/glock17_convex.obj", AssetManager::GetTexture("glock")));
	AssetManager::AddModel("glockhand", Model("Assets/Objects/FBX/glock17_shoot.dae", AssetManager::GetTexture("glock")));
	AssetManager::GetModel("glockhand")->GetMeshByName("Arms_L_R_Mesh_002-mesh")->SetTexture(AssetManager::GetTexture("uvmap"));

	AssetManager::AddModel("ak47", Model("Assets/Objects/FBX/ak47.fbx", "Assets/Objects/ak47_convex.obj", AssetManager::GetTexture("ak47")));
	AssetManager::AddModel("door", Model(Mesh("Assets/Objects/door.obj"), AssetManager::GetTexture("door")));
	AssetManager::AddModel("door_frame", Model(Mesh("Assets/Objects/door_frame.obj"), AssetManager::GetTexture("door")));
	AssetManager::AddModel("player", Model("Assets/Objects/FBX/player.fbx", AssetManager::GetTexture("uvmap")));
	AssetManager::AddModel("map_floor", Model("Assets/Objects/Map1/floors.fbx", AssetManager::GetTexture("wooden_floor")));
	AssetManager::AddModel("map_walls", Model("Assets/Objects/Map1/walls.fbx", AssetManager::GetTexture("beige_wall")));
	AssetManager::AddModel("map_ceiling", Model("Assets/Objects/Map1/ceiling.fbx", AssetManager::GetTexture("beige_wall")));
	AssetManager::AddModel("shotgun", Model("Assets/Objects/fbx/remington.fbx", "Assets/Objects/shotgun_convex.obj", AssetManager::GetTexture("shotgun")));

	AssetManager::AddDecal("bullet_hole", AssetManager::GetTexture("bullet_hole"), glm::vec3(0.02, 0.005, 0.02));
	AssetManager::AddDecal("panda_decal", AssetManager::GetTexture("panda"), glm::vec3(1, 0.1, 1));
	AssetManager::AddDecal("flower_decal", AssetManager::GetTexture("flower"), glm::vec3(1, 0.1, 1));
	AssetManager::AddDecal("tank_decal", AssetManager::GetTexture("tank"), glm::vec3(1, 0.1, 1));
	AssetManager::AddDecal("pizza_decal", AssetManager::GetTexture("pizza"), glm::vec3(1, 0.1, 1));
	AssetManager::AddDecal("freaky_decal", AssetManager::GetTexture("freaky"), glm::vec3(1, 0.1, 1));

	AnimationManager::AddAnimation(Animation("Assets/Objects/FBX/ak47.fbx", "ak47_reload"));
	AnimationManager::AddAnimation(Animation("Assets/Animations/door_open.fbx", "door_open"));
	AnimationManager::AddAnimation(Animation("Assets/Animations/door_close.fbx", "door_close"));
}


void Scene::Load() { 
	LoadAssets();


	WeaponManager::Init();

	AssetManager::AddGameObject("anim_test1", AssetManager::GetModel("anim_test1"), glm::vec3(0, 1, -3), true, 0, None);
	AssetManager::AddGameObject(GameObject("glock1", AssetManager::GetModel("glockhand"), glm::vec3(5, 1, -5), false, 0, Convex));

	AssetManager::AddGameObject("map1_floor", AssetManager::GetModel("map_floor"), glm::vec3(0, 1.6, 0), true, 0, Concave);
	AssetManager::AddGameObject("map1_walls", AssetManager::GetModel("map_walls"), glm::vec3(0, 1.6, 0), true, 0, Concave);
	AssetManager::AddGameObject("map1_ceiling", AssetManager::GetModel("map_ceiling"), glm::vec3(0, 1.6, 0), true, 0, Convex);

	AssetManager::AddGameObject("window1", AssetManager::GetModel("window"), glm::vec3(-11, 2, 14),true, 0,Concave);
	AssetManager::AddGameObject("window_glass1", AssetManager::GetModel("window_glass"), glm::vec3(-11, 2, 14), true, 0, Concave);
	AssetManager::GetGameObject("window_glass1")->SetShaderType("Transparent");
	AssetManager::GetGameObject("window_glass1")->SetRotationX(1.5708f);


	AssetManager::AddGameObject("red_glass", AssetManager::GetModel("cube"), glm::vec3(-7, 2, 6), true, 0, Convex);
	AssetManager::GetGameObject("red_glass")->SetShaderType("Transparent");

	AssetManager::AddGameObject("red_glass1", AssetManager::GetModel("cube1"), glm::vec3(-6, 2, 8), true, 0, Convex);
	AssetManager::GetGameObject("red_glass1")->SetShaderType("Transparent");

	AssetManager::AddGameObject("floor", AssetManager::GetModel("floor"), glm::vec3(0, 0, 0), true, 0, Box);

	crates.push_back(Crate(glm::vec3(1, 2, 1), "crate1", AssetManager::GetModel("crate")));
	crates.push_back(Crate(glm::vec3(-3, 2, -3), "crate2", AssetManager::GetModel("crate")));
	crates.push_back(Crate(glm::vec3(-10, 2, 14), "crate3", AssetManager::GetModel("crate")));

	gunPickUps.push_back(GunPickUp("ak47", "ak47_pickup", AssetManager::GetModel("ak47"), glm::vec3(1, 30, 1)));
	gunPickUps.push_back(GunPickUp("glock", "glock_pickup", AssetManager::GetModel("glock"), glm::vec3(1, 25, 0)));

	
	doors.push_back(Door("door1", AssetManager::GetModel("door"), AssetManager::GetModel("door_frame"), glm::vec3(-10.6, 0, 0.05), glm::vec3(0, 0, 0)));
	doors.push_back(Door("door2", AssetManager::GetModel("door"), AssetManager::GetModel("door_frame"), glm::vec3(-10.6, 0, 9.95), glm::vec3(0, 0, 0)));
	doors.push_back(Door("door3", AssetManager::GetModel("door"), AssetManager::GetModel("door_frame"), glm::vec3(-12.3, 0, 4.6), glm::vec3(0, 1.5708f, 0), false));
	doors.push_back(Door("door4", AssetManager::GetModel("door"), AssetManager::GetModel("door_frame"), glm::vec3(-10, 0, 4.6), glm::vec3(0,1.5708f,0)));
	



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

	// MAX LIGHTS BY DEFAULT IS 128 if you want more lights go to lighting.frag and change MAXLIGHTS
	{
		Light light(glm::vec3(-11, 2, 15), glm::vec3(1, 0.779, 0.529) * 5.0f, 0.09, 0.032);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(-11, 2, 6), glm::vec3(1, 0.779, 0.529) * 5.0f, 0.09, 0.032);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(-15, 2, 5), glm::vec3(1, 0.25, 0) * 5.0f, 0.09, 0.032);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(-7.5, 2, 5), glm::vec3(1, 0.25, 0) * 5.0f, 0.09, 0.032);
		lights.push_back(light);
	}

	{
		Light light(glm::vec3(-2.5, 2, -5), glm::vec3(1, 0.25, 0) * 5.0f, 0.09, 0.0320);
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
	{
		Light light(glm::vec3(-1, 10, -1), glm::vec3(1, 1, 1) * 50.0f, 0.09, 0.0320);
		lights.push_back(light);
	}


	
	Player::Init();
	Player::setPosition(glm::vec3(0, 10, 0));

	// TODO: not currently working
	//AssetManager::SaveAssets("Assets/Saves/mainScene.json");


	ModelMatrixId = glGetUniformLocation(Renderer::GetCurrentProgramID(), "model");
	animatior = Animator();


	glBindVertexArray(sky.GetSkyBoxVAO());
}

void Scene::Update(float deltaTime) {
	animatior.UpdateAnimation(deltaTime);
	for (int i = 0; i < lights.size(); i++) {
		if(glm::distance(lights[i].position,Player::getPosition()) < lights[i].updateDistance || lights[i].Dynamic)
			lights[i].GenerateShadows();
	}
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
			//make guns render on top;
			NeedRendering.push_back(gameobjectRender);
			continue;
		}
		if (!gameobjectRender->GetModel()->GetAABB()->isOnFrustum(Camera::GetFrustum(), gameobjectRender->getTransform()))
			continue;

		auto transforms = animatior.GetFinalBoneMatrices(gameobjectRender->GetName());
		for (int i = 0; i < transforms.size(); ++i) {
			std::string pos = "finalBonesMatrices[" + std::to_string(i) + "]";
			Renderer::setMat4(glGetUniformLocation(programid, pos.c_str()), transforms[i]);
		}

		glm::mat4 ModelMatrix = gameobjectRender->GetModelMatrix();
		glm::mat4 modelViewMatrix = ViewMatrix * ModelMatrix;
		glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelViewMatrix)));

		glUniformMatrix3fv(glGetUniformLocation(programid, "normalMatrix3"), 1, GL_FALSE, &normalMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(programid, "M"), 1, GL_FALSE, &ModelMatrix[0][0]);
		gameobjectRender->RenderObject(programid);
	}

}

void Scene::AddGunPickUp(GunPickUp gunpickup) {
	gunPickUps.push_back(gunpickup);
}
void Scene::RenderAllObjects(GLuint programid) {
	NeedRendering.clear();
	glm::mat4 ViewMatrix = Camera::getViewMatrix();
	for (int i = 0; i < AssetManager::GetGameObjectsSize(); i++) {
		GameObject* gameobjectRender = AssetManager::GetGameObject(i);

		if (!gameobjectRender->ShouldRender())
			continue;
		if (gameobjectRender->GetShaderType() != "Default") {
			//make guns render on top;
			NeedRendering.push_back(gameobjectRender);
			continue;
		}

		auto transforms = animatior.GetFinalBoneMatrices(gameobjectRender->GetName());
		for (int i = 0; i < transforms.size(); ++i) {
			std::string pos = "finalBonesMatrices[" + std::to_string(i) + "]";
			Renderer::setMat4(glGetUniformLocation(programid, pos.c_str()), transforms[i]);
		}

		glm::mat4 ModelMatrix = gameobjectRender->GetModelMatrix();
		glUniformMatrix4fv(glGetUniformLocation(programid, "M"), 1, GL_FALSE, &ModelMatrix[0][0]);
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

std::vector<Light> Scene::getLights() {
	return lights;
}

SkyBox Scene::GetSkyBox() {
	return sky;
}
std::vector<GameObject*> Scene::NeedRenderingObjects() {
	return NeedRendering;
}
Animator* Scene::GetAnimator() {
	return &animatior;
}


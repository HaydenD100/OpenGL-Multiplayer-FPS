#include "Scene.h"


Scene::Scene() {

}

void Scene::LoadAssets() {
	AssetManager::ClearAssets();
	AnimationManager::ClearAnimations();
	//Thank you to tokyosplif for some of the models and sounds

	AssetManager::AddTexture("white", "Assets/Textures/white.png", 0.5, 0.0);

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
	AssetManager::AddTexture("ak47", "Assets/Textures/ak47.png", "Assets/Normals/ak47_normal.png", 0.2, 0.7);
	AssetManager::AddTexture("drawer", "Assets/Textures/drawerred.png", "Assets/Normals/drawer_normal.png", 0.7, 0);
	AssetManager::AddTexture("lamp", "Assets/Textures/lamp.png", "Assets/Normals/lamp_normal.png", 0.7, 0.1);
	AssetManager::AddTexture("beige_wall", "Assets/Textures/beige_wall.jpg", "Assets/Normals/beige_wall_normal.jpg", 0.7, 0);
	AssetManager::AddTexture("wooden_floor", "Assets/Textures/wooden_floor.jpg", "Assets/Normals/wooden_floor_normal.jpg", 0.2, 0.2);
	AssetManager::AddTexture("arm", "Assets/Textures/Arm_COL.png", "Assets/Normals/Arm_NOR.png", 0.7, 0);
	AssetManager::AddTexture("brick", "Assets/Textures/brick.png", "Assets/Normals/bricks_normal.png", "Assets/Roughness/brick_roughness.png", 0);
	//AssetManager::AddTexture("sand_ground", "Assets/Textures/sand_ground.jpg", "Assets/Normals/sand_ground_normal.jpg", "Assets/Roughness/sand_ground_roughness.jpg", 0);
	AssetManager::AddTexture("sand_ground", "Assets/Textures/brick.png", "Assets/Normals/test.jpg", 0.1f, 0.9f);


	AssetManager::AddTexture("pallet", "Assets/Textures/pallet.png", "Assets/Normals/pallet_normal.png", "Assets/Roughness/pallet_roughness.png", "Assets/Metalic/pallet_metallic.png");
	//AssetManager::GetTexture("pallet")->SetEmissive(true);
	AssetManager::AddTexture("barrel", "Assets/Textures/barrel.jpg", "Assets/Normals/barrel_normal.jpg", "Assets/Roughness/barrel_roughness.jpg", "Assets/Metalic/barrel_metallic.jpg");
	AssetManager::AddTexture("cargo_crate", "Assets/Textures/cargo_crate.jpg", "Assets/Normals/cargo_crate_normal.jpg", "Assets/Roughness/cargo_crate_roughness.jpg", "Assets/Metalic/cargo_crate_metallic.jpg");
	AssetManager::AddTexture("knife", "Assets/Textures/knife.png", "Assets/Normals/knife_normal.png", "Assets/Roughness/knife_roughness.png", "Assets/Metalic/knife_metallic.png");





	AssetManager::AddTexture("shotgun", "Assets/Textures/remington.png", "Assets/Normals/remington_normal.png", "Assets/Roughness/remington_roughness.png","Assets/Metalic/remington_metallic.png");

	//Double Barel
	AssetManager::AddTexture("double_barrel_shotgun_main_barrel", "Assets/Objects/FBX/DoubleBarrel/Main Barrel_albedo.jpg", "Assets/Objects/FBX/DoubleBarrel/Main Barrel_normal.png", "Assets/Objects/FBX/DoubleBarrel/Main Barrel_roughness.jpg", "Assets/Objects/FBX/DoubleBarrel/Main Barrel_metallic.jpg");
	AssetManager::AddTexture("double_barrel_shotgun_wooden_grip_back", "Assets/Objects/FBX/DoubleBarrel/Woooden Grip Back_albedo.jpg", "Assets/Objects/FBX/DoubleBarrel/Woooden Grip Back_normal.png", "Assets/Objects/FBX/DoubleBarrel/Woooden Grip Back_roughness.jpg", "Assets/Objects/FBX/DoubleBarrel/Woooden Grip Back_metallic.jpg");
	AssetManager::AddTexture("double_barrel_shotgun_wooden_grip_large", "Assets/Objects/FBX/DoubleBarrel/Wooden Grip Large_albedo.jpg", "Assets/Objects/FBX/DoubleBarrel/Wooden Grip Large_normal.png", "Assets/Objects/FBX/DoubleBarrel/Wooden Grip Large_roughness.jpg", "Assets/Objects/FBX/DoubleBarrel/Wooden Grip Large_metallic.jpg");
	AssetManager::AddTexture("double_barrel_shotgun_metal_receiver", "Assets/Objects/FBX/DoubleBarrel/Metal Receiver_albedo.jpg", "Assets/Objects/FBX/DoubleBarrel/Metal Receiver_normal.png", "Assets/Objects/FBX/DoubleBarrel/Metal Receiver_roughness.jpg", "Assets/Objects/FBX/DoubleBarrel/Metal Receiver_metallic.jpg");

	AssetManager::AddTexture("glass", "Assets/Textures/glass.png", 0.1, 1);
	AssetManager::AddTexture("transparent", "Assets/Textures/glass.png", 0, 0);


	// TODO: not currently working
	//AssetManager::LoadAssets("Assets/Saves/mainScene.json");
	//Loads Mode

	AssetManager::AddModel("window", Model("Assets/Objects/FBX/window.fbx", AssetManager::GetTexture("window")));
	AssetManager::AddModel("window_glass", Model("Assets/Objects/FBX/window_glass.fbx", AssetManager::GetTexture("glass")));

	AssetManager::AddModel("swat", Model("Assets/Objects/FBX/swat_death.dae", AssetManager::GetTexture("uvmap")));
	
	AssetManager::AddModel("playertwo", Model("Assets/Objects/FBX/bean_death.dae","Assets/Objects/player_mesh.obj", AssetManager::GetTexture("uvmap")));


	
	AssetManager::AddModel("probe", Model("Assets/Objects/FBX/probe_cube.fbx", AssetManager::GetTexture("uvmap")));
	AssetManager::AddModel("cube", Model("Assets/Objects/FBX/cube.fbx", AssetManager::GetTexture("uvmap")));

	AssetManager::AddModel("light_cube", Model("Assets/Objects/FBX/light_cube.fbx", AssetManager::GetTexture("uvmap")));

	AssetManager::AddModel("Room", "Assets/Objects/Map1/shapespark-example-room.fbx", AssetManager::GetTexture("uvmap"));


	AssetManager::AddModel("fence1", Model("Assets/Objects/fence1.fbx", AssetManager::GetTexture("concrete")));
	AssetManager::AddModel("fence2", Model("Assets/Objects/fence2.fbx", AssetManager::GetTexture("concrete")));
	AssetManager::AddModel("fence3", Model("Assets/Objects/fence3.fbx", AssetManager::GetTexture("concrete")));
	AssetManager::AddModel("floor", Model("Assets/Objects/FBX/floor.fbx", AssetManager::GetTexture("sand")));
	AssetManager::AddModel("slope", Model("Assets/Objects/FBX/slope.fbx", AssetManager::GetTexture("sand")));
	AssetManager::AddModel("crate", Model("Assets/Objects/FBX/crate.fbx", AssetManager::GetTexture("crate")));
	AssetManager::AddModel("cube", Model("Assets/Objects/FBX/cube.fbx", AssetManager::GetTexture("red_glass")));
	AssetManager::AddModel("cube1", Model("Assets/Objects/FBX/cube.fbx", AssetManager::GetTexture("green_glass")));

	AssetManager::AddModel("glock", Model("Assets/Objects/FBX/glock17.fbx", "Assets/Objects/glock17_convex.obj", AssetManager::GetTexture("glock")));
	AssetManager::AddModel("glockhand", Model("Assets/Objects/FBX/glock17_shoot1.dae", AssetManager::GetTexture("glock")));
	AssetManager::GetModel("glockhand")->GetMeshByName("Arms_L_R_Mesh_002-mesh")->SetTexture(AssetManager::GetTexture("arm"));

	AssetManager::AddModel("double_barrel", Model("Assets/Objects/FBX/double_barrel.fbx","Assets/Objects/db_convex.obj", AssetManager::GetTexture("double_barrel_shotgun_main_barrel")));
	AssetManager::GetModel("double_barrel")->GetMeshByName("meshId0_name")->SetTexture(AssetManager::GetTexture("double_barrel_shotgun_main_barrel"));
	AssetManager::GetModel("double_barrel")->GetMeshByName("meshId2_name")->SetTexture(AssetManager::GetTexture("double_barrel_shotgun_wooden_grip_large"));
	AssetManager::GetModel("double_barrel")->GetMeshByName("meshId3_name")->SetTexture(AssetManager::GetTexture("double_barrel_shotgun_wooden_grip_back"));
	AssetManager::GetModel("double_barrel")->GetMeshByName("meshId10_name")->SetTexture(AssetManager::GetTexture("double_barrel_shotgun_metal_receiver"));

	
	AssetManager::AddModel("ak47hand", Model("Assets/Objects/FBX/ak47_shoot.dae", "Assets/Objects/ak47_convex.obj", AssetManager::GetTexture("ak47")));
	AssetManager::GetModel("ak47hand")->GetMeshByName("Arms_L_R_Mesh_002-mesh")->SetTexture(AssetManager::GetTexture("arm"));

	AssetManager::AddModel("knifehand", Model("Assets/Objects/FBX/knife_equip.dae", "Assets/Objects/glock17_convex.obj", AssetManager::GetTexture("knife")));
	AssetManager::GetModel("knifehand")->GetMeshByName("Arms_L_R_Mesh_002-mesh")->SetTexture(AssetManager::GetTexture("arm"));


	AssetManager::AddModel("double_barrel_hand", Model("Assets/Objects/FBX/db_shoot.dae", "Assets/Objects/db_convex.obj", AssetManager::GetTexture("double_barrel_shotgun_main_barrel")));
	AssetManager::GetModel("double_barrel_hand")->GetMeshByName("meshId0_name-mesh")->SetTexture(AssetManager::GetTexture("double_barrel_shotgun_main_barrel"));
	AssetManager::GetModel("double_barrel_hand")->GetMeshByName("meshId2_name-mesh")->SetTexture(AssetManager::GetTexture("double_barrel_shotgun_wooden_grip_large"));
	AssetManager::GetModel("double_barrel_hand")->GetMeshByName("meshId3_name-mesh")->SetTexture(AssetManager::GetTexture("double_barrel_shotgun_wooden_grip_back"));
	AssetManager::GetModel("double_barrel_hand")->GetMeshByName("meshId10_name-mesh")->SetTexture(AssetManager::GetTexture("double_barrel_shotgun_metal_receiver"));
	AssetManager::GetModel("double_barrel_hand")->GetMeshByName("Arms_L_R_Mesh_002-mesh")->SetTexture(AssetManager::GetTexture("arm"));



	//AssetManager::AddModel("map_floor", Model("Assets/Objects/Map1/floors.fbx", AssetManager::GetTexture("wooden_floor")));
	//AssetManager::AddModel("map_walls", Model("Assets/Objects/Map1/walls.fbx", AssetManager::GetTexture("beige_wall")));
	//AssetManager::AddModel("map_ceiling", Model("Assets/Objects/Map1/ceiling.fbx", AssetManager::GetTexture("beige_wall")));

	//AssetManager::AddModel("map_test1", Model("Assets/Maps/map_test1.fbx", AssetManager::GetTexture("wooden_floor")));
	//AssetManager::GetModel("map_test1")->GetMeshByName("Map_floor")->SetTexture(AssetManager::GetTexture("wooden_floor"));
	AssetManager::AddModel("map_test1", Model("Assets/Maps/dusty_1.fbx", AssetManager::GetTexture("brick")));

	AssetManager::AddModel("map_test2", Model("Assets/Maps/other_map.obj", AssetManager::GetTexture("white")));
	AssetManager::AddModel("couch", Model("Assets/Objects/FBX/couch.fbx", AssetManager::GetTexture("white")));


	AssetManager::AddModel("map_indirectLight", Model("Assets/Maps/lightingTest.obj", AssetManager::GetTexture("white")));
	AssetManager::AddModel("water", Model("Assets/Objects/FBX/water_test.obj", AssetManager::GetTexture("white")));


	Model* model = AssetManager::GetModel("map_test1");
	model->GetMeshByName("floor.001")->SetTexture(AssetManager::GetTexture("sand_ground"));
	model->GetMeshByName("barrel1")->SetTexture(AssetManager::GetTexture("barrel"));
	model->GetMeshByName("barrel2")->SetTexture(AssetManager::GetTexture("barrel"));
	model->GetMeshByName("barrel3")->SetTexture(AssetManager::GetTexture("barrel"));
	model->GetMeshByName("cargo_crate")->SetTexture(AssetManager::GetTexture("cargo_crate"));
	model->GetMeshByName("cargo_crate1")->SetTexture(AssetManager::GetTexture("cargo_crate"));

	model->GetMeshByName("pallet1")->SetTexture(AssetManager::GetTexture("pallet"));
	model->GetMeshByName("pallet2")->SetTexture(AssetManager::GetTexture("pallet"));
	model->GetMeshByName("pallet5")->SetTexture(AssetManager::GetTexture("pallet"));
	model->GetMeshByName("pallet4")->SetTexture(AssetManager::GetTexture("pallet"));
	model->GetMeshByName("pallet3.001")->SetTexture(AssetManager::GetTexture("pallet"));

	AssetManager::AddModel("ak47", Model("Assets/Objects/FBX/ak47.fbx", "Assets/Objects/ak47_convex.obj", AssetManager::GetTexture("ak47")));
	AssetManager::AddModel("door", Model(Mesh("Assets/Objects/door.obj"), AssetManager::GetTexture("door")));
	AssetManager::AddModel("door_frame", Model(Mesh("Assets/Objects/door_frame.obj"), AssetManager::GetTexture("door")));
	AssetManager::AddModel("player", Model("Assets/Objects/FBX/player.fbx", AssetManager::GetTexture("uvmap")));

	AssetManager::AddModel("shotgun", Model("Assets/Objects/fbx/remington.fbx", "Assets/Objects/shotgun_convex.obj", AssetManager::GetTexture("shotgun")));

	AssetManager::AddDecal("bullet_hole", AssetManager::GetTexture("bullet_hole"), glm::vec3(0.02, 0.005, 0.02));
	AssetManager::AddDecal("panda_decal", AssetManager::GetTexture("panda"), glm::vec3(1, 0.1, 1));
	AssetManager::AddDecal("flower_decal", AssetManager::GetTexture("flower"), glm::vec3(1, 0.1, 1));
	AssetManager::AddDecal("tank_decal", AssetManager::GetTexture("tank"), glm::vec3(1, 0.1, 1));
	AssetManager::AddDecal("pizza_decal", AssetManager::GetTexture("pizza"), glm::vec3(1, 0.1, 1));
	AssetManager::AddDecal("freaky_decal", AssetManager::GetTexture("freaky"), glm::vec3(1, 0.1, 1));

	//these are diffrent animations from skinnedanimation
	AnimationManager::AddAnimation(Animation("Assets/Animations/door_open.fbx", "door_open"));
	AnimationManager::AddAnimation(Animation("Assets/Animations/door_close.fbx", "door_close"));

	
}


void Scene::Load() { 
	LoadAssets();


	WeaponManager::Init();


	{
		//AssetManager::AddGameObject("map_test1", AssetManager::GetModel("map_test1"), glm::vec3(0, 0, 0), true, 0, Concave);
		//AssetManager::GetGameObject("map_test1")->SetRotationX(-1.5708f);

		
		//AssetManager::AddGameObject("map_indirectLight", AssetManager::GetModel("map_indirectLight"), glm::vec3(0, 6, 0), true, 0, Concave);
		AssetManager::AddGameObject("map_test2", AssetManager::GetModel("map_test2"), glm::vec3(0, 6, 0), true, 0, Concave);

		//AssetManager::AddGameObject("couch", AssetManager::GetModel("couch"), glm::vec3(-1.69, 6.4, -3.56), true, 0, Box);

		//AssetManager::GetGameObject("couch")->SetRotationX(-1.5708f);

		AssetManager::AddGameObject("water_test", AssetManager::GetModel("water"), glm::vec3(-1, 10, -6), true, 0, Concave);
		AssetManager::GetGameObject("water_test")->SetShaderType("water");
		
		// Sets renderer
		std::vector<std::string> faces{
			"Assets/Skybox/Space/right.png",
				"Assets/Skybox/Space/left.png",
				"Assets/Skybox/Space/top.png",
				"Assets/Skybox/Space/bottom.png",
				"Assets/Skybox/Space/front.png",
				"Assets/Skybox/Space/back.png"
		};
		sky = SkyBox(faces);
		/*
		{
			Light light(glm::vec3(5.89, 3.1, -2.5), glm::vec3(1, 0., 0.753) * 2.0f, 0.35, 0.44);
			lights.push_back(light);
		}

		{
			Light light(glm::vec3(2.2, 1.7, -7.37), glm::vec3(0.922, 0.612,0) * 2.0f, 0.35, 0.44);
			lights.push_back(light);
		}

		{
			Light light(glm::vec3(3.16, 3.1, -8), glm::vec3(1, 0.922, 0.753) * 2.0f, 0.35, 0.44);
			lights.push_back(light);
		}
		{
			Light light(glm::vec3(12.6, 3.1, -3.49), glm::vec3(1, 0.922, 0.753) * 2.0f, 0.35, 0.44);
			lights.push_back(light);
		}
		{
			Light light(glm::vec3(2.7, 3.1, 7.75), glm::vec3(1, 0., 0.753) * 2.0f, 0.35, 0.44);
			lights.push_back(light);
		}
		*/

		{
			Light light(glm::vec3(-3.27, 7.2, 3.42), glm::vec3(1, 0.11, 0) * 6.0f, 0.22, 0.20);
			lights.push_back(light);
		}
		{
			Light light(glm::vec3(-4.44, 6.6, 0), glm::vec3(0, 0.573, 1) * 6.0f, 0.22, 0.20);
			lights.push_back(light);
		}
		{
			Light light(glm::vec3(0, 12, -2.4), glm::vec3(1, 0.922, 0.678) * 8.0f, 0.07, 0.017);
			lights.push_back(light);
		}
		
		
		
	}

	//gunSpawners.push_back(GunSpawner("ak47", "spawner1",glm::vec3(1, 18, -1)));

	
	Player::Init();
	Player::setPosition(glm::vec3(0, 10, 0));

	// TODO: not currently working
	//AssetManager::SaveAssets("Assets/Saves/mainScene.json");
	Animator::Init();
	glBindVertexArray(sky.GetSkyBoxVAO());
}

void Scene::Update(float deltaTime) {
	//problem should pull this out
	Player::Update(deltaTime);
	Animator::UpdateAnimation(deltaTime);


	for (int i = 0; i < lights.size(); i++) {
		if(glm::distance(lights[i].position,Player::getPosition()) < lights[i].updateDistance || lights[i].Dynamic)
			lights[i].GenerateShadows();
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
		if (gunPickUps[gun].Interact())
			gunPickUps.erase(gunPickUps.begin() + gun);
	}
	for (int spawner = 0; spawner < gunSpawners.size(); spawner++) {
		gunSpawners[spawner].CheckForSpawn();
	}

	for (int crate = 0; crate < crates.size(); crate++) {
		crates[crate].Update();
	}

	AudioManager::UpdateListener(Player::getPosition(), Player::getForward(), PhysicsManager::GetRigidbody("PlayerRB")->GetForce());
	AudioManager::Update();
}



void Scene::AddGunPickUp(GunPickUp gunpickup) {
	gunPickUps.push_back(gunpickup);
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
int Scene::DoesGunPickUpExsit(std::string name) {
	for (int i = 0; i < gunPickUps.size(); i++) {
		if (gunPickUps[i].GetName() == name)
			return 1;
	}
	return 0;
}



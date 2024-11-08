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
	AssetManager::AddTexture("arm", "Assets/Textures/Arm_COL.png", "Assets/Normals/Arm_NOR.png", 0.7, 0);
	AssetManager::AddTexture("brick", "Assets/Textures/brick.png", "Assets/Normals/bricks_normal.png", "Assets/Roughness/brick_roughness.png", 0);
	AssetManager::AddTexture("sand_ground", "Assets/Textures/sand_ground.jpg", "Assets/Normals/sand_ground_normal.jpg", "Assets/Roughness/sand_ground_roughness.jpg", 0);

	AssetManager::AddTexture("pallet", "Assets/Textures/pallet.png", "Assets/Normals/pallet_normal.png", "Assets/Roughness/pallet_roughness.png", "Assets/Metalic/pallet_metallic.png");
	AssetManager::AddTexture("barrel", "Assets/Textures/barrel.jpg", "Assets/Normals/barrel_normal.jpg", "Assets/Roughness/barrel_roughness.jpg", "Assets/Metalic/barrel_metallic.jpg");
	AssetManager::AddTexture("cargo_crate", "Assets/Textures/cargo_crate.jpg", "Assets/Normals/cargo_crate_normal.jpg", "Assets/Roughness/cargo_crate_roughness.jpg", "Assets/Metalic/cargo_crate_metallic.jpg");





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
	
	AssetManager::AddModel("playertwo", Model("Assets/Objects/FBX/bean_death.dae", AssetManager::GetTexture("uvmap")));


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
	Model* model = AssetManager::GetModel("map_test1");
	model->GetMeshByName("floor.001")->SetTexture(AssetManager::GetTexture("sand_ground"));
	model->GetMeshByName("barrel1")->SetTexture(AssetManager::GetTexture("barrel"));
	model->GetMeshByName("barrel2")->SetTexture(AssetManager::GetTexture("barrel"));
	model->GetMeshByName("barrel3")->SetTexture(AssetManager::GetTexture("barrel"));
	model->GetMeshByName("cargo_crate")->SetTexture(AssetManager::GetTexture("cargo_crate"));
	model->GetMeshByName("pallet1")->SetTexture(AssetManager::GetTexture("pallet"));
	model->GetMeshByName("pallet2")->SetTexture(AssetManager::GetTexture("pallet"));
	model->GetMeshByName("pallet3")->SetTexture(AssetManager::GetTexture("pallet"));
	model->GetMeshByName("pallet4")->SetTexture(AssetManager::GetTexture("pallet"));





	//Stuff for another map
	/*
	auto model = AssetManager::GetModel("map_test1");
	auto beigeWallTexture = AssetManager::GetTexture("beige_wall");

	for (int i = 1; i <= 40; ++i) {
		std::stringstream ss;
		ss << "map_walls." << std::setw(3) << std::setfill('0') << i;
		std::string meshName = ss.str();

		model->GetMeshByName(meshName)->SetTexture(beigeWallTexture);
	}

	AssetManager::GetModel("map_test1")->GetMeshByName("map_roof.008")->SetTexture(AssetManager::GetTexture("beige_wall"));
	*/
	

	//AssetManager::GetModel("doublebarrel")->GetMeshByName("Arms_L_R_Mesh_002-mesh")->SetTexture(AssetManager::GetTexture("uvmap"));

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


	//AssetManager::AddGameObject("map1_floor", AssetManager::GetModel("map_floor"), glm::vec3(0, 1.6, 0), true, 0, Concave);
	//AssetManager::AddGameObject("map1_walls", AssetManager::GetModel("map_walls"), glm::vec3(0, 1.6, 0), true, 0, Concave);
	AssetManager::AddGameObject("map_test1", AssetManager::GetModel("map_test1"), glm::vec3(0, 0, 0), true, 0, Concave);
	AssetManager::GetGameObject("map_test1")->SetRotationX(-1.5708f);

	/*
	AssetManager::AddGameObject("window1", AssetManager::GetModel("window"), glm::vec3(-11, 2, 14),true, 0,Concave);
	AssetManager::AddGameObject("window_glass1", AssetManager::GetModel("window_glass"), glm::vec3(-11, 2, 14), true, 0, Concave);
	AssetManager::GetGameObject("window_glass1")->SetShaderType("Transparent");
	AssetManager::GetGameObject("window_glass1")->SetRotationX(1.5708f);
	*/
	/*
	AssetManager::AddGameObject("red_glass", AssetManager::GetModel("cube"), glm::vec3(-7, 2, 6), true, 0, Convex);
	AssetManager::GetGameObject("red_glass")->SetShaderType("Transparent");

	AssetManager::AddGameObject("red_glass1", AssetManager::GetModel("cube1"), glm::vec3(-6, 2, 8), true, 0, Convex);
	AssetManager::GetGameObject("red_glass1")->SetShaderType("Transparent");

	AssetManager::AddGameObject("floor", AssetManager::GetModel("floor"), glm::vec3(0, 0, 0), true, 0, Box);
	*/
	
	/*
	crates.push_back(Crate(glm::vec3(1, 2, 1), "crate1", AssetManager::GetModel("crate")));
	crates.push_back(Crate(glm::vec3(-3, 2, -3), "crate2", AssetManager::GetModel("crate")));
	crates.push_back(Crate(glm::vec3(-10, 2, 14), "crate3", AssetManager::GetModel("crate")));

	crates.push_back(Crate(glm::vec3(10, 2, -3), "crate1", AssetManager::GetModel("crate")));
	crates.push_back(Crate(glm::vec3(-13, 2, -5), "crate2", AssetManager::GetModel("crate")));
	crates.push_back(Crate(glm::vec3(10, 2, 14), "crate3", AssetManager::GetModel("crate")));
	crates.push_back(Crate(glm::vec3(3, 2, -9), "crate1", AssetManager::GetModel("crate")));
	crates.push_back(Crate(glm::vec3(7, 2, 7), "crate2", AssetManager::GetModel("crate")));
	crates.push_back(Crate(glm::vec3(-10, 2, 14), "crate3", AssetManager::GetModel("crate")));

	crates.push_back(Crate(glm::vec3(1, 14.5, -1), "crate1", AssetManager::GetModel("crate")));
	crates.push_back(Crate(glm::vec3(2, 14.5, -1), "crate2", AssetManager::GetModel("crate")));
	crates.push_back(Crate(glm::vec3(1, 14.5, -2), "crate3", AssetManager::GetModel("crate")));
	*/
	//gunPickUps.push_back(GunPickUp("ak47", "ak47_pickup", AssetManager::GetModel("ak47"), glm::vec3(1, 30, 1)));
	//gunPickUps.push_back(GunPickUp("glock", "glock_pickup", AssetManager::GetModel("glock"), glm::vec3(1, 25, 0)));

	/*
	doors.push_back(Door("door1", AssetManager::GetModel("door"), AssetManager::GetModel("door_frame"), glm::vec3(0.4, 0, -5), glm::vec3(0, 0, 0)));
	doors.push_back(Door("door2", AssetManager::GetModel("door"), AssetManager::GetModel("door_frame"), glm::vec3(-9.4, 0, -5), glm::vec3(0, 0, 0)));
	doors.push_back(Door("door3", AssetManager::GetModel("door"), AssetManager::GetModel("door_frame"), glm::vec3(-9.4, 0, -15), glm::vec3(0, 0, 0)));
	doors.push_back(Door("door4", AssetManager::GetModel("door"), AssetManager::GetModel("door_frame"), glm::vec3(-9.4, 0, -24.8), glm::vec3(0,0,0)));
	*/
	
	//AssetManager::AddGameObject(GameObject("swat", AssetManager::GetModel("swat"), glm::vec3(0, 0, 0), false, 0,Capsule,0.5,2,0));
	

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

	/*
	// MAX LIGHTS BY DEFAULT IS 128 if you want more lights go to lighting.frag and change MAXLIGHTS
	{
		Light light(glm::vec3(-3.6,5, 2), glm::vec3(1, 0.779, 0.529) * 7.0f, 0.027, 0.0028);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(-5, 5, -10), glm::vec3(1, 0.779, 0.529) * 6.0f, 0.027, 0.0028);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(-10, 5, -19), glm::vec3(1, 0.779, 0.529) * 9.0f, 0.027, 0.0028);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(0, 5, -20), glm::vec3(1, 0.25, 0) * 8.0f, 0.027, 0.0028);
		lights.push_back(light);
	}

	{
		Light light(glm::vec3(-5, 5, -31), glm::vec3(1, 0.779, 0.529) * 10.0f, 0.027, 0.0028);
		lights.push_back(light);
	}

	{
		Light light(glm::vec3(-18, 2, -20), glm::vec3(1, 0.25, 0) * 5.0f, 0.09, 0.0320);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(-14, 5, -1), glm::vec3(1, 0.779, 0.529) * 5.0f, 0.09, 0.0320);
		lights.push_back(light);
	}

	{
		Light light(glm::vec3(3, 5, -9), glm::vec3(1, 0.779, 0.529) * 5.0f, 0.09, 0.0320);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(-21, 5, 12), glm::vec3(1, 0.779, 0.529) * 5.0f, 0.09, 0.0320);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(-6, 5, 21), glm::vec3(1, 0.779, 0.529) * 5.0f, 0.09, 0.0320);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(13, 2, 18), glm::vec3(1, 0.779, 0.529) * 5.0f, 0.09, 0.0320);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(18, 2, 0), glm::vec3(1, 0.779, 0.529) * 5.0f, 0.09, 0.0320);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(1, 6.5, -1), glm::vec3(1, 0.779, 0.529) * 5.0f, 0.09, 0.0320);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(1, 10, -1), glm::vec3(1, 0.779, 0.529) * 5.0f, 0.09, 0.0320);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(1, 14.5, -1), glm::vec3(1, 0.779, 0.529) * 5.0f, 0.09, 0.0320);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(1, 18, -1), glm::vec3(1, 0.779, 0.529) * 5.0f, 0.09, 0.0320);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(19, 3.5, 10), glm::vec3(1, 0.5, 0.529) * 5.0f, 0.09, 0.0320);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(21, 3.5, -21), glm::vec3(0.8, 0, 0.8) * 5.0f, 0.09, 0.0320);
		lights.push_back(light);
	}
	*/
	{
		Light light(glm::vec3(13.4, 4, -9), glm::vec3(1, 0.779, 0.529) * 2.0f, 0.35, 0.44);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(7.7, 4, 10), glm::vec3(1, 0.779, 0.529) * 2.0f, 0.35, 0.44);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(4.86, 4, -1.5), glm::vec3(1, 0.779, 0.529) * 2.0f, 0.35, 0.44);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(13, 4, 7.46), glm::vec3(1, 0.779, 0.529) * 2.0f, 0.35, 0.44);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(-1.77, 3, 7.47), glm::vec3(1, 0.779, 0.529) * 2.0f, 0.35,	0.44);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(3.88, 4, -7), glm::vec3(1, 0.779, 0.529) * 2.0f, 0.35,	0.44);
		lights.push_back(light);
	}
	{
		Light light(glm::vec3(7, 15, -2), glm::vec3(1, 0.922, 0.753) * 10.0f, 0.09, 0.0320);
		lights.push_back(light);
	}

	/*
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
	*/

	//gunSpawners.push_back(GunSpawner("ak47", "spawner1",glm::vec3(1, 18, -1)));

	
	Player::Init();
	Player::setPosition(glm::vec3(0, 30, 0));

	// TODO: not currently working
	//AssetManager::SaveAssets("Assets/Saves/mainScene.json");


	ModelMatrixId = glGetUniformLocation(Renderer::GetCurrentProgramID(), "model");
	Animator::Init();

	//swat_death = SkinnedAnimation("Assets/Objects/FBX/swat_death.dae", AssetManager::GetModel("swat"), 0);
	//Animator::PlayAnimation(&swat_death, "swat", false);
	//TODO :: dosent work yet
	PathFinding::Init();
	glBindVertexArray(sky.GetSkyBoxVAO());
}

void Scene::Update(float deltaTime) {
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
		if (!gameobjectRender->GetModel()->GetAABB()->isOnFrustum(Camera::GetFrustum(), gameobjectRender->getTransform()) && !gameobjectRender->DontCull())
			continue;

		auto transforms = gameobjectRender->GetFinalBoneMatricies();
		if (transforms[0] != glm::mat4(1)) {
			glUniform1i(glGetUniformLocation(programid, "animated"), true);

			for (int i = 0; i < transforms.size(); ++i) {
				std::string pos = "finalBonesMatrices[" + std::to_string(i) + "]";
				Renderer::setMat4(glGetUniformLocation(programid, pos.c_str()), transforms[i]);
			}
		}
		else
			glUniform1i(glGetUniformLocation(programid, "animated"), false);

		

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

		//auto transforms = animatior.GetFinalBoneMatrices(gameobjectRender->GetName());
		auto transforms = gameobjectRender->GetFinalBoneMatricies();
		if (transforms[0] != glm::mat4(1)) {
			glUniform1i(glGetUniformLocation(programid, "animated"), true);

			for (int i = 0; i < transforms.size(); ++i) {
				std::string pos = "finalBonesMatrices[" + std::to_string(i) + "]";
				Renderer::setMat4(glGetUniformLocation(programid, pos.c_str()), transforms[i]);
			}
		}
		else
			glUniform1i(glGetUniformLocation(programid, "animated"), false);

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
int Scene::DoesGunPickUpExsit(std::string name) {
	for (int i = 0; i < gunPickUps.size(); i++) {
		if (gunPickUps[i].GetName() == name)
			return 1;
	}
	return 0;
}



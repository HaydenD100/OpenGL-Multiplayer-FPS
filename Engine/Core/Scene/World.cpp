#include "World.h"
#include "Engine/Physics/BulletPhysics.h"
#include "Physics/Physics.h"
#include "Engine/Core/Input.h"

#include "Engine/Animation/SkinnedAnimatior.h"


namespace World {

	EnviromentLighting envLight;

	std::vector<std::unique_ptr<GameObject>> g_water;
	std::vector<std::unique_ptr<GameObject>> g_objects;
	std::vector<std::unique_ptr<GameObject>> g_glass;
	std::vector<GunPickUp> m_gunPickups;
	std::vector<Light> g_lights;
	std::vector<std::unique_ptr<TriggerCollider>> g_triggers;
	std::vector<Sprite> g_sprites;


	float m_seaLevel = 0;

	void LoadAssets() {
		//AssetManager::ClearAssets();
		//AnimationManager::ClearAnimations();
		//Thank you to Tokyosplif for some of the models and sounds
		//Mehdi Shahsavan credits for some of these models


		AssetManager::AddTexture("white", "Assets/Textures/white.png", 0.5, 0.0);
		AssetManager::AddTexture("dev_textures", "Assets/Textures/dev_textures.png", "Assets/Normals/no_normal.png", "Assets/Objects/FBX/DoubleBarrel/Main Barrel_roughness.jpg", "Assets/Objects/FBX/DoubleBarrel/Main Barrel_metallic.jpg");

		AssetManager::AddTexture("metalic", "Assets/Textures/white.png", 0.0f, 1.0f);

		//AssetManager::AddTexture("cornel", "Assets/Textures/cornel-box.png", 0.8, 0.0);
		


		AssetManager::AddTexture("flash", "Assets/Sprites/MuzzleFlash.png", 0.5, 0.0);

		AssetManager::AddTexture("white_light", "Assets/Textures/white.png", 0.5, 0.0);
		AssetManager::GetTexture("white_light")->SetEmissive(true);
		AssetManager::AddTexture("panda", "Assets/Textures/panda_decal.png", 0, 0);
		AssetManager::AddTexture("flower", "Assets/Textures/flower_decal.png", 0, 0);
		AssetManager::AddTexture("freaky", "Assets/Textures/freaky_decal.png", 0, 0);
		AssetManager::AddTexture("pizza", "Assets/Textures/pizza_decal.png", 0, 0);
		AssetManager::AddTexture("tank", "Assets/Textures/tank_decal.png", 0, 0);
		//AssetManager::AddTexture("crate", "Assets/Textures/crate.png", 0.7, 0);
		//AssetManager::AddTexture("window", "Assets/Textures/window.png", 0.7, 0);
		AssetManager::AddTexture("bullet_hole", "Assets/Textures/bullet_hole.png", "Assets/Normals/window_normal.png", 0.5, 0);
		AssetManager::AddTexture("glock", "Assets/Textures/glock_17.png", "Assets/Normals/glock_17_normal.png", 0.5, 0.5);
		AssetManager::AddTexture("ak47", "Assets/Textures/ak47.png", "Assets/Normals/ak47_normal.png", 0.2, 0.7);
		AssetManager::AddTexture("drawer", "Assets/Textures/drawerred.png", "Assets/Normals/drawer_normal.png", 0.7, 0);
		//AssetManager::AddTexture("lamp", "Assets/Textures/lamp.png", "Assets/Normals/lamp_normal.png", 0.7, 0.1);
		AssetManager::AddTexture("arm", "Assets/Textures/Arm_COL.png", "Assets/Normals/Arm_NOR.png", 0.7, 0);
		AssetManager::AddTexture("ladder", "Assets/Textures/ladder.png", "Assets/Normals/ladder_normal.png", "Assets/Roughness/ladder_roughness.png", "Assets/Metalic/ladder_metallic.png");
		//AssetManager::AddTexture("pallet", "Assets/Textures/pallet.png", "Assets/Normals/pallet_normal.png", "Assets/Roughness/pallet_roughness.png", "Assets/Metalic/pallet_metallic.png");
		AssetManager::AddTexture("knife", "Assets/Textures/knife.png", "Assets/Normals/knife_normal.png", "Assets/Roughness/knife_roughness.png", "Assets/Metalic/knife_metallic.png");
		AssetManager::AddTexture("shotgun", "Assets/Textures/remington.png", "Assets/Normals/remington_normal.png", "Assets/Roughness/remington_roughness.png", "Assets/Metalic/remington_metallic.png");
		//AssetManager::AddTexture(Texture("shelf"));
		//AssetManager::AddTexture(Texture("vase"));
		//AssetManager::AddTexture(Texture("plant"));
		AssetManager::AddTexture("angled-tiled-floor", "Assets/Textures/angled-tiled-floor.png", 0.5f, 0.0f);
		AssetManager::AddTexture("CratePile", "Assets/Textures/Crate.jpeg", "Assets/Normals/Crate.jpeg", "Assets/Roughness/Crate.jpeg", "Assets/Metalic/vase_metalic.png");

		//Double Barel
		AssetManager::AddTexture("double_barrel_shotgun_main_barrel", "Assets/Objects/FBX/DoubleBarrel/Main Barrel_albedo.jpg", "Assets/Objects/FBX/DoubleBarrel/Main Barrel_normal.png", "Assets/Objects/FBX/DoubleBarrel/Main Barrel_roughness.jpg", "Assets/Objects/FBX/DoubleBarrel/Main Barrel_metallic.jpg");
		AssetManager::AddTexture("double_barrel_shotgun_wooden_grip_back", "Assets/Objects/FBX/DoubleBarrel/Woooden Grip Back_albedo.jpg", "Assets/Objects/FBX/DoubleBarrel/Woooden Grip Back_normal.png", "Assets/Objects/FBX/DoubleBarrel/Woooden Grip Back_roughness.jpg", "Assets/Objects/FBX/DoubleBarrel/Woooden Grip Back_metallic.jpg");
		AssetManager::AddTexture("double_barrel_shotgun_wooden_grip_large", "Assets/Objects/FBX/DoubleBarrel/Wooden Grip Large_albedo.jpg", "Assets/Objects/FBX/DoubleBarrel/Wooden Grip Large_normal.png", "Assets/Objects/FBX/DoubleBarrel/Wooden Grip Large_roughness.jpg", "Assets/Objects/FBX/DoubleBarrel/Wooden Grip Large_metallic.jpg");
		AssetManager::AddTexture("double_barrel_shotgun_metal_receiver", "Assets/Objects/FBX/DoubleBarrel/Metal Receiver_albedo.jpg", "Assets/Objects/FBX/DoubleBarrel/Metal Receiver_normal.png", "Assets/Objects/FBX/DoubleBarrel/Metal Receiver_roughness.jpg", "Assets/Objects/FBX/DoubleBarrel/Metal Receiver_metallic.jpg");

		AssetManager::AddTexture("transparent", "Assets/Textures/dusty1.png", "Assets/Normals/dirty_glass.png", 0.1f, 0.0f);
		AssetManager::AddTexture("unicorn", "Assets/Textures/unicorn.png", "Assets/Normals/unicorn_normal.png", 0.3f, 0.0f);


		AssetManager::AddTexture("uvmap", "Assets/Textures/uvmap.png", 0, 0);

		AssetManager::AddModel("quad", Model("Assets/Objects/quad.obj", AssetManager::GetTexture("uvmap")));

		AssetManager::AddModel("probe", Model("Assets/Objects/FBX/probe_cube.fbx", AssetManager::GetTexture("uvmap")));
		AssetManager::AddModel("cube", Model("Assets/Objects/FBX/cube.fbx", AssetManager::GetTexture("uvmap")));
		AssetManager::AddModel("light_cube", Model("Assets/Objects/FBX/light_cube.fbx", AssetManager::GetTexture("uvmap")));

		AssetManager::GetTexture("uvmap")->SetEmissive(true);
		AssetManager::AddModel("shaderBall", Model("Assets/Objects/shaderBall.obj", AssetManager::GetTexture("transparent")));
		AssetManager::AddModel("cubeGlass", Model("Assets/Objects/FBX/cube.fbx", AssetManager::GetTexture("transparent")));
		//AssetManager::AddModel("window", Model("Assets/Objects/FBX/window.fbx", AssetManager::GetTexture("window")));
		//AssetManager::AddModel("window_glass", Model("Assets/Objects/FBX/window_glass.fbx", AssetManager::GetTexture("glass")));

		//AssetManager::AddModel("running", Model("Assets/Objects/FBX/Running.fbx", AssetManager::GetTexture("white")));


		AssetManager::AddModel("ladder", Model("Assets/Objects/FBX/ladder.fbx", AssetManager::GetTexture("ladder")));
		//AssetManager::AddModel("shelf", Model("Assets/Objects/FBX/shelf.fbx", AssetManager::GetTexture("shelf")));
		//AssetManager::AddModel("vase", Model("Assets/Objects/FBX/vase.fbx", AssetManager::GetTexture("vase")));
		//AssetManager::GetModel("vase")->GetMeshByName("Mesh.001")->SetTexture(AssetManager::GetTexture("plant"));
		//AssetManager::GetModel("vase")->GetMeshByName("Mesh.002")->SetTexture(AssetManager::GetTexture("plant"));

		//AssetManager::AddModel("Bench", Model("Assets/Objects/FBX/Bench.fbx", AssetManager::GetTexture("angled-tiled-floor")));

		AssetManager::AddModel("Tiltedfloor", Model("Assets/Objects/floor.obj", AssetManager::GetTexture("angled-tiled-floor")));

		//AssetManager::AddModel("ceiling_light", Model("Assets/Objects/FBX/ceiling_light.fbx", AssetManager::GetTexture("Industrial_Light")));
		//AssetManager::GetModel("ceiling_light")->GetMeshByName("l1.001")->SetTexture(AssetManager::GetTexture("white_light"));
		//AssetManager::GetModel("ceiling_light")->GetMeshByName("l1")->SetTexture(AssetManager::GetTexture("white_light"));


		AssetManager::AddModel("playertwo", Model("Assets/Objects/FBX/bean_death.dae", "Assets/Objects/player_mesh.obj", AssetManager::GetTexture("uvmap")));
		//AssetManager::AddModel("window", Model("Assets/Objects/FBX/window.fbx", AssetManager::GetTexture("window")));




		//AssetManager::AddModel("fence1", Model("Assets/Objects/fence1.fbx", AssetManager::GetTexture("concrete")));
		//AssetManager::AddModel("fence2", Model("Assets/Objects/fence2.fbx", AssetManager::GetTexture("concrete")));
		//AssetManager::AddModel("fence3", Model("Assets/Objects/fence3.fbx", AssetManager::GetTexture("concrete")));
		//AssetManager::AddModel("floor", Model("Assets/Objects/FBX/floor.fbx", AssetManager::GetTexture("sand")));
		//AssetManager::AddModel("slope", Model("Assets/Objects/FBX/slope.fbx", AssetManager::GetTexture("sand")));
		//AssetManager::AddModel("crate", Model("Assets/Objects/FBX/crate.fbx", AssetManager::GetTexture("crate")));
		//AssetManager::AddModel("cube", Model("Assets/Objects/FBX/cube.fbx", AssetManager::GetTexture("red_glass")));
		//AssetManager::AddModel("cube1", Model("Assets/Objects/FBX/cube.fbx", AssetManager::GetTexture("green_glass")));

		AssetManager::AddModel("model_crate", Model("Assets/Objects/FBX/model_crate.obj", AssetManager::GetTexture("CratePile")));

		AssetManager::AddModel("glock", Model("Assets/Objects/FBX/glock17.fbx", "Assets/Objects/glock17_convex.obj", AssetManager::GetTexture("glock")));
		AssetManager::AddModel("glockhand", Model("Assets/Objects/FBX/glock17_shoot1.dae", AssetManager::GetTexture("glock")));
		AssetManager::GetModel("glockhand")->GetMeshByName("Arms_L_R_Mesh_002-mesh")->SetTexture(AssetManager::GetTexture("arm"));

		AssetManager::AddModel("double_barrel", Model("Assets/Objects/FBX/double_barrel.fbx", "Assets/Objects/db_convex.obj", AssetManager::GetTexture("double_barrel_shotgun_main_barrel")));
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

		//AssetManager::AddModel("map_test1", Model("Assets/Maps/dusty_1.fbx", AssetManager::GetTexture("brick")));
		//AssetManager::AddModel("map_test2", Model("Assets/Maps/other_map1.obj", AssetManager::GetTexture("white")));
		//AssetManager::AddModel("couch", Model("Assets/Objects/FBX/couch.fbx", AssetManager::GetTexture("white")));
		//AssetManager::AddModel("map_indirectLight", Model("Assets/Maps/lightingTest.obj", AssetManager::GetTexture("white")));
		AssetManager::AddModel("water", Model("Assets/Objects/FBX/water_test.obj", AssetManager::GetTexture("white"), 0));
		AssetManager::AddModel("water_COL", Model("Assets/Objects/FBX/water_test.obj", AssetManager::GetTexture("white")));

		//AssetManager::AddModel("GI_map_1", Model("Assets/Maps/cornel_box.obj", AssetManager::GetTexture("cornel")));
		AssetManager::AddModel("GI_map_1", Model("Assets/Maps/sand_box.obj", AssetManager::GetTexture("angled-tiled-floor")));
		//AssetManager::GetModel("GI_map_1")->GetMeshByName("stairs_plane")->ToggleRender(false);

		AssetManager::AddModel("Cube", Model("Assets/Objects/FBX/cube.fbx", AssetManager::GetTexture("metalic")));
		AssetManager::AddModel("seafloor", Model("Assets/Objects/FBX/seafloor.obj", AssetManager::GetTexture("white")));

		AssetManager::AddModel("pool", Model("Assets/Objects/FBX/pool.obj", AssetManager::GetTexture("angled-tiled-floor")));
		AssetManager::AddModel("pool_water", Model("Assets/Objects/FBX/pool_water.obj", AssetManager::GetTexture("white"), 0));


		AssetManager::AddModel("room1", Model("Assets/Maps/room1.obj", AssetManager::GetTexture("white")));


		AssetManager::AddModel("target", Model("Assets/Objects/FBX/target.obj", AssetManager::GetTexture("white")));

		AssetManager::AddModel("breakable_crate_t", Model("Assets/Objects/FBX/Crate/top.obj", AssetManager::GetTexture("white")));
		AssetManager::AddModel("breakable_crate_b", Model("Assets/Objects/FBX/Crate/bottom.obj", AssetManager::GetTexture("white")));
		AssetManager::AddModel("breakable_crate_l", Model("Assets/Objects/FBX/Crate/side.obj", AssetManager::GetTexture("white")));

		AssetManager::AddModel("uni_float", Model("Assets/Objects/FBX/unicorn.fbx", AssetManager::GetTexture("unicorn")));
		AssetManager::AddModel("uni_float_defalated", Model("Assets/Objects/FBX/unicorn_defalated_rig.fbx", AssetManager::GetTexture("unicorn")));

		//Super laggy
		//AssetManager::AddModel("GI_map_1", Model("Assets/Maps/Sponza/sponza.obj", AssetManager::GetTexture("white")));

		/*
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
		*/
		//AssetManager::AddModel("cat", Model("Assets/Objects/FBX/run_fast.fbx", AssetManager::GetTexture("white")));

		AssetManager::AddModel("ak47", Model("Assets/Objects/FBX/ak47.fbx", "Assets/Objects/ak47_convex.obj", AssetManager::GetTexture("ak47")));
		//AssetManager::AddModel("door", Model(Mesh("Assets/Objects/door.obj"), AssetManager::GetTexture("door")));
		//AssetManager::AddModel("door_frame", Model(Mesh("Assets/Objects/door_frame.obj"), AssetManager::GetTexture("door")));
		AssetManager::AddModel("player", Model("Assets/Objects/FBX/player.fbx", AssetManager::GetTexture("uvmap")));

		AssetManager::AddModel("shotgun", Model("Assets/Objects/fbx/remington.fbx", "Assets/Objects/shotgun_convex.obj", AssetManager::GetTexture("shotgun")));

		AssetManager::AddDecal("bullet_hole", AssetManager::GetTexture("bullet_hole"), glm::vec3(0.02, 0.005, 0.02));
		AssetManager::AddDecal("panda_decal", AssetManager::GetTexture("panda"), glm::vec3(1, 0.1, 1));
		AssetManager::AddDecal("flower_decal", AssetManager::GetTexture("flower"), glm::vec3(1, 0.1, 1));
		AssetManager::AddDecal("tank_decal", AssetManager::GetTexture("tank"), glm::vec3(1, 0.1, 1));
		AssetManager::AddDecal("pizza_decal", AssetManager::GetTexture("pizza"), glm::vec3(1, 0.1, 1));
		AssetManager::AddDecal("freaky_decal", AssetManager::GetTexture("freaky"), glm::vec3(1, 0.1, 1));

		//these are diffrent animations from skinnedanimation
		//AnimationManager::AddAnimation(Animation(, "unicorn_deflated"));
		//AnimationManager::AddAnimation(Animation("Assets/Animations/door_close.fbx", "door_close"));	
		//ragdoll = SkinnedAnimation("Assets/Objects/FBX/unicorn_defalated_rig.fbx", AssetManager::GetModel("uni_float_defalated"), 0, "uni_float_defalated");

	}



	void Load() {
		LoadAssets();

		AddGameObject("Tiltedfloor", AssetManager::GetModel("Tiltedfloor"), glm::vec3(0, 0.1, 0), true, 0, Box);
		GetGameObject("Tiltedfloor")->IncludInGI(true);



		//AddGameObject("Cube", AssetManager::GetModel("Cube"), glm::vec3(0, 6, 0), false, 10.0f, Box);
		AddGameObject("room1", AssetManager::GetModel("room1"), glm::vec3(0, 0.1, 0), true, 0, Concave);
		GetGameObject("room1")->IncludInGI(true);

		AddGameObject("seafloor", AssetManager::GetModel("seafloor"), glm::vec3(0, -10, 0), true, 0, Concave);


		AddGameObject("pool", AssetManager::GetModel("pool"), glm::vec3(3, -0.9, 0), true, 0, Concave);
		g_water.emplace_back(std::make_unique<GameObject>("pool_water", AssetManager::GetModel("pool_water"), glm::vec3(3, -2.6, 0), true, 0, None));
		g_water[0]->GetRigidBody()->setUserIndex(0);
		g_water[0]->GetRigidBody()->setUserPointer((void*)ObjectType::WATER);

		g_water.emplace_back(std::make_unique<GameObject>("water", AssetManager::GetModel("water"), glm::vec3(0, -2, 0), true, 0, None));
		g_water[1]->GetRigidBody()->setUserIndex(1);
		g_water[1]->GetRigidBody()->setUserPointer((void*)ObjectType::WATER);
		m_seaLevel = -2;

		g_triggers.emplace_back(std::make_unique<TriggerCollider>(glm::vec3(3, -1.3, -1.8), glm::vec3(4, 1.8, 4)));

		AddGlass("shaderBall_glass", AssetManager::GetModel("shaderBall"), glm::vec3(0, 0, 0), false, 1.0, Box);
		AddGlass("cubeGlass", AssetManager::GetModel("cubeGlass"), glm::vec3(-3, 0, 0), false, 0.0, Convex);


		AddGlass("shaderBall_glass", AssetManager::GetModel("shaderBall"), glm::vec3(-6.46, -1, 14), false, 0.0, Box);
		AddGameObject("Crates", AssetManager::GetModel("model_crate"), glm::vec3(3, 0, 3), true, 0, Convex);
		//World::AddGameObject("ladder_object", AssetManager::GetModel("ladder"), glm::vec3(0, 0, 0), true, 0, Concave);
		//World::GetGameObject("ladder_object")->SetRotationX(-1.5708f);

		AddGameObject("ladder_object", AssetManager::GetModel("ladder"), glm::vec3(0, 0, 0), true, 0, Concave);
		GetGameObject("ladder_object")->SetRotationX(-1.5708f);


		AddGameObject("uni_float", AssetManager::GetModel("uni_float"), glm::vec3(6, 2, 0), true, 2, Convex);
		AudioManager::AddSound("Assets/Audio/balloonpop.wav", "pop", glm::vec3(0, 0, 0), 20, 1.8);
		Destructible desruct;
		//desruct.CreateConvex(AssetManager::GetModel("uni_float_defalated"));
		desruct.m_destoryed_object = "uni_float_defalated";
		desruct.m_destoryed_sound = "pop";

		GetGameObject("uni_float")->destructable = desruct;

		AddGameObject("uni_float1", AssetManager::GetModel("uni_float"), glm::vec3(3, 2, 2), true, 0.5, Convex);
		GetGameObject("uni_float1")->GetRigidBody()->setUserPointer((void*)ObjectType::DESTORYABLE);
		GetGameObject("uni_float1")->destructable = desruct;
		GetGameObject("uni_float1")->m_buoyancy = 6.0;

		AddGameObject("uni_float2", AssetManager::GetModel("uni_float"), glm::vec3(3, 4, 2), true, 0.5, Convex);
		GetGameObject("uni_float2")->GetRigidBody()->setUserPointer((void*)ObjectType::DESTORYABLE);
		GetGameObject("uni_float2")->destructable = desruct;
		GetGameObject("uni_float2")->m_buoyancy = 6.0;

		AddGameObject("uni_float3", AssetManager::GetModel("uni_float"), glm::vec3(3, 5, 2), true, 0.5, Convex);
		GetGameObject("uni_float3")->GetRigidBody()->setUserPointer((void*)ObjectType::DESTORYABLE);
		GetGameObject("uni_float3")->destructable = desruct;
		GetGameObject("uni_float3")->m_buoyancy = 6.0;

		//AddGameObject("crate_t", AssetManager::GetModel("breakable_crate_t"), glm::vec3(6, 2, 0), true, 2, Convex);
		//AddGameObject("crate_b", AssetManager::GetModel("breakable_crate_b"), glm::vec3(6, 2, 0), true, 2, Convex);
		//AddGameObject("crate_l", AssetManager::GetModel("breakable_crate_l"), glm::vec3(6, 2, 0), true, 2, Convex);
		//AddGameObject("crate_l2", AssetManager::GetModel("breakable_crate_l"), glm::vec3(6, 2, 0), true, 2, Convex);
		//GetGameObject("crate_l2")->setRotation(glm::vec3(0,glm::radians(90.0f),0));

		//TODO clean all this up
		//btTransform localA, localB;

		//localA.setIdentity(); // or offset if needed
		//localB.setIdentity();

		//btFixedConstraint* fixed = new btFixedConstraint(*GetGameObject("crate_t")->GetRigidBody(), *GetGameObject("crate_b")->GetRigidBody(), localA, localB);
		//PhysicsManagerBullet::GetDynamicWorld()->addConstraint(fixed, true);
		//btFixedConstraint* fixed1 = new btFixedConstraint(*GetGameObject("crate_t")->GetRigidBody(), *GetGameObject("crate_l")->GetRigidBody(), localA, localB);
		//PhysicsManagerBullet::GetDynamicWorld()->addConstraint(fixed1, true);
		//localB.setOrigin(btVector3(1.0, 0.0, 0.0)); // local offset in bodyB's space

		//btFixedConstraint* fixed2 = new btFixedConstraint(*GetGameObject("crate_l")->GetRigidBody(), *GetGameObject("crate_l2")->GetRigidBody(), localA, localB);
		//PhysicsManagerBullet::GetDynamicWorld()->addConstraint(fixed2, true);


		AddGameObject("targetHolder", AssetManager::GetModel("target"), glm::vec3(6, 2, 0), true, 0, Convex);
		AddGameObject("target", AssetManager::GetModel("target"), glm::vec3(6, 1.5, 0), true, 5, Convex);
		AddGameObject("targetHolder2", AssetManager::GetModel("target"), glm::vec3(6, 2, 0.5), true, 0, Convex);
		AddGameObject("target2", AssetManager::GetModel("target"), glm::vec3(6, 1.5, 0.5), true, 5, Convex);
		AddGameObject("targetHolder3", AssetManager::GetModel("target"), glm::vec3(6, 2, 1), true, 0, Convex);
		AddGameObject("target3", AssetManager::GetModel("target"), glm::vec3(6, 1.5, 1), true, 5, Convex);

		btVector3 pivotInA(0.0f, -0.22f, 0.0f);    // Hinge is at origin in body A
		btVector3 pivotInB(0.0f, 0.22f, 0.0f);    // Hinge is 1 unit right in body B's local space
		btVector3 axisInA(0.0f, 0.0f, 1.0f);     // Rotate around Y
		btVector3 axisInB(0.0f, 0.0f, 1.0f);     // Same rotation axis in B

		// Create hinge constraint
		btHingeConstraint* hinge = new btHingeConstraint(
			*GetGameObject("targetHolder")->GetRigidBody(), *GetGameObject("target")->GetRigidBody(),
			pivotInA, pivotInB,
			axisInA, axisInB,
			false // <-- Add this
		);
		// Optional: limit the hinge rotation
		hinge->setLimit(-SIMD_PI / 1.2, SIMD_PI / 1.2);
		// Add to dynamics world
		PhysicsManagerBullet::GetDynamicWorld()->addConstraint(hinge, true);

		// Create hinge constraint
		btHingeConstraint* hinge1 = new btHingeConstraint(
			*GetGameObject("targetHolder2")->GetRigidBody(), *GetGameObject("target2")->GetRigidBody(),
			pivotInA, pivotInB,
			axisInA, axisInB,
			false // <-- Add this
		);

		// Optional: limit the hinge rotation
		hinge1->setLimit(-SIMD_PI / 1.2, SIMD_PI / 1.2);
		// Add to dynamics world
		PhysicsManagerBullet::GetDynamicWorld()->addConstraint(hinge1, true);

		// Create hinge constraint
		btHingeConstraint* hinge2 = new btHingeConstraint(
			*GetGameObject("targetHolder3")->GetRigidBody(), *GetGameObject("target3")->GetRigidBody(),
			pivotInA, pivotInB,
			axisInA, axisInB,
			false // <-- Add this
		);
		// Optional: limit the hinge rotation
		hinge2->setLimit(-SIMD_PI / 1.2, SIMD_PI / 1.2);
		// Add to dynamics world
		PhysicsManagerBullet::GetDynamicWorld()->addConstraint(hinge2, true);

		// Sets renderer
		std::vector<std::string> faces{
			"Assets/Skybox/daylight/right.png",
				"Assets/Skybox/daylight/left.png",
				"Assets/Skybox/daylight/top.png",
				"Assets/Skybox/daylight/bottom.png",
				"Assets/Skybox/daylight/front.png",
				"Assets/Skybox/daylight/back.png"
		};

		envLight.sky = SkyBox(faces);
		//Average light of skybox
		envLight.indirectLight = glm::vec3(0.188, 0.278, 0.4);
		//envLight.indirectLight = glm::vec3(0.569, 0.69, 0.965);
		std::cout << "test load\n";

		g_lights.emplace_back(glm::vec3(0, 10, 0), glm::vec3(1, 0.996, 0.82), 10, 50);
		g_lights.back().SetUpShadows();
		g_lights.emplace_back(glm::vec3(0, 6, -2.4), glm::vec3(1, 0.922, 0.678) * 7.5f, 0.07, 0.017);
		g_lights.back().SetUpShadows();
		std::cout << "test load 1\n";

	}

	void Update(float deltaTime) {
		
		for (size_t i = 0; i < g_sprites.size(); /* no i++ here */) {
			if (glfwGetTime() >= g_sprites[i].birthTime + g_sprites[i].timePerFrame) {
				g_sprites[i].frameindex++;
				g_sprites[i].birthTime = glfwGetTime();
			}

			if (g_sprites[i].columncount * g_sprites[i].rowcount <= g_sprites[i].frameindex) {
				g_sprites.erase(g_sprites.begin() + i);
			}
			else {
				++i;
			}
		}


		for (int i = 0; i < g_triggers.size(); i++) {

		}
		for (int i = 0; i < g_lights.size(); i++) {
			if (glm::distance(g_lights[i].position, Player::getPosition()) < g_lights[i].radius * 1.4f && g_lights[i].Dynamic)
				g_lights[i].GenerateShadows();
		}
		for (int i = 0; i < g_objects.size(); i++) {
			g_objects[i]->Update();
			btRigidBody* rb = g_objects[i]->GetRigidBody().get();
			if (!rb || g_objects[i]->m_mass == 0)
				continue;

			glm::vec3 overlap = g_triggers[0]->CheckOverlap(rb);
			bool insideTrigger = overlap.x > 0 && overlap.y > 0 && overlap.z > 0;

			//A little hack for the unicorn's if not they dont float on the middle +0.3
			if (g_objects[i]->GetPosition().y < m_seaLevel + 0.3 || insideTrigger) {

				if (g_objects[i]->destructable.m_destoryed) {
					float buoyancy = g_objects[i]->m_buoyancy; // e.g. 8
					float upwardForce = 4;
					rb->applyCentralForce(btVector3(0, upwardForce, 0));
					continue;
				}

				float depth = 0.0;
				if (g_objects[i]->GetPosition().y < m_seaLevel + 0.3)
					depth = m_seaLevel + 0.3 - g_objects[i]->GetPosition().y;
				else {
					depth = overlap.y;
				}
				//rb->setGravity(btVector3(0, g_objects[i]->m_buoyancy * depth, 0));

				float buoyancy = g_objects[i]->m_buoyancy; // e.g. 8
				float upwardForce = buoyancy * depth * 8;
				float wave = glm::sin(ENGINE_PI / 2 * deltaTime) * 1;
				upwardForce += wave > 0.0f ? wave : 0.0f;
				rb->applyCentralForce(btVector3(0, upwardForce, 0));
				rb->setDamping(0.5f, 0.5f);

				btQuaternion currentRot = rb->getOrientation();
				btQuaternion targetRot(btVector3(1, 0, 0), -SIMD_HALF_PI);


				btQuaternion error = targetRot * currentRot.inverse();
				error.normalize();

				btVector3 axis(error.x(), error.y(), error.z());
				float angle = 2 * acos(error.w());
				if (angle > SIMD_PI) angle -= SIMD_2_PI;

				if (axis.length2() > 0.0001f) {
					axis.normalize();

					float stiffness = 3.0f;
					btVector3 correctiveTorque = axis * angle * stiffness;

					float damping = 4.5f;
					btVector3 angularVel = rb->getAngularVelocity();
					btVector3 dampingTorque = -angularVel * damping;

					rb->applyTorque(correctiveTorque + dampingTorque);
				}

			}
			else {
				//Object probably wont ever leave the water so no need to do this
				//rb->setDamping(0.0f, 0.0f);
			}

		}
		for (int i = 0; i < g_glass.size(); i++) {
			g_glass[i]->Update();
		}


		for (int i = 0; i < m_gunPickups.size(); i++)
		{
			int results = m_gunPickups[i].Interact();
			if (results) {
				m_gunPickups.erase(m_gunPickups.begin() + i);
				continue;
			}
		}
	}


	EnviromentLighting GetEnviromentLighting() {
		return envLight;
	}

	Light* GetLight(int i) {
		return &g_lights[i];
	}

	void SetLight(Light&& light, int index) {
		if (index < g_lights.size()) {
			//g_lights[index] = std::move(light);
		}
		else {
			g_lights.emplace_back(std::move(light));
		}
	}
	void RemoveLight(int index) {
		//if (index < g_lights.size())
			//g_lights.erase(g_lights.begin() + index);
	}

	size_t AddGameObject(std::unique_ptr<GameObject> gameobject) {
		gameobject->GetRigidBody()->setUserPointer((void*)(g_objects.size()));
		g_objects.emplace_back(std::move(gameobject));
		return g_objects.size() - 1;
	}

	size_t AddGlass(std::string name, Model* model, glm::vec3 position, bool save, float mass, ColliderShape shape) {
		// Add to g_glass vector
		g_glass.emplace_back(std::make_unique<GameObject>(name, model, position, save, mass, shape));
		size_t index = g_glass.size() - 1;  // Get index in g_glass
		g_glass[index]->GetRigidBody()->setUserIndex(index);
		g_glass[index]->GetRigidBody()->setUserPointer((void*)ObjectType::GLASS);

		return index;
	}

	size_t AddGameObject(std::string name, Model* model, glm::vec3 position, bool save, float mass, ColliderShape shape) {
		g_objects.emplace_back(std::make_unique<GameObject>(name, model, position, save, mass, shape));
		g_objects.back()->GetRigidBody()->setUserIndex(static_cast<int>(g_objects.size() - 1));
		return g_objects.size() - 1;
	}

	GameObject* GetGameObject(std::string name) {
		for (int i = 0; i < g_objects.size(); i++) {
			if (g_objects[i]->GetName() == name)
				return g_objects[i].get();
		}
		std::cout << "Object: " << name << " doesnt exsit \n";
		return nullptr;
	}


	void RemoveGameObject(std::string name) {
		for (int i = 0; i < g_objects.size(); i++) {
			if (g_objects[i]->GetName() == name) {
				PhysicsManagerBullet::GetDynamicWorld()->removeRigidBody(g_objects[i]->GetRigidBody().get());
				g_objects.erase(g_objects.begin() + i);
			}
		}
	}

	void Destory() {

	}
}

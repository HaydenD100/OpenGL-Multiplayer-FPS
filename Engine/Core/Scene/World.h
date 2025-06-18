#pragma once
#include <iostream>
#include <vector>
#include <ostream>

#include "Engine/Audio/Audio.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Core/Camera.h"
#include "Engine/Core/Camera.h"
#include "Engine/Backend.h"
#include "Engine/Game/Player.h"
#include "Engine/Game/Door.h"
#include "Engine/Game/Gun.h"
#include "Engine/Animation/Animation.h"
#include "Engine/Game/Door.h"
#include "Engine/Game/Gun.h"
#include "Engine/Game/Crate.h"
#include "Engine/Core/Lights/Light.h"
#include "Engine/Physics/BulletPhysics.h"
#include "Engine/Animation/Animation.h"
#include "Engine/Animation/SkinnedAnimatior.h"
#include "Engine/Pathfinding/Pathfinding.h"
#include "Engine/Physics/TriggerCollider.h"

namespace World
{
	void Load();
	void Update(float dt);

	void AddGunPickUp(GunPickUp gunpickup);

	void LoadAssets();

	Light* GetLight(int i);
	void SetLight(Light&& light, int index);
	void RemoveLight(int index);

	EnviromentLighting GetEnviromentLighting();

	unsigned long long AddGameObject(std::string name, Model* model, glm::vec3 position, bool save, float mass, ColliderShape shape);
	size_t AddGlass(std::string name, Model* model, glm::vec3 position, bool save, float mass, ColliderShape shape);

	size_t AddGameObject(std::unique_ptr<GameObject> gameobject);
	GameObject* GetGameObject(std::string name);

	void RemoveGameObject(std::string name);

	extern EnviromentLighting envLight;

	extern std::vector<std::unique_ptr<GameObject>> g_water;
	extern std::vector<std::unique_ptr<GameObject>> g_objects;
	extern std::vector<std::unique_ptr<GameObject>> g_glass;
	extern std::vector<GunPickUp> m_gunPickups;
	extern std::vector<Light> g_lights;
	extern std::vector<std::unique_ptr<TriggerCollider>> g_triggers;
	extern std::vector<Sprite> g_sprites;

	extern float m_seaLevel;
};


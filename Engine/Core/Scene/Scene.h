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



struct EnviromentLighting {
	SkyBox sky = SkyBox();
	glm::vec3 indirectLight = glm::vec3(0, 0, 0);
};
 
class Scene
{
public:
	Scene();

	void Load();
	void Update(float dt);

	void AddGunPickUp(GunPickUp gunpickup);

	void LoadAssets();
 
	Light* GetLight(int i);
	void SetLight(Light light, int index);
	void RemoveLight(int index);

	EnviromentLighting GetEnviromentLighting();
	std::vector<GameObject*> NeedRenderingObjects();

	unsigned long long AddGameObject(std::string name, Model* model, glm::vec3 position, bool save, float mass, ColliderShape shape);
	size_t AddGlass(std::string name, Model* model, glm::vec3 position, bool save, float mass, ColliderShape shape);

	size_t AddGameObject(GameObject gameobject);
	GameObject* GetGameObject(std::string name);

	void RemoveGameObject(std::string name);

	std::vector<GameObject> g_water;
	std::vector<GameObject> g_objects;
	std::vector<GameObject> g_glass;
	std::vector<GunPickUp> m_gunPickups;

	std::vector<Light> g_lights;




private:
	std::vector<GameObject*> NeedRendering;
	EnviromentLighting envLight;

	//holds the model shader ID for Gemoetry
	GLuint ModelMatrixId;

	SkinnedAnimation m_running;
	SkinnedAnimation cute_cat;

};

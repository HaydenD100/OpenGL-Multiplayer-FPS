#pragma once
#include <iostream>
#include <vector>
#include <ostream>

#include "Engine/Audio/Audio.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Core/GameObject.h"
#include "Engine/Core/Renderer.h"
#include "Engine/Core/Camera.h"
#include "Engine/Core/Camera.h"
#include "Engine/Backend.h"
#include "Engine/Game/Player.h"
#include "Engine/Game/Door.h"
#include "Engine/Game/Gun.h"
#include "Engine/Core/Animation.h"
#include "Engine/Game/Door.h"
#include "Engine/Game/Gun.h"
#include "Engine/Game/Crate.h"
#include "Engine/Core/Lights/Light.h"
#include "Engine/Physics/BulletPhysics.h"
#include "Engine/Core/Animation.h"
 
class Scene
{
public:
	Scene();

	void Load();
	void Update(float dt);
	void RenderObjects();
	void RenderObjects(const char* shaderName);

	void AddGunPickUp(GunPickUp gunpickup);

	int GetGunPickUpSize();
	Crate* GetCrate(std::string name);
	Model* GetModel(std::string name);
private:
	SkyBox sky = SkyBox();
	// Objects
	std::vector<Door> doors;
	std::vector<Crate> crates;
	std::vector<GunPickUp> gunPickUps;
	std::vector<Light> lights;

	//holds the models
	std::map<std::string, Model> models;
};

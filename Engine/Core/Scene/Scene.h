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
	void RenderObjects(GLuint programid);
	void RenderAllObjects(GLuint programid);

	void AddGunPickUp(GunPickUp gunpickup);

	void LoadAssets();

	int GetGunPickUpSize();
	Crate* GetCrate(std::string name);
	std::vector<Light> getLights();

	SkyBox GetSkyBox();

	std::vector<GameObject*> NeedRenderingObjects();

private:
	std::vector<GameObject*> NeedRendering;
	SkyBox sky = SkyBox();
	// Objects
	std::vector<Door> doors;
	std::vector<Crate> crates;
	std::vector<GunPickUp> gunPickUps;
	std::vector<Light> lights;

	//holds the models

	GLuint ModelMatrixId;
};

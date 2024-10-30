#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <fstream>
#include <iostream>

#include "Engine/Core/GameObject.h"
#include "Engine/Core/Common.h"
#include "Engine/Core/Renderer.h"
#include "Engine/Core/Texture.h"
#include "Engine/Core/Decal.h"

// Json stuff
#include "Loaders/nlohmann/json.hpp"
using json = nlohmann::json;

namespace AssetManager
{
	void Init();
	void SaveAssets(const char* path);
	void LoadAssets(const char* path);

	// returns index of object
	size_t AddGameObject(GameObject gameobject);
	unsigned long long AddGameObject(std::string name, Model* model, glm::vec3 position, bool save, float mass, ColliderShape shape);

	unsigned long long AddDecal(std::string name, Texture* texture, glm::vec3 size);
	Decal* GetDecal(std::string name);
	Decal* GetDecal(int index);
	size_t DecalSize();


	std::vector<Decal>* GetAllDecals();
	void ClearAllDecalInstances();

	unsigned long long AddDecalInstance(glm::vec3 position, glm::vec3 normal, Decal* decal, GameObject* Parent);
	DecalInstance* GetDecalInstance(int index);
	std::vector<DecalInstance>* GetAllDecalInstances();
	size_t DecalInstanceSize();


	size_t AddTexture(Texture texture);
	size_t AddTexture(const char* name, const char* path, float roughness, float metalic);
	size_t AddTexture(const char* name, const char* path, const char* normalPath, float roughness, float metalic);
	size_t AddTexture(const char* name, const char* path, const char* normaPath, const char* RoughnessPath, const char* MetalicPath);

	Model* GetModel(std::string name);
	Model* AddModel(std::string name, const char* path, Texture* texture);
	Model* AddModel(std::string name, Model model);

	size_t AddSkinnedAnimation(SkinnedAnimation skinnedanimation);
	SkinnedAnimation* GetSkinnedAnimation(std::string name);


	void RemoveGameObject(std::string name);
	void RemoveGameObject(int index);
	void ClearAssets();

	Texture* GetMissingTexture();

	void CleanUp();

	GameObject* GetGameObject(std::string name);
	GameObject* GetGameObject(int index);
	std::vector<GameObject> GetAllGameObjects();
	size_t GetGameObjectsSize();
	size_t GetDecalsSize();

	Texture* GetTexture(std::string name);
}

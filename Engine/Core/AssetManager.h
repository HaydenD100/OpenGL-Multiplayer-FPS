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

	unsigned long long AddDecal(glm::vec3 position, glm::vec3 normal, glm::vec3 scale, Texture* texture,
	                            GameObject* parent);
	Decal* GetDecal(int index);
	std::vector<Decal>* GetAllDecals();

	size_t AddTexture(Texture texture);
	size_t AddTexture(const char* name, const char* path);
	size_t AddTexture(const char* name, const char* path, const char* normalPath);
	size_t AddTexture(const char* name, const char* path, const char* normaPath, const char* SpecularPath);


	void RemoveGameObject(std::string name);
	void RemoveGameObject(int index);

	void CleanUp();

	GameObject* GetGameObject(std::string name);
	GameObject* GetGameObject(int index);
	std::vector<GameObject> GetAllGameObjects();
	size_t GetGameObjectsSize();
	size_t GetDecalsSize();

	Texture* GetTexture(std::string name);
}

#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <fstream>
#include <iostream>

#include "GameObject.h"
#include "Common.h"
#include "Renderer.h"
#include "Decal.h"

// Json stuff
#include "Loaders/nlohmann/json.hpp"
using json = nlohmann::json;


namespace AssetManager
{
	void Init();
	void SaveAssets(const char* path);
	void LoadAssets(const char* path);

	// returns index of object
	int AddGameObject(GameObject gameobject);
	int AddGameObject(std::string name, const char* path,Texture* texture, glm::vec3 position, bool save, float mass, ColliderShape shape);

	int AddDecal(glm::vec3 position, glm::vec3 normal, glm::vec3 scale, Texture* texture, GameObject* parent);
	Decal* GetDecal(int index);
	std::vector<Decal>* GetAllDecals();

	int AddTexture(Texture texture);
	int AddTexture(const char* name, const char* path);
	int AddTexture(const char* name, const char* path, const char* normalPath);


	void RemoveGameObject(std::string name);
	void RemoveGameObject(int index);

	void CleanUp();

	GameObject* GetGameObject(std::string name);
	GameObject* GetGameObject(int index);
	std::vector<GameObject> GetAllGameObjects();
	int GetGameObjectsSize();
	int GetDecalsSize();

	Texture* GetTexture(std::string name);

};


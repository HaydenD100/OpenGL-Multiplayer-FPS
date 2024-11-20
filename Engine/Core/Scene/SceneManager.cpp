#include "SceneManager.h"

namespace SceneManager
{
	std::vector<Scene> scenes;
	int currentScene;

	void SceneManager::Init() {
		currentScene = 0;
	}

	size_t SceneManager::CreateScene(Scene scene) {
		scenes.push_back(scene);
		return scenes.size() - 1;
	}
	
	void SceneManager::LoadScene(int index) {
		if (index < scenes.size())
			currentScene = index;
		scenes[currentScene].Load();
		std::cout << "Loaded Scene " << index << std::endl;
	}

	void SceneManager::Update(float dt) {
		scenes[currentScene].Update(dt);
	}
	


	Scene* SceneManager::GetCurrentScene() {
		return &scenes[currentScene];
	}
}

#include "AssetManager.h"
#include "Engine/Core/DecalInstance.h"
#include "Engine/Core/Scene/SceneManager.h"

namespace AssetManager
{
	//will replace the decals starting from 0 if the max decals has been reached, replacing the oldest decals to make room for the new decals
	//setting this number to anything hire then 0 will reserve the first n spots for decals that wont be replaced
	int nextDecalSpot = 0;

	std::vector<Texture> Textures;
	std::vector<Decal> Decals;
	std::vector<DecalInstance> DecalInstances;
	std::vector<SkinnedAnimation> SkinnedAnimations;

	std::map<std::string, Model> models;

	Texture missing_texture;

	const char* save_path = "Assets/Saves/save1.json";


	std::string path;

	void AssetManager::Init() {
		Textures.clear();
		Decals.clear();
		DecalInstances.clear();

		DecalInstances.reserve(MAXDECALS);
		DecalInstances.resize(MAXDECALS);

		missing_texture = Texture("missing_texture", "Assets/Textures/missing_texture.png", 0, 0);

	}

	void AssetManager::ClearAssets() {
		Textures.clear();
		models.clear();
	}

	void AssetManager::LoadAssets(const char* loadJson) {
		std::ifstream f(loadJson);
		
		json data = json::parse(f);
		//if (data == NULL)
			//return;
		
		if (data["GameObjects"] == NULL)
			return;
		std::cout << "loaded object count " << data["GameObjects"].size() << std::endl;

		for (int gameobject = 0; gameobject < data["GameObjects"].size(); gameobject++)
		{
			std::string name = data["GameObjects"][gameobject]["name"];

			glm::vec3 position = glm::vec3(data["GameObjects"][gameobject]["positionX"], data["GameObjects"][gameobject]["positionY"], data["GameObjects"][gameobject]["positionZ"]);
			glm::vec3 rotation = glm::vec3(data["GameObjects"][gameobject]["rotationX"], data["GameObjects"][gameobject]["rotationY"], data["GameObjects"][gameobject]["rotationZ"]);

			//glm::vec3 rotation = glm::vec3(data["GameObjects"][gameobject][5], data["GameObjects"][gameobject][6], data["GameObjects"][gameobject][7]);
			//glm::vec3 scale = glm::vec3(data["GameObjects"][gameobject][8], data["GameObjects"][gameobject][9], data["GameObjects"][gameobject][10]);
			GameObject* p_gameobject = SceneManager::GetCurrentScene()->GetGameObject(name);
			if (p_gameobject != nullptr) {
				p_gameobject->setPosition(position);
			}
		}
		
		for (int i = 0; i < data["lights"].size(); i++) {
			glm::vec3 position;
			position.x = data["lights"][i]["positionX"];
			position.y = data["lights"][i]["positionY"];
			position.z = data["lights"][i]["positionZ"];

			glm::vec3 colour;
			colour.x = data["lights"][i]["colourR"];
			colour.y = data["lights"][i]["colourG"];
			colour.z = data["lights"][i]["colourB"];


			Light light = Light(position,colour, data["lights"][i]["strength"], data["lights"][i]["radius"]);
			SceneManager::GetCurrentScene()->SetLight(light, i);
		}
		
	}

	// TODO: this doesn't work yet, still needs to be updated
	void AssetManager::SaveAssets(const char* path) {
		json save;
		std::vector<json> SerializedGameObjects;
		std::vector<json> SerializedLights;

		
		// name,pos,rotation,scale
		for (int i = 0; i < SceneManager::GetCurrentScene()->g_objects.size(); i++) {
			GameObject* gameobject = &SceneManager::GetCurrentScene()->g_objects[i];
			json gameobjectJSON;
			gameobjectJSON["name"] = gameobject->GetName();
			glm::vec3 position = gameobject->GetPosition();
			gameobjectJSON["positionX"] = position.x;
			gameobjectJSON["positionY"] = position.y;
			gameobjectJSON["positionZ"] = position.z;

			glm::vec3 rotation = gameobject->getRotation();
			gameobjectJSON["rotationX"] = rotation.x;
			gameobjectJSON["rotationY"] = rotation.y;
			gameobjectJSON["rotationZ"] = rotation.z;
			SerializedGameObjects.push_back(gameobjectJSON);
		}

		for (int i = 0; i < SceneManager::GetCurrentScene()->g_lights.size(); i++) {
			Light* light = &SceneManager::GetCurrentScene()->g_lights[i];
			json lightJSON;
			lightJSON["colourR"] = light->colour.r;
			lightJSON["colourG"] = light->colour.g;
			lightJSON["colourB"] = light->colour.b;

			lightJSON["positionX"] = light->position.x;
			lightJSON["positionY"] = light->position.y;
			lightJSON["positionZ"] = light->position.z;

			lightJSON["radius"] = light->radius;
			lightJSON["strength"] = light->strength;
			SerializedLights.push_back(lightJSON);
		}
		
		save["GameObjects"] = SerializedGameObjects;
		save["lights"] = SerializedLights;
		// Write JSON object to file
		std::ofstream file(path);
		if (file.is_open()) {
			file << std::setw(4) << save << std::endl;
			file.close();
			std::cout << "JSON data has been written to " << path << std::endl;
		}
		else {
			std::cerr << "Failed to open " << path << std::endl;
		}
	}

	// Returns index of object

	size_t AddDecal(std::string name, Texture* texture, glm::vec3 size) {
		Decals.push_back(Decal(name, texture,size));
		return Decals.size() - 1;
	}
	Decal* GetDecal(std::string name) {
		for (int i = 0; i < Decals.size(); i++) {
			if (Decals[i].GetName() == name)
				return &Decals[i];
		}
		return nullptr;
	}
	Decal* GetDecal(int index) {
		return &Decals[index];
	}
	size_t DecalSize() {
		return Decals.size();
	}
	void ClearAllDecalInstances() {
		for (int i = 0; i < Decals.size(); i++) {
			Decals[i].ClearInstace();
		}
	}


	unsigned long long AddDecalInstance(glm::vec3 position, glm::vec3 normal, Decal* decal, GameObject* Parent) {
		if (nextDecalSpot < MAXDECALS) {
			DecalInstances[nextDecalSpot] = DecalInstance(position, normal, decal, Parent);
			nextDecalSpot++;
		}
		else
			nextDecalSpot = 0;

		return Decals.size() - 1;
		
	}
	DecalInstance* GetDecalInstance(int index){
		return &DecalInstances[index];
	}

	std::vector<DecalInstance>* GetAllDecalInstances() {
		return &DecalInstances;
	}
	size_t DecalInstanceSize() {
		return DecalInstances.size();
	}


	Model* AssetManager::GetModel(std::string name) {
		if (&models[name] == nullptr)
			std::cout << "Model: " << name << " doesnt exsit \n";
		return &models[name];
	}
	Model* AssetManager::AddModel(std::string name, const char* path, Texture* texture) {
		models[name] = Model(path, texture);
		models[name].SetName(name);
		return &models[name];
	}
	Model* AssetManager::AddModel(std::string name, Model model) {
		models[name] = model;
		return &models[name];
	}
	


	std::vector<Decal>* GetAllDecals() {
		return &Decals;
	}

	size_t AssetManager::AddTexture(Texture texture) {
		//Make sure textures cant have the same name
		for (int i = 0; i < Textures.size(); i++) {
			if (Textures[i].GetName() == texture.GetName())
				return i;
		}
		Textures.push_back(texture);
		return Textures.size() - 1;
	}

	size_t AssetManager::AddTexture(const char* name, const char* path, float roughness, float metalic) { 
		Textures.push_back(Texture(name, path, roughness, metalic));
		return Textures.size() - 1;
	}

	size_t AddTexture(const char* name, const char* path, const char* normalPath, float roughness, float metalic) {
		Textures.push_back(Texture(name, path,normalPath, roughness, metalic));
		return Textures.size() - 1;
	}	
	size_t AddTexture(const char* name, const char* path, const char* normalPath, const char* RoughnessPath, const char* MetalicPath) {
		Textures.push_back(Texture(name, path, normalPath, RoughnessPath, MetalicPath));
		return Textures.size() - 1;
	}
	Texture* GetMissingTexture() {
		return &missing_texture;
	}
	void AssetManager::CleanUp() {

	}
	size_t AssetManager::GetDecalsSize() {
		return Decals.size();
	}
	Texture* AssetManager::GetTexture(std::string name) {
		for (int i = 0; i < Textures.size(); i++) {
			if (Textures[i].GetName() == name)
				return &Textures[i];
		}
		std::cout << "Texture: " << name << " doesnt exsit \n";

		return &missing_texture;
	}

	size_t AddSkinnedAnimation(SkinnedAnimation skinnedanimation) {
		SkinnedAnimations.push_back(skinnedanimation);
		return SkinnedAnimations.size();
	}
	SkinnedAnimation* GetSkinnedAnimation(std::string name) {
		for (int i = 0; i < SkinnedAnimations.size(); i++) {
			if (SkinnedAnimations[i].GetName() == name)
				return &SkinnedAnimations[i];
		}
		return nullptr;
	}
}

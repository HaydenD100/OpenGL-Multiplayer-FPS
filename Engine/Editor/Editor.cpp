#include "Editor.h"
#include "Engine/Core/UI/UI.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Core/Scene/SceneManager.h"
#include "Engine/Core/Common.h"
#include "Engine/Physics/BulletPhysics.h"

namespace Editor
{
	std::vector<GameObject*> objects;
	void RenderUI() {
		ImGui::Begin("Lights");
		size_t s_light = SceneManager::GetCurrentScene()->GetLightsSize();
		for (int i = 0; i < s_light; i++) {
			ImGui::Text("Light " + i);
			if (ImGui::Button("Remove Light")) {
				SceneManager::GetCurrentScene()->RemoveLight(i);
			}
			Light* light = SceneManager::GetCurrentScene()->GetLight(i);
			ImGui::ColorEdit3("Colour", glm::value_ptr(light->colour));
			ImGui::InputFloat3("Position", glm::value_ptr(light->position));
			ImGui::InputFloat("Radius", &light->radius);
			ImGui::InputFloat("Strength", &light->strength);
		}
		//if (ImGui::Button("Add Light")) {
			//if the index of setlight is greater then the scene lights it will just push_back
			//SceneManager::GetCurrentScene()->SetLight(Light(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), 5, 20), 99999);
		//s}
		ImGui::End();
		ImGui::Begin("Editor");
		if (ImGui::Button("Save Assets")) {
			AssetManager::SaveAssets();
		}
		ImGui::End();

	}
	void Update() {
		if (!ImGui::GetIO().WantCaptureMouse && Input::LeftMousePressed()) {
			glm::vec3 ray = ScreenPointToRayFunc(Input::GetMouseX(), Input::GetMouseY(), Backend::GetWidth(), Backend::GetHeight(), Camera::getViewMatrix(), Camera::getProjectionMatrix());
			glm::vec3 cameraPosition = Camera::GetPosition();
			glm::vec3 out_end = cameraPosition + ray * 1000.0f;

			btCollisionWorld::ClosestRayResultCallback RayCallback(
				btVector3(cameraPosition.x, cameraPosition.y, cameraPosition.z),
				btVector3(out_end.x, out_end.y, out_end.z)
			);
			RayCallback.m_collisionFilterGroup = GROUP_PLAYER;
			RayCallback.m_collisionFilterMask = GROUP_STATIC | GROUP_DYNAMIC;
			PhysicsManagerBullet::GetDynamicWorld()->rayTest(btVector3(cameraPosition.x, cameraPosition.y, cameraPosition.z), btVector3(out_end.x, out_end.y, out_end.z), RayCallback);
			if (RayCallback.m_collisionObject != NULL) {
				GameObject* gameobject = AssetManager::GetGameObject(RayCallback.m_collisionObject->getUserIndex());
				bool alreadyIncluded = false;
				for (GameObject* object : objects) {
					if (gameobject == object)
						alreadyIncluded = true;
				}
				if (gameobject != nullptr && !alreadyIncluded)
				{
					objects.push_back(gameobject);
				}
			}
		}

		for (GameObject* object : objects) {
			RenderObjectInfo(object);
		}
		
	}
	void RenderObjectInfo(GameObject* p_gameobject) {
		static bool positionModified = false;
		static bool rotationModified = false;

		ImGui::Begin(p_gameobject->GetName().c_str());

		// Position
		glm::vec3 position = p_gameobject->getPosition();
		if (ImGui::InputFloat3("Position", glm::value_ptr(position))) {
			positionModified = true;  // Mark position as modified
		}

		// Rotation
		glm::vec3 rotation = p_gameobject->getRotation();
		if (ImGui::InputFloat3("Rotation", glm::value_ptr(rotation))) {
			rotationModified = true;  // Mark rotation as modified
		}

		// Apply changes if modified
		if (positionModified) {
			p_gameobject->setPosition(position);
			positionModified = false;  // Reset the flag
		}
		if (rotationModified) {
			p_gameobject->setRotation(rotation);
			rotationModified = false;  // Reset the flag
		}

		ImGui::End();
	}
};

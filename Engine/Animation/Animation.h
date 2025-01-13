#pragma once
#include <GL/glew.h>
//#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <string>

#include "Engine/Core/GameObject.h"
#include "Engine/Core/AssetManager.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//------------------IMPORTANT--------------------------
//This and the Animation Manager should not be used for animations, this will only transform the gameobject over keyframes while the Skinned Animations and Animator use bones that will munipluate the verticies this was added before I implemented SkinnedAnimations

struct KeyFrame
{
	float duration;
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	KeyFrame(glm::vec3 Position, glm::quat Rotation, glm::vec3 Scale, float Duration);
};

class Animation
{
public:
	Animation(std::string Name);
	Animation(const char* path, std::string Name);
	Animation(std::vector<KeyFrame> Keyframes, std::string Name);

	void AddKeyFrame(KeyFrame Keyframe);
	std::vector<KeyFrame> GetKeyFrames();

	size_t GetKeyFrameSize();
	bool Playing();
	void Stop();
	void Start();
	void Pause();
	void SetKeyFrame(int keyframeIndex);
	void TransformObject();
	std::string GetName();
	void SetGameObject(GameObject* gameobject);

private:
	void NextKeyFrame();
	std::vector<KeyFrame> keyframes;

	float timeStart = 0;
	int currentKeyFrame = 0;
	bool playing = false;

	glm::vec3 initalRotation;
	glm::vec3 initalPosition;

	glm::vec3 startingPosition;
	glm::quat startingRotation;

	std::string name;

	GameObject* gameObject;
};

namespace AnimationManager
{
	void AddAnimation(Animation animation);
	void ClearAnimations();
	void Play(std::string Name, std::string ObjectName);
	void Stop(std::string Name);
	void Pause(std::string Name);
	void Update(float deltaTime);
	bool IsAnimationPlaying(std::string name);
	Animation* GetAnimation(std::string Name);
}

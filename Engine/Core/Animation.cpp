#include "Animation.h"

void KeyFrame::Create(glm::vec3 Position, glm::vec3 Rotation, float Duration) {
	position = Position;
	rotation = Rotation;
	duration = Duration;
}

Animation::Animation(std::string Name) {
	name = Name;
}

Animation::Animation(std::vector<KeyFrame> Keyframes, std::string Name) {
	keyframes = Keyframes;
	name = Name;
}

void Animation::AddKeyFrame(KeyFrame Keyframe) {
	keyframes.push_back(Keyframe);
}

int Animation::GetKeyFrameSize() {
	return keyframes.size();
}

bool Animation::Playing() {
	return playing;
}

void Animation::Stop() {
	playing = false;
	currentKeyFrame = -1;
	gameObject = nullptr;
}

void Animation::Start() {
	playing = true;
}

void Animation::Pause() {
	playing = false;
}

void Animation::SetKeyFrame(int keyframeIndex) {
	if (keyframeIndex < keyframes.size())
		currentKeyFrame = keyframeIndex;
}

void Animation::SetGameObject(GameObject* gameobject) {
	this->gameObject = gameobject;
}

void Animation::Transform() {
	float t = glfwGetTime() / timeStart + keyframes[currentKeyFrame].duration;
	if (t >= 1) {
		NextKeyFrame();
		return;
	}
	glm::vec3 newPosition = (1 - t) * startingPosition  + t * keyframes[currentKeyFrame].position;
	glm::vec3 newRotation = (1 - t) * startingRotation + t * keyframes[currentKeyFrame].rotation;

	gameObject->setPosition(newPosition);
	gameObject->setRotation(newRotation);
}

void Animation::NextKeyFrame() {
	if (currentKeyFrame < keyframes.size())
	{
		startingPosition = gameObject->getPosition();
		startingRotation = gameObject->getRotation();
		currentKeyFrame++;
		timeStart = glfwGetTime();
	}
	else
		Stop();
}

std::string Animation::GetName() {
	return name;
}

namespace AnimationManager {
	std::vector <Animation> animations;
	
	void AnimationManager::AddAnimation(Animation animation) {
		animations.push_back(animation);
	}
	
	void AnimationManager::Play(std::string Name,std::string ObjectName) {
		GetAnimation(Name)->SetGameObject(AssetManager::GetGameObject(ObjectName));
		GetAnimation(Name)->Start();
	}
	
	void AnimationManager::Stop(std::string Name) {
		GetAnimation(Name)->Stop();
	}
	
	void AnimationManager::Pause(std::string Name) {
		GetAnimation(Name)->Pause();
	}

	void AnimationManager::Update(float deltaTime) {
		for (int i = 0; i < animations.size(); i++) {
			if (animations[i].Playing()) {
				animations[i].Transform();
			}
		}
	}
	
	Animation* AnimationManager::GetAnimation(std::string Name) {
		for (int i = 0; i < animations.size(); i++) {
			if (animations[i].GetName() == Name)
				return &animations[i];
		}
	}
}

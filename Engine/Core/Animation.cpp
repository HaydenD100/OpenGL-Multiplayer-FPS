#include "Animation.h"

KeyFrame::KeyFrame(glm::vec3 Position, glm::quat Rotation, glm::vec3 Scale, float Duration) {
	position = Position;
	rotation = Rotation;
	scale = Scale;
	duration = Duration;
}

Animation::Animation(std::string Name) {
	name = Name;
}

Animation::Animation(const char* path, std::string Name) {
	name = Name;

	Assimp::Importer import;
	const aiScene * scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	std::cout << "Assimp: Loading Model " << path << std::endl;
	
	if (scene->HasAnimations()) {
		for (unsigned int i = 0; i < scene->mAnimations[0]->mNumChannels; i++)
		{
			aiNodeAnim* animationChannel = scene->mAnimations[0]->mChannels[i];
			for (unsigned int i = 0; i < animationChannel->mNumPositionKeys; i++)
			{
				
				float duration;
				if (i != 0)
					duration = animationChannel->mPositionKeys[i].mTime - animationChannel->mPositionKeys[i - 1].mTime;
				else
					duration = animationChannel->mPositionKeys[i].mTime;

				//This is just becuase the duration is how many frames the animation takes
				// right now i dont have a way to say 10 frames is whatever time so im just
				// hard coding 1 frame = 0.1 seconds ill change this later tho.

				duration = duration / 10;

				const aiVector3D& positionKey = animationChannel->mPositionKeys[i].mValue;

				//
				glm::vec3 position = glm::vec3(positionKey.x / 100, positionKey.y / 100, positionKey.z / 100);
				glm::quat glmQuat = glm::quat(animationChannel->mRotationKeys[i].mValue.w, animationChannel->mRotationKeys[i].mValue.x, animationChannel->mRotationKeys[i].mValue.y, animationChannel->mRotationKeys[i].mValue.z);
				glm::vec3 scale = glm::vec3(animationChannel->mScalingKeys[i].mValue.x, animationChannel->mScalingKeys[i].mValue.y, animationChannel->mScalingKeys[i].mValue.z);

				keyframes.push_back(KeyFrame(position, glmQuat, scale, duration));

				glm::vec3 testrot = glm::eulerAngles(glmQuat);
				//std::cout << "pos: " << positionKey.x << " y: " << positionKey.y << " z: " << positionKey.z << "\n";
				//std::cout << "rot: " << testrot.x << " y: " << testrot.y << " z: " << testrot.z << "\n";


			}
		}
	}
	else
		std::cout << "Error no animations found " << path << std::endl;
}

Animation::Animation(std::vector<KeyFrame> Keyframes, std::string Name) {
	keyframes = Keyframes;
	name = Name;
}

void Animation::AddKeyFrame(KeyFrame Keyframe) {
	keyframes.push_back(Keyframe);
}

size_t Animation::GetKeyFrameSize() {
	return keyframes.size();
}
std::vector<KeyFrame> Animation::GetKeyFrames() {
	return keyframes;
}

bool Animation::Playing() {
	return playing;
}

void Animation::Stop() {
	gameObject->setPosition(initalPosition + keyframes[keyframes.size() - 1].position);
	gameObject->setRotation(initalRotation + glm::eulerAngles(keyframes[keyframes.size() - 1].rotation));

	playing = false;
	currentKeyFrame = 0;
	gameObject = nullptr;
}


void Animation::Start() {
	playing = true;
	initalPosition = gameObject->getPosition();
	initalRotation = gameObject->getRotation();
	startingPosition = initalPosition;
	startingRotation = initalRotation;
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

void Animation::TransformObject() {
	float t = (glfwGetTime() - timeStart) / keyframes[currentKeyFrame].duration;
	if (t > 1) {
		NextKeyFrame();
		return;
	}
	//TODO: Add scaling, need to scale the bullet rigidbody so collider scales
	
	Transform animationTransform;
	animationTransform.position = initalPosition + glm::mix(startingPosition, keyframes[currentKeyFrame].position, t);
	animationTransform.rotation = initalRotation + glm::eulerAngles(glm::slerp(startingRotation, keyframes[currentKeyFrame].rotation, t));

	//std::cout << "pos: " << keyframes[currentKeyFrame].position.x << " y: " << keyframes[currentKeyFrame].position.y << " z: " << keyframes[currentKeyFrame].position.z << "\n";
	//std::cout << "rot: " << animationTransform.rotation.x << " y: " << animationTransform.rotation.y << " z: " << animationTransform.rotation.z << "\n";

	gameObject->SetTransform(animationTransform);
}

void Animation::NextKeyFrame() {
	if (currentKeyFrame < keyframes.size() - 1){
		startingPosition = keyframes[currentKeyFrame].position;
		startingRotation = keyframes[currentKeyFrame].rotation;
		currentKeyFrame++;
		timeStart = glfwGetTime();
	}
	else
		Stop();
}

std::string Animation::GetName() {
	return name;
}

namespace AnimationManager
{
	std::vector <Animation> animations;
	
	void AnimationManager::AddAnimation(Animation animation) {
		animations.push_back(animation);
	}
	void ClearAnimations() {
		animations.clear();
	}

	
	void AnimationManager::Play(std::string Name,std::string ObjectName) {
		GameObject* gameobject = AssetManager::GetGameObject(ObjectName);
		Animation* animation = GetAnimation(Name);
		if (gameobject != nullptr && animation != nullptr) {
			animation->SetGameObject(gameobject);
			animation->Start();
		}
		else
			std::cout << "Null Object pointer passed to animation or animation does not exsit \n";
		
	}

	bool IsAnimationPlaying(std::string name) {
		for (int i = 0; i < animations.size(); i++) {
			if (animations[i].GetName() == name)
				return animations[i].Playing();
		}
		return false;
	}
	
	void AnimationManager::Stop(std::string Name) {
		GetAnimation(Name)->Stop();
	}
	
	void AnimationManager::Pause(std::string Name) {
		GetAnimation(Name)->Pause();
	}

	void AnimationManager::Update(float deltaTime) {
		for (int i = 0; i < animations.size(); i++) {
			if (animations[i].Playing()) 
				animations[i].TransformObject();
		}
	}
	
	Animation* AnimationManager::GetAnimation(std::string Name) {
		for (int i = 0; i < animations.size(); i++) {
			if (animations[i].GetName() == Name)
				return &animations[i];
		}
		return nullptr;
	}
}

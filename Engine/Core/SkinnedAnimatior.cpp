#include "SkinnedAnimatior.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include "Engine/Core/AssetManager.h"


namespace Animator {
    std::vector<AnimationInstance> currentAnimationInstances;
    float m_DeltaTime;

    void Animator::Init() {

    }

    void Animator::Init(SkinnedAnimation* Animation, std::string GameObjectname) {
        AnimationInstance temp;
        temp.Animation = Animation;
        temp.m_CurrentTime = 0.0;
        temp.GameObjectName = GameObjectname;

    }

    void Animator::UpdateAnimation(float dt) {
        m_DeltaTime = dt;
        for (int i = 0; i < currentAnimationInstances.size(); i++) {
            if (currentAnimationInstances[i].Animation && currentAnimationInstances[i].isPlaying) {
                currentAnimationInstances[i].m_CurrentTime += currentAnimationInstances[i].Animation->GetTicksPerSecond() * dt;
                if (currentAnimationInstances[i].m_CurrentTime > currentAnimationInstances[i].Animation->GetDuration() && !currentAnimationInstances[i].loop) {
                    currentAnimationInstances[i].isPlaying = false;
                    currentAnimationInstances[i].m_CurrentTime = 0;
                    continue;
                }
                currentAnimationInstances[i].m_CurrentTime = fmod(currentAnimationInstances[i].m_CurrentTime, currentAnimationInstances[i].Animation->GetDuration());
                CalculateBoneTransform(&currentAnimationInstances[i].Animation->GetRootNode(), glm::mat4(1.0f), i);
            }
        }
    }

    void Animator::PlayAnimation(SkinnedAnimation* pAnimation, std::string GameObjectname, bool loop) {
        for (int i = 0; i < currentAnimationInstances.size(); i++) {
            if (currentAnimationInstances[i].Animation == pAnimation && currentAnimationInstances[i].GameObjectName == GameObjectname && currentAnimationInstances[i].loop == loop) {
                if (currentAnimationInstances[i].isPlaying == true)
                    currentAnimationInstances[i].m_CurrentTime = 0;
                else
                    currentAnimationInstances[i].isPlaying = true;
                return;
            }
        }
        AnimationInstance temp;
        temp.Animation = pAnimation;
        temp.m_CurrentTime = 0.0;
        temp.GameObjectName = GameObjectname;
        temp.loop = loop;
        temp.isPlaying = true;

        currentAnimationInstances.push_back(temp);
    }

    void Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform, int index) {
        std::string nodeName = node->name;
        glm::mat4 nodeTransform = node->transformation;

        Bone* Bone = currentAnimationInstances[index].Animation->FindBone(nodeName);

        if (Bone) {
            Bone->Update(currentAnimationInstances[index].m_CurrentTime);
            nodeTransform = Bone->GetLocalTransform();
        }

        glm::mat4 globalTransformation = parentTransform * nodeTransform;

        auto boneInfoMap = currentAnimationInstances[index].Animation->GetBoneIDMap();
        GameObject* gameobject = AssetManager::GetGameObject(currentAnimationInstances[index].GameObjectName);
        if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
            int indexBone = boneInfoMap[nodeName].id;
            gameobject->SetFinalBoneMatricies(indexBone, globalTransformation * boneInfoMap[nodeName].offset);
        }

        for (int i = 0; i < node->childrenCount; i++)
            CalculateBoneTransform(&node->children[i], globalTransformation, index);
    }

    //FianlBoneMats are now stored in the gameobject so they can be modified for ragdolls in the future
    /*
    std::vector<glm::mat4> Animator::GetFinalBoneMatrices(std::string gameObjectname) {
        for (int i = 0; i < currentAnimationInstances.size(); i++) {
            if (currentAnimationInstances[i].GameObjectName == gameObjectname && currentAnimationInstances[i].isPlaying) {
                return
            }
        }

        for (int i = 0; i < currentAnimationInstances.size(); i++) {
            if (currentAnimationInstances[i].GameObjectName == gameObjectname) {
                return currentAnimationInstances[i].m_FinalBoneMatrices;
            }
        }
        std::vector<glm::mat4> zero;
        return zero;
    }
    */
    AnimationInstance* Animator::GetAnimationInsatce(int i) {
        return &currentAnimationInstances[i];
    }

    size_t Animator::GetAnimationInstanceSize() {
        return currentAnimationInstances.size();
    }
}

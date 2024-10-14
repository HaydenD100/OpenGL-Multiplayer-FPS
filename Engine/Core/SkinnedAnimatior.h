#pragma once
#include "Engine/Core/SkinnedAnimation.h"



struct AnimationInstance {
    std::string GameObjectName;
    std::vector<glm::mat4> m_FinalBoneMatrices;
    SkinnedAnimation* Animation;
    float m_CurrentTime;
    
    bool isPlaying = true;
};

class Animator
{
public:
    Animator() = default;
    Animator(SkinnedAnimation* Animation, std::string GameObjectname)
    {
        AnimationInstance temp;
        temp.Animation = Animation;
        temp.m_CurrentTime = 0.0;
        temp.m_FinalBoneMatrices.reserve(100);
        temp.GameObjectName = GameObjectname;
        for (int i = 0; i < 100; i++)
            temp.m_FinalBoneMatrices.push_back(glm::mat4(1.0f));

        currentAnimationInstances.push_back(temp);


        
    }

    void UpdateAnimation(float dt)
    {
        m_DeltaTime = dt;
        for (int i = 0; i < currentAnimationInstances.size(); i++) {
            if (currentAnimationInstances[i].Animation && currentAnimationInstances[i].isPlaying)
            {
                currentAnimationInstances[i].m_CurrentTime += currentAnimationInstances[i].Animation->GetTicksPerSecond() * dt;
                currentAnimationInstances[i].m_CurrentTime = fmod(currentAnimationInstances[i].m_CurrentTime, currentAnimationInstances[i].Animation->GetDuration());
                CalculateBoneTransform(&currentAnimationInstances[i].Animation->GetRootNode(), glm::mat4(1.0f), i);
            }
        }
        
    }

    void PlayAnimation(SkinnedAnimation* pAnimation, std::string GameObjectname)
    {
        for (int i = 0; i < currentAnimationInstances.size(); i++) {
            if (currentAnimationInstances[i].Animation == pAnimation && currentAnimationInstances[i].GameObjectName == GameObjectname)
            {
                currentAnimationInstances[i].isPlaying = true;
                return;
            }
        }
        AnimationInstance temp;
        temp.Animation = pAnimation;
        temp.m_CurrentTime = 0.0;
        temp.m_FinalBoneMatrices.reserve(100);
        temp.GameObjectName = GameObjectname;

        for (int i = 0; i < 100; i++)
            temp.m_FinalBoneMatrices.push_back(glm::mat4(1.0f));

        currentAnimationInstances.push_back(temp);

    }

    void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform, int index)
    {
       
        std::string nodeName = node->name;
        glm::mat4 nodeTransform = node->transformation;

        Bone* Bone = currentAnimationInstances[index].Animation->FindBone(nodeName);

        
        if (Bone)
        {
            Bone->Update(currentAnimationInstances[index].m_CurrentTime);
            nodeTransform = Bone->GetLocalTransform();
        }
        
        glm::mat4 globalTransformation = parentTransform * nodeTransform;

        auto boneInfoMap = currentAnimationInstances[index].Animation->GetBoneIDMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end())
        {
            int indexBone = boneInfoMap[nodeName].id;
            glm::mat4 offset = boneInfoMap[nodeName].offset;
            currentAnimationInstances[index].m_FinalBoneMatrices[indexBone] = globalTransformation * offset;
        }

        for (int i = 0; i < node->childrenCount; i++)
            CalculateBoneTransform(&node->children[i], globalTransformation, index);
    }

    std::vector<glm::mat4> GetFinalBoneMatrices(std::string gameObjectname)
    {
        for (int i = 0; i < currentAnimationInstances.size(); i++) {
            if (currentAnimationInstances[i].GameObjectName == gameObjectname)
            {
                return currentAnimationInstances[i].m_FinalBoneMatrices;
            }
        }
        std::vector <glm::mat4> zero;
        return zero;
    }


    AnimationInstance* GetAnimationInsatce(int i) {
        return &currentAnimationInstances[i];
    }

    size_t GetAnimationInstanceSize() {
        return currentAnimationInstances.size();
    }

private:
    std::vector<AnimationInstance> currentAnimationInstances;
    float m_DeltaTime;
};
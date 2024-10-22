#pragma once
#include "Engine/Core/SkinnedAnimation.h"



struct AnimationInstance {
    std::string GameObjectName;
    std::vector<glm::mat4> m_FinalBoneMatrices;
    SkinnedAnimation* Animation;
    float m_CurrentTime;
    
    bool isPlaying = true;
    bool loop = false;
};

class Animator
{
public:
    Animator() {
        m_FinalBoneMatrices1.reserve(100);

        for (int i = 0; i < 100; i++) {
            glm::mat4 modelMatrix = glm::mat4(1);
            //modelMatrix = glm::rotate(modelMatrix, -glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));
            m_FinalBoneMatrices1.push_back(modelMatrix);
        }
    }
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
                if (currentAnimationInstances[i].m_CurrentTime > currentAnimationInstances[i].Animation->GetDuration() && !currentAnimationInstances[i].loop) {

                    currentAnimationInstances[i].isPlaying = false;
                    currentAnimationInstances[i].m_CurrentTime = 0;
                    CalculateBoneTransform(&currentAnimationInstances[i].Animation->GetRootNode(), glm::mat4(1.0f), i);
                    continue;
                }
                currentAnimationInstances[i].m_CurrentTime = fmod(currentAnimationInstances[i].m_CurrentTime, currentAnimationInstances[i].Animation->GetDuration());
                CalculateBoneTransform(&currentAnimationInstances[i].Animation->GetRootNode(), glm::mat4(1.0f), i);
            }
        }
        
    }

    void PlayAnimation(SkinnedAnimation* pAnimation, std::string GameObjectname, bool loop = true)
    {
        for (int i = 0; i < currentAnimationInstances.size(); i++) {
            //so you cant have mutliple isntances of the same animation and object
            if (currentAnimationInstances[i].Animation == pAnimation && currentAnimationInstances[i].GameObjectName == GameObjectname && currentAnimationInstances[i].loop == loop)
            {
                //reset the animation if its already playing
                if (currentAnimationInstances[i].isPlaying == true)
                    currentAnimationInstances[i].m_CurrentTime = 0;
                else
                    currentAnimationInstances[i].isPlaying = true;
                return;
            }
        }
        std::cout << "Created new Animation \n";
        AnimationInstance temp;
        temp.Animation = pAnimation;
        temp.m_CurrentTime = 0.0;
        temp.m_FinalBoneMatrices.reserve(100);
        temp.GameObjectName = GameObjectname;
        temp.loop = loop;
        temp.isPlaying = true;

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
            currentAnimationInstances[index].m_FinalBoneMatrices[indexBone] = globalTransformation * boneInfoMap[nodeName].offset;
            //TODO :: make this work so the root transformation dosent need to be an idenity matrix 
            //currentAnimationInstances[index].m_FinalBoneMatrices[indexBone] = currentAnimationInstances[index].Animation->GetInverseGlobal() * globalTransformation * boneInfoMap[nodeName].offset;
        }

        for (int i = 0; i < node->childrenCount; i++)
            CalculateBoneTransform(&node->children[i], globalTransformation, index);
    }

    std::vector<glm::mat4> GetFinalBoneMatrices(std::string gameObjectname)
    {
        for (int i = 0; i < currentAnimationInstances.size(); i++) {
            if (currentAnimationInstances[i].GameObjectName == gameObjectname && currentAnimationInstances[i].isPlaying)
            {
                return currentAnimationInstances[i].m_FinalBoneMatrices;
            }            
        }
        //a hack for now but i want to fix it, issue is all the animations were exported as blender foward y up z and opengl is forward -z and up y and reexporting them to the corrent forward and up messes up and aninatmion
        //this right now isnt slow but i have to fix it before i add more animations
        for (int i = 0; i < currentAnimationInstances.size(); i++) {
            if (currentAnimationInstances[i].GameObjectName == gameObjectname)
            {
                return currentAnimationInstances[i].m_FinalBoneMatrices;
            }
        }
        std::vector<glm::mat4> zero;
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
    std::vector<glm::mat4> m_FinalBoneMatrices1;

};
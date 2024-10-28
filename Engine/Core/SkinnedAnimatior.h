#pragma once
#include "Engine/Core/SkinnedAnimation.h"
#include <vector>
#include <string>
#include <glm/mat4x4.hpp>

struct AnimationInstance {
    std::string GameObjectName;
    SkinnedAnimation* Animation;
    float m_CurrentTime;

    bool isPlaying = true;
    bool loop = false;
};

class Animator
{
public:
    Animator();
    Animator(SkinnedAnimation* Animation, std::string GameObjectname);

    void UpdateAnimation(float dt);
    void PlayAnimation(SkinnedAnimation* pAnimation, std::string GameObjectname, bool loop = true);
    void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform, int index);

    // std::vector<glm::mat4> GetFinalBoneMatrices(std::string gameObjectname);
    AnimationInstance* GetAnimationInsatce(int i);
    size_t GetAnimationInstanceSize();

private:
    std::vector<AnimationInstance> currentAnimationInstances;
    float m_DeltaTime;
    std::vector<glm::mat4> m_FinalBoneMatrices1;
};
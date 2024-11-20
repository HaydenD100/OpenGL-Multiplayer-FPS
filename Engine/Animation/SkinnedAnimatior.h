#pragma once
#include "Engine/Animation/SkinnedAnimation.h"
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

namespace Animator
{

    void Init();
    void Init(SkinnedAnimation* Animation, std::string GameObjectname);

    void UpdateAnimation(float dt);
    void PlayAnimation(SkinnedAnimation* pAnimation, std::string GameObjectname, bool loop = true);
    void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform, int index);

    // std::vector<glm::mat4> GetFinalBoneMatrices(std::string gameObjectname);
    AnimationInstance* GetAnimationInsatce(int i);
    size_t GetAnimationInstanceSize();
};
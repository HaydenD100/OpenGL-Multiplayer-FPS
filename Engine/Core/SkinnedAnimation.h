#pragma once
#include "Engine/Core/Bone.h"
#include "Engine/Core/Model.h"

struct AssimpNodeData
{
    glm::mat4 transformation;
    std::string name;
    int childrenCount;
    std::vector<AssimpNodeData> children;
};

class SkinnedAnimation
{
public:
    SkinnedAnimation() = default;

    SkinnedAnimation(const std::string& animationPath, Model* model, int index = 0, std::string AnimationName = "none")
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
        if (!scene) {
            std::cout << "ERROR " << animationPath << "\n";
        }
        assert(scene && scene->mRootNode);
        std::cout << "Aniamtions: " << scene->mNumAnimations << " For" << animationPath << "\n";
        if (scene->mNumAnimations) {
            auto animation = scene->mAnimations[index];
            m_Duration = animation->mDuration;
            m_TicksPerSecond = animation->mTicksPerSecond;
            m_GlobalInversetransform = glm::inverse(ConvertMatrixToGLMFormat(scene->mRootNode->mTransformation));
            ReadHeirarchyData(m_RootNode, scene->mRootNode);
            ReadMissingBones(animation, *model);


            //std::vector<float> boneLengths;
            //calculateBoneLengths(scene->mRootNode, scene, boneLengths);

            // Output the bone lengths
            //for (float length : boneLengths) {
                //std::cout << "Bone length: " << length << std::endl;
            //}
        }
        animationName = AnimationName;
    }

    ~SkinnedAnimation()
    {
    }

    Bone* FindBone(const std::string& name)
    {
        auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
            [&](const Bone& Bone)
            {
                return Bone.GetBoneName() == name;
            }
        );
        if (iter == m_Bones.end()) return nullptr;
        else return &(*iter);
    }


    inline float GetTicksPerSecond() { return m_TicksPerSecond; }

    inline float GetDuration() { return m_Duration; }

    inline const AssimpNodeData& GetRootNode() { return m_RootNode; }

    inline const glm::mat4 GetInverseGlobal() { return m_GlobalInversetransform; }

    inline const std::map<std::string, BoneInfo>& GetBoneIDMap()
    {
        return m_BoneInfoMap;
    }

    std::string GetName() {
        return animationName;
    }

private:
    void ReadMissingBones(const aiAnimation* animation, Model& model)
    {
        int size = animation->mNumChannels;

        auto& boneInfoMap = model.GetBoneInfoMap();//getting m_BoneInfoMap from Model class
        int& boneCount = model.GetBoneCount(); //getting the m_BoneCounter from Model class

        //reading channels(bones engaged in an animation and their keyframes)
        for (int i = 0; i < size; i++)
        {
            auto channel = animation->mChannels[i];
            std::string boneName = channel->mNodeName.data;

            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                boneInfoMap[boneName].id = boneCount;
                boneCount++;
            }
  
            m_Bones.push_back(Bone(channel->mNodeName.data,boneInfoMap[channel->mNodeName.data].id, channel));


        }

        m_BoneInfoMap = boneInfoMap;
    }

    void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src)
    {
        assert(src);

        dest.name = src->mName.data;
        dest.transformation = ConvertMatrixToGLMFormat(src->mTransformation);
        dest.childrenCount = src->mNumChildren;

        for (int i = 0; i < src->mNumChildren; i++)
        {
            AssimpNodeData newData;
            ReadHeirarchyData(newData, src->mChildren[i]);
            dest.children.push_back(newData);
        }
    }

    //Not in use but i wnat to use it for ragdolls later on
    // Recursive function to calculate bone lengths
    void calculateBoneLengths(const aiNode* node, const aiScene* scene, std::vector<float>& boneLengths, const glm::mat4& parentTransform = glm::mat4(1.0f)) {
        glm::mat4 nodeTransform = ConvertMatrixToGLMFormat(node->mTransformation);
        glm::mat4 globalTransform = parentTransform * nodeTransform;

        // Get the position of this node in world space (ignore rotation and scale)
        glm::vec3 currentPos(globalTransform[3][0], globalTransform[3][1], globalTransform[3][2]);

        for (unsigned int i = 0; i < node->mNumChildren; i++) {
            const aiNode* childNode = node->mChildren[i];

            glm::mat4 childTransform = ConvertMatrixToGLMFormat(childNode->mTransformation);
            glm::mat4 childGlobalTransform = globalTransform * childTransform;

            // Calculate the child's position in world space
            glm::vec3 childPos(childGlobalTransform[3][0], childGlobalTransform[3][1], childGlobalTransform[3][2]);

            // Calculate length between this node and its child
            float length = glm::length(childPos - currentPos);
            boneLengths.push_back(length);

            // Recursive call for the child node
            calculateBoneLengths(childNode, scene, boneLengths, globalTransform);
        }
    }


    std::string animationName;
    float m_Duration;
    int m_TicksPerSecond;
    std::vector<Bone> m_Bones;
    AssimpNodeData m_RootNode;
    std::map<std::string, BoneInfo> m_BoneInfoMap;
    glm::mat4 m_GlobalInversetransform;
};
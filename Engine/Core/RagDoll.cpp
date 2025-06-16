#include "RagDoll.h"


RagDoll::RagDoll(std::string bonePath, std::string GameobjectName) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(bonePath, aiProcess_Triangulate);
    if (!scene) {
        std::cout << "ERROR " << bonePath << "\n";
    }
    assert(scene && scene->mRootNode);
    if (scene->mRootNode) {
        scene->mRootNode->mTransformation;
    }
}


void RagDoll::UpdateRagDoll(float dt) {
    //CalculateBoneTransform(&currentAnimationInstances[i].Animation->GetRootNode(), glm::mat4(1.0f), i);
}


/*
void RagDoll::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform, int index) {
    std::string nodeName = node->name;
    glm::mat4 nodeTransform = node->transformation;

    Bone* Bone = currentAnimationInstances[index].Animation->FindBone(nodeName);

    if (Bone) {
        Bone->Update(currentAnimationInstances[index].m_CurrentTime);
        nodeTransform = Bone->GetLocalTransform();
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;

    auto boneInfoMap = currentAnimationInstances[index].Animation->GetBoneIDMap();
    GameObject* gameobject = World::GetGameObject(currentAnimationInstances[index].GameObjectName);
    if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
        int indexBone = boneInfoMap[nodeName].id;
        gameobject->SetFinalBoneMatricies(indexBone, globalTransformation * boneInfoMap[nodeName].offset);
    }

    for (int i = 0; i < node->childrenCount; i++)
        CalculateBoneTransform(&node->children[i], globalTransformation, index);
}
*/
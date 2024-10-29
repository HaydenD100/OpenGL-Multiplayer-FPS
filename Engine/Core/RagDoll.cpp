#include "RagDoll.h"


RagDoll::RagDoll(std::string bonePath, std::string GameobjectName) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(bonePath, aiProcess_Triangulate);
    if (!scene) {
        std::cout << "ERROR " << bonePath << "\n";
    }
    assert(scene && scene->mRootNode);
    std::cout << "Aniamtions: " << scene->mNumAnimations << " For" << bonePath << "\n";
    if (scene->mRootNode) {
        scene->mRootNode->mTransformation;
        
    }
}

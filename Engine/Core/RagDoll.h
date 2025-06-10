#pragma once
#include "Engine/Core/Common/Header.h"
#include "Engine/Animation/Bone.h"
#include "Engine/Renderer/Model.h"

#include <iostream>
#include <map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct RigidComponent {
	int ID;
	float capsuleLength, capsuleRadius;
	glm::mat4 transform;
};
struct JointComponent {
	std::string name;
	int parentID, childID;

	bool joint_enabled;
};

class RagDoll
{
public:
	RagDoll() = default;
	RagDoll(std::string bonePath, std::string GameobjectName);
	void UpdateRagDoll(float dt);
	//void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform, int index);
	//Make another constructor that takes a custom file


private:
	std::string name;
	std::string gameObjectname;

	std::vector<RigidComponent> rigidComponents;
	std::vector<JointComponent> jointComponents;
	
	
};




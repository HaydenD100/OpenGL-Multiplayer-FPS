#pragma once
#include "Engine/Core/Common/Header.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>

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
	//Make another constructor that takes a custom file


private:
	std::string name;
	std::string gameObjectname;

	std::vector<RigidComponent> rigidComponents;
	std::vector<JointComponent> jointComponents;
	
	
};


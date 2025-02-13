#pragma once
#include "Engine/Core/Common/Defines.h"
#include <vector>
#include <string>
#include "Engine/Physics/BulletPhysics.h"
#include "Engine/Renderer/Model.h"


//#include "Loaders/nlohmann/json.hpp"


//Just a little reminder that scaling will scale the model but not the collider or the AABB from frustum culling yet

class GameObject
{
public:
	GameObject();
	GameObject(std::string name, bool save, float mass, ColliderShape shape);
	GameObject(std::string name, glm::vec3 position, bool save, float mass, ColliderShape shape);
	GameObject(std::string name, Model* model, glm::vec3 position, bool save, float mass, btConvexHullShape* shape);
	GameObject(std::string name, Model* model, glm::vec3 position, bool save, float mass, btCollisionShape* shape);

	GameObject(std::string name, Model* model, glm::vec3 position, bool save, float mass, ColliderShape shape);
	GameObject(std::string name, Model* model, glm::vec3 position, bool save, float mass, ColliderShape shape, float width, float height, float depth);

	glm::mat4 GetModelMatrix();
	glm::mat4 GetLocalModelMatrix();
	
	void RenderObject(GLuint programID);

	void setPosition(glm::vec3 position);
	void setRotation(glm::vec3 rotation);
	void setScale(glm::vec3 scale);

	void SetShaderType(std::string shaderType);
	std::string GetShaderType();

	void Update();

	glm::vec3 getPosition();
	glm::vec3 getRotation();
	glm::vec3 getScale();

	void addPosition(glm::vec3 position);

	void setPositionX(float x);
	void setPositionY(float y);
	void setPositionZ(float z);

	void SetRotationX(float rotation);
	void SetRotationY(float rotation);
	void SetRotationZ(float rotation);

	Transform getTransform();
	

	void SetTransform(Transform transform);
	 
	void SetScale(float scale);

	std::string GetName();
	std::string GetParentName();

	void SetParentName(std::string name);

	// For Json Saving
	std::vector<unsigned short> getIndices();
	std::vector<glm::vec3> getIndexedVerticies();
	std::vector<glm::vec2> getIndexedUvs();
	std::vector<glm::vec3> getIndexedNormals();
	const char* GetTextureName();

	bool CanSave();
	void SetRender(bool render);
	bool ShouldRender();

	void SetDelete(bool state);
	bool ShouldDlete();

	void SetUserPoint(void* pointer);

	Model* GetModel();
	btRigidBody* GetRigidBody();
	btCollisionShape* GetCollisionShape();
	btConvexHullShape* GetConvexHull();

	std::vector<glm::mat4> GetFinalBoneMatricies();
	void SetFinalBoneMatricies(int index, glm::mat4 mat);

	glm::vec3 GetPosition() const {
		return btToGlmVector3(body->getWorldTransform().getOrigin());
	}

	bool IsDynamic();
	bool DontCull();
	void SetDontCull(bool cull);
	void IncludInRayCast();
	bool IncludedInRayCast();
	void IncludInGI(bool state);

	bool IncludedInGI();

private:
	Transform transform = Transform();
	Model* model = nullptr;

	std::string parentName;
	std::string name;
	bool isDynamic = false;
	//Water or Glass
	std::string shaderType = "Default";
	
	// Physics stuff
	btCollisionShape* collider = nullptr;
	btConvexHullShape* convexHullShape = nullptr;
	btBvhTriangleMeshShape* triangleCollison = nullptr;
	btTransform Btransform;
	btRigidBody* body;
	
	//canSave is used for json
	bool canSave = false;
	bool render = true;
	bool dontCull = false;
	bool shouldDelete = false;
	bool includedInRayCast = false;
	bool includedInGI = false;

	//change to this
	std::vector<glm::mat4> m_FinalBoneMatrices;

};

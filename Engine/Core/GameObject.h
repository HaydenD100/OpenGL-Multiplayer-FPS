#pragma once
#include <vector>
#include <string>

#include "Engine/Core/Common/Defines.h"
#include "Engine/Physics/BulletPhysics.h"
#include "Engine/Renderer/Model.h"

enum ObjectType {
	DEFAULT,
	GLASS,
	PLAYER,
	WATER,
	DESTORYABLE
};
//#include "Loaders/nlohmann/json.hpp"


struct Destructible {
	std::string m_destoryed_object = "";
	std::string m_destoryed_sound = "";
	int m_destoryed = 0;

	btConvexHullShape* convexHullShape;

	void CreateConvex(Model* model) {
		m_destoryed_object = model->GetName();
		convexHullShape = new btConvexHullShape();
		//Models can have Conex hull meshes, these meshes are a simplifed version of the original mesh and can imported from a OBJ file
		if (model->GetColliderShapeVerticiesSize() > 0) {
			for (const auto& vertex : model->GetColliderShapeVerticies()) {
				convexHullShape->addPoint(glmToBtVector3(vertex));
			}
		}
		else {
			for (int i = 0; i < model->GetAllMeshes()->size(); i++) {
				for (const auto& vertex : model->GetMesh(i)->indexed_vertices) {
					convexHullShape->addPoint(glmToBtVector3(vertex));
				}
				//only create a collider for the entire model/everymesh if the entire mesh is being renderered
				if (!model->RenderAll())
					break;
			}
		}
		convexHullShape->optimizeConvexHull();
	}

};

//Just a little reminder that scaling will scale the model but not the collider or the AABB from frustum culling yet

class GameObject
{
public:
	GameObject();
	~GameObject();

	GameObject(const GameObject&) = delete;
	GameObject& operator=(const GameObject&) = delete;

	GameObject(GameObject&& other) noexcept;
	GameObject& operator=(GameObject&& other) noexcept;

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

	const std::string& GetName() const;
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
	void SetModel(Model* model);
	std::shared_ptr<btRigidBody> GetRigidBody();
	btCollisionShape* GetCollisionShape();
	btConvexHullShape* GetConvexHull();

	std::vector<glm::mat4> GetFinalBoneMatricies();
	void SetFinalBoneMatricies(int index, glm::mat4 mat);

	glm::vec3 GetPosition() const {
		return btToGlmVector3(m_rigidBody->getWorldTransform().getOrigin());
	}

	bool IsDynamic();
	bool DontCull();
	void SetDontCull(bool cull);
	void IncludInRayCast();
	bool IncludedInRayCast();
	void IncludInGI(bool state);


	bool IncludedInGI();

	ObjectType objectType = ObjectType::DEFAULT;


	Destructible destructable;

	float m_mass = 0;
	float m_buoyancy = 0.01;
private:
	Transform transform = Transform();
	Model* model = nullptr;

	std::string parentName = "";
	std::string name = "";
	bool isDynamic = false;
	//Water or Glass REDUNANT
	std::string shaderType = "Default";
	
	// Physics stuff
	btCollisionShape* collider = nullptr;
	btConvexHullShape* convexHullShape = nullptr;
	btBvhTriangleMeshShape* triangleCollison = nullptr;
	btDefaultMotionState* myMotionState = nullptr;
	btTransform Btransform;

	std::shared_ptr<btRigidBody> m_rigidBody;

	
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

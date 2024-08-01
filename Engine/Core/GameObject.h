#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <string>

#include "Common.h"
#include "Renderer.h"

#include "../Loaders/vboindexer.h"
#include "../Loaders/loader.hpp"
#include "Engine/Physics/BulletPhysics.h"



#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>



class GameObject
{
public:
	GameObject();
	GameObject(std::string name, bool save, float mass, ColliderShape shape);
	GameObject(std::string name, glm::vec3 position, bool save, float mass, ColliderShape shape);
	GameObject(std::string name, const char* path, bool save, float mass, ColliderShape shape);
	GameObject(std::string name, const char* path, glm::vec3 position, bool save, float mass, ColliderShape shape);
	GameObject(std::string name, const char* path,Texture* texture, glm::vec3 position, bool save, float mass, ColliderShape shape);
	GameObject(std::string name, const char* path, Texture* texture, glm::vec3 position, bool save, float mass, ColliderShape shape,float margin);

	GameObject(std::string name, const char* path, Texture* texture, glm::vec3 position, bool save, float mass, ColliderShape shape,float width, float height, float depth);

	GameObject(std::string name, std::string parentname, Texture* texture, glm::vec3 position,glm::vec3 rotation, glm::vec3 scale, std::vector<unsigned short> indices,
		std::vector<glm::vec3> indexed_vertices,std::vector<glm::vec2> indexed_uvs,std::vector<glm::vec3> indexed_normals, bool save, float mass, ColliderShape shape);
	GameObject(std::string name, std::string parentname, Texture* texure, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale,
		std::vector<glm::vec3> vertices,
		std::vector<glm::vec2> uvs,
		std::vector<glm::vec3> normals,
		std::vector<glm::vec3> tangents,
		std::vector<glm::vec3> bitangents,
		std::vector<unsigned short> indices,
		std::vector<glm::vec3> indexed_vertices,
		std::vector<glm::vec2> indexed_uvs,
		std::vector<glm::vec3> indexed_normals,
		std::vector<glm::vec3> indexed_tangents,
		std::vector<glm::vec3> indexed_bitangents, 
		bool canSave,
		bool render,
		bool shouldDelete, float mass, 
		btConvexHullShape* collidershape);

	void LoadModel(const char* path);
	void Copy(std::string copyName);
	glm::mat4 GetModelMatrix();
	glm::mat4 GetLocalModelMatrix();


	void RenderObject(GLuint& programID);

	void setPosition(glm::vec3 position);
	void setRotation(glm::vec3 rotation);
	void setScale(glm::vec3 scale);

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

	void SetScale(float scale);

	std::string GetName();
	std::string GetParentName();

	void SetParentName(std::string name);

	//forJsonSaving
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

	void computeTangentBasis(
		// inputs
		std::vector<glm::vec3>& vertices,
		std::vector<glm::vec2>& uvs,
		std::vector<glm::vec3>& normals,
		// outputs
		std::vector<glm::vec3>& tangents,
		std::vector<glm::vec3>& bitangents
	);

	btRigidBody* GetRigidBody();
	btCollisionShape* GetCollisionShape();
	btConvexHullShape* GetConvexHull();



private:
	Transform transform = Transform();
	Texture* texture = NULL;

	std::string parentName;
	std::string name;
	

	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> bitangents;

	std::vector<unsigned short> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	std::vector<glm::vec3> indexed_tangents;
	std::vector<glm::vec3> indexed_bitangents;

	GLuint vertexbuffer;
	GLuint uvbuffer;
	GLuint normalbuffer;
	GLuint elementbuffer;
	GLuint VertexArrayID;
	GLuint tangentbuffer;
	GLuint bitangentbuffer;
	
	// Physics stuff
	btCollisionShape* collider = nullptr;
	btConvexHullShape* convexHullShape = nullptr;
	btTransform Btransform;
	btRigidBody* body;



	bool canSave = false;
	bool render = true;
	bool shouldDelete = false;

};

#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Engine/Core/GameObject.h"

#include <vector>
#include <string>

class Ray {
public:
	glm::vec3 direction;
	glm::vec3 origin;
	Ray();
	Ray(glm::vec3 dir, glm::vec3 org);
	void UpdateRay(glm::vec3 dir, glm::vec3 org);
	bool intersectsTriangle(std::vector<glm::vec3> verticies, glm::mat4 ModelMatrix);
};

class Cube
{
public:
	Cube(glm::vec3 position, float width, float height, float depth, std::string name);
	Cube(glm::vec3 position, glm::vec3 min, glm::vec3 max, std::string name);

	std::string GetName();
	std::string GetTag();
	void SetTag(std::string Tag);

	void setPosition(glm::vec3 position);
	glm::vec3 getPosition();
	float getDepth();
	float getHeight();
	float getWidth();
	void setDimensions(float width, float height, float depth);

	glm::vec3 getMin();
	glm::vec3 getMax();

	bool GetIsTrigger();
	void SetIsTrigger(bool trigger);
	void SetStatic(bool Static);
	bool GetStatic();

	void SetDelete(bool Delete);
	bool ShouldDelete();
	
	bool TouchingLeft(Cube* collider, float velocity);
	bool TouchingRight(Cube* collider, float velocity);
	bool TouchingFront(Cube* collider, float velocity);
	bool TouchingBack(Cube* collider, float velocity);
	bool TouchingBottom(Cube* collider, float velocity);
	bool TouchingTop(Cube* collider, float velocity);

	float intersect(Ray r, float t0, float t1);

	Cube(GameObject* gameobject, std::string names);
	void Regenerate(GameObject* gameobject);

private:
	std::string name = "None";
	std::string tag = "None";
	glm::vec3 min;
	glm::vec3 max;
	glm::vec3 position;
	float width, depth, height;
	bool isTrigger = false;
	bool Static = true;
	bool shouldDelete = false;
};

class RigidBody {
public:
	RigidBody();
	RigidBody(glm::vec3 position,std::string name);
	glm::vec3 GetPosition();
	void SetPosition(glm::vec3 position);
	void SetCollider(std::string colliderName);
	std::string GetCollider();

	std::string GetName();

	void NewPosition(float deltaTime);
	void NewPositionY(float deltaTime);

	void AddForce(glm::vec3 force);
	void AddForceX(float force);
	void AddForceY(float force);
	void AddForceZ(float force);

	void SetForce(glm::vec3 force);
	void SetForceX(float force);
	void SetForceY(float force);
	void SetForceZ(float force);

	void RemoveForceX();
	void RemoveForceY();
	void RemoveForceZ();

	glm::vec3 GetForce();

private:
	std::string name = "None";
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec3 velocity = glm::vec3(0,0,0);
	std::string collider = "None";
};

namespace PhysicsManager {
	RigidBody* AddRigidbody(glm::vec3 position, std::string name);
	Cube* AddCube(glm::vec3 position, glm::vec3 min, glm::vec3 max, std::string name);
	Cube* AddCube(glm::vec3 position, float width, float height, float depth, std::string name);
	Cube* AddCube(GameObject* gameobject, std::string name);

	void RemoveCube(std::string name);
	void RemoveRigidbody(std::string name);

	Cube* GetCollider(std::string name);
	RigidBody* GetRigidbody(std::string name);

	void Update(float deltaTime);
};

struct RayInfo {
	std::string name = "Nothing";
	float distance = 9999;
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec3 normal = glm::vec3(0, 0, 0);
	Cube* collider = nullptr;
};

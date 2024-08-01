#pragma once
#include "GameObject.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Physics/BulletPhysics.h"



GameObject::GameObject() {

}
GameObject::GameObject(std::string name, bool save, float mass, ColliderShape shape) {
	this->name = name;
	parentName = "";
}
GameObject::GameObject(std::string name, glm::vec3 position, bool save, float mass, ColliderShape shape) {
	this->name = name;
	setPosition(position);
	parentName = "";
	canSave = save;


}
GameObject::GameObject(std::string name, const char* path, bool save, float mass, ColliderShape shape) {
	this->name = name;
	LoadModel(path);
	parentName = "";
	canSave = save;


}
GameObject::GameObject(std::string name, const char* path, glm::vec3 position, bool save, float mass, ColliderShape shape) {
	this->name = name;
	LoadModel(path);
	setPosition(position);
	parentName = "";
	canSave = save;


}
GameObject::GameObject(std::string name, const char* path, Texture* texture, glm::vec3 position, bool save, float mass, ColliderShape shape) {
	this->name = name;
	this->texture = texture;
	LoadModel(path);
	parentName = "";
	canSave = save;

	float width = 1;
	float height = 1;
	float depth = 1;
	Btransform.setOrigin(glmToBtVector3(position));
	glm::vec3 dimensions(1, 1, 1);

	if (shape == Box) {
		glm::vec3 minPoint(std::numeric_limits<float>::max());
		glm::vec3 maxPoint(std::numeric_limits<float>::lowest());

		for (const auto& vertex : indexed_vertices) {
			if (vertex.x < minPoint.x) minPoint.x = vertex.x;
			if (vertex.y < minPoint.y) minPoint.y = vertex.y;
			if (vertex.z < minPoint.z) minPoint.z = vertex.z;

			if (vertex.x > maxPoint.x) maxPoint.x = vertex.x;
			if (vertex.y > maxPoint.y) maxPoint.y = vertex.y;
			if (vertex.z > maxPoint.z) maxPoint.z = vertex.z;
		}
		dimensions = maxPoint - minPoint;
		collider = new btBoxShape(btVector3(btScalar(dimensions.x / 2), btScalar(dimensions.y / 2), btScalar(dimensions.z / 2)));

	}
	else if (shape == Convex) {
		convexHullShape = new btConvexHullShape();
		for (const auto& vertex : indexed_vertices) {
			convexHullShape->addPoint(glmToBtVector3(vertex));
		}
		convexHullShape->optimizeConvexHull();
	}
	else if (shape == Sphere)
	{
		glm::vec3 minPoint(std::numeric_limits<float>::max());
		glm::vec3 maxPoint(std::numeric_limits<float>::lowest());

		for (const auto& vertex : indexed_vertices) {
			if (vertex.x < minPoint.x) minPoint.x = vertex.x;
			if (vertex.y < minPoint.y) minPoint.y = vertex.y;
			if (vertex.z < minPoint.z) minPoint.z = vertex.z;

			if (vertex.x > maxPoint.x) maxPoint.x = vertex.x;
			if (vertex.y > maxPoint.y) maxPoint.y = vertex.y;
			if (vertex.z > maxPoint.z) maxPoint.z = vertex.z;
		}
		dimensions = maxPoint - minPoint;
		collider = new btSphereShape(btScalar(dimensions.x / 2));
	}
	else if (shape == Capsule) {
		glm::vec3 minPoint(std::numeric_limits<float>::max());
		glm::vec3 maxPoint(std::numeric_limits<float>::lowest());

		for (const auto& vertex : indexed_vertices) {
			if (vertex.x < minPoint.x) minPoint.x = vertex.x;
			if (vertex.y < minPoint.y) minPoint.y = vertex.y;
			if (vertex.z < minPoint.z) minPoint.z = vertex.z;

			if (vertex.x > maxPoint.x) maxPoint.x = vertex.x;
			if (vertex.y > maxPoint.y) maxPoint.y = vertex.y;
			if (vertex.z > maxPoint.z) maxPoint.z = vertex.z;
		}
		dimensions = maxPoint - minPoint;
		collider = new btCapsuleShape(btScalar(dimensions.x / 2), (btScalar(dimensions.y / 2)));
	}

	PhysicsManagerBullet::AddColliderShape(collider);
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic && collider != nullptr)
		collider->calculateLocalInertia(btScalar(mass), localInertia);
	else if (isDynamic && convexHullShape != nullptr)
		convexHullShape->calculateLocalInertia(btScalar(mass), localInertia);

	Btransform.setIdentity();
	Btransform.setOrigin(btVector3(position.x, position.y, position.z));

	//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(Btransform);
	if (convexHullShape == nullptr)
	{
		btRigidBody::btRigidBodyConstructionInfo rbInfo(btScalar(mass), myMotionState, collider, localInertia);
		body = new btRigidBody(rbInfo);
	}
	else {
		btRigidBody::btRigidBodyConstructionInfo rbInfo(btScalar(mass), myMotionState, convexHullShape, localInertia);
		body = new btRigidBody(rbInfo);
	}

	body->setActivationState(DISABLE_DEACTIVATION);
	body->setFriction(0.7f);
	body->setUserIndex(-1);
	//add the body to the dynamics world
	if(mass != 0)
		PhysicsManagerBullet::GetDynamicWorld()->addRigidBody(body, GROUP_DYNAMIC, GROUP_PLAYER | GROUP_STATIC | GROUP_DYNAMIC);
	else
		PhysicsManagerBullet::GetDynamicWorld()->addRigidBody(body, GROUP_STATIC, GROUP_PLAYER | GROUP_STATIC | GROUP_DYNAMIC);
	setPosition(position);
}
GameObject::GameObject(std::string name, const char* path, Texture* texture, glm::vec3 position, bool save, float mass, ColliderShape shape, float margin) {
	this->name = name;
	this->texture = texture;
	LoadModel(path);
	parentName = "";
	canSave = save;

	float width = 1;
	float height = 1;
	float depth = 1;
	Btransform.setOrigin(glmToBtVector3(position));
	glm::vec3 dimensions(1, 1, 1);

	if (shape == Box) {
		glm::vec3 minPoint(std::numeric_limits<float>::max());
		glm::vec3 maxPoint(std::numeric_limits<float>::lowest());

		for (const auto& vertex : indexed_vertices) {
			if (vertex.x < minPoint.x) minPoint.x = vertex.x;
			if (vertex.y < minPoint.y) minPoint.y = vertex.y;
			if (vertex.z < minPoint.z) minPoint.z = vertex.z;

			if (vertex.x > maxPoint.x) maxPoint.x = vertex.x;
			if (vertex.y > maxPoint.y) maxPoint.y = vertex.y;
			if (vertex.z > maxPoint.z) maxPoint.z = vertex.z;
		}
		dimensions = maxPoint - minPoint;
		collider = new btBoxShape(btVector3(btScalar(dimensions.x / 2), btScalar(dimensions.y / 2), btScalar(dimensions.z / 2)));
	}
	else if (shape == Convex) {
		convexHullShape = new btConvexHullShape();
		for (const auto& vertex : indexed_vertices) {
			convexHullShape->addPoint(glmToBtVector3(vertex));
		}
		convexHullShape->optimizeConvexHull();
	}
	else if (shape == Sphere)
	{
		glm::vec3 minPoint(std::numeric_limits<float>::max());
		glm::vec3 maxPoint(std::numeric_limits<float>::lowest());

		for (const auto& vertex : indexed_vertices) {
			if (vertex.x < minPoint.x) minPoint.x = vertex.x;
			if (vertex.y < minPoint.y) minPoint.y = vertex.y;
			if (vertex.z < minPoint.z) minPoint.z = vertex.z;

			if (vertex.x > maxPoint.x) maxPoint.x = vertex.x;
			if (vertex.y > maxPoint.y) maxPoint.y = vertex.y;
			if (vertex.z > maxPoint.z) maxPoint.z = vertex.z;
		}
		dimensions = maxPoint - minPoint;
		collider = new btSphereShape(btScalar(dimensions.x / 2));

	}
	else if (shape == Capsule) {
		glm::vec3 minPoint(std::numeric_limits<float>::max());
		glm::vec3 maxPoint(std::numeric_limits<float>::lowest());

		for (const auto& vertex : indexed_vertices) {
			if (vertex.x < minPoint.x) minPoint.x = vertex.x;
			if (vertex.y < minPoint.y) minPoint.y = vertex.y;
			if (vertex.z < minPoint.z) minPoint.z = vertex.z;

			if (vertex.x > maxPoint.x) maxPoint.x = vertex.x;
			if (vertex.y > maxPoint.y) maxPoint.y = vertex.y;
			if (vertex.z > maxPoint.z) maxPoint.z = vertex.z;
		}
		dimensions = maxPoint - minPoint;
		collider = new btCapsuleShape(btScalar(dimensions.x / 2), (btScalar(dimensions.y / 2)));
	}
	if(collider != nullptr)
		collider->setMargin(margin);
	if (convexHullShape != nullptr)
		convexHullShape->setMargin(margin);

	PhysicsManagerBullet::AddColliderShape(collider);
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic && collider != nullptr)
		collider->calculateLocalInertia(btScalar(mass), localInertia);
	else if (isDynamic && convexHullShape != nullptr)
		convexHullShape->calculateLocalInertia(btScalar(mass), localInertia);

	Btransform.setIdentity();
	Btransform.setOrigin(btVector3(position.x, position.y, position.z));

	// Using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(Btransform);
	if (convexHullShape == nullptr)
	{
		btRigidBody::btRigidBodyConstructionInfo rbInfo(btScalar(mass), myMotionState, collider, localInertia);
		body = new btRigidBody(rbInfo);
	}
	else {
		btRigidBody::btRigidBodyConstructionInfo rbInfo(btScalar(mass), myMotionState, convexHullShape, localInertia);
		body = new btRigidBody(rbInfo);
	}

	body->setActivationState(DISABLE_DEACTIVATION);
	body->setFriction(0.9f);
	body->setUserIndex(-1);
	//add the body to the dynamics world
	if (mass != 0)
		PhysicsManagerBullet::GetDynamicWorld()->addRigidBody(body, GROUP_DYNAMIC, GROUP_PLAYER | GROUP_STATIC | GROUP_DYNAMIC);
	else
		PhysicsManagerBullet::GetDynamicWorld()->addRigidBody(body, GROUP_STATIC, GROUP_PLAYER | GROUP_STATIC | GROUP_DYNAMIC);
	setPosition(position);
}

GameObject::GameObject(std::string name, const char* path, Texture* texture, glm::vec3 position, bool save, float mass, ColliderShape shape, float width, float height, float depth) {
	this->name = name;
	this->texture = texture;
	LoadModel(path);
	parentName = "";
	canSave = save;

	Btransform.setOrigin(glmToBtVector3(position));


	if(shape == Sphere)
		collider = new btSphereShape(btScalar(height / 2));
	else
		collider = new btBoxShape(btVector3(btScalar(width / 2), btScalar(height / 2), btScalar(depth / 2)));

	PhysicsManagerBullet::AddColliderShape(collider);
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		collider->calculateLocalInertia(btScalar(mass), localInertia);

	Btransform.setIdentity();
	Btransform.setOrigin(btVector3(position.x, position.y, position.z));

	// Using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(Btransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(btScalar(mass), myMotionState, collider, localInertia);
	body = new btRigidBody(rbInfo);
	body->setActivationState(DISABLE_DEACTIVATION);
	body->setFriction(0.7f);
	body->setUserIndex(-1);
	
	// Add the body to the dynamics world
	if (mass != 0)
		PhysicsManagerBullet::GetDynamicWorld()->addRigidBody(body, GROUP_DYNAMIC, GROUP_PLAYER | GROUP_STATIC | GROUP_DYNAMIC);
	else
		PhysicsManagerBullet::GetDynamicWorld()->addRigidBody(body, GROUP_STATIC, GROUP_PLAYER | GROUP_STATIC | GROUP_DYNAMIC);
	setPosition(position);
}


GameObject::GameObject(std::string name, std::string parentname, Texture* texture, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, std::vector<unsigned short> indice,
	std::vector<glm::vec3> indexed_vert, std::vector<glm::vec2> indexed_uv, std::vector<glm::vec3> indexed_norms, bool save, float mass, ColliderShape shape)
{
	canSave = save;
	this->name = name;
	setPosition(position);
	this->texture = texture;
	parentName = parentname;
	setRotation(rotation);
	transform.scale.x = scale.x;
	transform.scale.y = scale.y;
	transform.scale.z = scale.z;
	indices = indice;
	indexed_normals = indexed_norms;
	indexed_uvs = indexed_uv;
	this->indexed_vertices = indexed_vert;

	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);


	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);


	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices as well

	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);
}

GameObject::GameObject(std::string name, std::string parentname, Texture* texture, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale,
	std::vector<glm::vec3> vertices, std::vector<glm::vec2> uvs, std::vector<glm::vec3> normals, std::vector<glm::vec3> tangents,
	std::vector<glm::vec3> bitangents, std::vector<unsigned short> indices, std::vector<glm::vec3> indexed_vertices,
	std::vector<glm::vec2> indexed_uvs, std::vector<glm::vec3> indexed_normals, std::vector<glm::vec3> indexed_tangents,
	std::vector<glm::vec3> indexed_bitangents, bool canSave, bool render, bool shouldDelete, float mass, btConvexHullShape* collidershape)
{
	std::cout << "test1";
	this->name = name;
	this->parentName = parentname;
	this->texture = texture;
	this->transform.position = position;
	this->transform.rotation = rotation;
	this->transform.scale = scale;
	this->vertices = vertices;
	this->uvs = uvs;
	this->normals = normals;
	this->tangents = tangents;
	this->bitangents = bitangents;
	this->indices = indices;
	this->indexed_vertices = indexed_vertices;
	this->indexed_uvs = indexed_uvs;
	this->indexed_normals = indexed_normals;
	this->indexed_tangents = indexed_tangents;
	this->indexed_bitangents = indexed_bitangents;
	this->canSave = canSave;
	this->render = render;
	this->shouldDelete = shouldDelete;
	convexHullShape = collidershape;
	std::cout << "here2" << std::endl;


	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);


	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);


	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &tangentbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_tangents.size() * sizeof(glm::vec3), &indexed_tangents[0], GL_STATIC_DRAW);

	glGenBuffers(1, &bitangentbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_bitangents.size() * sizeof(glm::vec3), &indexed_bitangents[0], GL_STATIC_DRAW);

	float width = 1;
	float height = 1;
	float depth = 1;
	Btransform.setOrigin(glmToBtVector3(position));
	std::cout << "here3" << std::endl;



	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic && collider != nullptr)
		collider->calculateLocalInertia(btScalar(mass), localInertia);
	else if (isDynamic && convexHullShape != nullptr)
		convexHullShape->calculateLocalInertia(btScalar(mass), localInertia);

	Btransform.setIdentity();
	Btransform.setOrigin(btVector3(position.x, position.y, position.z));

	// Using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(Btransform);
	if (convexHullShape == nullptr)
	{
		btRigidBody::btRigidBodyConstructionInfo rbInfo(btScalar(mass), myMotionState, collider, localInertia);
		body = new btRigidBody(rbInfo);
	}
	else {
		btRigidBody::btRigidBodyConstructionInfo rbInfo(btScalar(mass), myMotionState, convexHullShape, localInertia);
		body = new btRigidBody(rbInfo);
	}

	body->setActivationState(DISABLE_DEACTIVATION);
	body->setFriction(0.7f);
	body->setUserIndex(-1);
	
	// Add the body to the dynamics world
	if (mass != 0)
		PhysicsManagerBullet::GetDynamicWorld()->addRigidBody(body, GROUP_DYNAMIC, GROUP_PLAYER | GROUP_STATIC | GROUP_DYNAMIC);
	else
		PhysicsManagerBullet::GetDynamicWorld()->addRigidBody(body, GROUP_STATIC, GROUP_PLAYER | GROUP_STATIC | GROUP_DYNAMIC);
	setPosition(position);
	std::cout << "here4" << std::endl;

}

void GameObject::LoadModel(const char* path) {
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	bool res = loader::loadOBJ(path, vertices, uvs, normals);
	computeTangentBasis(vertices, uvs, normals, tangents, bitangents);
	indexer::indexVBO(vertices, uvs, normals, tangents, bitangents, indices, indexed_vertices, indexed_uvs, indexed_normals,indexed_tangents,indexed_bitangents);


	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);


	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);


	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &tangentbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_tangents.size() * sizeof(glm::vec3), &indexed_tangents[0], GL_STATIC_DRAW);

	glGenBuffers(1, &bitangentbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_bitangents.size() * sizeof(glm::vec3), &indexed_bitangents[0], GL_STATIC_DRAW);
}


void GameObject::Copy(std::string copyName) {
	std::cout << "damn";
	AssetManager::AddGameObject(GameObject(copyName, parentName, texture, getPosition(), getRotation(), getScale(), vertices, uvs, normals, tangents, bitangents, indices, indexed_vertices, indexed_uvs, indexed_normals, indexed_tangents, indexed_bitangents, canSave, render, shouldDelete,1,convexHullShape));

}
void GameObject::SetUserPoint(void* pointer) {
	body->setUserPointer(pointer);
}

// Parent child transformations
glm::mat4 GameObject::GetModelMatrix() {

	glm::mat4 matrix = transform.to_mat4();
	//Btransform.getOpenGLMatrix(glm::value_ptr(matrix));

	if (parentName != "") {
		GameObject* parent = AssetManager::GetGameObject(parentName);
		if (parent != nullptr) {
			matrix = parent->GetModelMatrix() * transform.to_mat4();
		}
	}
	return  matrix;
}

// Gets the models world space without translations from the parent to the world and not the parent
glm::mat4 GameObject::GetLocalModelMatrix() {
	return transform.to_mat4();
}

btRigidBody* GameObject::GetRigidBody() {
	return body;
}
btCollisionShape* GameObject::GetCollisionShape() {
	return collider;
}
void GameObject::Update() {
	transform.position = glm::vec3(body->getWorldTransform().getOrigin().x(), body->getWorldTransform().getOrigin().y(), body->getWorldTransform().getOrigin().z());
	transform.rotation = glm::eulerAngles(glm::quat(body->getWorldTransform().getRotation().w(), body->getWorldTransform().getRotation().x(), body->getWorldTransform().getRotation().y(), body->getWorldTransform().getRotation().z()));
}

void GameObject::RenderObject(GLuint& programID) {
	
	if (!render)
		return;
	glUseProgram(programID);

	if (texture != nullptr)
	{
		glActiveTexture(texture->GetTextureNumber() + GL_TEXTURE0);
		GLuint TextureID = glGetUniformLocation(programID, "DiffuseTextureSampler");
		glBindTexture(GL_TEXTURE_2D, texture->GetTexture());
		glUniform1i(TextureID, texture->GetTextureNumber());

		glActiveTexture(texture->GetTextureNormalNumber() + GL_TEXTURE0);
		GLuint NormalID = glGetUniformLocation(programID, "NormalTextureSampler");
		glBindTexture(GL_TEXTURE_2D, texture->GetTextureNormal());
		glUniform1i(NormalID, texture->GetTextureNormalNumber());
	}
	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);
	// 4th attribute buffer : tangents
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, tangentbuffer);
	glVertexAttribPointer(
		3,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// 5th attribute buffer : bitangents
	glEnableVertexAttribArray(4);
	glBindBuffer(GL_ARRAY_BUFFER, bitangentbuffer);
	glVertexAttribPointer(
		4,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// Index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

	// Draw the triangles !

	glDrawElements(
		GL_TRIANGLES,      // mode
		indices.size(),    // count
		GL_UNSIGNED_SHORT,   // type
		(void*)0           // element array buffer offset
	);


	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(4);
}


void GameObject::setPosition(glm::vec3 position) {
	transform.position = position;
	btTransform& t = body->getWorldTransform();
	t.setOrigin(btVector3(position.x,position.y,position.z));
	body->getMotionState()->setWorldTransform(t);
}
void GameObject::setRotation(glm::vec3 rotation) {
	transform.rotation = rotation;
	btTransform& t = body->getWorldTransform();
	btQuaternion quat;
	quat.setEuler(rotation.y, rotation.x, rotation.z);
	t.setRotation(quat);
	body->getMotionState()->setWorldTransform(t);
}
void GameObject::setScale(glm::vec3 scale) {
	transform.scale = scale;
}

glm::vec3 GameObject::getPosition() {
	return btToGlmVector3(body->getWorldTransform().getOrigin());
}
glm::vec3 GameObject::getRotation() {
	return btQuatToGLMVec(body->getWorldTransform().getRotation());
}
glm::vec3 GameObject::getScale() {
	return transform.scale;
}
void GameObject::addPosition(glm::vec3 position) {
	transform.position += position;
	body->getWorldTransform().setOrigin(btVector3(transform.position.x, transform.position.y, transform.position.z));
	//body->getMotionState()->setWorldTransform(t);
}


void GameObject::setPositionX(float x) {
	transform.position.x = x;
	body->getWorldTransform().setOrigin(btVector3(transform.position.x, transform.position.y, transform.position.z));
	//body->getMotionState()->setWorldTransform(t);
}
void GameObject::setPositionY(float y) {
	transform.position.y = y;
	body->getWorldTransform().setOrigin(btVector3(transform.position.x, transform.position.y, transform.position.z));
	//body->getMotionState()->setWorldTransform(t);
}
void GameObject::setPositionZ(float z) {
	transform.position.z = z;
	btTransform& t = body->getWorldTransform();
	t.setOrigin(btVector3(transform.position.x, transform.position.y, transform.position.z));
	body->getMotionState()->setWorldTransform(t);
}

void GameObject::SetRotationX(float x) {
	transform.rotation.x = x;
	btTransform& t = body->getWorldTransform();
	btQuaternion quat;
	quat.setEuler(transform.rotation.y, transform.rotation.x, transform.rotation.z);
	t.setRotation(quat);
	body->getMotionState()->setWorldTransform(t);
}
void GameObject::SetRotationY(float y) {
	transform.rotation.y = y;
	btTransform& t = body->getWorldTransform();
	btQuaternion quat;
	quat.setEuler(transform.rotation.y, transform.rotation.x, transform.rotation.z);
	t.setRotation(quat);
	body->getMotionState()->setWorldTransform(t);
}
void GameObject::SetRotationZ(float z) {
	transform.rotation.z = z;
	btTransform& t = body->getWorldTransform();
	btQuaternion quat;
	quat.setEuler(transform.rotation.y, transform.rotation.x, transform.rotation.z);
	t.setRotation(quat);
	body->getMotionState()->setWorldTransform(t);
}

void GameObject::SetScale(float scale) {
	transform.scale = glm::vec3(scale);
}

std::string GameObject::GetName() {
	return name;
}
std::string GameObject::GetParentName() {
	return parentName;
}
void GameObject::SetParentName(std::string name) {
	parentName = name;
}


std::vector<unsigned short> GameObject::getIndices() {
	return indices;
}
std::vector<glm::vec3>  GameObject::getIndexedVerticies() {
	return indexed_vertices;
}
std::vector<glm::vec2>  GameObject::getIndexedUvs() {
	return indexed_uvs;
}
std::vector<glm::vec3>  GameObject::getIndexedNormals() {
	return indexed_normals;
}
btConvexHullShape* GameObject::GetConvexHull() {
	return convexHullShape;
}
const char* GameObject::GetTextureName() {
	return texture->GetName();
}
bool GameObject::CanSave() {
	return canSave;
}
void GameObject::SetRender(bool render) {
	this->render = render;
}
bool GameObject::ShouldRender() {
	return render;
}

void GameObject::SetDelete(bool state) {
	shouldDelete = state;
}
bool GameObject::ShouldDlete() {
	return shouldDelete;
}

void GameObject::computeTangentBasis(
	// inputs
	std::vector<glm::vec3>& vertices,
	std::vector<glm::vec2>& uvs,
	std::vector<glm::vec3>& normals,
	// outputs
	std::vector<glm::vec3>& tangents,
	std::vector<glm::vec3>& bitangents
) {
	for (int i = 0; i < vertices.size(); i += 3) {
		// Shortcuts for vertices
		glm::vec3& v0 = vertices[i + 0];
		glm::vec3& v1 = vertices[i + 1];
		glm::vec3& v2 = vertices[i + 2];

		// Shortcuts for UVs
		glm::vec2& uv0 = uvs[i + 0];
		glm::vec2& uv1 = uvs[i + 1];
		glm::vec2& uv2 = uvs[i + 2];
		// Edges of the triangle : position delta
		glm::vec3 deltaPos1 = v1 - v0;
		glm::vec3 deltaPos2 = v2 - v0;

		// UV delta
		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
		glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

		// Set the same tangent for all three vertices of the triangle.
// They will be merged later, in vboindexer.cpp
		tangents.push_back(tangent);
		tangents.push_back(tangent);
		tangents.push_back(tangent);

		// Same thing for bitangents
		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);
		bitangents.push_back(bitangent);
	}
}

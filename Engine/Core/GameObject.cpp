#include "GameObject.h"
#include "Engine/Core/AssetManager.h"
#include "Engine/Physics/BulletPhysics.h"

GameObject::GameObject() = default;

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
GameObject::GameObject(std::string name, Model* model, glm::vec3 position, bool save, float mass, btConvexHullShape* shape) {
	this->name = name;
	this->model = model;
	parentName = "";
	canSave = save;
	Btransform.setOrigin(glmToBtVector3(position));
	convexHullShape = shape;

	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		convexHullShape->calculateLocalInertia(btScalar(mass), localInertia);

	Btransform.setIdentity();
	Btransform.setOrigin(btVector3(position.x, position.y, position.z));

	// Using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(Btransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(btScalar(mass), myMotionState, convexHullShape, localInertia);
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

GameObject::GameObject(std::string name, Model* model, glm::vec3 position, bool save, float mass, btCollisionShape* shape) {
	{
		this->name = name;
		this->model = model;
		parentName = "";
		canSave = save;
		Btransform.setOrigin(glmToBtVector3(position));
		collider = shape;

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
}



GameObject::GameObject(std::string name, Model* model, glm::vec3 position, bool save, float mass, ColliderShape shape) {
	if (model == nullptr)
		std::cout << "model is null \n";
	this->name = name;
	this->model = model;
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

		for (int i = 0; i < model->GetAllMeshes()->size(); i++) {
			for (const auto& vertex : model->GetMesh(i)->indexed_vertices) {
				if (vertex.x < minPoint.x) minPoint.x = vertex.x;
				if (vertex.y < minPoint.y) minPoint.y = vertex.y;
				if (vertex.z < minPoint.z) minPoint.z = vertex.z;

				if (vertex.x > maxPoint.x) maxPoint.x = vertex.x;
				if (vertex.y > maxPoint.y) maxPoint.y = vertex.y;
				if (vertex.z > maxPoint.z) maxPoint.z = vertex.z;
			}
			if (!model->RenderAll())
				break;
		}
		dimensions = maxPoint - minPoint;
		collider = new btBoxShape(btVector3(btScalar(dimensions.x / 2), btScalar(dimensions.y / 2), btScalar(dimensions.z / 2)));
	}
	else if (shape == Convex) {
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
	else if (shape == Concave) {

		btTriangleMesh*	triangleShape = new btTriangleMesh();


		for (int i = 0; i < model->GetAllMeshes()->size(); i++) {
			for (int x = 0; x < model->GetMesh(i)->indices.size(); x += 3) {
				int i1 = model->GetMesh(i)->indices[x];
				int i2 = model->GetMesh(i)->indices[x + 1];
				int i3 = model->GetMesh(i)->indices[x + 2];

				glm::vec3 vertex1 = model->GetMesh(i)->GetVertices(i1);
				glm::vec3 vertex2 = model->GetMesh(i)->GetVertices(i2);
				glm::vec3 vertex3 = model->GetMesh(i)->GetVertices(i3);

				triangleShape->addTriangle(
					btVector3(vertex1.x, vertex1.y, vertex1.z),
					btVector3(vertex2.x, vertex2.y, vertex2.z),
					btVector3(vertex3.x, vertex3.y, vertex3.z)
				);

			}

			//only create a collider for the entire model/everymesh if the entire mesh is being renderered
			if (!model->RenderAll())
				break;
		}
		
		triangleCollison = new btBvhTriangleMeshShape(triangleShape, true);


	}
	else if (shape == Sphere) {
		glm::vec3 minPoint(std::numeric_limits<float>::max());
		glm::vec3 maxPoint(std::numeric_limits<float>::lowest());
		for (int i = 0; i < model->GetAllMeshes()->size(); i++) {
			for (const auto& vertex : model->GetMesh(i)->indexed_vertices) {
				if (vertex.x < minPoint.x) minPoint.x = vertex.x;
				if (vertex.y < minPoint.y) minPoint.y = vertex.y;
				if (vertex.z < minPoint.z) minPoint.z = vertex.z;

				if (vertex.x > maxPoint.x) maxPoint.x = vertex.x;
				if (vertex.y > maxPoint.y) maxPoint.y = vertex.y;
				if (vertex.z > maxPoint.z) maxPoint.z = vertex.z;
			}
			if (!model->RenderAll())
				break;
		}
		dimensions = maxPoint - minPoint;
		collider = new btSphereShape(btScalar(dimensions.x / 2));
	}
	else if (shape == Capsule) {
		glm::vec3 minPoint(std::numeric_limits<float>::max());
		glm::vec3 maxPoint(std::numeric_limits<float>::lowest());

		for (int i = 0; i < model->GetAllMeshes()->size(); i++) {
			for (const auto& vertex : model->GetMesh(i)->indexed_vertices) {
				if (vertex.x < minPoint.x) minPoint.x = vertex.x;
				if (vertex.y < minPoint.y) minPoint.y = vertex.y;
				if (vertex.z < minPoint.z) minPoint.z = vertex.z;

				if (vertex.x > maxPoint.x) maxPoint.x = vertex.x;
				if (vertex.y > maxPoint.y) maxPoint.y = vertex.y;
				if (vertex.z > maxPoint.z) maxPoint.z = vertex.z;
			}
			if (!model->RenderAll())
				break;
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

	// Using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(Btransform);
	if (convexHullShape != nullptr) {
		btRigidBody::btRigidBodyConstructionInfo rbInfo(btScalar(mass), myMotionState, convexHullShape, localInertia);
		body = new btRigidBody(rbInfo);
	}
	else if (triangleCollison != nullptr) {
		btRigidBody::btRigidBodyConstructionInfo rbInfo(btScalar(mass), myMotionState, triangleCollison, localInertia);
		body = new btRigidBody(rbInfo);
	}
	else {
		btRigidBody::btRigidBodyConstructionInfo rbInfo(btScalar(mass), myMotionState, collider, localInertia);
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
}


GameObject::GameObject(std::string name, Model* model, glm::vec3 position, bool save, float mass, ColliderShape shape, float width, float height, float depth) {
	this->name = name;
	this->model = model;
	parentName = "";
	canSave = save;

	Btransform.setOrigin(glmToBtVector3(position));

	if (shape == Sphere)
		collider = new btSphereShape(btScalar(height / 2));
	else
		collider = new btBoxShape(btVector3(btScalar(width / 2), btScalar(height / 2), btScalar(depth / 2)));

	if(width == 0 || height == 0 || depth == 0)
		collider = new btEmptyShape();

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







Model* GameObject::GetModel() {
	return model;
}

void GameObject::SetUserPoint(void* pointer) {
	body->setUserPointer(pointer);
}

// Parent child transformations
glm::mat4 GameObject::GetModelMatrix() {

	glm::mat4 matrix = transform.to_mat4();
	//Btransform.getOpenGLMatrix(glm::value_ptr(matrix));

	if (!parentName.empty()) {
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
	model->RenderModel(programID);

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
void GameObject::SetTransform(Transform transform) {
	this->transform = transform;

	btTransform& t = body->getWorldTransform();
	btQuaternion quat;
	quat.setEuler(transform.rotation.y, transform.rotation.x, transform.rotation.z);
	t.setRotation(quat);
	body->getMotionState()->setWorldTransform(t);
	body->getWorldTransform().setOrigin(btVector3(transform.position.x, transform.position.y, transform.position.z));
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
	return model->GetCurrentMesh()->indices;
}

std::vector<glm::vec3>  GameObject::getIndexedVerticies() {
	return model->GetCurrentMesh()->indexed_vertices;
}

std::vector<glm::vec2>  GameObject::getIndexedUvs() {
	return model->GetCurrentMesh()->indexed_uvs;
}

std::vector<glm::vec3>  GameObject::getIndexedNormals() {
	return model->GetCurrentMesh()->indexed_normals;
}

btConvexHullShape* GameObject::GetConvexHull() {
	return convexHullShape;
}

const char* GameObject::GetTextureName() {
	return model->GetTextureName();
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

void GameObject::SetShaderType(std::string shaderType) {
	this->shaderType = shaderType;
}
std::string GameObject::GetShaderType() {
	return shaderType;
}



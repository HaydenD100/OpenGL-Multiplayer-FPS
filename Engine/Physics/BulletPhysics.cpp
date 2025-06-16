#include "BulletPhysics.h"
#include "Engine/Core/AssetManager.h"

namespace PhysicsManagerBullet
{
	btAlignedObjectArray<btCollisionShape*> collisionShapes;

	//std::vector<Collider> colliders;
	// debugDrawer;

	btDiscreteDynamicsWorld* dynamicsWorld;
	btSequentialImpulseConstraintSolver* solver;
	btBroadphaseInterface* overlappingPairCache;
	btCollisionDispatcher* dispatcher;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btOverlappingPairCache* cache;
	
	void AddColliderShape(btCollisionShape* collider) {
		collisionShapes.push_back(collider);
	}
	
	void AddRigidBody(btRigidBody* body) {
		dynamicsWorld->addRigidBody(body);
	}
	
	btDiscreteDynamicsWorld* GetDynamicWorld() {
		return dynamicsWorld;
	}

	void Init() {
		///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
		collisionConfiguration = new btDefaultCollisionConfiguration();

		///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
		dispatcher = new btCollisionDispatcher(collisionConfiguration);

		///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
		overlappingPairCache = new btDbvtBroadphase();

		cache = new btHashedOverlappingPairCache();


		///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
		solver = new btSequentialImpulseConstraintSolver;

		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

		dynamicsWorld->setGravity(btVector3(0, -10, 0));

		dynamicsWorld->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());


		//debugDrawer = new DebugDrawer(Renderer::GetProgramID("debug"));
		//dynamicsWorld->setDebugDrawer(debugDrawer);
		//dynamicsWorld->debugDrawWorld();
	}
	
	void Update(float deltaTime) {
		dynamicsWorld->stepSimulation(deltaTime, 3);
		//dynamicsWorld->debugDrawWorld();
	}
	void Delete() {
		// Remove rigid bodies from the world and delete them
		if (dynamicsWorld) {
			for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; --i) {
				btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
				btRigidBody* body = btRigidBody::upcast(obj);
				if (body && body->getMotionState()) {
					delete body->getMotionState();
				}
				dynamicsWorld->removeCollisionObject(obj);
				delete obj;
			}
		}

		// Delete collision shapes
		for (int i = 0; i < collisionShapes.size(); ++i) {
			delete collisionShapes[i];
		}
		collisionShapes.clear();

		// Delete Bullet physics world components
		delete dynamicsWorld;
		dynamicsWorld = nullptr;

		delete solver;
		solver = nullptr;

		delete overlappingPairCache;
		overlappingPairCache = nullptr;

		delete dispatcher;
		dispatcher = nullptr;

		delete collisionConfiguration;
		collisionConfiguration = nullptr;

		delete cache;
		cache = nullptr;

		// Optional: delete debug drawer if you add one later
		// delete debugDrawer;
		// debugDrawer = nullptr;
	}

}

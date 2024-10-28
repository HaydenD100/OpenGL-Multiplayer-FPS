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

		///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
		solver = new btSequentialImpulseConstraintSolver;

		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

		dynamicsWorld->setGravity(btVector3(0, -10, 0));

		//debugDrawer = new DebugDrawer(Renderer::GetProgramID("debug"));
		//dynamicsWorld->setDebugDrawer(debugDrawer);
		//dynamicsWorld->debugDrawWorld();
	}
	
	void Update(float deltaTime) {
		dynamicsWorld->stepSimulation(deltaTime, 3);
		dynamicsWorld->debugDrawWorld();
	}
}

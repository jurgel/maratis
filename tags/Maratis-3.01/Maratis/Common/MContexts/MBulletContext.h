/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MBulletContext.h
//
// Bullet Physics Context 
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================


#ifndef _M_BULLET_CONTEXT_H
#define _M_BULLET_CONTEXT_H

#include <btBulletDynamicsCommon.h>
#include <MCore.h>


class MBulletContext : public MPhysicsContext
{
private:

	btDefaultCollisionConfiguration * m_collisionConfiguration;
	btCollisionDispatcher * m_dispatcher;
	//btAxisSweep3 * m_overlappingPairCache;
	btDbvtBroadphase * m_overlappingPairCache;
	btSequentialImpulseConstraintSolver * m_solver;
	btDiscreteDynamicsWorld * m_dynamicsWorld;
	
	vector<btCollisionShape*> m_collisionShapes;
	vector<btCollisionObject*> m_collisionObjects;
	vector<btTypedConstraint*> m_constraints;

public:

	// constructor
	MBulletContext(void);

	// destructor
	~MBulletContext(void);

	// init
	void init(const MVector3 & worldMin, const MVector3 & worldMax);

	// clear
	void clear(void);

	// update simulation
	void updateSimulation(void);

	// world gravity
	void setWorldGravity(const MVector3 & gravity);

	// create object
	void createGhost(unsigned int * objectId, unsigned int shapeId, const MVector3 & position, const MQuaternion & rotation);
	void createRigidBody(unsigned int * objectId, unsigned int shapeId, const MVector3 & position, const MQuaternion & rotation, float mass);

	// delete object
	void deleteObject(unsigned int * objectId);

	// object properties
	void enableObjectKinematic(unsigned int objectId);
	void disableObjectKinematic(unsigned int objectId);

	void setObjectShape(unsigned int objectId, unsigned int shapeId);
	void setObjectMass(unsigned int objectId, float mass);
	void setObjectRestitution(unsigned int objectId, float restitution);
	void setObjectFriction(unsigned int objectId, float friction);
	void setObjectLinearFactor(unsigned int objectId, const MVector3 & linearFactor);
	void setObjectAngularFactor(unsigned int objectId, float angularFactor);
	void setObjectDamping(unsigned int objectId, float linearDamping, float angularDamping);
	void setObjectTransform(unsigned int objectId, const MVector3 & position, const MQuaternion & rotation);
	void getObjectTransform(unsigned int objectId, MVector3 * position, MQuaternion * rotation);

	// affectors
	void addCentralForce(unsigned int objectId, const MVector3 & force);
	void getCentralForce(unsigned int objectId, MVector3 * force);
	void addTorque(unsigned int objectId, const MVector3 & torque);
	void getTorque(unsigned int objectId, MVector3 * torque);
	void clearForces(unsigned int objectId);

	// objects collision
	int isObjectInCollision(unsigned int objectId);
	bool isObjectsCollision(unsigned int objectId, unsigned int object2Id);

	// create shape
	void createMultiShape(unsigned int * shapeId);

	void createBoxShape(unsigned int * shapeId, const MVector3 & scale);
	void createSphereShape(unsigned int * shapeId, float radius);
	void createConeShape(unsigned int * shapeId, float radius, float height);
	void createCapsuleShape(unsigned int * shapeId, float radius, float height);
	void createCylinderShape(unsigned int * shapeId, float radius, float height);

	void createConvexHullShape(unsigned int * shapeId, const MVector3 * vertices, unsigned int verticesNumber);
	void createTriangleMeshShape(unsigned int * shapeId, const MVector3 * vertices, unsigned int verticesNumber, const void * indices, unsigned int indicesNumber, M_TYPES indicesType);

	// delete shape
	void deleteShape(unsigned int * shapeId);

	// add child shape to multishape
	void addChildShape(unsigned int multiShapeId, unsigned int shapeId, const MVector3 & position, const MQuaternion & rotation);

	// create constraint
	void createConstraint(unsigned int * constraintId, unsigned int parentObjectId, unsigned int objectId, const MVector3 & pivot, bool disableParentCollision);
	void setLinearLimit(unsigned int constraintId, const MVector3 & lower, const MVector3 & upper);
	void setAngularLimit(unsigned int constraintId, const MVector3 & lower, const MVector3 & upper);
	
	// delete constraint
	void deleteConstraint(unsigned int * constraintId);
};

#endif
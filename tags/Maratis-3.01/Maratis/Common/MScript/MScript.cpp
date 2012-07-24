/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MScript.cpp
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


#include "MScript.h"


char g_currentDirectory[256] = "";

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global Functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////

bool isFunctionOk(lua_State * L, const char * name, unsigned int nbArgs)
{
	int nbArguments = lua_gettop(L);
	if(nbArguments < (int)nbArgs)
	{
		printf("ERROR script : \"%s\" need at least %d parameter(s)\n", name, nbArgs);
		return false;
	}
	return true;
}

void pushFloatArray(lua_State* L, float * values, unsigned int nbValues)
{
	lua_newtable(L);

	for(unsigned int i=0; i<nbValues; i++)
	{
		lua_pushinteger(L, i+1);
		lua_pushnumber(L, values[i]);
		lua_rawset(L, -3);
	}
}

MObject3d * getObject3d(unsigned int id)
{
	MLevel * level = MEngine::getInstance()->getLevel();
	MScene * scene = level->getCurrentScene();

	if(id < scene->getObjectsNumber())
		return scene->getObjectByIndex(id);

	return NULL;
}

bool getVector2(lua_State * L, int index, MVector2 * vector)
{
	if(lua_istable(L, index) && (lua_objlen(L, index) >= 2))
	{
		lua_pushnil(L);

		lua_next(L, index);
		vector->x = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_next(L, index);
		vector->y = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);

		return true;
	}

	return false;
}

bool getVector3(lua_State * L, int index, MVector3 * vector)
{
	if(lua_istable(L, index) && (lua_objlen(L, index) >= 3))
	{
		lua_pushnil(L);

		lua_next(L, index);
		vector->x = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_next(L, index);
		vector->y = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_next(L, index);
		vector->z = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);

		return true;
	}

	return false;
}

int getObject(lua_State * L)
{
	MLevel * level = MEngine::getInstance()->getLevel();
	MScene * scene = level->getCurrentScene();

	if(! isFunctionOk(L, "getObject", 1))
	{
		lua_pushinteger(L, (lua_Integer)-1);
		return 1;
	}

	const char * name = lua_tostring(L, 1);
	if(name)
	{
		unsigned int id;
		if(scene->getObjectIndex(name, &id))
		{
			lua_pushinteger(L, (lua_Integer)id);
			return 1;
		}
	}

	printf("ERROR script : object \"%s\" doesn't exit\n", name);
	lua_pushinteger(L, (lua_Integer)-1);
	return 1;
}

int rotate(lua_State * L)
{
	if(! isFunctionOk(L, "rotate", 3))
		return 0;

	int nbArguments = lua_gettop(L);

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		MVector3 axis;
		if(getVector3(L, 2, &axis))
		{
			// get angle
			float angle = (float)lua_tonumber(L, 3);

			// is local ?
			bool local = false;
			if(nbArguments > 3)
			{
				const char * mode = lua_tostring(L, 4);
				if(strcmp(mode, "local") == 0)
					local = true;
			}
			
			// rotate
			if(local)
				object->addAxisAngleRotation(axis, angle);
			else
				object->addAxisAngleRotation(object->getInverseRotatedVector(axis), angle);
		}
	}

	return 0;
}

int translate(lua_State * L)
{
	if(! isFunctionOk(L, "translate", 2))
		return 0;

	int nbArguments = lua_gettop(L);

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		MVector3 axis;
		if(getVector3(L, 2, &axis))
		{
			// is local ?
			bool local = false;
			if(nbArguments > 2)
			{
				const char * mode = lua_tostring(L, 3);
				if(strcmp(mode, "local") == 0)
					local = true;
			}

			// rotate
			if(local)
			{
				axis = object->getRotatedVector(axis);
				object->setPosition(object->getPosition() + axis);
			}
			else
				object->setPosition(object->getPosition() + axis);
		}
	}

	return 0;
}

int getPosition(lua_State * L)
{
	if(! isFunctionOk(L, "getPosition", 1))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		pushFloatArray(L, object->getPosition(), 3);
		return 1;
	}

	return 0;
}

int getRotation(lua_State * L)
{
	if(! isFunctionOk(L, "getRotation", 1))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		pushFloatArray(L, object->getEulerRotation(), 3);
		return 1;
	}

	return 0;
}

int getScale(lua_State * L)
{
	if(! isFunctionOk(L, "getScale", 1))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		pushFloatArray(L, object->getScale(), 3);
		return 1;
	}

	return 0;
}

int setPosition(lua_State * L)
{
	if(! isFunctionOk(L, "setPosition", 2))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		MVector3 position;
		if(getVector3(L, 2, &position))
		{
			object->setPosition(position);
		}
	}

	return 0;
}

int setRotation(lua_State * L)
{
	if(! isFunctionOk(L, "setRotation", 2))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		MVector3 rotation;
		if(getVector3(L, 2, &rotation))
		{
			object->setEulerRotation(rotation);
		}
	}

	return 0;
}

int setScale(lua_State * L)
{
	if(! isFunctionOk(L, "setScale", 2))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		MVector3 scale;
		if(getVector3(L, 2, &scale))
		{
			object->setScale(scale);
		}
	}

	return 0;
}

int isVisible(lua_State * L)
{
	if(! isFunctionOk(L, "isVisible", 1))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		int vis = object->isVisible();
		lua_pushboolean(L, vis);
		return 1;
	}

	return 0;
}

int activate(lua_State * L)
{
	if(! isFunctionOk(L, "activate", 1))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		object->setActive(true);
		return 0;
	}

	return 0;
}

int deactivate(lua_State * L)
{
	if(! isFunctionOk(L, "deactivate", 1))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		object->setActive(false);
		return 0;
	}

	return 0;
}

int changeAnimation(lua_State * L)
{
	if(! isFunctionOk(L, "changeAnimation", 2))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;

			unsigned int anim = (unsigned int)lua_tointeger(L, 2);
			entity->changeAnimation(anim);
			return 0;
		}
	}

	return 0;
}

int isAnimationOver(lua_State * L)
{
	if(! isFunctionOk(L, "isAnimationOver", 1))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			int isAnimOver = entity->isAnimationOver();
			lua_pushboolean(L, isAnimOver);
			return 1;
		}
	}

	return 0;
}

int getCurrentAnimation(lua_State * L)
{
	if(! isFunctionOk(L, "getCurrentAnimation", 1))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			int anim = (int)entity->getAnimationId();
			lua_pushinteger(L, anim);
			return 1;
		}
	}

	return 0;
}

int getGravity(lua_State * L)
{
	MLevel * level = MEngine::getInstance()->getLevel();
	MScene * scene = level->getCurrentScene();

	pushFloatArray(L, *scene->getGravity(), 3);

	return 1;
}

int setGravity(lua_State * L)
{
	MLevel * level = MEngine::getInstance()->getLevel();
	MScene * scene = level->getCurrentScene();

	if(! isFunctionOk(L, "setGravity", 1))
		return 0;

	MVector3 gravity;
	if(getVector3(L, 1, &gravity))
		scene->setGravity(gravity);

	return 0;
}

int changeCurrentCamera(lua_State * L)
{
	MLevel * level = MEngine::getInstance()->getLevel();
	MScene * scene = level->getCurrentScene();
	
	if(! isFunctionOk(L, "changeCurrentCamera", 1))
		return 0;

	unsigned int id = (unsigned int)lua_tointeger(L, 1);
	MObject3d * object = getObject3d(id);
	if(object)
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			unsigned int i;
			unsigned int size = scene->getCamerasNumber();
			for(i=0; i<size; i++)
			{
				if(object == (MObject3d *)scene->getCameraByIndex(i))
				{
					scene->setCurrentCamera(i);
					break;
				}
			}
		}
	}

	return 0;
}

int addCentralForce(lua_State * L)
{
	int nbArguments = lua_gettop(L);
	if(! isFunctionOk(L, "addCentralForce", 2))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MVector3 force;
			if(getVector3(L, 2, &force))
			{
				// is local ?
				bool local = false;
				if(nbArguments > 2)
				{
					const char * mode = lua_tostring(L, 3);
					if(strcmp(mode, "local") == 0)
						local = true;
				}

				MOEntity * entity = (MOEntity*)object;
				MPhysicsProperties * phyProps = entity->getPhysicsProperties();
				if(phyProps)
				{
					if(local)
						force = object->getRotatedVector(force);

					MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();
					physics->addCentralForce(phyProps->getCollisionObjectId(), force);
				}
			}
		}
	}

	return 0;
}

int clearForces(lua_State * L)
{
	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			MPhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();
				physics->clearForces(phyProps->getCollisionObjectId());
			}
		}
	}

	return 0;
}

int addTorque(lua_State * L)
{
	int nbArguments = lua_gettop(L);
	if(! isFunctionOk(L, "addTorque", 2))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MVector3 torque;
			if(getVector3(L, 2, &torque))
			{
				// is local ?
				bool local = false;
				if(nbArguments > 2)
				{
					const char * mode = lua_tostring(L, 3);
					if(strcmp(mode, "local") == 0)
						local = true;
				}

				MOEntity * entity = (MOEntity*)object;
				MPhysicsProperties * phyProps = entity->getPhysicsProperties();
				if(phyProps)
				{
					if(local)
						torque = object->getRotatedVector(torque);

					MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();
					physics->addTorque(phyProps->getCollisionObjectId(), torque);
				}
			}
		}
	}

	return 0;
}

int getLinearDamping(lua_State * L)
{
	if(! isFunctionOk(L, "getLinearDamping", 1))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);
	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			MPhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				float linearDamping = phyProps->getLinearDamping();
				lua_pushnumber(L, (lua_Number)linearDamping);
				return 1;
			}
		}
	}

	return 0;
}

int setLinearDamping(lua_State * L)
{
	MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();

	if(! isFunctionOk(L, "setLinearDamping", 2))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);
	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			MPhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				float linearDamping = (float)lua_tonumber(L, 2);
				phyProps->setLinearDamping(linearDamping);
				physics->setObjectDamping(
					phyProps->getCollisionObjectId(),
					phyProps->getLinearDamping(),
					phyProps->getAngularDamping()
					);
				return 0;
			}
		}
	}

	return 0;
}

int getAngularDamping(lua_State * L)
{
	if(! isFunctionOk(L, "getAngularDamping", 1))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);
	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			MPhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				float angularDamping = phyProps->getAngularDamping();
				lua_pushnumber(L, (lua_Number)angularDamping);
				return 1;
			}
		}
	}

	return 0;
}

int setAngularDamping(lua_State * L)
{
	MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();

	if(! isFunctionOk(L, "setAngularDamping", 2))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);
	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			MPhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				float angularDamping = (float)lua_tonumber(L, 2);
				phyProps->setAngularDamping(angularDamping);
				physics->setObjectDamping(
					phyProps->getCollisionObjectId(),
					phyProps->getLinearDamping(),
					phyProps->getAngularDamping()
					);
				return 0;
			}
		}
	}

	return 0;
}

int getNumCollisions(lua_State * L)
{
	if(! isFunctionOk(L, "getNumCollisions", 1))
		return 0;
	
	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);
	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			MPhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();
				int collision = physics->isObjectInCollision(phyProps->getCollisionObjectId());
				lua_pushinteger(L, collision);
				return 1;
			}
		}
	}
	
	return 0;
}

int isCollisionTest(lua_State * L)
{
	if(! isFunctionOk(L, "isCollisionTest", 1))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);
	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_ENTITY)
		{
			MOEntity * entity = (MOEntity*)object;
			MPhysicsProperties * phyProps = entity->getPhysicsProperties();
			if(phyProps)
			{
				MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();
				int collision = MIN(1, physics->isObjectInCollision(phyProps->getCollisionObjectId()));
				lua_pushboolean(L, collision);
				return 1;
			}
		}
	}

	return 0;
}

int isCollisionBetween(lua_State * L)
{
	if(! isFunctionOk(L, "isCollisionBetween", 2))
		return 0;

	MObject3d * object1;
	MObject3d * object2;

	unsigned int id1 = (unsigned int)lua_tointeger(L, 1);
	unsigned int id2 = (unsigned int)lua_tointeger(L, 2);
	if((object1 = getObject3d(id1)) && (object2 = getObject3d(id2)))
	{
		if((object1->getType() == M_OBJECT3D_ENTITY) && (object2->getType() == M_OBJECT3D_ENTITY))
		{
			MOEntity * entity1 = (MOEntity*)object1;
			MOEntity * entity2 = (MOEntity*)object2;

			MPhysicsProperties * phyProps1 = entity1->getPhysicsProperties();
			MPhysicsProperties * phyProps2 = entity2->getPhysicsProperties();
			if(phyProps1 && phyProps2)
			{
				MPhysicsContext * physics = MEngine::getInstance()->getPhysicsContext();
				int collision = physics->isObjectsCollision(
					phyProps1->getCollisionObjectId(),
					phyProps2->getCollisionObjectId()
					);
				lua_pushboolean(L, collision);
				return 1;
			}
		}
	}

	return 0;
}

int isKeyPressed(lua_State * L)
{
	MInputContext * input = MEngine::getInstance()->getInputContext();

	if(! isFunctionOk(L, "isKeyPressed", 1))
		return 0;

	const char * name = lua_tostring(L, 1);
	if(name)
	{
		int keyPressed = input->isKeyPressed(name);
		lua_pushboolean(L, keyPressed);
		return 1;
	}

	return 0;
}

int onKeyDown(lua_State * L)
{
	MInputContext * input = MEngine::getInstance()->getInputContext();

	if(! isFunctionOk(L, "onKeyDown", 1))
		return 0;

	const char * name = lua_tostring(L, 1);
	if(name)
	{
		int keyDown = input->onKeyDown(name);
		lua_pushboolean(L, keyDown);
		return 1;
	}

	return 0;
}

int onKeyUp(lua_State * L)
{
	MInputContext * input = MEngine::getInstance()->getInputContext();

	if(! isFunctionOk(L, "onKeyUp", 1))
		return 0;

	const char * name = lua_tostring(L, 1);
	if(name)
	{
		int keyUp = input->onKeyUp(name);
		lua_pushboolean(L, keyUp);
		return 1;
	}

	return 0;
}

int getAxis(lua_State * L)
{
	MInputContext * input = MEngine::getInstance()->getInputContext();

	if(! isFunctionOk(L, "getAxis", 1))
		return 0;

	const char * name = lua_tostring(L, 1);
	if(name)
	{
		float axis = input->getAxis(name);
		lua_pushnumber(L, axis);
		return 1;
	}

	return 0;
}

int playSound(lua_State * L)
{
	if(! isFunctionOk(L, "playSound", 1))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_SOUND)
		{
			MOSound * sound = (MOSound*)object;
			sound->play();
		}
	}

	return 0;
}

int pauseSound(lua_State * L)
{
	if(! isFunctionOk(L, "pauseSound", 1))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_SOUND)
		{
			MOSound * sound = (MOSound*)object;
			sound->pause();
		}
	}

	return 0;
}

int stopSound(lua_State * L)
{
	if(! isFunctionOk(L, "stopSound", 1))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_SOUND)
		{
			MOSound * sound = (MOSound*)object;
			sound->stop();
		}
	}

	return 0;
}

int changeScene(lua_State * L)
{
	MEngine * engine = MEngine::getInstance();
	MLevel * level = engine->getLevel();
	MScriptContext * scriptContext = engine->getScriptContext();
	
	if(! isFunctionOk(L, "changeScene", 1))
		return 0;

	unsigned int id = (unsigned int)lua_tointeger(L, 1);
	level->changeCurrentScene(id);

	return 0;
}

int loadLevel(lua_State * L)
{
	MEngine * engine = MEngine::getInstance();

	if(! isFunctionOk(L, "loadLevel", 1))
		return 0;

	const char * filename = lua_tostring(L, 1);
	if(filename)
		engine->loadLevel(filename);

	return 0;
}

int getLightColor(lua_State * L)
{
	if(! isFunctionOk(L, "getLightColor", 1))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			MOLight * light = (MOLight*)object;
			pushFloatArray(L, *light->getColor(), 3);
			return 1;
		}
	}

	return 0;
}

int getLightRadius(lua_State * L)
{
	if(! isFunctionOk(L, "getLightRadius", 1))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			MOLight * light = (MOLight*)object;
			lua_pushnumber(L, (lua_Number)light->getRadius());
			return 1;
		}
	}

	return 0;
}

int getLightIntensity(lua_State * L)
{
	if(! isFunctionOk(L, "getLightIntensity", 1))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			MOLight * light = (MOLight*)object;
			lua_pushnumber(L, (lua_Number)light->getIntensity());
			return 1;
		}
	}

	return 0;
}

int setLightColor(lua_State * L)
{
	if(! isFunctionOk(L, "setLightColor", 2))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		MVector3 color;
		if(object->getType() == M_OBJECT3D_LIGHT && getVector3(L, 2, &color))
		{
			MOLight * light = (MOLight*)object;
			light->setColor(color);
			return 0;
		}
	}

	return 0;
}

int setLightRadius(lua_State * L)
{
	if(! isFunctionOk(L, "setLightRadius", 2))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			float radius = (float)lua_tonumber(L, 2);

			MOLight * light = (MOLight*)object;
			light->setRadius(radius);
			return 0;
		}
	}

	return 0;
}

int setLightIntensity(lua_State * L)
{
	if(! isFunctionOk(L, "setLightIntensity", 2))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_LIGHT)
		{
			float intensity = (float)lua_tonumber(L, 2);

			MOLight * light = (MOLight*)object;
			light->setIntensity(intensity);
			return 0;
		}
	}

	return 0;
}

int getSoundGain(lua_State * L)
{
	if(! isFunctionOk(L, "getSoundGain", 1))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_SOUND)
		{
			MOSound * sound = (MOSound*)object;
			lua_pushnumber(L, (lua_Number)sound->getGain());
			return 1;
		}
	}

	return 0;
}

int setSoundGain(lua_State * L)
{
	if(! isFunctionOk(L, "setSoundGain", 2))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_SOUND)
		{
			float gain = (float)lua_tonumber(L, 2);
			MOSound * sound = (MOSound*)object;
			sound->setGain(gain);
			return 0;
		}
	}

	return 0;
}

int setCameraClearColor(lua_State * L)
{
	if(! isFunctionOk(L, "setCameraClearColor", 2))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		MVector3 color;
		if(object->getType() == M_OBJECT3D_CAMERA && getVector3(L, 2, &color))
		{
			MOCamera * camera = (MOCamera*)object;
			camera->setClearColor(color);
			return 0;
		}
	}

	return 0;
}

int getCameraClearColor(lua_State * L)
{
	if(! isFunctionOk(L, "getCameraClearColor", 1))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			MOCamera * camera = (MOCamera*)object;
			pushFloatArray(L, *camera->getClearColor(), 3);
			return 1;
		}
	}

	return 0;
}

int setCameraNear(lua_State * L)
{
	if(! isFunctionOk(L, "setCameraNear", 2))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			float clipNear = (float)lua_tonumber(L, 2);
			MOCamera * camera = (MOCamera*)object;
			camera->setClippingNear(clipNear);
			return 0;
		}
	}

	return 0;
}

int getCameraNear(lua_State * L)
{
	if(! isFunctionOk(L, "getCameraNear", 1))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			MOCamera * camera = (MOCamera*)object;
			lua_pushnumber(L, (lua_Number)camera->getClippingNear());
			return 1;
		}
	}

	return 0;
}

int setCameraFar(lua_State * L)
{
	if(! isFunctionOk(L, "setCameraFar", 2))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			float clipFar = (float)lua_tonumber(L, 2);
			MOCamera * camera = (MOCamera*)object;
			camera->setClippingFar(clipFar);
			return 0;
		}
	}

	return 0;
}

int getCameraFar(lua_State * L)
{
	if(! isFunctionOk(L, "getCameraFar", 1))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			MOCamera * camera = (MOCamera*)object;
			lua_pushnumber(L, (lua_Number)camera->getClippingFar());
			return 1;
		}
	}

	return 0;
}

int setCameraFov(lua_State * L)
{
	if(! isFunctionOk(L, "setCameraFov", 2))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			float fov = (float)lua_tonumber(L, 2);
			MOCamera * camera = (MOCamera*)object;
			camera->setFov(fov);
			return 0;
		}
	}

	return 0;
}

int getCameraFov(lua_State * L)
{
	if(! isFunctionOk(L, "getCameraFov", 1))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			MOCamera * camera = (MOCamera*)object;
			lua_pushnumber(L, (lua_Number)camera->getFov());
			return 1;
		}
	}

	return 0;
}

int setCameraFogDistance(lua_State * L)
{
	if(! isFunctionOk(L, "setCameraFogDistance", 2))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			float fogDist = (float)lua_tonumber(L, 2);
			MOCamera * camera = (MOCamera*)object;
			camera->setFogDistance(fogDist);
			return 0;
		}
	}

	return 0;
}

int getCameraFogDistance(lua_State * L)
{
	if(! isFunctionOk(L, "getCameraFogDistance", 1))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			MOCamera * camera = (MOCamera*)object;
			lua_pushnumber(L, (lua_Number)camera->getFogDistance());
			return 1;
		}
	}

	return 0;
}

int enableCameraOrtho(lua_State * L)
{
	if(! isFunctionOk(L, "enableCameraOrtho", 2))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			bool ortho = lua_toboolean(L, 2) == 1;
			MOCamera * camera = (MOCamera*)object;
			camera->enableOrtho(ortho);
			return 0;
		}
	}

	return 0;
}

int isCameraOrtho(lua_State * L)
{
	if(! isFunctionOk(L, "isCameraOrtho", 1))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			MOCamera * camera = (MOCamera*)object;
			lua_pushboolean(L, camera->isOrtho());
			return 1;
		}
	}

	return 0;
}

int enableCameraFog(lua_State * L)
{
	if(! isFunctionOk(L, "enableCameraFog", 2))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			bool fog = lua_toboolean(L, 2) == 1;
			MOCamera * camera = (MOCamera*)object;
			camera->enableFog(fog);
			return 0;
		}
	}

	return 0;
}

int isCameraFogEnabled(lua_State * L)
{
	if(! isFunctionOk(L, "isCameraFogEnabled", 1))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_CAMERA)
		{
			MOCamera * camera = (MOCamera*)object;
			lua_pushboolean(L, camera->hasFog());
			return 1;
		}
	}

	return 0;
}

int getBehaviorVariable(lua_State * L)
{
	if(! isFunctionOk(L, "getBehaviorVariable", 3))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		unsigned int bId = (unsigned int)lua_tointeger(L, 2);
		const char * varName = lua_tostring(L, 3);

		if(varName)
		{
			unsigned bSize = object->getBehaviorsNumber();
			if(bId < bSize)
			{
				MBehavior * behavior = object->getBehavior(bId);
				unsigned int v;
				unsigned int vSize = behavior->getVariablesNumber();
				for(v=0; v<vSize; v++)
				{
					MVariable variable = behavior->getVariable(v);
					if(strcmp(variable.getName(), varName) == 0)
					{
						switch(variable.getType())
						{
						case M_VARIABLE_BOOL:
							{
								lua_pushboolean(L, *(bool *)variable.getPointer());
								return 1;
							}
						case M_VARIABLE_INT:
						case M_VARIABLE_UINT:
							{
								lua_pushinteger(L, *(int *)variable.getPointer());
								return 1;
							}
						case M_VARIABLE_FLOAT:
							{
								lua_pushnumber(L, *(float *)variable.getPointer());
								return 1;
							}
						case M_VARIABLE_STRING:
							{
								lua_pushstring(L, ((MString *)variable.getPointer())->getData());
								return 1;
							}
						case M_VARIABLE_VEC2:
							{
								pushFloatArray(L, *(MVector2 *)variable.getPointer(), 2);
								return 1;
							}
						case M_VARIABLE_VEC3:
							{
								pushFloatArray(L, *(MVector3 *)variable.getPointer(), 3);
								return 1;
							}
						}
					}
				}
			}
		}
	}

	return 0;
}

int setBehaviorVariable(lua_State * L)
{
	if(! isFunctionOk(L, "setBehaviorVariable", 4))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		unsigned int bId = (unsigned int)lua_tointeger(L, 2);
		const char * varName = lua_tostring(L, 3);

		if(varName)
		{
			unsigned bSize = object->getBehaviorsNumber();
			if(bId < bSize)
			{
				MBehavior * behavior = object->getBehavior(bId);
				unsigned int v;
				unsigned int vSize = behavior->getVariablesNumber();
				for(v=0; v<vSize; v++)
				{
					MVariable variable = behavior->getVariable(v);
					if(strcmp(variable.getName(), varName) == 0)
					{
						switch(variable.getType())
						{
						case M_VARIABLE_BOOL:
							{
								bool val = lua_toboolean(L, 4) == 1;
								*(bool *)variable.getPointer() = val;
								return 0;
							}
						case M_VARIABLE_INT:
						case M_VARIABLE_UINT:
							{
								int val = lua_tointeger(L, 4);
								*(int *)variable.getPointer() = val;
								return 0;
							}
						case M_VARIABLE_FLOAT:
							{
								float val = (float)lua_tonumber(L, 4);
								*(float *)variable.getPointer() = val;
								return 0;
							}
						case M_VARIABLE_STRING:
							{
								const char * str = lua_tostring(L, 4);
								if(str)
									((MString *)variable.getPointer())->set(str);
								return 0;
							}
						case M_VARIABLE_VEC2:
							{
								MVector2 vec;
								if(getVector2(L, 2, &vec))
									*(MVector2 *)variable.getPointer() = vec;
								return 0;
							}
						case M_VARIABLE_VEC3:
							{
								MVector3 vec;
								if(getVector3(L, 2, &vec))
									*(MVector3 *)variable.getPointer() = vec;
								return 0;
							}
						}
					}
				}
			}
		}
	}

	return 0;
}

int centerCursor(lua_State * L)
{
	MEngine * engine = MEngine::getInstance();
	MSystemContext * system = engine->getSystemContext();
	MInputContext * input = engine->getInputContext();

	unsigned int width = 0;
	unsigned int height = 0;
	system->getScreenSize(&width, &height);
	int x = width/2;
	int y = height/2;

	system->setCursorPosition(x, y);
	input->setAxis("MOUSE_X", (float)(x / (float)width));
	input->setAxis("MOUSE_Y", (float)(y / (float)height));

	return 0;
}

int getText(lua_State * L)
{
	if(! isFunctionOk(L, "getText", 1))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_TEXT)
		{
			const char * text = ((MOText *)object)->getText();
			if(text){
				lua_pushstring(L, text);
				return 1;
			}
		}
	}

	return 0;
}

int setText(lua_State * L)
{
	if(! isFunctionOk(L, "setText", 2))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		const char * text = lua_tostring(L, 2);
		if((text) && (object->getType() == M_OBJECT3D_TEXT))
		{
			((MOText *)object)->setText(text);
		}
	}

	return 0;
}

int getTextColor(lua_State * L)
{
	MScript * script = (MScript *)MEngine::getInstance()->getScriptContext();

	if(! isFunctionOk(L, "getTextColor", 1))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_TEXT)
		{
			MVector4 * color = ((MOText *)object)->getColor();
			script->pushFloatArray(*color, 4);
			return 1;
		}
	}

	return 0;
}

int setTextColor(lua_State * L)
{
	MScript * script = (MScript *)MEngine::getInstance()->getScriptContext();

	if(! isFunctionOk(L, "setTextColor", 2))
		return 0;

	MObject3d * object;
	unsigned int id = (unsigned int)lua_tointeger(L, 1);

	if(object = getObject3d(id))
	{
		if(object->getType() == M_OBJECT3D_TEXT)
		{
			MVector4 color;
			script->getFloatArray(1, color, 4);
			((MOText *)object)->setColor(color);
		}
	}

	return 0;
}

int doFile(lua_State * L)
{
	if(! isFunctionOk(L, "doFile", 1))
		return 0;

	const char * filename = lua_tostring(L, 1);

	// save current directory
	char current[256];
	strcpy(current, g_currentDirectory);
	
	// make global filename
	char globalFilename[256];
	getGlobalFilename(globalFilename, g_currentDirectory, filename);

	// do file and update current directory
	getRepertory(g_currentDirectory, globalFilename);
	luaL_dofile(L, globalFilename);

	// set back to current
	strcpy(g_currentDirectory, current);
	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Init
/////////////////////////////////////////////////////////////////////////////////////////////////////////

MScript::MScript(void):
m_isRunning(false)
{
	init();
}

MScript::~MScript(void)
{
	clear();
}

void MScript::init(void)
{
	// create context
	m_state = lua_open();

	luaopen_base(m_state);
	luaopen_table(m_state);
	luaopen_string(m_state);
	luaopen_math(m_state);

	// load custom functions
	lua_register(m_state, "getObject",	 getObject);
	lua_register(m_state, "rotate",		 rotate);
	lua_register(m_state, "translate",	 translate);
	lua_register(m_state, "getPosition", getPosition);
	lua_register(m_state, "getRotation", getRotation);
	lua_register(m_state, "getScale",	 getScale);
	lua_register(m_state, "setPosition", setPosition);
	lua_register(m_state, "setRotation", setRotation);
	lua_register(m_state, "setScale",	 setScale);
	lua_register(m_state, "isVisible",	 isVisible);
	lua_register(m_state, "activate",	 activate);
	lua_register(m_state, "deactivate", deactivate);

	// behavior
	lua_register(m_state, "getBehaviorVariable", getBehaviorVariable);
	lua_register(m_state, "setBehaviorVariable", setBehaviorVariable);

	// animation
	lua_register(m_state, "getCurrentAnimation", getCurrentAnimation);
	lua_register(m_state, "changeAnimation",	 changeAnimation);
	lua_register(m_state, "isAnimationOver",	 isAnimationOver);

	// physics
	lua_register(m_state, "setGravity",			setGravity);
	lua_register(m_state, "getGravity",			getGravity);
	lua_register(m_state, "addCentralForce",	addCentralForce);
	lua_register(m_state, "addTorque",			addTorque);
	lua_register(m_state, "getLinearDamping",	getLinearDamping);
	lua_register(m_state, "setLinearDamping",	setLinearDamping);
	lua_register(m_state, "getAngularDamping",	getAngularDamping);
	lua_register(m_state, "setAngularDamping",	setAngularDamping);
	lua_register(m_state, "isCollisionTest",	isCollisionTest);
	lua_register(m_state, "isCollisionBetween", isCollisionBetween);
	lua_register(m_state, "clearForces",		clearForces);
	lua_register(m_state, "getNumCollisions",	getNumCollisions);

	// input
	lua_register(m_state, "isKeyPressed", isKeyPressed);
	lua_register(m_state, "onKeyDown",	  onKeyDown);
	lua_register(m_state, "onKeyUp",	  onKeyUp);
	lua_register(m_state, "getAxis",	  getAxis);

	// sound
	lua_register(m_state, "playSound",	  playSound);
	lua_register(m_state, "pauseSound",	  pauseSound);
	lua_register(m_state, "stopSound",	  stopSound);
	lua_register(m_state, "getSoundGain", getSoundGain);
	lua_register(m_state, "setSoundGain", setSoundGain);
	
	// scene/level
	lua_register(m_state, "changeScene", changeScene);
	lua_register(m_state, "loadLevel",	 loadLevel);

	// light
	lua_register(m_state, "getLightColor",	   getLightColor);
	lua_register(m_state, "getLightRadius",	   getLightRadius);
	lua_register(m_state, "getLightIntensity", getLightIntensity);
	lua_register(m_state, "setLightColor",	   setLightColor);
	lua_register(m_state, "setLightRadius",	   setLightRadius);
	lua_register(m_state, "setLightIntensity", setLightIntensity);

	// camera
	lua_register(m_state, "changeCurrentCamera",  changeCurrentCamera);
	lua_register(m_state, "getCameraClearColor",  getCameraClearColor);
	lua_register(m_state, "getCameraFov",		  getCameraFov);
	lua_register(m_state, "getCameraNear",		  getCameraNear);
	lua_register(m_state, "getCameraFar",		  getCameraFar);
	lua_register(m_state, "getCameraFogDistance", getCameraFogDistance);
	lua_register(m_state, "isCameraOrtho",		  isCameraOrtho);
	lua_register(m_state, "isCameraFogEnabled",	  isCameraFogEnabled);
	lua_register(m_state, "setCameraClearColor",  setCameraClearColor);
	lua_register(m_state, "setCameraFov",		  setCameraFov);
	lua_register(m_state, "setCameraNear",	      setCameraNear);
	lua_register(m_state, "setCameraFar",	      setCameraFar);
	lua_register(m_state, "setCameraFogDistance", setCameraFogDistance);
	lua_register(m_state, "enableCameraOrtho",    enableCameraOrtho);
	lua_register(m_state, "enableCameraFog",	  enableCameraFog);

	// text
	lua_register(m_state, "getText", getText);
	lua_register(m_state, "setText", setText);
	lua_register(m_state, "getTextColor", getTextColor);
	lua_register(m_state, "setTextColor", setTextColor);

	// cursor
	lua_register(m_state, "centerCursor", centerCursor);

	// dofile
	lua_register(m_state, "dofile", doFile);

	// register functions
	map<string, int (*)(void)>::iterator
		mit (m_functions.begin()),
		mend(m_functions.end());

	for(;mit!=mend;++mit)
		lua_register(m_state, mit->first.c_str(), function);
}

void MScript::clear(void)
{
	lua_close(m_state);
	m_isRunning = false;
}

int MScript::function(lua_State * L)
{
	MScript * script = (MScript *)MEngine::getInstance()->getScriptContext();

	lua_Debug ar;
	lua_getstack(L, 0, &ar);
	lua_getinfo(L, "n", &ar);

	map<string, int (*)(void)>::iterator iter = script->m_functions.find(ar.name);
	if(iter != script->m_functions.end())
		return iter->second();

	return 0;
}

void MScript::runScript(const char * filename)
{
	if(m_isRunning)
	{
		clear();
		init();
	}

	// run script
	if(! filename)
	{
		m_isRunning = false;
		return;
	}

	if(strlen(filename) == 0)
	{
		m_isRunning = false;
		return;
	}

	getRepertory(g_currentDirectory, filename);
	if(luaL_dofile(m_state, filename) != 0)
	{
		// print errors
		printf("ERROR lua script :\n %s\n", lua_tostring(m_state, -1));
		m_isRunning = false;
		return;
	}

	m_isRunning = true;
}

void MScript::callFunction(const char * name)
{
	if(m_isRunning)
	{
		lua_getglobal(m_state, name);
		if(lua_isfunction(m_state, -1))
		{
			if(lua_pcall(m_state, 0, 0, 0) != 0)
			{
				printf("ERROR lua script :\n %s\n", lua_tostring(m_state, -1));
				m_isRunning = false;
			}
		}
		else
		{
			lua_pop(m_state, 1);
		}
	}
}

void MScript::addFunction(const char * name, int (*function)(void)){
	m_functions[name] = function;
}

unsigned int MScript::getArgsNumber(void){
	return lua_gettop(m_state);
}

void MScript::getIntArray(unsigned int arg, int * values, unsigned int valuesNumber)
{
	arg++;
	if(lua_istable(m_state, arg) && (lua_objlen(m_state, arg) >= valuesNumber))
	{
		lua_pushnil(m_state);
		for(unsigned int i=0; i<valuesNumber; i++)
		{
			lua_next(m_state, arg);
			values[i] = (int)lua_tointeger(m_state, -1);
			lua_pop(m_state, 1);
		}
	}
}

void MScript::getFloatArray(unsigned int arg, float * values, unsigned int valuesNumber)
{
	arg++;
	if(lua_istable(m_state, arg) && (lua_objlen(m_state, arg) >= valuesNumber))
	{
		lua_pushnil(m_state);
		for(unsigned int i=0; i<valuesNumber; i++)
		{
			lua_next(m_state, arg);
			values[i] = (float)lua_tonumber(m_state, -1);
			lua_pop(m_state, 1);
		}
	}
}

const char * MScript::getString(unsigned int arg){
	return lua_tostring(m_state, arg+1);
}

int MScript::getInteger(unsigned int arg){
	return (int)lua_tointeger(m_state, arg+1);
}

float MScript::getFloat(unsigned int arg){
	return (float)lua_tonumber(m_state, arg+1);
}

void MScript::pushIntArray(const int * values, unsigned int valuesNumber)
{
	lua_newtable(m_state);
	for(unsigned int i=0; i<valuesNumber; i++)
	{
		lua_pushinteger(m_state, (lua_Integer)i+1);
		lua_pushnumber(m_state, (lua_Integer)values[i]);
		lua_rawset(m_state, -3);
	}
}

void MScript::pushFloatArray(const float * values, unsigned int valuesNumber)
{
	lua_newtable(m_state);
	for(unsigned int i=0; i<valuesNumber; i++)
	{
		lua_pushinteger(m_state, (lua_Integer)i+1);
		lua_pushnumber(m_state, (lua_Number)values[i]);
		lua_rawset(m_state, -3);
	}
}

void MScript::pushBoolean(bool value){
	lua_pushboolean(m_state, (int)value);
}

void MScript::pushString(const char * string){
	lua_pushstring(m_state, string);
}

void MScript::pushInteger(int value){
	lua_pushinteger(m_state, (lua_Integer)value);
}

void MScript::pushFloat(float value){
	lua_pushnumber(m_state, (lua_Number)value);
}
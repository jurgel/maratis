/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MLevelSave.cpp
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


#include <MEngine.h>
#include "MLevelSave.h"

static char rep[256];


void openNode(FILE * file, const char * name, unsigned int tab)
{
	for(unsigned int i=0; i<tab; i++)
		fprintf(file, "\t");
	fprintf(file, "<%s>", name);
}

void closeNode(FILE * file, const char * name, unsigned int tab)
{
	for(unsigned int i=0; i<tab; i++)
		fprintf(file, "\t");
	fprintf(file, "</%s>", name);
}

void openAttributeNode(FILE * file, const char * name, unsigned int tab)
{
	for(unsigned int i=0; i<tab; i++)
		fprintf(file, "\t");
	fprintf(file, "<%s ", name);
}

void closeAttributeNode(FILE * file)
{
	fprintf(file, " />");
}

void writeBool(FILE * file, const char * name, bool variable)
{
	fprintf(file, "%s=\"", name);
	if(variable)
		fprintf(file, "true");
	else
		fprintf(file, "false");
	fprintf(file, "\"");
}

void writeString(FILE * file, const char * name, const char * string)
{
	fprintf(file, "%s=\"%s\"", name, string);
}

void writeInt(FILE * file, const char * name, int variable)
{
	fprintf(file, "%s=\"%d\"", name, variable);
}

void writeFloat(FILE * file, const char * name, float variable)
{
	fprintf(file, "%s=\"%f\"", name, variable);
}

void writeFloatValues(FILE * file, const char * name, float * vector, unsigned int size)
{
	fprintf(file, "%s=\"", name);
	for(unsigned int i=0; i<size; i++)
	{
		if(i>0) fprintf(file, " ");
		fprintf(file, "%f", *vector);
		vector++;
	}
	fprintf(file, "\"");
}

void writePhysics(FILE * file, MPhysicsProperties * physicsProperties)
{
	openAttributeNode(file, "physics", 3);
	fprintf(file, "\n");

	// shape
	fprintf(file, "\t\t\t\t");
	switch(physicsProperties->getCollisionShape())
	{
	case M_COLLISION_SHAPE_BOX:
		writeString(file, "shape", "Box");
		break;
	case M_COLLISION_SHAPE_SPHERE:
		writeString(file, "shape", "Sphere");
		break;
	case M_COLLISION_SHAPE_CONE:
		writeString(file, "shape", "Cone");
		break;
	case M_COLLISION_SHAPE_CAPSULE:
		writeString(file, "shape", "Capsule");
		break;
	case M_COLLISION_SHAPE_CYLINDER:
		writeString(file, "shape", "Cylinder");
		break;
	case M_COLLISION_SHAPE_CONVEX_HULL:
		writeString(file, "shape", "ConvexHull");
		break;
	case M_COLLISION_SHAPE_TRIANGLE_MESH:
		writeString(file, "shape", "TriangleMesh");
		break;
	}
	fprintf(file, "\n");

	// ghost
	fprintf(file, "\t\t\t\t");
	writeBool(file, "ghost", physicsProperties->isGhost());
	fprintf(file, "\n");

	// mass
	fprintf(file, "\t\t\t\t");
	writeFloat(file, "mass", physicsProperties->getMass());
	fprintf(file, "\n");

	// friction
	fprintf(file, "\t\t\t\t");
	writeFloat(file, "friction", physicsProperties->getFriction());
	fprintf(file, "\n");

	// restitution
	fprintf(file, "\t\t\t\t");
	writeFloat(file, "restitution", physicsProperties->getRestitution());
	fprintf(file, "\n");

	// linearDamping
	fprintf(file, "\t\t\t\t");
	writeFloat(file, "linearDamping", physicsProperties->getLinearDamping());
	fprintf(file, "\n");

	// angularDamping
	fprintf(file, "\t\t\t\t");
	writeFloat(file, "angularDamping", physicsProperties->getAngularDamping());
	fprintf(file, "\n");

	// angularFactor
	fprintf(file, "\t\t\t\t");
	writeFloat(file, "angularFactor", physicsProperties->getAngularFactor());
	fprintf(file, "\n");

	// linearFactor
	fprintf(file, "\t\t\t\t");
	writeFloatValues(file, "linearFactor", *physicsProperties->getLinearFactor(), 3);
	
	closeAttributeNode(file);
	fprintf(file, "\n");
	
	// constraint
	MPhysicsConstraint * constraint = physicsProperties->getConstraint();
	if(constraint)
	{
		openAttributeNode(file, "constraint", 3);
		fprintf(file, "\n");
		
		// parent
		fprintf(file, "\t\t\t\t");
		writeString(file, "parent", constraint->parentName.getData());
		fprintf(file, "\n");
		
		// pivot
		fprintf(file, "\t\t\t\t");
		writeFloatValues(file, "pivot", constraint->pivot, 3);
		fprintf(file, "\n");
		
		// lowerLinearLimit
		fprintf(file, "\t\t\t\t");
		writeFloatValues(file, "lowerLinearLimit", constraint->lowerLinearLimit, 3);
		fprintf(file, "\n");
		
		// upperLinearLimit
		fprintf(file, "\t\t\t\t");
		writeFloatValues(file, "upperLinearLimit", constraint->upperLinearLimit, 3);
		fprintf(file, "\n");
		
		// lowerAngularLimit
		fprintf(file, "\t\t\t\t");
		writeFloatValues(file, "lowerAngularLimit", constraint->lowerAngularLimit, 3);
		fprintf(file, "\n");
		
		// upperAngularLimit
		fprintf(file, "\t\t\t\t");
		writeFloatValues(file, "upperAngularLimit", constraint->upperAngularLimit, 3);
		fprintf(file, "\n");
		
		// disableParentCollision
		fprintf(file, "\t\t\t\t");
		writeBool(file, "disableParentCollision", constraint->disableParentCollision);
		
		closeAttributeNode(file);
		fprintf(file, "\n");
	}
}

void writeCameraProperties(FILE * file, MOCamera * camera)
{
	openAttributeNode(file, "properties", 3);
	fprintf(file, "\n");

	// clearColor
	fprintf(file, "\t\t\t\t");
	writeFloatValues(file, "clearColor", *camera->getClearColor(), 3);
	fprintf(file, "\n");

	// ortho
	fprintf(file, "\t\t\t\t");
	writeBool(file, "ortho", camera->isOrtho());
	fprintf(file, "\n");

	// fov
	fprintf(file, "\t\t\t\t");
	writeFloat(file, "fov", camera->getFov());
	fprintf(file, "\n");

	// clippingNear
	fprintf(file, "\t\t\t\t");
	writeFloat(file, "clippingNear", camera->getClippingNear());
	fprintf(file, "\n");

	// clippingFar
	fprintf(file, "\t\t\t\t");
	writeFloat(file, "clippingFar", camera->getClippingFar());
	fprintf(file, "\n");

	// fog
	fprintf(file, "\t\t\t\t");
	writeBool(file, "fog", camera->hasFog());
	fprintf(file, "\n");

	// fogDistance
	fprintf(file, "\t\t\t\t");
	writeFloat(file, "fogDistance", camera->getFogDistance());

	closeAttributeNode(file);
	fprintf(file, "\n");
}

void writeEntityProperties(FILE * file, MOEntity * entity)
{
	openAttributeNode(file, "properties", 3);
	fprintf(file, "\n");

	// invisible
	fprintf(file, "\t\t\t\t");
	writeBool(file, "invisible", entity->isInvisible());

	closeAttributeNode(file);
	fprintf(file, "\n");
}

void writeSoundProperties(FILE * file, MOSound * sound)
{
	openAttributeNode(file, "properties", 3);
	fprintf(file, "\n");

	// loop
	fprintf(file, "\t\t\t\t");
	writeBool(file, "loop", sound->isLooping());
	fprintf(file, "\n");

	// pitch
	fprintf(file, "\t\t\t\t");
	writeFloat(file, "pitch", sound->getPitch());
	fprintf(file, "\n");

	// gain
	fprintf(file, "\t\t\t\t");
	writeFloat(file, "gain", sound->getGain());
	fprintf(file, "\n");

	// radius
	fprintf(file, "\t\t\t\t");
	writeFloat(file, "radius", sound->getRadius());
	fprintf(file, "\n");

	// rolloff
	fprintf(file, "\t\t\t\t");
	writeFloat(file, "rolloff", sound->getRolloff());
	fprintf(file, "\n");
	
	// relative
	fprintf(file, "\t\t\t\t");
	writeBool(file, "relative", sound->isRelative());
	
	closeAttributeNode(file);
	fprintf(file, "\n");
}

void writeTextProperties(FILE * file, MOText * text)
{
	openAttributeNode(file, "properties", 3);
	fprintf(file, "\n");

	// size
	fprintf(file, "\t\t\t\t");
	writeFloat(file, "size", text->getSize());
	fprintf(file, "\n");

	// align
	fprintf(file, "\t\t\t\t");
	switch(text->getAlign())
	{
	case M_ALIGN_LEFT:
		writeString(file, "align", "Left");
		break;
	case M_ALIGN_RIGHT:
		writeString(file, "align", "Right");
		break;
	case M_ALIGN_CENTER:
		writeString(file, "align", "Center");
		break;
	}
	fprintf(file, "\n");

	// color
	fprintf(file, "\t\t\t\t");
	writeFloatValues(file, "color", *text->getColor(), 4);

	closeAttributeNode(file);
	fprintf(file, "\n");

	// text
	if(text->getText())
	{
		openNode(file, "textData", 3);
		fprintf(file, "\n");

		fprintf(file, "\t\t\t\t<![CDATA[");
		fprintf(file, "%s", text->getText());
		fprintf(file, "]]>");
		fprintf(file, "\n");

		closeNode(file, "textData", 3);
		fprintf(file, "\n");
	}
}

void writeSceneProperties(FILE * file, MScene * scene)
{
	openAttributeNode(file, "properties", 2);
	fprintf(file, "\n");

	// data mode
	M_DATA_MODES dataMode = scene->getDataMode();
	switch(dataMode)
	{
	case M_DATA_STATIC:
		fprintf(file, "\t\t\t");
		writeString(file, "data", "Static");
		fprintf(file, "\n");
		break;
	case M_DATA_DYNAMIC:
		fprintf(file, "\t\t\t");
		writeString(file, "data", "Dynamic");
		fprintf(file, "\n");
		break;
	case M_DATA_STREAM:
		fprintf(file, "\t\t\t");
		writeString(file, "data", "Stream");
		fprintf(file, "\n");
		break;
	}

	// gravity
	fprintf(file, "\t\t\t");
	writeFloatValues(file, "gravity", *scene->getGravity(), 3);

	closeAttributeNode(file);
	fprintf(file, "\n");
}

void writeLightProperties(FILE * file, MOLight * light)
{
	openAttributeNode(file, "properties", 3);
	fprintf(file, "\n");

	// radius
	fprintf(file, "\t\t\t\t");
	writeFloat(file, "radius", light->getRadius());
	fprintf(file, "\n");

	// color
	fprintf(file, "\t\t\t\t");
	writeFloatValues(file, "color", *light->getColor(), 3);
	fprintf(file, "\n");

	// intensity
	fprintf(file, "\t\t\t\t");
	writeFloat(file, "intensity", light->getIntensity());
	fprintf(file, "\n");

	// spotAngle
	fprintf(file, "\t\t\t\t");
	writeFloat(file, "spotAngle", light->getSpotAngle());
	fprintf(file, "\n");

	// spotExponent
	fprintf(file, "\t\t\t\t");
	writeFloat(file, "spotExponent", light->getSpotExponent());
	fprintf(file, "\n");
	
	// shadow
	fprintf(file, "\t\t\t\t");
	writeBool(file, "shadow", light->isCastingShadow());
	
	if(light->isCastingShadow())
	{
		fprintf(file, "\n");
		
		// shadow bias
		fprintf(file, "\t\t\t\t");
		writeFloat(file, "shadowBias", light->getShadowBias());
		fprintf(file, "\n");
	
		// shadow blur
		fprintf(file, "\t\t\t\t");
		writeFloat(file, "shadowBlur", light->getShadowBlur());
		fprintf(file, "\n");
	
		// shadow quality
		fprintf(file, "\t\t\t\t");
		writeInt(file, "shadowQuality", light->getShadowQuality());
	}
	
	closeAttributeNode(file);
	fprintf(file, "\n");
}

void writeObjectTransform(FILE * file, MObject3d * object)
{
	openAttributeNode(file, "active", 3);
	writeBool(file, "value", object->isActive());
	closeAttributeNode(file);
	fprintf(file, "\n");

	openAttributeNode(file, "transform", 3);
	fprintf(file, "\n");

	// parent
	MObject3d * parent = object->getParent();
	if(parent)
	{
		fprintf(file, "\t\t\t\t");
		writeString(file, "parent", parent->getName());
		fprintf(file, "\n");
	}

	// position
	fprintf(file, "\t\t\t\t");
	writeFloatValues(file, "position", object->getPosition(), 3);
	fprintf(file, "\n");

	// rotation
	fprintf(file, "\t\t\t\t");
	MVector3 rotation = object->getEulerRotation();
	writeFloatValues(file, "rotation", rotation, 3);
	fprintf(file, "\n");

	// scale
	fprintf(file, "\t\t\t\t");
	writeFloatValues(file, "scale", object->getScale(), 3);

	closeAttributeNode(file);
	fprintf(file, "\n");
}

void writeBehavior(FILE * file, MBehavior * behavior)
{
	openAttributeNode(file, "Behavior", 3);
	writeString(file, "name", behavior->getName());
	fprintf(file, ">\n");

	openAttributeNode(file, "properties", 4);
	fprintf(file, "\n");

	unsigned int i;
	unsigned int size = behavior->getVariablesNumber();
	for(i=0; i<size; i++)
	{
		MVariable variable = behavior->getVariable(i);

		M_VARIABLE_TYPE varType = variable.getType();
		if(varType == M_VARIABLE_NULL)
			continue;

		const char * name = variable.getName();

		fprintf(file, "\t\t\t\t\t");

		switch(variable.getType())
		{
		case M_VARIABLE_BOOL:
			writeBool(file, name, *((bool*)variable.getPointer()));
			break;
		case M_VARIABLE_INT:
		case M_VARIABLE_UINT:
			writeInt(file, name, *((int*)variable.getPointer()));
			break;
		case M_VARIABLE_FLOAT:
			writeFloat(file, name, *((float*)variable.getPointer()));
			break;
		case M_VARIABLE_STRING:
			writeString(file, name, ((MString*)variable.getPointer())->getData());
			break;
		case M_VARIABLE_VEC2:
			writeFloatValues(file, name, *((MVector2*)variable.getPointer()), 2);
			break;
		case M_VARIABLE_VEC3:
			writeFloatValues(file, name, *((MVector3*)variable.getPointer()), 3);
			break;
		case M_VARIABLE_VEC4:
			writeFloatValues(file, name, *((MVector4*)variable.getPointer()), 4);
			break;
		case M_VARIABLE_TEXTURE_REF:
			{
				MTextureRef * textureRef = *(MTextureRef **)variable.getPointer();
				if(textureRef)
				{
					char filename[256];
					getLocalFilename(filename, rep, textureRef->getFilename());
					writeString(file, name, filename);
				}
				
				break;
			}
		}

		if((i+1) < size)
			fprintf(file, "\n");
	}

	closeAttributeNode(file);
	fprintf(file, "\n");

	closeNode(file, "Behavior", 3);
	fprintf(file, "\n");
}

void writeBehaviors(FILE * file, MObject3d * object)
{
	unsigned int i;
	unsigned int size = object->getBehaviorsNumber();
	for(i=0; i<size; i++)
	{
		MBehavior * behavior = object->getBehavior(i);
		writeBehavior(file, behavior);
	}
}

bool xmlLevelSave(MLevel * level, const char * filename)
{
	FILE * file = fopen(filename, "wt");
	if(! file)
		return false;

	char localFile[256];

	// get rep
	getRepertory(rep, filename);

	openNode(file, "Maratis version=\"3.0\"", 0);
	fprintf(file, "\n\n");
	openNode(file, "Level", 0);
	fprintf(file, "\n\n");

	openAttributeNode(file, "properties", 1);
	writeInt(file, "currentScene", level->getCurrentSceneId());
	closeAttributeNode(file);
	fprintf(file, "\n\n");

	// scenes
	unsigned int i;
	unsigned int scnSize = level->getScenesNumber();
	for(i=0; i<scnSize; i++)
	{
		MScene * scene = level->getSceneByIndex(i);

		openAttributeNode(file, "Scene", 1);
		writeString(file, "name", scene->getName());
		fprintf(file, ">\n\n");
		
		// script
		if(scene->getScriptFilename())
		{
			if(strlen(scene->getScriptFilename()) > 0)
			{
				getLocalFilename(localFile, rep, scene->getScriptFilename());

				openAttributeNode(file, "script", 2);
				writeString(file, "file", localFile);
				closeAttributeNode(file);
				fprintf(file, "\n");
			}
		}

		// properties
		writeSceneProperties(file, scene);
		fprintf(file, "\n");

		// lights
		unsigned int l;
		unsigned int lSize = scene->getLightsNumber();
		for(l=0; l<lSize; l++)
		{
			MOLight * light = scene->getLightByIndex(l);

			openAttributeNode(file, "Light", 2);
			writeString(file, "name", light->getName());
			fprintf(file, ">\n");

			// transform
			writeObjectTransform(file, light);

			// properties
			writeLightProperties(file, light);

			// behaviors
			writeBehaviors(file, light);

			closeNode(file, "Light", 2);
			fprintf(file, "\n");
		}

		// cameras
		unsigned int c;
		unsigned int cSize = scene->getCamerasNumber();
		for(c=0; c<cSize; c++)
		{
			MOCamera * camera = scene->getCameraByIndex(c);

			openAttributeNode(file, "Camera", 2);
			writeString(file, "name", camera->getName());
			fprintf(file, ">\n");

			// transform
			writeObjectTransform(file, camera);

			// properties
			writeCameraProperties(file, camera);

			// behaviors
			writeBehaviors(file, camera);

			closeNode(file, "Camera", 2);
			fprintf(file, "\n");
		}

		// sound
		unsigned int s;
		unsigned int sSize = scene->getSoundsNumber();
		for(s=0; s<sSize; s++)
		{
			MOSound * sound = scene->getSoundByIndex(s);

			openAttributeNode(file, "Sound", 2);

			// name
			writeString(file, "name", sound->getName());

			// file
			const char * soundFile = NULL;

			// sound ref
			MSoundRef * ref = sound->getSoundRef();
			if(ref)
				soundFile = ref->getFilename();

			if(soundFile)
			{
				getLocalFilename(localFile, rep, soundFile);
				fprintf(file, " ");
				writeString(file, "file", localFile);
			}

			fprintf(file, ">\n");

			// transform
			writeObjectTransform(file, sound);

			// properties
			writeSoundProperties(file, sound);

			// behaviors
			writeBehaviors(file, sound);

			closeNode(file, "Sound", 2);
			fprintf(file, "\n");
		}

		// entities
		unsigned int e;
		unsigned int eSize = scene->getEntitiesNumber();
		for(e=0; e<eSize; e++)
		{
			MOEntity * entity = scene->getEntityByIndex(e);

			openAttributeNode(file, "Entity", 2);

			// name
			writeString(file, "name", entity->getName());

			// file
			MMeshRef * ref = entity->getMeshRef();
			if(ref)
			{
				const char * meshFile = ref->getFilename();
				if(meshFile)
				{
					getLocalFilename(localFile, rep, meshFile);
					fprintf(file, " ");
					writeString(file, "file", localFile);
				}
			}

			fprintf(file, ">\n");

			// bounding box
			openAttributeNode(file, "BoundingBox", 3);
			writeFloatValues(file, "min", *entity->getBoundingBox()->getMin(), 3);
			fprintf(file, " ");
			writeFloatValues(file, "max", *entity->getBoundingBox()->getMax(), 3);
			closeAttributeNode(file);
			fprintf(file, "\n");

			// anim
			openAttributeNode(file, "anim", 3);
			writeInt(file, "id", (int)entity->getAnimationId());
			closeAttributeNode(file);
			fprintf(file, "\n");

			// transform
			writeObjectTransform(file, entity);

			// properties
			writeEntityProperties(file, entity);

			// physics
			MPhysicsProperties * physicsProperties = entity->getPhysicsProperties();
			if(physicsProperties)
				writePhysics(file, physicsProperties);

			// behaviors
			writeBehaviors(file, entity);

			closeNode(file, "Entity", 2);
			fprintf(file, "\n");
		}

		// text
		unsigned int t;
		unsigned int tSize = scene->getTextsNumber();
		for(t=0; t<tSize; t++)
		{
			MOText * text = scene->getTextByIndex(t);

			openAttributeNode(file, "Text", 2);

			// name
			writeString(file, "name", text->getName());

			// font
			MFont * font = text->getFont();
			if(font)
			{
				MFontRef * ref = text->getFontRef();
				const char * fontFile = ref->getFilename();
				if(fontFile)
				{
					getLocalFilename(localFile, rep, fontFile);
					fprintf(file, " ");
					writeString(file, "file", localFile);
				}
			}

			fprintf(file, ">\n");

			// transform
			writeObjectTransform(file, text);

			// properties
			writeTextProperties(file, text);

			// behaviors
			writeBehaviors(file, text);

			closeNode(file, "Text", 2);
			fprintf(file, "\n");
		}

		fprintf(file, "\n");
		closeNode(file, "Scene", 1);
		fprintf(file, "\n");
	}


	fprintf(file, "\n");
	closeNode(file, "Level", 0);
	fprintf(file, "\n\n");
	closeNode(file, "Maratis", 0);

	fclose(file);
	return true;
}
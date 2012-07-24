/////////////////////////////////////////////////////////////////////////////////////////////////////////
// ManualUse example
// MyGame.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
//  Maratis, Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//========================================================================

// This game example is showing how to manually create a scene and load objects
// it can also be used in Maratis as game plugin (like WaterGameDemo)


#include "MyGame.h"


MyGame::MyGame(void):
MGame()
{}

MyGame::~MyGame(void)
{}

void MyGame::onBegin(void)
{
	MEngine * engine = MEngine::getInstance();
	MSystemContext * system = engine->getSystemContext();

	// get level
	MLevel * level = engine->getLevel();
	if(! level)
		return;

	// create a scene manually
	if(level->getScenesNumber() == 0)
	{
		char filename[256];
		const char * workingDir = system->getWorkingDirectory();

		// add scene
		MScene * scene = level->addNewScene();

		// camera
		MOCamera * camera = scene->addNewCamera();
		camera->setClearColor(MVector3(0.5f, 0.5f, 0.5f)); // set grey clear color
		camera->setPosition(MVector3(0.0f, -80.0f, 20.0f));
		camera->setEulerRotation(MVector3(90.0f, 0.0f, 0.0f));

		// add entity
		getGlobalFilename(filename, workingDir, "meshs/robot.mesh");
		MMeshRef * meshRef = level->loadMesh(filename);
		scene->addNewEntity(meshRef);

		// add light
		MOLight * light = scene->addNewLight();
		light->setPosition(MVector3(0.0f, 0.0f, 100.0f));
		light->setRadius(1000.0f);
	}
}

void MyGame::update(void)
{
	MEngine * engine = MEngine::getInstance();
	MInputContext * input = engine->getInputContext();

	MGame::update(); // call standard update, or replace it with cutom code

	// get level
	MLevel * level = engine->getLevel();
	if(! level)
		return;

	// get current scene
	MScene * scene = level->getCurrentScene();
	if(! scene)
		return;

	// rotate the entity
	MOEntity * entity = scene->getEntityByIndex(0);
	entity->addAxisAngleRotation(MVector3(0.0f, 0.0f, 1.0f), 1.0f);

	// change light intensity with keyboard
	MOLight * light = scene->getLightByIndex(0);

	if(input->isKeyPressed("UP")){ // inputs are also virtual, you can create your hown keys or axis
		light->setIntensity(light->getIntensity() + 0.1f);
	}

	if(input->isKeyPressed("DOWN")){
		light->setIntensity(MAX(0.0f, light->getIntensity() - 0.1f));
	}
}

/*
// custom draw, if not defined, draw is MGame::draw()
void MyGame::draw(void)
{
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();

	// get level
	MLevel * level = engine->getLevel();
	if(! level)
		return;

	// get current scene
	MScene * scene = level->getCurrentScene();
	if(! scene)
		return;

	render->enableDepthTest();

	MOCamera * camera = scene->getCameraByIndex(0);
	
	render->setClearColor(*camera->getClearColor());
	render->clear(M_BUFFER_COLOR | M_BUFFER_DEPTH);

	camera->enable();
	camera->updateListener();
	scene->draw(camera);
}*/
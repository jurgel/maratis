/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MGame.cpp
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


#include "../Includes/MEngine.h"


void MGame::update(void)
{
	MEngine * engine = MEngine::getInstance();
	MScriptContext * scriptContext = engine->getScriptContext();

	// update script
	if(scriptContext)
		scriptContext->callFunction("onSceneUpdate");

	// get level
	MLevel * level = MEngine::getInstance()->getLevel();
	if(! level)
		return;

	// get current scene
	MScene * scene = level->getCurrentScene();
	if(! scene)
		return;
		
	// get HUD
	MScene * hud = level->getHUD();

	// update behaviors
	scene->updateObjectsBehaviors();
	//hud->updateObjectsBehaviors();

	// update scene
	scene->update();
	if(hasHUD)
	    hud->update();

	// update physics
	scene->updatePhysics();
	//hud->updatePhysics();

	// update objects matrices
	scene->updateObjectsMatrices();
	if(hasHUD)
	    hud->updateObjectsMatrices();

	// flush input
	engine->getInputContext()->flush();

	// update postponed requests
	engine->updateRequests();
}

void MGame::draw(void)
{
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();

	// get level
	MLevel * level = MEngine::getInstance()->getLevel();
	if(! level)
		return;

	// get current scene
	MScene * scene = level->getCurrentScene();
	if(! scene)
		return;
		
	// get HUD
	MScene * hud = level->getHUD();

	// clear buffer
	render->enableDepthTest();

	if(scene->getCamerasNumber() == 0)
	{
		// draw scene with default camera
		MOCamera camera;
		
		render->setClearColor(*camera.getClearColor());
		render->clear(M_BUFFER_COLOR | M_BUFFER_DEPTH);

		camera.enable();
		camera.updateListener();
		scene->draw(&camera);
	}
	else
	{
		MOCamera * camera = NULL;
		unsigned int currentCamera = scene->getCurrentCamera();
		if(currentCamera < scene->getCamerasNumber())
			camera = scene->getCameraByIndex(currentCamera);
		else
			camera = scene->getCameraByIndex(0);
		
		render->setClearColor(*camera->getClearColor());
		render->clear(M_BUFFER_COLOR | M_BUFFER_DEPTH);

		camera->enable();
		camera->updateListener();
		scene->draw(camera);
	}
	
	// draw behaviors
	scene->drawObjectsBehaviors();
	
	if(hasHUD)
	{
	    if(hud->getCamerasNumber() == 0)
	    {
		    // draw hud with default camera
		    MOCamera camera;
		
		    render->clear(M_BUFFER_DEPTH);

		    camera.enable();
		    hud->draw(&camera);
	    }
	    else
	    {
		    MOCamera * camera = NULL;
		
		    camera = hud->getCameraByIndex(0); // first camera
		
		    render->clear(M_BUFFER_DEPTH);

		    camera->enable();
		    hud->draw(camera);
	    }

		// draw behaviors
		hud->drawObjectsBehaviors();
	}
}

void MGame::onBeginScene(void)
{
	// get level
	MLevel * level = MEngine::getInstance()->getLevel();
	if(! level)
		return;

	// get current scene
	MScene * scene = level->getCurrentScene();
	if(! scene)
		return;
		
	// set HUD
	hasHUD = false;
	MScene * hud = level->getSceneByName("HUD Layer");
	if(hud != NULL)
	{
	    hasHUD = true;
	    level->setHUD(hud);
    }

	// begin scene
	scene->begin();
	//hud->begin();
}

void MGame::onEndScene(void)
{
	MEngine * engine = MEngine::getInstance();
	engine->getPhysicsContext()->clear();

	// get level
	MLevel * level = MEngine::getInstance()->getLevel();
	if(! level)
		return;

	// get current scene
	MScene * scene = level->getCurrentScene();
	if(! scene)
		return;
		
	// get HUD
	//MScene * hud = level->getHUD();
	//if(! hud)
	//    return;

	// end scene
	scene->end();
	//hud->end();
}
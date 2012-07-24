/////////////////////////////////////////////////////////////////////////////////////////////////////////
// ManualUse example
// main.cpp
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

// This example is showing how to use Maratis-Engine manually
// code is basically the same as MaratisPlayer, without Plugin support


#include <MEngine.h>
#include <MWindow.h>

#include <MGameWinEvents.h>

//MCore_Implements
#include <MContexts/MGLContext.h>
#include <MContexts/MALContext.h>
#include <MContexts/MBulletContext.h>
#include <MContexts/MWinContext.h>
#include <MLoaders/MDevILLoader.h>
#include <MLoaders/MSndFileLoader.h>
#include <MLoaders/MFreetypeLoader.h>
#include <MLoaders/MBinFontLoader.h>

// MaratisCore
#include <MFileManager/MLevelLoad.h>
#include <MBehaviors/MBLookAt.h>
#include <MBehaviors/MBFollow.h>
#include <MScript/MScript.h>
#include <MInput/MInput.h>
#include <MFileManager/MMeshLoad.h>
#include <MFileManager/MLevelLoad.h>
#include <MProject/MProject.h>
#include <MRenderers/MStandardRenderer.h>
#include <MRenderers/MFixedRenderer.h>

// My Game
#include "MyGame.h"


// window events
void windowEvents(MWinEvent * windowEvents)
{
	MEngine * engine = MEngine::getInstance();

	// game
	MGame * game = engine->getGame();
	if(game)
	{
		if(game->isRunning()){
			gameWinEvents(windowEvents);
		}
	}

	if(windowEvents->type == MWIN_EVENT_KEY_DOWN && windowEvents->data[0] == MKEY_ESCAPE){
		MWindow::getInstance()->setActive(false);
	}

	if(windowEvents->type == MWIN_EVENT_WINDOW_CLOSE){
		MWindow::getInstance()->setActive(false);
	}
}

void update(void)
{
	MEngine * engine = MEngine::getInstance();
	MGame * game = engine->getGame();

	if(game)
	{
		if(game->isRunning())
		{
			game->update();
		}
	}
}

void draw(void)
{
	MWindow * window = MWindow::getInstance();
	MEngine * engine = MEngine::getInstance();
	MRenderingContext * render = engine->getRenderingContext();
	MGame * game = engine->getGame();

	// set basic viewport
	render->disableScissorTest();
	render->setViewport(0, 0, window->getWidth(), window->getHeight());

	if(game)
	{
		if(game->isRunning())
		{
			game->draw();
		}
	}

	window->swapBuffer();
}

// main
int main(int argc, char **argv)
{
	setlocale(LC_NUMERIC, "C");

	unsigned int width = 1024;
	unsigned int height = 768;
	bool fullscreen = false;

	// get engine
	MEngine * engine = MEngine::getInstance();

	// get window
	MWindow * window = MWindow::getInstance();
	window->setPointerEvent(windowEvents); // window events

	// create window
	window->create("Maratis - ManualUse example", width, height, 32, fullscreen);
	if(fullscreen)
		window->hideCursor();

	// set current directory
	{
		char rep[256];
		getRepertory(rep, argv[0]);
		window->setCurrentDirectory(rep);
	}

	// create virtual contexts
	MSoundContext * soundContext = new MALContext();
	MRenderingContext * render = new MGLContext();
	MPhysicsContext * physics = new MBulletContext();
	MScriptContext * script = new MScript();
	MInputContext *	input = new MInput();
	MSystemContext * system = new MWinContext();
	
	// create default Level and Game
	MLevel * level = new MLevel();
	MGame * game = new MyGame(); // MyGame

	// init MEngine (you can replace all contexts by others and add or use different data loaders)
	engine->setSoundContext(soundContext); // sound context
	engine->setRenderingContext(render); // rendering context
	engine->setPhysicsContext(physics); // physics context
	engine->setScriptContext(script); // script context
	engine->setInputContext(input); // input context
	engine->setSystemContext(system); // system context

	engine->getImageLoader()->addLoader(M_loadImage); // image loader
	engine->getSoundLoader()->addLoader(M_loadSound); // sound loader
	// engine->getLevelLoader()->addLoader(xmlLevelLoad); // level loader : uncomment if wanted
	// engine->getFontLoader()->addLoader(M_loadFont); // font loader : uncomment if wanted
	// engine->getFontLoader()->addLoader(M_loadBinFont); // bin font loader : uncomment if wanted

	// add some default "Maratis" behaviors : uncomment if wanted or add custom
	// engine->getBehaviorManager()->addBehavior(MBLookAt::getStaticName(), M_OBJECT3D_CAMERA, MBLookAt::getNew);
	// engine->getBehaviorManager()->addBehavior(MBFollow::getStaticName(), M_OBJECT3D, MBFollow::getNew);

	// add renderers
	engine->getRendererManager()->addRenderer(MStandardRenderer::getStaticName(), MStandardRenderer::getNew);
	engine->getRendererManager()->addRenderer(MFixedRenderer::getStaticName(), MFixedRenderer::getNew);
	
	// mesh loader
	engine->getMeshLoader()->addLoader(xmlMeshLoad);
	engine->getArmatureAnimLoader()->addLoader(xmlArmatureAnimLoad);
	engine->getTexturesAnimLoader()->addLoader(xmlTextureAnimLoad);
	engine->getMaterialsAnimLoader()->addLoader(xmlMaterialAnimLoad);

	// set level
	engine->setLevel(level);

	// set game
	engine->setGame(game);

	// set renderer (standard)
	MRenderer * renderer = engine->getRendererManager()->getRendererByName("StandardRenderer")->getNewRenderer();
	engine->setRenderer(renderer);

	// begin game
	game->begin();

	// time
	unsigned int frequency = 60;
	unsigned long previousFrame = 0;
	unsigned long startTick = window->getSystemTick();
	
	// on events
	while(window->isActive())
	{
		// on events
		if(window->onEvents())
		{
			// compute target tick
			unsigned long currentTick = window->getSystemTick();

			unsigned long tick = currentTick - startTick;
			unsigned long frame = (unsigned long)(tick * (frequency * 0.001f));

			// update elapsed time
			unsigned int i;
			unsigned int steps = (unsigned int)(frame - previousFrame);

			if(window->getFocus())
			{
				// don't wait too much
				if(steps >= (frequency/2))
				{
					update();
					draw();
					previousFrame += steps;
					continue;
				}
				
				// update
				for(i=0; i<steps; i++)
				{
					update();
					previousFrame++;
				}

				// draw
				if(steps > 0){
					draw();
				}
			}
			else
			{
				previousFrame = frame;
				window->swapBuffer();
			}
		}
	}

	game->end();
	renderer->destroy();

	SAFE_DELETE(game);
	SAFE_DELETE(level);

	SAFE_DELETE(render);
	SAFE_DELETE(soundContext);
	SAFE_DELETE(physics);
	SAFE_DELETE(script);
	SAFE_DELETE(input);
	SAFE_DELETE(system);

	return 0;
}
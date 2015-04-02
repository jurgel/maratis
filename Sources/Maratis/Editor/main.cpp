/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// main.cpp
//
// GUI test
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2013 Anael Seghezzi <www.maratis3d.com>
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


// MSDK
#include <MEngine.h>
#include <MGui.h>

// Common
#include <GUI/MGUI.h>
#include <Contexts/GL/MGLContext.h>
#include <Loaders/FreeImage/MFreeImageLoader.h>
#include <Loaders/Freetype/MFreetypeLoader.h>
#include <Renderers/Fixed/MFixedRenderer.h>
#include <Renderers/Standard/MStandardRenderer.h>
#include <Loaders/Mesh/MMeshLoad.h>

#include "MEditor.h"


// test data
static MRenderingContext * render = NULL;
static MRenderer * renderer = NULL;
static MSystemContext * systemContext = NULL;
static MLevel * level = NULL;

static MViewport * my3dView = NULL;


void winEvents2(MWindow * rootWindow, MWIN_EVENT_TYPE event)
{
	MEngine * engine = MEngine::getInstance();
	
	switch(event)
	{
	case MWIN_EVENT_CREATE:
		{
			MGuiWindow * gw = rootWindow->addNewWindow();
			gw->setScale(MVector2(rootWindow->getWidth(), rootWindow->getHeight()));
			gw->setColor(MVector3(1));
			
			MTextureRef * texRef = MTextureRef::getNew(0, "RAY", 0);
			MDataManager * texMan = engine->getLevel()->getTextureManager();
			texMan->addRef(texRef);
			
			MGuiImage * image = gw->addNewImage();
			image->setScale(gw->getScale());
			image->setNormalTexture(texRef);
		}
		break;
		
	case MWIN_EVENT_CLOSE:
		{
			MGUI_closeWindow(rootWindow);
		}
		break;
		
	default:
		break;
	}
}

void winEvents(MWindow * rootWindow, MWIN_EVENT_TYPE event)
{
	MEngine * engine = MEngine::getInstance();
	MEditor * editor = MEditor::getInstance();
	
	
	switch(event)
	{
	case MWIN_EVENT_CREATE:
		{
			// add loaders
			engine->getImageLoader()->addLoader(M_loadImage);
			engine->getFontLoader()->addLoader(M_loadFont);
			engine->getMeshLoader()->addLoader(xmlMeshLoad);
			engine->getArmatureAnimLoader()->addLoader(xmlArmatureAnimLoad);
			engine->getTexturesAnimLoader()->addLoader(xmlTextureAnimLoad);
			engine->getMaterialsAnimLoader()->addLoader(xmlMaterialAnimLoad);
			
			// system
			systemContext = new MGUIContext();
			engine->setSystemContext(systemContext);
			
			// rendering context
			render = new MGLContext();
			engine->setRenderingContext(render);
			
			// renderer
			renderer = new MStandardRenderer();
			engine->setRenderer(renderer);
			
			// engine level
			level = new MLevel();
			engine->setLevel(level);
			
			// test scene
			{
				char filename[256];
				const char * workingDir = systemContext->getWorkingDirectory();
		
				MScene * scene = level->addNewScene();
				getGlobalFilename(filename, workingDir, "Resources/meshes/default/Jules.mesh");
				MOEntity * entity = scene->addNewEntity(level->loadMesh(filename));
				entity->setPosition(MVector3(0, 0, 1));
				entity->setEulerRotation(MVector3(0, 0, 10));
				entity->setAnimationSpeed(0);
				
				MOCamera * camera = scene->addNewCamera();
				camera->setPosition(MVector3(200, 100, 200));
				camera->enableOrtho(true);
				
				MOSound * sound = scene->addNewSound(NULL);
				sound->setPosition(MVector3(100, 0, 0));
				
				getGlobalFilename(filename, workingDir, "Resources/meshes/default/box.mesh");
				MOEntity * entity2 = scene->addNewEntity(level->loadMesh(filename));
				entity2->setScale(MVector3(100, 100, 1));
				MMaterial * material = entity2->createLocalMaterial(0);
				material->setDiffuse(MVector3(0.8f));
				
				MOLight * light = scene->addNewLight();
				light->setPosition(MVector3(150, 0, 100));
				light->setColor(MVector3(1, 1, 1));
				light->setRadius(500);
				light->setLightType(M_LIGHT_DIRECTIONAL);
				light->addAxisAngleRotation(MVector3(1, 0, 0), 30);
				//light->castShadow(true);
				light->setShadowBlur(0.05f);
				light->setShadowQuality(1024);
				light->setShadowBias(10);
				
				
				scene->update();
				scene->updateObjectsMatrices();
			}
			
			// editor init
			editor->init();
			
			my3dView = new MV3dEdit();
			my3dView->create(rootWindow);
			my3dView->resize(MVector2(0, 0), MVector2(rootWindow->getWidth(), rootWindow->getHeight()));
		}
		break;
		
	case MWIN_EVENT_CLOSE:
		{
			MGUI_closeWindow(rootWindow);
		}
		break;
		
	case MWIN_EVENT_DESTROY:
		{
			SAFE_DELETE(my3dView);
			
			editor->clear();
			SAFE_DELETE(renderer);
			SAFE_DELETE(render);
			SAFE_DELETE(systemContext);
		}
		break;
		
	default:
		break;
	}
	
	// my3dView
	if(my3dView)
		my3dView->onEvent(rootWindow, event);
}


void drawCallback(MWindow * rootWindow)
{
	level->getCurrentScene()->update();
	level->getCurrentScene()->updateObjectsMatrices();
}



int main(int argc, char **argv)
{
	setlocale(LC_NUMERIC, "C");
	
	char dir[256];
	getDirectory(dir, argv[0]);
	MGUI_setCurrentDirectory(dir);
		
	// init
	if(! MGUI_init())
		return EXIT_FAILURE;
	
	M_initFreeImage();
	
	// create main window
	MWindow * window = MGUI_createWindow("test1", 10, 200, 800, 600, winEvents);
	if(! window)
	{
		MGUI_close();
		M_closeFreeImage();
		return EXIT_FAILURE;
	}

	window->setDrawCallback(drawCallback);
	
	//MGUI_createWindow("test2", 850, 200, 64*8, 45*8, winEvents2);
	
	// update
	while(1)
	{
		if(! MGUI_update())
			break;
	}
	
	// close
	MGUI_close();
	M_closeFreeImage();
	return EXIT_SUCCESS;
}

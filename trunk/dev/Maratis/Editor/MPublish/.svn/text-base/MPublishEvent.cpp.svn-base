/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MPublishEvent.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2012 Philipp Geyer <nistur.com>
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

#include <MEngine.h>
#include <MGui.h>
#include <MProject/MProject.h>

#include "MPublisher.h"
#include "MPublishEvent.h"

#include <sys/stat.h>

#ifdef linux
	#include <sys/types.h>
#endif


// some helper functions for repetitive packaging work
static std::string s_pubDir;

const char* getPubDir()
{
	if(s_pubDir.empty())
	{
		MEngine* engine = MEngine::getInstance();
		MSystemContext* system = engine->getSystemContext();

		char dir[256];
		getGlobalFilename(dir, system->getWorkingDirectory(), "published");
		s_pubDir = dir;
	}

	return s_pubDir.c_str();
}

MPackage openProjectPackage(const char* projName)
{
	MEngine* engine = MEngine::getInstance();
	MSystemContext* system = engine->getSystemContext();

	char projFile[256];
	getLocalFilename(projFile, system->getWorkingDirectory(), projName);
	char* ext = strstr(projFile, ".mproj");
	if(ext == 0)
	{
		return 0;
	}
		
	sprintf(ext, ".npk");
	createDirectory(getPubDir());
	char packFile[256];
	getGlobalFilename(packFile, getPubDir(), projFile);

	MPackageManager* packageManager = engine->getPackageManager();
	MPackage package = packageManager->openPackage(packFile);

	return package;
}

/* MPublishEventClearDirectory
 * Will clear the directory before publishing to it.
 */
class MPublishEventClearDirectory : public MPublishEvent
{
	void	execute(const char* projName)
	{
		clearDirectory(getPubDir());
	}

	int		getPriority() { return 0; }
};

M_PUBLISH_EVENT_IMPLEMENT(MPublishEventClearDirectory);

// Simple publishing. Will just package the whole directory
#define M_PUBLISH_PACKAGE_DIR(dir) \
class MPublishEvent##dir##Package : public MPublishEvent \
{ \
	void	execute(const char* projName) \
	{ \
		MEngine* engine = MEngine::getInstance(); \
		MSystemContext* system = engine->getSystemContext(); \
		char directory[256]; \
		getGlobalFilename(directory, system->getWorkingDirectory(), #dir); \
		std::vector<std::string> files; \
		readDirectory(directory, &files, 1, 1); \
		MPackage package = openProjectPackage(projName); \
		MPackageManager* packageManager = engine->getPackageManager();\
		for(int i = 0; i < files.size(); ++i) \
		{ \
			packageManager->addFileToPackage(files[i].c_str(), package); \
		} \
		packageManager->closePackage(package); \
	} \
	int		getPriority() { return 5; } \
}; \
M_PUBLISH_EVENT_IMPLEMENT(MPublishEvent##dir##Package);

#define M_PUBLISH_DIR(dir) \
class MPublishEvent##dir : public MPublishEvent \
{ \
	void	execute(const char* projName) \
	{ \
		MEngine* engine = MEngine::getInstance(); \
		MSystemContext* system = engine->getSystemContext(); \
		char directory[256]; \
		getGlobalFilename(directory, system->getWorkingDirectory(), #dir); \
		char destDirectory[256]; \
		getGlobalFilename(destDirectory, getPubDir(), #dir); \
		copyDirectory(directory, destDirectory); \
	} \
	int		getPriority() { return 5; } \
}; \
M_PUBLISH_EVENT_IMPLEMENT(MPublishEvent##dir); \

M_PUBLISH_PACKAGE_DIR(maps)
M_PUBLISH_PACKAGE_DIR(scripts)
M_PUBLISH_PACKAGE_DIR(sounds)
M_PUBLISH_PACKAGE_DIR(shaders)
M_PUBLISH_PACKAGE_DIR(fonts)
M_PUBLISH_DIR(meshs)
M_PUBLISH_DIR(levels)
M_PUBLISH_DIR(plugins)


static void embedProject(const char * src, const char * dest, const char * game, const char * level, const char * renderer)
{
	FILE* fp = 0;
	fp = fopen(src, "rb");
	if(! fp)
		return;
	
	fseek(fp, 0, SEEK_END);
	size_t size = ftell(fp);
	rewind(fp);
	char* buff = new char[size];
	if(size != fread(buff, sizeof(char), size, fp))
	{
		fclose(fp);
		delete [] buff;
		return;
	}
	fclose(fp);
	
	for(char* pChar = buff; pChar != buff+size; ++pChar)
	{
		if(*pChar == '[')
		{
			if(strcmp(pChar, s_embedded_game_name) == 0)
			{
				strcpy(pChar, game);
			}
			else if(strcmp(pChar, s_embedded_level_name) == 0)
			{
				strcpy(pChar, level);
			}
			else if(strcmp(pChar, s_embedded_renderer) == 0)
			{
				strcpy(pChar, renderer);
			}
		}
	}
	
	fp = fopen(dest, "wb");
	fwrite(buff, sizeof(char), size, fp);
	fclose(fp);
}

static void copyDirFiles(const char * src, const char * dest, const char * filter)
{
	std::vector<std::string> files;
	readDirectory(src, &files);
	for(int i = 0; i < files.size(); ++i)
	{
		if(strstr(files[i].c_str(), filter))
		{
			char filename[256];
			getGlobalFilename(filename, dest, files[i].c_str());
			copyFile(files[i].c_str(), filename);
		}
	}
}


#ifdef WIN32
void copySysWindows(const char* projName)
{
#ifdef _DEBUG
	const char * appName = "MaratisPlayerDebug.exe";
#else
	const char * appName = "MaratisPlayer.exe";
#endif
	
	MEngine* engine = MEngine::getInstance();
	MSystemContext* system = engine->getSystemContext();

	char filename[256];
	getLocalFilename(filename, system->getWorkingDirectory(), projName);

	if(char* ext = strstr(filename, ".mproj"))
	{
		MProject proj;
		if(proj.loadXML(projName))
		{
			strcpy(ext, ".exe");
			char destName[256];
			getGlobalFilename(destName, getPubDir(), filename);
			
			ext = 0;
			embedProject(appName, destName, filename, proj.startLevel.c_str(), proj.renderer.c_str());
		
			// find all dynamic libraries
			copyDirFiles(".", getPubDir(), ".dll");
		}
	}
}
#endif


#ifdef __APPLE__
void copySysOSX(const char* projName)
{
#ifdef _DEBUG
	const char * appName = "MaratisPlayerDebug";
#else
	const char * appName = "MaratisPlayer";
#endif
	
	MWindow * window = MWindow::getInstance();
	MEngine* engine = MEngine::getInstance();
	MSystemContext* system = engine->getSystemContext();

	char filename[256];
	getLocalFilename(filename, system->getWorkingDirectory(), projName);

	if(char* ext = strstr(filename, ".mproj"))
	{
		MProject proj;
		if(proj.loadXML(projName))
		{
			strcpy(ext, ".app");
			
			char path[256];
			char srcName[256];
			char destName[256];
			
			getGlobalFilename(destName, getPubDir(), filename);
			
			sprintf(path, "../../../%s.app", appName);
			getGlobalFilename(srcName, window->getCurrentDirectory(), path);
			copyDirectory(srcName, destName);
			
			strcpy(ext, "");
			sprintf(srcName, "%s/Contents/MacOS/%s", destName, appName);
			sprintf(destName, "%s/Contents/MacOS/%s", destName, filename);
			rename(srcName, destName);
			
			embedProject(destName, destName, filename, proj.startLevel.c_str(), proj.renderer.c_str());
			chmod(destName, 0777);
			
			// find all dynamic libraries
			//copyDirFiles(".", getPubDir(), ".dylib"); // in fact there are already in the app
			
			// we could put all data (and Game.dylib) in app/Contents/Resources/
		}
	}
}
#endif


#ifdef linux
void copySysLinux(const char* projName)
{
#ifdef _DEBUG
	const char * appName = "MaratisPlayerDebug";
#else
	const char * appName = "MaratisPlayer";
#endif
	
	MEngine* engine = MEngine::getInstance();
	MSystemContext* system = engine->getSystemContext();

	char filename[256];
	getLocalFilename(filename, system->getWorkingDirectory(), projName);

	if(char* ext = strstr(filename, ".mproj"))
	{
		*ext = 0;
		
		MProject proj;
		if(proj.loadXML(projName))
		{
			char destName[256];
			getGlobalFilename(destName, getPubDir(), filename);
			
			embedProject(appName, destName, filename, proj.startLevel.c_str(), proj.renderer.c_str());
			chmod(destName, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
			
			// find all dynamic libraries
			copyDirFiles(".", getPubDir(), ".so");
		}
	}
}
#endif


// copy Maratis system files, Player executable and DLLs
class MPublishEventCopySys : public MPublishEvent
{
	void	execute(const char* projName)
	{
		
		// for now these are #define'd out. In reality the engine should ship with 
		// players for all OS's and then you can choose which OS(/device) you want
		// to publish to
#ifdef WIN32
		copySysWindows(projName);
#elif __APPLE__
		copySysOSX(projName);
#elif linux
		copySysLinux(projName);
#endif
	}

	int		getPriority() { return 7; }
};
M_PUBLISH_EVENT_IMPLEMENT(MPublishEventCopySys);

// copy Game plugin, if one exists
class MPublishEventCopyGame : public MPublishEvent
{
	void	execute(const char* projName)
	{
		MEngine* engine = MEngine::getInstance();
		MSystemContext* system = engine->getSystemContext();

		char filename[256];
		char destFilename[256];

#ifdef WIN32
		const char* pluginName = "Game.dll";
#elif __APPLE__
		const char* pluginName = "Game.dylib";
#elif linux
		const char* pluginName = "Game.so";
#endif

		getGlobalFilename(filename, system->getWorkingDirectory(), pluginName);
		getGlobalFilename(destFilename, getPubDir(), pluginName);

		if(isFileExist(filename))
		{
			copyFile(filename, destFilename);
		}
	}

	int		getPriority() { return 7; }
};
M_PUBLISH_EVENT_IMPLEMENT(MPublishEventCopyGame);

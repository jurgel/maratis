/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// FileTools.cpp
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


#include "../Includes/MCore.h"

#include <dirent.h>

#ifdef WIN32
	#include <direct.h>
	#define mkdir _mkdir
	#define rmdir _rmdir
#elif __APPLE__
	#include <sys/stat.h>
	#define mkdir(file) mkdir(file, 0777)
#endif


bool copyFile(const char * inFilename, const char * outFilename)
{
	FILE * in = fopen(inFilename, "rb");
	if(! in)
	{
		printf("unable to read %s file\n", inFilename);
		return false;
	}

	FILE * out = fopen(outFilename, "wb");
	if(! out)
	{
		printf("unable to create %s file\n", outFilename);
		fclose(in);
		return false;
	}

	size_t n;
    char buffer[BUFSIZ];

    while((n = fread(buffer, sizeof(char), sizeof(buffer), in)) > 0)
    {
        if(fwrite(buffer, sizeof(char), n, out) != n)
            printf("write failed\n");
    }

	fclose(in);
	fclose(out);
	return true;
}

bool createDirectory(const char * filename)
{
	if(mkdir(filename) != -1)
		return true;
	else
		return false;
}

bool isFileExist(const char * filename)
{
	FILE * file = fopen(filename, "r");
	if(! file)
		return false;
	fclose(file);
	return true;
}

bool isDirectory(const char * filename)
{
    DIR * pdir = opendir(filename);
	if(! pdir)
		return false;
    closedir(pdir);
	return true;
}

bool isEmptyDirectory(const char * filename)
{
    DIR * pdir = opendir(filename);
	if(! pdir)
		return true;

	dirent * pent = NULL;
    while(pent = readdir(pdir))
	{
		if(strcmp(pent->d_name, ".") == 0)
			continue;

		if(strcmp(pent->d_name, "..") == 0)
			continue;

		return false;
	}

    closedir(pdir);
	return true;
}

bool clearDirectory(const char * filename)
{
    DIR * pdir = opendir(filename);
	if(! pdir)
		return false;

	dirent * pent = NULL;

    while(pent = readdir(pdir))
	{
		if(strcmp(pent->d_name, ".") == 0)
			continue;

		if(strcmp(pent->d_name, "..") == 0)
			continue;

		char file[256];
		getGlobalFilename(file, filename, pent->d_name);

		if(! isDirectory(file))
			remove(file);
    }

    closedir(pdir);
    return true;
}

bool removeDirectory(const char * filename)
{
    DIR * pdir = opendir(filename);
	if(! pdir)
		return false;

	dirent * pent = NULL;

    while(pent = readdir(pdir))
	{
		if(strcmp(pent->d_name, ".") == 0)
			continue;

		if(strcmp(pent->d_name, "..") == 0)
			continue;

		char file[256];
		getGlobalFilename(file, filename, pent->d_name);

		if(isDirectory(file))
			removeDirectory(file);
		else
			remove(file);
    }

    closedir(pdir);
	rmdir(filename);
    return true;
}

bool copyDirectory(const char * inFilename, const char * outFilename)
{
    DIR * pdir = opendir(inFilename);
	if(! pdir)
		return false;

	mkdir(outFilename);
	dirent * pent = NULL;

    while(pent = readdir(pdir))
	{
		if(strcmp(pent->d_name, ".") == 0)
			continue;

		if(strcmp(pent->d_name, "..") == 0)
			continue;

		char fileIn[256];
		char fileOut[256];
		getGlobalFilename(fileIn, inFilename, pent->d_name);
		getGlobalFilename(fileOut, outFilename, pent->d_name);

		if(isDirectory(fileIn))
			copyDirectory(fileIn, fileOut);
		else
			copyFile(fileIn, fileOut);
    }

    closedir(pdir);
    return true;
}

bool readDirectory(const char * filename, vector<string> * files)
{
	DIR * pdir = opendir(filename);
	if(! pdir)
		return false;

	dirent * pent = NULL;
    while(pent = readdir(pdir))
	{
		if(strcmp(pent->d_name, ".") == 0)
			continue;

		if(strcmp(pent->d_name, "..") == 0)
			continue;

		char file[256];
		getGlobalFilename(file, filename, pent->d_name);
		files->push_back(string(file));
    }

    closedir(pdir);
    return true;
}
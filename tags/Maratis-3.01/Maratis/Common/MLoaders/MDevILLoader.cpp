/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MDevILLoader.cpp
//
// Devil image loader
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


#include <IL/il.h>
#include <IL/ilu.h>

#include <MCore.h>
#include "MDevILLoader.h"


void DevILInit(void)
{
	ilInit();
	ilEnable(IL_CONV_PAL);
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
}

void DevILShutDown(void)
{
	ilShutDown();
}

bool M_loadImage(const char * filename, void * data)
{
	DevILInit();

	// gen image
	ILuint ImgId = 0;
	ilGenImages(1, &ImgId);

	// bind image
	ilBindImage(ImgId);

	// load image
	if(! ilLoadImage(filename))
	{
		ilDeleteImages(1, &ImgId);
		DevILShutDown();
		return false;
	}

	// get properties
	int bytePerPix = ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL);

	int width  = ilGetInteger(IL_IMAGE_WIDTH);
	int height = ilGetInteger(IL_IMAGE_HEIGHT);

	if(bytePerPix == 4)
		ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	else
		ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

	// create image
	MImage * image = (MImage *)data;
	image->create(M_UBYTE, (unsigned int)width, (unsigned int)height, (unsigned int)bytePerPix);

	// copy data
	unsigned int size = image->getSize();
	memcpy(image->getData(), ilGetData(), size);

	ilDeleteImages(1, &ImgId);
	DevILShutDown();
	return true;
}

bool M_saveImage(const char * filename, void * data, unsigned int quality)
{
	DevILInit();

	ILuint ImgId = 0;
	ilGenImages(1, &ImgId);

	// bind this image name.
	ilBindImage(ImgId);

	MImage * image = (MImage *)data;
	unsigned int width = image->getWidth();
	unsigned int height = image->getHeight();
	unsigned int components = image->getComponents();

	if(components == 3)
		ilTexImage(width, height, 1, 3, IL_RGB, IL_UNSIGNED_BYTE, image->getData());
	else if(components == 4)
		ilTexImage(width, height, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, image->getData());

	iluFlipImage();

	if(quality < 100)
		ilSetInteger(IL_JPG_QUALITY, quality);

	ilEnable(IL_FILE_OVERWRITE);
	ilSaveImage(filename);

	ilDeleteImages(1, &ImgId);
	DevILShutDown();
	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis Image tools
// Sobel.h
//
// Sobel filter
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

#include <MCore.h>
#include "Common.h"
#include "Sobel.h"


static float convolve(MImage * src, int x, int y, float * k)
{
	unsigned int width = src->getWidth();
	float * data = (float *)src->getData();
	float *p = k, sum = 0;
	
	for(int i=-1; i<=1; i++)
	for(int j=-1; j<=1; j++)
	{
		sum += (*(data + width*(y+i) + (x+j))) * (*p);
		p++;
	}
	
	return sum;
}

bool sobelFilter(MImage * src, MImage * dest, int radius)
{
	if(! isImageValid(src) || radius == 0 || dest == NULL)
		return false;

	radius = ABS(radius);

	int width = (int)src->getWidth();
	int height = (int)src->getHeight();
	unsigned int components = src->getComponents();
	M_TYPES dataType = src->getDataType();

	if(! (components == 1 && dataType == M_FLOAT))
		return false;

	if(width < 3 || height < 3)
		return false;
	
	float *kx, *ky;
	
	if(radius == 1)
	{
		static float ky1[9] = {0, -1, 0, 0, 0, 0, 0, 1, 0};
		static float kx1[9] = {0, 0, 0, -1, 0, 1, 0, 0, 0};
		kx = kx1;
		ky = ky1;
	}
	else
	{
		static float ky2[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
		static float kx2[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
		kx = kx2;
		ky = ky2;
	}
	
	dest->create(M_FLOAT, width, height, 2);
	memset(dest->getData(), 0, sizeof(float)*dest->getSize());
	
	float * destPixel;
	float * destData = (float *)dest->getData();
	
	int x, y;
	for(y=1; y<height-1; y++)
	{
		destPixel = destData + y*width*2 + 2;
		for(x=1; x<width-1; x++)
		{
			destPixel[0] = convolve(src, x, y, kx);
			destPixel[1] = convolve(src, x, y, ky);
			destPixel += 2;
		}
	}
	
	return true;
}

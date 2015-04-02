/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis Image tools
// Resize.cpp
//
// Image resize
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
#include "Resize.h"


bool pyrDownImage(MImage * image)
{
	if(! isImageValid(image))
		return false;
	
	M_TYPES type = image->getDataType();
	
	if(type == M_FLOAT)
	{
		int width = (int)image->getWidth();
		int height = (int)image->getHeight();
		int halfWidth = (width+1)/2;
		int halfHeight = (height+1)/2;
		unsigned int components = image->getComponents();
	
		MImage copy(*image);
		image->create(type, halfWidth, halfHeight, components);
		
		float * copyData = (float *)copy.getData();
		float * destData = (float *)image->getData();
		
		#pragma omp parallel for schedule(dynamic, 8)
		for(int y=0; y<halfHeight; y++)
		{
			float * color;
			float * pixel = destData + halfWidth*y*components;
		
			int i;
			int y2 = y*2;
			int y2p = MIN(y2+1, height-1);
			
			for(int x=0; x<halfWidth; x++)
			{
				int x2 = x*2;
				int x2p = MIN(x2+1, width-1);
			
				color = copyData + (y2*width + x2)*components;
				for(i=0; i<components; i++)
					pixel[i] = color[i];
				
				color = copyData + (y2p*width + x2)*components;
				for(i=0; i<components; i++)
					pixel[i] += color[i];
				
				color = copyData + (y2*width + x2p)*components;
				for(i=0; i<components; i++)
					pixel[i] += color[i];
				
				color = copyData + (y2p*width + x2p)*components;
				for(i=0; i<components; i++)
					pixel[i] += color[i];
				
				for(i=0; i<components; i++)
					pixel[i] *= 0.25f;
				
				pixel+=components;
			}
		}
	
		return true;
	}
	else if(type == M_UBYTE)
	{
		convertToFloat(image);
		pyrDownImage(image);
		convertToUbyte(image);
		return true;
	}
	
	return false;
}

bool resizeImage(MImage * image, unsigned int destWidth, unsigned int destHeight, float filter)
{
	if(! isImageValid(image))
		return false;
		
	M_TYPES type = image->getDataType();
	
	if(type == M_FLOAT)
	{
		MImage copy(*image);
		
		unsigned int components = image->getComponents();
		unsigned int srcWidth = image->getWidth();
		unsigned int srcHeight = image->getHeight();

		float xFilter = filter;
		float yFilter = filter;
		
		MVector2 scale(srcWidth/(float)destWidth, srcHeight/(float)destHeight);

		image->create(type, destWidth, destHeight, components);
		float * destData = (float *)image->getData();
		
		if(filter > 0)
		{
			#pragma omp parallel for schedule(dynamic, 8)
			for(unsigned int y=0; y<destHeight; y++)
			{
				float color[4];
				float totalColor[4];
				float * pixel = destData + destWidth*y*components;
			
				for(unsigned int x=0; x<destWidth; x++)
				{
					unsigned int i;
					MVector2 srcPos, destPos = MVector2((float)x, (float)y);
				
					float score = 0;
					memset(totalColor, 0, components*sizeof(float));
		
					float dx, dy;
					for(dy=-yFilter; dy<=yFilter; dy+=yFilter)
					for(dx=-xFilter; dx<=xFilter; dx+=xFilter)
					{
						srcPos = (destPos + MVector2(dx, dy))*scale;
						getImageSubPixel_float(&copy, srcPos.x-0.5f, srcPos.y-0.5f, color);
						for(i=0; i<components; i++)
							totalColor[i] += color[i];
						score++;
					}
					
					for(i=0; i<components; i++)
						pixel[i] = (totalColor[i] / score);
			
					pixel += components;
				}
			}
		}
		else if(filter == 0)
		{
			#pragma omp parallel for schedule(dynamic, 8)
			for(unsigned int y=0; y<destHeight; y++)
			{
				float * pixel = destData + destWidth*y*components;
				for(unsigned int x=0; x<destWidth; x++)
				{
					MVector2 srcPos = MVector2((float)x, (float)y)*scale;
					getImageSubPixel_float(&copy, srcPos.x-0.5f, srcPos.y-0.5f, pixel);
					pixel += components;
				}
			}
		}
		else
		{
			for(unsigned int y=0; y<destHeight; y++)
			{
				float * pixel = destData + destWidth*y*components;
				for(unsigned int x=0; x<destWidth; x++)
				{
					MVector2 srcPos = MVector2((float)x, (float)y)*scale;
					copy.readPixel(srcPos.x, srcPos.y, pixel);
					pixel += components;
				}
			}
		}
	}
	else if(type == M_UBYTE)
	{
		convertToFloat(image);
		resizeImage(image, destWidth, destHeight, filter);
		convertToUbyte(image);
		return true;
	}
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis Image tools
// Blur.cpp
//
// Image blur
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
#include "Blur.h"


bool gaussianBlur(MImage * image, unsigned int radius)
{
	if(! isImageValid(image) || radius == 0)
		return false;
	
	int width = (int)image->getWidth();
	int height = (int)image->getHeight();
    unsigned int size = image->getSize();
	unsigned int components = image->getComponents();
	
	
    // make copy
	MImage work, out, copy;
	out.create(M_FLOAT, width, height, components);
	work.create(M_FLOAT, width, height, components);
	copy.create(M_FLOAT, width, height, components);
	
	float * outData = (float *)out.getData();
	float * workData = (float *)work.getData();
	float * copyData = (float *)copy.getData();
	float * blurTable;
	
	if(image->getDataType() == M_UBYTE)
	{
		unsigned char * imageData = (unsigned char *)image->getData();
		
		// convert to float
	    float div = 1.0f/255.0f;
		
		unsigned int i;
        for(i=0; i<size; i++)
            copyData[i] = imageData[i]*div;
	}
	else if(image->getDataType() == M_FLOAT)
	{
		memcpy(copyData, image->getData(), sizeof(float)*size);
	}
	
	
	// clear work
	memset(workData, 0, sizeof(float)*size);
	
	
	// build blur table
	{
		int r;
		int tableSize = radius*2+1;
		float gaussfac = 1.6f;
		
		blurTable = new float[tableSize];
		
		if(radius==1)
		{
			blurTable[0] = 0.2f;
			blurTable[1] = 0.6f;
			blurTable[2] = 0.2f;
		}
		else
		{
			float sum = 0.0f;
			for(r=-(int)radius; r<=(int)radius; r++)
			{
				float x = fabs((float)r / (float)radius)*gaussfac;
				float val = (float)(1.0/exp(x*x) - 1.0/exp(gaussfac*gaussfac*2.25));
				
				sum += val;
				blurTable[r+radius] = val;
			}
			
			sum = 1.0f/sum;
			for(r=0; r<tableSize; r++)
				blurTable[r] *= sum;
		}
	}
	
	
    // blur
    {
        int yStep = width*components;
		
        // horizontal blur
		#pragma omp parallel for schedule(dynamic, 8)
        for(int y=0; y<height; y++)
        {
			int i, x, xx;
			
			float * workPixel = workData + y*yStep;
            float * copyY = copyData + y*yStep;
            for(x=0; x<width; x++)
            {
                int start = MAX(0, x-(int)radius);
                int end = MIN(width-1, x+(int)radius);
				
				float * val = blurTable + (start - (x-radius));
				
                float sum = 0.0f;
				memset(workPixel, 0, sizeof(float)*components);
				
                float * copyPixel = copyY + start*components;
				for(xx=start; xx<=end; xx++)
				{
					for(i=0; i<components; i++)
						workPixel[i] += (*copyPixel++) * (*val);
					sum += (*val);
					val++;
				}
				
				sum = 1.0f/sum;
				for(i=0; i<components; i++)
					(*workPixel++) *= sum;
            }
        }
		
        // vertical blur
		#pragma omp parallel for schedule(dynamic, 8)
        for(int x=0; x<width; x++)
        {
			int i, y, yy;
			
            float * outPixel = outData + x*components;
            float * workX = workData + x*components;
			
            for(y=0; y<height; y++)
            {
                int start = MAX(0, y-(int)radius);
                int end = MIN(height-1, y+(int)radius);
				
				float * val = blurTable + (start - (y-(int)radius));
				
                float sum = 0.0f;
				memset(outPixel, 0, sizeof(float)*components);
				
				float * workPixel = workX + yStep*start;
				for(yy=start; yy<=end; yy++)
				{
					for(i=0; i<components; i++)
						outPixel[i] += workPixel[i] * (*val);
					sum += (*val);
					val++;
					workPixel += yStep;
				}
				
				sum = 1.0f/sum;
				for(i=0; i<components; i++)
					outPixel[i] *= sum;
				
                outPixel += yStep;
            }
        }
    }
	
	
	// copy result
	if(image->getDataType() == M_UBYTE)
	{
		unsigned char * imageData = (unsigned char *)image->getData();
		
		unsigned int i;
        for(i=0; i<size; i++)
        {
            imageData[i] = CLAMP(outData[i]*255, 0, 255);
        }
	}
	else if(image->getDataType() == M_FLOAT)
	{
		memcpy(image->getData(), outData, sizeof(float)*size);
	}
	
	
	// clear
	delete [] blurTable;
	return true;
}

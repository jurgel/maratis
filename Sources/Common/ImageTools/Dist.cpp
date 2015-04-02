/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis Image tools
// Dist.cpp
//
// Distance transformation
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
#include "Dist.h"


// From paper :
// Distance Transforms of Sampled Functions
// by Pedro F. Felzenszwalb and Daniel P. Huttenlocher

static void distTransform1d(float * input, float * dist_output, int * coord_output, int n)
{
	int k;
	int * v = new int[n];
	float * z = new float[n+1];

	v[0] = 0;
	z[0] = -1e20;
	z[1] = 1e20;
	
	k = 0;
	for(int q=1; q<n; q++)
	{
		float s;
		while(1)
		{
			s = ((input[q] + q*q) - (input[v[k]] + v[k]*v[k])) / (float)(2*q - 2*v[k]);
			if(s > z[k])
				break;
			k--;
		}
		
		k++;
		v[k] = q;
		z[k] = s;
		z[k+1] = 1e20;
	}
	
	if(coord_output)
	{
		k = 0;
		for(int q=0; q<n; q++)
		{
			while(z[k+1] < q) k++;
			dist_output[q] = (q - v[k])*(q - v[k]) + input[v[k]];
			coord_output[q] = v[k];
		}
	}
	else
	{
		k = 0;
		for(int q=0; q<n; q++)
		{
			while(z[k+1] < q) k++;
			dist_output[q] = (q - v[k])*(q - v[k]) + input[v[k]];
		}
	}

	delete [] v;
	delete [] z;
}

static void distTransform2d(float * image, int * coords, int width, int height)
{
	int x, y;
	float * tmp_in = new float[MAX(width, height)];
	float * tmp_out = new float[MAX(width, height)];
	
	if(coords)
	{
		int * tmp_coords = new int[MAX(width, height)];
		int * tmp_coords2 = new int[width];
	
		// vertical pass
		for(x=0; x<width; x++)
		{
			for(y=0; y<height; y++)
				tmp_in[y] = *(image + width*y + x);
		
			distTransform1d(tmp_in, tmp_out, tmp_coords, height);
		
			for(y=0; y<height; y++)
			{
				*(image + width*y + x) = tmp_out[y];
				*(coords + width*y + x) = tmp_coords[y];
			}
		}

		// horizontal pass
		for(y=0; y<height; y++)
		{
			distTransform1d(image + width*y, tmp_out, tmp_coords, width);
			memcpy(image + width*y, tmp_out, width*sizeof(float));
			memcpy(tmp_coords2, coords + width*y, width*sizeof(int));
			
			for(x=0; x<width; x++)
			{
				int _x = tmp_coords[x];
				int _y = tmp_coords2[_x];
				*(coords + width*y + x) = _y*width + _x;
			}
		}
		
		delete [] tmp_coords;
		delete [] tmp_coords2;
	}
	else
	{
		// vertical pass
		for(x=0; x<width; x++)
		{
			for(y=0; y<height; y++)
				tmp_in[y] = *(image + width*y + x);
		
			distTransform1d(tmp_in, tmp_out, NULL, height);
		
			for(y=0; y<height; y++)
				*(image + width*y + x) = tmp_out[y];
		}

		// horizontal pass
		for(y=0; y<height; y++)
		{
			distTransform1d(image + width*y, tmp_out, NULL, width);
			memcpy(image + width*y, tmp_out, width*sizeof(float));
		}
	}

	delete [] tmp_in;
	delete [] tmp_out;
}

bool distTransform(MImage * src, MImage * dest, MImage * voronoi)
{
	if(! isImageValid(src) || ! dest)
		return false;
		
	if(! (src->getDataType() == M_UBYTE && src->getComponents() == 1))
		return false;
		
	unsigned int width = src->getWidth();
	unsigned int height = src->getHeight();
	
	dest->create(M_FLOAT, width, height, 1);
	
	unsigned char * srcData = (unsigned char *)src->getData();
	float * destData = (float *)dest->getData();
	
	unsigned int i, size = src->getSize();
	for(i=0; i<size; i++)
	{
		if(srcData[i] == 0)
			destData[i] = 1e20;
		else
			destData[i] = 0;
	}

	if(voronoi)
	{
		voronoi->create(M_INT, width, height, 1);
		distTransform2d(destData, (int *)voronoi->getData(), (int)width, (int)height);
	}
	else{
		distTransform2d(destData, NULL, (int)width, (int)height);
	}
	
	for(i=0; i<size; i++)
		destData[i] = sqrtf(destData[i]);
	
	return true;
}

bool voronoiFill(MImage * src, MImage * voronoi)
{
	if(! isImageValid(src) || ! isImageValid(voronoi))
		return false;
		
	int * voData = (int *)voronoi->getData();
	unsigned int components = src->getComponents();
	
	if(src->getDataType() == M_UBYTE)
	{
		unsigned char * srcData = (unsigned char *)src->getData();

		unsigned int i, size = voronoi->getSize();
		for(i=0; i<size; i++)
		{
			for(int c=0; c<components; c++)
				srcData[i*components+c] = srcData[voData[i]*components+c];
		}
		
		return true;
	}
	else if(src->getDataType() == M_FLOAT)
	{
		float * srcData = (float *)src->getData();

		unsigned int i, size = voronoi->getSize();
		for(i=0; i<size; i++)
		{
			for(int c=0; c<components; c++)
				srcData[i*components+c] = srcData[voData[i]*components+c];
		}
		
		return true;
	}
	else if(src->getDataType() == M_INT)
	{
		int * srcData = (int *)src->getData();

		unsigned int i, size = voronoi->getSize();
		for(i=0; i<size; i++)
		{
			for(int c=0; c<components; c++)
				srcData[i*components+c] = srcData[voData[i]*components+c];
		}
		
		return true;
	}
	
	return false;
}

bool voronoiFill(MImage * src)
{
	if(! isImageValid(src))
		return false;
		
	MImage binSrc(*src);
	convertToGreyscale(&binSrc);
	convertToUbyte(&binSrc);
		
	MImage dist, vo;
	if(! distTransform(&binSrc, &dist, &vo))
		return false;
	
	return voronoiFill(src, &vo);
}

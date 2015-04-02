/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis Image tools
// Draw.h
//
// Drawing tools
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
#include "Draw.h"


void drawLine(MImage * image, int x0, int y0, int x1, int y1, void * color, float radius)
{
	unsigned int width = image->getWidth();
	unsigned int height = image->getHeight();
	
	int dx =  ABS(x1-x0), sx = x0<x1 ? 1 : -1;
	int dy = -ABS(y1-y0), sy = y0<y1 ? 1 : -1;
	int err = dx+dy, e2;
	
	while(1)
	{
		if(isInFrame(x0, y0, width, height))
		{
			if(radius <= 1)
			{
				image->writePixel(x0, y0, color);
			}
			else
				drawCircle(image, MVector2(x0+0.5f, y0+0.5f), radius, color);
		}
		
		if(x0==x1 && y0==y1)
			break;
		
		e2 = 2*err;
		if(e2 >= dy) { err += dy; x0 += sx; }
		if(e2 <= dx) { err += dx; y0 += sy; }
	}
}


// Midpoint Circle Algorithm : http://en.wikipedia.org/wiki/Midpoint_circle_algorithm
void drawLineCircle(MImage * image, int x0, int y0, int radius, void * color)
{
	unsigned int width = image->getWidth();
	unsigned int height = image->getHeight();

	int x = radius, y = 0;
	int radiusError = 1-x;
 
	while(x >= y)
	{
		if(isInFrame(x + x0, y + y0, width, height))
			image->writePixel(x + x0, y + y0, color);
			
		if(isInFrame(y + x0, x + y0, width, height))
			image->writePixel(y + x0, x + y0, color);
			
		if(isInFrame(-x + x0, y + y0, width, height))
			image->writePixel(-x + x0, y + y0, color);
			
		if(isInFrame(-y + x0, x + y0, width, height))
			image->writePixel(-y + x0, x + y0, color);
		
		if(isInFrame(-x + x0, -y + y0, width, height))
			image->writePixel(-x + x0, -y + y0, color);
			
		if(isInFrame(-y + x0, -x + y0, width, height))
			image->writePixel(-y + x0, -x + y0, color);
			
		if(isInFrame(x + x0, -y + y0, width, height))
			image->writePixel(x + x0, -y + y0, color);
			
		if(isInFrame(y + x0, -x + y0, width, height))
			image->writePixel(y + x0, -x + y0, color);
 
		y++;
        
		if(radiusError<0)
			radiusError+=2*y+1;
        else
        {
			x--;
			radiusError+=2*(y-x+1);
        }
	}
}

void drawCircle(MImage * image, const MVector2 & center, float radius, void * color)
{
	float R2 = radius*radius;

	int width = image->getWidth();
	int height = image->getHeight();
	
	int minx = MAX((int)(center.x-radius), 0);
	int maxx = MIN((int)(center.x+radius)+1, width-1);
	int miny = MAX((int)(center.y-radius), 0);
	int maxy = MIN((int)(center.y+radius)+1, height-1);
	
	for(int y=miny; y<=maxy; y++)
	{
		for(int x=minx; x<=maxx; x++)
		{
			if((MVector2(x, y) - center).getSquaredLength() < R2)
				image->writePixel(x, y, color);
		}
	}
}

void drawLines(MImage * image, MVector2 * points, unsigned int nb, void * color, float radius)
{
	if(nb < 2)
		return;
	
	for(int p=1; p<nb; p++)
		drawLine(image, points[p-1].x, points[p-1].y, points[p].x, points[p].y, color, radius);
}

// drawPolygon adapted from :
// http://alienryderflex.com/polygon_fill/
// public-domain code by Darel Rex Finley, 2007

void drawPolygon(MImage * image, MVector2 * points, unsigned int nb, void * color)
{
	if(nb < 3)
		return;
	
	int nodes, pixelY, i, j, swap;
	int * nodeX = new int[nb];
	unsigned int width = image->getWidth();
	unsigned int height = image->getHeight();
	
	
	// find bounding box
	int IMAGE_LEFT, IMAGE_RIGHT;
	int IMAGE_TOP, IMAGE_BOT;
	{
		MVector2 min, max;
		
		min = max = points[0];
		for(i=1; i<nb; i++)
		{
			min.x = MIN(min.x, points[i].x);
			min.y = MIN(min.y, points[i].y);
			max.x = MAX(max.x, points[i].x);
			max.y = MAX(max.y, points[i].y);
		}
		
		IMAGE_LEFT = MAX(0, (int)min.x);
		IMAGE_RIGHT = MIN((int)width - 1, (int)max.x + 1);
		IMAGE_TOP = MAX(0, (int)min.y);
		IMAGE_BOT = MIN((int)height - 1, (int)max.y + 1);
	}


	//  loop through the rows of the image.
	for(pixelY=IMAGE_TOP; pixelY<IMAGE_BOT; pixelY++)
	{
		//  build a list of nodes.
		nodes=0; j=nb-1;
		for(i=0; i<nb; i++)
		{
			if((points[i].y < (float)pixelY && points[j].y >= (float)pixelY) ||
			   (points[j].y < (float)pixelY && points[i].y >= (float)pixelY))
			{
				nodeX[nodes++] = (int)(
					points[i].x +
					(pixelY - points[i].y) / (points[j].y - points[i].y) *
					(points[j].x - points[i].x)
				);
			}
			j=i;
		}

		//  sort the nodes, via a simple “Bubble” sort.
		i=0;
		while(i < nodes-1)
		{
			if(nodeX[i] > nodeX[i+1])
			{
				swap = nodeX[i];
				nodeX[i] = nodeX[i+1];
				nodeX[i+1] = swap;
				if(i) i--;
			}
			else{
				i++;
			}
		}

		// fill the pixels between node pairs.
		for(i=0; i<nodes; i+=2)
		{
			if(nodeX[i] >= IMAGE_RIGHT)
				break;
				
			if(nodeX[i+1] > IMAGE_LEFT)
			{
				if(nodeX[i] < IMAGE_LEFT)
					nodeX[i] = IMAGE_LEFT;
					
				if(nodeX[i+1] > IMAGE_RIGHT)
					nodeX[i+1] = IMAGE_RIGHT;
				
				for(j=nodeX[i]; j<nodeX[i+1]; j++)
					image->writePixel(j, pixelY, color);
			}
		}
	}
	
	delete [] nodeX;
}

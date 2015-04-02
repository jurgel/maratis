/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis Image tools
// Thin.cpp
//
// Image thinning
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
#include "Thin.h"


// C code from the article
// "Efficient Binary Image Thinning using Neighborhood Maps"
// by Joseph M. Cychosz, in "Graphics Gems IV", Academic Press, 1994
// Thins the image using Rosenfeld's parallel thinning algorithm.
// Adapted to use MImage

// Direction masks:
//   N	   S	 W     E
static int masks[] = {0200, 0002, 0040, 0010};

//	True if pixel neighbor map indicates the pixel is 8-simple and
//	not an end point and thus can be deleted.  The neighborhood
//	map is defined as an integer of bits abcdefghi with a non-zero
//	bit representing a non-zero pixel.  The bit assignment for the
//	neighborhood is:
//
//				a b c
//				d e f
//				g h i

static unsigned char deleteMap[512] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

bool thinImage(MImage * image)
{
	if(! isImageValid(image))
		return false;

	// takes only ubyte 1 component image
	if(! (image->getDataType() == M_UBYTE && image->getComponents() == 1))
		return false;
	
	int xsize, ysize;		// Image resolution
	int x, y;				// Pixel location
	int	i;					// Pass index
	int	pc = 0;				// Pass count
	int	count = 1;			// Deleted pixel count
	int	p, q;				// Neighborhood maps of adjacent cells
	unsigned char ** ip;	// scanline pointers, ip[y][x]
	unsigned char * qb;		// Neighborhood maps of previous scanline
	int	m;					// Deletion direction mask

	xsize = (int)image->getWidth();
	ysize = (int)image->getHeight();
	qb = (unsigned char *) malloc(xsize*sizeof(char));
	qb[xsize-1] = 0; // Used for lower-right pixel

	// image data
	unsigned char * data = (unsigned char *)image->getData();
	
	// alloc scanline pointers
	ip = (unsigned char **) malloc(sizeof(void *)*ysize);
	
	// set scanline pointers
	for(y=0; y<ysize; y++){
		ip[y] = data + y*xsize;
	}

	while(count) // Scan image while deletions
	{		
	    pc++;
	    count = 0;

	    for(i=0; i<4; i++)
		{
			m = masks[i];
			
			// Build initial previous scan buffer
			p = ip[0][0] != 0;
			for(x=0; x<xsize-1; x++)
				qb[x] = p = ((p<<1)&0006) | (ip[0][x+1] != 0);
			
			// Scan image for pixel deletion candidates
			for(y=0; y<ysize-1; y++)
			{
				q = qb[0];
				p = ((q<<3)&0110) | (ip[y+1][0] != 0);
				
				for(x=0; x<xsize-1; x++)
				{
					q = qb[x];
					p = ((p<<1)&0666) | ((q<<3)&0110) | (ip[y+1][x+1] != 0);
					qb[x] = p;
					if(((p&m) == 0) && deleteMap[p])
					{
						if(ip[y][x] != 0)
						{
							count++;
							ip[y][x] = 0;
						}
					}
				}
				
				// Process right edge pixel
				p = (p<<1)&0666;
				if((p&m) == 0 && deleteMap[p])
				{
					if(ip[y][xsize-1] != 0)
					{
						count++;
						ip[y][xsize-1] = 0;
					}
				}
			}
			
			// Process bottom scan line
			for(x=0; x<xsize; x++)
			{
				q = qb[x];
				p = ((p<<1)&0666) | ((q<<3)&0110);
				if((p&m) == 0 && deleteMap[p])
				{
					if(ip[ysize-1][x] != 0)
					{
						count++;
						ip[ysize-1][x] = 0;
					}
				}
			}
	    }
	}

	free(qb);
	free(ip);
	return true;
}

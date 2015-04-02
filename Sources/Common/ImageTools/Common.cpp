/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis Image tools
// Common.cpp
//
// Image common
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


static void ubyteToFloat(MImage * src, MImage * dest)
{
	static const float ubyte_div = 1.0f/255.0f;
	
	unsigned char * data1 = (unsigned char *)src->getData();
	float * data2 = (float *)dest->getData();
	unsigned int size = src->getSize();
	for(unsigned int i=0; i<size; i++)
		data2[i] = (float)(data1[i])*ubyte_div;
}

static void ushortToFloat(MImage * src, MImage * dest)
{
	static const float ushort_div = 1.0f/65535.0f;
	
	unsigned short * data1 = (unsigned short *)src->getData();
	float * data2 = (float *)dest->getData();
	unsigned int size = src->getSize();
	for(unsigned int i=0; i<size; i++)
		data2[i] = (float)(data1[i])*ushort_div;
}

static void intToFloat(MImage * src, MImage * dest)
{
	int * data1 = (int *)src->getData();
	float * data2 = (float *)dest->getData();
	unsigned int size = src->getSize();
	for(unsigned int i=0; i<size; i++)
		data2[i] = (float)(data1[i]);
}

static void floatToUbyte(MImage * src, MImage * dest)
{
	float * data1 = (float *)src->getData();
	unsigned char * data2 = (unsigned char *)dest->getData();
	unsigned int size = src->getSize();
	for(int i=0; i<size; i++)
		data2[i] = (unsigned char)CLAMP(data1[i]*255, 0, 255);
}

bool isImageValid(MImage * image)
{
	if(! image)
		return false;

	if(image->getSize() == 0)
		return false;
		
	return true;
}

bool copySubImage(MImage * source, MImage * dest, int x, int y, unsigned int w, unsigned int h)
{
	if(! isImageValid(source))
		return false;
		
	int swidth = source->getWidth();
	int sheight = source->getHeight();
	unsigned int components = source->getComponents();
	M_TYPES dataType = source->getDataType();
	
	int minx = MAX(0, x);
	int miny = MAX(0, y);
	int maxx = MIN(x+(int)w-1, swidth-1);
	int maxy = MIN(y+(int)h-1, sheight-1);
		
	int dwidth = maxx - minx + 1;
	int dheight = maxy - miny + 1;
	if(dwidth==0 || dheight==0)
		return false;
	
	dest->create(dataType, dwidth, dheight, components);
	
	int sstep = swidth*components;
	int dstep = dwidth*components;
	
	switch(dataType)
	{
	default:
		break;
	case M_UBYTE:
		{
			unsigned char * sData = (unsigned char *)source->getData();
			unsigned char * dData = (unsigned char *)dest->getData();
			sData += (miny*swidth + minx)*components;
			for(int y=miny; y<=maxy; y++)
			{
				memcpy(dData, sData, dstep*sizeof(unsigned char));
				dData+=dstep;
				sData+=sstep;
			}
		}
		break;
	case M_USHORT:
		{
			unsigned short * sData = (unsigned short *)source->getData();
			unsigned short * dData = (unsigned short *)dest->getData();
			sData += (miny*swidth + minx)*components;
			for(int y=miny; y<=maxy; y++)
			{
				memcpy(dData, sData, dstep*sizeof(unsigned short));
				dData+=dstep;
				sData+=sstep;
			}
		}
		break;
	case M_INT:
		{
			int * sData = (int *)source->getData();
			int * dData = (int *)dest->getData();
			sData += (miny*swidth + minx)*components;
			for(int y=miny; y<=maxy; y++)
			{
				memcpy(dData, sData, dstep*sizeof(int));
				dData+=dstep;
				sData+=sstep;
			}
		}
		break;
	case M_FLOAT:
		{
			float * sData = (float *)source->getData();
			float * dData = (float *)dest->getData();
			sData += (miny*swidth + minx)*components;
			for(int y=miny; y<=maxy; y++)
			{
				memcpy(dData, sData, dstep*sizeof(float));
				dData+=dstep;
				sData+=sstep;
			}
		}
		break;
	}

	return false;
}

bool convertToFloat(MImage * image)
{
	if(! isImageValid(image))
		return false;
		
	unsigned int width = image->getWidth();
	unsigned int height = image->getHeight();
	unsigned int components = image->getComponents();
	
	if(image->getDataType() == M_UBYTE)
	{
		MImage copy(*image);
		image->create(M_FLOAT, width, height, components);
		ubyteToFloat(&copy, image);
		return true;
	}
	else if(image->getDataType() == M_USHORT)
	{
		MImage copy(*image);
		image->create(M_FLOAT, width, height, components);
		ushortToFloat(&copy, image);
		return true;
	}
	else if(image->getDataType() == M_INT)
	{
		MImage copy(*image);
		image->create(M_FLOAT, width, height, components);
		intToFloat(&copy, image);
		return true;
	}
	
	return false;
}

bool convertToUbyte(MImage * image)
{
	if(! isImageValid(image))
		return false;
		
	unsigned int width = image->getWidth();
	unsigned int height = image->getHeight();
	unsigned int components = image->getComponents();
	
	if(image->getDataType() == M_FLOAT)
	{
		MImage copy(*image);
		image->create(M_UBYTE, width, height, components);
		floatToUbyte(&copy, image);
		return true;
	}
	
	return false;
}

bool convertComponentToGreyscale(MImage * image, unsigned int id)
{
	if(! isImageValid(image))
		return false;

	if(id >= image->getComponents())
		return false;

	unsigned int width = image->getWidth();
	unsigned int height = image->getHeight();
	unsigned int comp = image->getComponents();

	if(image->getDataType() == M_UBYTE)
	{
		MImage copy(*image);
		image->create(M_UBYTE, width, height, 1);
		unsigned char * greyPixel = (unsigned char *)image->getData();
		unsigned char * imagePixel = (unsigned char *)copy.getData();
	
		unsigned int x, y;
		for(y=0; y<height; y++)
		for(x=0; x<width; x++)
		{
			(*greyPixel) = imagePixel[id];
			imagePixel+=comp;
			greyPixel++;
		}
		
		return true;
	}
	else if(image->getDataType() == M_FLOAT)
	{
		MImage copy(*image);
		image->create(M_FLOAT, width, height, 1);
		float * greyPixel = (float *)image->getData();
		float * imagePixel = (float *)copy.getData();
	
		unsigned int x, y;
		for(y=0; y<height; y++)
		for(x=0; x<width; x++)
		{
			(*greyPixel) = imagePixel[id];
			imagePixel+=comp;
			greyPixel++;
		}
		
		return true;
	}
	
	return false;
}

bool convertAlphaToGreyscale(MImage * image)
{
	if(! isImageValid(image))
		return false;

	if(image->getComponents() != 4)
		return false;
	
	return convertComponentToGreyscale(image, 3);
}

bool convertGreyscaleToAlpha(MImage * image)
{
	if(! isImageValid(image))
		return false;

	if(image->getComponents() != 1)
		return false;

	unsigned int width = image->getWidth();
	unsigned int height = image->getHeight();

	if(image->getDataType() == M_UBYTE)
	{
		MImage copy(*image);
		image->create(M_UBYTE, width, height, 4);

		unsigned char * greyPixel = (unsigned char *)copy.getData();
		unsigned char * imagePixel = (unsigned char *)image->getData();
	
		unsigned int x, y;
		for(y=0; y<height; y++)
		for(x=0; x<width; x++)
		{
			imagePixel[0] = imagePixel[1] = imagePixel[2] = 255;
			imagePixel[3] = (*greyPixel);
			imagePixel+=4;
			greyPixel++;
		}
		
		return true;
	}
	else if(image->getDataType() == M_FLOAT)
	{
		MImage copy(*image);
		image->create(M_FLOAT, width, height, 4);

		float * greyPixel = (float *)copy.getData();
		float * imagePixel = (float *)image->getData();
	
		unsigned int x, y;
		for(y=0; y<height; y++)
		for(x=0; x<width; x++)
		{
			imagePixel[0] = imagePixel[1] = imagePixel[2] = 1;
			imagePixel[3] = (*greyPixel);
			imagePixel+=4;
			greyPixel++;
		}
		
		return true;
	}
	
	return false;
}

bool convertToGreyscale(MImage * image, float rf, float gf, float bf)
{
	if(! isImageValid(image))
		return false;

	if(image->getComponents() == 1)
		return false;

	if(image->getDataType() == M_UBYTE)
	{
		convertToFloat(image);
		convertToGreyscale(image);
		convertToUbyte(image);
		return true;
	}
	else if(image->getDataType() != M_FLOAT)
		return false;
	
	unsigned int width = image->getWidth();
	unsigned int height = image->getHeight();
	unsigned int components = image->getComponents();
	
	if(image->getComponents() == 3)
	{
		MImage copy(*image);
		image->create(M_FLOAT, width, height, 1);
		float * greyPixel = (float *)image->getData();
		float * imagePixel = (float *)copy.getData();
	
		unsigned int x, y;
		for(y=0; y<height; y++)
		for(x=0; x<width; x++)
		{
			(*greyPixel) = imagePixel[0]*rf + imagePixel[1]*gf + imagePixel[2]*bf;
			imagePixel += 3;
			greyPixel++;
		}
	}
	else if(image->getComponents() == 4)
	{
		MImage copy(*image);
		image->create(M_FLOAT, width, height, 1);
		float * greyPixel = (float *)image->getData();
		float * imagePixel = (float *)copy.getData();
	
		unsigned int x, y;
		for(y=0; y<height; y++)
		for(x=0; x<width; x++)
		{
			(*greyPixel) = 0.1f + (imagePixel[0]*rf + imagePixel[1]*gf + imagePixel[2]*bf)*imagePixel[3]*0.9f;
			imagePixel += 4;
			greyPixel++;
		}
	}
	else
	{
		MImage copy(*image);
		image->create(M_FLOAT, width, height, 1);
		float * greyPixel = (float *)image->getData();
		float * imagePixel = (float *)copy.getData();
	
		unsigned int x, y;
		for(y=0; y<height; y++)
		for(x=0; x<width; x++)
		{
			float val = 0;
			for(int i=0; i<components; i++)
				val += ABS(imagePixel[i]);
			(*greyPixel) = val / (float)components;
			imagePixel += components;
			greyPixel++;
		}
	}
	
	return true;
}

static bool convertNormalMap(MImage * image)
{
	if(! isImageValid(image))
		return false;

	if(! (image->getDataType() == M_FLOAT && image->getComponents() == 2))
		return false;
	
	int width = image->getWidth();
	int height = image->getHeight();
	
	MImage copy(*image);
	image->create(M_FLOAT, width, height, 3);
	
	float * pixel = (float *)image->getData();
	float * copyPixel = (float *)copy.getData();
	for(int y=0; y<height; y++)
	for(int x=0; x<width; x++)
	{
		pixel[0] = (CLAMP(copyPixel[0], -1, 1)+1)*0.5f;
		pixel[1] = (CLAMP(copyPixel[1], -1, 1)+1)*0.5f;
		pixel[2] = 0;
		
		copyPixel+=2;
		pixel+=3;
	}
	
	return true;
}

bool convertToRGB(MImage * image)
{
	if(convertNormalMap(image))
		return true;

	if(! isImageValid(image))
		return false;

	if(image->getComponents() != 1)
		return false;

	unsigned int width = image->getWidth();
	unsigned int height = image->getHeight();

	if(image->getDataType() == M_UBYTE)
	{
		MImage copy(*image);
		image->create(M_UBYTE, width, height, 3);
		unsigned char * rgbPixel = (unsigned char *)image->getData();
		unsigned char * imagePixel = (unsigned char *)copy.getData();
	
		unsigned int x, y;
		for(y=0; y<height; y++)
		for(x=0; x<width; x++)
		{
			rgbPixel[0] = rgbPixel[1] = rgbPixel[2] = *imagePixel;
			imagePixel++;
			rgbPixel+=3;
		}
		
		return true;
	}
	else if(image->getDataType() == M_FLOAT)
	{
		MImage copy(*image);
		image->create(M_FLOAT, width, height, 3);
		float * rgbPixel = (float *)image->getData();
		float * imagePixel = (float *)copy.getData();
	
		unsigned int x, y;
		for(y=0; y<height; y++)
		for(x=0; x<width; x++)
		{
			rgbPixel[0] = rgbPixel[1] = rgbPixel[2] = *imagePixel;
			imagePixel++;
			rgbPixel+=3;
		}
		
		return true;
	}
	
	return false;	
}

void getImageSubPixel_ubyte(MImage * image, float x, float y, float * color)
{
	int width = (int)image->getWidth();
	int height = (int)image->getHeight();

	unsigned int i;
	unsigned int components = image->getComponents();

	x = CLAMP(x, 0, width-1);
	y = CLAMP(y, 0, height-1);

	int ix = (int)(x);
	int iy = (int)(y);
	float fx = x - ix;
	float fy = y - iy;
	float A, B;

	unsigned char * colors[4];
	unsigned char * imgData = (unsigned char*)image->getData();
	
	colors[0] = imgData + (width*iy + ix)*components;
	colors[1] = imgData + (width*iy + MIN(width-1, ix+1))*components;
	colors[2] = imgData + (width*MIN(height-1, iy+1) + ix)*components;
	colors[3] = imgData + (width*MIN(height-1, iy+1) + MIN(width-1, ix+1))*components;
	
	for(i=0; i<components; i++)
	{
		A = colors[0][i] + (colors[2][i] - colors[0][i])*fy;
		B = colors[1][i] + (colors[3][i] - colors[1][i])*fy;
		color[i] = A + (B-A)*fx;
	}
}

void getImageSubPixel_float(MImage * image, float x, float y, float * color)
{
	int width = (int)image->getWidth();
	int height = (int)image->getHeight();

	unsigned int i, components = image->getComponents();

	int ix = (int)(x);
	int iy = (int)(y);
	float fx = x - ix;
	float fy = y - iy;
	float A, B;

	float * imgData = (float *)image->getData();
		
	ix = CLAMP(ix, 0, width-1);
	iy = CLAMP(iy, 0, height-1);
	int ix2 = CLAMP(ix+1, 0, width-1);
	int iy2 = CLAMP(iy+1, 0, height-1);
		
	float * colors0 = imgData + (width*iy + ix)*components;
	float * colors1 = imgData + (width*iy + ix2)*components;
	float * colors2 = imgData + (width*iy2 + ix)*components;
	float * colors3 = imgData + (width*iy2 + ix2)*components;
	
	for(i=0; i<components; i++)
	{
		A = colors0[i] + (colors2[i] - colors0[i])*fy;
		B = colors1[i] + (colors3[i] - colors1[i])*fy;
		color[i] = A + (B-A)*fx;
	}
}

void getImageSubPixel(MImage * image, float x, float y, float * color)
{
	if(image->getDataType() == M_UBYTE)
		getImageSubPixel_ubyte(image, x, y, color);
		
	else if(image->getDataType() == M_FLOAT)
		getImageSubPixel_float(image, x, y, color);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MImage.cpp
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


MImage::MImage(void):
m_data(NULL),
m_dataType(M_UBYTE),
m_components(0),
m_width(0),
m_height(0),
m_size(0)
{}

MImage::MImage(const MImage & image):
m_data(NULL),
m_dataType(M_UBYTE),
m_components(0),
m_width(0),
m_height(0),
m_size(0)
{
	copyFrom(image);
}

MImage::~MImage(void)
{
	clear();
}

void MImage::copyFrom(const MImage & image)
{
	create(image.m_dataType, image.m_width, image.m_height, image.m_components);
	
	switch(m_dataType)
	{
	case M_UBYTE:
		memcpy(m_data, image.m_data, image.m_size*sizeof(char));
		break;
	case M_USHORT:
		memcpy(m_data, image.m_data, image.m_size*sizeof(short));
		break;
	case M_INT:
		memcpy(m_data, image.m_data, image.m_size*sizeof(int));
		break;
	case M_FLOAT:
		memcpy(m_data, image.m_data, image.m_size*sizeof(float));
		break;
	default:
		break;
	}
}

void MImage::clear(void)
{
	SAFE_FREE(m_data);
	m_width = 0;
	m_height = 0;
	m_components = 0;
	m_size = 0;
}

void MImage::create(M_TYPES dataType, unsigned int width, unsigned int height, unsigned int components)
{
	if(width == m_width && height == m_height && components == m_components && dataType == m_dataType)
		return;
		
	SAFE_FREE(m_data);

    m_width = width;
    m_height = height;
	m_components = components;
	m_dataType = dataType;
    m_size = components * width * height;

	if(m_size > 0)
	switch(dataType)
	{
	default:
		break;
	case M_UBYTE:
		m_data = malloc(m_size*sizeof(char));
		break;
	case M_USHORT:
		m_data = malloc(m_size*sizeof(short));
		break;
	case M_INT:
		m_data = malloc(m_size*sizeof(int));
		break;
	case M_FLOAT:
		m_data = malloc(m_size*sizeof(float));
		break;
	}
}

void MImage::writePixel(unsigned int x, unsigned int y, void * color)
{
	unsigned int c;
	unsigned int pixelId = ((m_width * y) + x)*m_components;

	switch(m_dataType)
	{
	case M_UBYTE:
		for(c=0; c<m_components; c++)
			((unsigned char *)m_data)[pixelId+c] = ((unsigned char *)color)[c];
		break;
	case M_USHORT:
		for(c=0; c<m_components; c++)
			((unsigned short *)m_data)[pixelId+c] = ((unsigned short *)color)[c];
		break;
	case M_INT:
		for(c=0; c<m_components; c++)
			((int *)m_data)[pixelId+c] = ((int *)color)[c];
		break;
	case M_FLOAT:
		for(c=0; c<m_components; c++)
			((float *)m_data)[pixelId+c] = ((float *)color)[c];
		break;
	default:
		break;
	}
}

void MImage::readPixel(unsigned int x, unsigned int y, void * color)
{
	unsigned int c;
	unsigned int pixelId = ((m_width * y) + x)*m_components;

	switch(m_dataType)
	{
	case M_UBYTE:
		for(c=0; c<m_components; c++)
			((unsigned char *)color)[c] = ((unsigned char *)m_data)[pixelId+c];
		break;
	case M_USHORT:
		for(c=0; c<m_components; c++)
			((unsigned short *)color)[c] = ((unsigned short *)m_data)[pixelId+c];
		break;
	case M_INT:
		for(c=0; c<m_components; c++)
			((int *)color)[c] = ((int *)m_data)[pixelId+c];
		break;
	case M_FLOAT:
		for(c=0; c<m_components; c++)
			((float *)color)[c] = ((float *)m_data)[pixelId+c];
		break;
	default:
		break;
	}
}

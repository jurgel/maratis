/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MRenderArray.cpp
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
//  Maratis, Copyright (c) 2003-2011 Anael Seghezzi <www.maratis3d.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//========================================================================


#include "../Maratis/Maratis.h"


#define M_MAX_ARRAY 30000

M_PRIMITIVE_TYPES g_primitiveType;
unsigned int g_verticesNumber;
MVector3 g_vertices[M_MAX_ARRAY];


void beginDraw(M_PRIMITIVE_TYPES primitiveType)
{
	g_verticesNumber = 0;
	g_primitiveType = primitiveType;
}

void pushVertex(const MVector3 & vertex)
{
	g_vertices[g_verticesNumber] = vertex;
	g_verticesNumber++;
}

void endDraw(void)
{
	MRenderingContext * render = MEngine::getInstance()->getRenderingContext();

	render->disableTexCoordArray();
	render->disableColorArray();
	render->disableNormalArray();
	render->enableVertexArray();

	render->setVertexPointer(M_FLOAT, 3, g_vertices);
	render->drawArray(g_primitiveType, 0, g_verticesNumber);
}


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

#ifndef _M_IT_DRAW_H
#define _M_IT_DRAW_H

void drawLine(MImage * image, int x0, int y0, int x1, int y1, void * color, float radius = 1);
void drawLines(MImage * image, MVector2 * points, unsigned int nb, void * color, float radius = 1);
void drawLineCircle(MImage * image, int x0, int y0, int radius, void * color);
void drawCircle(MImage * image, const MVector2 & center, float radius, void * color);
void drawPolygon(MImage * image, MVector2 * points, unsigned int nb, void * color);

#endif

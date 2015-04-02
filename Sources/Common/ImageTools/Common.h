/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis Image tools
// Common.h
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

#ifndef _M_IT_COMMON_H
#define _M_IT_COMMON_H

// useful macros
#define isInFrame(x, y, width, height) ((int)x>=0 && (int)y>=0 && (int)x<(int)width && (int)y<(int)height)

// 8 closests pixel coords
static const int closestPixels[8][2] = {
	{-1, 0},
	{1, 0},
	{0, -1},
	{0, 1},
	{1, -1},
	{1, 1},
	{-1, 1},
	{-1, -1}
};

// test image validity
bool isImageValid(MImage * image);

// copy
bool copySubImage(MImage * source, MImage * dest, int x, int y, unsigned int w, unsigned int h);

// convertions
bool convertToFloat(MImage * image);
bool convertToUbyte(MImage * image);
bool convertToGreyscale(MImage * image, float rf=0.3f, float gf=0.5f, float bf=0.2f);
bool convertToRGB(MImage * image);
bool convertComponentToGreyscale(MImage * image, unsigned int id);
bool convertAlphaToGreyscale(MImage * image);
bool convertGreyscaleToAlpha(MImage * image);

// get interpolated image sub-pixel (if you know the image type, use getImageSubPixel_**)
void getImageSubPixel_ubyte(MImage * image, float x, float y, float * color);
void getImageSubPixel_float(MImage * image, float x, float y, float * color);
void getImageSubPixel(MImage * image, float x, float y, float * color);

#endif

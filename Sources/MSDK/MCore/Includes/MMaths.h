/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MMaths.h
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


#ifndef _M_MATHS_H
#define _M_MATHS_H

#include <math.h>

class MVector2;
class MVector3;
class MVector4;
class MColor;
class MMatrix4x4;
class MQuaternion;

// PI
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// degree/radian conversions
#define DEG_TO_RAD ((M_PI * 2) / 360.0)
#define RAD_TO_DEG (1.0 / DEG_TO_RAD)

// MIN
#ifndef MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#endif

// MAX
#ifndef MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#endif

// CLAMP
#ifndef CLAMP
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#endif

// ABS
#ifndef ABS
#define ABS(a)  (((a) < 0) ? -(a) : (a))
#endif

// power of two
#define isPowerOfTwo(x)	(((x&(x - 1)) == 0) && (x != 0))
M_CORE_EXPORT unsigned int getNextPowerOfTwo(unsigned int x);

// loop float value
M_CORE_EXPORT float loopFloat(float val, float min, float max);

// range
struct M_CORE_EXPORT MRange
{
	int start;
	int end;
};

// sort
M_CORE_EXPORT void sortFloatList(float floatList[], int start, int end);
M_CORE_EXPORT void sortFloatIndexList(int indexList[], float floatList[], int start, int end);

// 2d utils
M_CORE_EXPORT float getPolygonArea2d(MVector2 * points, unsigned int nb);
M_CORE_EXPORT float getPolyLineLength2d(MVector2 * points, unsigned int nb);

// 2d collision
M_CORE_EXPORT bool lineToLineIntersection(const MVector2 & A, const MVector2 & B, const MVector2 & C, const MVector2 & D, MVector2 * I);
M_CORE_EXPORT bool boxToBoxCollision2d(const MVector2 & minA, const MVector2 & maxA, const MVector2 & minB, const MVector2 & maxB);
M_CORE_EXPORT bool isPointInBox2d(const MVector2 & point, const MVector2 & min, const MVector2 & max);

// 3d utils
M_CORE_EXPORT void simplifyDP(float tol, MVector3 * v, int j, int k, int * mk);
M_CORE_EXPORT float getPolyLineLength(MVector3 * points, unsigned int nb);
M_CORE_EXPORT MVector3 getTriangleNormal(const MVector3 & A, const MVector3 & B, const MVector3 & C);

// 3d collision
M_CORE_EXPORT bool boxToBoxCollision(const MVector3 & minA, const MVector3 & maxA, const MVector3 & minB, const MVector3 & maxB);
M_CORE_EXPORT bool isPointInBox(const MVector3 & point, const MVector3 & min, const MVector3 & max);
M_CORE_EXPORT bool isPointInTriangle(const MVector3 & point, const MVector3 & A, const MVector3 & B, const MVector3 & C, const MVector3 & normal);
M_CORE_EXPORT bool isTriangleInBox(const MVector3 & A, const MVector3 & B, const MVector3 & C, const MVector3 & boxCenter, const MVector3 & boxHalfSize);

// 3d raytracing
M_CORE_EXPORT float raySphereIntersection(const MVector3 & origin, const MVector3 & direction, const MVector3 & sphereCenter, float sphereRadius2);
M_CORE_EXPORT float rayPlaneIntersection(const MVector3 & origin, const MVector3 & direction, const MVector3 & planePoint, const MVector3 & planeNormal);
M_CORE_EXPORT float rayBoxIntersection(const MVector3 & origin, const MVector3 & direction, const MVector3 & min, const MVector3 & max);
M_CORE_EXPORT float rayTriangleIntersection(const MVector3 & origin, const MVector3 & direction, const MVector3 & A, const MVector3 & B, const MVector3 & C, float & u, float & v);

// interpolations
M_CORE_EXPORT float linearInterpolation(float y1, float y2, float mu);
M_CORE_EXPORT float cubicInterpolation(float y0, float y1, float y2, float y3, float mu);
M_CORE_EXPORT float CatmullRomInterpolation(float y0, float y1, float y2, float y3, float mu);
M_CORE_EXPORT float HermiteInterpolation(float y0, float y1, float y2, float y3, float mu, float tension, float bias, bool begin = false, bool end = false);

// color convertions
M_CORE_EXPORT float HueToRGB(float v1, float v2, float vH);
M_CORE_EXPORT MVector3 RGBToHSV(MVector3 rgbColor);
M_CORE_EXPORT MVector3 HSVToRGB(MVector3 HSVColor);
M_CORE_EXPORT MVector3 RGBToHSL(MVector3 rgbColor);
M_CORE_EXPORT MVector3 HSLToRGB(MVector3 hslColor);

// classes
#include "MVector2.h"
#include "MVector3.h"
#include "MVector4.h"
#include "MColor.h"
#include "MMatrix4x4.h"
#include "MQuaternion.h"

#endif
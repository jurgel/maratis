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


// PI
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Degree/Radian conversions
#define DEG_TO_RAD ((M_PI * 2) / 360.0f)
#define RAD_TO_DEG (1.0f / DEG_TO_RAD)

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

// Power of two
#define isPowerOfTwo(x)	(((x&(x - 1)) == 0) && (x != 0))
M_CORE_EXPORT unsigned int getNextPowerOfTwo(unsigned int x);


// structs
struct M_CORE_EXPORT MRange
{
	int start;
	int end;
};


// classes
class MVector2;
class MVector3;
class MVector4;
class MColor;
class MMatrix4x4;
class MQuaternion;


// functions
M_CORE_EXPORT void sortFloatList(int indexList[], float floatList[], int start, int end);
M_CORE_EXPORT void sortFloatList(float floatList[], int start, int end);
M_CORE_EXPORT bool isBoxToBoxCollision(const MVector3 & minA, const MVector3 & maxA, const MVector3 & minB, const MVector3 & maxB);
M_CORE_EXPORT bool isBoxToBox2dCollision(const MVector2 & minA, const MVector2 & maxA, const MVector2 & minB, const MVector2 & maxB);
M_CORE_EXPORT bool isPointInBox(const MVector3 & point, const MVector3 & min, const MVector3 & max);
M_CORE_EXPORT bool isPointInBox2d(const MVector2 & point, const MVector2 & min, const MVector2 & max);
M_CORE_EXPORT bool isEdgeToBoxCollision(const MVector3 & origin, const MVector3 & dest, const MVector3 & min, const MVector3 & max);
M_CORE_EXPORT bool isEdgeToEdge2dIntersection(const MVector2 & A, const MVector2 & B, const MVector2 & C, const MVector2 & D, MVector2 * I);
M_CORE_EXPORT bool isPointInTriangle(const MVector3 & point, const MVector3 & a, const MVector3 & b, const MVector3 & c, const MVector3 & normal);
M_CORE_EXPORT bool isLineCircleIntersection(const MVector2 & origin, const MVector2 & dest, const MVector2 & circleCenter, float circleRadius);
M_CORE_EXPORT bool isRaySphereIntersection(const MVector3 & origin, const MVector3 & direction, const MVector3 & sphereCenter, float sphereRadius, MVector3 * point);
M_CORE_EXPORT bool isRayPlaneIntersection(const MVector3 & origin, const MVector3 & direction, const MVector3 & planePoint, const MVector3 & planeNormal, MVector3 * point);
M_CORE_EXPORT bool isEdgePlaneIntersection(const MVector3 & origin, const MVector3 & dest, const MVector3 & planePoint, const MVector3 & normal, MVector3 * point);
M_CORE_EXPORT bool isEdgeTriangleIntersection(const MVector3 & origin, const MVector3 & dest, const MVector3 & a, const MVector3 & b, const MVector3 & c, const MVector3 & normal, MVector3 * point);

M_CORE_EXPORT MVector3 getTriangleNormal(const MVector3 & a, const MVector3 & b, const MVector3 & c);


// include classes
#include "MVector2.h"
#include "MVector3.h"
#include "MVector4.h"
#include "MColor.h"
#include "MMatrix4x4.h"
#include "MQuaternion.h"

#endif
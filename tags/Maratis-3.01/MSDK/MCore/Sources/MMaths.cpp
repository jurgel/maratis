/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MCore
// MMaths.cpp
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


void sortFloatList(int indexList[], float floatList[], int start, int end)
{
	int i = start;
	int j = end;
	float middle = floatList[indexList[(start + end)/2]];
	
	while(i <= j)
	{
		while(floatList[indexList[i]] > middle) i++;
		while(floatList[indexList[j]] < middle) j--;
		
		if(i <= j)
		{
			int temp = indexList[i];
			indexList[i] = indexList[j];
			indexList[j] = temp;
			i++; 
			j--;
		}
	}
	
	if(i < end)
		sortFloatList(indexList, floatList, i, end);
	if(start < j)
		sortFloatList(indexList, floatList, start, j);
}

void sortFloatList(float floatList[], int start, int end)
{
	int i = start;
	int j = end;
	float middle = floatList[(start + end)/2];
	
	while(i <= j)
	{
		while(floatList[i] > middle) i++;
		while(floatList[j] < middle) j--;
		
		if(i <= j)
		{
			float temp = floatList[i];
			floatList[i] = floatList[j];
			floatList[j] = temp;
			i++; 
			j--;
		}
	}
	
	if(i < end)
		sortFloatList(floatList, i, end);
	if(start < j)
		sortFloatList(floatList, start, j);
}

unsigned int getNextPowerOfTwo(unsigned int x)
{
	if(x == 0)
		return 0;

	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x++;

	return x;
}

bool isEdgeToEdge2dIntersection(const MVector2 & A, const MVector2 & B, const MVector2 & C, const MVector2 & D, MVector2 * I)
{
	MVector2 DP, QA, QB;
	float d, la, lb;

	DP.x = C.x - A.x ; DP.y = C.y - A.y;
	QA.x = B.x - A.x ; QA.y = B.y - A.y;
	QB.x = D.x - C.x ; QB.y = D.y - C.y;

	d  =   QA.y * QB.x - QB.y * QA.x;
	la = ( QB.x * DP.y - QB.y * DP.x ) / d;

	if(la < 0)
		return false;

	if(la > 1)
		return false;

	lb = ( QA.x * DP.y - QA.y * DP.x ) / d;

	if(lb < 0)
		return false;

	if(lb > 1)
		return false;

	I->x = A.x + la * QA.x;
	I->y = A.y + la * QA.y;

	return true;
}

bool isLineCircleIntersection(const MVector2 & origin, const MVector2 & dest, const MVector2 & circleCenter, float circleRadius)
{
	float rr = circleRadius*circleRadius;

	if((origin - circleCenter).getSquaredLength() < rr)
		return true;

	if((dest - circleCenter).getSquaredLength() < rr)
		return true;

	MVector2 vec = dest - origin;
	MVector2 N = MVector2(-vec.y, vec.x).getNormalized();

	float d = N.dotProduct(circleCenter - origin);

	if(fabs(d) >= circleRadius)
		return false;

	MVector2 I = circleCenter - (N * d);

	if(fabs(vec.x) > fabs(vec.y))
	{
		if(((I.x > origin.x) && (I.x < dest.x)) ||
		   ((I.x < origin.x) && (I.x > dest.x)))
		   return true;

		return false;
	}

	if(((I.y > origin.y) && (I.y < dest.y)) ||
	   ((I.y < origin.y) && (I.y > dest.y)))
	   return true;

	return false;
}

bool isBoxToBoxCollision(const MVector3 & minA, const MVector3 & maxA, const MVector3 & minB, const MVector3 & maxB)
{
	// X
	if(minA.x > maxB.x) return false;
	if(maxA.x < minB.x) return false;

	// Y
	if(minA.y > maxB.y) return false;
	if(maxA.y < minB.y) return false;

	// Z
	if(minA.z > maxB.z) return false;
	if(maxA.z < minB.z) return false;

	return true;
}

bool isBoxToBox2dCollision(const MVector2 & minA, const MVector2 & maxA, const MVector2 & minB, const MVector2 & maxB)
{
	// X
	if(minA.x > maxB.x) return false;
	if(maxA.x < minB.x) return false;

	// Y
	if(minA.y > maxB.y) return false;
	if(maxA.y < minB.y) return false;

	return true;
}

bool isPointInBox(const MVector3 & point, const MVector3 & min, const MVector3 & max)
{
	if(point.x >= (min.x-0.01f) && point.x <= (max.x+0.01f) &&
	   point.y >= (min.y-0.01f) && point.y <= (max.y+0.01f) &&
	   point.z >= (min.z-0.01f) && point.z <= (max.z+0.01f))
	   return true;

	return false;
}

bool isPointInBox2d(const MVector2 & point, const MVector2 & min, const MVector2 & max)
{
	if(point.x > max.x)
		return false;

	if(point.x < min.x)
		return false;

	if(point.y > max.y)
		return false;

	if(point.y < min.y)
		return false;

	return true;
}

bool isEdgeToBoxCollision(const MVector3 & origin, const MVector3 & dest, const MVector3 & min, const MVector3 & max)
{
	MVector3 nrm[6];
	MVector3 pts[6];
	MVector3 pt;

	nrm[0].set( 1, 0, 0); pts[0] = max;
	nrm[1].set(-1, 0, 0); pts[1] = min;
	nrm[2].set( 0, 1, 0); pts[2] = max;
	nrm[3].set( 0,-1, 0); pts[3] = min;
	nrm[4].set( 0, 0, 1); pts[4] = max;
	nrm[5].set( 0, 0,-1); pts[5] = min;

	if(isPointInBox(origin, min, max) || isPointInBox(dest, min, max))
		return true;

	for(int i=0; i<6; i++)
	{
		if(isEdgePlaneIntersection(origin, dest, pts[i], nrm[i], &pt))
		{
			if(isPointInBox(pt, min, max))
				return true;
		}
	}

	return false;
}

bool isEdgePlaneIntersection(const MVector3 & origin, const MVector3 & dest, const MVector3 & planePoint, const MVector3 & normal, MVector3 * point)
{
	MVector3 vec_tmp[2], vec[2];
	float pts_dst[2], val_tmp[2];

	vec[0].x = origin.x - planePoint.x;
	vec[0].y = origin.y - planePoint.y;
	vec[0].z = origin.z - planePoint.z;

	vec[1].x = dest.x - planePoint.x;
	vec[1].y = dest.y - planePoint.y;
	vec[1].z = dest.z - planePoint.z;

	// points distance
	pts_dst[0] = vec[0].dotProduct(normal);

	if(pts_dst[0] > 0)
	{
		pts_dst[1] = vec[1].dotProduct(normal);

		if(pts_dst[1] < 0)
		{
			if(! point)
				return true;

			vec_tmp[0].x = origin.x - dest.x;
			vec_tmp[0].y = origin.y - dest.y;
			vec_tmp[0].z = origin.z - dest.z;

			val_tmp[0] = vec_tmp[0].dotProduct(normal);
			val_tmp[1] = pts_dst[0] / val_tmp[0];

			point->x = origin.x - vec_tmp[0].x * val_tmp[1];
			point->y = origin.y - vec_tmp[0].y * val_tmp[1];
			point->z = origin.z - vec_tmp[0].z * val_tmp[1];

			return true;
		}
	}

	return false;
}

bool isRaySphereIntersection(const MVector3 & origin, const MVector3 & direction, const MVector3 & sphereCenter, float sphereRadius, MVector3 * point)
{
	MVector3 vec = origin - sphereCenter;
	float b = direction.dotProduct(vec);
	float c = vec.getSquaredLength() - (sphereRadius * sphereRadius);
	float d = (b * b) - c;

	if(d < 0)
		return false;

	float distance = -b - sqrtf(d);

	point->x = (origin.x + (distance * direction.x));
	point->y = (origin.y + (distance * direction.y));
	point->z = (origin.z + (distance * direction.z));

	return true;
}

bool isRayPlaneIntersection(const MVector3 & origin, const MVector3 & direction, const MVector3 & planePoint, const MVector3 & planeNormal, MVector3 * point)
{
	float constant = - planeNormal.dotProduct(planePoint);

	float normalDotDir = planeNormal.dotProduct(direction);
	float planeDistance = planeNormal.dotProduct(origin) + constant;

	float t = - planeDistance / normalDotDir;

	point->x = (origin.x + (t * direction.x));
	point->y = (origin.y + (t * direction.y));
	point->z = (origin.z + (t * direction.z));

	return true;
}

bool isPointInTriangle(const MVector3 & point, const MVector3 & a, const MVector3 & b, const MVector3 & c, const MVector3 & normal)
{
	MVector3 nrm = getTriangleNormal(point, a, b);

	if(nrm.dotProduct(normal) < 0)
		return false;

	nrm = getTriangleNormal(point, b, c);
	if(nrm.dotProduct(normal) < 0)
		return false;

	nrm = getTriangleNormal(point, c, a);
	if(nrm.dotProduct(normal) < 0)
		return false;

	return true;
}

bool isEdgeTriangleIntersection(const MVector3 & origin, const MVector3 & dest, const MVector3 & a, const MVector3 & b, const MVector3 & c, const MVector3 & normal, MVector3 * point)
{
	MVector3 iPoint;

	if(isEdgePlaneIntersection(origin, dest, a, normal, &iPoint))
	{
		if(isPointInTriangle(iPoint, a, b, c, normal))
		{
			if(! point)
				return true;

			point->x = iPoint.x;
			point->y = iPoint.y;
			point->z = iPoint.z;
			return true;
		}
	}

	return false;
}

MVector3 getTriangleNormal(const MVector3 & a, const MVector3 & b, const MVector3 & c)
{
	MVector3 vec0 = a - b;
	MVector3 vec1 = a - c;

	return vec0.crossProduct(vec1).getNormalized();
}
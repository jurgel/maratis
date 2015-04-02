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


// following code adapted from Tomas Akenine-Moller AABB-triangle overlap test code
static bool planeBoxOverlap(const MVector3 & normal, const MVector3 & vert, const MVector3 & maxbox)
{
	MVector3 vmin, vmax;
	
	if(normal.x > 0.0f)
	{
		vmin.x = -maxbox.x - vert.x;
		vmax.x =  maxbox.x - vert.x;
	}
	else
	{
		vmin.x =  maxbox.x - vert.x;
		vmax.x = -maxbox.x - vert.x;
	}
	
	if(normal.y > 0.0f)
	{
		vmin.y = -maxbox.y - vert.y;
		vmax.y =  maxbox.y - vert.y;
	}
	else
	{
		vmin.y =  maxbox.y - vert.y;
		vmax.y = -maxbox.y - vert.y;
	}
	
	if(normal.z > 0.0f)
	{
		vmin.z = -maxbox.z - vert.z;
		vmax.z =  maxbox.z - vert.z;
	}
	else
	{
		vmin.z =  maxbox.z - vert.z;
		vmax.z = -maxbox.z - vert.z;
	}

	if(normal.dotProduct(vmin) > 0.0f)
		return false;
	
	if(normal.dotProduct(vmax) >= 0.0f)
		return true;
	
	return false;
}

#define AXISTEST_X01(a, b, fa, fb)						\
	p0 = a*v0.y - b*v0.z;								\
	p2 = a*v2.y - b*v2.z;								\
	if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;}	\
	rad = fa * boxHalfSize.y + fb * boxHalfSize.z;		\
	if(min>rad || max<-rad) return 0;

#define AXISTEST_X2(a, b, fa, fb)						\
	p0 = a*v0.y - b*v0.z;								\
	p1 = a*v1.y - b*v1.z;								\
	if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;}	\
	rad = fa * boxHalfSize.y + fb * boxHalfSize.z;		\
	if(min>rad || max<-rad) return 0;

#define AXISTEST_Y02(a, b, fa, fb)						\
	p0 = -a*v0.x + b*v0.z;								\
	p2 = -a*v2.x + b*v2.z;								\
	if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;}	\
	rad = fa * boxHalfSize.x + fb * boxHalfSize.z;		\
	if(min>rad || max<-rad) return 0;

#define AXISTEST_Y1(a, b, fa, fb)						\
	p0 = -a*v0.x + b*v0.z;								\
	p1 = -a*v1.x + b*v1.z;								\
	if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;}	\
	rad = fa * boxHalfSize.x + fb * boxHalfSize.z;		\
	if(min>rad || max<-rad) return 0;

#define AXISTEST_Z12(a, b, fa, fb)						\
	p1 = a*v1.x - b*v1.y;								\
	p2 = a*v2.x - b*v2.y;								\
	if(p2<p1) {min=p2; max=p1;} else {min=p1; max=p2;}	\
	rad = fa * boxHalfSize.x + fb * boxHalfSize.y;		\
	if(min>rad || max<-rad) return 0;

#define AXISTEST_Z0(a, b, fa, fb)						\
	p0 = a*v0.x - b*v0.y;								\
	p1 = a*v1.x - b*v1.y;								\
	if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;}	\
	rad = fa * boxHalfSize.x + fb * boxHalfSize.y;		\
	if(min>rad || max<-rad) return 0;

#define FINDMINMAX(x0, x1, x2, min, max)				\
	min = max = x0;										\
	if(x1<min) min=x1;									\
	if(x1>max) max=x1;									\
	if(x2<min) min=x2;									\
	if(x2>max) max=x2;

bool isTriangleInBox(const MVector3 & A, const MVector3 & B, const MVector3 & C, const MVector3 & boxCenter, const MVector3 & boxHalfSize)
{
	// use separating axis theorem to test overlap between triangle and box
	// need to test for overlap in these directions:
	// 1) the {x,y,z}-directions (actually, since we use the AABB of the triangle
	// we do not even need to test these)
	// 2) normal of the triangle
	// 3) crossproduct(edge from tri, {x,y,z}-directin)
	// this gives 3x3=9 more tests

	float min, max, p0, p1, p2, rad, fex, fey, fez;
	MVector3 normal, e0, e1, e2;
	MVector3 v0, v1, v2;

	// move everything so that the boxCenter is in (0, 0, 0)
	v0 = A - boxCenter;
	v1 = B - boxCenter;
	v2 = C - boxCenter;

	// compute triangle edges
	e0 = v1 - v0;
	e1 = v2 - v1;
	e2 = v0 - v2;

	// Bullet 3:
	// test the 9 tests first (this was faster)
	fex = fabsf(e0.x);
	fey = fabsf(e0.y);
	fez = fabsf(e0.z);

	AXISTEST_X01(e0.z, e0.y, fez, fey);
	AXISTEST_Y02(e0.z, e0.x, fez, fex);
	AXISTEST_Z12(e0.y, e0.x, fey, fex);
	
	fex = fabsf(e1.x);
	fey = fabsf(e1.y);
	fez = fabsf(e1.z);
	
	AXISTEST_X01(e1.z, e1.y, fez, fey);
	AXISTEST_Y02(e1.z, e1.x, fez, fex);
	AXISTEST_Z0 (e1.y, e1.x, fey, fex);
	
	fex = fabsf(e2.x);
	fey = fabsf(e2.y);
	fez = fabsf(e2.z);
	
	AXISTEST_X2 (e2.z, e2.y, fez, fey);
	AXISTEST_Y1 (e2.z, e2.x, fez, fex);
	AXISTEST_Z12(e2.y, e2.x, fey, fex);

	// Bullet 1:
	// first test overlap in the {x,y,z}-directions
	// find min, max of the triangle each direction, and test for overlap in
	// that direction -- this is equivalent to testing a minimal AABB around
	// the triangle against the AABB

	// test in X-direction
	FINDMINMAX(v0.x, v1.x, v2.x, min, max);
	if(min>boxHalfSize.x || max<-boxHalfSize.x) return false;

	// test in Y-direction
	FINDMINMAX(v0.y, v1.y, v2.y, min, max);
	if(min>boxHalfSize.y || max<-boxHalfSize.y) return false;

	// test in Z-direction
	FINDMINMAX(v0.z, v1.z, v2.z, min, max);
	if(min>boxHalfSize.z || max < -boxHalfSize.z) return false;

	// Bullet 2:
	// test if the box intersects the plane of the triangle
	// compute plane equation of triangle: normal*x+d=0
	normal = e0.crossProduct(e1);

	if(! planeBoxOverlap(normal, v0, boxHalfSize)) return false;

	return true; // box and triangle overlaps
}

void sortFloatIndexList(int indexList[], float floatList[], int start, int end)
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
			
			int itemp = indexList[i];
			indexList[i] = indexList[j];
			indexList[j] = itemp;
			
			i++; 
			j--;
		}
	}
	
	if(i < end)
		sortFloatIndexList(indexList, floatList, i, end);
	if(start < j)
		sortFloatIndexList(indexList, floatList, start, j);
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

float loopFloat(float val, float min, float max)
{
	if(val >= min && val < max)
		return val;
	
	float d = max - min;
	if(d <= 0)
		return min;
	
	float v = (val - min) / d;
	v = v - (float)((int)v);
	
	if(v < 0)
		return max + (d*v);
	else
		return min + (d*v);
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

bool lineToLineIntersection(const MVector2 & A, const MVector2 & B, const MVector2 & C, const MVector2 & D, MVector2 * I)
{
	MVector2 DP, QA, QB;
	float d, la, lb;

	DP.x = C.x - A.x ; DP.y = C.y - A.y;
	QA.x = B.x - A.x ; QA.y = B.y - A.y;
	QB.x = D.x - C.x ; QB.y = D.y - C.y;

	d = QA.y * QB.x - QB.y * QA.x;
	if(d == 0)
		return false;
		
	la = (QB.x * DP.y - QB.y * DP.x) / d;

	if(la < 0 || la > 1)
		return false;

	lb = (QA.x * DP.y - QA.y * DP.x) / d;

	if(lb < 0 || lb > 1)
		return false;

	if(I)
	{
		I->x = A.x + la * QA.x;
		I->y = A.y + la * QA.y;
	}
	return true;
}

bool boxToBoxCollision2d(const MVector2 & minA, const MVector2 & maxA, const MVector2 & minB, const MVector2 & maxB)
{
	if(minA.x > maxB.x) return false;
	if(maxA.x < minB.x) return false;
	if(minA.y > maxB.y) return false;
	if(maxA.y < minB.y) return false;
	return true;
}

bool isPointInBox2d(const MVector2 & point, const MVector2 & min, const MVector2 & max)
{
	if(point.x > max.x) return false;
	if(point.x < min.x) return false;
	if(point.y > max.y) return false;
	if(point.y < min.y) return false;
	return true;
}

bool boxToBoxCollision(const MVector3 & minA, const MVector3 & maxA, const MVector3 & minB, const MVector3 & maxB)
{
	if(minA.x > maxB.x) return false;
	if(maxA.x < minB.x) return false;
	if(minA.y > maxB.y) return false;
	if(maxA.y < minB.y) return false;
	if(minA.z > maxB.z) return false;
	if(maxA.z < minB.z) return false;
	return true;
}

bool isPointInBox(const MVector3 & point, const MVector3 & min, const MVector3 & max)
{
	if(point.x > max.x) return false;
	if(point.x < min.x) return false;
	if(point.y > max.y) return false;
	if(point.y < min.y) return false;
	if(point.z > max.z) return false;
	if(point.z < min.z) return false;
	return true;
}

// Ray-box intersection using IEEE numerical properties to ensure that the
// test is both robust and efficient, as described in:
//
//      Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
//      "An Efficient and Robust Ray-Box Intersection Algorithm"
//      Journal of graphics tools, 10(1):49-54, 2005
//
float rayBoxIntersection(const MVector3 & origin, const MVector3 & direction, const MVector3 & min, const MVector3 & max)
{
	MVector3 inv_direction(
		1 / direction.x,
		1 / direction.y,
		1 / direction.z
	);

	int sign[3];
	sign[0] = (inv_direction.x < 0);
	sign[1] = (inv_direction.y < 0);
	sign[2] = (inv_direction.z < 0);
	MVector3 parameters[2] = {min, max};
	
	float tmin = (parameters[sign[0]].x - origin.x) * inv_direction.x;
	float tmax = (parameters[1-sign[0]].x - origin.x) * inv_direction.x;
	float tymin = (parameters[sign[1]].y - origin.y) * inv_direction.y;
	float tymax = (parameters[1-sign[1]].y - origin.y) * inv_direction.y;
	
	if((tmin > tymax) || (tymin > tmax))
		return 0;
  
	if(tymin > tmin)
		tmin = tymin;
	if(tymax < tmax)
		tmax = tymax;
		
	float tzmin = (parameters[sign[2]].z - origin.z) * inv_direction.z;
	float tzmax = (parameters[1-sign[2]].z - origin.z) * inv_direction.z;

	if((tmin > tzmax) || (tzmin > tmax))
		return 0;
		
	if(tzmin > tmin)
		tmin = tzmin;
	if(tzmax < tmax)
		tmax = tzmax;
	
	return tmin;
}

float rayPlaneIntersection(const MVector3 & origin, const MVector3 & direction, const MVector3 & planePoint, const MVector3 & planeNormal)
{
	float constant = - planeNormal.dotProduct(planePoint);
	float normaldotProductDir = planeNormal.dotProduct(direction);
	float planeDistance = planeNormal.dotProduct(origin) + constant;
	return - planeDistance / normaldotProductDir;
}

float raySphereIntersection(const MVector3 & origin, const MVector3 & direction, const MVector3 & sphereCenter, float sphereRadius2)
{
	MVector3 vec = sphereCenter - origin;
	float b = vec.dotProduct(direction);
	float det = b*b - vec.dotProduct(vec) + sphereRadius2;

	if(det<0)
		return 0;
	else
		det=sqrtf(det);
	
	return b-det;
}

float rayTriangleIntersection(const MVector3 & origin, const MVector3 & direction, const MVector3 & A, const MVector3 & B, const MVector3 & C, float & u, float & v)
{
	MVector3 edge1 = B - A;
	MVector3 edge2 = C - A;
	MVector3 pvec = direction.crossProduct(edge2);
	
	float det = edge1.dotProduct(pvec);
	if(det == 0)
		return 0;
		
	float inv_det = 1.0f/det;
	MVector3 tvec = origin - A;
	
	// u parameter
	u = tvec.dotProduct(pvec) * inv_det;
	if(u < 0.0 || u > 1.0)
		return 0;
		
	MVector3 qvec = tvec.crossProduct(edge1);
	
	// v parameter
	v = direction.dotProduct(qvec) * inv_det;
	
	// inverted comparison (to catch NaNs)
	if(v >= 0.0 && (u+v) <= 1.0)
	{
		return edge2.dotProduct(qvec) * inv_det;
	}
	
	return 0;
}

bool isPointInTriangle(const MVector3 & point, const MVector3 & A, const MVector3 & B, const MVector3 & C, const MVector3 & normal)
{
	MVector3 nrm = (point-A).crossProduct(point-B);
	if(nrm.dotProduct(normal) < 0)
		return false;

	nrm = (point-B).crossProduct(point-C);
	if(nrm.dotProduct(normal) < 0)
		return false;

	nrm = (point-C).crossProduct(point-A);
	if(nrm.dotProduct(normal) < 0)
		return false;

	return true;
}

MVector3 getTriangleNormal(const MVector3 & A, const MVector3 & B, const MVector3 & C)
{
	MVector3 vec1 = A - B;
	MVector3 vec2 = A - C;
	return vec1.crossProduct(vec2).getNormalized();
}

float getPolygonArea2d(MVector2 * points, unsigned int nb)
{
	float fx = 0;
	float fy = 0;
			 
	unsigned int p;
	for(p=0; p<nb; p++)
	{
		int nextp = p+1;
		if(nextp >= nb)
			nextp = 0;
					
		fx += points[p].x * points[nextp].y;
		fy += points[p].y * points[nextp].x;
	}
	
	return ABS((fx - fy)*0.5f);
}

float getPolyLineLength2d(MVector2 * points, unsigned int nb)
{
	float L = 0;
				
	unsigned int p;
	for(p=1; p<nb; p++)
		L += (points[p] - points[p-1]).getLength();
	
	return L;
}

float getPolyLineLength(MVector3 * points, unsigned int nb)
{
	float L = 0;
				
	unsigned int p;
	for(p=1; p<nb; p++)
		L += (points[p] - points[p-1]).getLength();
	
	return L;
}

float linearInterpolation(float y1, float y2, float mu)
{
	return (y1 + (y2 - y1)*mu);
}

float cubicInterpolation(float y0, float y1, float y2, float y3, float mu)
{
	float a0,a1,a2,a3,mu2;
	
	mu2 = mu*mu;
	
	a0 = y3 - y2 - y0 + y1;
	a1 = y0 - y1 - a0;
	a2 = y2 - y0;
	a3 = y1;
	
	return (a0*mu*mu2+a1*mu2+a2*mu+a3);
}

float CatmullRomInterpolation(float y0, float y1, float y2, float y3, float mu)
{
	float a0,a1,a2,a3,mu2;
	
	mu2 = mu*mu;
	
	a0 = -0.5f*y0 + 1.5f*y1 - 1.5f*y2 + 0.5f*y3;
	a1 = y0 - 2.5f*y1 + 2.0f*y2 - 0.5f*y3;
	a2 = -0.5f*y0 + 0.5f*y2;
	a3 = y1;
	
	return (a0*mu*mu2+a1*mu2+a2*mu+a3);
}

float HermiteInterpolation(float y0, float y1, float y2, float y3, float mu, float tension, float bias, bool begin, bool end)
{
	float m0,m1,mu2,mu3;
	float a0,a1,a2,a3;
	
	mu2 = mu * mu;
	mu3 = mu2 * mu;
	m0 = (y1-y0)*(1+bias)*(1-tension)*0.5f;
	m1 = (y3-y2)*(1-bias)*(1-tension)*0.5f;
	
	if(!begin)
		m0 += (y2-y1)*(1-bias)*(1-tension)*0.5f;
		
	if(!end)
		m1 += (y2-y1)*(1+bias)*(1-tension)*0.5f;
	
	a0 =  2*mu3 - 3*mu2 + 1;
	a1 =    mu3 - 2*mu2 + mu;
	a2 =    mu3 -   mu2;
	a3 = -2*mu3 + 3*mu2;
	
	return(a0*y1+a1*m0+a2*m1+a3*y2);
}

MVector3 RGBToHSV(MVector3 rgbColor)
{
	float r = rgbColor.x;
	float g = rgbColor.y;
	float b = rgbColor.z;
	
	float h = 0;
	float s = 0;
	float v = 0;
	float min, max, delta;
	
	min = r;
	if(g < min) min = g;
	if(b < min) min = b;
	
	max = r;
	if(g > max) max = g;
	if(b > max) max = b;
	
	v = max;
	
	delta = max - min;
	
	if(delta == 0)
		return MVector3(h, s, v);
	
	if(max != 0)
		s = delta/max;
	else
		return MVector3(h, s, v);
	
	if(r == max)
		h = (g - b) / delta;
	else if(g == max)
		h = 2 + (b - r) / delta;
	else
		h = 4 + (r - g) / delta;
	
	h *= 60;
	if(h < 0)
		h += 360;
	
	return MVector3(h, s, v);
}

MVector3 HSVToRGB(MVector3 HSVColor)
{
	int i;
	float r, g, b;
	float f, p, q, t;
	
	float h = HSVColor.x;
	float s = HSVColor.y;
	float v = HSVColor.z;
	
	if(s == 0)
	{
		r = g = b = v;
		return MVector3(r, g, b);
	}
	
	h /= 60;
	i = (int)floor(h);
	f = h - i;
	p = v * (1 - s);
	q = v * (1 - s * f);
	t = v * (1 - s * (1 - f));
	
	switch(i)
	{
		case 0:
			r = v;
			g = t;
			b = p;
			break;
		case 1:
			r = q;
			g = v;
			b = p;
			break;
		case 2:
			r = p;
			g = v;
			b = t;
			break;
		case 3:
			r = p;
			g = q;
			b = v;
			break;
		case 4:
			r = t;
			g = p;
			b = v;
			break;
		default:
			r = v;
			g = p;
			b = q;
			break;
	}
	
	return MVector3(r, g, b);
}

MVector3 RGBToHSL(MVector3 rgbColor)
{
	float varMin = rgbColor.x;
	if(rgbColor.y < varMin)
		varMin = rgbColor.y;
	if(rgbColor.z < varMin)
		varMin = rgbColor.z;
	
	float varMax = rgbColor.x;
	if(rgbColor.y > varMax)
		varMax = rgbColor.y;
	if(rgbColor.z > varMax)
		varMax = rgbColor.z;
	
	float delMax = varMax - varMin;
	float L = (varMax + varMin) / 2;
	float H = 0, S = 0;
	
	if(delMax == 0)
		return MVector3(H, S, L);
	
	if(L < 0.5f)
		S = delMax / (varMax + varMin);
	else
		S = delMax / (2 - varMax - varMin);
	
	float delR = (((varMax - rgbColor.x) / 6.0f) + (delMax / 2.0f) ) / delMax;
	float delG = (((varMax - rgbColor.y) / 6.0f) + (delMax / 2.0f) ) / delMax;
	float delB = (((varMax - rgbColor.z) / 6.0f) + (delMax / 2.0f) ) / delMax;
	
	if(rgbColor.x == varMax)
		H = delB - delG;
	else if(rgbColor.y == varMax)
		H = (1.0f / 3.0f) + delR - delB;
	else if(rgbColor.z == varMax)
		H = (2.0f / 3.0f) + delG - delR;
	
	if(H < 0)
		H += 1;
	if(H > 1)
		H -= 1;
	
	return MVector3(H, S, L);
}

float HueToRGB(float v1, float v2, float vH)
{
	if(vH < 0)
		vH += 1;
	
	if(vH > 1)
		vH -= 1;
	
	if((6 * vH) < 1)
		return (v1 + (v2 - v1) * 6 * vH);
	
	if((2 * vH) < 1)
		return v2;
	
	if((3 * vH) < 2)
		return (v1 + (v2 - v1) * ((2.0f / 3.0f) - vH) * 6);
	
	return v1;
}

MVector3 HSLToRGB(MVector3 hslColor)
{
	float H = hslColor.x;
	float S = hslColor.y;
	float L = hslColor.z;
	
	if(S == 0)
		return MVector3(L, L, L);
	
	float var1, var2;
	
	if(L < 0.5f)
		var2 = L * (1 + S);
	else
		var2 = (L + S) - (S * L);
	
	var1 = 2 * L - var2;
	
	float R = HueToRGB(var1, var2, H + (1.0f / 3.0f));
	float G = HueToRGB(var1, var2, H);
	float B = HueToRGB(var1, var2, H - (1.0f / 3.0f));
	
	return MVector3(R, G, B);
}

// Copyright 2002, softSurfer (www.softsurfer.com)
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.
// Modified to use MVector3
//
//  This is the Douglas-Peucker recursive simplification routine
//  It just marks vertices that are part of the simplified polyline
//  for approximating the polyline subchain v[j] to v[k].
//    Input:  tol = approximation tolerance
//            v[] = polyline array of vertex points
//            j,k = indices for the subchain v[j] to v[k]
//    Output: mk[] = array of markers matching vertex array v[]

void simplifyDP(float tol, MVector3 * v, int j, int k, int * mk)
{
    if(k <= j+1) // there is nothing to simplify
        return;
	
    // check for adequate approximation by segment S from v[j] to v[k]
    int			maxi = j;					// index of vertex farthest from S
    float		maxd2 = 0;					// distance squared of farthest vertex
    float		tol2 = tol * tol;			// tolerance squared
    MVector3	S[2] = {v[j], v[k]};		// segment from v[j] to v[k]
    MVector3	u = S[1] - S[0];			// segment direction vector
    float		cu = u.getSquaredLength();	// segment length squared
	
    // test each vertex v[i] for max distance from S
    // compute using the Feb 2001 Algorithm's dist_Point_to_Segment()
    // Note: this works in any dimension (2D, 3D, ...)
    MVector3 w;
    MVector3 Pb;               // base of perpendicular from v[i] to S
    float  b, cw, dv2;        // dv2 = distance v[i] to S squared
	
    for(int i=j+1; i<k; i++)
    {
        // compute distance squared
        w = v[i] - S[0];
        cw = w.dotProduct(u);
        if(cw <= 0)
            dv2 = (v[i] - S[0]).getSquaredLength();
        else if ( cu <= cw )
            dv2 = (v[i] - S[1]).getSquaredLength();
        else {
            b = cw / cu;
            Pb = S[0] + b * u;
            dv2 = (v[i] - Pb).getSquaredLength();
        }
		
        // test with current max distance squared
        if (dv2 <= maxd2)
            continue;
		
        // v[i] is a new max vertex
        maxi = i;
        maxd2 = dv2;
    }
	
    if(maxd2 > tol2) // error is worse than the tolerance
    {
        // split the polyline at the farthest vertex from S
        mk[maxi] = 1;      // mark v[maxi] for the simplified polyline
		
        // recursively simplify the two subpolylines at v[maxi]
        simplifyDP( tol, v, j, maxi, mk );  // polyline v[j] to v[maxi]
        simplifyDP( tol, v, maxi, k, mk );  // polyline v[maxi] to v[k]
    }
	
    // else the approximation is OK, so ignore intermediate vertices
    return;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MEngine
// MBox3d.cpp
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


#include "../Includes/MEngine.h"


MBox3d::MBox3d(void)
{}

MBox3d::MBox3d(const MBox3d & box):
m_min(box.m_min),
m_max(box.m_max)
{}

MBox3d::MBox3d(const MVector3 & min, const MVector3 & max):
m_min(min),
m_max(max)
{}

bool MBox3d::isInCollisionWith(MBox3d * box)
{
	if(! isBoxToBoxCollision(m_min, m_max, *box->getMin(), *box->getMax()))
		return false;

	return true;
}

void MBox3d::initFromPoints(const MVector3 * points, unsigned int pointsNumber)
{
	if(pointsNumber > 0)
	{
		MVector3 * min = getMin();
		MVector3 * max = getMax();

		const MVector3 * point = &points[0];
		(*min) = (*max) = (*point);

		unsigned int i;
		for(i=1; i<pointsNumber; i++)
		{
			point = &points[i];

			min->x = MIN(min->x, point->x);
			min->y = MIN(min->y, point->y);
			min->z = MIN(min->z, point->z);

			max->x = MAX(max->x, point->x);
			max->y = MAX(max->y, point->y);
			max->z = MAX(max->z, point->z);
		}
	}
}
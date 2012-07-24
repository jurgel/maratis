/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MPublishEvent.h
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//========================================================================
// Copyright (c) 2012 Philipp Geyer <nistur.com>
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

#ifndef __M_PUBLISH_EVENT_H__
#define __M_PUBLISH_EVENT_H__

// base command class for publish events.
class MPublishEvent
{
public:
	virtual void	execute(const char* projName) = 0;
	virtual int		getPriority() = 0;
};

#define M_PUBLISH_EVENT_IMPLEMENT(eventName) \
class eventName##AutoAdd \
{ \
public: \
	eventName##AutoAdd() \
	{ \
		MPublisher* pub = MPublisher::getInstance(); \
		pub->addEvent(new eventName); \
	} \
}; \
static eventName##AutoAdd s_##eventName;

#endif /*__M_PUBLISH_EVENT_H__*/
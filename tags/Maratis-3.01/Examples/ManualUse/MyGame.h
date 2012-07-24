/////////////////////////////////////////////////////////////////////////////////////////////////////////
// ManualUse example
// MyGame.h
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

// This game example is showing how to manually create a scene and load objects
// it can also be used in Maratis as game plugin (like WaterGameDemo)


#ifndef _MY_GAME_H
#define _MY_GAME_H

#include <MEngine.h>


class MyGame : public MGame
{
public:

	// constructors / destructors
	MyGame(void);
	~MyGame(void);

public:

	// events (virtual from MGame class)
	void onBegin(void);
	void update(void);
	//void draw(void);
};

#endif
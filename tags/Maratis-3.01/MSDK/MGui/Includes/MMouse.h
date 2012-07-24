/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MMouse.h
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


#ifndef _M_MOUSE_H
#define _M_MOUSE_H


class MMouse
{
public:

	MMouse(void)
	{
		m_wheelDirection = 0;
		m_buttons[0] = m_buttons[1] = m_buttons[2] = false;
		m_position[0] = m_position[1] = 0;
		m_direction[0] = m_direction[1] = 0;
		m_lastPosition[0] = m_lastPosition[1] = 0;
	}

private:

	bool m_buttons[3];
	
	int	m_position[2];
	int	m_direction[2];
	int	m_wheelDirection;
	int	m_lastPosition[2];

public:

	// instance
	static MMouse * getInstance()
	{
		static MMouse m_instance;
		return &m_instance;
	};

	inline void refreshLastPosition(){ m_lastPosition[0] = m_position[0]; m_lastPosition[1] = m_position[1]; }

	void setPosition(const int x, const int y);
	inline void setDirection(const int x, const int y){ m_direction[0] = x; m_direction[1] = y; }
	inline void setWheelDirection(const int w){ m_wheelDirection = w; }

	inline bool isLeftButtonPushed()	{ return m_buttons[0]; }
	inline bool isMiddleButtonPushed()	{ return m_buttons[1]; }
	inline bool isRightButtonPushed()	{ return m_buttons[2]; }

	inline void downButton(const unsigned int id){ 
		switch(id)
		{
		case 0: m_buttons[0] = true; break;
		case 1: m_buttons[1] = true; break;
		case 2: m_buttons[2] = true; break;
		}
	}
	inline void upButton(const unsigned int id){ 
		switch(id)
		{
		case 0: m_buttons[0] = false; break;
		case 1: m_buttons[1] = false; break;
		case 2: m_buttons[2] = false; break;
		}
	}

	inline int getXPosition(){ return m_position[0]; }
	inline int getYPosition(){ return m_position[1]; }
	inline int getXDirection(){ return m_direction[0]; }
	inline int getYDirection(){ return m_direction[1]; }
	inline int getWheelDirection(){ return m_wheelDirection; }
};

#endif
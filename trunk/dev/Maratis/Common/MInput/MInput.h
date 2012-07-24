/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MInput.h
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


#ifndef _M_INPUT_H
#define _M_INPUT_H

#include <map>
#include <MEngine.h>

class MVector2;

class MInput : public MInputContext
{
public :

	MInput(void);
	~MInput(void);

private :

    struct TouchData
    {
        MTouchPhase phase;
        MVector2 touchPoint;
        MVector2 lastTouchPoint;
        
        TouchData()
        {
            phase = MTouchPhaseNone;
            touchPoint.loadIdentity();
            lastTouchPoint.loadIdentity();
        }
    };
    
	map<string, int> m_keys;
	map<string, float> m_axis;
	map<string, int> m_props;
    map<string, MVector3> m_vecprops;
    map<int, TouchData> m_touches;

public:

	void createKey(const char * name);
	void createAxis(const char * name);
	void createProperty(const char * name);
    void createVectorProperty(const char * name);

	void downKey(const char * name);
	void upKey(const char * name);
	void setAxis(const char * name, float axis);
	void setProperty(const char * name, int prop);
    void setVectorProperty(const char * name, MVector3 vec);

	bool isKeyPressed(const char * name);
	bool onKeyDown(const char * name);
	bool onKeyUp(const char * name);
	float getAxis(const char * name);
	int getProperty(const char * name);
    MVector3 getVectorProperty(const char * name);
    
    // Multi-Touch Support
    // Touch events
    void beginTouch(int touchID, MVector2 touchPoint);
    void updateTouch(int touchID, MVector2 touchPoint);
    void endTouch(int touchID, MVector2 touchPoint);
    void cancelTouch(int touchID, MVector2 touchPoint);
    
    // Get Touch data
    MVector2 getTouchPosition(int touchID);
    MVector2 getLastTouchPosition(int touchID);
    MTouchPhase getTouchPhase(int touchID);

	void flush(void);
};

#endif
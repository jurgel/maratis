/////////////////////////////////////////////////////////////////////////////////////////////////////////
// MGui
// MWin32Window.h
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


#ifndef _M_WIN32_WINDOW_H
#define _M_WIN32_WINDOW_H

#include <MCore.h>
#include <windows.h>
#include <list>


class MWindow
{
private:

	// windows contexts
	HDC m_hDC;
	HWND m_hWnd;
	HGLRC m_hRC;
	HINSTANCE m_hInstance;

	// infos
	bool m_focus;
	bool m_active;
	bool m_fullscreen;

	int	m_position[2];
	unsigned int m_width;
	unsigned int m_height;
	unsigned int m_colorBits;

	char m_title[256];
	char m_workingDirectory[256];

	// joysticks
	MJoystick m_joysticks[2];

	// events function pointer
	void (* m_pointerEvent)(MWinEvent * windowEvent);

public:

	MWindow(void);
	~MWindow(void);

	// instance
	static MWindow * getInstance(void)
	{
		static MWindow m_instance;
		return &m_instance;
	};

public:

	// create
	bool create(const char * title, unsigned int width, unsigned int height, int colorBits, bool fullscreen);

	// events
	inline void setPointerEvent(void (*pointerEvent)(MWinEvent * windowEvent)){ m_pointerEvent = pointerEvent; }
	void sendEvents(MWinEvent * events);
	bool onEvents(void);

	// cursor
	void setCursorPos(int x, int y);
	void hideCursor(void);
	void showCursor(void);

	// properties
	bool getFocus(void){ return m_focus; }
	inline bool isActive(void){ return m_active; }
	inline bool isFullscreen(void){ return m_fullscreen; }

	void setTitle(const char * title);
	void setFullscreen(bool fullscreen);
	void setFocus(bool focus){ m_focus = focus; }
	inline void setActive(bool active){ m_active = active; }

	// coords
	bool isMouseOverWindow(void);

	inline int getXPosition(void){ return m_position[0]; }
	inline int getYPosition(void){ return m_position[1]; }
	inline unsigned int getWidth(void){ return m_width; }
	inline unsigned int getHeight(void){ return m_height; }

	// swap buffer
	void swapBuffer(void);

	// system tick
	static unsigned long getSystemTick(void);

	// directories
	void setCurrentDirectory(const char * directory);
	void setWorkingDirectory(const char * directory);

	const char * getTempDirectory(void);
	const char * getCurrentDirectory(void);
	inline const char * getWorkingDirectory(void){ return m_workingDirectory; }

	// joysticks
	MJoystick * getJoystick1(void){ return &m_joysticks[0]; }
	MJoystick * getJoystick2(void){ return &m_joysticks[1]; }

	// execute
	void execute(const char * path, const char * args);

	// files parsing
    bool getOpenMultipleFiles(const char * title, const char * filter, string * repertory, list <string> * filesList);
    const char * getOpenDir(const char * title, const char * startPath = NULL);
	const char * getOpenFilename(const char * title, const char * filter, const char * startPath = NULL);
    const char * getSaveFilename(const char * title, const char * filter, const char * startPath = NULL);
};

#define MKEY_BACKSPACE	VK_BACK
#define MKEY_TAB		VK_TAB
#define MKEY_CLEAR		VK_CLEAR
#define MKEY_RETURN		VK_RETURN
#define MKEY_PAUSE		VK_PAUSE
#define MKEY_ESCAPE		VK_ESCAPE
#define MKEY_SPACE		VK_SPACE
#define MKEY_DELETE		VK_DELETE
#define MKEY_KP0		VK_NUMPAD0
#define MKEY_KP1		VK_NUMPAD1
#define MKEY_KP2		VK_NUMPAD2
#define MKEY_KP3		VK_NUMPAD3
#define MKEY_KP4		VK_NUMPAD4
#define MKEY_KP5		VK_NUMPAD5
#define MKEY_KP6		VK_NUMPAD6
#define MKEY_KP7		VK_NUMPAD7
#define MKEY_KP8		VK_NUMPAD8
#define MKEY_KP9		VK_NUMPAD9
#define MKEY_KP_ENTER	VK_RETURN
#define MKEY_UP			VK_UP
#define MKEY_DOWN		VK_DOWN
#define MKEY_RIGHT		VK_RIGHT
#define MKEY_LEFT		VK_LEFT
#define MKEY_INSERT		VK_INSERT
#define MKEY_HOME		VK_HOME
#define MKEY_END		VK_END
#define MKEY_PAGEUP		VK_PRIOR
#define MKEY_PAGEDOWN	VK_NEXT
#define MKEY_F1			VK_F1
#define MKEY_F2			VK_F2
#define MKEY_F3			VK_F3
#define MKEY_F4			VK_F4
#define MKEY_F5			VK_F5
#define MKEY_F6			VK_F6
#define MKEY_F7			VK_F7
#define MKEY_F8			VK_F8
#define MKEY_F9			VK_F9
#define MKEY_F10		VK_F10
#define MKEY_F11		VK_F11
#define MKEY_F12		VK_F12
#define MKEY_NUMLOCK	VK_NUMLOCK
#define MKEY_RSHIFT		VK_RSHIFT
#define MKEY_LSHIFT		VK_LSHIFT
#define MKEY_SHIFT		VK_SHIFT
#define MKEY_RCONTROL	VK_RCONTROL
#define MKEY_LCONTROL	VK_LCONTROL
#define MKEY_CONTROL	VK_CONTROL
#define MKEY_RALT		17
#define MKEY_LALT		18
#define MKEY_ADD		107
#define MKEY_SUB		109
#define MKEY_HELP		VK_HELP
#define MKEY_PRINT		VK_PRINT
#define MKEY_MENU		VK_MENU

#endif

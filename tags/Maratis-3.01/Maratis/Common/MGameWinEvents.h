/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Maratis
// MGameWinEvents.h
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


#ifndef _M_GAME_WIN_EVENTS_H
#define _M_GAME_WIN_EVENTS_H


const char * getKeyName(unsigned int key)
{
	// ascii
	if(key >= 65 && key <= 90)
	{
		static char name[2] = {0, 0};
		name[0] = (char)key;
		return name;
	}

	// keyboard keys
	switch(key)
	{
	case MKEY_BACKSPACE: return "BACKSPACE";
	case MKEY_TAB:		 return "TAB";
	case MKEY_ESCAPE:	 return "ESCAPE";
	case MKEY_SPACE:	 return "SPACE";
	case MKEY_DELETE:	 return "DELETE";

	case MKEY_KP_ENTER:	 return "ENTER";
	case MKEY_UP:		 return "UP";
	case MKEY_DOWN:		 return "DOWN";
	case MKEY_LEFT:		 return "LEFT";
	case MKEY_RIGHT:	 return "RIGHT";
	case MKEY_F1:		 return "F1";
	case MKEY_F2:		 return "F2";
	case MKEY_F3:		 return "F3";
	case MKEY_F4:		 return "F4";
	case MKEY_F5:		 return "F5";
	case MKEY_F6:		 return "F6";
	case MKEY_F7:		 return "F7";
	case MKEY_F8:		 return "F8";
	case MKEY_F9:		 return "F9";
	case MKEY_F10:		 return "F10";
	case MKEY_F11:		 return "F11";
	case MKEY_F12:		 return "F12";
	case MKEY_RSHIFT:	 return "RSHIFT";
	case MKEY_LSHIFT:	 return "LSHIFT";
	case MKEY_RCONTROL:	 return "RCONTROL";
	case MKEY_LCONTROL:	 return "LCONTROL";
	case MKEY_RALT:		 return "RALT";
	case MKEY_LALT:		 return "LALT";

	case MKEY_KP0: return "0";
	case MKEY_KP1: return "1";
	case MKEY_KP2: return "2";
	case MKEY_KP3: return "3";
	case MKEY_KP4: return "4";
	case MKEY_KP5: return "5";
	case MKEY_KP6: return "6";
	case MKEY_KP7: return "7";
	case MKEY_KP8: return "8";
	case MKEY_KP9: return "9";
	}

	return NULL;
}

void gameWinEvents(MWinEvent * windowEvents)
{
	MWindow * window = MWindow::getInstance();
	MInputContext * input = MEngine::getInstance()->getInputContext();
	MMouse * mouse = MMouse::getInstance();
	MKeyboard * keyboard = MKeyboard::getInstance();

	switch(windowEvents->type)
	{
	case MWIN_EVENT_MOUSE_BUTTON_DOWN:
		switch(windowEvents->data[0])
		{
		case MMOUSE_BUTTON_LEFT:
			input->downKey("MOUSE_BUTTON1");
			break;
		case MMOUSE_BUTTON_MIDDLE:
			input->downKey("MOUSE_BUTTON2");
			break;
		case MMOUSE_BUTTON_RIGHT:
			input->downKey("MOUSE_BUTTON3");
			break;
		}
		break;

	case MWIN_EVENT_MOUSE_BUTTON_UP:
		switch(windowEvents->data[0])
		{
		case MMOUSE_BUTTON_LEFT:
			input->upKey("MOUSE_BUTTON1");
			break;
		case MMOUSE_BUTTON_MIDDLE:
			input->upKey("MOUSE_BUTTON2");
			break;
		case MMOUSE_BUTTON_RIGHT:
			input->upKey("MOUSE_BUTTON3");
			break;
		}
		break;

	case MWIN_EVENT_MOUSE_MOVE:
		input->setAxis("MOUSE_X", (float)(mouse->getXPosition() / (float)window->getWidth()));
		input->setAxis("MOUSE_Y", (float)(mouse->getYPosition() / (float)window->getHeight()));
		break;

	case MWIN_EVENT_KEY_DOWN:
		{
			unsigned int key = windowEvents->data[0];
			const char * name = getKeyName(key);
			printf("key %d %s\n", key, name);
			if(name)
				input->downKey(name);
		}
		break;

	case MWIN_EVENT_KEY_UP:
		{
			unsigned int key = windowEvents->data[0];
			const char * name = getKeyName(key);
			if(name)
				input->upKey(name);
		}
		break;

	case MWIN_EVENT_JOYSTICK1_UPDATE:
		{
			MJoystick * joystick = window->getJoystick1();

			char name[32];
			for(int i=0; i<8; i++)
			{
				if(joystick->onKeyDown(i))
				{
					sprintf(name, "JOY1_BUTTON%d", i+1);
					input->downKey(name);
				}
				else if(joystick->onKeyUp(i))
				{
					sprintf(name, "JOY1_BUTTON%d", i+1);
					input->upKey(name);
				}
			}

			input->setAxis("JOY1_X", joystick->getX());
			input->setAxis("JOY1_Y", joystick->getY());
			input->setAxis("JOY1_Z", joystick->getZ());
			input->setAxis("JOY1_R", joystick->getR());
			input->setAxis("JOY1_U", joystick->getU());
			input->setAxis("JOY1_V", joystick->getV());
		}
		break;

	case MWIN_EVENT_JOYSTICK2_UPDATE:
		{
			MJoystick * joystick = window->getJoystick2();

			char name[32];
			for(int i=0; i<8; i++)
			{
				if(joystick->onKeyDown(i))
				{
					sprintf(name, "JOY2_BUTTON%d", i+1);
					input->downKey(name);
				}
				else if(joystick->onKeyUp(i))
				{
					sprintf(name, "JOY2_BUTTON%d", i+1);
					input->upKey(name);
				}
			}

			input->setAxis("JOY2_X", joystick->getX());
			input->setAxis("JOY2_Y", joystick->getY());
			input->setAxis("JOY2_Z", joystick->getZ());
			input->setAxis("JOY2_R", joystick->getR());
			input->setAxis("JOY2_U", joystick->getU());
			input->setAxis("JOY2_V", joystick->getV());
		}
		break;
	}
}

#endif
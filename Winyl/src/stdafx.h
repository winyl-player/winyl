/*  This file is part of Winyl Player source code.
    Copyright (C) 2008-2018, Alex Kras. <winylplayer@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

/* IMPORTANT!
I describe here why WINVER = 0x0501 but _WIN32_WINNT = 0x0601.
WINVER is set for WinXP, so size of structures will be for this system, otherwise strange things will happen.
_WIN32_WINNT is set for Win7, so COM for this system will be defined and used.
I do not know why WINVER is responsible for size of structures and _WIN32_WINNT for COM, but it is helpful for us.
Here some info about these defines: http://blogs.msdn.com/b/oldnewthing/archive/2007/04/11/2079137.aspx
it does not answer the question though.
---
When drop WinXP support WINVER should be 0x0600 and will not forget to change compiler option SUBSYSTEM version to 6.00.
---
Note: it is compiling under VS 2005 (without SP1) with including only header files from SDK Win 7 (this info is old, but I'll leave it for history).
*/

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0601	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

//Comment for include GdiPlus
//#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers

#define STRICT
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>

#ifdef IsMaximized
#undef IsMaximized
#endif

#include "uxtheme.h"

#include "ShObjIdl.h"
#include "Shlobj.h"


//#include <GdiPlus.h>
////using namespace Gdiplus;
//#pragma comment(lib, "GdiPlus.lib")




#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

// Old code for Visual Leak Detector
//#define _CRTDBG_MAP_ALLOC
//#include <crtdbg.h>
//#include <vld.h>

// To show line number in output window if memory leak occured
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
//#define new DEBUG_NEW
#endif
// Add next lines for cases where new uses with additional parameters (if "#define new DEBUG_NEW" is used)
//#ifdef new
//#undef new
//#endif

#include "mtypes.h"

#include "AutoHandle.h"

#include "FutureWin.h"
extern FutureWin* futureWin;

//#define UCD_OPENFILE     1

//#define UWM_TRAYMSG	     WM_APP
#define UWM_FILLTREE     WM_USER + 100
#define UWM_FILLLIST     WM_USER + 101
#define UWM_PLAYFILE     WM_USER + 102
#define UWM_NEXTFILE     WM_USER + 103
#define UWM_PLAYDRAW     WM_USER + 104
#define UWM_RATING       WM_USER + 105
#define UWM_CHANGESKIN   WM_USER + 106
#define UWM_LISTMENU     WM_USER + 107
#define UWM_TREEMENU     WM_USER + 108
#define UWM_TREESWAP     WM_USER + 109
#define UWM_LISTSWAP     WM_USER + 110
#define UWM_LISTDRAG     WM_USER + 111
#define UWM_RADIOMETA    WM_USER + 113
#define UWM_RADIOSTART   WM_USER + 114
#define UWM_GLOBHOTKEY   WM_USER + 115
//#define UWM_POPUP        WM_USER + 116
#define UWM_POPUPMENU    WM_USER + 117
#define UWM_STOP         WM_USER + 118
//#define UWM_MOUSEFOCUS   WM_USER + 119
//#define UWM_MOUSEWHEEL   WM_USER + 120
//#define UWM_SEARCHCLEAR  WM_USER + 121
#define UWM_SEARCHCHANGE WM_USER + 122
//#define UWM_SEARCHREDRAW WM_USER + 123
#define UWM_ACTION       WM_USER + 124
#define UWM_MINIPLAYER   WM_USER + 125
#define UWM_SCANEND      WM_USER + 126
#define UWM_BASSNEXT     WM_USER + 127
#define UWM_BASSWASTOP   WM_USER + 128
#define UWM_TREEDRAG     WM_USER + 129
#define UWM_TRAYMSG      WM_USER + 130
#define UWM_NOMINIPLAYER WM_USER + 131
#define UWM_LISTSEL      WM_USER + 132
#define UWM_BASSCHFREE   WM_USER + 133
#define UWM_LYRICSDONE   WM_USER + 134
#define UWM_LYRICSMENU   WM_USER + 135
#define UWM_LYRICSRECV   WM_USER + 136
#define UWM_COVERDONE    WM_USER + 137
#define UWM_SEARCHDONE   WM_USER + 138
#define UWM_TIMERTHREAD  WM_USER + 139



#define UWM_COMMAND WM_APP + 1003

// Commands - wParam of WWM_COMMAND
#define CMD_NULL         0   // Does nothing, always return 1
#define CMD_NONE         1   // Does nothing, always return 1
#define CMD_NONE2        2   // Does nothing, always return 1
#define CMD_MAGIC        3   // Does nothing, always return magic number 1237
#define CMD_VER_WINYL    4   // Return Winyl version (30201 = 3.2.1)
#define CMD_VER_API      5   // Return Command API version (10000 = 1.0.0)

// Command API 1.0.0 for Winyl 2.5.0
#define CMD_PLAY         100 // Play the focused track, if paused then play from the start
#define CMD_PLAY_EX      101 // Same as Play but if paused then resume instead of play from the start
#define CMD_PAUSE        102 // Pause the playing track, if stopped then does nothing
#define CMD_PAUSE_EX     103 // Same as Pause but if stopped then start the playback instead of does nothing
#define CMD_STOP         104 // Stop playback
#define CMD_NEXT         105 // Play next track
#define CMD_PREV         106 // Play previous track
#define CMD_CLOSE        107 // Close Winyl (Does nothing at this time)
#define CMD_CHECK_PLAY   200 // Return 1 if playing
#define CMD_CHECK_PAUSE  201 // Return 1 if paused (must be used with CMD_CHECK_PLAY)
#define CMD_CHECK_RADIO  202 // Return 1 if playing file from internet (must be used with CMD_CHECK_PLAY)
#define CMD_GET_VOLUME   210 // Return volume in percent (0-100%)
#define CMD_SET_VOLUME   211 // lParam = volume in percent (0-100%)
#define CMD_VOLUME_UP    212 // Volume Up by 5 percent
#define CMD_VOLUME_DOWN  213 // Volume Down by 5 percent
#define CMD_GET_MUTE     220 // Return 1 if mute is on
#define CMD_MUTE_ON      221 // Mute sound
#define CMD_MUTE_OFF     222 // Unmute sound
#define CMD_MUTE_REV     223 // Toggle mute state
#define CMD_GET_REPEAT   230 // Return 1 if repeat is on
#define CMD_REPEAT_ON    231 // Turn repeat on
#define CMD_REPEAT_OFF   232 // Turn repeat off
#define CMD_REPEAT_REV   233 // Toggle repeat state
#define CMD_GET_SHUFFLE  240 // Return 1 if shuffle is on
#define CMD_SHUFFLE_ON   241 // Turn shuffle on
#define CMD_SHUFFLE_OFF  242 // Turn shuffle off
#define CMD_SHUFFLE_REV  243 // Toggle shuffle state
#define CMD_GET_RATING   250 // Get playing track rating
#define CMD_SET_RATING   251 // Set playing track rating

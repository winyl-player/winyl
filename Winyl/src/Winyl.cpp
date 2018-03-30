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

#include "stdafx.h"
#include "Winyl.h"
#include "WinylApp.h"
//#include "BugTrap/BugTrap.h"
//#pragma comment(lib, "src/BugTrap/BugTrapU.lib")

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF /*| _CRTDBG_CHECK_ALWAYS_DF*/ | _CRTDBG_LEAK_CHECK_DF /*| _CRTDBG_DELAY_FREE_MEM_DF*/);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// Output all memory problems to a new window to make it more noticeable.
	// Comment next lines to output to the new window with more info (don't forget to uncomment it after tests).
	// Use DEBUG_NEW to show line number in the output window (see stdafx.h).
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_WNDW);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_WNDW);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_WNDW);
#endif

	// Use HeapEnableTerminationOnCorruption flag. It is better to crash than continue to work with corrupted heap.
	// HeapEnableTerminationOnCorruption is not needed if Win64 or SUBSYSTEM major version is 6 or higher,
	// it is enabled by default in this case, see: http://blogs.msdn.com/b/oldnewthing/archive/2013/12/27/10484882.aspx
	// so remove it when drop WinXP support.
#if defined(_WIN64) || (WINVER >= 0x0600)
	assert(false);
#else
	::HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
#endif

// Leak detectors/crash reporters: (store links here for now)
// Visual Leak Detector: https://github.com/KindDragon/vld/wiki/Using-Visual-Leak-Detector
// BugTrap: https://github.com/bchavez/BugTrap
// CrashRpt: http://crashrpt.sourceforge.net/
// Doctor Dump: https://drdump.com/CrashRpt.CPP/About

// Keep BugTrap config, we can use it to find difficult bugs in the future
//	BT_InstallSehFilter();
//	BT_SetAppName(_T("Winyl"));
//	BT_SetSupportEMail(_T("email@example.com"));
//	BT_SetFlags(BTF_DETAILEDMODE | BTF_EDITMAIL);
//	BT_SetSupportServer(_T("localhost"), 9999);
//	BT_SetSupportURL(_T("http://www.example.com"));

	// Reduce the scope for better resource leak detection
	{
		WinylApp app;
		app.Init();
	}

#ifdef _DEBUG
	DWORD gdiObjects = ::GetGuiResources(::GetCurrentProcess(), GR_GDIOBJECTS);
	DWORD userObjects = ::GetGuiResources(::GetCurrentProcess(), GR_USEROBJECTS);
	::OutputDebugStringW((L"GDI Objects: " + std::to_wstring(gdiObjects) + L'\n').c_str());
	::OutputDebugStringW((L"USER Objects: " + std::to_wstring(userObjects) + L'\n').c_str());
	// This assert help us to find large (and sometimes small) resource leak.
	// It is hard to calculate max gdi and user objects here because for example
	// file dialog add many objects if opened, so I just use max values which I saw.
	// So the values can be changed if assert appears without any reason.
	// Under comment max values which I saw without file dialog (use it for testing).
	assert(gdiObjects <= 100); // 47
	assert(userObjects <= 25); // 4
#endif

	// Check for memory corruption
	assert(_CrtCheckMemory());

	return 0;
}

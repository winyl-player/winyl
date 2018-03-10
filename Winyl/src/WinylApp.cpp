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

#include "StdAfx.h"
#include "resource.h"
#include "WinylApp.h"
#include "WinylWnd.h"
#include "DropTargetOpen.h"
#include "ExImage.h"
#include "FileSystem.h"

FutureWin* futureWin = nullptr;

WinylApp::WinylApp()
{

}

WinylApp::~WinylApp()
{
	//if (gdiplusToken)
	//	Gdiplus::GdiplusShutdown(gdiplusToken);

	// OleUninitialize should be here after all other parts of the program are unloaded
	// or if it will be called before for example BASS_ASIO_Free that uses COM it will cause bugs
	if (isOleInitialize)
		::OleUninitialize();
}

void WinylApp::Init()
{
	INITCOMMONCONTROLSEX InitCtrls = {};
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES|ICC_LINK_CLASS;
	::InitCommonControlsEx(&InitCtrls);

	bool cmdEmbedding = false;
	bool cmdPortable = false;
	bool cmdUnregister = false;
	std::wstring openFiles;

	ParseCommandLine(openFiles, cmdEmbedding, cmdPortable, cmdUnregister);

	// Remove file associations when start with /Unregister command line flag
	if (cmdUnregister)
	{
		OSVERSIONINFO osVersion = {};
		osVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		::GetVersionEx(&osVersion);

		Associations assoc;
		assoc.RemoveAllAssoc(osVersion.dwMajorVersion >= 6 ? true : false);
		return;
	}

	//::CoInitialize(NULL);
	::OleInitialize(NULL);
	isOleInitialize = true;

	bool isAlreadyRunning = false;

	HANDLE mutexHandle = NULL;
	DWORD mutexError = 0;

	bool isPortableProfile = cmdPortable;
	bool isPortableVersion = false;

	std::wstring programPath;
	std::wstring profilePath;

#ifndef _DEBUG
	programPath = FileSystem::GetProgramPath();
#else
	programPath = FileSystem::GetCurrentDir();
#endif

	if (FileSystem::Exists(programPath + L"Portable.dat"))
	{
		isPortableVersion = true;
		isPortableProfile = false;

		std::wstring mutexName = programPath + L"WinylMutex";
		std::replace(mutexName.begin(), mutexName.end(), '\\', '/');

		::SetLastError(0);
		mutexHandle = ::CreateMutex(NULL, FALSE, mutexName.c_str());
		mutexError = ::GetLastError();
	}
	else
	{
		::SetLastError(0);
		mutexHandle = ::CreateMutex(NULL, FALSE, L"WinylMutex");
		mutexError = ::GetLastError();
	}

#ifdef _DEBUG
	isPortableVersion = false;
	isPortableProfile = false;
#endif

#ifndef _DEBUG
	if (!isPortableVersion && !isPortableProfile)
	{
		profilePath = FileSystem::GetAppDataPath();
		profilePath += L"Winyl";
		FileSystem::CreateDir(profilePath);
		profilePath.push_back('\\');
	}
	else
#endif
	{
		profilePath = programPath + L"Profile";
		FileSystem::CreateDir(profilePath);
		profilePath.push_back('\\');
	}

	if (mutexError == ERROR_ALREADY_EXISTS || mutexError == ERROR_ACCESS_DENIED)
		isAlreadyRunning = true;

	assert(isOleInitialize == true);
	if (cmdEmbedding)
	{
		openFiles.clear();
		DropTargetOpen::GetDropFiles(::GetModuleHandle(NULL), openFiles);
	}

	if (isAlreadyRunning)
	{
		HWND hWnd = FindWindow(L"WinylWnd", NULL);

		if (hWnd)
		{
			if (!openFiles.empty())
			{
				SendOpenFiles(hWnd, openFiles, cmdEmbedding);
			}
			else
			{
				::SendMessage(hWnd, UWM_NOMINIPLAYER, 0, 0);

				// http://blogs.msdn.com/b/oldnewthing/archive/2009/02/20/9435239.aspx

				if (::IsIconic(hWnd))
					::ShowWindow(hWnd, SW_RESTORE);
				::SetForegroundWindow(hWnd);
				::InvalidateRect(hWnd, NULL, TRUE);
			}
		}

		//::OleUninitialize(); // Important!
		return;
	}

	// Init Future Windows Versions module
	futureWin = new FutureWin();

	// Init Gdiplus
	//Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	//Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	{ // Start
	ExImage::Source::ImagingFactory::Instance().Init();

	WinylWnd winylWnd;
	winylWnd.SetPortableProfile(isPortableProfile);
	winylWnd.SetPortableVersion(isPortableVersion);
	winylWnd.SetProgramPath(std::move(programPath));
	winylWnd.SetProfilePath(std::move(profilePath));
	winylWnd.SetOpenFiles(openFiles);
	if (winylWnd.NewWindow())
	{
		winylWnd.RunShowWindow();
		winylWnd.PrepareLibrary();

		HACCEL accelerators = ::LoadAccelerators(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_ACCELERATOR));

		MSG msg;
		while (::GetMessage(&msg, NULL, 0, 0))// > 0)
		{
			// Do not use it yet (affects only DlgProgress anyway)
			//if (DialogEx::IsDialogExMessage(&msg))
			//	continue;

			// If dragging something then process Escape key here to stop Drag'n'Drop
			// Also in WindowProc of the main window process WM_ACTIVATE we can't do it here
			if (winylWnd.isDragDrop)
			{
				//if (msg.message == WM_SYSKEYDOWN)
				//	continue;
				if (msg.message == WM_KEYDOWN)
				{
					if (msg.wParam == VK_ESCAPE)
						winylWnd.StopDragDrop();
				//	else
				//		continue;
				}
			}
			if (!winylWnd.skinEdit->IsFocus())
				::TranslateAccelerator(winylWnd.Wnd(), accelerators, &msg);

			winylWnd.PreTranslateMouseWheel(&msg); // Send mouse wheel messages from here
			winylWnd.PreTranslateRelayEvent(&msg);
			::TranslateMessage(&msg);
			//winylWnd.toolTips.RelayEvent(&msg);
			::DispatchMessage(&msg);
		}

		::DestroyAcceleratorTable(accelerators);
	}

	ExImage::Source::ImagingFactory::Instance().Free();

	// Destructors here
	} // End

	////Gdiplus::GdiplusShutdown(gdiplusToken);

	delete futureWin;

	//::CoUninitialize();
	//::OleUninitialize();

	//if (mutexHandle) ::CloseHandle(mutexHandle);

	return;
}

bool WinylApp::SendOpenFiles(HWND hWnd, const std::wstring& openFiles, bool isEmbedding)
{
	if (hWnd && !openFiles.empty())
	{
		const std::wstring& file = openFiles;
	
		COPYDATASTRUCT cd;
		if (isEmbedding)
			cd.dwData = 2;
		else
			cd.dwData = 1;
		cd.cbData = (DWORD)file.size() * sizeof(wchar_t) + sizeof(wchar_t);
		cd.lpData = (PVOID)file.c_str();
		::SendMessage(hWnd, WM_COPYDATA, (WPARAM)hWnd, (LPARAM)&cd);

		return true;
	}

	return false;
}

void WinylApp::ParseCommandLine(std::wstring& openFiles, bool& isEmbedding, bool& isPortable, bool& isUnregister)
{
	for (int i = 1; i < __argc; ++i)
	{
		if (__wargv[i][0] == '/' || __wargv[i][0] == '-')
		{
			wchar_t* param = (__wargv[i] + 1);

			if (lstrcmpi(param, L"p") == 0)
				isPortable = true;
			else if (lstrcmpi(param, L"embedding") == 0)
				isEmbedding = true;
			else if (lstrcmpi(param, L"unregister") == 0)
				isUnregister = true;
		}
		else
		{
			if (openFiles.empty())
				openFiles = __wargv[i];
		}
	}
}

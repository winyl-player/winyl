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
#include "DropTargetOpen.h"

DropTargetOpen::DropTargetOpen(void)
{
}

DropTargetOpen::~DropTargetOpen(void)
{
}

DropTargetOpen::ProcessReference* DropTargetOpen::g_ppr = NULL;

std::wstring* DropTargetOpen::pointerOpenFiles = NULL;

void DropTargetOpen::OpenFilesFromDataObject(IDataObject* pdto)
{
	FORMATETC fmte = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stgm;
	if (SUCCEEDED(pdto->GetData(&fmte, &stgm)))
	{
		HDROP hdrop = reinterpret_cast<HDROP>(stgm.hGlobal);
		UINT numberFiles = ::DragQueryFile(hdrop, 0xFFFFFFFF, NULL, 0);
		for (UINT i = 0; i < numberFiles; ++i)
		{
			wchar_t file[MAX_PATH * 16];
			if (::DragQueryFile(hdrop, i, file, sizeof(file) / sizeof(wchar_t)))
			{
				if (i > 0)
					(*pointerOpenFiles).push_back('|');

				(*pointerOpenFiles) += file;					
			}
		}
		::ReleaseStgMedium(&stgm);
	}

	::PostQuitMessage(0);
}

bool DropTargetOpen::GetDropFiles(HINSTANCE hInstance, std::wstring& openFiles)
{
	pointerOpenFiles = &openFiles;

//	CoInitialize(NULL);

	SimpleClassFactory s_scf;

	UUID CLSID_Scratch;
	::CLSIDFromString(L"{254F0A10-5970-4149-9C8F-4E08D2525427}", &CLSID_Scratch);

	ProcessReference ref;
	g_ppr = &ref;

	DWORD dwRegisterCookie = 0;
	HRESULT hrRegister = ::CoRegisterClassObject(CLSID_Scratch, &s_scf,
				CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE, &dwRegisterCookie);

	if (SUCCEEDED(hrRegister))
	{
		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= ::DefWindowProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= hInstance;
		wcex.hIcon			= NULL;
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName	= NULL;
		wcex.lpszClassName	= L"WinylWndOpen";
		wcex.hIconSm		= NULL;

		RegisterClassEx(&wcex);

		HWND hWnd = ::CreateWindow(L"WinylWndOpen", L"", WS_OVERLAPPEDWINDOW,
								CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
								NULL, NULL, hInstance, 0);

		MSG msg;
		while (::GetMessage(&msg, NULL, 0, 0))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		g_ppr = NULL;

		::DestroyWindow(hWnd);

		if (SUCCEEDED(hrRegister))
			::CoRevokeClassObject(dwRegisterCookie);
	}

//	CoUninitialize();

	if (SUCCEEDED(hrRegister))
		return true;

	return false;
}

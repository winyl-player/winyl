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

// DragIconWnd.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DragIconWnd.h"


// DragIconWnd

DragIconWnd::DragIconWnd()
{

}

DragIconWnd::~DragIconWnd()
{

}

bool DragIconWnd::CreateDragIcon()
{
	if (wndDragIcon)
		return true;

	HINSTANCE instance = ::GetModuleHandle(NULL);

	//const wchar_t* className = L"DragIconWnd";
      
	//WNDCLASSEX wcex;
	//wcex.cbSize = sizeof(WNDCLASSEX);

	//if (!::GetClassInfoEx(appInstance, className, &wcex))
	//{
	//	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	//	wcex.lpfnWndProc	= ::DefWindowProc;
	//	wcex.cbClsExtra		= 0;
	//	wcex.cbWndExtra		= 0;
	//	wcex.hInstance		= instance;
	//	wcex.hIcon			= NULL;
	//	wcex.hCursor		= ::LoadCursorW(NULL, IDC_ARROW);
	//	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	//	wcex.lpszMenuName	= NULL;
	//	wcex.lpszClassName	= className;
	//	wcex.hIconSm		= NULL;

	//	RegisterClassEx(&wcex);
	//}

	wndDragIcon = ::CreateWindowEx(WS_EX_LAYERED|WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE|WS_EX_TOPMOST, WC_STATIC, NULL,
		NULL, 0, 0, iconSizeX, iconSizeY, NULL, NULL, instance, NULL);

	if (wndDragIcon == NULL)
		return false;

	HICON iconDrag = (HICON)::LoadImageW(instance, MAKEINTRESOURCE(IDI_ICON_DRAG), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);

	HDC dcMemory = ::CreateCompatibleDC(NULL);

	HBITMAP bmMemory = ExImage::Create32BppBitmap(iconSizeX, iconSizeY);
	HGDIOBJ oldMemory = ::SelectObject(dcMemory, bmMemory);

	//::DrawIcon(dcMemory, 0, 0, iconDrag);
	::DrawIconEx(dcMemory, 0, 0, iconDrag, 0, 0, 0, NULL, DI_NORMAL | DI_COMPAT);
	::DestroyIcon(iconDrag);

	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 200, AC_SRC_ALPHA};

	POINT ptSrc = {0, 0};
	POINT ptDst = {0, 0};
	SIZE sz = {iconSizeX, iconSizeY};

	::UpdateLayeredWindow(wndDragIcon, NULL, &ptDst, &sz, dcMemory, &ptSrc, 0, &bf, ULW_ALPHA);

	::SelectObject(dcMemory, oldMemory);
	::DeleteObject(bmMemory);
	::DeleteDC(dcMemory);

	return true;
}

void DragIconWnd::MoveIcon(int x, int y)
{
	if (wndDragIcon == NULL)
	{
		CreateDragIcon();
		::MoveWindow(wndDragIcon, x, y + 4, iconSizeX, iconSizeY, FALSE);
		::ShowWindow(wndDragIcon, SW_SHOWNOACTIVATE);
	}
	else
		::MoveWindow(wndDragIcon, x, y + 4, iconSizeX, iconSizeY, FALSE);
}

void DragIconWnd::HideIcon()
{
	if (wndDragIcon)
	{
		::DestroyWindow(wndDragIcon);
		wndDragIcon = NULL;
	}
}




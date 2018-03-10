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
#include "resource.h"
#include "Win7TaskBar.h"
#include "ContextMenu.h"

Win7TaskBar::Win7TaskBar()
{

}

Win7TaskBar::~Win7TaskBar()
{

}

void Win7TaskBar::SetCover(HWND wnd, const ExImage::Source& image)
{
	if (!futureWin->IsSevenOrLater())
		return;

	cover.Free();

	image.MakeThumbnail(cover, 200, 200);

	BOOL isEnable = TRUE;
	futureWin->DwmSetWindowAttribute(wnd, DWMWA_HAS_ICONIC_BITMAP, &isEnable, sizeof(isEnable));
	futureWin->DwmSetWindowAttribute(wnd, DWMWA_FORCE_ICONIC_REPRESENTATION, &isEnable, sizeof(isEnable));
	futureWin->DwmSetWindowAttribute(wnd, DWMWA_DISALLOW_PEEK, &isEnable, sizeof(isEnable));

	futureWin->DwmInvalidateIconicBitmaps(wnd);
}

void Win7TaskBar::EmptyCover(HWND wnd)
{
	if (!futureWin->IsSevenOrLater())
		return;

	cover.Free();

	BOOL isEnable = FALSE;
	futureWin->DwmSetWindowAttribute(wnd, DWMWA_HAS_ICONIC_BITMAP, &isEnable, sizeof(isEnable));
	futureWin->DwmSetWindowAttribute(wnd, DWMWA_FORCE_ICONIC_REPRESENTATION, &isEnable, sizeof(isEnable));
	futureWin->DwmSetWindowAttribute(wnd, DWMWA_DISALLOW_PEEK, &isEnable, sizeof(isEnable));
}

void Win7TaskBar::MessageCoverBitmap(HWND wnd, int width, int height)
{
	if (!futureWin->IsSevenOrLater())
		return;

	if (cover.IsValid())
	{
		if (width < height)
			height = width;
		else
			width = height;

		ExImage::Source thumb;
		cover.MakeThumbnail(thumb, width, height, true);

		HBITMAP bitmapHandle = thumb.GetHBITMAP();

		futureWin->DwmSetIconicThumbnail(wnd, bitmapHandle, 0);

		::DeleteObject(bitmapHandle);
	}
}

void Win7TaskBar::MessageCoverPreview(HWND wnd)
{
/*	if (!futureWin->IsSevenOrBetter())
		return;

	if (pCover)
	{
		HBITMAP hBitmap = NULL;

		((Bitmap*)pCover)->GetHBITMAP(Color(0, 0, 0, 0), &hBitmap);

		WINDOWPLACEMENT pl;
		::GetWindowPlacement(hWnd, &pl);

		CPoint pt((pl.rcNormalPosition.right -  pl.rcNormalPosition.left) / 2 - 100,
			(pl.rcNormalPosition.bottom -  pl.rcNormalPosition.top) / 2 - 100);

		futureWin->DwmSetIconicLivePreviewBitmap(hWnd, hBitmap, &pt, DWM_SIT_DISPLAYFRAME);

		::DeleteObject(hBitmap);
	}*/
}

void Win7TaskBar::AddButtons(HWND wnd, bool isPlay)
{
	if (!futureWin->IsSevenOrLater())
		return;

	AutoCom<ITaskbarList3> taskList;
	if (taskList.CoCreateInstance(CLSID_TaskbarList) == S_OK)
	{
		HINSTANCE instance = ::GetModuleHandleW(NULL);

		int cxIcon = ::GetSystemMetrics(SM_CXSMICON);
		int cyIcon = ::GetSystemMetrics(SM_CYSMICON);

		THUMBBUTTON button[4] = {};

		button[0].dwMask = THB_ICON;
		button[0].iId = ID_MENU_PREV;
		button[0].hIcon = (HICON)::LoadImageW(instance, MAKEINTRESOURCE(IDI_ICON_THUMB_PREV), IMAGE_ICON, cxIcon, cyIcon, LR_DEFAULTCOLOR);
		button[1].dwMask = THB_ICON;
		button[1].iId = ID_MENU_PAUSE;
		if (isPlay)
			button[1].hIcon = (HICON)::LoadImageW(instance, MAKEINTRESOURCE(IDI_ICON_THUMB_PAUSE), IMAGE_ICON, cxIcon, cyIcon, LR_DEFAULTCOLOR);
		else
			button[1].hIcon = (HICON)::LoadImageW(instance, MAKEINTRESOURCE(IDI_ICON_THUMB_PLAY), IMAGE_ICON, cxIcon, cyIcon, LR_DEFAULTCOLOR);
		button[2].dwMask = THB_ICON;
		button[2].iId = ID_MENU_STOP;
		button[2].hIcon = (HICON)::LoadImageW(instance, MAKEINTRESOURCE(IDI_ICON_THUMB_STOP), IMAGE_ICON, cxIcon, cyIcon, LR_DEFAULTCOLOR);
		button[3].dwMask = THB_ICON;
		button[3].iId = ID_MENU_NEXT;
		button[3].hIcon = (HICON)::LoadImageW(instance, MAKEINTRESOURCE(IDI_ICON_THUMB_NEXT), IMAGE_ICON, cxIcon, cyIcon, LR_DEFAULTCOLOR);

		taskList->ThumbBarAddButtons(wnd, 4, button);

		::DestroyIcon(button[0].hIcon);
		::DestroyIcon(button[1].hIcon);
		::DestroyIcon(button[2].hIcon);
		::DestroyIcon(button[3].hIcon);
	}
}

void Win7TaskBar::UpdateButtons(HWND wnd, bool isPlay)
{
	if (!futureWin->IsSevenOrLater())
		return;

	AutoCom<ITaskbarList3> taskList;
	if (taskList.CoCreateInstance(CLSID_TaskbarList) == S_OK)
	{
		HINSTANCE instance = ::GetModuleHandleW(NULL);

		int cxIcon = ::GetSystemMetrics(SM_CXSMICON);
		int cyIcon = ::GetSystemMetrics(SM_CYSMICON);

		THUMBBUTTON button[1] = {};

		button[0].dwMask = THB_ICON;
		button[0].iId = ID_MENU_PAUSE;
		if (isPlay)
			button[0].hIcon = (HICON)::LoadImageW(instance, MAKEINTRESOURCE(IDI_ICON_THUMB_PAUSE), IMAGE_ICON, cxIcon, cyIcon, LR_DEFAULTCOLOR);
		else
			button[0].hIcon = (HICON)::LoadImageW(instance, MAKEINTRESOURCE(IDI_ICON_THUMB_PLAY), IMAGE_ICON, cxIcon, cyIcon, LR_DEFAULTCOLOR);

		taskList->ThumbBarUpdateButtons(wnd, 1, button);
		
		::DestroyIcon(button[0].hIcon);
	}
}

void Win7TaskBar::SetProgressValue(HWND wnd, int current, int total)
{
	if (!futureWin->IsSevenOrLater())
		return;

	AutoCom<ITaskbarList3> taskList;
	if (taskList.CoCreateInstance(CLSID_TaskbarList) == S_OK)
	{
		taskList->SetProgressValue(wnd, current, total);
	}
}

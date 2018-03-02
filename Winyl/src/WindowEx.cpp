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
#include "WindowEx.h"
#include <cassert>

std::map<HWND, WindowEx*> WindowEx::mapWindows;

WindowEx::WindowEx()
{

}

WindowEx::~WindowEx()
{
	if (thisWnd)
		::DestroyWindow(thisWnd);
}

bool WindowEx::CreateClassWindow(HWND parent, WCHAR* className, DWORD style, DWORD exStyle, 
								  int x, int y, int cx, int cy, const wchar_t* title, HICON icon, HICON iconSm, bool isDoubleClicks)
{
	assert(thisWnd == NULL);

	HINSTANCE instance = ::GetModuleHandleW(NULL);

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);

	if (!::GetClassInfoExW(instance, className, &wcex))
	{
		// Do not add CS_HREDRAW and CS_VREDRAW, we use manual redraw on MW_SIZE or WM_MOVE
		wcex.style          = (isDoubleClicks ? CS_DBLCLKS : 0);// | CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= MainWindowProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= instance;
		wcex.hIcon			= icon;
		wcex.hIconSm        = iconSm;
		wcex.hCursor		= ::LoadCursorW(NULL, IDC_ARROW);
		wcex.hbrBackground  = ::CreateSolidBrush(RGB(255, 255, 255));//(HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName	= NULL;
		wcex.lpszClassName	= className;

		RegisterClassExW(&wcex);
	}

	//thisWnd = ::CreateWindowEx(exStyle, className, NULL, style, x, y, cx, cy, parent, NULL, appInstance, NULL);
	::CreateWindowExW(exStyle, className, title, style, x, y, cx, cy, parent, NULL, instance, (LPVOID)this);
	
	if (thisWnd == NULL)
		return false;

/*	thisParentWnd = parent;

	mapWindows[thisWnd] = this;*/

	return true;
}

bool WindowEx::CreateClassWindow(HWND parent, WCHAR* className, DWORD style, DWORD exStyle,
								  RECT* rc, const wchar_t* title, HICON icon, HICON iconSm, bool isDoubleClicks)
{
	return CreateClassWindow(parent, className, style, exStyle,
		rc->left, rc->top, rc->right - rc->left, rc->bottom - rc->top, title, icon, iconSm, isDoubleClicks);
}

bool WindowEx::CreateClassWindow(HWND parent, WCHAR* className, DWORD style, DWORD exStyle, bool isDoubleClicks)
{
	return CreateClassWindow(parent, className, style, exStyle, 0, 0, 0, 0, NULL, NULL, NULL, isDoubleClicks);
}

LRESULT WindowEx::MainWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_NCCREATE)
	{
		WindowEx* windowEx = (WindowEx*)((CREATESTRUCT*)lParam)->lpCreateParams;

		mapWindows[hWnd] = windowEx;
		windowEx->thisWnd = hWnd;
		windowEx->thisParentWnd = ::GetParent(hWnd);

		return windowEx->WindowProc(hWnd, message, wParam, lParam);
	}

	std::map<HWND, WindowEx*>::iterator it = mapWindows.find(hWnd);
	if (it != mapWindows.end())
	{
		if (message == WM_DESTROY)
		{
			LRESULT result = it->second->WindowProc(hWnd, message, wParam, lParam);
			it->second->thisWnd = NULL;
			it->second->thisParentWnd = NULL;
			mapWindows.erase(it);
			return result;
		}
		else
			return it->second->WindowProc(hWnd, message, wParam, lParam);
	}

	return ::DefWindowProcW(hWnd, message, wParam, lParam);
}

LRESULT WindowEx::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return ::DefWindowProcW(hWnd, message, wParam, lParam);
}

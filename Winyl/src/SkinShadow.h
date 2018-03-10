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

#pragma once

#include "WindowEx.h"
#include "ExImage.h"

class SkinShadow
{
public:
	SkinShadow();
	virtual ~SkinShadow();

	void NewShadow(HWND parent);

	bool LoadSkin(const std::wstring& programPath, const std::wstring& skinName, ZipFile* zipFile);

	void Move(int x, int y, int cx, int cy);
	void Show(bool show);

private:
	bool NewWindow(WindowEx& window, HWND parent);
	void DrawShadow(HWND wnd, int type, int x, int y, int cx, int cy);

	WindowEx windowTop;
	WindowEx windowBottom;
	WindowEx windowLeft;
	WindowEx windowRight;

	bool isWindowVisible = false;

	ExImage shadowTop;
	ExImage shadowTopLeft;
	ExImage shadowTopRight;
	ExImage shadowBottom;
	ExImage shadowBottomLeft;
	ExImage shadowBottomRight;
	ExImage shadowLeft;
	ExImage shadowLeftTop;
	ExImage shadowLeftBottom;
	ExImage shadowRight;
	ExImage shadowRightTop;
	ExImage shadowRightBottom;
};


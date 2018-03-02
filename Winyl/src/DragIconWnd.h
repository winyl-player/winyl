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

#include "ExImage.h"

// DragIconWnd

class DragIconWnd
{

public:
	DragIconWnd();
	virtual ~DragIconWnd();

	void MoveIcon(int x, int y);
	void HideIcon();

private:
	bool CreateDragIcon();

	HWND wndDragIcon = NULL;
	int iconSizeX = 32;
	int iconSizeY = 33; // Fix 33 - fix small bottom tail from icon on some PCs
};



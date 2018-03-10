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

class MoveResize
{

public:
	MoveResize();
	virtual ~MoveResize();

	void Resize(int cx, int cy);
	bool SetCursor();
	void MouseMove(HWND wnd, const CPoint& point, bool isMove);
	void MouseDown(HWND wnd, const CPoint& point, bool isMove);
	void MouseLeave();
	void SetResizeBorder(const CRect& rcResizeBorder) {rcResize = rcResizeBorder;}
		
private:
	CRect rcSize = {0, 0, 0, 0};
	CRect rcMove = {0, 0, 0, 0};

	enum class MouseType
	{
		Move        = 1,
		Left        = 2,
		Right       = 3,
		Top         = 4,
		Bottom      = 5,
		TopLeft     = 6,
		TopRight    = 7,
		BottomLeft  = 8,
		BottomRight = 9
	};

	MouseType mouseType = MouseType::Move;

	CRect rcResize = {0, 0, 0, 0};
//	CRect rcAlpha;

	MouseType GetMouseType(const CPoint& point);
};



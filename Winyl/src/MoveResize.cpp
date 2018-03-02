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

// MoveResize.cpp : implementation file
//

#include "stdafx.h"
#include "MoveResize.h"


// MoveResize

MoveResize::MoveResize()
{

}

MoveResize::~MoveResize()
{

}

MoveResize::MouseType MoveResize::GetMouseType(const CPoint &point)
{
	if (rcMove.PtInRect(point))
		return MouseType::Move;
	else if (CRect(0, rcResize.top, rcResize.left, rcMove.bottom).PtInRect(point))
		return MouseType::Left;
	else if (CRect(rcMove.right, rcResize.top, rcSize.right, rcMove.bottom).PtInRect(point))
		return MouseType::Right;
	else if (CRect(rcResize.left + 10, 0, rcMove.right - 10, rcResize.top).PtInRect(point))
		return MouseType::Top;
	else if (CRect(rcResize.left + 10, rcMove.bottom, rcMove.right - 10, rcSize.bottom).PtInRect(point))
		return MouseType::Bottom;
	else if (CRect(rcMove.right - 10, rcMove.bottom, rcSize.right, rcSize.bottom).PtInRect(point))
		return MouseType::BottomRight;
	else if (CRect(0, rcMove.bottom, rcResize.left + 10, rcSize.bottom).PtInRect(point))
		return MouseType::BottomLeft;
	else if (CRect(0, 0, rcResize.left + 10, rcResize.top).PtInRect(point))
		return MouseType::TopLeft;
	else if (CRect(rcMove.right - 10, 0, rcSize.right, rcResize.top).PtInRect(point))
		return MouseType::TopRight;

	return MouseType::Move;
}

void MoveResize::Resize(int cx, int cy)
{
	rcSize.SetRect(0, 0, cx, cy);
	rcMove.SetRect(rcResize.left, rcResize.top, cx - rcResize.right, cy - rcResize.bottom);
}

bool MoveResize::SetCursor()
{
	if (mouseType == MouseType::Move)
		return false;

	switch (mouseType)
	{
		case MouseType::Left:
		case MouseType::Right:
			::SetCursor(::LoadCursorW(NULL, IDC_SIZEWE));
			return true;

		case MouseType::Top:
		case MouseType::Bottom:
			::SetCursor(::LoadCursorW(NULL, IDC_SIZENS));
			return true;

		case MouseType::TopLeft:
		case MouseType::BottomRight:
			::SetCursor(::LoadCursorW(NULL, IDC_SIZENWSE));
			return true;

		case MouseType::TopRight:
		case MouseType::BottomLeft:
			::SetCursor(::LoadCursorW(NULL, IDC_SIZENESW));
			return true;
	}

	return false;
}

void MoveResize::MouseMove(HWND wnd, const CPoint& point, bool isMove)
{
	if (isMove || ::IsZoomed(wnd))
		mouseType = MouseType::Move;
	else
		mouseType = GetMouseType(point);
}

void MoveResize::MouseDown(HWND wnd, const CPoint& point, bool isMove)
{
	if (::IsZoomed(wnd))
	{
		mouseType = MouseType::Move;
		return;
	}

	if (isMove)
		mouseType = MouseType::Move;
	else
		mouseType = GetMouseType(point);

	// The following code uses some undocumented features
	// SC_MOVE part is used by many programs like: WMP, iTunes, .NET (DragMove) so it should be fine
	// SC_SIZE part is pure magic (I just don't remember where I found it maybe I just used Spy++ to get it)

	// Some magic stuff
	switch (mouseType)
	{
		case MouseType::Move:
			::SendMessage(wnd, WM_SYSCOMMAND, SC_MOVE|0x02, NULL);
		break;
		case MouseType::Left:
			::SendMessage(wnd, WM_SYSCOMMAND, SC_SIZE|(HTLEFT-0x09), NULL);
		break;
		case MouseType::Right:
			::SendMessage(wnd, WM_SYSCOMMAND, SC_SIZE|(HTRIGHT-0x09), NULL);
		break;
		case MouseType::Top:
			::SendMessage(wnd, WM_SYSCOMMAND, SC_SIZE|(HTTOP-0x09), NULL);
		break;
		case MouseType::Bottom:
			::SendMessage(wnd, WM_SYSCOMMAND, SC_SIZE|(HTBOTTOM-0x09), NULL);
		break;
		case MouseType::TopLeft:
			::SendMessage(wnd, WM_SYSCOMMAND, SC_SIZE|(HTTOPLEFT-0x09), NULL);
		break;
		case MouseType::TopRight:
			::SendMessage(wnd, WM_SYSCOMMAND, SC_SIZE|(HTTOPRIGHT-0x09), NULL);
		break;
		case MouseType::BottomLeft:
			::SendMessage(wnd, WM_SYSCOMMAND, SC_SIZE|(HTBOTTOMLEFT-0x09), NULL);
		break;
		case MouseType::BottomRight:
			::SendMessage(wnd, WM_SYSCOMMAND, SC_SIZE|(HTBOTTOMRIGHT-0x09), NULL);
		break;
	}

	// Don't forget to release the button
	::SendMessage(wnd, WM_LBUTTONUP, NULL, MAKELPARAM(point.x, point.y));

	// End of the magic
}

void MoveResize::MouseLeave()
{
	mouseType = MouseType::Move;
}

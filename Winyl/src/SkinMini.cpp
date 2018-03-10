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
#include "SkinMini.h"
#include "FileSystem.h"

SkinMini::SkinMini()
{

}

SkinMini::~SkinMini()
{

}

LRESULT SkinMini::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(thisWnd, &ps);
		OnPaint(hdc, ps);
		EndPaint(thisWnd, &ps);
		return 0;
	case WM_ERASEBKGND:
		return 1;
	case WM_MOUSEMOVE:
		TRACKMOUSEEVENT tme;
		OnMouseMove((UINT)wParam, CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = thisWnd;
		tme.dwHoverTime = 0;
		TrackMouseEvent(&tme);
		return 0;
	case WM_MOUSELEAVE:
		OnMouseLeave();
		return 0;
	case WM_NCHITTEST:
		OnNcHitTest(CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		break;
	case WM_SETCURSOR:
		if (OnSetCursor((HWND)wParam, LOWORD(lParam), HIWORD(lParam)))
			return 1;
		break;
	case WM_WINDOWPOSCHANGING:
		OnWindowPosChanging((WINDOWPOS*)lParam);
		return 0;
	case WM_SIZE:
		OnSize((UINT)wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_GETMINMAXINFO:
		OnGetMinMaxInfo((MINMAXINFO*)lParam);
		return 0;
	case WM_LBUTTONDOWN:
		OnLButtonDown((UINT)wParam, CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		return 0;
	case WM_LBUTTONUP:
		OnLButtonUp((UINT)wParam, CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		return 0;
	case WM_LBUTTONDBLCLK:
		OnLButtonDblClk((UINT)wParam, CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		return 0;
	case WM_CONTEXTMENU:
		OnContextMenu((HWND)wParam, CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		return 0;
	case WM_SYSCOMMAND:
		if (OnSysCommand(wParam, lParam))
			return 0;
		break;
	case WM_DESTROY:
		OnDestroy();
		return 0;
	case WM_DROPFILES:
		OnDropFiles((HDROP)wParam);
		return 0;
	}

	return ::DefWindowProc(hWnd, message, wParam, lParam);
}

bool SkinMini::NewWindow(HWND parent, CRect& rcPos, HICON iconLarge, HICON iconSmall)
{
	wndParent = parent;

	int styleTopmost = WS_EX_TOPMOST;
	if (zOrder == 2)
		styleTopmost = 0;

	if (CreateClassWindow(parent, L"WinylMiniWnd", WS_OVERLAPPED|WS_SYSMENU|WS_MINIMIZEBOX|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
		WS_EX_ACCEPTFILES|styleTopmost, rcPos, L"Winyl", iconLarge, iconSmall, true)) // WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE
	{
		//::SetParent(thisWnd, ::FindWindowW(L"Progman", L"Program Manager"));

		skinDraw.SetWindowStyle(thisWnd, parent);

		LoadWindows();

		moveResize.SetResizeBorder(skinDraw.GetResizeBorder());

		//moveResize.CorrectSize(this);
		//skinDraw.RefreshWindow(); // Refresh all skin elements

		::UpdateWindow(thisWnd);

		return true;
	}

	return false;
}

void SkinMini::SetZOrder(int newZOrder)
{
	if (thisWnd)
	{
		if (newZOrder == 2 && (zOrder == 0 || zOrder == 1))
			::SetWindowPos(thisWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		else if ((newZOrder == 0 || newZOrder == 1) && zOrder == 2)
			::SetWindowPos(thisWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

	zOrder = newZOrder;
}

bool SkinMini::IsSkinFile(const std::wstring& programPath, const std::wstring& skinName, ZipFile* zipFile)
{
	std::wstring file;
	if (zipFile == nullptr)
	{
		file += programPath;
		file += L"Skin";
		file.push_back('\\');
		file += skinName;
		file.push_back('\\');
	}
	file += L"Mini.xml";

	if (zipFile == nullptr)
	{
		if (!FileSystem::Exists(file))
			return false;

		return true;
	}

	return zipFile->CheckFileInZip(file);
}

bool SkinMini::LoadSkin(const std::wstring& programPath, const std::wstring& skinName, ZipFile* zipFile)
{
	return skinDraw.LoadSkin(programPath, skinName, zipFile, true);
}

bool SkinMini::LoadWindows()
{
	for (std::size_t i = 0, isize = skinDraw.Layouts().size(); i < isize; ++i)
	{
		for (std::size_t j = 0, jsize = skinDraw.Layouts()[i]->elements.size(); j < jsize; ++j)
		{
			SkinElement* element = skinDraw.Layouts()[i]->elements[j]->element.get();

			if (element->type == SkinElement::Type::Spectrum)
			{
				visuals.emplace_back(new SkinVis());
				
				visuals.back()->NewWindow(thisWnd);
				visuals.back()->LoadSkin(element->skinName, element->zipFile);
				
				element->SetWindow(visuals.back()->Wnd());
			}
		}
	}

	return true;
}

void SkinMini::OnSize(UINT nType, int cx, int cy)
{
	if (nType == SIZE_MINIMIZED)
	{
		skinDraw.Minimized();
	}
	else if (nType == SIZE_RESTORED || nType == SIZE_MAXIMIZED) // Also when resized
	{
		moveResize.Resize(cx, cy);
		skinDraw.Resize(cx, cy, TRUE);
	}
}

void SkinMini::OnMouseMove(UINT nFlags, CPoint point)
{
	SkinElement* element = skinDraw.MouseMove(nFlags, point);

	if (element)
		Action(element, MouseAction::Move);
}

void SkinMini::OnMouseLeave()
{
	moveResize.MouseLeave();
	skinDraw.MouseLeave();
}

void SkinMini::OnLButtonDown(UINT nFlags, CPoint point)
{
	//::SetFocus(thisWnd);

	SkinElement* element = skinDraw.MouseDown(nFlags, point);

	if (skinDraw.IsPressElement())
	{
		::SetCapture(thisWnd);
		if (element)
			Action(element, MouseAction::Down);
	}
	else // Move window
	{
		skinDraw.EmptyClick();

		moveResize.MouseDown(thisWnd, point, skinDraw.IsStyleBorder());
	}
}

void SkinMini::OnLButtonUp(UINT nFlags, CPoint point)
{
	::ReleaseCapture();

	SkinElement* element = skinDraw.MouseUp(nFlags, point);

	if (element)
		Action(element, MouseAction::Up);
}

void SkinMini::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (!skinDraw.IsClickElement())
		::SendMessage(wndParent, UWM_MINIPLAYER, 0, 0);
}

void SkinMini::OnNcHitTest(CPoint point)
{
	CPoint pt = point;
	::ScreenToClient(thisWnd, &pt);

	if (skinDraw.IsHoverElement())
		moveResize.MouseMove(thisWnd, pt, true);
	else
		moveResize.MouseMove(thisWnd, pt, skinDraw.IsStyleBorder());
}

bool SkinMini::OnSetCursor(HWND hWnd, UINT nHitTest, UINT message)
{
//	if (message == 0) // When open a menu set default cursor
//	{
//		::SetCursor(::LoadCursorW(NULL, IDC_ARROW));
//		return true;
//	}
	if (message == 0) // When open a menu set default cursor
		return false;

	// hWnd == thisWnd is needed because sometimes a wrong cursor is set
	// if mouse is not over the main window and over playlist/library for example
	if (hWnd == thisWnd && moveResize.SetCursor() && !skinDraw.IsHoverElement())
		return true;

	return false;
}

void SkinMini::OnPaint(HDC dc, PAINTSTRUCT& ps)
{
	skinDraw.Paint(dc, ps);
}

void SkinMini::Action(SkinElement* element, MouseAction mouseAction)
{
	SkinElement::Type type = element->type;
	int param = element->GetParam();

	switch (type)
	{
		case SkinElement::Type::Minimize:
			skinDraw.MouseLeave();
			skinDraw.SetVisible(false);
			break;

		case SkinElement::Type::Maximize:
			break;

		case SkinElement::Type::MiniPlayer:
			::SendMessage(wndParent, UWM_MINIPLAYER, (WPARAM)0, 0);
			break;

		case SkinElement::Type::Button:
			skinDraw.ChangeTrigger(&element->skinTrigger, 0);
			break;

		case SkinElement::Type::Switch:
			skinDraw.ChangeTrigger(&element->skinTrigger, !param);
			skinDraw.DrawTriggerSwitch(element, !param);
			break;

		default:
			::SendMessage(wndParent, UWM_ACTION, (WPARAM)mouseAction, (LPARAM)element);
			break;
	}
}

void SkinMini::OnContextMenu(HWND hWnd, CPoint point)
{
	if (hWnd == thisWnd)
		contextMenu->ShowMainMenu(wndParent, point);
}


void SkinMini::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMaxTrackSize.x = skinDraw.GetMaxSize().cx;
	lpMMI->ptMaxTrackSize.y = skinDraw.GetMaxSize().cy;

	lpMMI->ptMinTrackSize.x = skinDraw.GetMinSize().cx;
	lpMMI->ptMinTrackSize.y = skinDraw.GetMinSize().cy;
}

void SkinMini::SetVisible(bool isVisible)
{
	skinDraw.SetVisible(isVisible);
}

void SkinMini::SetTransparency(int transparency)
{
	skinDraw.SetOpacity(255 - (transparency * 255 / 100));
}


bool SkinMini::OnSysCommand(WPARAM wParam, LPARAM lParam)
{
	// Need to process these messages if mini player uses native window caption
	if ((wParam & 0xFFF0) == SC_MINIMIZE)
	{
		SetVisible(false);
		return true;
	}
	else if ((wParam & 0xFFF0) == SC_RESTORE)
	{
		MessageBeep(1);
		SetVisible(true);
		return true;
	}
	else if ((wParam & 0xFFF0) == SC_MAXIMIZE)
	{
		::SendMessage(wndParent, UWM_MINIPLAYER, 0, 0);
		return true;
	}
	else if ((wParam & 0xFFF0) == SC_CLOSE)
	{
		::DestroyWindow(wndParent);
		return true;
	}

	return false;
}

void SkinMini::OnDestroy()
{
	WINDOWPLACEMENT pl;
	::GetWindowPlacement(thisWnd, &pl);

	rcValidRect = pl.rcNormalPosition;
}

void SkinMini::OnDropFiles(HDROP hDropInfo)
{
	::SendMessage(wndParent, WM_DROPFILES, (WPARAM)hDropInfo, 0);
}

void SkinMini::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
//	if (lpwndpos->hwnd == this->m_hWnd)
//		lpwndpos->flags |= SWP_NOZORDER;

//	lpwndpos->hwndInsertAfter = NULL;
//	if (lpwndpos->hwnd == this->m_hWnd)
//		lpwndpos->flags |= SWP_NOZORDER;
//	lpwndpos->flags |= SWP_NOOWNERZORDER|SWP_NOSENDCHANGING;

	// Window style should be WS_EX_TOPMOST when we use it! Or the main window will be always on desktop too!
	if (zOrder == 1)
		lpwndpos->hwndInsertAfter = HWND_BOTTOM;
	//lpwndpos->flags |= SWP_NOOWNERZORDER;
}

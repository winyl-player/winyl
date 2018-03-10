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
#include "SkinAlpha.h"

SkinAlpha::SkinAlpha()
{

}

SkinAlpha::~SkinAlpha()
{

}

LRESULT SkinAlpha::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
	case WM_TIMER:
		OnTimer((UINT_PTR)wParam);
		return 0;
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
	case WM_SIZE:
		OnSize((UINT)wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
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
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
		return 0;
	}

	return ::DefWindowProc(hWnd, message, wParam, lParam);
}

bool SkinAlpha::NewWindow(HWND parent)
{
	wndParent = parent;

	if (CreateClassWindow(parent, L"SkinAlpha", WS_POPUP, WS_EX_LAYERED|WS_EX_TOOLWINDOW, true))
	{
		return true;
	}

	return false;
}

bool SkinAlpha::LoadSkin(const std::wstring& programPath, const std::wstring& skinName, ZipFile* zipFile)
{
	return skinDraw.LoadSkinAlpha(programPath, skinName, zipFile);
}

void SkinAlpha::OnPaint(HDC dc, PAINTSTRUCT& ps)
{
	skinDraw.Paint(dc, ps);
}

void SkinAlpha::OnSize(UINT nType, int cx, int cy)
{
	if (::IsIconic(wndParent))
	{
		skinDraw.Minimized();
	}
	else
	{
		moveResize->Resize(cx, cy);
		skinDraw.Resize(cx, cy, false);
		skinDraw.RedrawWindow();
	}
}

void SkinAlpha::OnMouseMove(UINT nFlags, CPoint point)
{
	SkinElement* element = skinDraw.MouseMove(nFlags, point);

	if (element)
		Action(element, MouseAction::Move);

	// Tracking tooltip
	if (element && element->type == SkinElement::Type::Volume)
	{
		int thumb = static_cast<SkinSlider*>(element)->GetThumbPosition();
		std::wstring percent = std::to_wstring(element->GetParam() / 1000);
		toolTips.TrackingToolTip(false, percent, element->rcRect.left + thumb, element->rcRect.top);
	}
	else if (element && element->type == SkinElement::Type::Track && *ptrMediaPlay && !*ptrMediaRadio)
	{
		int thumb = static_cast<SkinSlider*>(element)->GetThumbPosition();
		int pos = (element->GetParam() * libAudio->GetTimeLength() + 100000 / 2) / 100000;
		std::wstring time = SkinText::TimeString(pos, false, libAudio->GetTimeLength(), true);
		toolTips.TrackingToolTip(false, time, element->rcRect.left + thumb, element->rcRect.top, true);
	}
	else if (!skinDraw.IsPressElement())
	{
		if (skinDraw.GetHoverElement() && skinDraw.GetHoverElement()->type == SkinElement::Type::Track && *ptrMediaPlay && !*ptrMediaRadio)
		{
			if (isTrackTooltip)
			{
				// https://msdn.microsoft.com/en-us/library/windows/desktop/hh298405%28v=vs.85%29.aspx
				// Make sure the mouse has actually moved. The presence of the tooltip 
				// causes Windows to send the message continuously.
				static POINT oldPoint = {};
				if (point != oldPoint)
				{
					oldPoint = point;
					int percent = static_cast<SkinSlider*>(skinDraw.GetHoverElement())->CalcPercent(point);
					int pos = (percent * libAudio->GetTimeLength() + 100000 / 2) / 100000;
					std::wstring time = SkinText::TimeString(pos, false, libAudio->GetTimeLength(), true);
					toolTips.TrackingToolTip(false, time, point.x, skinDraw.GetHoverElement()->rcRect.top, true);
				}
			}
			else
				::SetTimer(thisWnd, TimerValue::TrackID, TimerValue::Track, NULL);
		}
		else if (isTrackTooltip)
		{
			isTrackTooltip = false;
			toolTips.DeactivateTrackingToolTip();
		}
	}
}

void SkinAlpha::OnMouseLeave()
{
	moveResize->MouseLeave();
	skinDraw.MouseLeave();
	if (isTrackTooltip)
	{
		isTrackTooltip = false;
		toolTips.DeactivateTrackingToolTip();
	}
}

void SkinAlpha::OnLButtonDown(UINT nFlags, CPoint point)
{
	//::SetFocus(thisWnd);

	SkinElement* element = skinDraw.MouseDown(nFlags, point);

	if (skinDraw.IsPressElement())
	{
		::SetCapture(thisWnd);
		if (element)
			Action(element, MouseAction::Down);

		if (element && element->type == SkinElement::Type::Volume)
		{
			int thumb = static_cast<SkinSlider*>(element)->GetThumbPosition();
			std::wstring percent = std::to_wstring(element->GetParam() / 1000);
			toolTips.TrackingToolTip(true, percent, element->rcRect.left + thumb, element->rcRect.top);
		}
		else if (element && element->type == SkinElement::Type::Track && *ptrMediaPlay && !*ptrMediaRadio)
		{
			int thumb = static_cast<SkinSlider*>(element)->GetThumbPosition();
			int pos = (element->GetParam() * libAudio->GetTimeLength() + 100000 / 2) / 100000;
			std::wstring time = SkinText::TimeString(pos, false, libAudio->GetTimeLength(), true);
			toolTips.TrackingToolTip(true, time, element->rcRect.left + thumb, element->rcRect.top, true);
		}
	}
	else // Move window
	{
		skinDraw.EmptyClick();

		moveResize->MouseDown(wndParent, point, skinDraw.IsStyleBorder());
	}
}

void SkinAlpha::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	SkinElement* element = skinDraw.MouseUp(nFlags, point);

	if (element)
		Action(element, MouseAction::Up);

	if (element && element->type == SkinElement::Type::Volume)
		toolTips.DeactivateTrackingToolTip();
	else if (element && element->type == SkinElement::Type::Track && !isTrackTooltip)
		toolTips.DeactivateTrackingToolTip();
}

void SkinAlpha::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (!skinDraw.IsClickElement())
		::SendMessage(wndParent, UWM_MINIPLAYER, 1, 0);
}

void SkinAlpha::OnNcHitTest(CPoint point)
{
	CPoint pt = point;
	::ScreenToClient(thisWnd, &pt);

	if (skinDraw.IsHoverElement())
		moveResize->MouseMove(wndParent, pt, TRUE);
	else
		moveResize->MouseMove(wndParent, pt, skinDraw.IsStyleBorder());
}

bool SkinAlpha::OnSetCursor(HWND hWnd, UINT nHitTest, UINT message)
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
	if (hWnd == thisWnd && moveResize->SetCursor() && !skinDraw.IsHoverElement())
		return true;

	return false;
}

void SkinAlpha::Action(SkinElement* element, MouseAction mouseAction)
{
	SkinElement::Type type = element->type;
	int param = element->GetParam();

	switch (type)
	{
		case SkinElement::Type::Button:
			//skinDraw.ChangeTrigger(&element->trigger, 0);
			break;

		case SkinElement::Type::Switch:
			//skinDraw.ChangeTrigger(&element->trigger, !param);
			//skinDraw.DrawTriggerSwitch(element, !param);
			break;

		default:
			::SendMessage(wndParent, UWM_ACTION, (WPARAM)mouseAction, (LPARAM)element);
			break;
	}
}

void SkinAlpha::OnContextMenu(HWND hWnd, CPoint point)
{
	if (hWnd == thisWnd)
		contextMenu->ShowMainMenu(wndParent, point); 
}

void SkinAlpha::OnNotify(WPARAM wParam, LPARAM lParam)
{
	LPNMTTDISPINFO nmtt = reinterpret_cast<LPNMTTDISPINFO>(lParam);
	if (nmtt->hdr.hwndFrom == toolTips.GetTipWnd() && nmtt->hdr.code == TTN_NEEDTEXT)
	{
		const std::wstring* text = toolTips.GetText((SkinElement*)wParam);
		if (text)
			nmtt->lpszText = (LPWSTR)text->c_str();
	}
}
/*
void SkinAlpha::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CWnd::OnActivate(nState, pWndOther, bMinimized);

//	if (nState == WA_ACTIVE || nState == WA_CLICKACTIVE)
//		pParentWnd->SendMessage(WM_ACTIVATE, MAKELONG(nState, 0), (LPARAM)pParentWnd->m_hWnd);

//	pParentWnd->BringWindowToTop();
}*/

void SkinAlpha::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TimerValue::TrackID)
	{
		isTrackTooltip = true;
		::KillTimer(thisWnd, TimerValue::TrackID);

		if (!skinDraw.IsPressElement() && skinDraw.GetHoverElement() && skinDraw.GetHoverElement()->type == SkinElement::Type::Track)
		{
			POINT point = {};
			::GetCursorPos(&point);
			::ScreenToClient(thisWnd, &point);
			int percent = static_cast<SkinSlider*>(skinDraw.GetHoverElement())->CalcPercent(point);
			int pos = (percent * libAudio->GetTimeLength() + 100000 / 2) / 100000;
			std::wstring time = SkinText::TimeString(pos, false, libAudio->GetTimeLength(), true);
			toolTips.TrackingToolTip(true, time, point.x, skinDraw.GetHoverElement()->rcRect.top, true);
		}
	}
}
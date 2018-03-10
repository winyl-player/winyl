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
#include "SkinScroll.h"

SkinScroll::SkinScroll()
{

}

SkinScroll::~SkinScroll()
{

}

LRESULT SkinScroll::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
	case WM_LBUTTONDOWN:
		OnLButtonDown((UINT)wParam, CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		return 0;
	case WM_LBUTTONUP:
		OnLButtonUp((UINT)wParam, CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		return 0;
	}

	return ::DefWindowProc(hWnd, message, wParam, lParam);
}

bool SkinScroll::CreateScroll(HWND parent, const std::wstring& fileSkin, ZipFile* zipFile)
{
	if (CreateClassWindow(parent, L"SkinScroll", WS_CHILDWINDOW|WS_VISIBLE, 0))
	{
		if (LoadSkin(fileSkin, zipFile))
		{
			if (imTrack.IsValid())
				scrollWidth = imTrack.Width();
			else
				scrollWidth = backSize;
			thumbMinSize = imThumbBegin[0].Height() + 
				imThumbFill[0].Height() + imThumbEnd[0].Height();

			if (sizeArrowUp == 0)
				sizeArrowUp = imArrowUp[0].Height();
			if (sizeArrowDown == 0)
				sizeArrowDown = imArrowDown[0].Height();

			return true;
		}
	}

	return false;
}

bool SkinScroll::LoadSkin(const std::wstring& file, ZipFile* zipFile)
{
	std::wstring path = PathEx::PathFromFile(file);

	XmlFile xmlFile;

	if (xmlFile.LoadEx(file, zipFile))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Scroll");
		
		if (xmlMain)
		{
			XmlNode xmlSize = xmlMain.FirstChild("Size");
			if (xmlSize)
			{
				xmlSize.Attribute("ArrowUp", &sizeArrowUp);
				xmlSize.Attribute("ArrowDown", &sizeArrowDown);
			}

			XmlNode xmlTrack = xmlMain.FirstChild("Track");
			if (xmlTrack)
			{
				std::wstring attr = xmlTrack.Attribute16("File");
				if (!attr.empty())
					imTrack.LoadEx(path + attr, zipFile);
				else
				{
					const char* color = xmlTrack.AttributeRaw("Color");
					if (color && strlen(color) == 6)
					{
						DWORD c = strtoul(color, 0, 16);
						backColor = RGB(GetBValue(c), GetGValue(c), GetRValue(c));
					}

					xmlTrack.Attribute("Size", &backSize);
				}
			}

			XmlNode xmlButtonUp = xmlMain.FirstChild("ArrowUp");
			if (xmlButtonUp)
			{
				XmlNode xmlNormal = xmlButtonUp.FirstChild("Normal");
				if (xmlNormal)
				{
					std::wstring attr = xmlNormal.Attribute16("File");
					if (!attr.empty())
						imArrowUp[0].LoadEx(path + attr, zipFile);
				}

				XmlNode xmlHover = xmlButtonUp.FirstChild("Hover");
				if (xmlHover)
				{
					std::wstring attr = xmlHover.Attribute16("File");
					if (!attr.empty())
						imArrowUp[1].LoadEx(path + attr, zipFile);
				}

				XmlNode xmlPress = xmlButtonUp.FirstChild("Press");
				if (xmlPress)
				{
					std::wstring attr = xmlPress.Attribute16("File");
					if (!attr.empty())
						imArrowUp[2].LoadEx(path + attr, zipFile);
				}
			}

			XmlNode xmlButtonDown = xmlMain.FirstChild("ArrowDown");
			if (xmlButtonDown)
			{
				XmlNode xmlNormal = xmlButtonDown.FirstChild("Normal");
				if (xmlNormal)
				{
					std::wstring attr = xmlNormal.Attribute16("File");
					if (!attr.empty())
						imArrowDown[0].LoadEx(path + attr, zipFile);
				}

				XmlNode xmlHover = xmlButtonDown.FirstChild("Hover");
				if (xmlHover)
				{
					std::wstring attr = xmlHover.Attribute16("File");
					if (!attr.empty())
						imArrowDown[1].LoadEx(path + attr, zipFile);
				}

				XmlNode xmlPress = xmlButtonDown.FirstChild("Press");
				if (xmlPress)
				{
					std::wstring attr = xmlPress.Attribute16("File");
					if (!attr.empty())
						imArrowDown[2].LoadEx(path + attr, zipFile);
				}
			}

			XmlNode xmlThumb = xmlMain.FirstChild("Thumb");
			if (xmlThumb)
			{
				XmlNode xmlNormal = xmlThumb.FirstChild("Normal");
				if (xmlNormal)
				{
					XmlNode xmlFill = xmlNormal.FirstChild("Fill");
					if (xmlFill)
					{
						std::wstring attr = xmlFill.Attribute16("File");
						if (!attr.empty())
							imThumbFill[0].LoadEx(path + attr, zipFile);
					}

					XmlNode xmlBegin = xmlNormal.FirstChild("Begin");
					if (xmlBegin)
					{
						std::wstring attr = xmlBegin.Attribute16("File");
						if (!attr.empty())
							imThumbBegin[0].LoadEx(path + attr, zipFile);
					}

					XmlNode xmlEnd = xmlNormal.FirstChild("End");
					if (xmlEnd)
					{
						std::wstring attr = xmlEnd.Attribute16("File");
						if (!attr.empty())
							imThumbEnd[0].LoadEx(path + attr, zipFile);
					}

					XmlNode xmlMiddle = xmlNormal.FirstChild("Middle");
					if (xmlMiddle)
					{
						std::wstring attr = xmlMiddle.Attribute16("File");
						if (!attr.empty())
							imThumbMiddle[0].LoadEx(path + attr, zipFile);
					}
				}

				XmlNode xmlHover = xmlThumb.FirstChild("Hover");
				if (xmlHover)
				{
					XmlNode xmlFill = xmlHover.FirstChild("Fill");
					if (xmlFill)
					{
						std::wstring attr = xmlFill.Attribute16("File");
						if (!attr.empty())
							imThumbFill[1].LoadEx(path + attr, zipFile);
					}

					XmlNode xmlBegin = xmlHover.FirstChild("Begin");
					if (xmlBegin)
					{
						std::wstring attr = xmlBegin.Attribute16("File");
						if (!attr.empty())
							imThumbBegin[1].LoadEx(path + attr, zipFile);
					}

					XmlNode xmlEnd = xmlHover.FirstChild("End");
					if (xmlEnd)
					{
						std::wstring attr = xmlEnd.Attribute16("File");
						if (!attr.empty())
							imThumbEnd[1].LoadEx(path + attr, zipFile);
					}

					XmlNode xmlMiddle = xmlHover.FirstChild("Middle");
					if (xmlMiddle)
					{
						std::wstring attr = xmlMiddle.Attribute16("File");
						if (!attr.empty())
							imThumbMiddle[1].LoadEx(path + attr, zipFile);
					}
				}

				XmlNode xmlPress = xmlThumb.FirstChild("Press");
				if (xmlPress)
				{
					XmlNode xmlFill = xmlPress.FirstChild("Fill");
					if (xmlFill)
					{
						std::wstring attr = xmlFill.Attribute16("File");
						if (!attr.empty())
							imThumbFill[2].LoadEx(path + attr, zipFile);
					}

					XmlNode xmlBegin = xmlPress.FirstChild("Begin");
					if (xmlBegin)
					{
						std::wstring attr = xmlBegin.Attribute16("File");
						if (!attr.empty())
							imThumbBegin[2].LoadEx(path + attr, zipFile);
					}

					XmlNode xmlEnd = xmlPress.FirstChild("End");
					if (xmlEnd)
					{
						std::wstring attr = xmlEnd.Attribute16("File");
						if (!attr.empty())
							imThumbEnd[2].LoadEx(path + attr, zipFile);
					}

					XmlNode xmlMiddle = xmlPress.FirstChild("Middle");
					if (xmlMiddle)
					{
						std::wstring attr = xmlMiddle.Attribute16("File");
						if (!attr.empty())
							imThumbMiddle[2].LoadEx(path + attr, zipFile);
					}
				}
			}
		}
	}
	else
		return false;

	return true;
}

void SkinScroll::OnPaint(HDC dc, PAINTSTRUCT& ps)
{
	//if (::IsWindowVisible(thisWnd))
		DrawScroll(dc);
}

void SkinScroll::Redraw()
{
	//if (::IsWindowVisible(thisWnd))
	{
		HDC dc = ::GetDC(thisWnd);
		if (dc)
		{
			DrawScroll(dc);
			::ReleaseDC(thisWnd, dc);
		}
	}
}

void SkinScroll::DrawScroll(HDC dc)
{
//	MessageBeep(1);

	CRect rc;
	::GetClientRect(thisWnd, rc);

	// New dcMem //
	HDC dcMemory = ::CreateCompatibleDC(dc);

	HBITMAP bmMemory = ExImage::Create32BppBitmap(rc.Width(), rc.Height());
	
	HGDIOBJ oldMemory = ::SelectObject(dcMemory, bmMemory);
	// End dcMem //

	if (bmBack == NULL)
	{
		::SetBkColor(dcMemory, backColor);
		ExImage::FillDC(dcMemory, 0, 0, rc.Width(), rc.Height());
	}
	else
	{
		CRect rcWnd = rc;
		::MapWindowPoints(thisWnd, ::GetParent(thisParentWnd), (LPPOINT)&rcWnd, 2);

		HDC dcBack = ::CreateCompatibleDC(dc);
		HGDIOBJ oldBack = ::SelectObject(dcBack, *bmBack);

		::BitBlt(dcMemory, 0, 0, rcWnd.Width(), rcWnd.Height(), dcBack, rcWnd.left, rcWnd.top, SRCCOPY);

		::SelectObject(dcBack, oldBack);
		::DeleteDC(dcBack);
	}

	// Draw background
	if (imTrack.IsValid())
		imTrack.Draw(dcMemory, 0, imArrowUp[0].Height(), imTrack.Width(), rc.Height() - imArrowUp[0].Height() - imArrowDown[0].Height());

	// Draw up down buttons
	if (isPressUp)
		imArrowUp[2].Draw(dcMemory, 0, 0);
	else if (isHoverUp)
		imArrowUp[1].Draw(dcMemory, 0, 0);
	else
		imArrowUp[0].Draw(dcMemory, 0, 0);

	if (isPressDown)
		imArrowDown[2].Draw(dcMemory, 0, rc.bottom - imArrowDown[0].Height());
	else if (isHoverDown)
		imArrowDown[1].Draw(dcMemory, 0, rc.bottom - imArrowDown[0].Height());
	else
		imArrowDown[0].Draw(dcMemory, 0, rc.bottom - imArrowDown[0].Height());
		
		

	// Calc additional values for thumb
	int thumbTop = imThumbBegin[0].Height();
	int thumbBottom = imThumbEnd[0].Height();

	int thumbFill = thumbSize - (thumbTop + thumbBottom);

	// Draw thumb
	if (isPressThumb)
	{
		imThumbBegin[2].Draw(dcMemory, 0, thumbPos);
		imThumbFill[2].Draw(dcMemory, 0, thumbPos + thumbTop, imThumbFill[2].Width(), thumbFill);
		imThumbEnd[2].Draw(dcMemory, 0, thumbPos + thumbTop + thumbFill);

		if (imThumbMiddle[2].IsValid())
			imThumbMiddle[2].Draw(dcMemory, 0, thumbPos + (thumbSize / 2) - (imThumbMiddle[0].Height() / 2));
	}
	else if (isHoverThumb)
	{
		imThumbBegin[1].Draw(dcMemory, 0, thumbPos);
		imThumbFill[1].Draw(dcMemory, 0, thumbPos + thumbTop, imThumbFill[1].Width(), thumbFill);
		imThumbEnd[1].Draw(dcMemory, 0, thumbPos + thumbTop + thumbFill);

		if (imThumbMiddle[1].IsValid())
			imThumbMiddle[1].Draw(dcMemory, 0, thumbPos + (thumbSize / 2) - (imThumbMiddle[1].Height() / 2));
	}
	else
	{
		imThumbBegin[0].Draw(dcMemory, 0, thumbPos);
		imThumbFill[0].Draw(dcMemory, 0, thumbPos + thumbTop, imThumbFill[0].Width(), thumbFill);
		imThumbEnd[0].Draw(dcMemory, 0, thumbPos + thumbTop + thumbFill);

		if (imThumbMiddle[0].IsValid())
			imThumbMiddle[0].Draw(dcMemory, 0, thumbPos + (thumbSize / 2) - (imThumbMiddle[0].Height() / 2));
	}

	// Copy dcMem //
	::BitBlt(dc, rc.left, rc.top, rc.Width(), rc.Height(), dcMemory, rc.left, rc.top, SRCCOPY);

	::SelectObject(dcMemory, oldMemory);

	::DeleteObject(bmMemory);
	::DeleteDC(dcMemory);
	// Copy dcMem //
}

bool SkinScroll::SetScrollPos(int pos, bool needRedraw)
{
	//scrollPosOld = scrollPos;

	pos = std::max(scrollMin, std::min(scrollMax - scrollPage, pos));

	if (pos == scrollPos)
		return false;
	else
	{
		scrollPos = pos;

		CalcThumbByPos(scrollPos);

		if (needRedraw)
		{
			::InvalidateRect(thisWnd, NULL, FALSE);
			//::UpdateWindow(thisWnd); // Do not!
		}

		return true;
	}
}

void SkinScroll::SetScrollInfo(int min, int max, int line, int page)
{
	scrollMin = min;
	scrollMax = max;
	scrollPage = page;
	scrollLine = line;

	scrollHeight = scrollPage;

	// Adjust scroll position
	scrollPos = std::max(scrollMin, std::min(scrollMax - scrollPage, scrollPos));

	CalcThumbByPos(scrollPos);

	::InvalidateRect(thisWnd, NULL, FALSE);
	//::UpdateWindow(thisWnd); // Do not!
}

void SkinScroll::ShowScroll(bool isShow)
{
	if (isShow)
		::ShowWindow(thisWnd, SW_SHOW);
	else
		::ShowWindow(thisWnd, SW_HIDE);
}

void SkinScroll::OnLButtonDown(UINT nFlags, CPoint point)
{
	::SetCapture(thisWnd);

	thumbMove = thumbPos - point.y;
	scrollPosReturn = scrollPos;

	if (nFlags & MK_SHIFT)
	{
		isShiftDown = true;
		ScrollThumb(point);
	}

	SetPress(point);

	if (actionType != ActionType::Thumb && actionType != ActionType::None)
	{
		ScrollUpDown();
		StartTimer(false);
	}
}

void SkinScroll::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	isShiftDown = false;

	StopTimer();

	ResetPress();
}

void SkinScroll::ScrollMessage()
{
	::SendMessage(thisParentWnd, WM_VSCROLL, SB_THUMBPOSITION, 0);
}

void SkinScroll::OnMouseMove(UINT nFlags, CPoint point)
{
	if (actionType == ActionType::Thumb)
	{
		ScrollThumb(point);
	}
	else if (actionType == ActionType::None)
	{
		SetHover(point);
	}
	else
	{
		ActionType type = GetActionType(point);

		if (type == actionType && !isTimer)
		{
			StartTimer(true);

			if (IsRedrawPressSet(actionType))
				::InvalidateRect(thisWnd, NULL, FALSE);
		}
		else if (type != actionType && isTimer)
		{
			StopTimer();

			if (IsRedrawPressReset(actionType))
				::InvalidateRect(thisWnd, NULL, FALSE);
		}
	}
}

void SkinScroll::OnMouseLeave()
{
	ResetHover();
}

void SkinScroll::StartTimer(bool isFast)
{
	if (!isFast)
		::SetTimer(thisWnd, 1, 300, NULL);
	else
	{
		ScrollUpDown();
		::SetTimer(thisWnd, 0, 50, NULL);
	}
	
	isTimer = true;
}

void SkinScroll::StopTimer()
{
	isTimer = false;

	::KillTimer(thisWnd, 1);
	::KillTimer(thisWnd, 0);
}

void SkinScroll::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		::KillTimer(thisWnd, 1);
		ScrollUpDown();
		::SetTimer(thisWnd, 0, 50, NULL);
	}
	else if (nIDEvent == 0)
	{
		if (actionType == ActionType::PageUp || actionType == ActionType::PageDown)
		{
			CPoint pt;
			::GetCursorPos(&pt);
			::ScreenToClient(thisWnd, &pt);

			if (GetActionType(pt, false) == ActionType::Thumb)
				StopTimer();
			else
				ScrollUpDown();
		}
		else
			ScrollUpDown();
	}
}

void SkinScroll::ScrollUpDown()
{
	int pos = scrollPos;

	if (actionType == ActionType::LineUp)
		pos -= scrollLine;
	else if (actionType == ActionType::LineDown)
		pos += scrollLine;
	else if (actionType == ActionType::PageUp)
		pos -= scrollPage;
	else if (actionType == ActionType::PageDown)
		pos += scrollPage;

	pos = std::max(scrollMin, std::min(scrollMax - scrollPage, pos));

	if (pos != scrollPos)
	{
		scrollPosOld = scrollPos;

		scrollPos = pos;
		CalcThumbByPos(scrollPos);

		Redraw();

		ScrollMessage();
	}
}

void SkinScroll::ScrollThumb(CPoint& pt)
{
	CRect rc;
	::GetClientRect(thisWnd, rc);

	// If mouse outside scroll area return to previous pos
	if (!(pt.x < rc.left - 135 || pt.x > rc.right + 135 ||
		pt.y < rc.top - 35 || pt.y > rc.bottom + 35))
	{
		int thumbPosOld = thumbPos;

		if (!isShiftDown)
			thumbPos = thumbMove + pt.y;
		else
			thumbPos = pt.y - thumbSize / 2;

		thumbPos = std::max(sizeArrowUp, std::min(scrollHeight - thumbSize - sizeArrowDown, thumbPos));

		if (thumbPos != thumbPosOld)
		{
			scrollPosOld = scrollPos;

			scrollPos = CalcPosByThumb();
			scrollPos = std::max(scrollMin, std::min(scrollMax - scrollPage, scrollPos));

			Redraw();

			ScrollMessage();
		}
	}
	else
	{
		if (scrollPos != scrollPosReturn)
		{
			scrollPosOld = scrollPos;

			scrollPos = scrollPosReturn;
			CalcThumbByPos(scrollPos);

			Redraw();

			ScrollMessage();
		}
	}
}

void SkinScroll::CalcThumbByPos(int pos)
{
	// thumbSize = (scrollHeight - (sizeArrowUp + sizeArrowDown)) * scrollPage / std::max(1, scrollMax - scrollMin);
	// thumbSize = (int)(0.5f + ((float)((scrollHeight - (sizeArrowUp + sizeArrowDown)) * scrollPage) / (float)std::max(1, scrollMax - scrollMin)));
	thumbSize = MulDivide(scrollHeight - (sizeArrowUp + sizeArrowDown), scrollPage, std::max(1, scrollMax - scrollMin));

	thumbSize = std::max(thumbMinSize, thumbSize);

	int thumbPage = scrollHeight - thumbSize - (sizeArrowUp + sizeArrowDown);

	// thumbPos = sizeArrowUp + (pos * thumbPage) / std::max(1, scrollMax - scrollMin - scrollPage);
	// thumbPos = (int)(0.5f + ((float)sizeArrowUp + (float)(pos * thumbPage) / (float)std::max(1, scrollMax - scrollMin - scrollPage)));
	thumbPos = sizeArrowUp + MulDivide(pos, thumbPage, std::max(1, scrollMax - scrollMin - scrollPage));
}

int SkinScroll::CalcPosByThumb()
{
	int thumbPage = scrollHeight - thumbSize - (sizeArrowUp + sizeArrowDown);

	// return (thumbPos - sizeArrowUp) * (scrollMax - scrollMin - scrollPage) / std::max(1, thumbPage);
	// return (int)(0.5f + ((float)((thumbPos - sizeArrowUp) * (scrollMax - scrollMin - scrollPage)) / (float)std::max(1, thumbPage)));
	return MulDivide(thumbPos - sizeArrowUp, scrollMax - scrollMin - scrollPage, std::max(1, thumbPage));
}

int SkinScroll::MulDivide(int number, int numerator, int denominator)
{
	// Same as ::MulDiv
	
	assert(denominator != 0);
	
	// Remove this to crash on divide by zero
	if (denominator == 0)
		denominator = 1;
	
	// Remove this to always rounded up (with this rounded to the nearest integer)
	if (denominator < 0)
	{
		denominator = -denominator;
		numerator = -numerator;
	}

	if (((number >= 0) && (numerator >= 0)) ||
		((number <  0) && (numerator <  0)))
		return (int)((((long long)number * numerator) + (denominator / 2)) / denominator);
	else
		return (int)((((long long)number * numerator) - (denominator / 2)) / denominator);

	// Overflow check
	//if ((result > 2147483647) || (result < -2147483647)) return -1;
}

SkinScroll::ActionType SkinScroll::GetActionType(const CPoint &pt, bool isPageUpDown)
{
	if (pt.x < 0 || pt.x >= scrollWidth)
		return ActionType::None;

	if (pt.y >= thumbPos && pt.y < (thumbPos + thumbSize))
		return ActionType::Thumb;
	else if (pt.y >= 0 && pt.y < sizeArrowUp)
		return ActionType::LineUp;
	else if (pt.y < scrollHeight && pt.y >= scrollHeight - sizeArrowDown)
		return ActionType::LineDown;
	else if (isPageUpDown)
	{
		if (pt.y >= 0 && pt.y < thumbPos)
			return ActionType::PageUp;
		else if (pt.y < scrollHeight && pt.y >= (thumbPos + thumbSize))
			return ActionType::PageDown;
	}

	return ActionType::None;
}

void SkinScroll::SetPress(const CPoint &pt)
{
	if (isShiftDown)
	{
		actionType = ActionType::Thumb;

		if (IsRedrawPressSet(ActionType::Thumb))
			::InvalidateRect(thisWnd, NULL, FALSE);

		return;
	}

	ActionType type = GetActionType(pt);

	if (type == ActionType::Thumb)
	{
		actionType = ActionType::Thumb;

		if (IsRedrawPressSet(ActionType::Thumb))
			::InvalidateRect(thisWnd, NULL, FALSE);
	}
	else if (type == ActionType::LineUp)
	{
		actionType = ActionType::LineUp;

		if (IsRedrawPressSet(ActionType::LineUp))
			::InvalidateRect(thisWnd, NULL, FALSE);
	}
	else if (type == ActionType::LineDown)
	{
		actionType = ActionType::LineDown;

		if (IsRedrawPressSet(ActionType::LineDown))
			::InvalidateRect(thisWnd, NULL, FALSE);
	}
	else if (type == ActionType::PageUp)
	{
		actionType = ActionType::PageUp;
	}
	else if (type == ActionType::PageDown)
	{
		actionType = ActionType::PageDown;
	}
}

void SkinScroll::ResetPress()
{
	actionType = ActionType::None;

	if (IsRedrawPressReset(ActionType::Thumb) ||
		IsRedrawPressReset(ActionType::LineUp) ||
		IsRedrawPressReset(ActionType::LineDown))
		::InvalidateRect(thisWnd, NULL, FALSE);
}

void SkinScroll::SetHover(const CPoint &pt)
{
	ActionType type = GetActionType(pt, false);

	if (type == ActionType::Thumb)
	{
		if (IsRedrawHoverReset(ActionType::LineUp) ||
			IsRedrawHoverReset(ActionType::LineDown))
			::InvalidateRect(thisWnd, NULL, FALSE);

		if (IsRedrawHoverSet(ActionType::Thumb))
			::InvalidateRect(thisWnd, NULL, FALSE);
	}
	else if (type == ActionType::LineUp)
	{
		if (IsRedrawHoverReset(ActionType::Thumb) ||
			IsRedrawHoverReset(ActionType::LineDown))
			::InvalidateRect(thisWnd, NULL, FALSE);

		if (IsRedrawHoverSet(ActionType::LineUp))
			::InvalidateRect(thisWnd, NULL, FALSE);
	}
	else if (type == ActionType::LineDown)
	{
		if (IsRedrawHoverReset(ActionType::Thumb) ||
			IsRedrawHoverReset(ActionType::LineUp))
			::InvalidateRect(thisWnd, NULL, FALSE);

		if (IsRedrawHoverSet(ActionType::LineDown))
			::InvalidateRect(thisWnd, NULL, FALSE);
	}
	else
	{
		if (IsRedrawHoverReset(ActionType::Thumb) ||
			IsRedrawHoverReset(ActionType::LineUp) ||
			IsRedrawHoverReset(ActionType::LineDown))
			::InvalidateRect(thisWnd, NULL, FALSE);
	}
}

void SkinScroll::ResetHover()
{
	if (IsRedrawHoverReset(ActionType::Thumb) ||
		IsRedrawHoverReset(ActionType::LineUp) ||
		IsRedrawHoverReset(ActionType::LineDown))
		::InvalidateRect(thisWnd, NULL, FALSE);
}

bool SkinScroll::IsRedrawHoverSet(ActionType type)
{
	if (type == ActionType::Thumb && !isHoverThumb)
	{
		if (imThumbFill[1].IsValid())
		{
			isHoverThumb = true;
			return true;
		}
	}
	if (type == ActionType::LineUp && !isHoverUp)
	{
		if (imArrowUp[1].IsValid())
		{
			isHoverUp = true;
			return true;
		}
	}
	if (type == ActionType::LineDown && !isHoverDown)
	{
		if (imArrowDown[1].IsValid())
		{
			isHoverDown = true;
			return true;
		}
	}

	return false;
}

bool SkinScroll::IsRedrawHoverReset(ActionType type)
{
	if (type == ActionType::Thumb && isHoverThumb)
	{
		isHoverThumb = false;
		return true;
	}
	if (type == ActionType::LineUp && isHoverUp)
	{
		isHoverUp = false;
		return true;
	}
	if (type == ActionType::LineDown && isHoverDown)
	{
		isHoverDown = false;
		return true;
	}

	return false;
}

bool SkinScroll::IsRedrawPressSet(ActionType type)
{
	if (type == ActionType::Thumb && !isPressThumb)
	{
		if (imThumbFill[2].IsValid())
		{
			isPressThumb = true;
			return true;
		}
	}
	if (type == ActionType::LineUp && !isPressUp)
	{
		if (imArrowUp[2].IsValid())
		{
			isPressUp = true;
			return true;
		}
	}
	if (type == ActionType::LineDown && !isPressDown)
	{
		if (imArrowDown[2].IsValid())
		{
			isPressDown = true;
			return true;
		}
	}

	return false;
}

bool SkinScroll::IsRedrawPressReset(ActionType type)
{
	if (type == ActionType::Thumb && isPressThumb)
	{
		isPressThumb = false;
		return true;
	}
	if (type == ActionType::LineUp && isPressUp)
	{
		isPressUp = false;
		return true;
	}
	if (type == ActionType::LineDown && isPressDown)
	{
		isPressDown = false;
		return true;
	}

	return false;
}

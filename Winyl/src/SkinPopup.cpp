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
#include "SkinPopup.h"
#include "FileSystem.h"

SkinPopup::SkinPopup()
{
	//dcScreen = ::GetDC(NULL);
}

SkinPopup::~SkinPopup()
{
	//if (dcScreen)
	//	::ReleaseDC(NULL, dcScreen);

	if (dcWindow)
		::ReleaseDC(thisWnd, dcWindow);

	if (bmMemory)
		::DeleteObject(bmMemory);
}

LRESULT SkinPopup::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
	case WM_CONTEXTMENU:
		OnContextMenu((HWND)wParam, CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		return 0;
	//case WM_SHOWWINDOW:
	//	// Forbid the system to hide the window (or when minimize the main window this window will hide too)
	//	return 0;
	}

	return ::DefWindowProc(hWnd, message, wParam, lParam);
}

bool SkinPopup::NewWindow(HWND parent)
{
	wndParent = parent;

	if (CreateClassWindow(NULL, L"SkinPopup", WS_POPUP, WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE))
	{
		dcWindow = ::GetDC(thisWnd);

		return true;
	}

	return false;
}

void SkinPopup::Popup()
{
	if (!isSkinLoad)
		return;

	timerHold = setTimerHold;
	timerShow = setTimerShow;
	timerHide = setTimerHide;

	position = setPosition;
	showEffect = setShowEffect;
	hideEffect = setHideEffect;

	PrepareWindow();
	SetTimerAction(TimerAction::Show);
}

void SkinPopup::OnTimer(UINT_PTR nIDEvent)
{
	if (timerAction == TimerAction::Hold) // Waiting
	{
		if (isMouseOnWindow || isMenuPopup)
			SetTimerAction(TimerAction::None);
		else
			SetTimerAction(TimerAction::Hide);
	}
	else if (timerAction == TimerAction::Show) // Showing
	{
		timer += (255 * TimerValue::Div / (float)timerShow);

		if (timer > 255)
		{
			FadeWindow(true, false);

			if (showEffect == EffectType::Vert)
				SlideVertWindow(true, false);
			else if (showEffect == EffectType::Horz)
				SlideHorzWindow(true, false);

			SetTimerAction(TimerAction::Hold);
		}
		else
		{
			FadeWindow(false, false);

			if (showEffect == EffectType::Vert)
				SlideVertWindow(false, false);
			else if (showEffect == EffectType::Horz)
				SlideHorzWindow(false, false);
		}
	}
	else if (timerAction == TimerAction::Hide) // Hiding
	{
		timer -= (255 * TimerValue::Div / (float)timerHide);

		if (timer < 0)
		{
			FadeWindow(false, true);

			if (hideEffect == EffectType::Vert)
				SlideVertWindow(false, true);
			else if (hideEffect == EffectType::Horz)
				SlideHorzWindow(false, true);

			SetTimerAction(TimerAction::None);
		}
		else
		{
			FadeWindow(false, false);

			if (hideEffect == EffectType::Vert)
				SlideVertWindow(false, false);
			else if (hideEffect == EffectType::Horz)
				SlideHorzWindow(false, false);
		}
	}
}

void SkinPopup::SetTimerAction(TimerAction action)
{
	::KillTimer(thisWnd, TimerValue::ID);

	timerAction = action;

	if (action == TimerAction::Hold)
		::SetTimer(thisWnd, TimerValue::ID, timerHold, NULL);
	else if (action == TimerAction::Show)
	{
		timer = 0;
		::SetTimer(thisWnd, TimerValue::ID, TimerValue::Time, NULL);
	}
	else if (action == TimerAction::Hide)
	{
		timer = 255;
		::SetTimer(thisWnd, TimerValue::ID, TimerValue::Time, NULL);
	}
}

void SkinPopup::PrepareWindow()
{
	::ShowWindow(thisWnd, SW_HIDE);

	CRect rcRect;

	// Get desktop working area (depending on the number of monitors)
	if (::GetSystemMetrics(SM_CMONITORS) > 1)
	{
		HMONITOR hMon = MonitorFromWindow(wndParent, MONITOR_DEFAULTTONEAREST);

		MONITORINFO mi;
		mi.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(hMon, &mi);

		rcRect = mi.rcWork;
	}
	else
	{
		RECT rc;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
		rcRect = rc;
	}

	// Get popup window position (also apply offset from the corner)
	if (position == PositionType::TopLeft)
	{
		rcRect.right = rcRect.left + width;
		rcRect.bottom = rcRect.top + height;

		rcRect.MoveToXY(rcRect.left + MarginDef::SizeX, rcRect.top + MarginDef::SizeY);
	}
	else if (position == PositionType::TopRight)
	{
		rcRect.left = rcRect.right - width;
		rcRect.bottom = rcRect.top + height;

		rcRect.MoveToXY(rcRect.left - MarginDef::SizeX, rcRect.top + MarginDef::SizeY);
	}
	else if (position == PositionType::BottomLeft)
	{
		rcRect.right = rcRect.left + width;
		rcRect.top = rcRect.bottom - height;

		rcRect.MoveToXY(rcRect.left + MarginDef::SizeX, rcRect.top - MarginDef::SizeY);
	}
	else if (position == PositionType::BottomRight)
	{
		rcRect.left = rcRect.right - width;
		rcRect.top = rcRect.bottom - height;

		rcRect.MoveToXY(rcRect.left - MarginDef::SizeX, rcRect.top - MarginDef::SizeY);
	}

	rcWindow = rcRect;

	// For slide effect the position will be offscreen
	if (showEffect == EffectType::Horz)
	{
		if (position == PositionType::TopLeft || position == PositionType::BottomLeft)
			rcRect.MoveToX(rcRect.left - (width + MarginDef::SizeX));
		else if (position == PositionType::TopRight || position == PositionType::BottomRight)
			rcRect.MoveToX(rcRect.left + (width + MarginDef::SizeX));
	}
	else if (showEffect == EffectType::Vert)
	{
		if (position == PositionType::TopLeft || position == PositionType::TopRight)
			rcRect.MoveToY(rcRect.top - (height + MarginDef::SizeY));
		else if (position == PositionType::BottomLeft || position == PositionType::BottomRight)
			rcRect.MoveToY(rcRect.top + (height + MarginDef::SizeY));
	}

	::MoveWindow(thisWnd, rcRect.left, rcRect.top, rcRect.Width(), rcRect.Height(), TRUE);

	// Draw window and set transparency to zero
	if (!isLayered)
	{
		if (valueCorners)
		{
			HRGN rgn;
			rgn = ::CreateRoundRectRgn(0, 0, rcRect.Width()+1, rcRect.Height()+1, valueCorners, valueCorners);
			::SetWindowRgn(thisWnd, rgn, TRUE);
		}
		else
			::SetWindowRgn(thisWnd, NULL, TRUE);

		RedrawWindowDefault();

		//if (isEffectFade)
			::SetLayeredWindowAttributes(thisWnd, NULL, 0, LWA_ALPHA);
		//else
		//	::SetLayeredWindowAttributes(thisWnd, NULL, 255, LWA_ALPHA);
	}
	else
	{
		RedrawWindowLayered();

		//if (bEffectFade)
			AlphaWindowLayered(0);
	}
	
	::SetWindowPos(thisWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOSIZE);
	::ShowWindow(thisWnd, SW_SHOWNOACTIVATE);
	
	if (!isLayered)
	{
		// Call InvalidateRect only after the window becomes visible
		// or there is a strange bug when slide effect (the window doesn't update)
		::InvalidateRect(thisWnd, NULL, TRUE);
	}
}

void SkinPopup::SlideHorzWindow(bool isLastStart, bool isLastEnd)
{
	if (isLastStart)
		::MoveWindow(thisWnd, rcWindow.left, rcWindow.top, rcWindow.Width(), rcWindow.Height(), TRUE);
	else if (isLastEnd)
		::ShowWindow(thisWnd, SW_HIDE);
	else
	{
		CRect rcRect = rcWindow;

		if (position == PositionType::TopLeft || position == PositionType::BottomLeft)
		{
			rcRect.MoveToX(rcRect.left - (width + MarginDef::SizeX));
			rcRect.MoveToX(rcRect.left + ((width + MarginDef::SizeX) * (int)timer) / 255);
		}
		else if (position == PositionType::TopRight || position == PositionType::BottomRight)
		{
			rcRect.MoveToX(rcRect.left + (width + MarginDef::SizeX));
			rcRect.MoveToX(rcRect.left - ((width + MarginDef::SizeX) * (int)timer) / 255);
		}

		::MoveWindow(thisWnd, rcRect.left, rcRect.top, rcRect.Width(), rcRect.Height(), TRUE);
	}
}

void SkinPopup::SlideVertWindow(bool isLastStart, bool isLastEnd)
{
	if (isLastStart)
		::MoveWindow(thisWnd, rcWindow.left, rcWindow.top, rcWindow.Width(), rcWindow.Height(), TRUE);
	else if (isLastEnd)
		::ShowWindow(thisWnd, SW_HIDE);
	else
	{
		CRect rcRect = rcWindow;

		if (position == PositionType::TopLeft || position == PositionType::TopRight)
		{
			rcRect.MoveToY(rcRect.top - (height + MarginDef::SizeY));
			rcRect.MoveToY(rcRect.top + ((height + MarginDef::SizeY) * (int)timer) / 255);
		}
		else if (position == PositionType::BottomLeft || position == PositionType::BottomRight)
		{
			rcRect.MoveToY(rcRect.top + (height + MarginDef::SizeY));
			rcRect.MoveToY(rcRect.top - ((height + MarginDef::SizeY) * (int)timer) / 255);
		}

		::MoveWindow(thisWnd, rcRect.left, rcRect.top, rcRect.Width(), rcRect.Height(), TRUE);
	}
}

void SkinPopup::FadeWindow(bool isLastStart, bool isLastEnd)
{
	if (!isLayered)
	{
		if (isLastStart)
			::SetLayeredWindowAttributes(thisWnd, NULL, 255, LWA_ALPHA);
		else if (isLastEnd)
			::ShowWindow(thisWnd, SW_HIDE);
		else
			::SetLayeredWindowAttributes(thisWnd, NULL, (int)timer, LWA_ALPHA);
	}
	else
	{
		if (isLastStart)
			AlphaWindowLayered(255);
		else if (isLastEnd)
			::ShowWindow(thisWnd, SW_HIDE);
		else
			AlphaWindowLayered((int)timer);
	}
}

void SkinPopup::OnLButtonDown(UINT nFlags, CPoint point)
{
	// When click on the window hide it

//	if (iTimerAction == TIMER_ACTION_HOLD)
//		SetTimerAction(TIMER_ACTION_HIDE);

	SetTimerAction(TimerAction::None);
	FadeWindow(false, true);
}

void SkinPopup::OnMouseMove(UINT nFlags, CPoint point)
{
	// While mouse over the window do not hide it
	isMouseOnWindow = true;
}

void SkinPopup::OnMouseLeave()
{
	// When mouse leave the window hide it

	isMouseOnWindow = false;

	if (timerAction == TimerAction::None)
		SetTimerAction(TimerAction::Hide);
}

void SkinPopup::OnPaint(HDC dc, PAINTSTRUCT& ps)
{
	if (isLayered) // Just in case
		return;

	CRect rc = ps.rcPaint; // Get invalidated rect

	HDC dcMemory = ::CreateCompatibleDC(dc);
	HGDIOBJ oldMemory = ::SelectObject(dcMemory, bmMemory);

	// Transfer from window cache into window
	::BitBlt(dc, rc.left, rc.top, rc.Width(), rc.Height(), dcMemory, rc.left, rc.top, SRCCOPY);

	// Release resources
	::SelectObject(dcMemory, oldMemory);
	::DeleteDC(dcMemory);
}

void SkinPopup::RedrawWindowDefault()
{
	CRect rcClient;
	::GetClientRect(thisWnd, rcClient);

	HDC dcMemory = ::CreateCompatibleDC(dcScreen);
	
	if (bmMemory) ::DeleteObject(bmMemory);
	bmMemory = ExImage::Create32BppBitmap(rcClient.Width(), rcClient.Height());

	HGDIOBJ oldMemory = ::SelectObject(dcMemory, bmMemory);

	::SetBkColor(dcMemory, RGB(255, 255, 255));
	::ExtTextOut(dcMemory, 0, 0, ETO_OPAQUE, &rcClient, NULL, 0, NULL);


	// Draw background
	if (isBackground)
		imBackground[0].Draw(dcMemory, 0, 0);
	else
	{
		imBackground[1].Draw(dcMemory, 0, 0);
		imBackground[0].Draw(dcMemory, imBackground[1].Width(), 0,
			width - imBackground[1].Width() - imBackground[2].Width(), imBackground[0].Height());
		imBackground[2].Draw(dcMemory, width - imBackground[2].Width(), 0);
	}

	// Draw elements
	for (std::size_t i = 0, size = elements.size(); i < size; ++i)
	{
		elements[i]->UpdateRect(rcClient);
		elements[i]->Draw(dcMemory, false);
	}

	// Release resources
	::SelectObject(dcMemory, oldMemory);
	::DeleteDC(dcMemory);

//	InvalidateRect moved to PrepareWindow check it to see why
//	::InvalidateRect(thisWnd, NULL, TRUE);
}

void SkinPopup::RedrawWindowLayered()
{
	CRect rcClient;
	::GetClientRect(thisWnd, rcClient);

	HDC dcMemory = ::CreateCompatibleDC(dcScreen);
	
	if (bmMemory) ::DeleteObject(bmMemory);
	bmMemory = ExImage::Create32BppBitmap(rcClient.Width(), rcClient.Height());

	HGDIOBJ oldMemory = ::SelectObject(dcMemory, bmMemory);


	// Draw background
	if (isBackground)
		imBackground[0].Draw(dcMemory, 0, 0);
	else
	{
		imBackground[1].Draw(dcMemory, 0, 0);
		imBackground[0].Draw(dcMemory, imBackground[1].Width(), 0,
			width - imBackground[1].Width() - imBackground[2].Width(), imBackground[0].Height());
		imBackground[2].Draw(dcMemory, width - imBackground[2].Width(), 0);
	}

	// Draw elements
	for (std::size_t i = 0, size = elements.size(); i < size; ++i)
	{
		elements[i]->UpdateRect(rcClient);
		elements[i]->Draw(dcMemory, true);
	}

	// We can draw layered window right away

	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

	CRect rcWindow;
	::GetWindowRect(thisWnd, rcWindow);

	POINT ptSrc = {0, 0};
	POINT ptDst = {rcWindow.left, rcWindow.top};
	SIZE sz = {rcWindow.Width(), rcWindow.Height()};

	// Use layered function to draw layered window
	::UpdateLayeredWindow(thisWnd, NULL, &ptDst, &sz, dcMemory, &ptSrc, 0, &bf, ULW_ALPHA);

	// Release resources
	::SelectObject(dcMemory, oldMemory);
	::DeleteDC(dcMemory);
}

void SkinPopup::AlphaWindowLayered(int opacity)
{
	HDC dcMemory = ::CreateCompatibleDC(dcScreen);
	
	HGDIOBJ oldMemory = ::SelectObject(dcMemory, bmMemory);

	BLENDFUNCTION bf = {AC_SRC_OVER, 0, (BYTE)opacity, AC_SRC_ALPHA};

	CRect rcWindow;
	::GetWindowRect(thisWnd, rcWindow);

	POINT ptSrc = {0, 0};
	POINT ptDst = {rcWindow.left, rcWindow.top};
	SIZE sz = {rcWindow.Width(), rcWindow.Height()};

	// Use layered function to draw layered window
	::UpdateLayeredWindow(thisWnd, NULL, &ptDst, &sz, dcMemory, &ptSrc, 0, &bf, ULW_ALPHA);

	// Release resources
	::SelectObject(dcMemory, oldMemory);
	::DeleteDC(dcMemory);
}

void SkinPopup::UnloadSkin()
{
	// This function is deprecated
	assert(false);

	SetTimerAction(TimerAction::None);

	::ShowWindow(thisWnd, SW_HIDE);

	// SetLayeredWindowAttributes and UpdateLayeredWindow doesn't like each other so remove and set layered style
	//ModifyStyleEx(WS_EX_LAYERED, 0);
	//ModifyStyleEx(0, WS_EX_LAYERED);

	isLayered = false;
	valueCorners = 0;

	imBackground[0].Clear();
	imBackground[1].Clear();
	imBackground[2].Clear();

	elements.clear();

	isSkinLoad = false;
}

bool SkinPopup::IsSkinFile(const std::wstring& programPath, const std::wstring& skinName, ZipFile* zipFile)
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
	file += L"Popup.xml";

	if (zipFile == nullptr)
	{
		if (!FileSystem::Exists(file))
			return false;

		return true;
	}

	return zipFile->CheckFileInZip(file);
}

bool SkinPopup::LoadSkin(const std::wstring& programPath, const std::wstring& skinName, ZipFile* zipFile)
{
	std::wstring path;
	if (zipFile == nullptr)
	{
		path += programPath;
		path += L"Skin";
		path.push_back('\\');
		path += skinName;
		path.push_back('\\');
	}

	std::wstring file = path + L"Popup.xml";

	XmlFile xmlFile;

	if (xmlFile.LoadEx(file, zipFile))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Popup");

		if (xmlMain)
		{
			XmlNode xmlTransparent = xmlMain.FirstChild("Transparent");
			if (xmlTransparent)
				xmlTransparent.Attribute("AlphaBlend", &isLayered);

			XmlNode xmlCorners = xmlMain.FirstChild("Corners");
			if (xmlCorners)
				xmlCorners.Attribute("All", &valueCorners);

			XmlNode xmlSize = xmlMain.FirstChild("Size");
			if (xmlSize)
			{
				xmlSize.Attribute("MinWidth", &minWidth);
				xmlSize.Attribute("MaxWidth", &maxWidth);
			}

			XmlNode xmlBackground = xmlMain.FirstChild("Background");
			if (xmlBackground)
			{
				XmlNode xmlFill = xmlBackground.FirstChild("Fill");
				if (xmlFill)
				{
					isBackground = false;

					std::wstring attr = xmlFill.Attribute16("File");
					if (!attr.empty())
					{
						imBackground[0].LoadEx(path + attr, zipFile);
						height = imBackground[0].Height();
					}

					XmlNode xmlBegin = xmlBackground.FirstChild("Begin");
					if (xmlBegin)
					{
						std::wstring attr = xmlBegin.Attribute16("File");
						if (!attr.empty())
							imBackground[1].LoadEx(path + attr, zipFile);
					}

					XmlNode xmlEnd = xmlBackground.FirstChild("End");
					if (xmlEnd)
					{
						std::wstring attr = xmlEnd.Attribute16("File");
						if (!attr.empty())
							imBackground[2].LoadEx(path + attr, zipFile);
					}
				}
				else
				{
					isBackground = true;

					std::wstring attr = xmlBackground.Attribute16("File");
					if (!attr.empty())
					{
						imBackground[0].LoadEx(path + attr, zipFile);
						width = imBackground[0].Width();
						height = imBackground[0].Height();
					}
				}
			}

			XmlNode xmlElements = xmlMain.FirstChild("Elements");

			if (xmlElements)
			{
				for (XmlNode xmlLoad = xmlElements.FirstChild(); xmlLoad; xmlLoad = xmlLoad.NextChild())
				{
					elements.emplace_back(new SkinPopupElement());
					elements.back()->LoadSkin(xmlLoad, path, zipFile);
				}
			}
		}
	}
	else
		return false;

	isSkinLoad = true;
	return true;
}

void SkinPopup::SetText(const std::wstring& title, const std::wstring& album, const std::wstring& artist,
						const std::wstring& genre, const std::wstring& year, int time)
{
	int newWidth = 0;

	for (std::size_t i = 0, size = elements.size(); i < size; ++i)
	{
		SkinElement* element = elements[i]->element.get();
		SkinPopupElement* popupElement = elements[i].get();

		if (element->type == SkinElement::Type::Title)
		{
			static_cast<SkinText*>(element)->SetText(title);
			if (!isBackground)
			{
				int w = static_cast<SkinText*>(element)->GetTextWidth(dcWindow) +
					popupElement->pos.left + popupElement->pos.right;
				if (w > newWidth) newWidth = w;
			}
		}
		else if (element->type == SkinElement::Type::Artist)
		{
			static_cast<SkinText*>(element)->SetText(artist);
			if (!isBackground)
			{
				int w = static_cast<SkinText*>(element)->GetTextWidth(dcWindow) +
					popupElement->pos.left + popupElement->pos.right;
				if (w > newWidth) newWidth = w;
			}
		}
		else if (element->type == SkinElement::Type::Album)
		{
			static_cast<SkinText*>(element)->SetText(album);
			if (!isBackground)
			{
				int w = static_cast<SkinText*>(element)->GetTextWidth(dcWindow) +
					popupElement->pos.left + popupElement->pos.right;
				if (w > newWidth) newWidth = w;
			}
		}
		else if (element->type == SkinElement::Type::Year)
		{
			static_cast<SkinText*>(element)->SetText(year);
			if (!isBackground)
			{
				int w = static_cast<SkinText*>(element)->GetTextWidth(dcWindow) +
					popupElement->pos.left + popupElement->pos.right;
				if (w > newWidth) newWidth = w;
			}
		}
		else if (element->type == SkinElement::Type::Genre)
		{
			static_cast<SkinText*>(element)->SetText(genre);
			if (!isBackground)
			{
				int w = static_cast<SkinText*>(element)->GetTextWidth(dcWindow) +
					popupElement->pos.left + popupElement->pos.right;
				if (w > newWidth) newWidth = w;
			}
		}
		else if (element->type == SkinElement::Type::TimeLength)
		{
			static_cast<SkinText*>(element)->SetTime(time, false);
			if (!isBackground)
			{
				int w = static_cast<SkinText*>(element)->GetTextWidth(dcWindow) +
					popupElement->pos.left + popupElement->pos.right;
				if (w > newWidth) newWidth = w;
			}
		}
	}

	if (!isBackground)
		width = std::max(minWidth, std::min(maxWidth, newWidth));
}

void SkinPopup::SetCover(ExImage::Source* image)
{
	for (std::size_t i = 0, size = elements.size(); i < size; ++i)
	{
		SkinElement* element = elements[i]->element.get();

		if (element->type == SkinElement::Type::Cover)
			static_cast<SkinCover*>(element)->SetImage(image);
	}
}

void SkinPopup::OnContextMenu(HWND hWnd, CPoint point)
{
	if (timerAction == TimerAction::Hold || timerAction == TimerAction::None)
	{
		isMenuPopup = true;

		::SendMessage(wndParent, UWM_POPUPMENU, NULL, MAKELPARAM(point.x, point.y));

		isMenuPopup = false;

		if (timerAction == TimerAction::None)
			SetTimerAction(TimerAction::Hide);
	}
}

void SkinPopup::Disable()
{
	SetTimerAction(TimerAction::Hide);
}

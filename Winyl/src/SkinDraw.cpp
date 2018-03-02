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

// SkinDraw.cpp : implementation file
//

#include "stdafx.h"
#include "SkinDraw.h"
#include "FileSystem.h"

// SkinDraw

SkinDraw::SkinDraw()
{
	//dcScreen = ::GetDC(NULL);
}

SkinDraw::~SkinDraw()
{
	//if (dcScreen)
	//	::ReleaseDC(NULL, dcScreen);

	if (bmMemory)
		::DeleteObject(bmMemory);

	if (bmBackgd)
		::DeleteObject(bmBackgd);
}

void SkinDraw::SetWindowStyle(HWND wnd, HWND timer)
{
	wndOwner = wnd;
	wndTimer = timer;

	if (isStyleBorder)
	{
		LONG_PTR exStyle = ::GetWindowLongPtr(wndOwner, GWL_EXSTYLE);
		::SetWindowLongPtr(wndOwner, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);

		LONG_PTR style = ::GetWindowLongPtr(wndOwner, GWL_STYLE);
		style &= ~(WS_CAPTION|WS_SIZEBOX|WS_MAXIMIZEBOX);

		if (isStyleBorder)
		{
			style |= WS_SIZEBOX;
			if (isStyleCaption)
			{
				style |= WS_CAPTION;
				if (isStyleMaximize)
				{
					style |= WS_MAXIMIZEBOX;
				}
			}
		}

		::SetWindowLongPtr(wndOwner, GWL_STYLE, style);
	}
	else
	{
		LONG_PTR style = ::GetWindowLongPtr(wndOwner, GWL_STYLE);
		style &= ~(WS_CAPTION|WS_SIZEBOX|WS_MAXIMIZEBOX);

		// Aero Snap Test (the implementation is not completed):
		// Add WS_THICKFRAME|WS_MAXIMIZEBOX to WS_CAPTION here, but maximize size will be incorrect
		// Add +1 to latest line in void WinylWnd::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
		// lpMMI->ptMaxSize.y = rcRect.Height()+1; and see interesting thing, maximize size still incorrect but only on this +1
		// -------
		// To proper fix maximize size:
		// in void SkinDraw::Resize(int cx, int cy, bool needRedraw)
		// change line, only if maximized, layouts[i]->UpdateRectLayout(cxFrame, cyFrame, cx - cxFrame, cy - cyFrame);
		// cxFrame = GetSystemMetrics(SM_CXSIZEFRAME);
		// cyFrame = GetSystemMetrics(SM_CYSIZEFRAME);
		// And, only if Aero Snap enabled, remove if (::IsZoomed(wnd))...
		// from MoveResize::MouseDown(HWND wnd, const CPoint& point, bool isMove)

		// Experimental feature for minimize/restore animation in Win7
		if (!isStyleBorder && !isLayered && !isLayeredAlpha && !isMiniPlayer)
			style |= WS_CAPTION; //|WS_THICKFRAME|WS_MAXIMIZEBOX // WS_CAPTION = WS_BORDER|WS_DLGFRAME minmax.c
		// End of the feature

		::SetWindowLongPtr(wndOwner, GWL_STYLE, style);


		LONG_PTR exStyle = ::GetWindowLongPtr(wndOwner, GWL_EXSTYLE);
		if (isLayered)
			::SetWindowLongPtr(wndOwner, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
		else
			::SetWindowLongPtr(wndOwner, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
	}

	// Reset window region. Should be always after setting styles (below is explanation).
	// This line not only reset region but also apply styles set before: WS_CAPTION|WS_SIZEBOX|WS_MAXIMIZEBOX
	// because something should be done with window (change window size, set region etc.)
	// to apply styles and recalculate client area. If don't do that there will be an issue when skin changed.
	// I'm not even sure that it works properly, if not the better solution is to recreate window with all styles.
	// Added: Now there is also SetWindowPos(SWP_DRAWFRAME) for the same purpose.
	// ::SetWindowRgn(wndOwner, NULL, TRUE);

	if (!isLayered && valueCorners)
	{
		CRect rc; ::GetWindowRect(wndOwner, rc);
		HRGN rgn = ::CreateRoundRectRgn(0, 0, rc.Width()+1, rc.Height()+1, valueCorners, valueCorners);
		::SetWindowRgn(wndOwner, rgn, TRUE);
	}
	else
	{
		// Experimental feature for minimize/restore animation in Win7
		if (!isStyleBorder && !isLayered && !isLayeredAlpha && !isMiniPlayer)
		{
			// Use CreateRoundRectRgn instead of CreateRectRgn, or there is window flickering in Vista
			CRect rc; ::GetWindowRect(wndOwner, rc);
			HRGN rgn = ::CreateRoundRectRgn(0, 0, rc.Width()+1, rc.Height()+1, 0, 0);
			::SetWindowRgn(wndOwner, rgn, TRUE);
		}
		else // End of the feature
			::SetWindowRgn(wndOwner, NULL, TRUE);
	}

	::SetWindowPos(wndOwner, NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	if (!isLayered)
	{
		// Refactoring is needed (reset previous MARGINS)
		if (futureWin->IsCompositionEnabled())
		{
			MARGINS mr = {};
			futureWin->DwmExtendFrameIntoClientArea(wndOwner, &mr);
		}

		EnableDwm(!!futureWin->IsCompositionEnabled());
	}
}

void SkinDraw::EnableDwm(bool isEnable)
{
	if (isAeroGlass)
	{
		isDwmEnabled = isEnable;

		if (isDwmEnabled)
			futureWin->DwmExtendFrameIntoClientArea(wndOwner, &mrGlass);
	}
}

void SkinDraw::EnableDwmShadow()
{
	if (isShadowDwm)
	{
		int ncrp = DWMNCRP_USEWINDOWSTYLE;
		futureWin->DwmSetWindowAttribute(wndOwner, DWMWA_NCRENDERING_POLICY, &ncrp, sizeof(ncrp));
		ncrp = DWMNCRP_ENABLED;
		if (futureWin->DwmSetWindowAttribute(wndOwner, DWMWA_NCRENDERING_POLICY, &ncrp, sizeof(ncrp)) == S_OK)
		{
			MARGINS mr = {};
			mr.cyBottomHeight = 1;
			futureWin->DwmExtendFrameIntoClientArea(wndOwner, &mr);
		}
	}
}

void SkinDraw::Resize(int cx, int cy, bool needRedraw)
{
	if (wndOwner == NULL)
		return;

	for (std::size_t i = 0, size = layouts.size(); i < size; ++i)
	{
		layouts[i]->UpdateRectLayout(0, 0, cx, cy);

		layouts[i]->UpdateRectElements();
	}

	if (needRedraw)
	{
		if (!isLayered && valueCorners)
		{
			// Reminder: Every region change will cause OnSize (WM_SIZE) extra (unnecessary) call
			HRGN rgn = ::CreateRoundRectRgn(0, 0, cx+1, cy+1, valueCorners, valueCorners);
			::SetWindowRgn(wndOwner, rgn, TRUE);
		}
		// Experimental feature for minimize/restore animation in Win7
		else
		{
			if (!isStyleBorder && !isLayered && !isLayeredAlpha && !isMiniPlayer)
			{
				// Use CreateRoundRectRgn instead of CreateRectRgn, or there is window flickering in Vista
				HRGN rgn = ::CreateRoundRectRgn(0, 0, cx+1, cy+1, 0, 0);
				::SetWindowRgn(wndOwner, rgn, TRUE);
			}
		}
		// End of the feature

		if (!::IsIconic(wndOwner) && ::IsWindowVisible(wndOwner))
		{
			if (isLayered)
				RedrawWindowLayered();
			else
			{
				RedrawWindowDefault();
				//::InvalidateRect(wndOwner, NULL, FALSE);
				::RedrawWindow(wndOwner, NULL, NULL, RDW_INVALIDATE|RDW_ALLCHILDREN);
			}
		}
	}
}

void SkinDraw::ChangeTrigger(SkinTrigger* trigger, int state)
{
	CRect rc;
	::GetClientRect(wndOwner, rc);

	for (std::size_t i = 0, isize = layouts.size(); i < isize; ++i)
	{
		for (std::size_t j = 0, jsize = trigger->Layouts().size(); j < jsize; ++j)
		{
			if (trigger->Layouts()[j].id == layouts[i]->id)
			{
				if (layouts[i]->TriggerLayout(rc.Width(), rc.Height(), trigger->Layouts()[j], layouts))
				{
					layouts[i]->UpdateRectElements();
					RedrawLayout(layouts[i].get());
				}
				break;
			}
		}
	}

	//::UpdateWindow(wndOwner);

//	RefreshWindow(); // Sometimes unnecessary call Resize()!
//	RedrawWindow();

//	if (skinDraw2)
//		skinDraw2->ChangeTrigger(trigger, state);
}

void SkinDraw::SplitterLayoutClick(SkinElement* element, int x, int y)
{
	SkinSplitter* splitter = static_cast<SkinSplitter*>(element);

	splitterLayoutX = x; splitterLayoutY = y;

	for (std::size_t i = 0, isize = layouts.size(); i < isize; ++i)
	{
		for (std::size_t j = 0, jsize = splitter->Layouts().size(); j < jsize; ++j)
		{
			if (splitter->Layouts()[j].id == layouts[i]->id)
			{
				layouts[i]->SplitterLayoutClick();
				break;
			}
		}
	}
}


void SkinDraw::SplitterLayout(SkinElement* element, int x, int y)
{
	bool needUpdate = false;

	CRect rc;
	::GetClientRect(wndOwner, rc);

	SkinSplitter* splitter = static_cast<SkinSplitter*>(element);

	for (std::size_t i = 0, isize = layouts.size(); i < isize; ++i)
	{
		for (std::size_t j = 0, jsize = splitter->Layouts().size(); j < jsize; ++j)
		{
			if (splitter->Layouts()[j].id == layouts[i]->id)
			{
				if (layouts[i]->SplitterLayout(x - splitterLayoutX, y - splitterLayoutY, rc.Width(), rc.Height(),
					splitter->Layouts()[j], splitter->GetMaxLimit()))
				{
					layouts[i]->UpdateRectElements();
					RedrawLayout(layouts[i].get());
					needUpdate = true;
				}

				break;
			}
		}
	}

	if (needUpdate)
		::UpdateWindow(wndOwner);
}

void SkinDraw::Paint(HDC dc, PAINTSTRUCT& ps)
{
	// Draw the window in standard way (without layered style)
	// The point of this is to replace invalidated data in the window with a new data in the window cache

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

void SkinDraw::RedrawWindow()
{
	if (wndOwner == NULL)
		return;

//	if (::IsIconic(wndOwner) || !::IsWindowVisible(wndOwner))
//		return;

	if (isLayered)
		RedrawWindowLayered();
	else
	{
		RedrawWindowDefault();
		//::InvalidateRect(wndOwner, NULL, FALSE);
	}
}

void SkinDraw::RefreshWindow()
{
	if (wndOwner == NULL)
		return;

	CRect rc;
	::GetClientRect(wndOwner, rc);

	Resize(rc.Width(), rc.Height(), false);
}

void SkinDraw::RedrawElement(SkinElement* element, bool isForceUpdate)
{
	if (wndOwner == NULL)
		return;

	// !::IsWindowVisible(wndOwner) added after a bug appeared in Win7:
	// layered window loaded wrong after non-layered window,
	// that is strange because before UpdateLayeredWindowIndirect
	// added to RedrawElementDefault everything works fine.
	// It's just a note this check is needed here anyway.
	if (::IsIconic(wndOwner) || !::IsWindowVisible(wndOwner))
		return;

	if (isLayered)
		RedrawElementLayered(element);
	else
	{
		RedrawElementDefault(element);
		::InvalidateRect(wndOwner, element->rcRect, FALSE);
		if (isForceUpdate)
			::UpdateWindow(wndOwner);
	}
}

void SkinDraw::RedrawWindowDefault()
{
	CRect rcClient;
	::GetClientRect(wndOwner, rcClient);

	HDC dcMemory = ::CreateCompatibleDC(dcScreen);
	
	if (bmMemory) ::DeleteObject(bmMemory);
	bmMemory = ExImage::Create32BppBitmap(rcClient.Width(), rcClient.Height());

	HGDIOBJ oldMemory = ::SelectObject(dcMemory, bmMemory);

	if (!isDwmEnabled)
	{
		::SetBkColor(dcMemory, RGB(185, 209, 234)); // RGB(215, 228, 242)
		::ExtTextOut(dcMemory, 0, 0, ETO_OPAQUE, &rcClient, NULL, 0, NULL);
	}

	HDC dcBackgd = ::CreateCompatibleDC(dcScreen);

	if (bmBackgd) ::DeleteObject(bmBackgd);
	bmBackgd = ExImage::Create32BppBitmap(rcClient.Width(), rcClient.Height());

	HGDIOBJ oldBackgd = ::SelectObject(dcBackgd, bmBackgd);

	if (!isDwmEnabled)
	{
		::SetBkColor(dcBackgd, RGB(185, 209, 234)); // RGB(215, 228, 242)
		::ExtTextOut(dcBackgd, 0, 0, ETO_OPAQUE, &rcClient, NULL, 0, NULL);
	}

	for (std::size_t i = 0, size = layouts.size(); i < size; ++i)
	{
		if (!layouts[i]->IsHidden()) // Skip hidden layouts
		{
			layouts[i]->DrawBackground(dcBackgd); // Draw background

			CRect rc = layouts[i]->rcRect;

			// Transfer from background cache into window cache
			::BitBlt(dcMemory, rc.left, rc.top, rc.Width(), rc.Height(), dcBackgd, rc.left, rc.top, SRCCOPY);

			layouts[i]->DrawElements(dcMemory, false); // Draw elements
		}
	}

	// Release resources
	::SelectObject(dcMemory, oldMemory);
	::SelectObject(dcBackgd, oldBackgd);

	::DeleteDC(dcMemory);
	::DeleteDC(dcBackgd);

//	pWnd->Invalidate(); // Do not uncomment! It's just a check that resources are released
}

void SkinDraw::RedrawElementDefault(SkinElement* element)
{
	HDC dcMemory = ::CreateCompatibleDC(dcScreen);
	HGDIOBJ oldMemory = ::SelectObject(dcMemory, bmMemory);

	HDC dcBackgd = ::CreateCompatibleDC(dcScreen);
	HGDIOBJ oldBackgd = ::SelectObject(dcBackgd, bmBackgd);

	CRect rc = element->rcRect;

	// First, draw background for the element (transfer from background cache into window cache)
	::BitBlt(dcMemory, rc.left, rc.top, rc.Width(), rc.Height(), dcBackgd, rc.left, rc.top, SRCCOPY);

	element->Draw(dcMemory, false); // Next, draw the element

	// Release resources
	::SelectObject(dcMemory, oldMemory);
	::SelectObject(dcBackgd, oldBackgd);

	::DeleteDC(dcMemory);
	::DeleteDC(dcBackgd);

//	pWnd->InvalidateRect(pElement->rcRect); // Do not uncomment! It's just a check that resources are released
}

void SkinDraw::RedrawWindowLayered()
{
	CRect rcClient;
	::GetClientRect(wndOwner, rcClient);

	HDC dcMemory = ::CreateCompatibleDC(dcScreen);
	
	if (bmMemory) ::DeleteObject(bmMemory);
	bmMemory = ExImage::Create32BppBitmap(rcClient.Width(), rcClient.Height());

	HGDIOBJ oldMemory = ::SelectObject(dcMemory, bmMemory);

	HDC dcBackgd = ::CreateCompatibleDC(dcScreen);

	if (bmBackgd) ::DeleteObject(bmBackgd);
	bmBackgd = ExImage::Create32BppBitmap(rcClient.Width(), rcClient.Height());

	HGDIOBJ oldBackgd = ::SelectObject(dcBackgd, bmBackgd);


	BLENDFUNCTION bfBack = {AC_SRC_OVER, 0, 255, 0};

	for (std::size_t i = 0, size = layouts.size(); i < size; ++i)
	{
		if (!layouts[i]->IsHidden()) // Skip hidden layouts
		{
			layouts[i]->DrawBackground(dcBackgd); // Draw background

			CRect rc  = layouts[i]->rcRect;

			// Transfer from background cache into window cache
			::GdiAlphaBlend(dcMemory, rc.left, rc.top, rc.Width(), rc.Height(),
				dcBackgd, rc.left, rc.top, rc.Width(), rc.Height(), bfBack);

			layouts[i]->DrawElements(dcMemory, true); // Draw elements
		}
	}

	// We can draw layered window right away

	BLENDFUNCTION bf = {AC_SRC_OVER, 0, (BYTE)valueOpacity, AC_SRC_ALPHA};

	CRect rcWindow; // Window position
	::GetWindowRect(wndOwner, rcWindow);

	POINT ptSrc = {0, 0};
	POINT ptDst = {rcWindow.left, rcWindow.top};
	SIZE sz = {rcWindow.Width(), rcWindow.Height()};

	// Use layered function to draw layered window
	if (!futureWin->IsVistaOrLater())
	{
		::UpdateLayeredWindow(wndOwner, NULL, &ptDst, &sz, dcMemory, &ptSrc, 0, &bf, ULW_ALPHA);
	}
	else // Vista has better layered function
	{
		UPDATELAYEREDWINDOWINFO info = {sizeof(info), NULL, &ptDst, &sz, dcMemory, &ptSrc, 0, &bf, ULW_ALPHA, NULL};
		futureWin->UpdateLayeredWindowIndirect(wndOwner, &info);
	}

	// Release resources
	::SelectObject(dcMemory, oldMemory);
	::SelectObject(dcBackgd, oldBackgd);

	::DeleteDC(dcMemory);
	::DeleteDC(dcBackgd);
}

void SkinDraw::RedrawElementLayered(SkinElement* element)
{
	HDC dcMemory = ::CreateCompatibleDC(dcScreen);
	HGDIOBJ oldMemory = ::SelectObject(dcMemory, bmMemory);

	HDC dcBackgd = ::CreateCompatibleDC(dcScreen);
	HGDIOBJ oldBackgd = ::SelectObject(dcBackgd, bmBackgd);

	CRect rc = element->rcRect;

	// First, draw background for the element (transfer from background cache into window cache)
	BLENDFUNCTION bfBack = {AC_SRC_OVER, 0, 255, 0};
	::GdiAlphaBlend(dcMemory, rc.left, rc.top, rc.Width(), rc.Height(),
		dcBackgd, rc.left, rc.top, rc.Width(), rc.Height(), bfBack);

	element->Draw(dcMemory, true); // Next, draw the element

	// We can draw layered window right away

	BLENDFUNCTION bf = {AC_SRC_OVER, 0, (BYTE)valueOpacity, AC_SRC_ALPHA};

	CRect rcWindow; // Window position
	::GetWindowRect(wndOwner, rcWindow);

	POINT ptSrc = {0, 0};
	POINT ptDst = {rcWindow.left, rcWindow.top};
	SIZE sz = {rcWindow.Width(), rcWindow.Height()};

	// Use layered function to draw layered window
	if (!futureWin->IsVistaOrLater())
	{
		::UpdateLayeredWindow(wndOwner, NULL, &ptDst, &sz, dcMemory, &ptSrc, 0, &bf, ULW_ALPHA);
	}
	else // Vista has better layered function (update only element area)
	{
		UPDATELAYEREDWINDOWINFO info = {sizeof(info), NULL, &ptDst, &sz, dcMemory, &ptSrc, 0, &bf, ULW_ALPHA, rc};
		futureWin->UpdateLayeredWindowIndirect(wndOwner, &info);
	}

	// Release resources
	::SelectObject(dcMemory, oldMemory);
	::SelectObject(dcBackgd, oldBackgd);

	::DeleteDC(dcMemory);
	::DeleteDC(dcBackgd);
}

void SkinDraw::RedrawLayout(SkinLayout* layout)
{
	if (isLayered)
		;
	else
	{
		RedrawLayoutDefault(layout);
		//::InvalidateRect(wndOwner, layout->rcRect, FALSE);
		::RedrawWindow(wndOwner, layout->rcRect, NULL, RDW_INVALIDATE|RDW_ALLCHILDREN);
	}
}

void SkinDraw::RedrawLayoutDefault(SkinLayout* layout)
{
	if (layout->IsHidden())
		return;

	HDC dcMemory = ::CreateCompatibleDC(dcScreen);
	HGDIOBJ oldMemory = ::SelectObject(dcMemory, bmMemory);

	HDC dcBackgd = ::CreateCompatibleDC(dcScreen);
	HGDIOBJ oldBackgd = ::SelectObject(dcBackgd, bmBackgd);


	CRect rc = layout->rcRect;

	//dcBackgd.FillSolidRect(rc, RGB(185, 209, 234)); // RGB(215, 228, 242)

	layout->DrawBackground(dcBackgd); // Draw background

	// Transfer from background cache into window cache
	::BitBlt(dcMemory, rc.left, rc.top, rc.Width(), rc.Height(), dcBackgd, rc.left, rc.top, SRCCOPY);

	layout->DrawElements(dcMemory, false); // Draw elements


	// Release resources
	::SelectObject(dcMemory, oldMemory);
	::SelectObject(dcBackgd, oldBackgd);

	::DeleteDC(dcMemory);
	::DeleteDC(dcBackgd);
}

void SkinDraw::Minimized()
{
	// This function is the same as Resize (0, 0) (Windows does that when minimize window)
	// but with some optimizations and explanations.
	// Also removes caches to free memory when minimize window.

	if (wndOwner == NULL)
		return;

	if (isLayered)
	{
		// This comment and the code looks very old
		// from the time when the logic was different. I don't think it necessary now.
		// -------
		// The point of the following code that we move layered window offscreen
		// (Windows does the same when minimize window) with UpdateLayeredWindow,
		// so when the window is restored there wasn't flickering of windows lying on the main window,
		// that mean that the all windows appears at the same time.

		CRect rcWindow;
		//::GetWindowRect(wndOwner, rcWindow);
		rcWindow.SetRect(-32000, -32000, -31900, -31900);

		HDC dcMemory = ::CreateCompatibleDC(dcScreen);
	
		if (bmMemory) ::DeleteObject(bmMemory);	
		bmMemory = ExImage::Create32BppBitmap(rcWindow.Width(), rcWindow.Height());

		HGDIOBJ oldMemory = ::SelectObject(dcMemory, bmMemory);

		// We can draw layered window right away

		BLENDFUNCTION bf = {AC_SRC_OVER, 0, (BYTE)valueOpacity, AC_SRC_ALPHA};

		POINT ptSrc = {0, 0};
		POINT ptDst = {rcWindow.left, rcWindow.top};
		SIZE sz = {rcWindow.Width(), rcWindow.Height()};

		// Use layered function to draw layered window
		if (!futureWin->IsVistaOrLater())
		{
			::UpdateLayeredWindow(wndOwner, NULL, &ptDst, &sz, dcMemory, &ptSrc, 0, &bf, ULW_ALPHA);
		}
		else // Vista has better layered function
		{
			UPDATELAYEREDWINDOWINFO info = {sizeof(info), NULL, &ptDst, &sz, dcMemory, &ptSrc, 0, &bf, ULW_ALPHA, NULL};
			futureWin->UpdateLayeredWindowIndirect(wndOwner, &info);
		}

		::SelectObject(dcMemory, oldMemory);
		::DeleteDC(dcMemory);

		::DeleteObject(bmBackgd); bmBackgd = NULL;
	}
	else
	{
		::DeleteObject(bmMemory); bmMemory = NULL;
		::DeleteObject(bmBackgd); bmBackgd = NULL;

		// Also set region here, or in Vista when Aero enabled when restore from tray there is window flickering
		// under other conditions everything is fine. In general the same code as in Resize. A strange bug.
		if (valueCorners)
		{
			HRGN rgn = ::CreateRoundRectRgn(0, 0, 100, 100, valueCorners, valueCorners);
			::SetWindowRgn(wndOwner, rgn, TRUE);
		}
		// Experimental feature for minimize/restore animation in Win7
		else
		{
			if (!isStyleBorder && !isLayered && !isLayeredAlpha && !isMiniPlayer)
			{
				// Use CreateRoundRectRgn instead of CreateRectRgn, or there is window flickering in Vista
				HRGN rgn = ::CreateRoundRectRgn(0, 0, 100, 100, 0, 0);
				::SetWindowRgn(wndOwner, rgn, TRUE);
			}
		}
		// End of the feature
	}
}

SkinElement* SkinDraw::MouseMove(unsigned flags, CPoint& point)
{
	hoverElement = nullptr;

	for (std::size_t i = 0, isize = layouts.size(); i < isize; ++i)
	{
		if (layouts[i]->IsHidden())
			continue;

		for (std::size_t j = 0, jsize = layouts[i]->elements.size(); j < jsize; ++j)
		{
			SkinElement* element = nullptr;

			element = layouts[i]->elements[j]->element->OnMouseMove(flags, point);

			if (pressElement && pressElement != element)
				element = nullptr;

			if (layouts[i]->elements[j]->element->IsHover())
				hoverElement = layouts[i]->elements[j]->element.get();

			if (element)
			{
				if (element->IsRedrawHover())
				{
					if (isFadeEnabled && element->IsFadeEffect() && !pressElement)
						RedrawElementFade(element);
					else
						RedrawElement(element, true);
				}

				// Set volume at the same time when move the volume slider
				if (element->IsPress())
				{
					if (element->type == SkinElement::Type::Volume ||
						element->type == SkinElement::Type::Track)
						return element;
				}

				//break;
			}
		}
	}

	return nullptr;
}

void SkinDraw::MouseLeave()
{
	hoverElement = nullptr;

	for (std::size_t i = 0, isize = layouts.size(); i < isize; ++i)
	{
		if (layouts[i]->IsHidden())
			continue;

		for (std::size_t j = 0, jsize = layouts[i]->elements.size(); j < jsize; ++j)
		{
			SkinElement* element = nullptr;

			element = layouts[i]->elements[j]->element->OnMouseLeave();

			if (element)
			{
				if (element->IsRedrawHover())
				{
					if (isFadeEnabled && element->IsFadeEffect() && !pressElement)
						RedrawElementFade(element);
					else
						RedrawElement(element, true);
				}
			}
		}
	}
}

SkinElement* SkinDraw::MouseDown(unsigned flags, CPoint& point)
{
	for (std::size_t i = 0, isize = layouts.size(); i < isize; ++i)
	{
		if (layouts[i]->IsHidden())
			continue;

		for (std::size_t j = 0, jsize = layouts[i]->elements.size(); j < jsize; ++j)
		{
			SkinElement* element = nullptr;

			element = layouts[i]->elements[j]->element->OnButtonDown(flags, point);

//			if (pressElement && pressElement != element)
//				element = nullptr;

			if (layouts[i]->elements[j]->element->IsPress())
			{
				pressElement = layouts[i]->elements[j]->element.get();
				clickElement = pressElement;
			}

			if (element)
			{
				/*if (element->IsPress())
				{
					pressElement = element;
					clickElement = element;
				}*/

				if (element->IsRedrawPress())
					RedrawElement(element, true);

				// Set volume and rating at the same time when click on it
				if (element->type == SkinElement::Type::Volume ||
					element->type == SkinElement::Type::Track  ||
					element->type == SkinElement::Type::Rating)
					return element;

				break;
			}
		}
	}

	return nullptr;
}

SkinElement* SkinDraw::MouseUp(unsigned flags, CPoint& point)
{
	for (std::size_t i = 0, isize = layouts.size(); i < isize; ++i)
	{
		if (layouts[i]->IsHidden())
			continue;

		for (std::size_t j = 0, jsize = layouts[i]->elements.size(); j < jsize; ++j)
		{
			SkinElement* element = nullptr;

			element = layouts[i]->elements[j]->element->OnButtonUp(flags, point);

//			if (pressElement && pressElement != element)
//				element = nullptr;

			if (element)
			{
				pressElement = nullptr;

				// Do not redraw when a new window is appear (it redraw itself on mouse leave)
				if (element->type != SkinElement::Type::Equalizer)
				{
					if (element->IsHover() && element->IsRedrawPress())
						RedrawElement(element);
				}
				else
				{
					if (element->IsHover() && element->IsRedrawPress() && !element->IsRedrawHover())
						RedrawElement(element);
				}

				// For sliders send the action regardless where the mouse button released
				// for others only if release the mouse button on the element.
				if (element->type == SkinElement::Type::Track ||
					element->type == SkinElement::Type::Volume)
				{
					ForceUpdate(); // Redraw sliders as soon as possible
					return element;
				}
				// For rating the action is already sent in MouseDown
				else if (element->IsHover() &&
					element->type != SkinElement::Type::Rating)
					return element;

				break;
			}
		}
	}

	pressElement = nullptr;
	return nullptr;
}

void SkinDraw::DrawPosition(int percent)
{
	for (std::size_t i = 0, isize = layouts.size(); i < isize; ++i)
	{
		for (std::size_t j = 0, jsize = layouts[i]->elements.size(); j < jsize; ++j)
		{
			SkinElement* element = layouts[i]->elements[j]->element.get();

			if (element->type == SkinElement::Type::Track && !element->IsPress())
			{
				element->SetParam(percent);

				if (!element->IsHidden())
					RedrawElement(element);
			}
		}
	}

	if (drawAlpha)
		drawAlpha->DrawPosition(percent);
	if (drawMini)
		drawMini->DrawPosition(percent);
}

void SkinDraw::DrawVolume(int percent)
{
	for (std::size_t i = 0, isize = layouts.size(); i < isize; ++i)
	{
		for (std::size_t j = 0, jsize = layouts[i]->elements.size(); j < jsize; ++j)
		{
			SkinElement* element = layouts[i]->elements[j]->element.get();

			if (element->type == SkinElement::Type::Volume && !element->IsPress())
			{
				element->SetParam(percent);

				if (!element->IsHidden())
					RedrawElement(element);
			}
		}
	}

	if (drawAlpha)
		drawAlpha->DrawVolume(percent);
	if (drawMini)
		drawMini->DrawVolume(percent);
}

void SkinDraw::DrawSwitch(SkinElement::Type type, int state)
{
	for (std::size_t i = 0, isize = layouts.size(); i < isize; ++i)
	{
		for (std::size_t j = 0, jsize = layouts[i]->elements.size(); j < jsize; ++j)
		{
			SkinElement* element = layouts[i]->elements[j]->element.get();

			if (element->type == type)
			{
				element->SetParam(state);

				if (!element->IsHidden())
					RedrawElement(element);
			}
		}
	}
}

void SkinDraw::DrawTriggerSwitch(SkinElement* element, int state)
{
	element->SetParam(state);

	if (!element->IsHidden())
		RedrawElement(element);
}

void SkinDraw::DrawPlay(bool isPlay)
{
	DrawSwitch(SkinElement::Type::PlayPause, isPlay);

	if (drawAlpha)
		drawAlpha->DrawPlay(isPlay);
	if (drawMini)
		drawMini->DrawPlay(isPlay);
}

void SkinDraw::DrawMute(bool isMute)
{
	DrawSwitch(SkinElement::Type::Mute, isMute);

	if (drawAlpha)
		drawAlpha->DrawMute(isMute);
	if (drawMini)
		drawMini->DrawMute(isMute);
}

void SkinDraw::DrawShuffle(bool isShuffle)
{
	DrawSwitch(SkinElement::Type::Shuffle, isShuffle);

	if (drawAlpha)
		drawAlpha->DrawShuffle(isShuffle);
	if (drawMini)
		drawMini->DrawShuffle(isShuffle);
}

void SkinDraw::DrawRepeat(bool isRepeat)
{
	DrawSwitch(SkinElement::Type::Repeat, isRepeat);

	if (drawAlpha)
		drawAlpha->DrawRepeat(isRepeat);	
	if (drawMini)
		drawMini->DrawRepeat(isRepeat);
}

void SkinDraw::DrawMaximize(bool isMaximize)
{
	DrawSwitch(SkinElement::Type::Maximize, isMaximize);

	if (drawAlpha)
		drawAlpha->DrawMaximize(isMaximize);
	if (drawMini)
		drawMini->DrawMaximize(isMaximize);
}

void SkinDraw::DrawCover(ExImage::Source* image)
{
	for (std::size_t i = 0, isize = layouts.size(); i < isize; ++i)
	{
		for (std::size_t j = 0, jsize = layouts[i]->elements.size(); j < jsize; ++j)
		{
			SkinElement* element = layouts[i]->elements[j]->element.get();

			if (element->type == SkinElement::Type::Cover)
			{
				// Default cover is set and try to set default just ignore
				if (!static_cast<SkinCover*>(element)->IsImage() && image == nullptr)
					continue;

				static_cast<SkinCover*>(element)->EnableFade(isFadeEnabled);

				static_cast<SkinCover*>(element)->SetImage(image);

				if (!element->IsHidden())
				{
					if (isFadeEnabled && element->IsFadeEffect())
						RedrawElementFade(element);
					else
						RedrawElement(element);
				}
			}
		}
	}
}

void SkinDraw::DrawText(const std::wstring& title, const std::wstring& album, const std::wstring& artist,
						const std::wstring& genre, const std::wstring& year, int time)
{
	for (std::size_t i = 0, isize = layouts.size(); i < isize; ++i)
	{
		for (std::size_t j = 0, jsize = layouts[i]->elements.size(); j < jsize; ++j)
		{
			SkinElement* element = layouts[i]->elements[j]->element.get();

			if (element->type == SkinElement::Type::Title)
			{
				static_cast<SkinText*>(element)->SetText(title);

				if (!element->IsHidden())
					RedrawElement(element);
			}
			else if (element->type == SkinElement::Type::Artist)
			{
				static_cast<SkinText*>(element)->SetText(artist);

				if (!element->IsHidden())
					RedrawElement(element);
			}
			else if (element->type == SkinElement::Type::Album)
			{
				static_cast<SkinText*>(element)->SetText(album);

				if (!element->IsHidden())
					RedrawElement(element);
			}
			else if (element->type == SkinElement::Type::ArtistTitle)
			{
				static_cast<SkinText*>(element)->SetText(artist);
				static_cast<SkinText*>(element)->SetText2(title);

				if (!element->IsHidden())
					RedrawElement(element);
			}
			else if (element->type == SkinElement::Type::Year)
			{
				static_cast<SkinText*>(element)->SetText(year);

				if (!element->IsHidden())
					RedrawElement(element);
			}
			else if (element->type == SkinElement::Type::Genre)
			{
				static_cast<SkinText*>(element)->SetText(genre);

				if (!element->IsHidden())
					RedrawElement(element);
			}
			else if (element->type == SkinElement::Type::TimeLength)
			{
				static_cast<SkinText*>(element)->SetTime(time, false);

				if (!element->IsHidden())
					RedrawElement(element);
			}
			else if (element->type == SkinElement::Type::TimeElapsed)
			{
				if (time == -1)
					static_cast<SkinText*>(element)->SetTextEmpty();
				else
					static_cast<SkinText*>(element)->SetTime(0, false);

				if (!element->IsHidden())
					RedrawElement(element);
			}
			else if (element->type == SkinElement::Type::TimeRemains)
			{
				if (time == -1)
					static_cast<SkinText*>(element)->SetTextEmpty();
				else
					static_cast<SkinText*>(element)->SetTime(time, true);

				if (!element->IsHidden())
					RedrawElement(element);
			}
			else if (element->type == SkinElement::Type::TimeElapsed2)
			{
				if (time == -1)
					static_cast<SkinText*>(element)->SetTextEmpty();
				else
					static_cast<SkinText*>(element)->SetTime(0, false);

				if (!element->IsHidden())
					RedrawElement(element);
			}
			else if (element->type == SkinElement::Type::TimeRemains2)
			{
				if (time == -1)
					static_cast<SkinText*>(element)->SetTextEmpty();
				else
					static_cast<SkinText*>(element)->SetTime(time, true);

				if (!element->IsHidden())
					RedrawElement(element);
			}
		}
	}

	if (drawAlpha)
		drawAlpha->DrawText(title, album, artist, genre, year, time);
	if (drawMini)
		drawMini->DrawText(title, album, artist, genre, year, time);
}

void SkinDraw::DrawTime(int time, int length, bool isLength)
{
	for (std::size_t i = 0, isize = layouts.size(); i < isize; ++i)
	{
		for (std::size_t j = 0, jsize = layouts[i]->elements.size(); j < jsize; ++j)
		{
			SkinElement* element = layouts[i]->elements[j]->element.get();

			if (element->type == SkinElement::Type::TimeElapsed)
			{
				static_cast<SkinText*>(element)->SetTime(time, false);

				if (!element->IsHidden())
					RedrawElement(element);
			}
			else if (element->type == SkinElement::Type::TimeRemains)
			{
				static_cast<SkinText*>(element)->SetTime(length - time, true);

				if (!element->IsHidden())
					RedrawElement(element);
			}
			if (element->type == SkinElement::Type::TimeElapsed2)
			{
				static_cast<SkinText*>(element)->SetTime(time, false, length, isLength);

				if (!element->IsHidden())
					RedrawElement(element);
			}
			else if (element->type == SkinElement::Type::TimeRemains2)
			{
				static_cast<SkinText*>(element)->SetTime(length - time, true, length, isLength);

				if (!element->IsHidden())
					RedrawElement(element);
			}
		}
	}

	if (drawAlpha)
		drawAlpha->DrawTime(time, length, isLength);
	if (drawMini)
		drawMini->DrawTime(time, length, isLength);
}

void SkinDraw::DrawStatusLine(int count, int total, int time, long long size, Language* lang)
{
	for (std::size_t i = 0, isize = layouts.size(); i < isize; ++i)
	{
		for (std::size_t j = 0, jsize = layouts[i]->elements.size(); j < jsize; ++j)
		{
			SkinElement* element = layouts[i]->elements[j]->element.get();

			if (element->type == SkinElement::Type::StatusLine)
			{
				static_cast<SkinText*>(element)->SetStatusLine(count, total, time, size, false, lang);

				if (!element->IsHidden())
					RedrawElement(element);
			}
			else if (element->type == SkinElement::Type::StatusLine2)
			{
				static_cast<SkinText*>(element)->SetStatusLine(count, total, time, size, true, lang);

				if (!element->IsHidden())
					RedrawElement(element);
			}
		}
	}

	if (drawAlpha)
		drawAlpha->DrawStatusLine(count, total, time, size, lang);
}

void SkinDraw::DrawStatusLineNone()
{
	for (std::size_t i = 0, isize = layouts.size(); i < isize; ++i)
	{
		for (std::size_t j = 0, jsize = layouts[i]->elements.size(); j < jsize; ++j)
		{
			SkinElement* element = layouts[i]->elements[j]->element.get();

			if (element->type == SkinElement::Type::StatusLine ||
				element->type == SkinElement::Type::StatusLine2)
			{
				static_cast<SkinText*>(element)->SetTextEmpty();

				if (!element->IsHidden())
					RedrawElement(element);
			}
		}
	}

	if (drawAlpha)
		drawAlpha->DrawStatusLineNone();
}

void SkinDraw::DrawTextNone()
{
	for (std::size_t i = 0, isize = layouts.size(); i < isize; ++i)
	{
		for (std::size_t j = 0, jsize = layouts[i]->elements.size(); j < jsize; ++j)
		{
			SkinElement* element = layouts[i]->elements[j]->element.get();

			if (element->type == SkinElement::Type::Title        ||
				element->type == SkinElement::Type::Artist       ||
				element->type == SkinElement::Type::Album        ||
				element->type == SkinElement::Type::ArtistTitle  ||
				element->type == SkinElement::Type::Year         ||
				element->type == SkinElement::Type::Genre        ||
				element->type == SkinElement::Type::TimeLength   ||
				element->type == SkinElement::Type::TimeElapsed  ||
				element->type == SkinElement::Type::TimeRemains  ||
				element->type == SkinElement::Type::TimeElapsed2 ||
				element->type == SkinElement::Type::TimeRemains2)
			{
				static_cast<SkinText*>(element)->SetTextEmpty();

				if (!element->IsHidden())
					RedrawElement(element);
			}
		}
	}

	if (drawAlpha)
		drawAlpha->DrawTextNone();
	if (drawMini)
		drawMini->DrawTextNone();
}

void SkinDraw::DrawRating(int rating)
{
	for (std::size_t i = 0, isize = layouts.size(); i < isize; ++i)
	{
		for (std::size_t j = 0, jsize = layouts[i]->elements.size(); j < jsize; ++j)
		{
			SkinElement* element = layouts[i]->elements[j]->element.get();

			if (element->type == SkinElement::Type::Rating)
			{
				element->SetParam(rating);

				if (!element->IsHidden())
					RedrawElement(element);
			}
		}
	}

	if (drawAlpha)
		drawAlpha->DrawRating(rating);
	if (drawMini)
		drawMini->DrawRating(rating);
}

void SkinDraw::DrawSearchClear(bool isShow)
{
	for (std::size_t i = 0, isize = layouts.size(); i < isize; ++i)
	{
		for (std::size_t j = 0, jsize = layouts[i]->elements.size(); j < jsize; ++j)
		{
			SkinElement* element = layouts[i]->elements[j]->element.get();

			if (element->type == SkinElement::Type::SearchClear)
			{
				element->HideReal(!isShow);
				if (!element->IsHiddenLayout())
					RedrawElement(element);
			}
		}
	}
}

void SkinDraw::UnloadSkin()
{
	pressElement = nullptr;
	hoverElement = nullptr;
	clickElement = nullptr;

	isLayered = false;
	isAeroGlass = false;
	isLayeredAlpha = false;
	valueCorners = 0;

	isMiniPlayer = false;

	isStyleBorder = false;
	isStyleCaption = false;
	isStyleMaximize = false;

	isLayoutsSave = false;

	isShadowDwm = false;
	isShadowLayered = false;

	szMinSize.SetSize(DefaultSize::MinSizeX, DefaultSize::MinSizeY);
	szMaxSize.SetSize(DefaultSize::MaxSizeX, DefaultSize::MaxSizeY);
	rcResizeBorder.SetRect(DefaultSize::ResizeLeft, DefaultSize::ResizeTop, DefaultSize::ResizeRight, DefaultSize::ResizeBottom);
	rcMaximizeBorder.SetRect(DefaultSize::MaxLeft, DefaultSize::MaxTop, DefaultSize::MaxRight, DefaultSize::MaxBottom);
	rcAlphaBorder.SetRect(0, 0, 0, 0);

	mrGlass = {-1, -1, -1, -1};

	fadeElements.clear();

	layouts.clear();
}

std::unique_ptr<ZipFile> SkinDraw::NewZipFile(const std::wstring& programPath, const std::wstring& skinName)
{
	std::wstring zipPath = programPath;
	zipPath += L"Skin";
	zipPath.push_back('\\');
	zipPath += skinName;

	std::unique_ptr<ZipFile> zipFile(new ZipFile());
	if (!zipFile->OpenFile(zipPath + L".wzp"))
		zipFile.reset();

	return zipFile;
}

bool SkinDraw::LoadSkin(const std::wstring& programPath, const std::wstring& skinName, ZipFile* zipFile, bool isMini)
{
	UnloadSkin();

	// Compose the file name for example "Skin\Default\Skin.xml"

	std::wstring path;
	if (zipFile == nullptr)
	{
		path += programPath;
		path += L"Skin";
		path.push_back('\\');
		path += skinName;
		path.push_back('\\');
	}

	std::wstring file;
	
	if (!isMini)
		file = path + L"Skin.xml";
	else
	{
		isMiniPlayer = true;
		file = path + L"Mini.xml";
	}

	// AddFontResourceEx(csPath + L"Font\\" + L"Alabama.ttf", FR_PRIVATE, NULL);

	XmlFile xmlFile;

	if (xmlFile.LoadEx(file, zipFile))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Window");

		if (xmlMain)
		{
			XmlNode xmlSkinLayouts = xmlMain.FirstChild("SkinLayouts");
			if (xmlSkinLayouts)
			{
				xmlSkinLayouts.Attribute("Save", &isLayoutsSave);
				layoutsVersion = xmlSkinLayouts.Attribute8("Version");
			}

			XmlNode xmlSize = xmlMain.FirstChild("Size");
			if (xmlSize)
			{
				int minSizeX = 0, minSizeY = 0, maxSizeX = 0, maxSizeY = 0;

				xmlSize.Attribute("MinWidth", &minSizeX);
				xmlSize.Attribute("MinHeight", &minSizeY);
				xmlSize.Attribute("MaxWidth", &maxSizeX);
				xmlSize.Attribute("MaxHeight", &maxSizeY);

				szMinSize.SetSize(minSizeX, minSizeY);
				szMaxSize.SetSize(maxSizeX, maxSizeY);
			}

			XmlNode xmlResizeBorder = xmlMain.FirstChild("ResizeBorder");
			if (xmlResizeBorder)
			{
				int left = 0, top = 0, right = 0, bottom = 0;

				xmlResizeBorder.Attribute("Left", &left);
				xmlResizeBorder.Attribute("Top", &top);
				xmlResizeBorder.Attribute("Right", &right);
				xmlResizeBorder.Attribute("Bottom", &bottom);

				rcResizeBorder.SetRect(left, top, right, bottom);
			}

			XmlNode xmlMaximizeBorder = xmlMain.FirstChild("MaximizeBorder");
			if (xmlMaximizeBorder)
			{
				int left = 0, top = 0, right = 0, bottom = 0;

				xmlMaximizeBorder.Attribute("Left", &left);
				xmlMaximizeBorder.Attribute("Top", &top);
				xmlMaximizeBorder.Attribute("Right", &right);
				xmlMaximizeBorder.Attribute("Bottom", &bottom);

				rcMaximizeBorder.SetRect(left, top, right, bottom);
			}

			XmlNode xmlTransparent = xmlMain.FirstChild("Transparent");
			if (xmlTransparent)
			{
				xmlTransparent.Attribute("AeroGlass", &isAeroGlass);
				if (!isAeroGlass)
				{
					if (isMini)
						xmlTransparent.Attribute("AlphaBlend", &isLayered);
					else
						xmlTransparent.Attribute("AlphaBlend", &isLayeredAlpha);
				}
			}

			XmlNode xmlWindowStyle = xmlMain.FirstChild("WindowStyle");
			if (xmlWindowStyle)
			{
				xmlWindowStyle.Attribute("Border", &isStyleBorder);
				if (isStyleBorder)
				{
					xmlWindowStyle.Attribute("Caption", &isStyleCaption);
					if (isStyleCaption)
					{
						xmlWindowStyle.Attribute("Maximize", &isStyleMaximize);
					}
				}
			}

			XmlNode xmlAeroGlassBorder = xmlMain.FirstChild("AeroGlassBorder");
			if (xmlAeroGlassBorder)
			{
				int left = 0, top = 0, right = 0, bottom = 0;

				xmlAeroGlassBorder.Attribute("Left", &left);
				xmlAeroGlassBorder.Attribute("Top", &top);
				xmlAeroGlassBorder.Attribute("Right", &right);
				xmlAeroGlassBorder.Attribute("Bottom", &bottom);

				mrGlass.cxLeftWidth = left;
				mrGlass.cyTopHeight = top;
				mrGlass.cxRightWidth = right;
				mrGlass.cyBottomHeight = bottom;
			}

			XmlNode xmlAlphaBlendBorder = xmlMain.FirstChild("AlphaBlendBorder");
			if (xmlAlphaBlendBorder)
			{
				int left = 0, top = 0, right = 0, bottom = 0;

				xmlAlphaBlendBorder.Attribute("Left", &left);
				xmlAlphaBlendBorder.Attribute("Top", &top);
				xmlAlphaBlendBorder.Attribute("Right", &right);
				xmlAlphaBlendBorder.Attribute("Bottom", &bottom);

				rcAlphaBorder.SetRect(left, top, right, bottom);
			}

			XmlNode xmlShadow = xmlMain.FirstChild("Shadow");
			if (xmlShadow)
			{
				if (futureWin->IsCompositionEnabled())
					xmlShadow.Attribute("Native", &isShadowDwm);
				if (!isShadowDwm || !futureWin->IsVistaOrLater())
					xmlShadow.Attribute("Composite", &isShadowLayered);
			}

			if (!isLayered)
			{
				XmlNode xmlCorners = xmlMain.FirstChild("Corners");
				if (xmlCorners)
					xmlCorners.Attribute("All", &valueCorners);
			}

			for (XmlNode xmlLayout = xmlMain.FirstChild("Layout"); xmlLayout; xmlLayout = xmlLayout.NextChild())
			{
				layouts.emplace_back(new SkinLayout());
				layouts.back()->LoadSkin(xmlLayout, path, zipFile);
			}
		}
	}
	else
		return false;

	return true;
}

bool SkinDraw::LoadSkinAlpha(const std::wstring& programPath, const std::wstring& skinName, ZipFile* zipFile)
{
	UnloadSkin();

	// Compose the file name for example "Skin\Default\Skin.xml"

	std::wstring path;
	if (zipFile == nullptr)
	{
		path += programPath;
		path += L"Skin";
		path.push_back('\\');
		path += skinName;
		path.push_back('\\');
	}

	std::wstring file = path + L"Alpha.xml";

	// AddFontResourceEx(csPath + L"Font\\" + L"Alabama.ttf", FR_PRIVATE, NULL);

	XmlFile xmlFile;

	if (xmlFile.LoadEx(file, zipFile))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Window");

		if (xmlMain)
		{
			for (XmlNode xmlLayout = xmlMain.FirstChild("Layout"); xmlLayout; xmlLayout = xmlLayout.NextChild())
			{
				layouts.emplace_back(new SkinLayout());
				layouts.back()->LoadSkin(xmlLayout, path, zipFile);
			}
			
			isLayered = true;
		}
	}
	else
		return false;

	return true;
}

bool SkinDraw::LoadSkinSettings(const std::wstring& profilePath, const std::wstring& skinName)
{
	if (!isLayoutsSave)
		return true;

	std::wstring file = profilePath;
	file += L"SkinSettings";
	file.push_back('\\');
	file += skinName + L".xml";
		
	XmlFile xmlFile;

	if (xmlFile.LoadFile(file))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("SkinSettings");

		if (xmlMain)
		{
			std::string version = xmlMain.Attribute8("LayoutsVersion");
			if (layoutsVersion != version)
				return false;

			XmlNode xmlLayout = xmlMain.FirstChild("Layout");

			for (std::size_t i = 0, size = layouts.size(); i < size; ++i)
			{
				if (!xmlLayout)
					break;

				int id = 0;
				xmlLayout.Attribute("ID", &id);

				if (layouts[i]->id == id)
				{
					bool isHide = layouts[i]->isHidden;
					xmlLayout.Attribute("Hide", &isHide);

					if (isHide != layouts[i]->isHidden)
						layouts[i]->HideLayout(isHide);
						
					xmlLayout.Attribute("Width",  &layouts[i]->pos.width);
					xmlLayout.Attribute("Height", &layouts[i]->pos.height);
					xmlLayout.Attribute("Left",   &layouts[i]->pos.left);
					xmlLayout.Attribute("Right",  &layouts[i]->pos.right);
					xmlLayout.Attribute("Top",    &layouts[i]->pos.top);
					xmlLayout.Attribute("Bottom", &layouts[i]->pos.bottom);
					xmlLayout.Attribute("LeftRight", &layouts[i]->pos.leftRight);
					xmlLayout.Attribute("TopBottom", &layouts[i]->pos.topBottom);

					xmlLayout = xmlLayout.NextChild();
				}
			}
		}
	}
	else
		return false;

	return true;
}

bool SkinDraw::SaveSkinSettings(const std::wstring& profilePath, const std::wstring& skinName)
{
	if (!isLayoutsSave)
	{
//		std::wstring file = profilePath + L"SkinSettings.xml";

//		if (FileSystem::Exists(file))
//			FileSystem::RemoveFile(file);

		return true;
	}

	std::wstring file = profilePath;
	file += L"SkinSettings";

	FileSystem::CreateDir(file);

	file.push_back('\\');
	file += skinName + L".xml";

	XmlFile xmlFile;
	XmlNode xmlMain = xmlFile.RootNode().AddChild("SkinSettings");

	if (xmlMain)
	{
		xmlMain.AddAttribute8("LayoutsVersion", layoutsVersion);

		for (std::size_t i = 0, size = layouts.size(); i < size; ++i)
		{
			if (layouts[i]->id > 0)
			{
				XmlNode xmlLayout = xmlMain.AddChild("Layout");
				if (xmlLayout)
				{
					xmlLayout.AddAttribute("ID", layouts[i]->id);
					xmlLayout.AddAttribute("Hide", layouts[i]->isHidden);

					if (layouts[i]->pos.isLeft)
					{
						xmlLayout.AddAttribute("Left", layouts[i]->pos.left);

						if (layouts[i]->pos.isWidth)
							xmlLayout.AddAttribute("Width", layouts[i]->pos.width);
						else if (layouts[i]->pos.isRight)
							xmlLayout.AddAttribute("Right", layouts[i]->pos.right);
						else if (layouts[i]->pos.isLeftRight)
							xmlLayout.AddAttribute("LeftRight", layouts[i]->pos.leftRight);
					}
					else if (layouts[i]->pos.isRight)
					{
						xmlLayout.AddAttribute("Right", layouts[i]->pos.right);

						if (layouts[i]->pos.isWidth)
							xmlLayout.AddAttribute("Width", layouts[i]->pos.width);
						else if (layouts[i]->pos.isLeftRight)
							xmlLayout.AddAttribute("LeftRight", layouts[i]->pos.leftRight);
					}
					else if (layouts[i]->pos.isLeftRight)
					{
						xmlLayout.AddAttribute("LeftRight", layouts[i]->pos.leftRight);

						if (layouts[i]->pos.isWidth)
							xmlLayout.AddAttribute("Width", layouts[i]->pos.width);
					}

					if (layouts[i]->pos.isTop)
					{
						xmlLayout.AddAttribute("Top", layouts[i]->pos.top);

						if (layouts[i]->pos.isHeight)
							xmlLayout.AddAttribute("Height", layouts[i]->pos.height);
						else if (layouts[i]->pos.isBottom)
							xmlLayout.AddAttribute("Bottom", layouts[i]->pos.bottom);
						else if (layouts[i]->pos.isTopBottom)
							xmlLayout.AddAttribute("TopBottom", layouts[i]->pos.topBottom);
					}
					else if (layouts[i]->pos.isBottom)
					{
						xmlLayout.AddAttribute("Bottom", layouts[i]->pos.bottom);

						if (layouts[i]->pos.isHeight)
							xmlLayout.AddAttribute("Height", layouts[i]->pos.height);
						else if (layouts[i]->pos.isTopBottom)
							xmlLayout.AddAttribute("TopBottom", layouts[i]->pos.topBottom);
					}
					else if (layouts[i]->pos.isTopBottom)
					{
						xmlLayout.AddAttribute("TopBottom", layouts[i]->pos.topBottom);

						if (layouts[i]->pos.isHeight)
							xmlLayout.AddAttribute("Height", layouts[i]->pos.height);
					}
				}
			}
		}
	}

	if (xmlFile.SaveFile(file))
		return true;

	return false;
}

void SkinDraw::EnableFade(bool isEnable)
{
	isFadeEnabled = isEnable;
}

void SkinDraw::RedrawElementFade(SkinElement* element)
{
	// If uncomment then if mizimize and restore then window fading of mizimize button is wrong
	if (::IsIconic(wndOwner)) // || !::IsWindowVisible(wndOwner))
		return;

	if (!element->IsHidden())
	{
		bool isAdd = true;
		for (std::size_t i = 0, size = fadeElements.size(); i < size; ++i)
		{
			if (fadeElements[i] == element)
			{
				isAdd = false;
				break;
			}
		}

		if (isAdd)
		{
			fadeElements.push_back(element);
			::SetTimer(wndTimer, 400, 30, NULL); // TIMER_FADE_ID
		}
	}
}

bool SkinDraw::FadeElement()
{
	if (fadeElements.empty())
		return true;

	if (!isLayered)
		return FadeElementDefault();
	else
		return FadeElementLayered();
}

bool SkinDraw::FadeElementDefault()
{
//	if (fadeElements.empty())
//		return true;

	HDC dcMemory = ::CreateCompatibleDC(dcScreen);
	HDC dcBackgd = ::CreateCompatibleDC(dcScreen);

	for (std::size_t i = 0; i < fadeElements.size(); ++i)
	{
		//MessageBeep(1);

		HGDIOBJ oldMemory = ::SelectObject(dcMemory, bmMemory);
		HGDIOBJ oldBackgd = ::SelectObject(dcBackgd, bmBackgd);

		SkinElement* element = fadeElements[i];

		CRect rc = element->rcRect;

		// First, draw background for the element (transfer from background cache into window cache)
		::BitBlt(dcMemory, rc.left, rc.top, rc.Width(), rc.Height(), dcBackgd, rc.left, rc.top, SRCCOPY);

		// Next, draw the element
		if (element->DrawFade(dcMemory, false))
		{
			fadeElements.erase(fadeElements.begin() + i);
			if (i > 0) --i;
		}

		// Release resources
		::SelectObject(dcMemory, oldMemory);
		::SelectObject(dcBackgd, oldBackgd);

		::InvalidateRect(wndOwner, element->rcRect, FALSE);
		::UpdateWindow(wndOwner);
	}

	::DeleteDC(dcMemory);
	::DeleteDC(dcBackgd);

	if (fadeElements.empty())
		return true;

	return false;
}

bool SkinDraw::FadeElementLayered()
{
//	if (fadeElements.empty())
//		return true;

	HDC dcMemory = ::CreateCompatibleDC(dcScreen);
	HDC dcBackgd = ::CreateCompatibleDC(dcScreen);

	HGDIOBJ oldMemory = ::SelectObject(dcMemory, bmMemory);
	HGDIOBJ oldBackgd = ::SelectObject(dcBackgd, bmBackgd);

	for (std::size_t i = 0; i < fadeElements.size(); ++i)
	{
		SkinElement* element = fadeElements[i];

		CRect rc = element->rcRect;

		// First, draw background for the element (transfer from background cache into window cache)
		BLENDFUNCTION bfBack = {0, 0, 255, 0};
		::GdiAlphaBlend(dcMemory, rc.left, rc.top, rc.Width(), rc.Height(),
			dcBackgd, rc.left, rc.top, rc.Width(), rc.Height(), bfBack);

		// Next, draw the element
		if (element->DrawFade(dcMemory, true))
		{
			fadeElements.erase(fadeElements.begin() + i);
			if (i > 0) --i;
		}

		// If WinVista update only element area
		if (futureWin->IsVistaOrLater())
		{
			BLENDFUNCTION bf = {AC_SRC_OVER, 0, (BYTE)valueOpacity, AC_SRC_ALPHA};

			CRect rcWindow; // Window position
			GetWindowRect(wndOwner, rcWindow);

			POINT ptSrc = {0, 0};
			POINT ptDst = {rcWindow.left, rcWindow.top};
			SIZE sz = {rcWindow.Width(), rcWindow.Height()};

			UPDATELAYEREDWINDOWINFO info = {sizeof(info), NULL, &ptDst, &sz, dcMemory, &ptSrc, 0, &bf, ULW_ALPHA, rc};

			// Vista has better layered function (update only element area)
			futureWin->UpdateLayeredWindowIndirect(wndOwner, &info);
		}
	}
	
	// If WinXP update whole window 
	// Call it here for optimimization to update all elements at once, anyway it slower than Vista variant
	if (!futureWin->IsVistaOrLater())
	{
		BLENDFUNCTION bf = {AC_SRC_OVER, 0, (BYTE)valueOpacity, AC_SRC_ALPHA};

		CRect rcWindow; // Window position
		::GetWindowRect(wndOwner, rcWindow);

		POINT ptSrc = {0, 0};
		POINT ptDst = {rcWindow.left, rcWindow.top};
		SIZE sz = {rcWindow.Width(), rcWindow.Height()};

		// Use layered function to draw layered window
		::UpdateLayeredWindow(wndOwner, NULL, &ptDst, &sz, dcMemory, &ptSrc, 0, &bf, ULW_ALPHA);
	}

	// Release resources
	::SelectObject(dcMemory, oldMemory);
	::SelectObject(dcBackgd, oldBackgd);

	::DeleteDC(dcMemory);
	::DeleteDC(dcBackgd);

	if (fadeElements.empty())
		return true;

	return false;
}

void SkinDraw::SetVisible(bool isShow)
{
	if (!isShow)
	{
		::ShowWindow(wndOwner, SW_HIDE);

		if (!isLayered && valueOpacity < 255)
		{
			LONG_PTR exStyle = ::GetWindowLongPtr(wndOwner, GWL_EXSTYLE);
			::SetWindowLongPtr(wndOwner, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
		}

		::DeleteObject(bmMemory); bmMemory = NULL;
		::DeleteObject(bmBackgd); bmBackgd = NULL;
	}
	else
	{
		RedrawWindow();

		if (!isLayered && valueOpacity < 255)
		{
			LONG_PTR exStyle = ::GetWindowLongPtr(wndOwner, GWL_EXSTYLE);
			::SetWindowLongPtr(wndOwner, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);

			::SetLayeredWindowAttributes(wndOwner, NULL, valueOpacity, LWA_ALPHA);
		}

		::ShowWindow(wndOwner, SW_SHOW);
	}
}

void SkinDraw::SetOpacity(int opacity255)
{
	if (wndOwner == NULL)
		return;

	valueOpacity = opacity255;

	if (::IsIconic(wndOwner) || !::IsWindowVisible(wndOwner))
		return;

	if (!isLayered)
	{
		LONG_PTR exStyle = ::GetWindowLongPtr(wndOwner, GWL_EXSTYLE);

		if (valueOpacity < 255)
		{
			::SetWindowLongPtr(wndOwner, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
			::SetLayeredWindowAttributes(wndOwner, NULL, valueOpacity, LWA_ALPHA);
		}
		else
			::SetWindowLongPtr(wndOwner, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
	}
	else
	{
		RedrawWindow();
	}
}

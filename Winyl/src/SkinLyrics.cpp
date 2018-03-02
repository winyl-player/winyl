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
#include "SkinLyrics.h"


SkinLyrics::SkinLyrics()
{

}

SkinLyrics::~SkinLyrics()
{
	if (fontLyrics)
		::DeleteObject(fontLyrics);

	if (fontNoLyrics)
		::DeleteObject(fontNoLyrics);
}

LRESULT SkinLyrics::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
	case WM_SIZE:
		OnSize((UINT)wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_SHOWWINDOW:
		OnShowWindow((BOOL)wParam, (UINT)lParam);
		return 0;
	case WM_VSCROLL:
		OnVScroll(LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
		return 0;
	case WM_CONTEXTMENU:
		OnContextMenu((HWND)wParam, CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		return 0;
	}

	return ::DefWindowProc(hWnd, message, wParam, lParam);
}

bool SkinLyrics::NewWindow(HWND parent)
{
	if (CreateClassWindow(parent, L"SkinLyrics", WS_CHILDWINDOW|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, 0, true))
		return true;

	return false;
}

void SkinLyrics::UpdateLyrics(std::vector<std::wstring>&& newlines)
{
	CRect rc;
	::GetClientRect(thisWnd, rc);

	lines = std::move(newlines);

	linesHeight.clear();
	linesHeight.resize(lines.size());

	HScrollSetPos(0, false);

	ResetScroll(rc.Width(), rc.Height());

	::InvalidateRect(thisWnd, NULL, FALSE);
}

void SkinLyrics::UpdateLyricsNull()
{
	CRect rc;
	::GetClientRect(thisWnd, rc);

	lines.clear();
	linesHeight.clear();

	HScrollSetPos(0, false);

	ResetScroll(rc.Width(), rc.Height());

	::InvalidateRect(thisWnd, NULL, FALSE);
}

bool SkinLyrics::LoadSkin(std::wstring& file, ZipFile* zipFile)
{
	std::wstring path = PathEx::PathFromFile(file);

	XmlFile xmlFile;

	if (xmlFile.LoadEx(file, zipFile))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Lyrics");

		if (xmlMain)
		{
			XmlNode xmlFont = xmlMain.FirstChild("Font");
			if (xmlFont)
			{
				const char* fname = nullptr; int fsize = 0; int fbold = 0; int fitalic = 0; int fclear = 0;

				fname = xmlFont.AttributeRaw("Name");
				xmlFont.Attribute("Size", &fsize);
				xmlFont.Attribute("Bold", &fbold);
				xmlFont.Attribute("Italic", &fitalic);
				xmlFont.Attribute("ClearType", &fclear);

				fontLyrics = ExImage::CreateElementFont(fname, fsize, fbold, fitalic, fclear);

				const char* color = xmlFont.AttributeRaw("Color");
				if (color && strlen(color) == 6)
				{
					DWORD c = strtoul(color, 0, 16);
					fontColorLyrics = RGB(GetBValue(c), GetGValue(c), GetRValue(c));
				}

				fontSizeDefault = fsize;
				fontBoldDefault = !!fbold;
			}

			XmlNode xmlFontNoLyrics = xmlMain.FirstChild("FontNoLyrics");
			if (xmlFontNoLyrics)
			{
				const char* fname = nullptr; int fsize = 0; int fbold = 0; int fitalic = 0; int fclear = 0;

				fname = xmlFontNoLyrics.AttributeRaw("Name");
				xmlFontNoLyrics.Attribute("Size", &fsize);
				xmlFontNoLyrics.Attribute("Bold", &fbold);
				xmlFontNoLyrics.Attribute("Italic", &fitalic);
				xmlFontNoLyrics.Attribute("ClearType", &fclear);

				fontNoLyrics = ExImage::CreateElementFont(fname, fsize, fbold, fitalic, fclear);

				const char* color = xmlFontNoLyrics.AttributeRaw("Color");
				if (color && strlen(color) == 6)
				{
					DWORD c = strtoul(color, 0, 16);
					fontColorNoLyrics = RGB(GetBValue(c), GetGValue(c), GetRValue(c));
				}
			}

			XmlNode xmlBackground = xmlMain.FirstChild("Background");
			if (xmlBackground)
			{
				const char* color = xmlBackground.AttributeRaw("Color");
				if (color && strlen(color) == 6)
				{
					DWORD c = strtoul(color, 0, 16);
					backColor = RGB(GetBValue(c), GetGValue(c), GetRValue(c));
				}
			}

			XmlNode xmlExperimental = xmlMain.FirstChild("Experimental");
			if (xmlExperimental)
			{
				xmlExperimental.Attribute("Transparent", &isBackTransparent);
				xmlExperimental.Attribute("FadeTopBottom", &fadeTopBottom);
			}

			XmlNode xmlMargin = xmlMain.FirstChild("Margin");
			if (xmlMargin)
			{
				xmlMargin.Attribute("Left", &margin.left);
				xmlMargin.Attribute("Right", &margin.right);
				xmlMargin.Attribute("Top", &margin.top);
				xmlMargin.Attribute("Bottom", &margin.bottom);
			}

			XmlNode xmlScroll = xmlMain.FirstChild("Scroll");
			if (xmlScroll)
			{
				std::wstring attr = xmlScroll.Attribute16("File");
				if (!attr.empty())
				{
					skinScroll.reset(new SkinScroll());
					if (isBackTransparent)
						skinScroll->SetBmBack(bmBack);
					if (!skinScroll->CreateScroll(thisWnd, path + attr, zipFile))
						skinScroll.reset();
				}
			}
		}
	}
	else
		return false;

	if (!skinScroll)
		nativeScroll = ::CreateWindowEx(0, WC_SCROLLBAR, L"", WS_CHILD|SBS_VERT, 0, 0, 0, 0, thisWnd, NULL, ::GetModuleHandle(NULL), NULL);

	if (!fontLyrics)
	{
		fontLyrics = ExImage::CreateElementFont(NULL, 0, false, false);
		fontColorLyrics = RGB(100, 100, 100);
	}

	if (!fontNoLyrics)
	{
		fontNoLyrics = ExImage::CreateElementFont(NULL, 0, false, false);
		fontColorNoLyrics = RGB(100, 100, 100);
	}

	heightLine = GetHeightLine();

	return true;
}

void SkinLyrics::OnPaint(HDC dc, PAINTSTRUCT& ps)
{
	CRect rc;
	if (!isBackTransparent)
		rc = ps.rcPaint;
	else
		::GetClientRect(thisWnd, rc);

	// New dcMemory //
	HDC dcMemory = ::CreateCompatibleDC(dc);
	HBITMAP bmMemory = ExImage::Create32BppBitmap(rc.Width(), rc.Height());
	HGDIOBJ oldMemory = ::SelectObject(dcMemory, bmMemory);
	// New dcMemory //

	if (!isBackTransparent)
	{
		::SetBkColor(dcMemory, backColor);
		ExImage::FillDC(dcMemory, 0, 0, rc.Width(), rc.Height());
	}
	else
	{
		CRect rcWnd = rc;
		::MapWindowPoints(thisWnd, thisParentWnd, (LPPOINT)&rcWnd, 2);

		HDC dcBack = ::CreateCompatibleDC(dc);
		HGDIOBJ oldBack = ::SelectObject(dcBack, *bmBack);

		::BitBlt(dcMemory, 0, 0, rcWnd.Width(), rcWnd.Height(), dcBack, rcWnd.left, rcWnd.top, SRCCOPY);

		::SelectObject(dcBack, oldBack);
		::DeleteDC(dcBack);
	}

	::SetBkMode(dcMemory, TRANSPARENT);

	CRect rc2;
	::GetClientRect(thisWnd, rc2);

	if (!lines.empty())
	{
		DrawTextLines(dcMemory, -rc.left, -HScrollGetPos() - rc.top, rc2.right - scrollWidth - rc.left, rc.Height(), rc2.right);

		if (isBackTransparent && fadeTopBottom)
		{
			CreateGradient(dc, rc);
			if (scrollWidth)
			{
				fadeTop.Draw(dcMemory, 0, 0);
				fadeBottom.Draw(dcMemory, 0, rc.bottom - fadeBottom.Height());
			}
		}
	}
	else
	{
		if (!stateEmpty)
		{
			::SetTextColor(dcMemory, fontColorNoLyrics);
			HGDIOBJ oldFont = ::SelectObject(dcMemory, fontNoLyrics);
			CRect rcFont(0, 20, rc2.right - scrollWidth, rc2.bottom);
			rcFont.OffsetRect(rc2.left - rc.left, rc2.top - rc.top);
			if (stateRecv)
				::DrawText(dcMemory, stringReceiving.c_str(), (int)stringReceiving.size(), rcFont, DT_CENTER|DT_SINGLELINE|DT_NOPREFIX|DT_NOCLIP);
			else if (statePlay)
				::DrawText(dcMemory, stringNotFound.c_str(), (int)stringNotFound.size(), rcFont, DT_CENTER|DT_SINGLELINE|DT_NOPREFIX|DT_NOCLIP);
			else
				::DrawText(dcMemory, stringNoLyrics.c_str(), (int)stringNoLyrics.size(), rcFont, DT_CENTER|DT_SINGLELINE|DT_NOPREFIX|DT_NOCLIP);
			::SelectObject(dcMemory, oldFont);
		}
	}

	// Copy dcMemory //
	::BitBlt(dc, rc.left, rc.top, rc.Width(), rc.Height(), dcMemory, 0, 0, SRCCOPY);
	::SelectObject(dcMemory, oldMemory);
	::DeleteObject(bmMemory);
	::DeleteDC(dcMemory);
	// Copy dcMemory //
}

void SkinLyrics::CreateGradient(HDC dc, const CRect& rcClient)
{
	if (fadeTop.IsValid() && fadeWinSizeX == rcClient.right && fadeWinSizeY == rcClient.bottom)
		return;

	fadeWinSizeX = rcClient.right;
	fadeWinSizeY = rcClient.bottom;

	CRect rcWnd = rcClient;
	::MapWindowPoints(thisWnd, thisParentWnd, (LPPOINT)&rcWnd, 2);

	HDC dcScreen = ::CreateCompatibleDC(dc);
	HGDIOBJ oldScreen = ::SelectObject(dcScreen, *bmBack);

	fadeTop.CreateGradientFromDC(true, fadeWinSizeX, fadeTopBottom, dcScreen, rcWnd.left, rcWnd.top);
	fadeBottom.CreateGradientFromDC(false, fadeWinSizeX, fadeTopBottom, dcScreen, rcWnd.left, rcWnd.bottom - fadeTopBottom);

	::SelectObject(dcScreen, oldScreen);
	::DeleteDC(dcScreen);
}

void SkinLyrics::DrawTextLines(HDC dc, int x, int y, int right, int height, int cx)
{
	x += margin.left;
	y += margin.top;
	right -= margin.right;

	cx -= (margin.left + margin.right);
	cx -= scrollWidth;

	::SetTextColor(dc, fontColorLyrics);
	HGDIOBJ oldFont = ::SelectObject(dc, fontLyrics);

	CSize szSpace;
	::GetTextExtentPoint32(dc, L" ", 1, &szSpace);

	for (std::size_t i = 0, size = lines.size(); i < size; ++i)
	{
		if (y > height)
			break;

		if (y > -linesHeight[i])
		{
			CRect rc(x, y, right, y + linesHeight[i]);

			int format = 0;
			if (lyricsAlign == 0)
				format = DT_CENTER|DT_NOPREFIX;
			else if (lyricsAlign == 1)
				format = DT_LEFT|DT_NOPREFIX;
			else
				format = DT_RIGHT|DT_NOPREFIX;

			// DrawText with DT_WORDBREAK is slow so use GetTextExtentPoint32 and calc size word by word

			if (!lines[i].empty())
			{
				CSize szText;

				int width = 0;
				int line = 0;

				for (std::size_t find = 0, start = 0; ; ++find)
				{
					if (lines[i][find] == ' ')
					{
						::GetTextExtentPoint32(dc, lines[i].c_str() + start, find - start, &szText);

						width += szText.cx + szSpace.cx;
						if (width - szSpace.cx > cx)
						{
							width = szText.cx + szSpace.cx;

							::DrawText(dc, lines[i].c_str() + line, start - line - 1, rc, format);
							rc.top += szSpace.cy;
							line = start;
						}

						start = find + 1;
					}
					else if (lines[i][find] == '\t')
					{
						::GetTextExtentPoint32(dc, lines[i].c_str() + start, find - start, &szText);

						width += szText.cx;
						if (width > cx)
						{
							width = szText.cx;

							::DrawText(dc, lines[i].c_str() + line, start - line - 1, rc, format);
							rc.top += szSpace.cy;
							line = start;
						}

						start = find + 1;
					}
					else if (lines[i][find] == '\0')
					{
						::GetTextExtentPoint32(dc, lines[i].c_str() + start, find - start, &szText);

						width += szText.cx;
						if (width > cx)
						{
							::DrawText(dc, lines[i].c_str() + line, start - line - 1, rc, format);
							rc.top += szSpace.cy;
							line = start;
						}
						::DrawText(dc, lines[i].c_str() + line, find - line, rc, format);
						break;
					}
				}
			}

			// Old
			/*if (lyricsAlign == 0)
				::DrawText(dc, lines[i].c_str(), (int)lines[i].size(), rc, DT_CENTER|DT_NOPREFIX|DT_WORDBREAK);
			else if (lyricsAlign == 1)
				::DrawText(dc, lines[i].c_str(), (int)lines[i].size(), rc, DT_LEFT|DT_NOPREFIX|DT_WORDBREAK);
			else
				::DrawText(dc, lines[i].c_str(), (int)lines[i].size(), rc, DT_RIGHT|DT_NOPREFIX|DT_WORDBREAK);*/
		}

		y += linesHeight[i];
	}

	::SelectObject(dc, oldFont);
}

int SkinLyrics::GetHeightLine()
{
	HDC dc = ::GetDC(thisWnd);

	HGDIOBJ oldFont = ::SelectObject(dc, fontLyrics);

	CSize szText;
	::GetTextExtentPoint32(dc, L" ", 1, &szText);

	return szText.cy;

	::SelectObject(dc, oldFont);
	::ReleaseDC(thisWnd, dc);
}

int SkinLyrics::GetHeightLines(int cx, int cy)
{
	int resultHeight = 0;

	if (lines.empty())
		return 0;

	HDC dc = ::GetDC(thisWnd);

	cx -= (margin.left + margin.right);

	cx -= scrollWidth;

	HGDIOBJ oldFont = ::SelectObject(dc, fontLyrics);

	CSize szSpace;
	::GetTextExtentPoint32(dc, L" ", 1, &szSpace);

	for (std::size_t i = 0, size = lines.size(); i < size; ++i)
	{
		// DrawText with DT_CALCRECT is slow so use GetTextExtentPoint32 and calc size word by word

		int height = 0;

		if (lines[i].empty())
		{
			height = szSpace.cy;
		}
		else
		{
			CSize szText;

			int count = 1;
			int width = 0;

			for (std::size_t find = 0, start = 0; ; ++find)
			{
				if (lines[i][find] == ' ')
				{
					::GetTextExtentPoint32(dc, lines[i].c_str() + start, find - start, &szText);

					width += szText.cx + szSpace.cx;
					if (width - szSpace.cx > cx)
					{
						width = szText.cx + szSpace.cx;
						count += 1;
					}

					start = find + 1;
				}
				else if (lines[i][find] == '\t')
				{
					::GetTextExtentPoint32(dc, lines[i].c_str() + start, find - start, &szText);

					width += szText.cx;
					if (width > cx)
					{
						width = szText.cx;
						count += 1;
					}

					start = find + 1;
				}
				else if (lines[i][find] == '\0')
				{
					::GetTextExtentPoint32(dc, lines[i].c_str() + start, find - start, &szText);

					width += szText.cx;
					if (width > cx)
						count += 1;

					break;
				}
			}

			height = count * szSpace.cy;
		}

		linesHeight[i] = height;

		resultHeight += height;

		// Old, slow.
		/*CRect rc(0, 0, cx, cy);

		if (!lines[i].empty())
			::DrawText(dc, lines[i].c_str(), (int)lines[i].size(), rc, DT_CALCRECT|DT_NOPREFIX|DT_WORDBREAK|DT_NOCLIP);
		else
			::DrawText(dc, L" ", 1, rc, DT_CALCRECT|DT_NOPREFIX|DT_SINGLELINE|DT_NOCLIP);

		linesHeight[i] = rc.Height();

		resultHeight += rc.Height();*/
	}

	::SelectObject(dc, oldFont);
	::ReleaseDC(thisWnd, dc);

	resultHeight += margin.top + margin.bottom;

	return resultHeight;
}

void SkinLyrics::ResetScroll(int cx, int cy)
{
	int oldScrollWidth = scrollWidth;

	ResetScrollImpl(cx, cy);

	if (scrollWidth != oldScrollWidth)
		ResetScrollImpl(cx, cy);
}

void SkinLyrics::ResetScrollImpl(int cx, int cy)
{
	heightLines = GetHeightLines(cx, cy);

	int scroll = HScrollGetWidth();

	HScrollResize(cx - scroll, 0, scroll, cy);

	if (heightLines > cy)
	{
		HScrollSetInfo(0, heightLines, heightLine, cy);

		scrollWidth = scroll;

		HScrollShow(true);
	}
	else
	{
		HScrollShow(false);

		scrollWidth = 0;

		HScrollSetInfo(0, heightLines, heightLine, cy);
	}
}

void SkinLyrics::OnSize(UINT nType, int cx, int cy)
{
	if (!::IsWindowVisible(thisWnd))
		return;

	ResetScroll(cx, cy);
}

void SkinLyrics::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CRect rc;
	::GetClientRect(thisWnd, rc);
	CSize sz = rc.Size();

	if (bShow)
	{
		isWindowVisible = true;

		if (szWindowHidden != sz)
			ResetScroll(sz.cx, sz.cy);

		callbackShowWindow();
	}
	else
	{
		isWindowVisible = false;

		szWindowHidden = sz;
	}
}

void SkinLyrics::OnVScroll(UINT nSBCode, UINT nPos, HWND hScrollBar)
{
	int oldPos = HScrollMessageGetPosOld();
	if (HScrollMessage((int)nSBCode))
	{
		StopSmoothScroll();

		if (!isBackTransparent)
			::ScrollWindowEx(thisWnd, 0, oldPos - HScrollGetPos(), NULL, NULL, NULL, NULL, SW_INVALIDATE);
		else
			::InvalidateRect(thisWnd, NULL, FALSE);
		::UpdateWindow(thisWnd);
	}
}

void SkinLyrics::MouseWheel(bool isReverse)
{
	UINT lineScroll = 3; // default value
	SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &lineScroll, 0);

	int scroll = 0;

	if (lineScroll == WHEEL_PAGESCROLL)
		scroll = HScrollGetPage();
	else
		scroll = lineScroll * heightLine;

	if (isReverse) scroll = -scroll;

	if (!isSmoothScrollEnabled)
	{
		int oldPos = HScrollGetPos();
		if (HScrollSetPos(HScrollGetPos() + scroll))
		{
			if (!isBackTransparent)
				::ScrollWindowEx(thisWnd, 0, oldPos - HScrollGetPos(), NULL, NULL, NULL, NULL, SW_INVALIDATE);
			else
				::InvalidateRect(thisWnd, NULL, FALSE);
			::UpdateWindow(thisWnd);
		}
	}
	else
		SmoothScroll(scroll);
}

void SkinLyrics::StopSmoothScroll()
{
	if (isSmoothScrollEnabled)
	{
		eventSmoothScroll->Reset();
		isSmoothScrollRun = false;
		smoothScrollAcc = 0;
	}
}

void SkinLyrics::SmoothScroll(int scroll)
{
	// For smooth scrolling use two important variables smoothScrollNew and smoothScrollAcc.
	// smoothScrollNew is a new scroll position that the scroll should reach.
	// smoothScrollAcc is the scroll acceleration:
	// if = 0 then scroll and timer are stopped, if > 0 then scroll is moving up, if < 0 moving down.

	if (smoothScrollAcc == 0)
		smoothScrollNew = HScrollGetPos() + scroll;
	else
		smoothScrollNew += scroll;

	// Adjust scroll position
	smoothScrollNew = std::max(HScrollGetMin(), std::min(HScrollGetMax() - HScrollGetPage(), smoothScrollNew));

	// Scroll is already in position
	if (smoothScrollNew == HScrollGetPos())
		return;

	// If scroll is not moving then start timer to move it
	if (smoothScrollAcc == 0)
	{
		isSmoothScrollRun = true;
		eventSmoothScroll->Set();
	}
}

void SkinLyrics::OnTimer(UINT_PTR nIDEvent)
{

}

void SkinLyrics::SmoothScrollRun()
{
	int diff = smoothScrollNew - HScrollGetPos();

	if (diff == 0) return;

	// Slow down scrolling when reaching the destination point
	if (diff > 0)
		smoothScrollAcc = diff / 16 + 1;
	else if (diff < 0)
		smoothScrollAcc = diff / 16 - 1;

	// Scrolling
	int oldPos = HScrollGetPos();
	if (HScrollSetPos(HScrollGetPos() + smoothScrollAcc))
	{
		if (!isBackTransparent)
			::ScrollWindowEx(thisWnd, 0, oldPos - HScrollGetPos(), NULL, NULL, NULL, NULL, SW_INVALIDATE);
		else
			::InvalidateRect(thisWnd, NULL, FALSE);
		::UpdateWindow(thisWnd);
	}

	if ((smoothScrollAcc > 0 && HScrollGetPos() >= smoothScrollNew) ||
		(smoothScrollAcc < 0 && HScrollGetPos() <= smoothScrollNew))
	{
		eventSmoothScroll->Reset();
		isSmoothScrollRun = false;
		smoothScrollAcc = 0;
	}
}

void SkinLyrics::OnContextMenu(HWND hWnd, CPoint point)
{
	::SendMessage(thisParentWnd, UWM_LYRICSMENU, point.x, point.y);
}

void SkinLyrics::SetFontSize(int size, bool bold)
{
	int height = fontSizeDefault;

	switch(size)
	{
	case 1: height = 8; break;
	case 2: height = 10; break;
	case 3: height = 12; break;
	case 4: height = 14; break;
	}

	LOGFONT logFont;
	if (::GetObject(fontLyrics, sizeof(LOGFONT), &logFont) != 0)
	{
		::DeleteObject(fontLyrics);

		logFont.lfHeight = -MulDiv(height, 96, 72);
		if (bold)
			logFont.lfWeight = FW_BOLD;
		else
			logFont.lfWeight = FW_NORMAL;

		fontLyrics = ::CreateFontIndirect(&logFont);

		if (!lines.empty())
		{
			heightLine = GetHeightLine();

			CRect rc;
			::GetClientRect(thisWnd, rc);

			ResetScroll(rc.Width(), rc.Height());

			::InvalidateRect(thisWnd, NULL, FALSE);
		}
	}
}

void SkinLyrics::SetAlign(int align)
{
	lyricsAlign = align;

	if (!lines.empty())
		::InvalidateRect(thisWnd, NULL, FALSE);
}

std::wstring SkinLyrics::GetLyrics()
{
	std::wstring lyrics;

	for (std::size_t i = 0, size = lines.size(); i < size; ++i)
	{
		if (i > 0)
		{
			lyrics.push_back('\r');
			lyrics.push_back('\n');
		}
		lyrics += lines[i];
	}

	return lyrics;
}

int SkinLyrics::HScrollGetPos()
{
	if (skinScroll)
		return skinScroll->GetScrollPos();
	else
		return ::GetScrollPos(nativeScroll, SB_CTL);
}

bool SkinLyrics::HScrollSetPos(int pos, bool needRedraw)
{
	if (skinScroll)
		return skinScroll->SetScrollPos(pos, needRedraw);
	else
	{
		int oldPos = ::GetScrollPos(nativeScroll, SB_CTL);
		if (oldPos == ::SetScrollPos(nativeScroll, SB_CTL, pos, needRedraw))
			return false;

		return true;
	}
}

int SkinLyrics::HScrollGetPage()
{
	if (skinScroll)
		return skinScroll->GetScrollPage();
	else
	{
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_PAGE;
		::GetScrollInfo(nativeScroll, SB_CTL, &si);

		return (int)si.nPage;
	}
}

void SkinLyrics::HScrollSetInfo(int min, int max, int line, int page)
{
	if (skinScroll)
	{
		// Get number of lines to scroll from Windows settings
		UINT lineScroll = 3; // default value
		SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &lineScroll, 0);

		skinScroll->SetScrollInfo(min, max, line * lineScroll, page);
	}
	else
	{
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_PAGE|SIF_RANGE;
		si.nMin = min;
		si.nMax = max - 1;
		si.nPage = page;

		::SetScrollInfo(nativeScroll, SB_CTL, &si, TRUE);

		// iMax - 1 and below + 1 because:
		// The SetScrollInfo function performs range checking on the values specified by the nPage and nPos
		// members of the SCROLLINFO structure. The nPage member must specify a value from 0 to nMax - nMin +1.
		// The nPos member must specify a value between nMin and nMax - max( nPage- 1, 0). If either value
		// is beyond its range, the function sets it to a value that is just within the range.
		// SetScrollInfo: http://msdn.microsoft.com/en-us/library/bb787595(VS.85).aspx
		// i.e. we compensate this 1 pixel otherwise for example SmoothScroll never reach the end on 1 pixel.
	}
}

int SkinLyrics::HScrollGetMax()
{
	if (skinScroll)
		return skinScroll->GetScrollMax();
	else
	{
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_RANGE;
		::GetScrollInfo(nativeScroll, SB_CTL, &si);

		return (int)si.nMax + 1;
	}
}

int SkinLyrics::HScrollGetMin()
{
	if (skinScroll)
		return skinScroll->GetScrollMin();
	else
	{
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_RANGE;
		::GetScrollInfo(nativeScroll, SB_CTL, &si);

		return (int)si.nMin;
	}
}

int SkinLyrics::HScrollGetWidth()
{
	if (skinScroll)
		return skinScroll->GetScrollWidth();
	else
		return GetSystemMetrics(SM_CXVSCROLL);
}

void SkinLyrics::HScrollShow(bool show)
{
	if (skinScroll)
	{
		skinScroll->ShowScroll(show);
	}
	else if (nativeScroll)
	{
		::ShowScrollBar(nativeScroll, SB_CTL, show);
	}
}

void SkinLyrics::HScrollResize(int x, int y, int cx, int cy)
{
	if (skinScroll)
	{
		::MoveWindow(skinScroll->Wnd(), x, y, cx, cy, FALSE);
	}
	else if (nativeScroll)
	{
		::MoveWindow(nativeScroll, x, y, cx, cy, FALSE);
	}
}

bool SkinLyrics::HScrollMessage(int message)
{
	if (skinScroll)
	{
		if (message == SB_THUMBPOSITION)
			return true;
	}
	else
	{
		// Get number of lines to scroll from Windows settings
		UINT lineScroll = 3; // default value
		SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &lineScroll, 0);

		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_POS|SIF_TRACKPOS|SIF_RANGE|SIF_PAGE;
		::GetScrollInfo(nativeScroll, SB_CTL, &si);

		int posScroll = (int)si.nPos;

		switch (message)
		{
			case SB_LINEUP:
				posScroll -= heightLine * lineScroll;
			break;

			case SB_LINEDOWN:
				posScroll += heightLine * lineScroll;
			break;

			case SB_PAGEUP:
				posScroll -= (int)si.nPage;
			break;

			case SB_PAGEDOWN:
				posScroll += (int)si.nPage;
			break;

			case SB_TOP:
				posScroll = (int)si.nMin;
			break;

			case SB_BOTTOM:
				posScroll = (int)si.nMax;
			break;

			case SB_THUMBTRACK:
			case SB_THUMBPOSITION:
				posScroll = (int)si.nTrackPos;
		}

		if (HScrollSetPos(posScroll))
			return true;
	}

	return false;
}

int SkinLyrics::HScrollMessageGetPosOld()
{
	if (skinScroll)
		return skinScroll->GetScrollPosOld();
	else
		return ::GetScrollPos(nativeScroll, SB_CTL);
}

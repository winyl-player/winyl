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
#include "SkinEdit.h"

SkinEdit::SkinEdit()
{

}

SkinEdit::~SkinEdit()
{
	if (font) ::DeleteObject(font);
	if (brushBack) ::DeleteObject(brushBack);
}

LRESULT SkinEdit::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
	case WM_CTLCOLOREDIT:
		return (LRESULT)OnCtlColor((HDC)wParam, (HWND)lParam);
	case WM_SIZE:
		OnSize((UINT)wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONDOWN:
		OnLButtonDown((UINT)wParam, CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		return 0;
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		return 0;
	}

	return ::DefWindowProc(hWnd, message, wParam, lParam);
}

SkinEdit* SkinEdit::skinEdit = nullptr;
WNDPROC SkinEdit::mainDlgProc = nullptr;

LRESULT SkinEdit::EditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_CHAR)
	{
		if (wParam == VK_ESCAPE)
		{
			assert(skinEdit->wndFocus);
			::SetFocus(skinEdit->wndFocus);
			return 0;
		}
		else if (wParam == VK_RETURN)
		{
			assert(skinEdit->wndFocus);
			::SetFocus(skinEdit->wndFocus);
			::SendMessage(skinEdit->ParentWnd(), UWM_PLAYFILE, 0, 0);
			return 0;
		}
	}

	return mainDlgProc(hWnd, message, wParam, lParam);
}

bool SkinEdit::NewWindow(HWND parent)
{
	CreateClassWindow(parent, L"SkinEdit", WS_CHILDWINDOW|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, 0);

	edit = ::CreateWindowEx(0, WC_EDIT, L"", WS_CHILD|WS_VISIBLE|ES_LEFT|ES_AUTOHSCROLL, 0, 0, 0, 0, thisWnd, NULL, ::GetModuleHandle(NULL), NULL);

	skinEdit = this;
	mainDlgProc = (WNDPROC)(LONG_PTR)::GetWindowLongPtr(edit, GWLP_WNDPROC);
	::SetWindowLongPtr(edit, GWLP_WNDPROC, (LONG_PTR)&EditProc);

	return true;
}

bool SkinEdit::LoadSkin(const std::wstring& file, ZipFile* zipFile)
{
//	std::wstring path;
//	CFileParser::PathFromFile(csFile, csPath);

	XmlFile xmlFile;

	if (xmlFile.LoadEx(file, zipFile))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Search");

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
	
				font = ExImage::CreateElementFont(fname, fsize, fbold, fitalic, fclear);
				::SendMessage(edit, WM_SETFONT, (WPARAM)font, FALSE);

				if (fsize == 0) fsize = 8;

				editHeight = MulDiv(fsize, 96, 72);
				editHeight += editHeight / 3; // 4

				colorText = RGB(0, 0, 0); // Default

				const char* strColor = xmlFont.AttributeRaw("Color");
				if (strColor)
				{
					colorText = strtoul(strColor, 0, 16);
					colorText = RGB(GetBValue(colorText), GetGValue(colorText), GetRValue(colorText));
				}

				colorText2 = RGB(0, 0, 0); // Default
				
				const char* strColor2 = xmlFont.AttributeRaw("Color2");
				if (strColor2)
				{
					colorText2 = strtoul(strColor2, 0, 16);
					colorText2 = RGB(GetBValue(colorText2), GetGValue(colorText2), GetRValue(colorText2));
				}

				bool upperCase = false;
				xmlFont.Attribute("UpperCase", &upperCase);
				if (upperCase)
				{
					LONG_PTR style = ::GetWindowLongPtr(edit, GWL_STYLE);
					::SetWindowLongPtr(edit, GWL_STYLE, style | ES_UPPERCASE);
				}
			}

			XmlNode xmlBackground = xmlMain.FirstChild("Background");
			if (xmlBackground)
			{
				colorBack = RGB(255, 255, 255); // Default

				const char* strColor = xmlBackground.AttributeRaw("Color");
				if (strColor)
				{
					colorBack = strtoul(strColor, 0, 16);
					colorBack = RGB(GetBValue(colorBack), GetGValue(colorBack), GetRValue(colorBack));
				}
			}
		}
	}
	else
		return false;

	brushBack = ::CreateSolidBrush(colorBack);

	return true;
}

void SkinEdit::OnSize(UINT nType, int cx, int cy)
{
	::MoveWindow(edit, 0, (cy - editHeight) / 2, cx, editHeight, FALSE);
}

HBRUSH SkinEdit::OnCtlColor(HDC hDC, HWND hWnd)
{
	if (hWnd == edit)
	{
		::SetTextColor(hDC, colorCurrent);

		//::SetBkMode(hDC, TRANSPARENT);
		::SetBkColor(hDC, colorBack);

		return brushBack;
	}

	return NULL;
}

void SkinEdit::OnPaint(HDC dc, PAINTSTRUCT& ps)
{
	CRect rc;
	::GetClientRect(thisWnd, rc);
	
	::FillRect(dc, rc, brushBack);

	// http://weblogs.asp.net/kennykerr/archive/2007/01/23/controls-and-the-desktop-window-manager.aspx

	/*
	CDCHandle bufferedDC;
	futureWin->BufferedPaintInit();
	HPAINTBUFFER hPaint = futureWin->BeginBufferedPaint(dc, dc.m_ps.rcPaint, BPBF_TOPDOWNDIB, NULL, bufferedDC);
	if (hPaint)
	{
		SendMessage(WM_PRINTCLIENT, (WPARAM)bufferedDC.m_hDC, PRF_CLIENT);
		futureWin->BufferedPaintSetAlpha(hPaint, NULL, 255);
		futureWin->EndBufferedPaint(hPaint, TRUE);
	}
	futureWin->BufferedPaintUnInit();
	return 0;
	*/

	// Do not call CWnd::OnPaint() for painting messages
}

void SkinEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
	::SetFocus(edit);
}

void SkinEdit::OnCommand(WPARAM wParam, LPARAM lParam)
{
	 if (edit != (HWND)lParam)
		 return;

	if (HIWORD(wParam) == EN_CHANGE)
		OnEnChangeEdit();
	else if (HIWORD(wParam) == EN_SETFOCUS)
		OnEnSetFocusEdit();
	else if (HIWORD(wParam) == EN_KILLFOCUS)
		OnEnKillFocusEdit();
}

void SkinEdit::OnEnChangeEdit()
{
	if (!isEditChange)
		return;

	bool needRedraw = false;

	if ((searchText.empty() && ::GetWindowTextLength(edit) > 0) ||
		(!searchText.empty() && ::GetWindowTextLength(edit) == 0))
		needRedraw = true;

	searchText = HelperGetWindowText(edit);

	::SendMessage(thisParentWnd, UWM_SEARCHCHANGE, needRedraw, 0);
}

void SkinEdit::OnEnSetFocusEdit()
{
	if (searchText.empty())
	{
		isEditChange = false;
		colorCurrent = colorText;
		::SetWindowText(edit, L"");
		isEditChange = true;
	}
}

void SkinEdit::OnEnKillFocusEdit()
{
	if (searchText.empty())
	{
		isEditChange = false;
		colorCurrent = colorText2;
		::SetWindowText(edit, standartText.c_str());
		isEditChange = true;
	}
}

bool SkinEdit::IsSearchEmpty()
{
	return searchText.empty();
}

void SkinEdit::SearchClear()
{
	if (!searchText.empty())
	{
		searchText.clear();

		isEditChange = false;
		colorCurrent = colorText2;
		::SetWindowText(edit, standartText.c_str());
		isEditChange = true;
	}
}

void SkinEdit::SetStandartText(const std::wstring& text)
{
	standartText = text;

	if (searchText.empty())
	{
		isEditChange = false;
		colorCurrent = colorText2;
		::SetWindowText(edit, standartText.c_str());
		isEditChange = true;
	}
}

bool SkinEdit::IsFocus()
{
	return (::GetFocus() == edit);
}

void SkinEdit::SetFocus()
{
	if (::IsWindowVisible(thisWnd))
		::SetFocus(edit);
}

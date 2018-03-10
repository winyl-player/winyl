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
#include "DlgEqualizer.h"

DlgEqualizer::DlgEqualizer()
{

}

DlgEqualizer::~DlgEqualizer()
{
	if (presetsMenu) ::DestroyMenu(presetsMenu);
	if (fontBar) ::DeleteObject(fontBar);
	if (fontSmall) ::DeleteObject(fontSmall);
}

INT_PTR DlgEqualizer::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		OnInitDialog();
		return TRUE;
	case WM_TIMER:
		OnTimer((UINT_PTR)wParam);
		return TRUE;
	case WM_VSCROLL:
		OnVScroll(LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			OnBnClickedOK();
			EndDialog(LOWORD(wParam));
			return TRUE;
		case IDCANCEL:
			OnBnClickedCancel();
			EndDialog(LOWORD(wParam));
			return TRUE;
		case IDC_CHECK_EQ:
			OnBnClickedCheckEQ();
			return TRUE;
		}
		OnCommand(wParam, lParam);
		return TRUE;
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
		return TRUE;
	}

	return FALSE;
}

void DlgEqualizer::OnInitDialog()
{
	::SetWindowText(thisWnd, lang->GetLine(Lang::EqualizerDialog, 0));
	::SetDlgItemText(thisWnd, IDCANCEL, lang->GetLine(Lang::EqualizerDialog, 1));
	::SetDlgItemText(thisWnd, IDC_CHECK_EQ, lang->GetLine(Lang::EqualizerDialog, 2));
	::SetDlgItemText(thisWnd, IDC_STATIC_PRESET, lang->GetLine(Lang::EqualizerDialog, 3));

	eqSteps = 10;
	
	// Get equalizer settings
	eqPreamp = FromFloat(libAudio->GetPreamp());
	for (int i = 0; i < 10; ++i)
		eqValues[i] = FromFloat(libAudio->GetEq(i));

	std::wstring preset = libAudio->GetEqPreset();
	StringEx::ReplaceString(preset, L"&", L"&&");

	isUserDefined = false;
	if (preset.empty())
	{
		for (int i = 0; i < 10; ++i)
		{
			if (eqPreamp != 24 || eqValues[i] != 24)
			{
				isUserDefined = true;
				break;
			}
		}
	}

	sliderPreamp = ::GetDlgItem(thisWnd, IDC_SLIDER_GAIN);
	sliderEQ0 = ::GetDlgItem(thisWnd, IDC_SLIDER_EQ0);
	sliderEQ1 = ::GetDlgItem(thisWnd, IDC_SLIDER_EQ1);
	sliderEQ2 = ::GetDlgItem(thisWnd, IDC_SLIDER_EQ2);
	sliderEQ3 = ::GetDlgItem(thisWnd, IDC_SLIDER_EQ3);
	sliderEQ4 = ::GetDlgItem(thisWnd, IDC_SLIDER_EQ4);
	sliderEQ5 = ::GetDlgItem(thisWnd, IDC_SLIDER_EQ5);
	sliderEQ6 = ::GetDlgItem(thisWnd, IDC_SLIDER_EQ6);
	sliderEQ7 = ::GetDlgItem(thisWnd, IDC_SLIDER_EQ7);
	sliderEQ8 = ::GetDlgItem(thisWnd, IDC_SLIDER_EQ8);
	sliderEQ9 = ::GetDlgItem(thisWnd, IDC_SLIDER_EQ9);

	// Set all sliders
	::SendMessage(sliderPreamp, TBM_SETRANGE, TRUE, MAKELPARAM(0, 48));
	::SendMessage(sliderEQ0, TBM_SETRANGE, TRUE, MAKELPARAM(0, 48));
	::SendMessage(sliderEQ1, TBM_SETRANGE, TRUE, MAKELPARAM(0, 48));
	::SendMessage(sliderEQ2, TBM_SETRANGE, TRUE, MAKELPARAM(0, 48));
	::SendMessage(sliderEQ3, TBM_SETRANGE, TRUE, MAKELPARAM(0, 48));
	::SendMessage(sliderEQ4, TBM_SETRANGE, TRUE, MAKELPARAM(0, 48));
	::SendMessage(sliderEQ5, TBM_SETRANGE, TRUE, MAKELPARAM(0, 48));
	::SendMessage(sliderEQ6, TBM_SETRANGE, TRUE, MAKELPARAM(0, 48));
	::SendMessage(sliderEQ7, TBM_SETRANGE, TRUE, MAKELPARAM(0, 48));
	::SendMessage(sliderEQ8, TBM_SETRANGE, TRUE, MAKELPARAM(0, 48));
	::SendMessage(sliderEQ9, TBM_SETRANGE, TRUE, MAKELPARAM(0, 48));

	::SendMessage(sliderPreamp, TBM_SETPOS, TRUE, eqPreamp);
	::SendMessage(sliderEQ0, TBM_SETPOS, TRUE, eqValues[0]);
	::SendMessage(sliderEQ1, TBM_SETPOS, TRUE, eqValues[1]);
	::SendMessage(sliderEQ2, TBM_SETPOS, TRUE, eqValues[2]);
	::SendMessage(sliderEQ3, TBM_SETPOS, TRUE, eqValues[3]);
	::SendMessage(sliderEQ4, TBM_SETPOS, TRUE, eqValues[4]);
	::SendMessage(sliderEQ5, TBM_SETPOS, TRUE, eqValues[5]);
	::SendMessage(sliderEQ6, TBM_SETPOS, TRUE, eqValues[6]);
	::SendMessage(sliderEQ7, TBM_SETPOS, TRUE, eqValues[7]);
	::SendMessage(sliderEQ8, TBM_SETPOS, TRUE, eqValues[8]);
	::SendMessage(sliderEQ9, TBM_SETPOS, TRUE, eqValues[9]);


	HFONT defFont = (HFONT)::SendMessage(::GetDlgItem(thisWnd, IDC_STATIC_ZERO), WM_GETFONT, 0, 0);
	
	LOGFONT logFont;
	::GetObject(defFont, sizeof(LOGFONT), &logFont);

	fontBar = ::CreateFontIndirect(&logFont);

	HDC dcTemp = ::GetDC(thisWnd);
	logFont.lfHeight = logFont.lfHeight * 85 / 100; // -::MulDiv(7, ::GetDeviceCaps(dcTemp, LOGPIXELSY), 72);
	::ReleaseDC(thisWnd, dcTemp);

	fontSmall = ::CreateFontIndirect(&logFont);

	::SendMessage(::GetDlgItem(thisWnd, IDC_STATIC_P12   ), WM_SETFONT, (WPARAM)fontSmall, FALSE);
	::SendMessage(::GetDlgItem(thisWnd, IDC_STATIC_ZERO  ), WM_SETFONT, (WPARAM)fontSmall, FALSE);
	::SendMessage(::GetDlgItem(thisWnd, IDC_STATIC_M12   ), WM_SETFONT, (WPARAM)fontSmall, FALSE);
	::SendMessage(::GetDlgItem(thisWnd, IDC_STATIC_PREAMP), WM_SETFONT, (WPARAM)fontSmall, FALSE);
	::SendMessage(::GetDlgItem(thisWnd, IDC_STATIC_EQ0   ), WM_SETFONT, (WPARAM)fontSmall, FALSE);
	::SendMessage(::GetDlgItem(thisWnd, IDC_STATIC_EQ1   ), WM_SETFONT, (WPARAM)fontSmall, FALSE);
	::SendMessage(::GetDlgItem(thisWnd, IDC_STATIC_EQ2   ), WM_SETFONT, (WPARAM)fontSmall, FALSE);
	::SendMessage(::GetDlgItem(thisWnd, IDC_STATIC_EQ3   ), WM_SETFONT, (WPARAM)fontSmall, FALSE);
	::SendMessage(::GetDlgItem(thisWnd, IDC_STATIC_EQ4   ), WM_SETFONT, (WPARAM)fontSmall, FALSE);
	::SendMessage(::GetDlgItem(thisWnd, IDC_STATIC_EQ5   ), WM_SETFONT, (WPARAM)fontSmall, FALSE);
	::SendMessage(::GetDlgItem(thisWnd, IDC_STATIC_EQ6   ), WM_SETFONT, (WPARAM)fontSmall, FALSE);
	::SendMessage(::GetDlgItem(thisWnd, IDC_STATIC_EQ7   ), WM_SETFONT, (WPARAM)fontSmall, FALSE);
	::SendMessage(::GetDlgItem(thisWnd, IDC_STATIC_EQ8   ), WM_SETFONT, (WPARAM)fontSmall, FALSE);
	::SendMessage(::GetDlgItem(thisWnd, IDC_STATIC_EQ9   ), WM_SETFONT, (WPARAM)fontSmall, FALSE);


	::CheckDlgButton(thisWnd, IDC_CHECK_EQ, libAudio->IsEqEnable());

	LoadPresets();

	// Menu init

	presetsMenu = ::CreatePopupMenu();
	::AppendMenu(presetsMenu, MF_STRING, 10000, lang->GetLine(Lang::EqualizerDialog, 4));
	::AppendMenu(presetsMenu, MF_SEPARATOR, NULL, NULL);

	for (std::size_t i = 0, size = presets.size(); i < size; ++i)
		::AppendMenu(presetsMenu, MF_STRING, 10000 + i + 1, presets[i].name.c_str());

	// Toolbar init

	CRect rcTL;
	::GetWindowRect(::GetDlgItem(thisWnd, IDC_STATIC_RECT), rcTL);
	::MapWindowPoints(NULL, thisWnd, (LPPOINT)&rcTL, 2);
	rcTL.top += (rcTL.Height() - 20) / 4; // For High DPI
	rcTL.bottom += (rcTL.Height() - 20) / 4; // For High DPI

	presetsBar = ::CreateWindowEx(0, TOOLBARCLASSNAME, L"", CCS_NODIVIDER|CCS_NORESIZE|TBSTYLE_LIST|TBSTYLE_FLAT|WS_CHILD|WS_VISIBLE,
		rcTL.left, rcTL.top, rcTL.Width(), rcTL.Height(), thisWnd, NULL, ::GetModuleHandle(NULL), NULL);
	
	::SendMessage(presetsBar, WM_SETFONT, (WPARAM)fontBar, FALSE);

	::SendMessage(presetsBar, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS);

	::SendMessage(presetsBar, TB_SETIMAGELIST, 0, NULL);

	int width = rcTL.Width() - 10 - (rcTL.Width() - 84) / 4; // For High DPI
	::SendMessage(presetsBar, TB_SETBUTTONWIDTH, 0, MAKELPARAM(width, width)); // MAKELPARAM(74, 74)

	TBBUTTON button = {};
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = BTNS_BUTTON|BTNS_WHOLEDROPDOWN;//|TBSTYLE_DROPDOWN;
	
	button.idCommand = 0;
	button.iBitmap = 0; // I_IMAGENONE;
	button.iString = reinterpret_cast<INT_PTR>(L""); // Need!

	if (::SendMessage(presetsBar, TB_INSERTBUTTON, 0, (LPARAM)&button))
	{
		TBBUTTONINFO buttonInfo = {};
		buttonInfo.cbSize = sizeof(TBBUTTONINFO);
		buttonInfo.dwMask = TBIF_TEXT;
		
		if (isUserDefined)
			buttonInfo.pszText = const_cast<wchar_t*>(lang->GetLine(Lang::EqualizerDialog, 5));
		else if (preset.empty())
			buttonInfo.pszText = const_cast<wchar_t*>(lang->GetLine(Lang::EqualizerDialog, 4));
		else
			buttonInfo.pszText = const_cast<wchar_t*>(preset.c_str());

		::SendMessage(presetsBar, TB_SETBUTTONINFO, 0, (LPARAM)&buttonInfo);
	}
}

void DlgEqualizer::OnBnClickedOK()
{

}

void DlgEqualizer::OnBnClickedCancel()
{

}

void DlgEqualizer::OnNotify(WPARAM wParam, LPARAM lParam)
{
	NMHDR* nmhdr = reinterpret_cast<NMHDR*>(lParam);

	static bool isReentrant = false;
	if (nmhdr->code == TTN_SHOW && !isReentrant)
	{
		isReentrant = true;
		wchar_t text[64] = L"";
		TOOLINFO ti = {};
		ti.cbSize = sizeof(ti);

		::SendMessage(nmhdr->hwndFrom, TTM_ENUMTOOLS, 0, (LPARAM)&ti);
		//::SendMessage(nmhdr->hwndFrom, TTM_GETTOOLINFO, 0, (LPARAM)&ti);

		ti.hinst = ::GetModuleHandle(NULL);
		ti.lpszText = text;

		int pos = (int)::SendMessage(::GetFocus(), TBM_GETPOS, 0, 0);
		if (pos <= 24)
			swprintf_s(text, L"+%d.%d", abs(24 - pos)/2, abs(24 - pos)%2*5);
		else
			swprintf_s(text, L"-%d.%d", abs(24 - pos)/2, abs(24 - pos)%2*5);

		::SendMessage(nmhdr->hwndFrom, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);
		isReentrant = false;
	}
	else if (nmhdr->code == TBN_DROPDOWN)
	{
		CRect rc;
		::SendMessage(presetsBar, TB_GETITEMRECT, 0, (LPARAM)&rc);
		::MapWindowPoints(presetsBar, NULL, (LPPOINT)&rc, 2);
		::TrackPopupMenu(presetsMenu, TPM_LEFTALIGN, rc.left, rc.bottom, 0, thisWnd, NULL);
	}
}

void DlgEqualizer::OnVScroll(UINT nSBCode, UINT nPos, HWND hScrollBar)
{
	switch (nSBCode)
	{
		case TB_THUMBTRACK:
		case TB_THUMBPOSITION:
		case TB_TOP:
		case TB_BOTTOM:
		case TB_PAGEUP:
		case TB_PAGEDOWN:
		case TB_LINEUP:
		case TB_LINEDOWN:
//		case TB_ENDTRACK:
		{
			if (!isUserDefined)
			{
				isUserDefined = true;

				TBBUTTONINFO button;
				button.cbSize = sizeof(TBBUTTONINFO);
				button.dwMask = TBIF_TEXT;
				button.pszText = const_cast<wchar_t*>(lang->GetLine(Lang::EqualizerDialog, 5));

				::SendMessage(presetsBar, TB_SETBUTTONINFO, 0, (LPARAM)&button);

				libAudio->SetEqPreset(L"");
			}

			if (hScrollBar == sliderPreamp)
			{
				eqPreamp = (int)::SendMessage(sliderPreamp, TBM_GETPOS, 0, 0);
				libAudio->SetPreamp(ToFloat(eqPreamp));
			}
			else if (hScrollBar == sliderEQ0)
			{
				eqValues[0] = (int)::SendMessage(sliderEQ0, TBM_GETPOS, 0, 0);
				libAudio->SetEq(0, ToFloat(eqValues[0]));
			}
			else if (hScrollBar == sliderEQ1)
			{
				eqValues[1] = (int)::SendMessage(sliderEQ1, TBM_GETPOS, 0, 0);
				libAudio->SetEq(1, ToFloat(eqValues[1]));
			}
			else if (hScrollBar == sliderEQ2)
			{
				eqValues[2] = (int)::SendMessage(sliderEQ2, TBM_GETPOS, 0, 0);
				libAudio->SetEq(2, ToFloat(eqValues[2]));
			}
			else if (hScrollBar == sliderEQ3)
			{
				eqValues[3] = (int)::SendMessage(sliderEQ3, TBM_GETPOS, 0, 0);
				libAudio->SetEq(3, ToFloat(eqValues[3]));
			}
			else if (hScrollBar == sliderEQ4)
			{
				eqValues[4] = (int)::SendMessage(sliderEQ4, TBM_GETPOS, 0, 0);
				libAudio->SetEq(4, ToFloat(eqValues[4]));
			}
			else if (hScrollBar == sliderEQ5)
			{
				eqValues[5] = (int)::SendMessage(sliderEQ5, TBM_GETPOS, 0, 0);
				libAudio->SetEq(5, ToFloat(eqValues[5]));
			}
			else if (hScrollBar == sliderEQ6)
			{
				eqValues[6] = (int)::SendMessage(sliderEQ6, TBM_GETPOS, 0, 0);
				libAudio->SetEq(6, ToFloat(eqValues[6]));
			}
			else if (hScrollBar == sliderEQ7)
			{
				eqValues[7] = (int)::SendMessage(sliderEQ7, TBM_GETPOS, 0, 0);
				libAudio->SetEq(7, ToFloat(eqValues[7]));
			}
			else if (hScrollBar == sliderEQ8)
			{
				eqValues[8] = (int)::SendMessage(sliderEQ8, TBM_GETPOS, 0, 0);
				libAudio->SetEq(8, ToFloat(eqValues[8]));
			}
			else if (hScrollBar == sliderEQ9)
			{
				eqValues[9] = (int)::SendMessage(sliderEQ9, TBM_GETPOS, 0, 0);
				libAudio->SetEq(9, ToFloat(eqValues[9]));
			}
		}
	}
}

void DlgEqualizer::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (LOWORD(wParam) >= 10000 && LOWORD(wParam) <= 12000)
	{
		isUserDefined = false;

		wchar_t text[256] = L"";
		::GetMenuString(presetsMenu, LOWORD(wParam), text, sizeof(text) / sizeof(wchar_t), MF_BYCOMMAND);

		TBBUTTONINFO button = {};
		button.cbSize = sizeof(TBBUTTONINFO);
		button.dwMask = TBIF_TEXT;
		button.pszText = text;

		::SendMessage(presetsBar, TB_SETBUTTONINFO, 0, (LPARAM)&button);

		if (LOWORD(wParam) == 10000)
			libAudio->SetEqPreset(L"");
		else
		{
			std::wstring preset = text;
			StringEx::ReplaceString(preset, L"&&", L"&");
			libAudio->SetEqPreset(preset);
		}

		CalculateOld();
		LoadPreset(LOWORD(wParam) - 10000);
		CalculateStep();

		ApplyEqualizer();

		eqElapse = eqSteps;
		::SetTimer(thisWnd, 0, 15, NULL);
	}
}

void DlgEqualizer::OnTimer(UINT_PTR nIDEvent)
{
	eqElapse--;

	eqPreampOld += eqPreampStep;

	for (int i = 0; i < 10; ++i)
		eqValuesOld[i] += eqValuesStep[i];

	if (eqElapse > 0)
	{
		::SendMessage(sliderPreamp, TBM_SETPOS, TRUE, (int)eqPreampOld);
		::SendMessage(sliderEQ0, TBM_SETPOS, TRUE, (int)eqValuesOld[0]);
		::SendMessage(sliderEQ1, TBM_SETPOS, TRUE, (int)eqValuesOld[1]);
		::SendMessage(sliderEQ2, TBM_SETPOS, TRUE, (int)eqValuesOld[2]);
		::SendMessage(sliderEQ3, TBM_SETPOS, TRUE, (int)eqValuesOld[3]);
		::SendMessage(sliderEQ4, TBM_SETPOS, TRUE, (int)eqValuesOld[4]);
		::SendMessage(sliderEQ5, TBM_SETPOS, TRUE, (int)eqValuesOld[5]);
		::SendMessage(sliderEQ6, TBM_SETPOS, TRUE, (int)eqValuesOld[6]);
		::SendMessage(sliderEQ7, TBM_SETPOS, TRUE, (int)eqValuesOld[7]);
		::SendMessage(sliderEQ8, TBM_SETPOS, TRUE, (int)eqValuesOld[8]);
		::SendMessage(sliderEQ9, TBM_SETPOS, TRUE, (int)eqValuesOld[9]);
	}
	else
	{
		::SendMessage(sliderPreamp, TBM_SETPOS, TRUE, eqPreamp);
		::SendMessage(sliderEQ0, TBM_SETPOS, TRUE, eqValues[0]);
		::SendMessage(sliderEQ1, TBM_SETPOS, TRUE, eqValues[1]);
		::SendMessage(sliderEQ2, TBM_SETPOS, TRUE, eqValues[2]);
		::SendMessage(sliderEQ3, TBM_SETPOS, TRUE, eqValues[3]);
		::SendMessage(sliderEQ4, TBM_SETPOS, TRUE, eqValues[4]);
		::SendMessage(sliderEQ5, TBM_SETPOS, TRUE, eqValues[5]);
		::SendMessage(sliderEQ6, TBM_SETPOS, TRUE, eqValues[6]);
		::SendMessage(sliderEQ7, TBM_SETPOS, TRUE, eqValues[7]);
		::SendMessage(sliderEQ8, TBM_SETPOS, TRUE, eqValues[8]);
		::SendMessage(sliderEQ9, TBM_SETPOS, TRUE, eqValues[9]);
	}

	if (eqElapse <= 0)
		::KillTimer(thisWnd, 0);
}

void DlgEqualizer::CalculateOld()
{
	eqPreampOld = (float)::SendMessage(sliderPreamp, TBM_GETPOS, 0, 0);
	eqValuesOld[0] = (float)::SendMessage(sliderEQ0, TBM_GETPOS, 0, 0);
	eqValuesOld[1] = (float)::SendMessage(sliderEQ1, TBM_GETPOS, 0, 0);
	eqValuesOld[2] = (float)::SendMessage(sliderEQ2, TBM_GETPOS, 0, 0);
	eqValuesOld[3] = (float)::SendMessage(sliderEQ3, TBM_GETPOS, 0, 0);
	eqValuesOld[4] = (float)::SendMessage(sliderEQ4, TBM_GETPOS, 0, 0);
	eqValuesOld[5] = (float)::SendMessage(sliderEQ5, TBM_GETPOS, 0, 0);
	eqValuesOld[6] = (float)::SendMessage(sliderEQ6, TBM_GETPOS, 0, 0);
	eqValuesOld[7] = (float)::SendMessage(sliderEQ7, TBM_GETPOS, 0, 0);
	eqValuesOld[8] = (float)::SendMessage(sliderEQ8, TBM_GETPOS, 0, 0);
	eqValuesOld[9] = (float)::SendMessage(sliderEQ9, TBM_GETPOS, 0, 0);
}

void DlgEqualizer::CalculateStep()
{
	eqPreampStep = (float)(eqPreamp - (int)::SendMessage(sliderPreamp, TBM_GETPOS, 0, 0)) / eqSteps;

	eqValuesStep[0] = (float)(eqValues[0] - (int)::SendMessage(sliderEQ0, TBM_GETPOS, 0, 0)) / eqSteps;
	eqValuesStep[1] = (float)(eqValues[1] - (int)::SendMessage(sliderEQ1, TBM_GETPOS, 0, 0)) / eqSteps;
	eqValuesStep[2] = (float)(eqValues[2] - (int)::SendMessage(sliderEQ2, TBM_GETPOS, 0, 0)) / eqSteps;
	eqValuesStep[3] = (float)(eqValues[3] - (int)::SendMessage(sliderEQ3, TBM_GETPOS, 0, 0)) / eqSteps;
	eqValuesStep[4] = (float)(eqValues[4] - (int)::SendMessage(sliderEQ4, TBM_GETPOS, 0, 0)) / eqSteps;
	eqValuesStep[5] = (float)(eqValues[5] - (int)::SendMessage(sliderEQ5, TBM_GETPOS, 0, 0)) / eqSteps;
	eqValuesStep[6] = (float)(eqValues[6] - (int)::SendMessage(sliderEQ6, TBM_GETPOS, 0, 0)) / eqSteps;
	eqValuesStep[7] = (float)(eqValues[7] - (int)::SendMessage(sliderEQ7, TBM_GETPOS, 0, 0)) / eqSteps;
	eqValuesStep[8] = (float)(eqValues[8] - (int)::SendMessage(sliderEQ8, TBM_GETPOS, 0, 0)) / eqSteps;
	eqValuesStep[9] = (float)(eqValues[9] - (int)::SendMessage(sliderEQ9, TBM_GETPOS, 0, 0)) / eqSteps;
}

void DlgEqualizer::LoadPreset(int preset)
{
	if (preset == 0)
	{
		eqPreamp = 24;

		for (int i = 0; i < 10; ++i)
			eqValues[i] = 24;
	}
	else
	{
		eqPreamp = FromFloat(presets[preset - 1].eqPreamp);

		for (int i = 0; i < 10; ++i)
			eqValues[i] = FromFloat(presets[preset - 1].eqValues[i]);
	}
}

int DlgEqualizer::FromFloat(float f)
{
	return 24 - ((int)(f * 10) / 5);
}

float DlgEqualizer::ToFloat(int i)
{
	return ((float)(int)(24 - i) / 2);
}

void DlgEqualizer::OnBnClickedCheckEQ()
{
	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_EQ))
	{
		libAudio->EnableEq(true);
		ApplyEqualizer();
		
		//if (!libAudio->IsFloatSupport())
		//	MessageBox(lang->GetLine(Lang::MESSAGE, 9) + CString('\n') + lang->GetLine(Lang::MESSAGE, 10), lang->GetLine(Lang::MESSAGE, 1), MB_OK|MB_ICONWARNING);
	}
	else
		libAudio->EnableEq(false);
}

void DlgEqualizer::ApplyEqualizer()
{
	libAudio->SetPreamp(ToFloat(eqPreamp));

	for (int i = 0; i < 10; ++i)
		libAudio->SetEq(i, ToFloat(eqValues[i]));
}

bool DlgEqualizer::LoadPresets()
{
	std::wstring file = programPath;
	file += L"Equalizer";
	file.push_back('\\');
	file += L"Presets.xml";

	XmlFile xmlFile;

	if (xmlFile.LoadFile(file))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Presets");
		
		if (xmlMain)
		{
			for (XmlNode xmlNode = xmlMain.FirstChild(); xmlNode; xmlNode = xmlNode.NextChild())
			{
				structPreset preset;

				std::wstring name = xmlNode.Attribute16("Name");
				if (!name.empty())
				{
					preset.name = name;
					StringEx::ReplaceString(preset.name, L"&", L"&&");
				}

				LoadGain(xmlNode, "Preamp", &preset.eqPreamp);

				LoadGain(xmlNode, "Band01", &preset.eqValues[0]);
				LoadGain(xmlNode, "Band02", &preset.eqValues[1]);
				LoadGain(xmlNode, "Band03", &preset.eqValues[2]);
				LoadGain(xmlNode, "Band04", &preset.eqValues[3]);
				LoadGain(xmlNode, "Band05", &preset.eqValues[4]);
				LoadGain(xmlNode, "Band06", &preset.eqValues[5]);
				LoadGain(xmlNode, "Band07", &preset.eqValues[6]);
				LoadGain(xmlNode, "Band08", &preset.eqValues[7]);
				LoadGain(xmlNode, "Band09", &preset.eqValues[8]);
				LoadGain(xmlNode, "Band10", &preset.eqValues[9]);

				presets.push_back(preset);
			}
		}
	}
	else
		return false;

	return true;
}

void DlgEqualizer::LoadGain(XmlNode& xmlNode, char* name, float* f)
{
	XmlNode xmlGain = xmlNode.FirstChild(name);
	if (xmlGain)
	{
		const char* gain = xmlGain.AttributeRaw("Gain");
		if (gain)
		{
			(*f) = (float)atof(gain);
			return;
		}
	}
	
	(*f) = 0.0f;
}

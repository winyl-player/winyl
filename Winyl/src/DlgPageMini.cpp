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
#include "DlgPageMini.h"

DlgPageMini::DlgPageMini()
{

}

DlgPageMini::~DlgPageMini()
{

}

INT_PTR DlgPageMini::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		OnInitDialog();
		return TRUE;
	case WM_COMMAND:
		return TRUE;
	case WM_HSCROLL:
		OnHScroll(LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
		return TRUE;
	}

	return FALSE;
}

void DlgPageMini::OnInitDialog()
{
	::SetDlgItemText(thisWnd, IDC_STATIC_ZORDER, lang->GetLine(Lang::MiniPage, 0));
	::SetDlgItemText(thisWnd, IDC_RADIO_ALWAYS_ON_TOP, lang->GetLine(Lang::MiniPage, 1));
	::SetDlgItemText(thisWnd, IDC_RADIO_ALWAYS_ON_DESKTOP, lang->GetLine(Lang::MiniPage, 2));
	::SetDlgItemText(thisWnd, IDC_RADIO_ALWAYS_ON_OFF, lang->GetLine(Lang::MiniPage, 3));
	::SetDlgItemText(thisWnd, IDC_STATIC_OPACITY, lang->GetLine(Lang::MiniPage, 4));

	sliderOpacity = ::GetDlgItem(thisWnd, IDC_SLIDER_OPACITY);

	if (settings->GetMiniZOrder() == 2)
		::CheckRadioButton(thisWnd, IDC_RADIO_ALWAYS_ON_TOP, IDC_RADIO_ALWAYS_ON_OFF, IDC_RADIO_ALWAYS_ON_OFF);
	else if (settings->GetMiniZOrder() == 1)
		::CheckRadioButton(thisWnd, IDC_RADIO_ALWAYS_ON_TOP, IDC_RADIO_ALWAYS_ON_OFF, IDC_RADIO_ALWAYS_ON_DESKTOP);
	else
		::CheckRadioButton(thisWnd, IDC_RADIO_ALWAYS_ON_TOP, IDC_RADIO_ALWAYS_ON_OFF, IDC_RADIO_ALWAYS_ON_TOP);

	::SendMessage(sliderOpacity, TBM_SETRANGE, TRUE, MAKELPARAM(0, 80));
	::SendMessage(sliderOpacity, TBM_SETPOS, TRUE, settings->GetMiniTransparency());
}

void DlgPageMini::OnHScroll(UINT nSBCode, UINT nPos, HWND hScrollBar)
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
			if (hScrollBar == sliderOpacity)
			{
				int pos = (int)::SendMessage(sliderOpacity, TBM_GETPOS, 0, 0);
				skinMini->SetTransparency(pos);
			}
	}
}

void DlgPageMini::SaveSettings()
{
	if (::IsDlgButtonChecked(thisWnd, IDC_RADIO_ALWAYS_ON_OFF))
		settings->SetMiniZOrder(2);
	else if (::IsDlgButtonChecked(thisWnd, IDC_RADIO_ALWAYS_ON_DESKTOP))
		settings->SetMiniZOrder(1);
	else
		settings->SetMiniZOrder(0);

	skinMini->SetZOrder(settings->GetMiniZOrder());

	int pos = (int)::SendMessage(sliderOpacity, TBM_GETPOS, 0, 0);
	settings->SetMiniTransparency(pos);
}

void DlgPageMini::CancelSettings()
{
	int pos = (int)::SendMessage(sliderOpacity, TBM_GETPOS, 0, 0);
	if (pos != settings->GetMiniTransparency())
	{
		skinMini->SetTransparency(settings->GetMiniTransparency());
	}
}

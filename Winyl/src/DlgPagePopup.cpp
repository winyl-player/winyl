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

// DlgPagePopup.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgPagePopup.h"


// DlgPagePopup dialog

DlgPagePopup::DlgPagePopup()
{

}

DlgPagePopup::~DlgPagePopup()
{

}

INT_PTR DlgPagePopup::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		OnInitDialog();
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_POPUP_TEST:
			OnBnClickedButtonPopupTest();
			break;
		}
		return TRUE;
	case WM_HSCROLL:
		OnHScroll(LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
		return TRUE;
	}

	return FALSE;
}

void DlgPagePopup::OnInitDialog()
{
	::SetDlgItemText(thisWnd, IDC_BUTTON_POPUP_TEST, lang->GetLine(Lang::PopupPage, 0));

	SetDlgItemText(thisWnd, IDC_STATIC_POPUP_TIME, lang->GetLine(Lang::PopupPage, 1));
	SetDlgItemText(thisWnd, IDC_STATIC_POPUP_DELAY, lang->GetLine(Lang::PopupPage, 2));
	SetDlgItemText(thisWnd, IDC_STATIC_POPUP_START, lang->GetLine(Lang::PopupPage, 3));
	SetDlgItemText(thisWnd, IDC_STATIC_POPUP_END, lang->GetLine(Lang::PopupPage, 4));

	SetDlgItemText(thisWnd, IDC_STATIC_POPUP_VIEW, lang->GetLine(Lang::PopupPage, 6));
	SetDlgItemText(thisWnd, IDC_STATIC_POPUP_POSITION, lang->GetLine(Lang::PopupPage, 7));
	SetDlgItemText(thisWnd, IDC_STATIC_POPUP_EFFECT, lang->GetLine(Lang::PopupPage, 12));
	SetDlgItemText(thisWnd, IDC_STATIC_POPUP_EFFECT_HIDE, lang->GetLine(Lang::PopupPage, 13));

	//::CheckDlgButton(thisWnd, IDC_CHECK_POPUP, settings->IsPopup());

	sliderHold = ::GetDlgItem(thisWnd, IDC_SLIDER_POPUP_HOLD);
	sliderShow = ::GetDlgItem(thisWnd, IDC_SLIDER_POPUP_SHOW);
	sliderHide = ::GetDlgItem(thisWnd, IDC_SLIDER_POPUP_HIDE);

	::SendMessage(sliderHold, TBM_SETRANGE, TRUE, MAKELPARAM(1, 50));
	::SendMessage(sliderShow, TBM_SETRANGE, TRUE, MAKELPARAM(1, 50));
	::SendMessage(sliderHide, TBM_SETRANGE, TRUE, MAKELPARAM(1, 50));

	::SendMessage(sliderHold, TBM_SETPOS, TRUE, settings->GetPopupHold()/100);
	::SendMessage(sliderShow, TBM_SETPOS, TRUE, settings->GetPopupShow()/100);
	::SendMessage(sliderHide, TBM_SETPOS, TRUE, settings->GetPopupHide()/100);

	HWND comboPosition = ::GetDlgItem(thisWnd, IDC_COMBO_POPUP_POSITION);
	ComboBox_AddString(comboPosition, lang->GetLine(Lang::PopupPage, 8));
	ComboBox_AddString(comboPosition, lang->GetLine(Lang::PopupPage, 9));
	ComboBox_AddString(comboPosition, lang->GetLine(Lang::PopupPage, 10));
	ComboBox_AddString(comboPosition, lang->GetLine(Lang::PopupPage, 11));

	HWND comboShow = ::GetDlgItem(thisWnd, IDC_COMBO_POPUP_SHOW);
	ComboBox_AddString(comboShow, lang->GetLine(Lang::PopupPage, 14));
	ComboBox_AddString(comboShow, lang->GetLine(Lang::PopupPage, 15));
	ComboBox_AddString(comboShow, lang->GetLine(Lang::PopupPage, 16));

	HWND comboHide = ::GetDlgItem(thisWnd, IDC_COMBO_POPUP_HIDE);
	ComboBox_AddString(comboHide, lang->GetLine(Lang::PopupPage, 14));
	ComboBox_AddString(comboHide, lang->GetLine(Lang::PopupPage, 15));
	ComboBox_AddString(comboHide, lang->GetLine(Lang::PopupPage, 16));

	ShowDelayTime(settings->GetPopupHold()/100, 0);
	ShowDelayTime(settings->GetPopupShow()/100, 1);
	ShowDelayTime(settings->GetPopupHide()/100, 2);

	ComboBox_SetCurSel(comboPosition, settings->GetPopupPosition());
	ComboBox_SetCurSel(comboShow, settings->GetPopupShowEffect());
	ComboBox_SetCurSel(comboHide, settings->GetPopupHideEffect());
}

void DlgPagePopup::OnHScroll(UINT nSBCode, UINT nPos, HWND hScrollBar)
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
			if (hScrollBar == sliderHold)
				ShowDelayTime((int)::SendMessage(sliderHold, TBM_GETPOS, 0, 0), 0);
			else if (hScrollBar == sliderShow)
				ShowDelayTime((int)::SendMessage(sliderShow, TBM_GETPOS, 0, 0), 1);
			else if (hScrollBar == sliderHide)
				ShowDelayTime((int)::SendMessage(sliderHide, TBM_GETPOS, 0, 0), 2);
	}
}

void DlgPagePopup::ShowDelayTime(int delay, int type)
{
	wchar_t sec[10];
	swprintf_s(sec, L"%d.%d ", delay / 10, delay % 10);

	if (type == 0)
		::SetDlgItemText(thisWnd, IDC_STATIC_POPUP_SEC1, (sec + lang->GetLineS(Lang::PopupPage, 5)).c_str());
	else if (type == 1)
		::SetDlgItemText(thisWnd, IDC_STATIC_POPUP_SEC2, (sec + lang->GetLineS(Lang::PopupPage, 5)).c_str());
	else if (type == 2)
		::SetDlgItemText(thisWnd, IDC_STATIC_POPUP_SEC3, (sec + lang->GetLineS(Lang::PopupPage, 5)).c_str());
}

void DlgPagePopup::OnBnClickedButtonPopupTest()
{
	if (skinPopup == NULL)
		return;

	int selPos = ComboBox_GetCurSel(::GetDlgItem(thisWnd, IDC_COMBO_POPUP_POSITION));
	int selShow = ComboBox_GetCurSel(::GetDlgItem(thisWnd, IDC_COMBO_POPUP_SHOW));
	int selHide = ComboBox_GetCurSel(::GetDlgItem(thisWnd, IDC_COMBO_POPUP_HIDE));

	int posHold = (int)::SendMessage(sliderHold, TBM_GETPOS, 0, 0);
	int posShow = (int)::SendMessage(sliderShow, TBM_GETPOS, 0, 0);
	int posHide = (int)::SendMessage(sliderHide, TBM_GETPOS, 0, 0);

	skinPopup->SetPosition(selPos);
	skinPopup->SetEffect(selShow, selHide);
	skinPopup->SetDelay(posHold*100, posShow*100, posHide*100);

	if (!(*ptrIsMediaPlay))
		skinPopup->SetText(L"Mama Said", L"Load", L"Metallica", L"Heavy Metal", L"1996", 319);

	skinPopup->Popup();

	skinPopup->SetPosition(settings->GetPopupPosition());
	skinPopup->SetEffect(settings->GetPopupShowEffect(), settings->GetPopupHideEffect());
	skinPopup->SetDelay(settings->GetPopupHold(), settings->GetPopupShow(), settings->GetPopupHide());
}

void DlgPagePopup::SaveSettings()
{
	int selPos = ComboBox_GetCurSel(::GetDlgItem(thisWnd, IDC_COMBO_POPUP_POSITION));
	int selShow = ComboBox_GetCurSel(::GetDlgItem(thisWnd, IDC_COMBO_POPUP_SHOW));
	int selHide = ComboBox_GetCurSel(::GetDlgItem(thisWnd, IDC_COMBO_POPUP_HIDE));

	int posHold = (int)::SendMessage(sliderHold, TBM_GETPOS, 0, 0);
	int posShow = (int)::SendMessage(sliderShow, TBM_GETPOS, 0, 0);
	int posHide = (int)::SendMessage(sliderHide, TBM_GETPOS, 0, 0);

	settings->SetPopupPosition(selPos);
	settings->SetPopupEffect(selShow, selHide);
	settings->SetPopupDelay(posHold*100, posShow*100, posHide*100);

	//contextMenu->CheckPopup(!!::IsDlgButtonChecked(thisWnd, IDC_CHECK_POPUP));
	contextMenu->CheckPopupPosition(selPos);

	if (skinPopup)
	{
		skinPopup->SetPosition(selPos);
		skinPopup->SetEffect(selShow, selHide);
		skinPopup->SetDelay(posHold*100, posShow*100, posHide*100);
	}
}

void DlgPagePopup::CancelSettings()
{

}

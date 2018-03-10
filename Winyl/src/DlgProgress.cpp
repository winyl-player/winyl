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
#include "DlgProgress.h"

DlgProgress::DlgProgress()
{

}

DlgProgress::~DlgProgress()
{

}

INT_PTR DlgProgress::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		if (wndMessage)
			CenterDialog(hDlg, wndMessage);
		OnInitDialog();
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			OnBnClickedOK();
			return TRUE;
		case IDCANCEL:
			OnBnClickedCancel();
			return TRUE;
		case IDC_BUTTON_MINIMIZE:
			OnBnClickedButtonMinimize();
			return TRUE;
		}
		return TRUE;
	}

	if (wmTaskbarButtonCreated && wmTaskbarButtonCreated == message)
		OnTaskbarButtonCreated();

	return FALSE;
}

void DlgProgress::OnInitDialog()
{
	if (wndMessage == NULL)
	{
		LONG_PTR style = ::GetWindowLongPtr(thisWnd, GWL_STYLE);
		::SetWindowLongPtr(thisWnd, GWL_STYLE, style & ~WS_MINIMIZEBOX);

		::ShowWindow(::GetDlgItem(thisWnd, IDC_BUTTON_MINIMIZE), SW_HIDE);
	}

	if (!progress.IsAddToPlaylist())
		::SetWindowText(thisWnd, lang->GetLine(Lang::ProcessDialog, 0));
	else
		::SetWindowText(thisWnd, lang->GetLine(Lang::ProcessDialog, 1));

	::SetDlgItemText(thisWnd, IDCANCEL, lang->GetLine(Lang::ProcessDialog, 2));
	::SetDlgItemText(thisWnd, IDC_BUTTON_MINIMIZE, lang->GetLine(Lang::ProcessDialog, 3));
	::SetDlgItemText(thisWnd, IDC_STATIC_FILE, lang->GetLine(Lang::ProcessDialog, 4));

	progressControl = ::GetDlgItem(thisWnd, IDC_PROGRESS);

	namespace ph = std::placeholders;

	progress.SetFuncFinish(std::bind(&DlgProgress::Finish, this));
	progress.SetFuncUpdateProgressMarquee(std::bind(&DlgProgress::UpdateProgressMarquee, this, ph::_1));
	progress.SetFuncUpdateProgressRange(std::bind(&DlgProgress::UpdateProgressRange, this, ph::_1, ph::_2));
	progress.SetFuncUpdateProgressPos(std::bind(&DlgProgress::UpdateProgressPos, this, ph::_1, ph::_2));
	progress.SetFuncUpdateProgressText(std::bind(&DlgProgress::UpdateProgressText, this, ph::_1));
	progress.SetFuncUpdateProgressTextEmpty(std::bind(&DlgProgress::UpdateProgressTextEmpty, this, ph::_1));

	progress.Init();
}

void DlgProgress::OnBnClickedOK()
{
	progress.WaitForJoin();

	if (wndMessage)
		::SendMessage(wndMessage, UWM_SCANEND, (WPARAM)isDestroyOnStop, 0);
	else
		EndDialog(IDOK);
}

void DlgProgress::OnBnClickedCancel()
{
	::EnableWindow(::GetDlgItem(thisWnd, IDCANCEL), FALSE);
	progress.Cancel();
}

void DlgProgress::OnBnClickedButtonMinimize()
{
	::ShowWindow(thisWnd, SW_MINIMIZE);
}

void DlgProgress::DestroyOnStop()
{
	isDestroyOnStop = true;
	OnBnClickedCancel();
}

void DlgProgress::Finish()
{
	::PostMessage(thisWnd, WM_COMMAND, IDOK, 0);
}

void DlgProgress::UpdateProgressMarquee(bool isMarquee)
{
	if  (isMarquee)
	{
		LONG_PTR style = ::GetWindowLongPtr(progressControl, GWL_STYLE);
		::SetWindowLongPtr(progressControl, GWL_STYLE, style | PBS_MARQUEE);

		::SendMessage(progressControl, PBM_SETMARQUEE, TRUE, 0);

		isMarqueeTaskbarButton = true;
		if (taskListControl)
			taskListControl->SetProgressState(thisWnd, TBPF_INDETERMINATE);
	}
	else
	{
		LONG_PTR style = ::GetWindowLongPtr(progressControl, GWL_STYLE);
		::SetWindowLongPtr(progressControl, GWL_STYLE, style & ~PBS_MARQUEE);

		isMarqueeTaskbarButton = false;
	}
}

void DlgProgress::UpdateProgressRange(int pos, int total)
{
	::SendMessage(progressControl, PBM_SETRANGE32, 0, total);
	::SendMessage(progressControl, PBM_SETPOS, pos, 0);
}

void DlgProgress::UpdateProgressPos(int pos, int total)
{
	::SendMessage(progressControl, PBM_SETPOS, pos, 0);
	if (taskListControl)
		taskListControl->SetProgressValue(thisWnd, pos, total);
}

void DlgProgress::UpdateProgressText(const std::wstring& text)
{
	::SetDlgItemText(thisWnd, IDC_STATIC_FILE, (lang->GetLineS(Lang::ProcessDialog, 5) + L' ' + text).c_str());
}

void DlgProgress::UpdateProgressTextEmpty(bool cancel)
{
	if (cancel)
		::SetDlgItemText(thisWnd, IDC_STATIC_FILE, lang->GetLine(Lang::ProcessDialog, 6));
	else
		::SetDlgItemText(thisWnd, IDC_STATIC_FILE, lang->GetLine(Lang::ProcessDialog, 4));
}

void DlgProgress::OnTaskbarButtonCreated()
{
	if (futureWin->IsSevenOrLater())
	{
		taskListControl.CoCreateInstance(CLSID_TaskbarList);

		if (taskListControl && isMarqueeTaskbarButton)
			taskListControl->SetProgressState(thisWnd, TBPF_INDETERMINATE);
	}
}
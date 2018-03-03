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

// DlgAbout.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgAbout.h"


// DlgAbout dialog

DlgAbout::DlgAbout()
{

}

DlgAbout::~DlgAbout()
{

}

INT_PTR DlgAbout::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
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
			EndDialog(LOWORD(wParam));
			return TRUE;
		}
		return TRUE;
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
		return TRUE;
	}

	return FALSE;
}

void DlgAbout::OnInitDialog()
{
	::SetWindowText(thisWnd, lang->GetLine(Lang::AboutDialog, 0));
	::SetDlgItemText(thisWnd, IDCANCEL, lang->GetLine(Lang::AboutDialog, 1));
	::SetDlgItemText(thisWnd, IDOK, lang->GetLine(Lang::AboutDialog, 2));
	::SetDlgItemText(thisWnd, IDC_STATIC_LINE1, lang->GetLine(Lang::AboutDialog, 3));
	::SetDlgItemText(thisWnd, IDC_STATIC_LINE2, lang->GetLine(Lang::AboutDialog, 4));

	// Move the icon to the center or when High DPI the position of the icon is incorrect 
	CRect rcDialog, rcIcon;
	::GetWindowRect(thisWnd, rcDialog);
	::GetWindowRect(::GetDlgItem(thisWnd, IDC_STATIC_ICON_WINYL), rcIcon);
	::MapWindowPoints(NULL, thisWnd, (LPPOINT)&rcIcon, 2);
	rcIcon.MoveToX((rcDialog.Width() - rcIcon.Width()) / 2);
	MoveWindow(::GetDlgItem(thisWnd, IDC_STATIC_ICON_WINYL), rcIcon.left, rcIcon.top, rcIcon.Width(), rcIcon.Height(), TRUE);
	///////////////////////////////////
	
	std::wstring site = lang->GetLineS(Lang::AboutDialog, 3) + L" winyl-player.github.io";
	std::wstring mail = lang->GetLineS(Lang::AboutDialog, 4) + L" winyl.player@gmail.com";

	std::wstring siteLink = lang->GetLineS(Lang::AboutDialog, 3) + L" <A HREF=\"https://winyl-player.github.io\">winyl-player.github.io</A>";
	std::wstring mailLink = lang->GetLineS(Lang::AboutDialog, 4) + L" <A HREF=\"mailto:winyl.player@gmail.com\">winyl.player@gmail.com</A>";

	wndSite = CreateLink(IDC_STATIC_SITE, site, siteLink);
	wndMail = CreateLink(IDC_STATIC_MAIL, mail, mailLink);

	// Compose "Translated by" text line
	if (!lang->GetLineS(Lang::AboutDialog, 5).empty())
	{
		std::wstring text = lang->GetLineS(Lang::AboutDialog, 5);
		mail.clear();
		std::size_t findDog = text.rfind('@');
		if (findDog != std::string::npos)
		{
			std::size_t findMail = text.rfind(' ', findDog);
			if (findMail != std::string::npos)
			{
				mail = text.substr(findMail + 1);
				text = text.substr(0, findMail + 1);
			}
			else
			{
				mail = text;
				text.clear();
			}
		}

		std::wstring newText = text + mail;
		std::wstring newMail = text;
		if (!mail.empty())
			newMail += L"<A HREF=\"mailto:" + mail + L"\">" + mail + L"</A>";

		wndTr = CreateLink(IDC_STATIC_TRANSLATED, newText, newMail);
	}
}

HWND DlgAbout::CreateLink(int idStatic, const std::wstring& text, const std::wstring& link)
{
	CRect rc;
	::GetWindowRect(::GetDlgItem(thisWnd, idStatic), rc);
	::MapWindowPoints(NULL, thisWnd, (LPPOINT)&rc, 2);

	HDC dc = ::GetDC(::GetDlgItem(thisWnd, idStatic));
	HFONT font = (HFONT)::SendMessage(::GetDlgItem(thisWnd, idStatic), WM_GETFONT, 0, 0);

	HGDIOBJ oldObject = ::SelectObject(dc, font);

	CSize sz;
	::GetTextExtentPoint32(dc, text.c_str(), (int)text.size(), &sz);

	rc.left += (rc.Width() - sz.cx) / 2;
	rc.right = rc.left + sz.cx;

	HWND wnd = CreateWindowEx(0, WC_LINK, link.c_str(), WS_VISIBLE|WS_CHILD|WS_TABSTOP, 
		rc.left, rc.top, rc.Width(), rc.Height(), thisWnd, NULL, ::GetModuleHandle(NULL), NULL);

	// Set the font or in Vista or newer incorrent font is set
	::SendMessage(wnd, WM_SETFONT, (WPARAM)font, FALSE);

	::SelectObject(dc, oldObject);
	::ReleaseDC(::GetDlgItem(thisWnd, idStatic), dc);

	return wnd;
}

void DlgAbout::OnBnClickedOK()
{
	::ShellExecute(NULL, L"open", L"https://www.patreon.com/winyl", NULL, NULL, SW_SHOW);
}

void DlgAbout::OnBnClickedCancel()
{

}

void DlgAbout::OnNotify(WPARAM wParam, LPARAM lParam)
{
	NMHDR* nmhdr = reinterpret_cast<NMHDR*>(lParam);

    switch (nmhdr->code)
    {
	    case NM_CLICK:
		case NM_RETURN:
        {
            if (nmhdr->hwndFrom == wndSite)
            {
		        PNMLINK pNMLink = (PNMLINK)lParam;
	            LITEM item = pNMLink->item;

				::ShellExecute(NULL, L"open", item.szUrl, NULL, NULL, SW_SHOW);
            }
            else if (nmhdr->hwndFrom == wndMail)
            {
		        PNMLINK pNMLink = (PNMLINK)lParam;
	            LITEM item = pNMLink->item;

				::ShellExecute(NULL, L"open", item.szUrl, NULL, NULL, SW_SHOW);
            }
            else if (nmhdr->hwndFrom == wndTr)
            {
		        PNMLINK pNMLink = (PNMLINK)lParam;
	            LITEM item = pNMLink->item;

				::ShellExecute(NULL, L"open", item.szUrl, NULL, NULL, SW_SHOW);
            }
        }
	}
}

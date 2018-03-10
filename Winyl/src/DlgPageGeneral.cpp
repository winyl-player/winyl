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
#include "DlgPageGeneral.h"
#include "DlgPageGeneralXP.h"

DlgPageGeneral::DlgPageGeneral()
{

}

DlgPageGeneral::~DlgPageGeneral()
{

}

INT_PTR DlgPageGeneral::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		OnInitDialog();
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_ASSOC:
			OnBnClickedButtonAssoc();
			break;
		}
		return TRUE;
	}

	return FALSE;
}

void DlgPageGeneral::OnInitDialog()
{
	::SetDlgItemText(thisWnd, IDC_STATIC_BEHAVIOR, lang->GetLine(Lang::GeneralPage, 0));
	::SetDlgItemText(thisWnd, IDC_CHECK_ADD_ALL, lang->GetLine(Lang::GeneralPage, 1));
	::SetDlgItemText(thisWnd, IDC_CHECK_PLAY_FOCUS, lang->GetLine(Lang::GeneralPage, 2));
	::SetDlgItemText(thisWnd, IDC_STATIC_LIBRARY_VIEW, lang->GetLine(Lang::GeneralPage, 3));
	//::SetDlgItemText(thisWnd, IDC_STATIC_SERVICES, lang->GetLine(Lang::GeneralPage, 4));
	::SetDlgItemText(thisWnd, IDC_STATIC_SERVICES, L"Last.FM");
	::SetDlgItemText(thisWnd, IDC_CHECK_LASTFM, lang->GetLine(Lang::GeneralPage, 5));
	//::SetDlgItemText(thisWnd, IDC_CHECK_MSN, lang->GetLine(Lang::GeneralPage, 6));
	::SetDlgItemText(thisWnd, IDC_STATIC_ASSOCIATIONS, lang->GetLine(Lang::GeneralPage, 7));
	::SetDlgItemText(thisWnd, IDC_CHECK_ASSOC, lang->GetLine(Lang::GeneralPage, 9));
	::SetDlgItemText(thisWnd, IDC_BUTTON_ASSOC, lang->GetLine(Lang::GeneralPage, 10));
	
	::SetDlgItemText(thisWnd, IDC_CHECK_ARTISTS, lang->GetLine(Lang::Library, 5));
	::SetDlgItemText(thisWnd, IDC_CHECK_COMPOSERS, lang->GetLine(Lang::Library, 6));
	::SetDlgItemText(thisWnd, IDC_CHECK_ALBUMS, lang->GetLine(Lang::Library, 7));
	::SetDlgItemText(thisWnd, IDC_CHECK_GENRES, lang->GetLine(Lang::Library, 8));
	::SetDlgItemText(thisWnd, IDC_CHECK_YEARS, lang->GetLine(Lang::Library, 9));
	::SetDlgItemText(thisWnd, IDC_CHECK_FOLDERS, lang->GetLine(Lang::Library, 11));
	::SetDlgItemText(thisWnd, IDC_CHECK_RADIOS, lang->GetLine(Lang::Library, 10));
	::SetDlgItemText(thisWnd, IDC_CHECK_SMARTLISTS, lang->GetLine(Lang::Library, 4));

	Associations assoc;
	isAssocFolder = assoc.QueryAssocFolder();
	::CheckDlgButton(thisWnd, IDC_CHECK_ASSOC, isAssocFolder);

	::CheckDlgButton(thisWnd, IDC_CHECK_ADD_ALL, settings->IsAddAllToLibrary());
	::CheckDlgButton(thisWnd, IDC_CHECK_PLAY_FOCUS, settings->IsPlayFocus());

	::CheckDlgButton(thisWnd, IDC_CHECK_LASTFM, settings->IsLastFM());
	//::CheckDlgButton(thisWnd, IDC_CHECK_MSN, settings->IsMSN());

	::CheckDlgButton(thisWnd, IDC_CHECK_ARTISTS, settings->IsLibraryArtists());
	::CheckDlgButton(thisWnd, IDC_CHECK_COMPOSERS, settings->IsLibraryComposers());
	::CheckDlgButton(thisWnd, IDC_CHECK_ALBUMS, settings->IsLibraryAlbums());
	::CheckDlgButton(thisWnd, IDC_CHECK_GENRES, settings->IsLibraryGenres());
	::CheckDlgButton(thisWnd, IDC_CHECK_YEARS, settings->IsLibraryYears());
	::CheckDlgButton(thisWnd, IDC_CHECK_FOLDERS, settings->IsLibraryFolders());
	::CheckDlgButton(thisWnd, IDC_CHECK_RADIOS, settings->IsLibraryRadios());
	::CheckDlgButton(thisWnd, IDC_CHECK_SMARTLISTS, settings->IsLibrarySmartlists());

	if (isHideAssoc)
	{
		::EnableWindow(::GetDlgItem(thisWnd, IDC_CHECK_ASSOC), FALSE);
		::EnableWindow(::GetDlgItem(thisWnd, IDC_BUTTON_ASSOC), FALSE);
	}
}

void DlgPageGeneral::OnBnClickedButtonAssoc()
{
	if (futureWin->IsVistaOrLater())
	{
		Associations assoc;
		assoc.AssocDialogVista();
	}
	else
	{
		DlgPageGeneralXP dlg;
		dlg.SetLanguage(lang);
		dlg.ModalDialog(thisWnd, IDD_DLGPAGEGENERALXP);
	}
}

void DlgPageGeneral::SaveSettings()
{
	bool isCheckArtists    = !!::IsDlgButtonChecked(thisWnd, IDC_CHECK_ARTISTS);
	bool isCheckComposers  = !!::IsDlgButtonChecked(thisWnd, IDC_CHECK_COMPOSERS);
	bool isCheckAlbums     = !!::IsDlgButtonChecked(thisWnd, IDC_CHECK_ALBUMS);
	bool isCheckGenres     = !!::IsDlgButtonChecked(thisWnd, IDC_CHECK_GENRES);
	bool isCheckYears      = !!::IsDlgButtonChecked(thisWnd, IDC_CHECK_YEARS);
	bool isCheckFolders    = !!::IsDlgButtonChecked(thisWnd, IDC_CHECK_FOLDERS);
	bool isCheckRadios     = !!::IsDlgButtonChecked(thisWnd, IDC_CHECK_RADIOS);
	bool isCheckSmartlists = !!::IsDlgButtonChecked(thisWnd, IDC_CHECK_SMARTLISTS);

	if (isCheckArtists    != settings->IsLibraryArtists()   ||
		isCheckComposers  != settings->IsLibraryComposers() ||
		isCheckAlbums     != settings->IsLibraryAlbums()    ||
		isCheckGenres     != settings->IsLibraryGenres()    ||
		isCheckYears      != settings->IsLibraryYears()     ||
		isCheckFolders    != settings->IsLibraryFolders()   ||
		isCheckRadios     != settings->IsLibraryRadios()    ||
		isCheckSmartlists != settings->IsLibrarySmartlists())
	{
		settings->SetLibraryArtists(isCheckArtists);
		settings->SetLibraryComposers(isCheckComposers);
		settings->SetLibraryAlbums(isCheckAlbums);
		settings->SetLibraryGenres(isCheckGenres);
		settings->SetLibraryYears(isCheckYears);
		settings->SetLibraryFolders(isCheckFolders);
		settings->SetLibraryRadios(isCheckRadios);
		settings->SetLibrarySmartlists(isCheckSmartlists);

		isUpdateLibrary = true;
	}

	bool isCheckAddAllToLibbrary = !!::IsDlgButtonChecked(thisWnd, IDC_CHECK_ADD_ALL);
	if (isCheckAddAllToLibbrary != settings->IsAddAllToLibrary())
	{
		settings->SetAddAllToLibrary(isCheckAddAllToLibbrary);
		isUpdateLibrary = true;
	}

	bool isCheckPlayFocus = !!::IsDlgButtonChecked(thisWnd, IDC_CHECK_PLAY_FOCUS);
	if (isCheckPlayFocus != settings->IsPlayFocus())
	{
		settings->SetPlayFocus(isCheckPlayFocus);
	}

	//bool isCheckMSN = !!::IsDlgButtonChecked(thisWnd, IDC_CHECK_MSN);
	//if (isCheckMSN != settings->IsMSN())
	//{
	//	settings->SetMSN(isCheckMSN);
	//}

	bool isCheckLastFM = !!::IsDlgButtonChecked(thisWnd, IDC_CHECK_LASTFM);
	if (isCheckLastFM != settings->IsLastFM())
	{
		settings->SetLastFM(isCheckLastFM);

		if (settings->IsLastFM())
			settings->SetLastFM(lastFM->Init());
		else
		{
			lastFM->Stop();
			lastFM->Free();
		}
	}

	bool isCheckAssoc = !!::IsDlgButtonChecked(thisWnd, IDC_CHECK_ASSOC);
	if (isCheckAssoc != isAssocFolder)
	{
		Associations assoc;
		assoc.SetProgramPath(programPath);
		// !!! Also changed in WinylWnd::DialogLanguage()
		assoc.SetPlayInWinylString(lang->GetLineS(Lang::GeneralPage, 8));

		if (isCheckAssoc)
			assoc.AddAssocFolder();
		else
			assoc.RemoveAssocFolder();
	}
}

void DlgPageGeneral::CancelSettings()
{

}


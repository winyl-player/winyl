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
#include "ContextMenu.h"
#include <cassert>
#include "LyricsLoader.h"

ContextMenu::ContextMenu()
{

}

ContextMenu::~ContextMenu()
{
	DestroyAll();
}

void ContextMenu::FillMenu()
{
	mainMenu        = ::CreatePopupMenu();
	mainMenuPlay    = ::CreatePopupMenu();
	mainMenuOpen    = ::CreatePopupMenu();
	mainMenuSetting = ::CreatePopupMenu();
	listMenu        = ::CreatePopupMenu();
	listMenuPlay    = ::CreatePopupMenu();
	listMenuJump    = ::CreatePopupMenu();
	treePlayMenu    = ::CreatePopupMenu();
	treeSmartMenu   = ::CreatePopupMenu();
	trayMenu        = ::CreatePopupMenu();
	popupMenu       = ::CreatePopupMenu();
	searchMenu      = ::CreatePopupMenu();
	lyricsMenu      = ::CreatePopupMenu();
	lyricsMenuFont  = ::CreatePopupMenu();
	lyricsMenuAlign = ::CreatePopupMenu();
	lyricsMenuProv  = ::CreatePopupMenu();

	FillMainMenu();
	FillTrayMenu();
	FillListMenu();
	FillTreePlaylistMenu();
	FillTreeSmartlistMenu();
	FillPopupMenu();
	FillSearchMenu();
	FillLyricsMenu();
}

void ContextMenu::DestroyAll()
{
	::DestroyMenu(mainMenu);
//	::DestroyMenu(mainMenuPlay);
//	::DestroyMenu(mainMenuOpen);
//	::DestroyMenu(mainMenuSetting);
	::DestroyMenu(listMenu);
//	::DestroyMenu(listMenuPlay);
	::DestroyMenu(treePlayMenu);
	::DestroyMenu(treeSmartMenu);
	::DestroyMenu(trayMenu);
	::DestroyMenu(popupMenu);
	::DestroyMenu(searchMenu);
	::DestroyMenu(lyricsMenu);
//	::DestroyMenu(lyricsMenuFont);
//	::DestroyMenu(lyricsMenuAlign);
}

void ContextMenu::ReloadMenu()
{
	DestroyAll();
	FillMenu();
}

void ContextMenu::FillMainMenu()
{
	::AppendMenu(mainMenu, MF_STRING, ID_MENU_LIBRARY, (lang->GetLineS(Lang::MainMenu, 0) + L"\t F3").c_str());
	::AppendMenu(mainMenu, MF_STRING, ID_MENU_SKIN, (lang->GetLineS(Lang::MainMenu, 1) + L"\t F4").c_str());
	
	::AppendMenu(mainMenu, MF_SEPARATOR, NULL, NULL);
	::AppendMenu(mainMenu, MF_STRING, ID_MENU_MINI, (lang->GetLineS(Lang::MainMenu, 2) + L"\t F5").c_str());

	::AppendMenu(mainMenu, MF_SEPARATOR, NULL, NULL);

//	::AppendMenu(mainMenu, MF_STRING, ID_MENU_OPEN_PLAYLIST, L"New Playlist" + L"\t Ctrl+N");
//	::AppendMenu(mainMenu, MF_STRING, ID_MENU_OPEN_PLAYLIST, L"New Smart Playlist" + L"\t Ctrl+M");

	::AppendMenu(mainMenu, MF_STRING|MF_POPUP, (UINT_PTR)mainMenuOpen, lang->GetLine(Lang::MainMenu, 3));
	::AppendMenu(mainMenuOpen, MF_STRING, ID_MENU_OPEN_FILE, (lang->GetLineS(Lang::MainMenu, 4) + L"\t Ctrl+O").c_str());
	::AppendMenu(mainMenuOpen, MF_STRING, ID_MENU_OPEN_FOLDER, (lang->GetLineS(Lang::MainMenu, 5) + L"\t Ctrl+L").c_str());
	::AppendMenu(mainMenuOpen, MF_STRING, ID_MENU_OPEN_URL, (lang->GetLineS(Lang::MainMenu, 6) + L"\t Ctrl+U").c_str());
	
	::AppendMenu(mainMenu, MF_STRING, ID_MENU_OPEN_PLAYLIST, lang->GetLine(Lang::MainMenu, 7));

	::AppendMenu(mainMenu, MF_SEPARATOR, NULL, NULL);

	::AppendMenu(mainMenu, MF_STRING|MF_POPUP, (UINT_PTR)mainMenuPlay, lang->GetLine(Lang::MainMenu, 8));
	::AppendMenu(mainMenuPlay, MF_STRING, ID_MENU_PLAY, (lang->GetLineS(Lang::MainMenu, 9) + L"\t Enter").c_str());
	::AppendMenu(mainMenuPlay, MF_STRING, ID_MENU_PAUSE, (lang->GetLineS(Lang::MainMenu, 10) + L"\t Space").c_str());
	::AppendMenu(mainMenuPlay, MF_STRING, ID_MENU_STOP, (lang->GetLineS(Lang::MainMenu, 11) + L"\t Ctrl+Space").c_str());
	::AppendMenu(mainMenuPlay, MF_STRING, ID_MENU_NEXT, (lang->GetLineS(Lang::MainMenu, 12) + L"\t Ctrl+Right").c_str());
	::AppendMenu(mainMenuPlay, MF_STRING, ID_MENU_PREV, (lang->GetLineS(Lang::MainMenu, 13) + L"\t Ctrl+Left").c_str());
	::AppendMenu(mainMenuPlay, MF_SEPARATOR, NULL, NULL);
	::AppendMenu(mainMenuPlay, MF_STRING, ID_MENU_MUTE, (lang->GetLineS(Lang::MainMenu, 14) + L"\t Ctrl+M").c_str());
	::AppendMenu(mainMenuPlay, MF_SEPARATOR, NULL, NULL);
	::AppendMenu(mainMenuPlay, MF_STRING, ID_MENU_SHUFFLE, (lang->GetLineS(Lang::MainMenu, 15) + L"\t Ctrl+S").c_str());
	::AppendMenu(mainMenuPlay, MF_STRING, ID_MENU_REPEAT, (lang->GetLineS(Lang::MainMenu, 16) + L"\t Ctrl+R").c_str());

	::AppendMenu(mainMenu, MF_STRING, ID_MENU_EQUALIZER, (lang->GetLineS(Lang::MainMenu, 17) + L"\t F6").c_str());

	::AppendMenu(mainMenu, MF_SEPARATOR, NULL, NULL);

	::AppendMenu(mainMenu, MF_STRING|MF_POPUP, (UINT_PTR)mainMenuSetting, lang->GetLine(Lang::MainMenu, 18));
	::AppendMenu(mainMenuSetting, MF_STRING, ID_MENU_HIDETOTRAY, lang->GetLine(Lang::MainMenu, 19));
	::AppendMenu(mainMenuSetting, MF_STRING, ID_MENU_POPUP, lang->GetLine(Lang::MainMenu, 20));
	::AppendMenu(mainMenuSetting, MF_STRING, ID_MENU_EFFECT, lang->GetLine(Lang::MainMenu, 21));
	::AppendMenu(mainMenuSetting, MF_STRING, ID_MENU_SMOOTH, lang->GetLine(Lang::MainMenu, 22));
//	::AppendMenu(mainMenuSetting, MF_STRING, ID_MENU_LASTFM, lang->GetLine(Lang::MAIN_MENU, 22));
	::AppendMenu(mainMenuSetting, MF_SEPARATOR, NULL, NULL);
	::AppendMenu(mainMenuSetting, MF_STRING, ID_MENU_HOTKEY, (lang->GetLineS(Lang::MainMenu, 24) + L"\t F7").c_str());
	::AppendMenu(mainMenuSetting, MF_STRING, ID_MENU_CONFIG, (lang->GetLineS(Lang::MainMenu, 25) + L"\t F8").c_str());
	
	
	::AppendMenu(mainMenu, MF_STRING, ID_MENU_LANGUAGE, lang->GetLine(Lang::MainMenu, 26));

	::AppendMenu(mainMenu, MF_SEPARATOR, NULL, NULL);

//	::AppendMenu(mainMenu, MF_STRING, ID_MENU_ABOUT, L"Check for Updates");
	::AppendMenu(mainMenu, MF_STRING, ID_MENU_ABOUT, lang->GetLine(Lang::MainMenu, 27));
//	::AppendMenu(mainMenu, MF_STRING, ID_MENU_DONATE, lang->GetLine(Lang::MAIN_MENU, 28));
}

void ContextMenu::FillTrayMenu()
{
	::AppendMenu(trayMenu, MF_STRING, ID_MENU_PLAY, lang->GetLine(Lang::TrayMenu, 0));
	::AppendMenu(trayMenu, MF_STRING, ID_MENU_PAUSE, lang->GetLine(Lang::TrayMenu, 1));
	::AppendMenu(trayMenu, MF_STRING, ID_MENU_STOP, lang->GetLine(Lang::TrayMenu, 2));
	::AppendMenu(trayMenu, MF_STRING, ID_MENU_NEXT, lang->GetLine(Lang::TrayMenu, 3));
	::AppendMenu(trayMenu, MF_STRING, ID_MENU_PREV, lang->GetLine(Lang::TrayMenu, 4));

	::AppendMenu(trayMenu, MF_SEPARATOR, NULL, NULL);

	::AppendMenu(trayMenu, MF_STRING, ID_MENU_EXIT, lang->GetLine(Lang::TrayMenu, 5));

	if (!futureWin->IsVistaOrLater())
	{
		InitDrawTrayMenuXP(trayMenu, ID_MENU_PLAY);
		InitDrawTrayMenuXP(trayMenu, ID_MENU_PAUSE);
		InitDrawTrayMenuXP(trayMenu, ID_MENU_STOP);
		InitDrawTrayMenuXP(trayMenu, ID_MENU_NEXT);
		InitDrawTrayMenuXP(trayMenu, ID_MENU_PREV);
		InitDrawTrayMenuXP(trayMenu, ID_MENU_EXIT);
	}
	else
	{
		InitDrawTrayMenuVista(trayMenu, ID_MENU_PLAY, IDI_ICON_TRAY_PLAY);
		InitDrawTrayMenuVista(trayMenu, ID_MENU_PAUSE, IDI_ICON_TRAY_PAUSE);
		InitDrawTrayMenuVista(trayMenu, ID_MENU_STOP, IDI_ICON_TRAY_STOP);
		InitDrawTrayMenuVista(trayMenu, ID_MENU_NEXT, IDI_ICON_TRAY_NEXT);
		InitDrawTrayMenuVista(trayMenu, ID_MENU_PREV, IDI_ICON_TRAY_PREV);
		InitDrawTrayMenuVista(trayMenu, ID_MENU_EXIT, IDI_ICON_TRAY_EXIT);
	}

	InitFixDrawTrayMenu(trayMenu);
}

void ContextMenu::FillListMenu()
{
	::AppendMenu(listMenu, MF_STRING, ID_MENU_LIST_PLAY, lang->GetLine(Lang::ListMenu, 0));
	::AppendMenu(listMenu, MF_STRING, ID_MENU_LIST_REPEAT, lang->GetLine(Lang::ListMenu, 1));

	::AppendMenu(listMenu, MF_STRING|MF_POPUP, (UINT_PTR)listMenuJump, lang->GetLine(Lang::ListMenu, 9));
	::AppendMenu(listMenuJump, MF_STRING, ID_MENU_JUMPTO_ARTIST, lang->GetLine(Lang::ListMenu, 10));
	::AppendMenu(listMenuJump, MF_STRING, ID_MENU_JUMPTO_ALBUM, lang->GetLine(Lang::ListMenu, 11));
	::AppendMenu(listMenuJump, MF_STRING, ID_MENU_JUMPTO_GENRE, lang->GetLine(Lang::ListMenu, 13));
	::AppendMenu(listMenuJump, MF_STRING, ID_MENU_JUMPTO_YEAR, lang->GetLine(Lang::ListMenu, 12));
	::AppendMenu(listMenuJump, MF_STRING, ID_MENU_JUMPTO_FOLDER, lang->GetLine(Lang::ListMenu, 14));

	::AppendMenu(listMenu, MF_SEPARATOR, NULL, NULL);

	::AppendMenu(listMenu, MF_STRING, ID_MENU_SELECT_ALL, lang->GetLine(Lang::ListMenu, 2));

	::AppendMenu(listMenu, MF_STRING|MF_POPUP, (UINT_PTR)listMenuPlay, lang->GetLine(Lang::ListMenu, 4));
	::AppendMenu(listMenu, MF_STRING, ID_MENU_DELETE, lang->GetLine(Lang::ListMenu, 3));

	::AppendMenu(listMenu, MF_SEPARATOR, NULL, NULL);

	::AppendMenu(listMenu, MF_STRING, ID_MENU_FILELOCATION, lang->GetLine(Lang::ListMenu, 7));
	::AppendMenu(listMenu, MF_STRING, ID_MENU_PROPERTIES, lang->GetLine(Lang::ListMenu, 8));
}

void ContextMenu::FillTreePlaylistMenu()
{
	::AppendMenu(treePlayMenu, MF_STRING, ID_MENU_PL_NEW, lang->GetLine(Lang::TreeMenu, 0));

	::AppendMenu(treePlayMenu, MF_SEPARATOR, NULL, NULL);

	::AppendMenu(treePlayMenu, MF_STRING, ID_MENU_PL_RENAME, lang->GetLine(Lang::TreeMenu, 2));
	::AppendMenu(treePlayMenu, MF_STRING, ID_MENU_PL_DELETE, lang->GetLine(Lang::TreeMenu, 3));
}

void ContextMenu::FillTreeSmartlistMenu()
{
	::AppendMenu(treeSmartMenu, MF_STRING, ID_MENU_SM_NEW, lang->GetLine(Lang::TreeMenu, 1));

	::AppendMenu(treeSmartMenu, MF_SEPARATOR, NULL, NULL);

	::AppendMenu(treeSmartMenu, MF_STRING, ID_MENU_SM_UPDATE, lang->GetLine(Lang::TreeMenu, 4));
	::AppendMenu(treeSmartMenu, MF_STRING, ID_MENU_SM_EDIT, lang->GetLine(Lang::TreeMenu, 5));
	::AppendMenu(treeSmartMenu, MF_STRING, ID_MENU_SM_RENAME, lang->GetLine(Lang::TreeMenu, 2));
	::AppendMenu(treeSmartMenu, MF_STRING, ID_MENU_SM_DELETE, lang->GetLine(Lang::TreeMenu, 3));
}

void ContextMenu::FillPopupMenu()
{
	::AppendMenu(popupMenu, MF_STRING, ID_MENU_POPUP_DISABLE, lang->GetLine(Lang::PopupMenu, 0));
	::AppendMenu(popupMenu, MF_SEPARATOR, NULL, NULL);
	::AppendMenu(popupMenu, MF_STRING, ID_MENU_POPUP_TL, lang->GetLine(Lang::PopupMenu, 1));
	::AppendMenu(popupMenu, MF_STRING, ID_MENU_POPUP_TR, lang->GetLine(Lang::PopupMenu, 2));
	::AppendMenu(popupMenu, MF_STRING, ID_MENU_POPUP_BL, lang->GetLine(Lang::PopupMenu, 3));
	::AppendMenu(popupMenu, MF_STRING, ID_MENU_POPUP_BR, lang->GetLine(Lang::PopupMenu, 4));
	::AppendMenu(popupMenu, MF_SEPARATOR, NULL, NULL);
	::AppendMenu(popupMenu, MF_STRING, ID_MENU_POPUP_CONFIG, lang->GetLine(Lang::PopupMenu, 5));
}

void ContextMenu::FillSearchMenu()
{
	::AppendMenu(searchMenu, MF_STRING, ID_MENU_SEARCH_ALL, lang->GetLine(Lang::Search, 1));
	::AppendMenu(searchMenu, MF_SEPARATOR, NULL, NULL);
	::AppendMenu(searchMenu, MF_STRING, ID_MENU_SEARCH_TRACK, lang->GetLine(Lang::Search, 2));
	::AppendMenu(searchMenu, MF_STRING, ID_MENU_SEARCH_ALBUM, lang->GetLine(Lang::Search, 3));
	::AppendMenu(searchMenu, MF_STRING, ID_MENU_SEARCH_ARTIST, lang->GetLine(Lang::Search, 4));
}

void ContextMenu::FillLyricsMenu()
{
	::AppendMenu(lyricsMenu, MF_STRING|MF_GRAYED, ID_MENU_LYRICS_SOURCE, L"");
	::AppendMenu(lyricsMenu, MF_SEPARATOR, NULL, NULL);
	::AppendMenu(lyricsMenu, MF_STRING, ID_MENU_LYRICS_COPY, lang->GetLine(Lang::LyricsMenu, 5));
	::AppendMenu(lyricsMenu, MF_STRING, ID_MENU_LYRICS_SAVE, lang->GetLine(Lang::LyricsMenu, 6));
	::AppendMenu(lyricsMenu, MF_SEPARATOR, NULL, NULL);

	::AppendMenu(lyricsMenu, MF_STRING|MF_POPUP, (UINT_PTR)lyricsMenuProv, lang->GetLine(Lang::LyricsMenu, 7));
	::AppendMenu(lyricsMenu, MF_STRING, ID_MENU_LYRICS_GOOGLE, lang->GetLine(Lang::LyricsMenu, 9));
	::AppendMenu(lyricsMenuProv, MF_STRING, ID_MENU_LYRICS_PROV_OFF, lang->GetLine(Lang::LyricsMenu, 8));
	::AppendMenu(lyricsMenuProv, MF_SEPARATOR, NULL, NULL);
	for (int i = 0; i < LyricsLoader::GetLyricsProviderCount(); i++)
		::AppendMenu(lyricsMenuProv, MF_STRING, ID_MENU_LYRICS_PROV_1 + i, LyricsLoader::GetLyricsProvider(i));

	::AppendMenu(lyricsMenu, MF_SEPARATOR, NULL, NULL);
	::AppendMenu(lyricsMenu, MF_STRING|MF_POPUP, (UINT_PTR)lyricsMenuAlign, lang->GetLine(Lang::LyricsMenu, 10));
	::AppendMenu(lyricsMenuAlign, MF_STRING, ID_MENU_LYRICS_ALIGN_LEFT, lang->GetLine(Lang::LyricsMenu, 11));
	::AppendMenu(lyricsMenuAlign, MF_STRING, ID_MENU_LYRICS_ALIGN_CENTER, lang->GetLine(Lang::LyricsMenu, 12));
	::AppendMenu(lyricsMenuAlign, MF_STRING, ID_MENU_LYRICS_ALIGN_RIGHT, lang->GetLine(Lang::LyricsMenu, 13));

	::AppendMenu(lyricsMenu, MF_STRING|MF_POPUP, (UINT_PTR)lyricsMenuFont, lang->GetLine(Lang::LyricsMenu, 14));
	::AppendMenu(lyricsMenuFont, MF_STRING, ID_MENU_LYRICS_FONT_DEFAULT, lang->GetLine(Lang::LyricsMenu, 15));
	::AppendMenu(lyricsMenuFont, MF_SEPARATOR, NULL, NULL);
	::AppendMenu(lyricsMenuFont, MF_STRING, ID_MENU_LYRICS_FONT_SMALL, lang->GetLine(Lang::LyricsMenu, 16));
	::AppendMenu(lyricsMenuFont, MF_STRING, ID_MENU_LYRICS_FONT_MEDIUM, lang->GetLine(Lang::LyricsMenu, 17));
	::AppendMenu(lyricsMenuFont, MF_STRING, ID_MENU_LYRICS_FONT_LARGE, lang->GetLine(Lang::LyricsMenu, 18));
	::AppendMenu(lyricsMenuFont, MF_STRING, ID_MENU_LYRICS_FONT_LARGEST, lang->GetLine(Lang::LyricsMenu, 19));
	::AppendMenu(lyricsMenuFont, MF_SEPARATOR, NULL, NULL);
	::AppendMenu(lyricsMenuFont, MF_STRING, ID_MENU_LYRICS_FONT_BOLD, lang->GetLine(Lang::LyricsMenu, 20));
}

void ContextMenu::ShowMainMenu(HWND wnd, CPoint& pt)
{
	::TrackPopupMenu(mainMenu, TPM_LEFTALIGN/*|TPM_RIGHTBUTTON*/, pt.x, pt.y, 0, wnd, NULL);
}

void ContextMenu::ShowSearchMenu(HWND wnd, CPoint& pt)
{
	::TrackPopupMenu(searchMenu, TPM_LEFTALIGN/*|TPM_RIGHTBUTTON*/, pt.x, pt.y, 0, wnd, NULL);
}

void ContextMenu::ShowLyricsMenu(HWND wnd, CPoint& pt)
{
	::TrackPopupMenu(lyricsMenu, TPM_LEFTALIGN/*|TPM_RIGHTBUTTON*/, pt.x, pt.y, 0, wnd, NULL);
}

void ContextMenu::ShowListMenu(HWND wnd, CPoint& pt, const std::vector<std::wstring>& playlists, bool isRadio, bool isPlaylist, int selPlaylist, bool isNowPlaying)
{
	if (isRadio)
	{
		::EnableMenuItem(listMenu, ID_MENU_DELETE, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
		::EnableMenuItem(listMenu, ID_MENU_LIST_REPEAT, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
		::EnableMenuItem(listMenu, ID_MENU_FILELOCATION, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
		::EnableMenuItem(listMenu, (UINT)(UINT_PTR)listMenuJump, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
	}

	::AppendMenu(listMenuPlay, MF_STRING, 10000, lang->GetLine(Lang::ListMenu, 5));
	if (isNowPlaying)
		::AppendMenu(listMenuPlay, MF_STRING, 10001, lang->GetLine(Lang::ListMenu, 6));
	else
		::AppendMenu(listMenuPlay, MF_STRING|MF_DISABLED|MF_GRAYED, 10001, lang->GetLine(Lang::ListMenu, 6));

	::AppendMenu(listMenuPlay, MF_SEPARATOR, NULL, NULL);

	// Add playlists to "Add to Playlist" menu
	for (std::size_t i = 0, size = playlists.size(); i < size; ++i)
		::AppendMenu(listMenuPlay, MF_STRING, 10002 + i, playlists[i].c_str());

	if (isPlaylist && selPlaylist >= 0)
		::EnableMenuItem(listMenuPlay, selPlaylist + 3, MF_BYPOSITION|MF_DISABLED|MF_GRAYED);
	

	::TrackPopupMenu(listMenu, TPM_LEFTALIGN/*|TPM_RIGHTBUTTON*/, pt.x, pt.y, 0, wnd, NULL);


	if (isRadio)
	{
		::EnableMenuItem(listMenu, ID_MENU_DELETE, MF_BYCOMMAND|MF_ENABLED);
		::EnableMenuItem(listMenu, ID_MENU_LIST_REPEAT, MF_BYCOMMAND|MF_ENABLED);
		::EnableMenuItem(listMenu, ID_MENU_FILELOCATION, MF_BYCOMMAND|MF_ENABLED);
		::EnableMenuItem(listMenu, (UINT)(UINT_PTR)listMenuJump, MF_BYCOMMAND|MF_ENABLED);
	}

	// Remove all items from "Add to Playlist" menu
	while (::RemoveMenu(listMenuPlay, 0, MF_BYPOSITION));
}

void ContextMenu::ShowTreePlaylistMenu(HWND wnd, CPoint& pt, bool isEnable)
{
	if (!isEnable)
	{
		::EnableMenuItem(treePlayMenu, ID_MENU_PL_RENAME, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
		::EnableMenuItem(treePlayMenu, ID_MENU_PL_DELETE, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
	}

	::TrackPopupMenu(treePlayMenu, TPM_LEFTALIGN/*|TPM_RIGHTBUTTON*/, pt.x, pt.y, 0, wnd, NULL);

	if (!isEnable)
	{
		::EnableMenuItem(treePlayMenu, ID_MENU_PL_RENAME, MF_BYCOMMAND|MF_ENABLED);
		::EnableMenuItem(treePlayMenu, ID_MENU_PL_DELETE, MF_BYCOMMAND|MF_ENABLED);
	}
}

void ContextMenu::ShowTreeSmartlistMenu(HWND wnd, CPoint& pt, bool isEnable)
{
	if (!isEnable)
	{
		::EnableMenuItem(treeSmartMenu, ID_MENU_SM_RENAME, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
		::EnableMenuItem(treeSmartMenu, ID_MENU_SM_DELETE, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
		::EnableMenuItem(treeSmartMenu, ID_MENU_SM_EDIT, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
		::EnableMenuItem(treeSmartMenu, ID_MENU_SM_UPDATE, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
	}

	::TrackPopupMenu(treeSmartMenu, TPM_LEFTALIGN/*|TPM_RIGHTBUTTON*/, pt.x, pt.y, 0, wnd, NULL);

	if (!isEnable)
	{
		::EnableMenuItem(treeSmartMenu, ID_MENU_SM_RENAME, MF_BYCOMMAND|MF_ENABLED);
		::EnableMenuItem(treeSmartMenu, ID_MENU_SM_DELETE, MF_BYCOMMAND|MF_ENABLED);
		::EnableMenuItem(treeSmartMenu, ID_MENU_SM_EDIT, MF_BYCOMMAND|MF_ENABLED);
		::EnableMenuItem(treeSmartMenu, ID_MENU_SM_UPDATE, MF_BYCOMMAND|MF_ENABLED);
	}
}

void ContextMenu::ShowTrayMenu(HWND wnd)
{
	::SetForegroundWindow(wnd);
	::SetFocus(wnd);

	CPoint pt;
	::GetCursorPos(&pt);

	::TrackPopupMenu(trayMenu, TPM_LEFTALIGN/*|TPM_RIGHTBUTTON*/, pt.x, pt.y, 0, wnd, NULL);
}

void ContextMenu::ShowPopupMenu(HWND wnd, CPoint& pt, bool isEnableConfig)
{
	if (!isEnableConfig)
		::EnableMenuItem(popupMenu, ID_MENU_POPUP_CONFIG, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);

	::TrackPopupMenu(popupMenu, TPM_LEFTALIGN/*|TPM_RIGHTBUTTON*/, pt.x, pt.y, 0, wnd, NULL);

	if (!isEnableConfig)
		::EnableMenuItem(popupMenu, ID_MENU_POPUP_CONFIG, MF_BYCOMMAND|MF_ENABLED);
}

void ContextMenu::CheckHideToTray(bool isCheck)
{
	if (isCheck)
		::CheckMenuItem(mainMenuSetting, ID_MENU_HIDETOTRAY, MF_BYCOMMAND|MF_CHECKED);
	else
		::CheckMenuItem(mainMenuSetting, ID_MENU_HIDETOTRAY, MF_BYCOMMAND|MF_UNCHECKED);
}

void ContextMenu::CheckShuffle(bool isCheck)
{
	if (isCheck)
		::CheckMenuItem(mainMenuPlay, ID_MENU_SHUFFLE, MF_BYCOMMAND|MF_CHECKED);
	else
		::CheckMenuItem(mainMenuPlay, ID_MENU_SHUFFLE, MF_BYCOMMAND|MF_UNCHECKED);
}

void ContextMenu::CheckRepeat(bool isCheck)
{
	if (isCheck)
		::CheckMenuItem(mainMenuPlay, ID_MENU_REPEAT, MF_BYCOMMAND|MF_CHECKED);
	else
		::CheckMenuItem(mainMenuPlay, ID_MENU_REPEAT, MF_BYCOMMAND|MF_UNCHECKED);
}

void ContextMenu::CheckMute(bool isCheck)
{
	if (isCheck)
		::CheckMenuItem(mainMenuPlay, ID_MENU_MUTE, MF_BYCOMMAND|MF_CHECKED);
	else
		::CheckMenuItem(mainMenuPlay, ID_MENU_MUTE, MF_BYCOMMAND|MF_UNCHECKED);
}
/*
void ContextMenu::CheckLastFM(bool isCheck)
{
	if (isCheck)
		::CheckMenuItem(mainMenuSetting, ID_MENU_LASTFM, MF_BYCOMMAND|MF_CHECKED);
	else
		::CheckMenuItem(mainMenuSetting, ID_MENU_LASTFM, MF_BYCOMMAND|MF_UNCHECKED);
}
*/
void ContextMenu::CheckEffect(bool isCheck)
{
	if (isCheck)
		::CheckMenuItem(mainMenuSetting, ID_MENU_EFFECT, MF_BYCOMMAND|MF_CHECKED);
	else
		::CheckMenuItem(mainMenuSetting, ID_MENU_EFFECT, MF_BYCOMMAND|MF_UNCHECKED);
}

void ContextMenu::CheckSmoothScroll(bool isCheck)
{
	if (isCheck)
		::CheckMenuItem(mainMenuSetting, ID_MENU_SMOOTH, MF_BYCOMMAND|MF_CHECKED);
	else
		::CheckMenuItem(mainMenuSetting, ID_MENU_SMOOTH, MF_BYCOMMAND|MF_UNCHECKED);
}

void ContextMenu::CheckPopup(bool isCheck)
{
	if (isCheck)
		::CheckMenuItem(mainMenuSetting, ID_MENU_POPUP, MF_BYCOMMAND|MF_CHECKED);
	else
		::CheckMenuItem(mainMenuSetting, ID_MENU_POPUP, MF_BYCOMMAND|MF_UNCHECKED);
}

void ContextMenu::CheckPopupPosition(int pos)
{
	::CheckMenuItem(popupMenu, ID_MENU_POPUP_TL, MF_BYCOMMAND|MF_UNCHECKED);
	::CheckMenuItem(popupMenu, ID_MENU_POPUP_TR, MF_BYCOMMAND|MF_UNCHECKED);
	::CheckMenuItem(popupMenu, ID_MENU_POPUP_BL, MF_BYCOMMAND|MF_UNCHECKED);
	::CheckMenuItem(popupMenu, ID_MENU_POPUP_BR, MF_BYCOMMAND|MF_UNCHECKED);

	if (pos == 0)
		::CheckMenuItem(popupMenu, ID_MENU_POPUP_TL, MF_BYCOMMAND|MF_CHECKED);
	else if (pos == 1)
		::CheckMenuItem(popupMenu, ID_MENU_POPUP_TR, MF_BYCOMMAND|MF_CHECKED);
	else if (pos == 2)
		::CheckMenuItem(popupMenu, ID_MENU_POPUP_BL, MF_BYCOMMAND|MF_CHECKED);
	else if (pos == 3)
		::CheckMenuItem(popupMenu, ID_MENU_POPUP_BR, MF_BYCOMMAND|MF_CHECKED);
}

void ContextMenu::CheckSearch(int type)
{
	::CheckMenuItem(searchMenu, ID_MENU_SEARCH_ALL, MF_BYCOMMAND|MF_UNCHECKED);
	::CheckMenuItem(searchMenu, ID_MENU_SEARCH_TRACK, MF_BYCOMMAND|MF_UNCHECKED);
	::CheckMenuItem(searchMenu, ID_MENU_SEARCH_ALBUM, MF_BYCOMMAND|MF_UNCHECKED);
	::CheckMenuItem(searchMenu, ID_MENU_SEARCH_ARTIST, MF_BYCOMMAND|MF_UNCHECKED);

	if (type == 0)
		::CheckMenuItem(searchMenu, ID_MENU_SEARCH_ALL, MF_BYCOMMAND|MF_CHECKED);
	else if (type == 1)
		::CheckMenuItem(searchMenu, ID_MENU_SEARCH_TRACK, MF_BYCOMMAND|MF_CHECKED);
	else if (type == 2)
		::CheckMenuItem(searchMenu, ID_MENU_SEARCH_ALBUM, MF_BYCOMMAND|MF_CHECKED);
	else if (type == 3)
		::CheckMenuItem(searchMenu, ID_MENU_SEARCH_ARTIST, MF_BYCOMMAND|MF_CHECKED);
}

void ContextMenu::CheckMiniPlayer(bool isCheck)
{
	if (isCheck)
		::CheckMenuItem(mainMenu, ID_MENU_MINI, MF_BYCOMMAND|MF_CHECKED);
	else
		::CheckMenuItem(mainMenu, ID_MENU_MINI, MF_BYCOMMAND|MF_UNCHECKED);
}

void ContextMenu::EnableMiniPlayer(bool isEnable)
{
	if (isEnable)
		::EnableMenuItem(mainMenu, ID_MENU_MINI, MF_BYCOMMAND|MF_ENABLED);
	else
		::EnableMenuItem(mainMenu, ID_MENU_MINI, MF_BYCOMMAND|MF_DISABLED|MF_GRAYED);
}

void ContextMenu::CheckLyricsAlign(int align)
{
	::CheckMenuItem(lyricsMenuAlign, ID_MENU_LYRICS_ALIGN_LEFT, MF_BYCOMMAND|MF_UNCHECKED);
	::CheckMenuItem(lyricsMenuAlign, ID_MENU_LYRICS_ALIGN_CENTER, MF_BYCOMMAND|MF_UNCHECKED);
	::CheckMenuItem(lyricsMenuAlign, ID_MENU_LYRICS_ALIGN_RIGHT, MF_BYCOMMAND|MF_UNCHECKED);

	if (align == 0)
		::CheckMenuItem(lyricsMenuAlign, ID_MENU_LYRICS_ALIGN_CENTER, MF_BYCOMMAND|MF_CHECKED);
	else if (align == 1)
		::CheckMenuItem(lyricsMenuAlign, ID_MENU_LYRICS_ALIGN_LEFT, MF_BYCOMMAND|MF_CHECKED);
	else if (align == 2)
		::CheckMenuItem(lyricsMenuAlign, ID_MENU_LYRICS_ALIGN_RIGHT, MF_BYCOMMAND|MF_CHECKED);
}

void ContextMenu::CheckLyricsFontSize(int size)
{
	::CheckMenuItem(lyricsMenuFont, ID_MENU_LYRICS_FONT_DEFAULT, MF_BYCOMMAND|MF_UNCHECKED);
	::CheckMenuItem(lyricsMenuFont, ID_MENU_LYRICS_FONT_SMALL, MF_BYCOMMAND|MF_UNCHECKED);
	::CheckMenuItem(lyricsMenuFont, ID_MENU_LYRICS_FONT_MEDIUM, MF_BYCOMMAND|MF_UNCHECKED);
	::CheckMenuItem(lyricsMenuFont, ID_MENU_LYRICS_FONT_LARGE, MF_BYCOMMAND|MF_UNCHECKED);
	::CheckMenuItem(lyricsMenuFont, ID_MENU_LYRICS_FONT_LARGEST, MF_BYCOMMAND|MF_UNCHECKED);

	if (size == 0)
		::CheckMenuItem(lyricsMenuFont, ID_MENU_LYRICS_FONT_DEFAULT, MF_BYCOMMAND|MF_CHECKED);
	else if (size == 1)
		::CheckMenuItem(lyricsMenuFont, ID_MENU_LYRICS_FONT_SMALL, MF_BYCOMMAND|MF_CHECKED);
	else if (size == 2)
		::CheckMenuItem(lyricsMenuFont, ID_MENU_LYRICS_FONT_MEDIUM, MF_BYCOMMAND|MF_CHECKED);
	else if (size == 3)
		::CheckMenuItem(lyricsMenuFont, ID_MENU_LYRICS_FONT_LARGE, MF_BYCOMMAND|MF_CHECKED);
	else if (size == 4)
		::CheckMenuItem(lyricsMenuFont, ID_MENU_LYRICS_FONT_LARGEST, MF_BYCOMMAND|MF_CHECKED);
}

void ContextMenu::CheckLyricsFontBold(bool bold)
{
	if (bold)
		::CheckMenuItem(lyricsMenuFont, ID_MENU_LYRICS_FONT_BOLD, MF_BYCOMMAND|MF_CHECKED);
	else
		::CheckMenuItem(lyricsMenuFont, ID_MENU_LYRICS_FONT_BOLD, MF_BYCOMMAND|MF_UNCHECKED);
}

void ContextMenu::CheckLyricsProvider(int provider)
{
	::CheckMenuItem(lyricsMenuProv, ID_MENU_LYRICS_PROV_OFF, MF_BYCOMMAND|MF_UNCHECKED);
	for (int i = 0; i < LyricsLoader::GetLyricsProviderCount(); i++)
		::CheckMenuItem(lyricsMenuProv, ID_MENU_LYRICS_PROV_1 + i, MF_BYCOMMAND|MF_UNCHECKED);

	if (provider == -1)
		::CheckMenuItem(lyricsMenuProv, ID_MENU_LYRICS_PROV_OFF, MF_BYCOMMAND|MF_CHECKED);
	else if (provider > -1)
		::CheckMenuItem(lyricsMenuProv, ID_MENU_LYRICS_PROV_1 + provider, MF_BYCOMMAND|MF_CHECKED);
}

void ContextMenu::SetLyricsSource(int source)
{
	std::wstring sourceString = lang->GetLine(Lang::LyricsMenu, 0);
	sourceString.push_back(' ');

	if (source == 0)
		sourceString += lang->GetLineS(Lang::LyricsMenu, 1);
	else if (source == 1)
		sourceString += lang->GetLineS(Lang::LyricsMenu, 2);
	else if (source == 2)
		sourceString += lang->GetLineS(Lang::LyricsMenu, 3);
	else if (source == 3)
		sourceString += lang->GetLineS(Lang::LyricsMenu, 4);

	::ModifyMenu(lyricsMenu, ID_MENU_LYRICS_SOURCE, MF_BYCOMMAND|MF_STRING|MF_GRAYED, ID_MENU_LYRICS_SOURCE, sourceString.c_str());
}

void ContextMenu::InitDrawTrayMenuXP(HMENU menu, int itemID)
{
	// http://www.nanoant.com/programming/themed-menus-icons-a-complete-vista-xp-solution

	MENUITEMINFO mii = {};
	mii.cbSize = sizeof(mii);
	mii.fMask = MIIM_BITMAP;
	mii.hbmpItem = HBMMENU_CALLBACK;
	::SetMenuItemInfo(menu, itemID, FALSE, &mii);
}

void ContextMenu::InitDrawTrayMenuVista(HMENU menu, int itemID, int iconID)
{
	// http://www.nanoant.com/programming/themed-menus-icons-a-complete-vista-xp-solution

	int cxIcon = ::GetSystemMetrics(SM_CXSMICON);
	int cyIcon = ::GetSystemMetrics(SM_CYSMICON);

	HICON icon = (HICON)::LoadImageW(::GetModuleHandleW(NULL), MAKEINTRESOURCE(iconID), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);

	HDC dcBitmap = ::CreateCompatibleDC(NULL);

	BITMAPINFO bi32 = {};
	bi32.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi32.bmiHeader.biWidth = cxIcon;
	bi32.bmiHeader.biHeight = cyIcon;
	bi32.bmiHeader.biPlanes = 1;
	bi32.bmiHeader.biBitCount = 32;
	bi32.bmiHeader.biCompression = BI_RGB;

	void* bits = NULL;

	HBITMAP bitmap = CreateDIBSection(NULL, &bi32, DIB_RGB_COLORS, (void**)&bits, NULL, NULL);
	HGDIOBJ oldBitmap = ::SelectObject(dcBitmap, bitmap);

	int x = (cxIcon > 16) ? (cxIcon - 16) / 2 : 0;
	int y = (cyIcon > 16) ? (cyIcon - 16) / 2 : 0;

	::DrawIconEx(dcBitmap, x, y, icon, 16, 16, 0, NULL, DI_NORMAL);
	::DestroyIcon(icon);

	::SelectObject(dcBitmap, oldBitmap);
	::DeleteDC(dcBitmap);
	//::DeleteObject(bitmap); // Don't do this

	MENUITEMINFO mii = {};
	mii.cbSize = sizeof(mii);
	mii.fMask = MIIM_BITMAP;
	mii.hbmpItem = bitmap;
	::SetMenuItemInfo(menu, itemID, FALSE, &mii);
}

void ContextMenu::InitFixDrawTrayMenu(HMENU menu)
{
	MENUINFO menuInfo = {};
	menuInfo.cbSize = sizeof(menuInfo);
	menuInfo.fMask = MIM_STYLE;

	::GetMenuInfo(menu, &menuInfo);

	MENUINFO menuInfoNew = menuInfo;
	menuInfoNew.dwStyle = (menuInfo.dwStyle & ~MNS_NOCHECK) | MNS_CHECKORBMP;
	::SetMenuInfo(menu, &menuInfoNew);
}

bool ContextMenu::MeasureDrawTrayMenuXP(bool isMeasure, LPARAM lParam)
{
	if (isMeasure)
	{
		MEASUREITEMSTRUCT* lpmis = (MEASUREITEMSTRUCT*)lParam;
		if (lpmis && lpmis->CtlType == ODT_MENU)
		{
			if (lpmis->itemHeight < 16)
				lpmis->itemHeight = 16;

			lpmis->itemWidth = lpmis->itemHeight;

			return true;
		}
	}
	else
	{
		DRAWITEMSTRUCT* lpdis = (DRAWITEMSTRUCT*)lParam;
		if (lpdis && lpdis->CtlType == ODT_MENU)
		{
			HINSTANCE instance = ::GetModuleHandleW(NULL);
			HICON icon = NULL;

			if (lpdis->itemID == ID_MENU_PLAY)
				icon = (HICON)::LoadImageW(instance, MAKEINTRESOURCE(IDI_ICON_TRAY_PLAY), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
			else if (lpdis->itemID == ID_MENU_PAUSE)
				icon = (HICON)::LoadImageW(instance, MAKEINTRESOURCE(IDI_ICON_TRAY_PAUSE), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
			else if (lpdis->itemID == ID_MENU_STOP)
				icon = (HICON)::LoadImageW(instance, MAKEINTRESOURCE(IDI_ICON_TRAY_STOP), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
			else if (lpdis->itemID == ID_MENU_NEXT)
				icon = (HICON)::LoadImageW(instance, MAKEINTRESOURCE(IDI_ICON_TRAY_NEXT), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
			else if (lpdis->itemID == ID_MENU_PREV)
				icon = (HICON)::LoadImageW(instance, MAKEINTRESOURCE(IDI_ICON_TRAY_PREV), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
			else if (lpdis->itemID == ID_MENU_EXIT)
				icon = (HICON)::LoadImageW(instance, MAKEINTRESOURCE(IDI_ICON_TRAY_EXIT), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);

			CRect rc = lpdis->rcItem;
			if (rc.Width() > 16)
				rc.left += (rc.Width() - 16) / 2;
			if (rc.Height() > 16)
				rc.top += (rc.Height() - 16) / 2;

			::DrawIconEx(lpdis->hDC, rc.left, rc.top, icon, 16, 16, 0, NULL, DI_NORMAL);
			::DestroyIcon(icon);

			return true;
		}
	}

	return false;
}

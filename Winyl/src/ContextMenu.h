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

#pragma once

// ContextMenu

#include "Language.h"

class ContextMenu
{

public:
	ContextMenu();
	virtual ~ContextMenu();

	void FillMenu();
	void ReloadMenu();
	void DestroyAll();
	void ShowMainMenu(HWND wnd, CPoint& pt);
	void ShowListMenu(HWND wnd, CPoint& pt, const std::vector<std::wstring>& playlists, bool isRadio, bool isPlaylist, int selPlaylist, bool isNowPlaying);
	void ShowTreePlaylistMenu(HWND wnd, CPoint& pt, bool isEnable);
	void ShowTreeSmartlistMenu(HWND wnd, CPoint& pt, bool isEnable);
	void ShowTrayMenu(HWND wnd);
	void ShowPopupMenu(HWND wnd, CPoint& pt, bool isEnableConfig);
	void ShowSearchMenu(HWND wnd, CPoint& pt);
	void ShowLyricsMenu(HWND wnd, CPoint& pt);

	void CheckHideToTray(bool isCheck);
	void CheckShuffle(bool isCheck);
	void CheckRepeat(bool isCheck);
	void CheckMute(bool isCheck);
	//void CheckLastFM(bool isCheck);
	void CheckEffect(bool isCheck);
	void CheckSmoothScroll(bool isCheck);
	void CheckPopup(bool isCheck);
	void CheckPopupPosition(int pos);
	void CheckSearch(int type);
	void CheckMiniPlayer(bool isCheck);
	void CheckLyricsAlign(int align);
	void CheckLyricsFontSize(int size);
	void CheckLyricsFontBold(bool bold);
	void CheckLyricsProvider(int provider);
	void SetLyricsSource(int source);

	void EnableMiniPlayer(bool isEnable);

	inline void SetLanguage(Language* language) {lang = language;}

	void InitDrawTrayMenuXP(HMENU menu, int itemID);
	void InitDrawTrayMenuVista(HMENU menu, int itemID, int iconID);
	void InitFixDrawTrayMenu(HMENU menu);
	bool MeasureDrawTrayMenuXP(bool isMeasure, LPARAM lParam);

private:
	Language* lang = nullptr;

	HMENU mainMenu = NULL;
	HMENU mainMenuPlay = NULL;
	HMENU mainMenuOpen = NULL;
	HMENU mainMenuSetting = NULL;
	HMENU listMenu = NULL;
	HMENU listMenuPlay = NULL;
	HMENU listMenuJump = NULL;
	HMENU treePlayMenu = NULL;
	HMENU treeSmartMenu = NULL;
	HMENU trayMenu = NULL;
	HMENU popupMenu = NULL;
	HMENU searchMenu = NULL;
	HMENU lyricsMenu = NULL;
	HMENU lyricsMenuFont = NULL;
	HMENU lyricsMenuAlign = NULL;
	HMENU lyricsMenuProv = NULL;

	void FillMainMenu();
	void FillTrayMenu();
	void FillListMenu();
	void FillTreePlaylistMenu();
	void FillTreeSmartlistMenu();
	void FillPopupMenu();
	void FillSearchMenu();
	void FillLyricsMenu();
};


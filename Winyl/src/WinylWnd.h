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

#include <vector>
#include <string>
#include "WinylApp.h"
#include "WindowEx.h"
#include "MoveResize.h"
#include "SkinLayout.h"
#include "SkinElement.h"
#include "SkinList.h"
#include "SkinTree.h"
#include "SkinDraw.h"
#include "DlgLibrary.h"
#include "TrayIcon.h"
#include "DBase.h"
#include "LibAudio.h"
#include "SkinVis.h"
#include "ContextMenu.h"
#include "DlgSkin.h"
#include "Language.h"
#include "DlgLanguage.h"
#include "DlgProgress.h"
#include "DlgProperties.h"
#include "DlgRename.h"
#include "Settings.h"
#include "DlgAbout.h"
#include "DlgEqualizer.h"
#include "LastFM.h"
#include "FileDialogEx.h"
#include "HotKeys.h"
#include "DlgHotKeys.h"
#include "DlgOpenURL.h"
#include "PlsFile.h"
#include "SkinPopup.h"
#include "DlgConfig.h"
#include "ToolTips.h"
#include "SkinEdit.h"
#include "SkinMini.h"
#include "SkinAlpha.h"
#include "Radio.h"
#include "Win7TaskBar.h"
#include "DlgSmart.h"
#include "Messengers.h"
#include "MyDropTarget.h"
#include "MyDropSource.h"
#include "MyDataObject.h"
#include "DragIconWnd.h"
#include "MessageBox.h"
#include "Associations.h"
#include "FontsLoader.h"
#include "DlgNewVersion.h"
#include "SkinLyrics.h"
#include "LyricsLoader.h"
#include "HttpClient.h"
#include "SkinShadow.h"

class WinylWnd : public WindowEx
{

public:
	WinylWnd();
	virtual ~WinylWnd();

	void SetPortableProfile(bool isPortable) {isPortableProfile = isPortable;}
	void SetPortableVersion(bool isPortable) {isPortableVersion = isPortable;}
	void SetProgramPath(std::wstring&& path) {programPath = std::move(path);}
	void SetProfilePath(std::wstring&& path) {profilePath = std::move(path);}
	void SetOpenFiles(std::wstring& files) {pointerOpenFiles = &files;}

	bool NewWindow();
	ToolTips toolTips;
	std::unique_ptr<SkinEdit> skinEdit;

	void RunShowWindow();
	void PrepareLibrary();

	void PreTranslateMouseWheel(MSG* msg);
	void PreTranslateRelayEvent(MSG* msg);
	bool isDragDrop = false;
	bool isDragDropOLE = false;
	void StopDragDrop();

	std::wstring ChangeFile(long long& outCue);
	void ChangeNode(bool isError, bool isRadio);

private:
	bool isPortableVersion = false;
	bool isPortableProfile = false;
	std::wstring* pointerOpenFiles = nullptr;

	MoveResize moveResize; // Class to move and resize the main window
	std::unique_ptr<SkinList> skinList; // Playlist window
	std::unique_ptr<SkinTree> skinTree; // Library window
	std::unique_ptr<SkinLyrics> skinLyrics; // Lyrics window
	std::vector<std::unique_ptr<SkinVis>> visuals; // Visualizer windows

	std::unique_ptr<SkinMini> skinMini; // Mini player window
	std::unique_ptr<SkinPopup> skinPopup; // Popup window
	std::unique_ptr<SkinAlpha> skinAlpha; // Skin Alpha window

	std::unique_ptr<SkinShadow> skinShadow; // Skin shadow windows

	bool isWindowIconic = false;

	HICON iconLarge = NULL;
	HICON iconSmall = NULL;

	std::vector<std::wstring> libraryFolders;

	DBase dBase; // Database library
	LibAudio libAudio; // Audio library

	SkinDraw skinDraw; // Skin library
	TrayIcon trayIcon; // Tray icon

	ContextMenu contextMenu; // Menus

	Language lang; // Language
	std::wstring programPath; // Program path
	std::wstring profilePath; // Profile path

	LastFM lastFM; // Last.FM scrobbling
	Settings settings; // Settings

	//CSHOUTcast radio; // Shoutcast radio
	Radio radio; // Radio

	HotKeys hotKeys; // Global hotkeys

	Win7TaskBar win7TaskBar;

	UINT wmTaskbarCreated = 0;
	UINT wmTaskbarButtonCreated = 0;

	bool isMediaPlay = false; // Play state
	bool isMediaPause = false; // Pause state
	bool isMediaRadio = false; // Radio state

	bool isRepeatTrack = false;

	long long idMediaLibrary = 0; // Library ID of the playing track
	long long idMediaPlaylist = 0; // Playlist ID of the playing track
	std::wstring radioString;

	// Cover thread
	Threading::Thread threadCover;
	Threading::Mutex mutexCover;
	std::atomic<bool> isThreadCover = false;

	std::wstring coverFile;
	std::wstring coverPath;
	std::wstring coverAlbum;
	std::wstring coverArtist;
	bool isCoverShowPopup = false;

	bool CoverThread(const std::wstring& file, const std::wstring& album, const std::wstring& artist);
	void CoverThreadStart();
	void CoverThreadRun();
	void CoverThreadDone();
	void SetCoverNone();

	// Lyrics thread
	Threading::Thread threadLyrics;
	Threading::Mutex mutexLyrics;
	std::atomic<bool> isThreadLyrics = false;

	bool isLyricsWindow = false;
	std::wstring lyricsFile, lyricsFileCur;
	std::wstring lyricsTitle, lyricsTitleCur;
	std::wstring lyricsArtist, lyricsArtistCur;
	std::vector<std::wstring> lyricsLines;
	int lyricsSource = 0;

	void LyricsThread(const std::wstring& file, const std::wstring& title, const std::wstring& artist);
	void LyricsThreadStart();
	void LyricsThreadRun();
	void LyricsThreadDone();
	void SetLyricsNone(bool isRadio = false);
	void LyricsThreadReceiving();
	void LyricsThreadReload();

	// Search thread
	Threading::Thread threadSearch;
	std::atomic<bool> isThreadSearch = false;

	void SearchThreadStart();
	void SearchThreadRun();
	void SearchThreadDone();

	// Smooth scrolling thread
	Threading::Thread threadTimerSmooth;
	Threading::Event eventTimerSmooth = false;
	std::atomic<bool> isTimerSmooth = false;

	void TimerEffectsThreadRun();

	bool isTrackTooltip = false;

	struct TimerValue // Consts for timers
	{
		static const int PosID  = 100; // Timer ID for track bar
		static const int Pos    = 500; // Update period for track bar
		static const int VisID  = 200; // Timer ID for visualizer
		static const int Vis    = 30;  // Update period for visualizer
		static const int TimeID = 300; // Timer ID for time line
		static const int Time   = 950; // Update period for time line
		static const int FadeID = 400; // Timer ID for animation
		static const int Track   = 200; // Update period for tracking tooltip
		static const int TrackID = 500; // Timer ID for tracking tooltip
	};

	enum class MouseAction
	{
		None = 0,
		Move = 1,
		Down = 2,
		Up   = 3
	};

	bool ReloadSkin(const std::wstring& skinName, bool isSkinPack);
	bool LoadWindows(bool isReload);
	bool LoadLibraryView(bool isReload = false);
	void EnableAll(bool isEnable);


	void Action(SkinElement* element, MouseAction mouseAction, bool isSkinDraw = false);
	void ActionMinimize();
	void ActionMaximize(bool isMaximize);
	void ActionStop();
	void ActionMute(bool isMute);
	void ActionVolume(int volume, bool isSkinDraw);
	void ActionSetRating(int rating, bool isSkinDraw);
	void ActionVolumeUp();
	void ActionVolumeDown();
	void ActionNextTrack();
	void ActionPrevTrack();
	void ActionPlay(bool isPlayFocused = false, bool isReplay = false, bool isRepeat = false);
	void ActionPause();
	void ActionShuffle(bool isShuffle);
	void ActionRepeat(bool isRepeat);
	void ActionPosition(int position);
	bool PlayNode(ListNodeUnsafe node, bool isRepeat = false, bool isNewNowPlaying = false, bool isReconnect = false);
	void ActionPlayEx();
	void ActionPauseEx();


	void FillTree(TreeNodeUnsafe node);
	void FillList(TreeNodeUnsafe node = nullptr);
	void FillListSearch();
	void FillJump(ListNodeUnsafe node, SkinTreeNode::Type type);

	DragIconWnd wndDragIcon;
	void DropFilesToPlaylist();

	bool SaveSettings();

	void SetWindowCaption(const std::wstring& artist, const std::wstring& title, bool isRadio = false);
	void SetWindowCaptionNull();
	void AddFileFolder(bool isFolder);
	void AddURL();
	void ImportPlaylist();

	void SkinDrawText(const std::wstring& title, const std::wstring& album, const std::wstring& artist,
					const std::wstring& genre, const std::wstring& year, bool isRadio);
	void StartRadio(LibAudio::Error error, bool isReconnect);

	void DialogConfig(int page = 0);
	void DialogLanguage();
	void DialogLibrary();
	void DialogEqualizer();
	void DialogProperties(bool isOpenLyrics = false);
	void PropertiesChanged(Properties *properties);

	void ScanLibraryStart(bool isRemoveMissing, bool isIgnoreDeleted, bool isFindMoved, bool isRescanAll);
	void ScanLibraryFinish(bool isDestroyOnStop);
	std::unique_ptr<DlgProgress> dlgProgressPtr;

	void MiniPlayer(bool isEnable);

	void ShowPopup();

	FontsLoader fontsLoader;

	bool LoadLibraryFolders();
	bool SaveLibraryFolders();

	void NewPlaylist(bool isSmartlist);
	void DeletePlaylist(bool isSmartlist);
	void RenamePlaylist(bool isSmartlist);
	void EditSmartlist();

	void DropNewDefault();
	void DropParseFiles(const std::wstring& file, std::vector<std::wstring>& files);
	void DropAddFiles(const std::vector<std::wstring>& files, bool isPlay, bool isDefaultPlaylist);
	bool DropIsPlaylist(const std::vector<std::wstring>& files);
	void DropAddPlaylist(const std::vector<std::wstring>& files, bool isPlay);

	void UpdateStatusLine();

	bool CopySelectedToClipboard(bool isClipboard);

	inline std::wstring StringCurrentDate()
	{
		SYSTEMTIME st = {};
		::GetLocalTime(&st);

		WCHAR date[48] = {};
		::GetDateFormat(NULL, DATE_SHORTDATE, &st, NULL, date, 48);

		return date;
	}

	bool isRadioWaitCursor = false;
	void EnableWaitRadioCursor(bool isEnableWait)
	{
		isRadioWaitCursor = isEnableWait;
		if (isRadioWaitCursor)
			::SetCursor(::LoadCursorW(NULL, IDC_APPSTARTING));
		else
			::SetCursor(::LoadCursorW(NULL, IDC_ARROW));
	}

private: // Messages
	LRESULT WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
	LRESULT WindowProcEx(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT WindowProcMy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void OnPaint(HDC dc, PAINTSTRUCT& ps);
	void OnTimer(UINT_PTR nIDEvent);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnMouseLeave();
	void OnNcHitTest(CPoint point);
	bool OnSetCursor(HWND hWnd, UINT nHitTest, UINT message);
	void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	void OnSize(UINT nType, int cx, int cy);
	void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnContextMenu(HWND hWnd, CPoint point);
	void OnCommand(WPARAM wParam, LPARAM lParam);
	void OnNotify(WPARAM wParam, LPARAM lParam);
	bool OnSysCommand(WPARAM wParam, LPARAM lParam);
	void OnDestroy();
	void OnDropFiles(HDROP hDropInfo);
	void OnCopyData(HWND hWnd, COPYDATASTRUCT* pCopyDataStruct);
};

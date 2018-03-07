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

// WinylWnd.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "WinylWnd.h"
#include "FileSystem.h"

// WinylWnd

// This class is a mess, need to refactor it, it's doing too many things already.

WinylWnd::WinylWnd()
{

}

WinylWnd::~WinylWnd()
{
	if (iconLarge) ::DestroyIcon(iconLarge);
	if (iconSmall) ::DestroyIcon(iconSmall);

	if (threadTimerSmooth.IsJoinable())
	{
		isTimerSmooth = true;
		eventTimerSmooth.Set();
		threadTimerSmooth.Join();
	}

	if (threadCover.IsJoinable())
	{
		threadCover.Join();
	}

	if (threadLyrics.IsJoinable())
	{
		threadLyrics.Join();
	}

	if (threadSearch.IsJoinable())
	{
		dBase.SetStopSearch(true);
		threadSearch.Join();
		dBase.SetStopSearch(false);
	}
}

LRESULT WinylWnd::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
	case WM_TIMER:
		OnTimer((UINT_PTR)wParam);
		return 0;
	case WM_MOUSEMOVE:
		TRACKMOUSEEVENT tme;
		OnMouseMove((UINT)wParam, CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		tme.cbSize = sizeof(TRACKMOUSEEVENT);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = thisWnd;
		tme.dwHoverTime = 0;
		TrackMouseEvent(&tme);
		return 0;
	case WM_MOUSELEAVE:
		OnMouseLeave();
		return 0;
	case WM_NCHITTEST:
		OnNcHitTest(CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	case WM_SETCURSOR:
		if (OnSetCursor((HWND)wParam, LOWORD(lParam), HIWORD(lParam)))
			return 1;
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	case WM_WINDOWPOSCHANGED:
		OnWindowPosChanged((WINDOWPOS*)lParam);
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	case WM_SIZE:
		OnSize((UINT)wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_GETMINMAXINFO:
		OnGetMinMaxInfo((MINMAXINFO*)lParam);
		return 0;
	case WM_LBUTTONDOWN:
		OnLButtonDown((UINT)wParam, CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		return 0;
	case WM_LBUTTONUP:
		OnLButtonUp((UINT)wParam, CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		return 0;
	case WM_LBUTTONDBLCLK:
		OnLButtonDblClk((UINT)wParam, CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		return 0;
	case WM_CONTEXTMENU:
		OnContextMenu((HWND)wParam, CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		return 0;
	case WM_COMMAND:
		OnCommand(wParam, lParam);
		return 0;
	case WM_NOTIFY:
		OnNotify(wParam, lParam);
		return 0;
	case WM_SYSCOMMAND:
		if (OnSysCommand(wParam, lParam))
			return 0;
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	case WM_DESTROY:
		OnDestroy();
		return 0;
	case WM_DROPFILES:
		OnDropFiles((HDROP)wParam);
		return 0;
	case WM_COPYDATA:
		OnCopyData((HWND)wParam, (COPYDATASTRUCT*)lParam);
		return 1;
	}

	return WindowProcEx(hWnd, message, wParam, lParam);
	//return ::DefWindowProc(hWnd, message, wParam, lParam);
}

bool WinylWnd::NewWindow()
{
	// Load settings
	settings.SetProfilePath(profilePath);
	settings.SetPortableVersion(isPortableVersion);
	settings.LoadSettings();

	// Load language
	lang.SetProgramPath(programPath);
	if (settings.IsFirstRun())
	{
		if (!lang.LoadLanguage(lang.GetSystemLanguage()))
			lang.LoadLanguage(settings.GetDefaultLanguage());
	}
	else
	{
		if (!lang.LoadLanguage(settings.GetLanguage()))
			lang.LoadLanguage(settings.GetDefaultLanguage());
	}

	if (settings.IsDBOldBeta())
	{
		MessageBox::Show(NULL, L"Winyl",
			L"Library database is incompatible.",
			MessageBox::Icon::Error);
		return false;
	}

	if (settings.IsConvertVersion3())
	{
		DlgNewVersion dlg;
		dlg.SetLanguage(&lang);
		dlg.SetProgramPath(programPath);
		dlg.SetProfilePath(profilePath);
		if (dlg.ModalDialog(thisWnd, IDD_DLGNEWVERSION) != IDOK)
		{
			MessageBox::Show(NULL, L"Winyl",
				L"Something bad happened when trying to convert the library.\nPlease contact with the developer.",
				MessageBox::Icon::Error);
			return false;
		}
		settings.SaveSettings();
	}
	if (settings.IsConvertPortable())
	{
		DlgNewVersion dlg;
		dlg.SetLanguage(&lang);
		dlg.SetProgramPath(programPath);
		dlg.SetProfilePath(profilePath);
		dlg.SetConvertPortable(settings.IsConvertPortableTo(), settings.IsConvertPortableFrom());
		dlg.ModalDialog(thisWnd, IDD_DLGNEWVERSION);
		settings.SaveSettings();
	}

	LoadLibraryFolders();

//	LARGE_INTEGER freq; QueryPerformanceFrequency(&freq);
//	LARGE_INTEGER counter1; QueryPerformanceCounter(&counter1);

	std::unique_ptr<ZipFile> zipFile;

	if (settings.IsSkinPack())
		zipFile = skinDraw.NewZipFile(programPath, settings.GetSkin());
	if (!zipFile)
		settings.SetSkinPack(false);

	// Load skin fonts if needed
	fontsLoader.LoadSkinFonts(programPath, settings.GetSkin(), zipFile.get(), true);

	// Load skin
	if (!skinDraw.LoadSkin(programPath, settings.GetSkin(), zipFile.get()))
	{
		fontsLoader.FreeSkinFonts();

		settings.SetSkin(settings.GetDefaultSkin());
		settings.SetSkinPack(settings.IsDefaultSkinPack());

		if (settings.IsSkinPack())
			zipFile = skinDraw.NewZipFile(programPath, settings.GetSkin());
		skinDraw.LoadSkin(programPath, settings.GetSkin(), zipFile.get());
	}

	skinDraw.LoadSkinSettings(profilePath, settings.GetSkin());

//	LARGE_INTEGER counter2; QueryPerformanceCounter(&counter2);
//	double perfDiffMs = (counter2.QuadPart - counter1.QuadPart) *1000.0 / freq.QuadPart;
//	int i = 0;

	// Adjust the main window position and size by the monitor
	settings.FixWinPosMain(skinDraw.GetMinSize(), skinDraw.GetMaxSize());

	// Adjust the main window position and size by Alpha Window if present
	settings.FixWinPosAlphaGet(skinDraw.IsLayeredAlpha(), skinDraw.GetAlphaBorder());

	std::wstring iconFile = programPath + L"Winyl.ico";
	if (futureWin->IsVistaOrLater())
	{
		if (FileSystem::Exists(iconFile))
		{
			int cxIconLarge = ::GetSystemMetrics(SM_CXICON);
			int cyIconLarge = ::GetSystemMetrics(SM_CYICON);

			// Use LoadImage for 32, 48, 64 etc. icons. LoadIconMetric load
			// the wrong icon from the external file in these cases for some reason.
			// In other cases like 125% DPI (40px) LoadIconMetric must be used.
			// It only affects the large icon.
			if (cxIconLarge % 16 == 0)
				iconLarge = (HICON)::LoadImageW(NULL, iconFile.c_str(), IMAGE_ICON, cxIconLarge, cyIconLarge, LR_LOADFROMFILE);
			else
				futureWin->LoadIconMetric(NULL, iconFile.c_str(), LIM_LARGE, &iconLarge);

			futureWin->LoadIconMetric(NULL, iconFile.c_str(), LIM_SMALL, &iconSmall);
		}
		else
		{
			futureWin->LoadIconMetric(::GetModuleHandleW(NULL), MAKEINTRESOURCE(IDR_MAINFRAME), LIM_LARGE, &iconLarge);
			futureWin->LoadIconMetric(::GetModuleHandleW(NULL), MAKEINTRESOURCE(IDR_MAINFRAME), LIM_SMALL, &iconSmall);
		}
	}
	else
	{
		int cxIconLarge = ::GetSystemMetrics(SM_CXICON);
		int cyIconLarge = ::GetSystemMetrics(SM_CYICON);
		int cxIconSmall = ::GetSystemMetrics(SM_CXSMICON);
		int cyIconSmall = ::GetSystemMetrics(SM_CYSMICON);

		if (FileSystem::Exists(iconFile))
		{
			iconLarge = (HICON)::LoadImageW(NULL, iconFile.c_str(), IMAGE_ICON, cxIconLarge, cyIconLarge, LR_LOADFROMFILE);
			iconSmall = (HICON)::LoadImageW(NULL, iconFile.c_str(), IMAGE_ICON, cxIconSmall, cyIconSmall, LR_LOADFROMFILE);
		}
		else
		{
			iconLarge = (HICON)::LoadImageW(::GetModuleHandleW(NULL), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, cxIconLarge, cyIconLarge, LR_DEFAULTCOLOR);
			iconSmall = (HICON)::LoadImageW(::GetModuleHandleW(NULL), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, cxIconSmall, cyIconSmall, LR_DEFAULTCOLOR);
		}
	}

	CreateClassWindow(NULL, L"WinylWnd", WS_OVERLAPPED|WS_SYSMENU|WS_MINIMIZEBOX|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
		WS_EX_APPWINDOW|WS_EX_ACCEPTFILES, settings.GetWinPosition(), L"Winyl", iconLarge, iconSmall, true);

	wmTaskbarCreated = RegisterWindowMessage(L"TaskbarCreated");
	wmTaskbarButtonCreated = RegisterWindowMessage(L"TaskbarButtonCreated");

	skinDraw.SetWindowStyle(thisWnd, thisWnd);
	skinDraw.EnableFade(settings.IsEffect());

	// Set ResizeBorder
	moveResize.SetResizeBorder(skinDraw.GetResizeBorder());

	if (skinDraw.IsLayeredAlpha())
	{
		skinAlpha.reset(new SkinAlpha());
		skinAlpha->SetLibAudio(&libAudio, &isMediaPlay, &isMediaRadio);
		skinAlpha->SetMoveResize(&moveResize);
		skinAlpha->SetContextMenu(&contextMenu);
		skinAlpha->NewWindow(thisWnd);
		skinAlpha->LoadSkin(programPath, settings.GetSkin(), zipFile.get());
		skinAlpha->skinDraw.SetWindowStyle(skinAlpha->Wnd(), thisWnd);
		skinAlpha->skinDraw.EnableFade(settings.IsEffect());
	}
	if (skinAlpha)
		skinDraw.SetSkinDrawAlpha(&skinAlpha->skinDraw);
	else
		skinDraw.SetSkinDrawAlpha(nullptr);

	if (skinDraw.IsShadowLayered())
	{
		skinShadow.reset(new SkinShadow());
		if (skinShadow->LoadSkin(programPath, settings.GetSkin(), zipFile.get()))
			skinShadow->NewShadow(thisWnd);
		else
			skinShadow.reset();
	}

	if (SkinMini::IsSkinFile(programPath, settings.GetSkin(), zipFile.get()))
	{
		skinMini.reset(new SkinMini());
		if (skinMini->LoadSkin(programPath, settings.GetSkin(), zipFile.get()))
		{
			// Adjust the mini player position and size by the monitor
			settings.FixWinPosMini(skinMini->skinDraw.GetMinSize(),
				skinMini->skinDraw.GetMaxSize());

			skinMini->SetZOrder(settings.GetMiniZOrder());
			skinMini->NewWindow(thisWnd, settings.GetWinPositionMini(), iconLarge, iconSmall);
			skinMini->SetContextMenu(&contextMenu);

			skinMini->skinDraw.EnableFade(settings.IsEffect());
			skinMini->SetTransparency(settings.GetMiniTransparency());
		}
		else
			skinMini.reset();
	}
	if (skinMini)
		skinDraw.SetSkinDrawMini(&skinMini->skinDraw);
	else
		skinDraw.SetSkinDrawMini(nullptr);

	// Set skin values
	skinDraw.DrawMute(settings.IsMute());
	if (settings.IsMute())
		skinDraw.DrawVolume(0);
	else
		skinDraw.DrawVolume(settings.GetVolume());

	skinDraw.DrawRepeat(settings.IsRepeat());
	skinDraw.DrawShuffle(settings.IsShuffle());

	skinDraw.DrawMaximize(settings.IsMaximized());

	skinDraw.DrawSearchClear(false);

	// Load popup window
	if (SkinPopup::IsSkinFile(programPath, settings.GetSkin(), zipFile.get()))
	{
		// Load skin for the popup window
		skinPopup.reset(new SkinPopup());
		if (skinPopup->LoadSkin(programPath, settings.GetSkin(), zipFile.get()))
		{
			skinPopup->NewWindow(thisWnd);

			skinPopup->SetPosition(settings.GetPopupPosition());
			skinPopup->SetEffect(settings.GetPopupShowEffect(), settings.GetPopupHideEffect());
			skinPopup->SetDelay(settings.GetPopupHold(), settings.GetPopupShow(), settings.GetPopupHide());
		}
		else
			skinPopup.reset();
	}

	HttpClient::SetProxy(settings.GetProxy(), settings.GetProxyHost(),  settings.GetProxyPort(),
		settings.GetProxyLogin(),  settings.GetProxyPass());

	// Set audio library parameters
	libAudio.SetProgramPath(programPath);
	libAudio.SetProfilePath(profilePath);
	libAudio.SetUserAgent(settings.GetBassUserAgent());
	if (!libAudio.Init(this, settings.GetBassDriver(), settings.GetBassDevice(),
		settings.IsBassBit32(), settings.IsBassSoftMix(), true)) // Init audio library
	{
		settings.SetBassDriver(0);
		settings.SetBassDevice(-1);
		libAudio.Init(this, 0, -1, settings.IsBassBit32(), settings.IsBassSoftMix(), true);
	}
	libAudio.SetNoVolumeEffect(settings.IsBassNoVolume(), settings.IsBassNoEffect());
	libAudio.SetPropertiesWA(settings.IsBassWasapiEvent(), settings.GetBassAsioChannel());
	libAudio.SetProxy(settings.GetProxy(), settings.GetProxyHost(),  settings.GetProxyPort(), 
		 settings.GetProxyLogin(),  settings.GetProxyPass());

	libAudio.SetMute(settings.IsMute());
	libAudio.SetVolume(settings.GetVolume());

	// Set database library parameters
	dBase.SetProgramPath(programPath);
	dBase.SetProfilePath(profilePath);
	dBase.SetPortableVersion(isPortableVersion);
	dBase.OpenLibrary();
	dBase.SetLanguage(&lang);

	hotKeys.SetProfilePath(profilePath);
	hotKeys.LoadHotKeys();
	hotKeys.RegisterHotKeys(thisWnd);

	trayIcon.NewIcon(thisWnd, iconSmall); // Tray icon
	trayIcon.SetHideToTray(settings.IsHideToTray());
	trayIcon.SetMiniPlayer(settings.IsMiniPlayer());

	contextMenu.SetLanguage(&lang);
	contextMenu.FillMenu(); // Menu

	contextMenu.CheckRepeat(settings.IsRepeat());
	contextMenu.CheckShuffle(settings.IsShuffle());
	contextMenu.CheckMute(settings.IsMute());
	contextMenu.CheckHideToTray(settings.IsHideToTray());

	if (!skinMini)
		contextMenu.EnableMiniPlayer(false);

	if (settings.IsLastFM())
		settings.SetLastFM(lastFM.Init());

//	contextMenu.CheckLastFM(settings.IsLastFM());
	contextMenu.CheckPopup(settings.IsPopup());
	contextMenu.CheckEffect(settings.IsEffect());
	contextMenu.CheckSmoothScroll(settings.IsSmoothScroll());

	contextMenu.CheckPopupPosition(settings.GetPopupPosition());

	contextMenu.CheckSearch(settings.GetSearchType());

	contextMenu.CheckLyricsAlign(settings.GetLyricsAlign());
	contextMenu.CheckLyricsFontSize(settings.GetLyricsFontSize());
	contextMenu.CheckLyricsFontBold(settings.GetLyricsFontBold());
	if (settings.IsLyricsProviderOff())
		contextMenu.CheckLyricsProvider(-1);
	else
	{
		if (settings.GetLyricsProvider().empty())
			contextMenu.CheckLyricsProvider(0);
		else
		{
			int prov = LyricsLoader::GetLyricsProviderByURL(settings.GetLyricsProvider());
			if (prov > -1)
				contextMenu.CheckLyricsProvider(prov);
			else
			{
				contextMenu.CheckLyricsProvider(0);
				settings.SetLyricsProvider(L"");
			}
		}
	}

	toolTips.SetLanguage(&lang);
	toolTips.SetWindow(thisWnd, &skinDraw);
	if (settings.IsToolTips())
		toolTips.Create();

	if (skinAlpha)
	{
		skinAlpha->toolTips.SetLanguage(&lang);
		skinAlpha->toolTips.SetWindow(skinAlpha->Wnd(), &skinAlpha->skinDraw);
		if (settings.IsToolTips())
			skinAlpha->toolTips.Create();
	}

	LoadWindows(false); // Load windows

	if (settings.IsSmoothScroll())
		threadTimerSmooth.Start(std::bind(&WinylWnd::TimerEffectsThreadRun, this));

	// [ver 2.1] It's not needed anymore it seems (but I don't have time to test)
	//skinDraw.RefreshWindow(); // Refresh all skin elements

	return true;
}

void WinylWnd::PrepareLibrary()
{
	if (settings.IsFirstRun())
	{
		settings.SaveSettings();

		// Refactoring needed
		EnableAll(false);
		DlgLibrary dlg;
		dlg.SetLanguage(&lang);
		dlg.SetFirstRun(true);
		dlg.pageLibrary.SetPortableVersion(isPortableVersion);
		dlg.pageLibrary.SetProgramPath(programPath);
		dlg.pageLibrary.SetLibraryFolders(&libraryFolders);
		if (dlg.ModalDialog(thisWnd, IDD_DLGLIBRARY) == IDOK)
		{
			SaveLibraryFolders();

			DlgProgress dlgProgress;
			dlgProgress.SetLanguage(&lang);
			dlgProgress.progress.SetDataBase(&dBase);
			dlgProgress.progress.SetPortableVersion(isPortableVersion);
			dlgProgress.progress.SetProgramPath(programPath);
			dlgProgress.progress.SetLibraryFolders(libraryFolders);
			dlgProgress.ModalDialog(thisWnd, IDD_DLGPROGRESS);
		}
		EnableAll(true);

		// Show first artist
		if (skinTree->GetRootNode())
		{
			TreeNodeUnsafe treeNode = skinTree->GetRootNode()->Child();
			if (treeNode)
			{
				treeNode = treeNode->Next();
				if (treeNode)
				{
					FillTree(treeNode);
					skinTree->ExpandNode(treeNode);

					treeNode = treeNode->Child();
					if (treeNode)
					{
						skinTree->SetFocusNode(treeNode);
						FillList(treeNode);
					}
				}
			}
		}
	}
	else if (settings.IsConvertVersion3())
	{
		MessageBox::Show(thisWnd, lang.GetLine(Lang::Message, 2),
			(lang.GetLineS(Lang::NewVersionDialog, 2) + L"\n" + lang.GetLineS(Lang::NewVersionDialog, 3)).c_str());

		ScanLibraryStart(false, true, false, true);
	}
}

void WinylWnd::RunShowWindow()
{
	if (skinMini && settings.IsMiniPlayer())
	{
		if (skinAlpha)
			::ShowWindow(skinAlpha->Wnd(), SW_SHOW);

		skinMini->SetVisible(true);
	}
	else
	{
		skinDraw.RedrawWindow();

		// Show Alpha Window before showing the main window
		if (skinAlpha)
			::ShowWindow(skinAlpha->Wnd(), SW_SHOW);

		if (settings.IsMaximized())
			::ShowWindow(thisWnd, SW_SHOWMAXIMIZED);
		else
			::ShowWindow(thisWnd, SW_SHOW);
	}

	if (!pointerOpenFiles->empty())
	{
		std::vector<std::wstring> files;

		DropParseFiles((*pointerOpenFiles), files);
		bool isImportPlaylist = DropIsPlaylist(files);
		bool isDefaultPlaylist = false;
		if (!isImportPlaylist)
		{
			DropNewDefault();
			isDefaultPlaylist = true;
		}

		::UpdateWindow(thisWnd);

		if (!isImportPlaylist)
			DropAddFiles(files, true, isDefaultPlaylist);
		else
			DropAddPlaylist(files, true);

		pointerOpenFiles->clear();
		pointerOpenFiles->shrink_to_fit();
	}
	else
	{
		// Select the last played track if there is one
		if (settings.GetLastPlayIndex())
		{
			ListNodeUnsafe node = skinList->FindNodeByIndex(settings.GetLastPlayIndex());
			if (node)
			{
				skinList->SetFocusNode(node, false);
				skinList->ScrollToFocusNode();
			}
		}
	}
}

bool WinylWnd::ReloadSkin(const std::wstring& skinName, bool isSkinPack)
{
	// Save skin settings
	skinDraw.SaveSkinSettings(profilePath, settings.GetSkin());

	// Save main window position
	WINDOWPLACEMENT pl;
	::GetWindowPlacement(thisWnd, &pl);

	settings.SetWinPosition(CRect(pl.rcNormalPosition));

	// Adjust the main window position and size by Alpha Window if present
	settings.FixWinPosAlphaSet(skinDraw.IsLayeredAlpha(), skinDraw.GetAlphaBorder());

	// Save mini player position
	if (skinMini)
	{
		WINDOWPLACEMENT pl;
		::GetWindowPlacement(skinMini->Wnd(), &pl);
		settings.SetWinPositionMini(CRect(pl.rcNormalPosition));
	}

	bool result = true; // To check that the skin is loaded correctly
	settings.SetSkin(skinName);

	if (threadSearch.IsJoinable())
	{
		dBase.SetStopSearch(true);
		threadSearch.Join();
		dBase.SetStopSearch(false);
	}

	// Destroy Alpha Window
	if (skinAlpha) skinAlpha.reset();

	// Destroy shadow windows
	if (skinShadow) skinShadow.reset();

	// Destroy all visualizer windows
	visuals.clear();

	// Destroy all other windows
	skinTree.reset();
	skinList.reset();
	skinEdit.reset();
	skinLyrics.reset();

	// Destroy tooltips
	toolTips.Destroy();

	/*{
		WINDOWPLACEMENT wp;
		//wp.length = sizeof(WINDOWPLACEMENT);

		GetWindowPlacement(&wp);
		wp.showCmd = SW_MINIMIZE;
		SetWindowPlacement(&wp);
	}*/

	// Hide the main window to do not see the skin rebuilding
	::ShowWindow(thisWnd, SW_HIDE);

	settings.SetSkinPack(isSkinPack);

	std::unique_ptr<ZipFile> zipFile;

	if (settings.IsSkinPack())
		zipFile = skinDraw.NewZipFile(programPath, settings.GetSkin());
	if (!zipFile)
		settings.SetSkinPack(false);

	// Load new skin fonts if needed
	fontsLoader.LoadSkinFonts(programPath, settings.GetSkin(), zipFile.get(), true);

	// Load new skin
	if (!skinDraw.LoadSkin(programPath, settings.GetSkin(), zipFile.get()))
	{
		fontsLoader.FreeSkinFonts();

		settings.SetSkin(settings.GetDefaultSkin());
		settings.SetSkinPack(settings.IsDefaultSkinPack());

		if (settings.IsSkinPack())
			zipFile = skinDraw.NewZipFile(programPath, settings.GetSkin());
		skinDraw.LoadSkin(programPath, settings.GetSkin(), zipFile.get());

		result = false;
	}

	// Load new skin settings
	skinDraw.LoadSkinSettings(profilePath, settings.GetSkin());

	// Adjust the main window position and size by the monitor
	settings.FixWinPosMain(skinDraw.GetMinSize(), skinDraw.GetMaxSize());

	// Set ResizeBorder
	moveResize.SetResizeBorder(skinDraw.GetResizeBorder());
	moveResize.Resize(settings.GetWinPosition().Width(), settings.GetWinPosition().Height());

	skinDraw.SetWindowStyle(thisWnd, thisWnd);
	skinDraw.EnableFade(settings.IsEffect());

	// Load Alpha Window
	if (skinDraw.IsLayeredAlpha())
	{
		skinAlpha.reset(new SkinAlpha());
		skinAlpha->SetLibAudio(&libAudio, &isMediaPlay, &isMediaRadio);
		skinAlpha->SetMoveResize(&moveResize);
		skinAlpha->SetContextMenu(&contextMenu);
		skinAlpha->NewWindow(thisWnd);
		skinAlpha->LoadSkin(programPath, settings.GetSkin(), zipFile.get());
		skinAlpha->skinDraw.SetWindowStyle(skinAlpha->Wnd(), thisWnd);
		skinAlpha->skinDraw.EnableFade(settings.IsEffect());
	}
	if (skinAlpha)
		skinDraw.SetSkinDrawAlpha(&skinAlpha->skinDraw);
	else
		skinDraw.SetSkinDrawAlpha(nullptr);

	// Load shadow windows
	if (skinDraw.IsShadowLayered())
	{
		skinShadow.reset(new SkinShadow());
		if (skinShadow->LoadSkin(programPath, settings.GetSkin(), zipFile.get()))
			skinShadow->NewShadow(thisWnd);
		else
			skinShadow.reset();
	}


	// Load mini player
	skinMini.reset();

	if (SkinMini::IsSkinFile(programPath, settings.GetSkin(), zipFile.get()))
	{
		// Load skin for the mini player
		skinMini.reset(new SkinMini());
		if (skinMini->LoadSkin(programPath, settings.GetSkin(), zipFile.get()))
		{
			// Adjust the mini player position and size by the monitor
			settings.FixWinPosMini(skinMini->skinDraw.GetMinSize(),
				skinMini->skinDraw.GetMaxSize());

			skinMini->SetZOrder(settings.GetMiniZOrder());
			skinMini->NewWindow(thisWnd, settings.GetWinPositionMini(), iconLarge, iconSmall);
			skinMini->SetContextMenu(&contextMenu);

			skinMini->skinDraw.EnableFade(settings.IsEffect());
			skinMini->SetTransparency(settings.GetMiniTransparency());
		}
		else
			skinMini.reset();
	}
	if (skinMini)
		skinDraw.SetSkinDrawMini(&skinMini->skinDraw);
	else
		skinDraw.SetSkinDrawMini(nullptr);

	settings.SetMaximized(false);

	// Set skin values
	skinDraw.DrawMute(settings.IsMute());
	if (settings.IsMute())
		skinDraw.DrawVolume(0);
	else
		skinDraw.DrawVolume(settings.GetVolume());

	skinDraw.DrawRepeat(settings.IsRepeat());
	skinDraw.DrawShuffle(settings.IsShuffle());

	skinDraw.DrawSearchClear(false);

	skinDraw.DrawMaximize(settings.IsMaximized());


	// Load popup window
	skinPopup.reset();

	if (SkinPopup::IsSkinFile(programPath, settings.GetSkin(), zipFile.get()))
	{
		// Load skin for the popup window
		skinPopup.reset(new SkinPopup());
		if (skinPopup->LoadSkin(programPath, settings.GetSkin(), zipFile.get()))
		{
			skinPopup->NewWindow(thisWnd);

			skinPopup->SetPosition(settings.GetPopupPosition());
			skinPopup->SetEffect(settings.GetPopupShowEffect(), settings.GetPopupHideEffect());
			skinPopup->SetDelay(settings.GetPopupHold(), settings.GetPopupShow(), settings.GetPopupHide());
		}
		else
			skinPopup.reset();
	}

	// Set tooltips
	toolTips.SetWindow(thisWnd, &skinDraw);
	if (settings.IsToolTips())
		toolTips.Create();

	if (skinAlpha)
	{
		skinAlpha->toolTips.SetLanguage(&lang);
		skinAlpha->toolTips.SetWindow(skinAlpha->Wnd(), &skinAlpha->skinDraw);
		if (settings.IsToolTips())
			skinAlpha->toolTips.Create();
	}

	// Reset Now Playing
	if (settings.GetNowPlayingType() > 0)
	{
		dBase.CloseNowPlaying();
		settings.EmptyNowPlaying();
	}

	// Load windows
	LoadWindows(true);

	// Adjust the main window position and size by Alpha Window if present
	settings.FixWinPosAlphaGet(skinDraw.IsLayeredAlpha(), skinDraw.GetAlphaBorder());

	// Set the main window position and size (MoveWindow calls WM_GETMINMAXINFO)
	::MoveWindow(thisWnd, settings.GetWinPosition().left, settings.GetWinPosition().top,
		settings.GetWinPosition().Width(), settings.GetWinPosition().Height(), TRUE);

	settings.SetMiniPlayer(false);
	contextMenu.CheckMiniPlayer(false);
	trayIcon.SetMiniPlayer(false);
		
	if (skinMini)
		contextMenu.EnableMiniPlayer(true);
	else
		contextMenu.EnableMiniPlayer(false);

	// Refresh and redraw all skin elements
	skinDraw.RefreshWindow();
	skinDraw.RedrawWindow();

	// SW_RESTORE instead of SW_SHOW to restore the main window if the mini player was enabled
	::ShowWindow(thisWnd, SW_RESTORE);

	// Show Alpha Window after showing the main window
	if (skinAlpha)
	{
		::ShowWindow(skinAlpha->Wnd(), SW_SHOW);

		EnableAll(false);
	}

	// Show shadow windows
	if (skinShadow)
		skinShadow->Show(true);

	return result;
}

bool WinylWnd::LoadWindows(bool isReload)
{
	for (std::size_t i = 0, isize = skinDraw.Layouts().size(); i < isize; ++i)
	{
		for (std::size_t j = 0, jsize = skinDraw.Layouts()[i]->elements.size(); j < jsize; ++j)
		{
			SkinElement* element = skinDraw.Layouts()[i]->elements[j]->element.get();

			if (element->type == SkinElement::Type::Library)
			{
				if (skinTree) continue;
				
				skinTree.reset(new SkinTree());
				skinTree->NewWindow(thisWnd);
				skinTree->LoadSkin(element->skinName, element->zipFile);
				skinTree->SetEventSmoothScroll(&eventTimerSmooth);
				
				element->SetWindow(skinTree->Wnd());
			}
			else if (element->type == SkinElement::Type::Playlist)
			{
				if (skinList) continue;

				skinList.reset(new SkinList());
				skinList->NewWindow(thisWnd);
				skinList->LoadSkin(element->skinName, element->zipFile);
				skinList->SetEventSmoothScroll(&eventTimerSmooth);
				
				element->SetWindow(skinList->Wnd());
			}
			else if (element->type == SkinElement::Type::Lyrics)
			{
				if (skinLyrics) continue;

				isLyricsWindow = true;

				skinLyrics.reset(new SkinLyrics());
				skinLyrics->SetBmBack(skinDraw.GetBmBack());
				skinLyrics->NewWindow(thisWnd);
				skinLyrics->LoadSkin(element->skinName, element->zipFile);
				skinLyrics->SetEventSmoothScroll(&eventTimerSmooth);

				element->SetWindow(skinLyrics->Wnd());
			}
			else if (element->type == SkinElement::Type::Search)
			{
				if (skinEdit) continue;

				skinEdit.reset(new SkinEdit());
				skinEdit->NewWindow(thisWnd);
				skinEdit->LoadSkin(element->skinName, element->zipFile);
				
				element->SetWindow(skinEdit->Wnd());
			}
			else if (element->type == SkinElement::Type::Spectrum)
			{
				visuals.emplace_back(new SkinVis());
				
				visuals.back()->NewWindow(thisWnd);
				visuals.back()->LoadSkin(element->skinName, element->zipFile);
				
				element->SetWindow(visuals.back()->Wnd());
			}
		}
	}

	if (!skinTree)
	{
		skinTree.reset(new SkinTree());
		skinTree->NewWindow(thisWnd);
		skinTree->SetEventSmoothScroll(&eventTimerSmooth);
	}
	if (!skinList)
	{
		skinList.reset(new SkinList());
		skinList->NewWindow(thisWnd);
		skinList->SetEventSmoothScroll(&eventTimerSmooth);
	}
	if (!skinEdit)
	{
		skinEdit.reset(new SkinEdit());
		skinEdit->NewWindow(thisWnd);
	}
	if (!skinLyrics)
	{
		isLyricsWindow = false;
		skinLyrics.reset(new SkinLyrics());
		skinLyrics->NewWindow(thisWnd);
		skinLyrics->SetEventSmoothScroll(&eventTimerSmooth);
	}

	skinEdit->SetFocusWnd(skinList->Wnd());

	skinList->SetNoItemsString(lang.GetLineS(Lang::Playlist, 4));
	skinList->EnablePlayFocus(settings.IsPlayFocus());
	skinList->EnableSmoothScroll(settings.IsSmoothScroll());
	skinTree->EnableSmoothScroll(settings.IsSmoothScroll());
	skinLyrics->EnableSmoothScroll(settings.IsSmoothScroll());
	skinLyrics->SetAlign(settings.GetLyricsAlign());
	skinLyrics->SetFontSize(settings.GetLyricsFontSize(), settings.GetLyricsFontBold());
	skinLyrics->RegisterCallbackShowWindow(std::bind(&WinylWnd::LyricsThreadReload, this));
	skinLyrics->SetLanguageNoLyrics(lang.GetLineS(Lang::Lyrics, 0));
	skinLyrics->SetLanguageNotFound(lang.GetLineS(Lang::Lyrics, 1));
	skinLyrics->SetLanguageReceiving(lang.GetLineS(Lang::Lyrics, 2));


	if (settings.GetSearchType() == 0)
		skinEdit->SetStandartText(lang.GetLineS(Lang::Search, 0));
	else if (settings.GetSearchType() == 1)
		skinEdit->SetStandartText(lang.GetLineS(Lang::Search, 2));
	else if (settings.GetSearchType() == 2)
		skinEdit->SetStandartText(lang.GetLineS(Lang::Search, 3));
	else if (settings.GetSearchType() == 3)
		skinEdit->SetStandartText(lang.GetLineS(Lang::Search, 4));

	if (settings.GetLibraryType())
	{
		if (pointerOpenFiles->empty())
			FillList();
	}

	LoadLibraryView();

	return true;
}

bool WinylWnd::LoadLibraryView(bool isReload)
{
	skinTree->SetControlRedraw(false);

	if (isReload)
		skinTree->DeleteAllNode();

	if (settings.IsLibraryNowPlaying())
		skinTree->InsertHead(nullptr, lang.GetLineS(Lang::Library, 2), SkinTreeNode::Type::NowPlaying, false);
	if (settings.IsLibraryArtists())
		skinTree->InsertHead(nullptr, lang.GetLineS(Lang::Library, 5), SkinTreeNode::Type::Artist);
	if (settings.IsLibraryComposers())
		skinTree->InsertHead(nullptr, lang.GetLineS(Lang::Library, 6), SkinTreeNode::Type::Composer);
	if (settings.IsLibraryAlbums())
		skinTree->InsertHead(nullptr, lang.GetLineS(Lang::Library, 7), SkinTreeNode::Type::Album);
	if (settings.IsLibraryGenres())
		skinTree->InsertHead(nullptr, lang.GetLineS(Lang::Library, 8), SkinTreeNode::Type::Genre);
	if (settings.IsLibraryYears())
		skinTree->InsertHead(nullptr, lang.GetLineS(Lang::Library, 9), SkinTreeNode::Type::Year);
	if (settings.IsLibraryFolders())
		skinTree->InsertHead(nullptr, lang.GetLineS(Lang::Library, 11), SkinTreeNode::Type::Folder);
	if (settings.IsLibraryRadios())
		skinTree->InsertHead(nullptr, lang.GetLineS(Lang::Library, 10), SkinTreeNode::Type::Radio);
	if (settings.IsLibrarySmartlists())
		skinTree->InsertHead(nullptr, lang.GetLineS(Lang::Library, 4), SkinTreeNode::Type::Smartlist, false, false);
//	if (settings.IsLibraryPlaylists())
		skinTree->InsertHead(nullptr, lang.GetLineS(Lang::Library, 3), SkinTreeNode::Type::Playlist, false, true);

	if (skinTree->GetPlaylistNode())
	{
		if (!dBase.LoadPlaylist(skinTree.get()))
			dBase.CreatePlaylist(skinTree.get(), lang.GetLineS(Lang::DefaultList, 3));
	}
	if (skinTree->GetSmartlistNode())
	{
		if (!dBase.LoadTreeSmartlists(skinTree.get()))
			dBase.CreateMySmartlists(skinTree.get());
	}

	skinTree->SetControlRedraw(true);

	return true;
}

void WinylWnd::OnPaint(HDC dc, PAINTSTRUCT& ps)
{
	skinDraw.Paint(dc, ps);
}

void WinylWnd::OnSize(UINT nType, int cx, int cy)
{
	if (nType == SIZE_MINIMIZED)
	{
		trayIcon.SizeMinimized(thisWnd);
		skinDraw.Minimized();
	}
	else if (nType == SIZE_RESTORED || nType == SIZE_MAXIMIZED) // Also when resized
	{
		//MessageBeep(1);
		if (!skinAlpha)
			moveResize.Resize(cx, cy);
		skinDraw.Resize(cx, cy, true);

		if (skinShadow)
			skinShadow->Show(nType != SIZE_MAXIMIZED);
	}
}

void WinylWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	if (skinDraw.IsSplitterPress())
		skinDraw.SplitterLayout(skinDraw.GetPressElement(), point.x, point.y);
	else
	{
		SkinElement* element = skinDraw.MouseMove(nFlags, point);

		if (element)
			Action(element, MouseAction::Move);

		// Tracking tooltip
		if (element && element->type == SkinElement::Type::Volume)
		{
			int thumb = static_cast<SkinSlider*>(element)->GetThumbPosition();
			std::wstring percent = std::to_wstring(element->GetParam() / 1000);
			toolTips.TrackingToolTip(false, percent, element->rcRect.left + thumb, element->rcRect.top);
		}
		else if (element && element->type == SkinElement::Type::Track && isMediaPlay && !isMediaRadio)
		{
			int thumb = static_cast<SkinSlider*>(element)->GetThumbPosition();
			int pos = (element->GetParam() * libAudio.GetTimeLength() + 100000 / 2) / 100000;
			std::wstring time = SkinText::TimeString(pos, false, libAudio.GetTimeLength(), true);
			toolTips.TrackingToolTip(false, time, element->rcRect.left + thumb, element->rcRect.top, true);
		}
		else if (!skinDraw.IsPressElement())
		{
			if (skinDraw.GetHoverElement() && skinDraw.GetHoverElement()->type == SkinElement::Type::Track && isMediaPlay && !isMediaRadio)
			{
				if (isTrackTooltip)
				{
					// https://msdn.microsoft.com/en-us/library/windows/desktop/hh298405%28v=vs.85%29.aspx
					// Make sure the mouse has actually moved. The presence of the tooltip 
					// causes Windows to send the message continuously.
					static POINT oldPoint = {};
					if (point != oldPoint)
					{
						oldPoint = point;
						int percent = static_cast<SkinSlider*>(skinDraw.GetHoverElement())->CalcPercent(point);
						int pos = (percent * libAudio.GetTimeLength() + 100000 / 2) / 100000;
						std::wstring time = SkinText::TimeString(pos, false, libAudio.GetTimeLength(), true);
						toolTips.TrackingToolTip(false, time, point.x, skinDraw.GetHoverElement()->rcRect.top, true);
					}
				}
				else
					::SetTimer(thisWnd, TimerValue::TrackID, TimerValue::Track, NULL);
			}
			else if (isTrackTooltip)
			{
				isTrackTooltip = false;
				toolTips.DeactivateTrackingToolTip();
			}
		}
	}
}

void WinylWnd::OnMouseLeave()
{
	moveResize.MouseLeave();
	skinDraw.MouseLeave();
	if (isTrackTooltip)
	{
		isTrackTooltip = false;
		toolTips.DeactivateTrackingToolTip();
	}
}

void WinylWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (skinEdit->IsFocus())
		::SetFocus(thisWnd);

	SkinElement* element = skinDraw.MouseDown(nFlags, point);

	if (skinDraw.IsSplitterPress())
	{
		::SetCapture(thisWnd);
		skinDraw.SplitterLayoutClick(skinDraw.GetPressElement(), point.x, point.y);
	}
	else if (skinDraw.IsPressElement())
	{
		::SetCapture(thisWnd);
		if (element)
			Action(element, MouseAction::Down);

		if (element && element->type == SkinElement::Type::Volume)
		{
			int thumb = static_cast<SkinSlider*>(element)->GetThumbPosition();
			std::wstring percent = std::to_wstring(element->GetParam() / 1000);
			toolTips.TrackingToolTip(true, percent, element->rcRect.left + thumb, element->rcRect.top);
		}
		else if (element && element->type == SkinElement::Type::Track && isMediaPlay && !isMediaRadio)
		{
			int thumb = static_cast<SkinSlider*>(element)->GetThumbPosition();
			int pos = (element->GetParam() * libAudio.GetTimeLength() + 100000 / 2) / 100000;
			std::wstring time = SkinText::TimeString(pos, false, libAudio.GetTimeLength(), true);
			toolTips.TrackingToolTip(true, time, element->rcRect.left + thumb, element->rcRect.top, true);
		}
	}
	else // Move window
	{
		skinDraw.EmptyClick();

		if (skinAlpha)
			moveResize.MouseDown(thisWnd, point, true);
		else
			moveResize.MouseDown(thisWnd, point, skinDraw.IsStyleBorder());
	}
}

void WinylWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();

	SkinElement* element = skinDraw.MouseUp(nFlags, point);

	if (element)
		Action(element, MouseAction::Up);

	if (element && element->type == SkinElement::Type::Volume)
		toolTips.DeactivateTrackingToolTip();
	else if (element && element->type == SkinElement::Type::Track && !isTrackTooltip)
		toolTips.DeactivateTrackingToolTip();
}

void WinylWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (!skinDraw.IsClickElement())
		MiniPlayer(true);
}

void WinylWnd::OnNcHitTest(CPoint point)
{
	CPoint pt = point;
	::ScreenToClient(thisWnd, &pt);

	if (skinAlpha)
		moveResize.MouseMove(thisWnd, pt, true);
	else if (skinDraw.IsHoverElement())
		moveResize.MouseMove(thisWnd, pt, true);
	else
		moveResize.MouseMove(thisWnd, pt, skinDraw.IsStyleBorder());
}

bool WinylWnd::OnSetCursor(HWND hWnd, UINT nHitTest, UINT message)
{
//	if (message == 0) // When open a menu set default cursor
//	{
//		::SetCursor(::LoadCursorW(NULL, IDC_ARROW));
//		return true;
//	}
	if (message == 0) // When open a menu set default cursor
		return false;

	if (isRadioWaitCursor && ::GetForegroundWindow() == thisWnd)
	{
		::SetCursor(::LoadCursorW(NULL, IDC_APPSTARTING));
		return true;
	}

	if (hWnd == thisWnd && skinDraw.IsSplitterHover())
	{
		assert(skinDraw.GetHoverElement()->GetType() == SkinElement::Type::Splitter);

		if (skinDraw.GetHoverElement()->GetType() == SkinElement::Type::Splitter)
		{
			if (static_cast<SkinSplitter*>(skinDraw.GetHoverElement())->IsHorizontal())
				::SetCursor(::LoadCursorW(NULL, IDC_SIZEWE));
			else
				::SetCursor(::LoadCursorW(NULL, IDC_SIZENS));

			return true;
		}
	}

//	if (pWnd == this && skinDraw.GetHoverElement() &&
//		(skinDraw.GetHoverElement()->iType == SkinElement::TYPE_VOLUME ||
//		skinDraw.GetHoverElement()->iType == SkinElement::TYPE_TRACK))
//	{
//		::SetCursor(::LoadCursorW(NULL, IDC_HAND));
//		return true;
//	}

	// hWnd == thisWnd is needed because sometimes a wrong cursor is set
	// if mouse is not over the main window and over playlist/library for example
	if (hWnd == thisWnd && moveResize.SetCursor() && !skinDraw.IsHoverElement())
		return true;

	return false;
}

void WinylWnd::OnDestroy()
{
	::KillTimer(thisWnd, TimerValue::PosID);
	::KillTimer(thisWnd, TimerValue::TimeID);
	if (!visuals.empty())
		::KillTimer(thisWnd, TimerValue::VisID);

	trayIcon.DeleteIcon(thisWnd);

	if (settings.IsLastFM())
		lastFM.Stop();
	//if (settings.IsMSN())
	//	Messengers::MSNStop();

	settings.SetLastPlayIndex(0);

	if (settings.GetNowPlayingType() > 0)
	{
		settings.ReturnNowPlaying();

		if (skinList->GetLastPlayNode())
		{
			if (skinList->GetLastPlayNode()->idPlaylist)
				settings.SetLastPlayIndex(skinList->GetLastPlayNode()->idPlaylist);
			else if (skinList->GetLastPlayNode()->idLibrary)
				settings.SetLastPlayIndex(skinList->GetLastPlayNode()->idLibrary);
		}
	}

	SaveSettings();

	PostQuitMessage(0);
}

bool WinylWnd::SaveSettings()
{
	settings.SetProfilePath(profilePath);

	// Use GetWindowPlacement instead of GetWindowRect or if minimized coords will be negative
	WINDOWPLACEMENT pl;
	GetWindowPlacement(thisWnd, &pl);
	settings.SetWinPosition(CRect(pl.rcNormalPosition));

	// Adjust the main window position and size by Alpha Window if present
	settings.FixWinPosAlphaSet(skinDraw.IsLayeredAlpha(), skinDraw.GetAlphaBorder());

	if (::IsZoomed(thisWnd))
		settings.SetMaximized(true);
	else
		settings.SetMaximized(false);

	if (skinMini)
	{
		//WINDOWPLACEMENT pl;
		//::GetWindowPlacement(skinMini->Wnd(), &pl);
		//settings.SetWinPositionMini(CRect(pl.rcNormalPosition));
		settings.SetWinPositionMini(skinMini->rcValidRect);
	}

	settings.SetLanguage(lang.GetLanguage());

	//settings.SetVolume(libAudio.GetVolume());
	//settings.SetMute(libAudio.GetMute());

	skinDraw.SaveSkinSettings(profilePath, settings.GetSkin());

	return settings.SaveSettings();
}

void WinylWnd::Action(SkinElement* element, MouseAction mouseAction, bool isSkinDraw)
{
	SkinElement::Type type = element->type;
	int param = element->GetParam();

	switch (type)
	{
	case SkinElement::Type::Close:
		if (dlgProgressPtr)
			dlgProgressPtr->DestroyOnStop();
		else
			::DestroyWindow(thisWnd);
		break;

	case SkinElement::Type::Minimize:
		ActionMinimize();
		break;

	case SkinElement::Type::Maximize:
		ActionMaximize(!settings.IsMaximized());
		break;

	case SkinElement::Type::Play:
		ActionPlay(false, true);
		break;

	case SkinElement::Type::Stop:
		ActionStop();
		break;

	case SkinElement::Type::Track:
		if (mouseAction == MouseAction::Up)
			ActionPosition(param);

		if (isMediaPlay && !isMediaRadio)
		{
			if (mouseAction == MouseAction::Down)
				::KillTimer(thisWnd, TimerValue::TimeID);
			else if (mouseAction == MouseAction::Up)
				::SetTimer(thisWnd, TimerValue::TimeID, TimerValue::Time, NULL);

			int timeLength = libAudio.GetTimeLength();
			if (mouseAction == MouseAction::Down || mouseAction == MouseAction::Move)
				skinDraw.DrawTime((param * timeLength + 100000 / 2) / 100000, timeLength, true);
			else
				skinDraw.DrawTime((param * timeLength + 100000 / 2) / 100000, timeLength, false);
		}
		break;

	case SkinElement::Type::Volume:
		ActionVolume(param, /*false*/isSkinDraw);
		if (skinMini && !isSkinDraw)
			skinMini->skinDraw.DrawVolume(param);
		break;

	case SkinElement::Type::Rating:
		ActionSetRating(param, /*false*/isSkinDraw);
		if (skinMini && !isSkinDraw)
			skinMini->skinDraw.DrawRating(param);
		break;

	case SkinElement::Type::PlayPause:
		ActionPauseEx();
		break;

	case SkinElement::Type::Mute:
		if (param == 1)
			ActionMute(false);
		else
			ActionMute(true);
		break;

	case SkinElement::Type::Repeat:
		if (param == 1)
			ActionRepeat(false);
		else
			ActionRepeat(true);
		break;

	case SkinElement::Type::Shuffle:
		if (param == 1)
			ActionShuffle(false);
		else
			ActionShuffle(true);
		break;

	case SkinElement::Type::MiniPlayer:
		MiniPlayer(true);
		break;

	case SkinElement::Type::Button:
		skinDraw.ChangeTrigger(&element->skinTrigger, 0);
		break;

	case SkinElement::Type::Switch:
		skinDraw.ChangeTrigger(&element->skinTrigger, !param);
		skinDraw.DrawTriggerSwitch(element, !param);
		break;

	case SkinElement::Type::Next:
		ActionNextTrack();
		break;

	case SkinElement::Type::Prev:
		ActionPrevTrack();
		break;

	case SkinElement::Type::SearchClear:
		if (!skinEdit->IsSearchEmpty())
		{
			skinEdit->SearchClear();
			skinDraw.DrawSearchClear(false);
			FillListSearch();
		}
		break;

	case SkinElement::Type::SearchMenu:
	{
		CPoint pt(element->GetRect().left, element->GetRect().bottom);
		::ClientToScreen(thisWnd, &pt);
		contextMenu.ShowSearchMenu(thisWnd, pt);
	}
	break;

	case SkinElement::Type::MainMenu:
	{
		CPoint pt(element->GetRect().left, element->GetRect().bottom);
		if (isSkinDraw && skinAlpha)
			::ClientToScreen(skinAlpha->Wnd(), &pt);
		else
			::ClientToScreen(thisWnd, &pt);
		contextMenu.ShowMainMenu(thisWnd, pt);
	}
	break;

	case SkinElement::Type::Equalizer:
		DialogEqualizer();
		break;
	}
}

void WinylWnd::ActionMinimize()
{
	skinDraw.MouseLeave();
	isWindowIconic = true;
	trayIcon.Minimize(thisWnd, skinDraw.IsLayeredAlpha());
}

void WinylWnd::ActionMaximize(bool isMaximize)
{
	settings.SetMaximized(isMaximize);
	skinDraw.DrawMaximize(isMaximize);

	isWindowIconic = false;

	if (isMaximize)
		trayIcon.Maximize(thisWnd, skinDraw.IsLayeredAlpha());
	else
		trayIcon.Restore(thisWnd, skinDraw.IsLayeredAlpha());
}

void WinylWnd::ActionStop()
{ // Stop the playback
	if (isMediaPlay == false)
		return;

	isMediaPlay = false;
	isMediaPause = false;
	isMediaRadio = false;

	libAudio.Stop();

	::KillTimer(thisWnd, TimerValue::PosID);
	::KillTimer(thisWnd, TimerValue::TimeID);
	if (!visuals.empty())
		::SetTimer(thisWnd, TimerValue::VisID, TimerValue::Vis, NULL);

	skinList->RemoveShuffle();
	if (skinList->GetPlayNode())
		skinList->SetPlayNode(nullptr);

	skinDraw.DrawPosition(0);
	skinDraw.DrawPlay(false);
	skinDraw.DrawRating(-1);
	skinDraw.DrawTextNone();
	SetLyricsNone();
	SetCoverNone();

	if (!settings.IsLibraryNowPlaying())
		dBase.CloseNowPlaying();
	idMediaLibrary = 0;
	idMediaPlaylist = 0;

	SetWindowCaptionNull();

	win7TaskBar.UpdateButtons(thisWnd, false);

	if (settings.IsLastFM())
		lastFM.Stop();
	//if (settings.IsMSN())
	//	Messengers::MSNStop();
}

void WinylWnd::ActionPlayEx()
{ // "Smart" Play
	// Same as Play but if paused then resume instead of play from the start
	if (isMediaPlay && isMediaPause)
		ActionPause();
	else
		ActionPlay(false, true);
}

void WinylWnd::ActionPauseEx()
{ // "Smart" Pause
	// Same as Pause but if stopped then start the playback instead of does nothing
	if (isMediaPlay)
		ActionPause();
	else
		ActionPlay(false);
}

void WinylWnd::ActionPlay(bool isPlayFocused, bool isReplay, bool isRepeat)
{ // Play a track
	skinDraw.DrawPosition(0);

	ListNodeUnsafe node = nullptr;

	if (isReplay)
		node = skinList->GetPlayNode(); // Play the same track

	if (node == nullptr)
	{
		if (isPlayFocused)// || !skinList->IsNowPlayingList())
			node = skinList->GetFocusNode(); // Or the selected track
		else
		{
			node = skinList->GetLastPlayNode(); // Or the last played
			if (node == nullptr)
				node = skinList->GetFocusNode(); // Or the selected track
		}
	}

	if (node == nullptr)
		node = skinList->FindPrevTrack(nullptr, true); // Or the first track

	if (node)
	{
		//skinList->SetPlayNode(pNode);
		if (!skinList->IsNowPlaying() ||
			(isPlayFocused && !isReplay && !skinList->IsNowPlayingOpen()))
		{
			PlayNode(node, isRepeat, true);
			//MessageBeep(1);
		}
		else
			PlayNode(node, isRepeat, false);

		if (settings.IsShuffle())
			skinList->AddShuffle(node);
	}
}

void WinylWnd::ActionPause()
{ // Pause/Resume the playback
	if (isMediaPlay)
	{
		if (isMediaRadio)
		{
			ActionStop();
			return;
		}

		if (!isMediaPause) // Pause
		{
			isMediaPause = true;

			libAudio.Pause();

			skinList->SetPause(true);

			::KillTimer(thisWnd, TimerValue::PosID);
			::KillTimer(thisWnd, TimerValue::TimeID);
			if (!visuals.empty())
				::SetTimer(thisWnd, TimerValue::VisID, TimerValue::Vis, NULL);

			skinDraw.DrawPlay(false);

			win7TaskBar.UpdateButtons(thisWnd, false);

			if (settings.IsLastFM())
				lastFM.Pause();
		}
		else // Resume
		{
			isMediaPause = false;

			libAudio.Play();

			skinList->SetPause(false);

			::SetTimer(thisWnd, TimerValue::PosID, TimerValue::Pos, NULL);
			::SetTimer(thisWnd, TimerValue::TimeID, TimerValue::Time, NULL);
			if (!visuals.empty())
				::SetTimer(thisWnd, TimerValue::VisID, TimerValue::Vis, NULL);

			skinDraw.DrawPlay(true);

			win7TaskBar.UpdateButtons(thisWnd, true);

			if (settings.IsLastFM())
				lastFM.Resume();
		}
	}
}

void WinylWnd::ActionPosition(int position)
{ // Set the position of the playing track
	if (isMediaPlay && !isMediaRadio)
	{
		libAudio.SetPosition(position);

		if (isMediaPause)
			ActionPause();
	}
	else // If nothing is playing then reset the position
		skinDraw.DrawPosition(0);
}

void WinylWnd::ActionMute(bool isMute)
{ // Mute the sound
	settings.SetMute(isMute);
	libAudio.SetMute(isMute);
	skinDraw.DrawMute(isMute);
	if (isMute)
		skinDraw.DrawVolume(0);
	else
		skinDraw.DrawVolume(settings.GetVolume());
	contextMenu.CheckMute(isMute);
}

void WinylWnd::ActionVolume(int volume, bool isSkinDraw)
{ // Set the volume
	settings.SetVolume(volume);
	libAudio.SetVolume(volume);

	if (isSkinDraw)
		skinDraw.DrawVolume(volume);

	if (settings.IsMute())
	{
		settings.SetMute(false);
		libAudio.SetMute(false);
		skinDraw.DrawMute(false);
		contextMenu.CheckMute(false);
	}
}

void WinylWnd::ActionSetRating(int rating, bool isSkinDraw)
{ // Set the rating of the playing track
	skinList->SetPlayRating(rating);

	if (isSkinDraw)
		skinDraw.DrawRating(rating);

	dBase.SetRating(idMediaLibrary, idMediaPlaylist, rating, true);
}

void WinylWnd::ActionVolumeUp()
{ // Volume up 5%
	int volume = settings.GetVolume(); //libAudio.GetVolume();
	volume += 5000;
	volume = std::min(volume, 100000);
	
	ActionVolume(volume, true);
}

void WinylWnd::ActionVolumeDown()
{ // Volume down 5%
	int volume = settings.GetVolume(); //libAudio.GetVolume();
	volume -= 5000;
	volume = std::max(volume, 0);
	
	ActionVolume(volume, true);
}

void WinylWnd::ActionNextTrack()
{ // Next track

	ListNodeUnsafe newNode = nullptr;
	ListNodeUnsafe playNode = skinList->GetPlayNode();

	if (settings.IsShuffle()) // Shuffle is on
	{
		// Increase skip count of the previous track
		if (!isMediaRadio) // If it wasn't a radio
			dBase.IncreaseSkip(idMediaLibrary, idMediaPlaylist);

		// Search for a random track that wasn't played before
		newNode = skinList->FindNextTrackShuffle();

		if (newNode) // Play
		{
			//skinList->SetPlayNode(newNode);
			skinList->ScrollToMyNode(newNode);
			PlayNode(newNode);
		}
		else // Found nothing means all tracks are played
		{
			if (settings.IsRepeat()) // Repeat is on
			{
				// Create a new shuffle list and search for a random track
				skinList->RemoveShuffle();
				newNode = skinList->FindNextTrackShuffle();

				if (newNode) // Play
				{
					//skinList->SetPlayNode(newNode);
					skinList->ScrollToMyNode(newNode);
					PlayNode(newNode);
				}
			}
			else // Stop the playback
				ActionStop(); // Also destroys the current shuffle list
		}

		return;
	}

	// Shuffle is off

	if (playNode) // Something is playing
	{
		// Increase skip count of the previous track
		if (!isMediaRadio) // If it wasn't a radio
			dBase.IncreaseSkip(idMediaLibrary, idMediaPlaylist);

		// Search for the next track
		ListNodeUnsafe nextNode = skinList->FindNextTrack(playNode);

		if (nextNode == nullptr) // Found nothing means that it was the last track
		{
			if (settings.IsRepeat()) // Repeat is on
				newNode = skinList->FindPrevTrack(nullptr); // Go to the first track
			else
				newNode = nullptr; // Stop the playback
		}
		else
			newNode = nextNode;

		if (newNode) // Play
		{
			//skinList->SetPlayNode(newNode);
			skinList->ScrollToMyNode(newNode);
			PlayNode(newNode);
		}
		else // Stop the playback
			ActionStop();
	}
	else // Nothing is playing
	{
		// Get last played track
		ListNodeUnsafe lastPlayNode = skinList->GetLastPlayNode();

		if (lastPlayNode)
		{
			// Search for the next track that after the last played track
			ListNodeUnsafe nextNode = skinList->FindNextTrack(lastPlayNode);

			if (nextNode == nullptr) // Found nothing means that it was the last track
			{
				if (settings.IsRepeat()) // Repeat is on
					newNode = skinList->FindPrevTrack(nullptr); // Go to the first track
				else
					newNode = nullptr; // Do nothing
			}
			else
				newNode = nextNode;
		}
		else // If there is not last played track just play the first track
			newNode = skinList->FindPrevTrack(nullptr);

		if (newNode) // Play
		{
			//skinList->SetPlayNode(newNode);
			skinList->ScrollToMyNode(newNode);
			PlayNode(newNode);
		}
	}
}

void WinylWnd::ActionPrevTrack()
{ // Previous track

	ListNodeUnsafe newNode = nullptr;
	ListNodeUnsafe playNode = skinList->GetPlayNode();

	if (settings.IsShuffle()) // Shuffle is on
	{
		// Get the previous track from the shuffle list
		newNode = skinList->FindPrevTrackShuffle();

		if (newNode) // Play
		{
			//skinList->SetPlayNode(newNode);
			skinList->ScrollToMyNode(newNode);
			PlayNode(newNode);
		}
		else // Stop the playback
			ActionStop(); // Also destroys the current shuffle list

		return;
	}

	// Shuffle is off

	if (playNode) // Something is playing
	{
		// Search for the previous track
		ListNodeUnsafe prevNode = skinList->FindPrevTrack(playNode);

		if (prevNode == nullptr) // Found nothing means that it was the first track
		{
			if (settings.IsRepeat()) // Repeat is on
				newNode = skinList->FindNextTrack(nullptr); // Go to the last track
			else
				newNode = nullptr; // Stop the playback
		}
		else
			newNode = prevNode;

		if (newNode) // Play
		{
			//skinList->SetPlayNode(newNode);
			skinList->ScrollToMyNode(newNode);
			PlayNode(newNode);
		}
		else // Stop the playback
			ActionStop();
	}
	else // Nothing is playing
	{
		// Get last played track
		ListNodeUnsafe lastPlayNode = skinList->GetLastPlayNode();

		if (lastPlayNode)
		{
			// Search for the previous track that before the last played track
			ListNodeUnsafe prevNode = skinList->FindPrevTrack(lastPlayNode);

			if (prevNode == nullptr) // Found nothing means that it was the first track
			{
				if (settings.IsRepeat()) // Repeat is on
					newNode = skinList->FindNextTrack(nullptr); // Go to the last track
				else
					newNode = nullptr; // Do nothing
			}
			else
				newNode = prevNode;
		}
		else // If there is not last played track just play the first track
			newNode = skinList->FindPrevTrack(nullptr);

		if (newNode) // Play
		{
			//skinList->SetPlayNode(newNode);
			skinList->ScrollToMyNode(newNode);
			PlayNode(newNode);
		}
	}
}

void WinylWnd::ActionShuffle(bool isShuffle)
{ // Toggle shuffle
	settings.SetShuffle(isShuffle);

	if (isShuffle)
	{
		if (skinList->GetPlayNode())
			skinList->AddShuffle(skinList->GetPlayNode());
	}
	else
		skinList->RemoveShuffle();

	skinDraw.DrawShuffle(isShuffle);
	contextMenu.CheckShuffle(isShuffle);
}

void WinylWnd::ActionRepeat(bool isRepeat)
{ // Toggle repeat
	settings.SetRepeat(isRepeat);
	skinDraw.DrawRepeat(isRepeat);
	contextMenu.CheckRepeat(isRepeat);
}

bool WinylWnd::PlayNode(ListNodeUnsafe node, bool isRepeat, bool isNewNowPlaying, bool isReconnect)
{
	isRepeatTrack = isRepeat;

	if (node == nullptr)
		return false;

	// This function can invalidate nodes so make the node safe just in case
	ListNodeSafe safeNode(node);


	isMediaRadio = false;

	LibAudio::Error error = LibAudio::Error::None;
	if (PathEx::IsURL(node->GetFile()))
	{
		isMediaRadio = true;
		libAudio.PlayURL(node->GetFile(), isReconnect);
	}
	else
		error = libAudio.PlayFile(node->GetFile(), node->GetCueValue());

	if (error != LibAudio::Error::None)
	{
		isMediaRadio = false;

		ActionStop();
		EnableAll(false);
		if (error == LibAudio::Error::File) // File error
			MessageBox::Show(thisWnd, lang.GetLine(Lang::Message, 0), lang.GetLine(Lang::Message, 3), MessageBox::Icon::Warning);
		else //if (error == LibAudio::Error::Device) // Device error
			MessageBox::Show(thisWnd, lang.GetLine(Lang::Message, 0), lang.GetLine(Lang::Message, 8), MessageBox::Icon::Warning);
		EnableAll(true);
		//skinList->SetPlayNode(nullptr);
		return false;
	}

	// This setting is stupid it should be always true
	if (!settings.IsLibraryNowPlaying())
	{
		if (!dBase.IsNowPlayingOpen())
		{
			dBase.CloseNowPlaying();
			dBase.NewNowPlaying();
		}
	}
	else
	{
		if (isNewNowPlaying)
		{
			assert(dBase.IsNowPlayingOpen() == skinList->IsNowPlayingOpen());

			if (!dBase.IsNowPlayingOpen())
			{
				dBase.CloseNowPlaying();
				dBase.NewNowPlaying();
			}
			if (!skinList->IsNowPlayingOpen())
			{
				skinList->DeleteNowPlaying();
				skinList->NewNowPlaying();
				settings.NewNowPlaying();
			}
		}
	}

	skinList->SetPlayNode(node);

	idMediaLibrary = node->idLibrary;
	idMediaPlaylist = node->idPlaylist;

	if (!isMediaRadio)
	{
		isMediaPlay = true;
		isMediaPause = false;

		::SetTimer(thisWnd, TimerValue::PosID, TimerValue::Pos, NULL);
		::SetTimer(thisWnd, TimerValue::TimeID, TimerValue::Time, NULL);
		if (!visuals.empty())
			::SetTimer(thisWnd, TimerValue::VisID, TimerValue::Vis, NULL);

		skinDraw.DrawPlay(true);

		std::wstring filename, title, album, artist, genre, year;
		dBase.GetSongTags(node->idLibrary, node->idPlaylist, filename, title, album, artist, genre, year);

		SkinDrawText(title.empty() ? filename : title, album, artist, genre, year, false);

		skinDraw.DrawRating(node->rating);

		LyricsThread(node->GetFile(), title, artist);

		//skinDraw.DrawCover(pNode->csFile, csAlbum);
		bool coverThread = CoverThread(node->GetFile(), album, artist);

		SetWindowCaption(artist, title.empty() ? filename : title);

		win7TaskBar.UpdateButtons(thisWnd, true);

		if (!coverThread)
			ShowPopup();

		if (settings.IsLastFM())
			lastFM.Start(artist, title.empty() ? filename : title, album, libAudio.GetTimeLength());
		//if (settings.IsMSN())
		//	Messengers::MSNPlay(artist, title.empty() ? filename : title);
	}
	else
	{
		EnableWaitRadioCursor(true);

		isMediaPlay = false;
		isMediaPause = false;

		radioString = node->GetLabel(SkinListElement::Type::Title);
		
		skinDraw.DrawRating(node->rating + (skinList->IsRadio() ? 100 : 0));

		SkinDrawText(radioString, L"", L"", L"", L"", true);

		::KillTimer(thisWnd, TimerValue::PosID);
		::KillTimer(thisWnd, TimerValue::TimeID);
		if (!visuals.empty())
			::SetTimer(thisWnd, TimerValue::VisID, TimerValue::Vis, NULL);

		skinDraw.DrawPosition(0);
		skinDraw.DrawPlay(false);
		SetLyricsNone(true);
		SetCoverNone();

		SetWindowCaption(L"", radioString, true);

		win7TaskBar.UpdateButtons(thisWnd, false);

		if (settings.IsLastFM())
			lastFM.Stop();
		//if (settings.IsMSN())
		//	Messengers::MSNStop();
	}

	return true;
}

void WinylWnd::StartRadio(LibAudio::Error error, bool isReconnect)
{
	EnableWaitRadioCursor(false);

	if (error == LibAudio::Error::None)
	{
		isMediaPlay = true;
		isMediaPause = false;

		//::SetTimer(thisWnd, TIMER_POS_ID, TIMER_POS, NULL);
		if (!visuals.empty())
			::SetTimer(thisWnd, TimerValue::VisID, TimerValue::Vis, NULL);

		skinDraw.DrawPlay(true);

		std::wstring title = radioString, artist, album;

		libAudio.GetRadioTags(title, artist, album);

		SkinDrawText(title, album, artist, L"", L"", true);

		SetWindowCaption(artist, title);

		win7TaskBar.UpdateButtons(thisWnd, true);

		ShowPopup();
	}
	else
	{
		if (!isReconnect)
		{
			EnableAll(false);
			if (error == LibAudio::Error::File) // File error
				MessageBox::Show(thisWnd, lang.GetLine(Lang::Message, 0), lang.GetLine(Lang::Message, 3), MessageBox::Icon::Warning);
			else if (error == LibAudio::Error::Inet) // Inet error
				MessageBox::Show(thisWnd, lang.GetLine(Lang::Message, 0), lang.GetLine(Lang::Message, 4), MessageBox::Icon::Warning);
			else //if (error == LibAudio::Error::Device) // Device error
				MessageBox::Show(thisWnd, lang.GetLine(Lang::Message, 0), lang.GetLine(Lang::Message, 8), MessageBox::Icon::Warning);
			EnableAll(true);
		}

		if (skinList->GetPlayNode())
			skinList->SetPlayNode(nullptr);

		skinDraw.DrawRating(-1);
		skinDraw.DrawTextNone();

		SetWindowCaptionNull();
	}
}

std::wstring WinylWnd::ChangeFile(long long& outCue)
{
	ListNodeUnsafe tempNode = nullptr;
	//tempFocusNode = nullptr;

	std::wstring file;
	outCue = 0;

	if (isRepeatTrack) // Return the same track
	{
		ListNodeUnsafe playNode = skinList->GetPlayNode();
		tempNode = playNode;

		if (tempNode)
			file = tempNode->GetFile();

		skinList->SetTempNode(tempNode);
		return file;
	}

	if (settings.IsShuffle()) // Return a random track
	{
		tempNode = skinList->FindNextTrackShuffle();
		if (tempNode == nullptr) // All tracks are played
		{
			if (settings.IsRepeat()) // Repeat is on
			{
				// Create a new shuffle list and return a random track
				skinList->RemoveShuffle();
				tempNode = skinList->FindNextTrackShuffle();
			}
		}
	}
	else 
	{
		ListNodeUnsafe focusNode = nullptr;

		if (settings.IsPlayFocus())
		{
			/*focusNode = skinList->GetFocusNode();

			tempFocusNode = focusNode;
			if (focusNode == nullptr && settings.IsLibraryNowPlaying())
				focusNode = skinList->GetFocusNodePlay();*/

			if (!settings.IsLibraryNowPlaying())
				focusNode = skinList->GetFocusNode();
			else
				focusNode = skinList->GetFocusNodePlay();
		}

		//ListNodeUnsafe focusNode = skinList->GetFocusNodePlay();
		ListNodeUnsafe playNode = skinList->GetPlayNode();

		// If nothing is selected use playing node
		if (focusNode == nullptr)
			focusNode = playNode;
	
		if (focusNode == nullptr)
		{ // If nothing is selected and playing then return the first track
			tempNode = skinList->FindPrevTrack(nullptr);
		}
		else if (focusNode == playNode)
		{ // If selected and playing are the same then return the next track
			ListNodeUnsafe nextNode = skinList->FindNextTrack(playNode);
			if (nextNode == nullptr) // If it is the last track
			{
				if (settings.IsRepeat()) // Repeat is on
					tempNode = skinList->FindPrevTrack(nullptr); // Return the first track
				else
					tempNode = nullptr; // Stop the playback
			}
			else
				tempNode = nextNode;
		}
		else
		{ // Return the selected track
			tempNode = focusNode;
		}
	}

	if (tempNode) // Return the track
	{
		file = tempNode->GetFile();
		outCue = tempNode->GetCueValue();
	}

	skinList->SetTempNode(tempNode);
	return file;
}

void WinylWnd::ChangeNode(bool isError, bool isRadio)
{
	if (skinList->GetTempNode() && !isError)
	{
		// This setting is stupid it should be always true
		if (!settings.IsLibraryNowPlaying())
		{
			if (!dBase.IsNowPlayingOpen())
			{
				dBase.CloseNowPlaying();
				dBase.NewNowPlaying();
			}
		}
		/*else
		{
			if (settings.IsPlayFocus() && tempFocusNode)
			{
				if (!dBase.IsNowPlaying())
				{
					dBase.CloseNowPlaying();
					dBase.NewNowPlaying();
				}
				if (!skinList->IsNowPlayingList())
				{
					skinList->DeleteNowPlaying();
					skinList->NewNowPlaying();
					settings.NewNowPlaying();
				}
			}
		}*/

		skinList->SetPlayNode(skinList->GetTempNode());
		if (isMediaPause)
			skinList->SetPause(isMediaPause);

		if (!isRadio)
		{
			isMediaRadio = false;

			skinDraw.DrawPosition(0);

			skinDraw.DrawRating(skinList->GetTempNode()->rating);

			std::wstring filename, title, album, artist, genre, year;
			dBase.GetSongTags(skinList->GetTempNode()->idLibrary, skinList->GetTempNode()->idPlaylist, filename, title, album, artist, genre, year);

			SkinDrawText(title.empty() ? filename : title, album, artist, genre, year, false);

			LyricsThread(skinList->GetTempNode()->GetFile(), title, artist);

			//skinDraw.DrawCover(pTempNode->csFile, csAlbum);
			bool coverThread = CoverThread(skinList->GetTempNode()->GetFile(), album, artist);

			SetWindowCaption(artist, title.empty() ? filename : title);

			// Increase skip count of the previous track
			dBase.IncreaseCount(idMediaLibrary, idMediaPlaylist);

			if (!coverThread)
				ShowPopup();

			if (settings.IsLastFM())
				lastFM.Start(artist, title.empty() ? filename : title, album, libAudio.GetTimeLength());
			//if (settings.IsMSN())
			//	Messengers::MSNPlay(artist, title.empty() ? filename : title);
		}
		else
		{
			isMediaRadio = true;
			isMediaPlay = false;
			isMediaPause = false;

			radioString = skinList->GetTempNode()->GetLabel(SkinListElement::Type::Title);

			skinDraw.DrawRating(skinList->GetTempNode()->rating + (skinList->IsRadio() ? 100 : 0));

			SkinDrawText(radioString, L"", L"", L"", L"", true);

			::KillTimer(thisWnd, TimerValue::PosID);
			::KillTimer(thisWnd, TimerValue::TimeID);
			if (!visuals.empty())
				::SetTimer(thisWnd, TimerValue::VisID, TimerValue::Vis, NULL);

			skinDraw.DrawPosition(0);
			skinDraw.DrawPlay(false);
			SetLyricsNone(true);
			SetCoverNone();

			SetWindowCaption(L"", radioString, true);

			win7TaskBar.UpdateButtons(thisWnd, false);

			if (settings.IsLastFM())
				lastFM.Stop();
			//if (settings.IsMSN())
			//	Messengers::MSNStop();
		}

		// Always at the end or IncreaseCount that above will set incorrect values
		idMediaLibrary = skinList->GetTempNode()->idLibrary;
		idMediaPlaylist = skinList->GetTempNode()->idPlaylist;

		skinList->SetTempNode(nullptr);
	}
	else
	{
		if (isMediaRadio)
		{
			if (isMediaPlay && !isMediaPause && skinList->GetPlayNode() && libAudio.IsRadioStream())
				PlayNode(skinList->GetPlayNode(), false, false, true);
			else
				ActionStop();
		}
		else
		{
			dBase.IncreaseCount(idMediaLibrary, idMediaPlaylist);
			ActionStop();
		}
	}
}

void WinylWnd::SkinDrawText(const std::wstring& title, const std::wstring& album, const std::wstring& artist,
							const std::wstring& genre, const std::wstring& year, bool isRadio)
{
	std::wstring album2 = album;
	std::wstring artist2 = artist;

	if (!isRadio)
	{
		if (album2.empty())
			album2 = lang.GetLineS(Lang::Playlist, 0);
		if (artist2.empty())
			artist2 = lang.GetLineS(Lang::Playlist, 0);
	}
	else
	{
		if (artist2.empty())
			artist2 = lang.GetLineS(Lang::Playlist, 3);
		if (album2.empty())
			album2 = lang.GetLineS(Lang::Playlist, 0);
	}

	skinDraw.DrawText(title, album2, artist2, genre, year, isMediaRadio ? -1 : libAudio.GetTimeLength());
	if (skinPopup)
		skinPopup->SetText(title, album2, artist2, genre, year, isMediaRadio ? -1 : libAudio.GetTimeLength());
}

void WinylWnd::FillTree(TreeNodeUnsafe node)
{
	if (!(node->IsType() && node->GetType() == SkinTreeNode::Type::Radio))
		BeginWaitCursor();

	if (node->IsType())
	{
		switch (node->GetType())
		{
		case SkinTreeNode::Type::Artist:
			dBase.FillTreeArtist(skinTree.get(), node);
			break;
		case SkinTreeNode::Type::Composer:
			dBase.FillTreeComposer(skinTree.get(), node);
			break;
		case SkinTreeNode::Type::Album:
			dBase.FillTreeAlbum(skinTree.get(), node);
			break;
		case SkinTreeNode::Type::Genre:
			dBase.FillTreeGenre(skinTree.get(), node);
			break;
		case SkinTreeNode::Type::Year:
			dBase.FillTreeYear(skinTree.get(), node);
			break;
		case SkinTreeNode::Type::Folder:
			dBase.FillTreeFolder(skinTree.get(), node,
				settings.IsAddAllToLibrary() ? nullptr : &libraryFolders);
			break;
		case SkinTreeNode::Type::Radio:
			radio.LoadTree(skinTree.get(), node);
		}
	}
	else if (node->IsValue())
	{
		switch (node->GetType())
		{
		case SkinTreeNode::Type::Artist:
			dBase.FillTreeArtistAlbum(skinTree.get(), node);
			break;
		case SkinTreeNode::Type::Composer:
			dBase.FillTreeComposerArtist(skinTree.get(), node);
			break;
		case SkinTreeNode::Type::Genre:
			dBase.FillTreeGenreArtist(skinTree.get(), node);
			break;
		case SkinTreeNode::Type::Year:
			dBase.FillTreeYearArtist(skinTree.get(), node);
			break;
		case SkinTreeNode::Type::Folder:
			dBase.FillTreeFolderSub(skinTree.get(), node);
			break;
		}
	}
	else if (node->IsArtist())
	{
		switch (node->Parent()->GetType())
		{
		case SkinTreeNode::Type::Composer:
			dBase.FillTreeComposerAlbum(skinTree.get(), node);
			break;
		case SkinTreeNode::Type::Genre:
			dBase.FillTreeGenreAlbum(skinTree.get(), node);
			break;
		case SkinTreeNode::Type::Year:
			dBase.FillTreeYearAlbum(skinTree.get(), node);
			break;
		}
	}

	if (!(node->IsType() && node->GetType() == SkinTreeNode::Type::Radio))
		EndWaitCursor();
}

void WinylWnd::FillList(TreeNodeUnsafe node)
{
	// First always need to stop and clear the search
	if (threadSearch.IsJoinable())
	{
		dBase.SetStopSearch(true);
		threadSearch.Join();
		dBase.SetStopSearch(false);
	}
	if (!skinEdit->IsSearchEmpty())
	{
		skinEdit->SearchClear();
		skinDraw.DrawSearchClear(false);
	}

	if (node && node->GetNodeType() == SkinTreeNode::NodeType::Head)
	{
		// Return to Now Playing view
		if (node->GetType() == SkinTreeNode::Type::NowPlaying)
		{
			settings.ReturnNowPlaying();
			dBase.ClosePlaylist();
			dBase.ReturnNowPlaying();
			skinList->NowPlayingList();

			UpdateStatusLine();
		}

		return;
	}

	if (node)
	{
		if (node->IsAlbum())
			settings.SetLibraryType(node->IsType(), (int)node->Parent()->Parent()->GetType());
		else if (node->IsArtist())
			settings.SetLibraryType(node->IsType(), (int)node->Parent()->GetType());
		else
			settings.SetLibraryType(node->IsType(), (int)node->GetType());
		settings.SetLibraryValue(node->IsValue(), node->GetValue());
		settings.SetLibraryArtist(node->IsArtist(), node->GetArtist());
		settings.SetLibraryAlbum(node->IsAlbum(), node->GetAlbum());
	}

	// !!! In FillJump below should be the same

	// If select the same as now playing playlist or library item (if playing) and not smatrlist.
	//if ((isMediaPlay || settings.GetLibraryType() == (int)SkinTreeNode::Type::Playlist) &&
	//	settings.IsNowPlaying() &&
	//	settings.GetLibraryType() != (int)SkinTreeNode::Type::Smartlist)
	if (isMediaPlay &&
		(settings.GetLibraryType() == (int)SkinTreeNode::Type::Playlist ||
		settings.GetLibraryType() == (int)SkinTreeNode::Type::Radio) &&
		settings.IsNowPlaying())
	{
		dBase.ClosePlaylist();
		dBase.ReturnNowPlaying();
		skinList->NowPlayingList();

		UpdateStatusLine();
		return;
	}

	if (!(settings.IsLibraryValue() && settings.GetLibraryType() == (int)SkinTreeNode::Type::Radio))
		BeginWaitCursor();

	dBase.ClosePlaylist();

	if (settings.IsLibraryAlbum())
	{
		switch ((SkinTreeNode::Type)settings.GetLibraryType())
		{
		case SkinTreeNode::Type::Artist:
			dBase.FillListArtistAlbum(skinList.get(), settings.GetLibraryValue(), settings.GetLibraryArtist(), settings.GetLibraryAlbum());
			break;
		case SkinTreeNode::Type::Composer:
			dBase.FillListComposerAlbum(skinList.get(), settings.GetLibraryValue(), settings.GetLibraryArtist(), settings.GetLibraryAlbum());
			break;
		case SkinTreeNode::Type::Genre:
			dBase.FillListGenreAlbum(skinList.get(), settings.GetLibraryValue(), settings.GetLibraryArtist(), settings.GetLibraryAlbum());
			break;
		case SkinTreeNode::Type::Year:
			dBase.FillListYearAlbum(skinList.get(), settings.GetLibraryValue(), settings.GetLibraryArtist(), settings.GetLibraryAlbum());
			break;
		}
	}
	else if (settings.IsLibraryArtist())
	{
		switch ((SkinTreeNode::Type)settings.GetLibraryType())
		{
		case SkinTreeNode::Type::Composer:
			dBase.FillListComposerArtist(skinList.get(), settings.GetLibraryValue(), settings.GetLibraryArtist());
			break;
		case SkinTreeNode::Type::Genre:
			dBase.FillListGenreArtist(skinList.get(), settings.GetLibraryValue(), settings.GetLibraryArtist());
			break;
		case SkinTreeNode::Type::Year:
			dBase.FillListYearArtist(skinList.get(), settings.GetLibraryValue(), settings.GetLibraryArtist());
			break;
		}
	}
	else if (settings.IsLibraryValue())
	{
		switch ((SkinTreeNode::Type)settings.GetLibraryType())
		{
		case SkinTreeNode::Type::Album:
			dBase.FillListAlbum(skinList.get(), settings.GetLibraryValue());
			break;
		case SkinTreeNode::Type::Artist:
			dBase.FillListArtist(skinList.get(), settings.GetLibraryValue());
			break;
		case SkinTreeNode::Type::Composer:
			dBase.FillListComposer(skinList.get(), settings.GetLibraryValue());
			break;
		case SkinTreeNode::Type::Genre:
			dBase.FillListGenre(skinList.get(), settings.GetLibraryValue());
			break;
		case SkinTreeNode::Type::Year:
			dBase.FillListYear(skinList.get(), settings.GetLibraryValue());
			break;
		case SkinTreeNode::Type::Folder:
			dBase.FillListFolder(skinList.get(), settings.GetLibraryValue());
			break;
		case SkinTreeNode::Type::Playlist:
			dBase.OpenPlaylist(settings.GetLibraryValue());
			dBase.FillPlaylist(skinList.get(), settings.GetLibraryValue());
			break;
		case SkinTreeNode::Type::Smartlist:
			dBase.FillSmartlist(skinList.get(), settings.GetLibraryValue());
			break;
		case SkinTreeNode::Type::Radio:
			radio.LoadList(skinList.get(), settings.GetLibraryValue());
			break;
		}
	}

	if (!(settings.IsLibraryValue() && settings.GetLibraryType() == (int)SkinTreeNode::Type::Radio))
		EndWaitCursor();

	UpdateStatusLine();
}

void WinylWnd::FillJump(ListNodeUnsafe node, SkinTreeNode::Type type)
{
	if (node == nullptr)
		return;

	BeginWaitCursor();

	// First always need to stop and clear the search
	if (threadSearch.IsJoinable())
	{
		dBase.SetStopSearch(true);
		threadSearch.Join();
		dBase.SetStopSearch(false);
	}
	if (!skinEdit->IsSearchEmpty())
	{
		skinEdit->SearchClear();
		skinDraw.DrawSearchClear(false);
	}

	//////////////
	if (type != SkinTreeNode::Type::Folder)
	{
		std::wstring filename, title, album, artist, genre, year;
		dBase.GetSongTags(node->idLibrary, node->idPlaylist, filename, title, album, artist, genre, year, false);

		settings.SetLibraryType(false, (int)type);
		settings.SetLibraryArtist(false, L"");
		settings.SetLibraryAlbum(false, L"");
		if (type == SkinTreeNode::Type::Artist)
			settings.SetLibraryValue(true, artist);
		else if (type == SkinTreeNode::Type::Album)
			settings.SetLibraryValue(true, album);
		else if (type == SkinTreeNode::Type::Year)
			settings.SetLibraryValue(true, year);
		else if (type == SkinTreeNode::Type::Genre)
			settings.SetLibraryValue(true, genre);
	}
	else // Folder
	{
		std::wstring folder = PathEx::PathFromFile(node->GetFile());

		if (isPortableVersion && !folder.empty())
		{
			if (folder[1] == ':') // Only for drives
			{
				std::wstring folderDrive(1, folder[0]);
				std::wstring programDrive(1, programPath[0]);
				if (StringEx::ToLowerUS(folderDrive) == StringEx::ToLowerUS(programDrive))
					folder[0] = '?';
			}
		}

		settings.SetLibraryType(false, (int)type);
		settings.SetLibraryValue(true, folder);
		settings.SetLibraryArtist(false, L"");
		settings.SetLibraryAlbum(false, L"");
	}

	// !!! From FillList above

	if ((isMediaPlay || settings.GetLibraryType() == (int)SkinTreeNode::Type::Playlist) &&
		settings.IsNowPlaying() &&
		settings.GetLibraryType() != (int)SkinTreeNode::Type::Smartlist)
	{
		dBase.ClosePlaylist();
		dBase.ReturnNowPlaying();
		skinList->NowPlayingList();

		EndWaitCursor();
		UpdateStatusLine();
		return;
	}

	dBase.ClosePlaylist();

	switch ((SkinTreeNode::Type)settings.GetLibraryType())
	{
	case SkinTreeNode::Type::Album:
		dBase.FillListAlbum(skinList.get(), settings.GetLibraryValue());
		break;
	case SkinTreeNode::Type::Artist:
		dBase.FillListArtist(skinList.get(), settings.GetLibraryValue());
		break;
	case SkinTreeNode::Type::Year:
		dBase.FillListYear(skinList.get(), settings.GetLibraryValue());
		break;
	case SkinTreeNode::Type::Genre:
		dBase.FillListGenre(skinList.get(), settings.GetLibraryValue());
		break;
	case SkinTreeNode::Type::Folder:
		dBase.FillListFolder(skinList.get(), settings.GetLibraryValue());
		break;
	}

	EndWaitCursor();

	UpdateStatusLine();
}

void WinylWnd::FillListSearch()
{
	if (!skinEdit->IsSearchEmpty())
	{
		dBase.SetStopSearch(true);

		settings.SetLibraryNoneOld();

		dBase.ClosePlaylist();

		skinList->SetControlRedraw(false);

		isThreadSearch = true;
		SearchThreadStart();
	}
	else // Return previous view
	{
		if (threadSearch.IsJoinable())
		{
			dBase.SetStopSearch(true);
			threadSearch.Join();
			dBase.SetStopSearch(false);
		}

		FillList();
	}
}

void WinylWnd::SearchThreadStart()
{
	if (!threadSearch.IsRunning())
	{
		if (threadSearch.IsJoinable())
			threadSearch.Join();

		threadSearch.Start(std::bind(&WinylWnd::SearchThreadRun, this));
	}
}

void WinylWnd::SearchThreadRun()
{
	while (isThreadSearch)
	{
		isThreadSearch = false;
		int type = settings.GetSearchType();
		std::wstring text = skinEdit->GetSearchText();

		dBase.SetStopSearch(false);
		skinList->DeleteAllNode();

		if (!text.empty())
		{
			if (type == 0)
				dBase.FillListSearchAll(skinList.get(), text);
			else if (type == 1)
				dBase.FillListSearchTrack(skinList.get(), text);
			else if (type == 2)
				dBase.FillListSearchAlbum(skinList.get(), text);
			else if (type == 3)
				dBase.FillListSearchArtist(skinList.get(), text);

			if (!dBase.IsStopSearch())
				UpdateStatusLine();
		}
	}

	if (IsWnd()) ::PostMessageW(Wnd(), UWM_SEARCHDONE, 0, 0);
}

void WinylWnd::SearchThreadDone()
{
	if (isThreadSearch)
	{
		SearchThreadStart();
		return;
	}

	skinList->SetControlRedraw(true);
}

bool WinylWnd::OnSysCommand(WPARAM wParam, LPARAM lParam)
{
	if((wParam & 0xFFF0) == SC_MINIMIZE)
	{
		isWindowIconic = true;

		// For layered window use fast (without animation) minimize
		if (skinDraw.IsLayeredAlpha())
		{
			trayIcon.Minimize(thisWnd, true);
			return true;
		}
	}
	else if((wParam & 0xFFF0) == SC_RESTORE)
	{
		isWindowIconic = false;

		// For layered window use fast (without animation) restore
		if (skinDraw.IsLayeredAlpha())
			trayIcon.Restore(thisWnd, true, settings.IsMaximized());

		if (skinList) // When the skin is changing skinList can be nullptr
			skinList->ScrollToPlayNode();
		LyricsThreadReload();

		if (skinDraw.IsLayeredAlpha())
			return true;
	}
	else if((wParam & 0xFFF0) == SC_CLOSE)
	{
		if (dlgProgressPtr)
		{
			dlgProgressPtr->DestroyOnStop();
			return true;
		}
	}

	return false;
}

LRESULT WinylWnd::WindowProcEx(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case UWM_TIMERTHREAD:
		if (skinList->IsSmoothScrollRun())
			skinList->SmoothScrollRun();
		if (skinTree->IsSmoothScrollRun())
			skinTree->SmoothScrollRun();
		if (skinLyrics->IsSmoothScrollRun())
			skinLyrics->SmoothScrollRun();
		return 0;

	// We use custom implementation of Drag'n'Drop
	// so if the window lost the focus we need to stop Drag'n'Drop
	// also we need to process Escape key in WinylApp.cpp
	case WM_ACTIVATE:
		if (isDragDrop)
			StopDragDrop();
		return 0;

	case WM_SETFOCUS:
		::SetFocus(skinList->Wnd());
		return 0;

	// Experimental feature for minimize/restore animation in Win7
	case WM_NCCALCSIZE:
		if (!skinDraw.IsStyleBorder() && !skinDraw.IsLayeredAlpha())
			return 0;
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	case WM_NCACTIVATE:
		if (!skinDraw.IsStyleBorder() && !skinDraw.IsLayeredAlpha())
			lParam = -1;
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	case WM_SETTEXT:
	case WM_SETICON:
		if (!skinDraw.IsStyleBorder() && !skinDraw.IsLayeredAlpha())
		{
			LONG_PTR style = ::GetWindowLongPtr(hWnd, GWL_STYLE);
			if ((style & WS_VISIBLE) && ::SetWindowLongPtr(hWnd, GWL_STYLE, style & ~WS_VISIBLE))
			{
				LRESULT result = ::DefWindowProc(hWnd, message, wParam, lParam);
				::SetWindowLongPtr(hWnd, GWL_STYLE, style | WS_VISIBLE);
				return result;
			}
		}
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	// End of the feature

	// Icons in the tray menu (only for WinXP)
	case WM_MEASUREITEM:
		return contextMenu.MeasureDrawTrayMenuXP(true, lParam);
	case WM_DRAWITEM:
		return contextMenu.MeasureDrawTrayMenuXP(false, lParam);

	// DWM messages
	case WM_THEMECHANGED:
		futureWin->ThemeChanged();
		return 0;
	case WM_DWMCOMPOSITIONCHANGED:
		skinDraw.EnableDwm(!!futureWin->IsCompositionEnabled());

		if (skinDraw.IsAeroGlass())
			skinDraw.RedrawWindow();

		if (skinMini)
		{
			skinMini->skinDraw.EnableDwm(!!futureWin->IsCompositionEnabled());

			if (skinMini->skinDraw.IsAeroGlass())
				skinMini->skinDraw.RedrawWindow();
		}
		return 0;
	case WM_DWMSENDICONICTHUMBNAIL:
		win7TaskBar.MessageCoverBitmap(thisWnd, HIWORD(lParam), LOWORD(lParam));
		return 0;
	case WM_DWMSENDICONICLIVEPREVIEWBITMAP:
		win7TaskBar.MessageCoverPreview(thisWnd);
		return 0;

	// Global hotkeys
	case WM_HOTKEY:
		switch ((HotKeys::KeyType)wParam)
		{
		case HotKeys::KeyType::None:
			break;
		case HotKeys::KeyType::Play:
			ActionPlayEx();
			break;
		case HotKeys::KeyType::Pause:
			ActionPauseEx();
			break;
		case HotKeys::KeyType::Stop:
			ActionStop();
			break;
		case HotKeys::KeyType::Next:
			ActionNextTrack();
			break;
		case HotKeys::KeyType::Prev:
			ActionPrevTrack();
			break;
		case HotKeys::KeyType::Mute:
			ActionMute(!settings.IsMute());
			break;
		case HotKeys::KeyType::VolumeUp:
			ActionVolumeUp();
			break;
		case HotKeys::KeyType::VolumeDown:
			ActionVolumeDown();
			break;
		case HotKeys::KeyType::Rating1:
			ActionSetRating(1, true);
			break;
		case HotKeys::KeyType::Rating2:
			ActionSetRating(2, true);
			break;
		case HotKeys::KeyType::Rating3:
			ActionSetRating(3, true);
			break;
		case HotKeys::KeyType::Rating4:
			ActionSetRating(4, true);
			break;
		case HotKeys::KeyType::Rating5:
			ActionSetRating(5, true);
			break;
		case HotKeys::KeyType::Popup:
			if (skinPopup && isMediaPlay)
				skinPopup->Popup();
			break;
		}
		return 0;

	// Media keys
	// http://blogs.msdn.com/b/oldnewthing/archive/2006/04/25/583093.aspx
	case WM_APPCOMMAND:
		switch (GET_APPCOMMAND_LPARAM(lParam))
		{
		case APPCOMMAND_MEDIA_PLAY:
			ActionPlayEx();
			return 1;
		case APPCOMMAND_MEDIA_PAUSE:
		case APPCOMMAND_MEDIA_PLAY_PAUSE:
			ActionPauseEx();
			return 1;
		case APPCOMMAND_MEDIA_STOP:
			ActionStop();
			return 1;
		case APPCOMMAND_MEDIA_NEXTTRACK:
			ActionNextTrack();
			return 1;
		case APPCOMMAND_MEDIA_PREVIOUSTRACK:
			ActionPrevTrack();
			return 1;
		//case APPCOMMAND_VOLUME_MUTE:
		//	ActionMute(!settings.IsMute());
		//	return 1;
		//case APPCOMMAND_VOLUME_UP:
		//	ActionVolumeUp();
		//	return 1;
		//case APPCOMMAND_VOLUME_DOWN:
		//	ActionVolumeDown();
		//	return 1;
		}
		return ::DefWindowProc(hWnd, message, wParam, lParam);
	}

	// Taskbar messages
	if (message == wmTaskbarCreated)
	{
		trayIcon.NewIcon(thisWnd, iconSmall);
		return 0;
	}
	if (message == wmTaskbarButtonCreated)
	{
		win7TaskBar.AddButtons(thisWnd, isMediaPlay);
		return 0;
	}

	return WindowProcMy(hWnd, message, wParam, lParam);
	//return ::DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT WinylWnd::WindowProcMy(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{

	case UWM_BASSNEXT:
	{
		libAudio.SyncProcEndImpl();
	}
	return 0;

	case UWM_RADIOSTART:
	{
		StartRadio(libAudio.StartRadio((int)wParam), !!lParam);
	}
	return 0;

	case UWM_BASSCHFREE:
	{
		libAudio.SyncFreeChannelImpl((HSTREAM)lParam);
		return true;
	}
	return 0;

	case UWM_BASSWASTOP:
	{
		libAudio.SyncWAStopPauseImpl(!!wParam, (HSTREAM)lParam);
		return true;
	}
	return 0;

//	case UWM_BASSPRELOAD:
//	{
//		libAudio.SyncProcPreloadImpl();
//		return true;
//	}
//	break;

	case UWM_ACTION:
	{
		SkinElement* element = (SkinElement*)lParam;
		Action(element, (MouseAction)wParam, true);
	}
	return 0;

	case UWM_MINIPLAYER:
	{
		MiniPlayer(wParam ? true : false);
	}
	return 0;

	case UWM_NOMINIPLAYER:
	{
		if (skinMini && settings.IsMiniPlayer())
			MiniPlayer(false);
	}
	return 0;

	case UWM_SEARCHCHANGE:
	{
		if (wParam)
		{
			if (skinEdit->IsSearchEmpty())
				skinDraw.DrawSearchClear(false);
			else
				skinDraw.DrawSearchClear(true);
		}
		FillListSearch();
	}
	return 0;

	case UWM_SEARCHDONE:
	{
		SearchThreadDone();
	}
	return 0;

	case UWM_FILLTREE:
	{
		TreeNodeUnsafe node = (TreeNodeUnsafe)lParam;
		FillTree(node);
	}
	return 0;

	case UWM_FILLLIST:
	{
		TreeNodeUnsafe node = (TreeNodeUnsafe)lParam;
		FillList(node);
	}
	return 0;

	case UWM_LISTSEL:
	{
		UpdateStatusLine();
	}
	return 0;

	case UWM_PLAYFILE:
	{
		ActionPlay(true);
	}
	return 0;

	//case UWM_NEXTFILE:
	//{
	//	*(std::wstring*)lParam = ChangeFile();
	//}
	//return 0;

	case UWM_PLAYDRAW:
	{
		ChangeNode((wParam ? true : false), (lParam ? true : false));
	}
	return 0;

	case UWM_RATING:
	{
		ListNodeUnsafe node = (ListNodeUnsafe)lParam;

		dBase.SetRating(node->idLibrary, node->idPlaylist, node->rating, false);

		if (node == skinList->GetPlayNode())
			skinDraw.DrawRating(node->rating);
	}
	return 0;

	case UWM_CHANGESKIN:
	{
		std::wstring skinName = std::wstring((wchar_t*)lParam);
		
		ActionStop();

		return ReloadSkin(skinName, wParam ? true : false);
	}
	return 0;

	case UWM_LISTMENU:
	{
		std::vector<std::wstring> playlists;
		int selPlaylist = -1;

		TreeNodeUnsafe playlistNode = skinTree->GetPlaylistNode();
		if (playlistNode->HasChild())
		{
			int i = 0;
			for (TreeNodeUnsafe treeNode = playlistNode->Child(); treeNode != nullptr; treeNode = treeNode->Next())
			{
				playlists.push_back(treeNode->GetTitle());

				if (dBase.IsPlaylistOpen() && selPlaylist == -1 &&
					settings.GetLibraryValue() == treeNode->GetValue())
					selPlaylist = i;

				++i;
			}
		}

		contextMenu.ShowListMenu(thisWnd, CPoint((int)wParam, (int)lParam), playlists,
			skinList->IsRadio(), dBase.IsPlaylistOpen(), selPlaylist, dBase.IsNowPlaying() && !dBase.IsNowPlayingOpen());
	}
	return 0;

	case UWM_TREEMENU:
	{
		if (skinTree->GetFocusNode())
		{
			bool isEnable = false;
			if (skinTree->GetFocusNode()->GetNodeType() == SkinTreeNode::NodeType::Text)
				isEnable = true;

			if (skinTree->GetFocusNode()->GetType() == SkinTreeNode::Type::Playlist)
				contextMenu.ShowTreePlaylistMenu(thisWnd, CPoint((int)wParam, (int)lParam), isEnable);
			else if (skinTree->GetFocusNode()->GetType() == SkinTreeNode::Type::Smartlist)
				contextMenu.ShowTreeSmartlistMenu(thisWnd, CPoint((int)wParam, (int)lParam), isEnable);
		}
	}
	return 0;

	case UWM_LYRICSMENU:
	{
		contextMenu.SetLyricsSource(lyricsSource);
		contextMenu.ShowLyricsMenu(thisWnd, CPoint((int)wParam, (int)lParam));
	}
	return 0;

	case UWM_TREESWAP:
	{
		dBase.SavePlaylist(skinTree.get());
		dBase.SaveTreeSmartlists(skinTree.get());
	}
	return 0;

	case UWM_LISTSWAP:
	{
		BeginWaitCursor();
		dBase.SwapPlaylist(skinList.get());
		EndWaitCursor();
	}
	return 0;

	case UWM_RADIOMETA:
	{
		std::wstring title = radioString, artist, album;
		libAudio.GetRadioTags(title, artist, album);

		SkinDrawText(title, album, artist, L"", L"", true);

		SetWindowCaption(artist, title);

		ShowPopup();
	}
	return 0;

//	case UWM_RADIOSTART:
//	{
//		StartRadio((int)wParam);
//	}
//	return 0;

	case UWM_TREEDRAG:
	{
		if (wParam)
			isDragDrop = false;
		else
		{
			isDragDrop = true;

			CPoint ptTree(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			CPoint ptScreen = ptTree;
			::ClientToScreen(skinTree->Wnd(), &ptScreen);
			CPoint ptWin = ptScreen;
			::ScreenToClient(thisWnd, &ptWin);

			HWND wndPoint = ::ChildWindowFromPoint(thisWnd, ptWin);

			if (wndPoint == skinTree->Wnd())
				skinTree->SetDropMovePoint(&ptTree);
			else
				skinTree->SetDropMovePoint(nullptr);
		}
	}
	return 0;

	case UWM_LISTDRAG:
	{
		if (wParam)
		{
			isDragDrop = false;
			wndDragIcon.HideIcon();

			if (wParam == 1)
				DropFilesToPlaylist();
			skinTree->SetDropPoint(nullptr);
		}
		else
		{
			isDragDrop = true;

			CPoint ptList(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			CPoint ptScreen = ptList;
			::ClientToScreen(skinList->Wnd(), &ptScreen);
			CPoint ptWin = ptScreen;
			::ScreenToClient(thisWnd, &ptWin);

			// Track the mouse and use ProcessId to make sure that it is our window
			HWND wndPoint = ::WindowFromPoint(ptScreen);
			DWORD processID = 0;
			::GetWindowThreadProcessId(wndPoint, &processID);
			if (::GetCurrentProcessId() != processID)
				wndPoint = NULL;

			// Mouse over the playlist
			if (wndPoint == skinList->Wnd())
				skinList->SetDropMovePoint(&ptList);
			else
				skinList->SetDropMovePoint(nullptr);

			// Mouse over the library
			if (wndPoint == skinTree->Wnd())
			{
				CPoint ptTree = ptScreen;
				::ScreenToClient(skinTree->Wnd(), &ptTree);
				skinTree->SetDropPoint(&ptTree);
			}
			else
				skinTree->SetDropPoint(nullptr);

			// Mouse leaves the main window
			if (wndPoint == NULL)
			{
				isDragDropOLE = true;
				wndDragIcon.HideIcon();

				ReleaseCapture();
				if (!CopySelectedToClipboard(false))
					::SetCapture(skinList->Wnd()); // Mouse returns to SkinList
				else
					skinList->SetDropMoveStop(); // Files are dropped to other program

				isDragDropOLE = false;
			}
			else
				wndDragIcon.MoveIcon(ptScreen.x, ptScreen.y);
		}
	}
	return 0;

	case UWM_COVERDONE:
	{
		CoverThreadDone();
	}
	return 0;

	case UWM_LYRICSDONE:
	{
		LyricsThreadDone();
	}
	return 0;

	case UWM_LYRICSRECV:
	{
		LyricsThreadReceiving();
	}
	return 0;

	case UWM_TRAYMSG:
	{
		switch(lParam)
		{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
			// When click on the tray icon then bring the window to the front
			// or minimize if it's already the foreground window or restore if it's minimized
			if (!settings.IsMiniPlayer())
			{
				CRect rc;
				::GetWindowRect(thisWnd, rc);
				HWND wndPoint = ::WindowFromPoint(rc.CenterPoint());
				DWORD processID = 0;
				::GetWindowThreadProcessId(wndPoint, &processID);

				if (::IsIconic(thisWnd) || ::GetCurrentProcessId() != processID)
				{
					isWindowIconic = false;
					::SetForegroundWindow(thisWnd);
					if (::IsIconic(thisWnd))
						trayIcon.Restore(thisWnd, skinDraw.IsLayeredAlpha(), settings.IsMaximized());
					if (skinList) // When the skin is changing skinList can be nullptr
						skinList->ScrollToPlayNode();
					LyricsThreadReload();
				}
				else
				{
					isWindowIconic = true;
					trayIcon.Minimize(thisWnd, skinDraw.IsLayeredAlpha());
				}
			}
			else
			{
				if (skinMini)
				{
					DWORD processID = 0, currentProcessID = 0;
					if (skinMini->GetZOrder() == 2)
					{
						CRect rc;
						::GetWindowRect(skinMini->Wnd(), rc);
						HWND wndPoint = ::WindowFromPoint(rc.CenterPoint());
						::GetWindowThreadProcessId(wndPoint, &processID);
						currentProcessID = ::GetCurrentProcessId();
					}

					if (::IsIconic(skinMini->Wnd()) || !::IsWindowVisible(skinMini->Wnd()) || currentProcessID != processID)
					{
						::SetForegroundWindow(skinMini->Wnd());
						skinMini->SetVisible(true);
					}
					else
						skinMini->SetVisible(false);
				}
			}
			break;
		//case WM_RBUTTONDOWN:
		//case WM_CONTEXTMENU:
		case WM_RBUTTONUP:
			contextMenu.ShowTrayMenu(thisWnd);
			break;
		case WM_MBUTTONDOWN:
			ActionPauseEx();
			break;
		}
	}
	return 0;

	case UWM_POPUPMENU:
	{
		contextMenu.ShowPopupMenu(thisWnd, CPoint(LOWORD(lParam), HIWORD(lParam)), !!::IsWindowEnabled(thisWnd));
	}
	return 0;

	case UWM_SCANEND:
	{
		ScanLibraryFinish(!!wParam);
	}
	return 0;

	case UWM_STOP:
	{
		ActionStop();
	}
	return 0;

	case UWM_COMMAND:
	{
		switch (wParam)
		{
		case CMD_NULL: return 1;
		case CMD_NONE: return 1;
		case CMD_NONE2: return 1;
		case CMD_MAGIC: return 1237;
		case CMD_VER_WINYL: return Settings::winylVersion;
		case CMD_VER_API: return Settings::apiVersion;
		case CMD_PLAY: ActionPlay(); return 1;
		case CMD_PLAY_EX: ActionPlayEx(); return 1;
		case CMD_PAUSE: ActionPause(); return 1;
		case CMD_PAUSE_EX: ActionPauseEx(); return 1;
		case CMD_STOP: ActionStop(); return 1;
		case CMD_NEXT: ActionNextTrack(); return 1;
		case CMD_PREV: ActionPrevTrack(); return 1;
		case CMD_CLOSE: return 1;
		case CMD_CHECK_PLAY: return isMediaPlay ? 1 : 0;
		case CMD_CHECK_PAUSE: return isMediaPause ? 1 : 0;
		case CMD_CHECK_RADIO: return isMediaRadio ? 1 : 0;
		case CMD_GET_VOLUME: return settings.GetVolume() / 1000;
		case CMD_SET_VOLUME: ActionVolume((int)lParam * 1000, true); return 1;
		case CMD_VOLUME_UP: ActionVolumeUp(); return 1;
		case CMD_VOLUME_DOWN: ActionVolumeDown(); return 1;
		case CMD_GET_MUTE: return settings.IsMute() ? 1 : 0;
		case CMD_MUTE_ON: ActionMute(true); return 1;
		case CMD_MUTE_OFF: ActionMute(false); return 1;
		case CMD_MUTE_REV: ActionMute(!settings.IsMute()); return 1;
		case CMD_GET_REPEAT: return settings.IsRepeat() ? 1 : 0;
		case CMD_REPEAT_ON: ActionRepeat(true); return 1;
		case CMD_REPEAT_OFF: ActionRepeat(false); return 1;
		case CMD_REPEAT_REV: ActionRepeat(!settings.IsRepeat()); return 1;
		case CMD_GET_SHUFFLE: return settings.IsShuffle() ? 1 : 0;
		case CMD_SHUFFLE_ON: ActionShuffle(true); return 1;
		case CMD_SHUFFLE_OFF: ActionShuffle(false); return 1;
		case CMD_SHUFFLE_REV: ActionShuffle(!settings.IsShuffle()); return 1;
		case CMD_GET_RATING: return skinList->GetPlayRating();
		case CMD_SET_RATING: if (isMediaPlay) {ActionSetRating((int)lParam, true); return 1;} return 0;
		}
	}
	return 0;

	}

	return ::DefWindowProc(hWnd, message, wParam, lParam);
}

bool WinylWnd::CopySelectedToClipboard(bool isClipboard)
{
	if (skinList->GetSelectedSize() == 0)
		return true;

	if (!isClipboard)
	{
		MyDropSource* myDropSource = new MyDropSource();
		MyDropTarget* myDropTarget = new MyDropTarget();

		myDropTarget->SetDropSource(myDropSource);
		::RegisterDragDrop(thisWnd, myDropTarget);

		size_t bufSize = sizeof(DROPFILES) + sizeof(wchar_t);

		for (std::size_t i = 0, size = skinList->GetSelectedSize(); i < size; ++i)
			bufSize += skinList->GetSelectedAt(i)->GetFile().size() * sizeof(wchar_t) + sizeof(wchar_t);

		HGLOBAL hMem = ::GlobalAlloc(GMEM_ZEROINIT|GMEM_MOVEABLE, bufSize);
		LPVOID buf = ::GlobalLock(hMem);

		((DROPFILES*)buf)->pFiles = sizeof(DROPFILES);
		((DROPFILES*)buf)->fWide = TRUE;

		char* bufCursor = (char*)buf + sizeof(DROPFILES);

		for (std::size_t i = 0, size = skinList->GetSelectedSize(); i < size; ++i)
		{
			size_t sizeFile = skinList->GetSelectedAt(i)->GetFile().size() * sizeof(wchar_t) + sizeof(wchar_t);

			//memcpy(bufCursor, skinList->GetSelectedAt(i)->GetFile().c_str(), sizeFile);
			memcpy_s(bufCursor, sizeFile, skinList->GetSelectedAt(i)->GetFile().c_str(), sizeFile);
			bufCursor += sizeFile;
		}
		memset(bufCursor, 0, sizeof(wchar_t));
		
		::GlobalUnlock(hMem);

		MyDataObject* myDataObject = new MyDataObject();
		myDataObject->hGlobal = hMem;

		DWORD dwEffect;
		::DoDragDrop(myDataObject, myDropSource, DROPEFFECT_COPY, &dwEffect);

		::RevokeDragDrop(thisWnd);

		bool isStopDrag = myDropSource->IsStopDrag();

		myDropTarget->Release(); // Self delete
		myDropSource->Release(); // Self delete
		myDataObject->Release(); // Self delete

		::GlobalFree(hMem);

		if (isStopDrag)
			return false;

		return true;
	}
	else
	{
		// http://blogs.msdn.com/b/oldnewthing/archive/2008/05/27/8553638.aspx

		if (!::OpenClipboard(thisWnd))
			return false;
		if (!::EmptyClipboard())
		{
			::CloseClipboard();
			return false;
		}

		// Copy files to clipboard

		size_t bufSize = sizeof(DROPFILES) + sizeof(wchar_t);

		for (std::size_t i = 0, size = skinList->GetSelectedSize(); i < size; ++i)
			bufSize += skinList->GetSelectedAt(i)->GetFile().size() * sizeof(wchar_t) + sizeof(wchar_t);

		HGLOBAL hMem = ::GlobalAlloc(GMEM_ZEROINIT|GMEM_MOVEABLE, bufSize);
		LPVOID buf = ::GlobalLock(hMem);

		((DROPFILES*)buf)->pFiles = sizeof(DROPFILES);
		((DROPFILES*)buf)->fWide = TRUE;

		char* bufCursor = (char*)buf + sizeof(DROPFILES);

		for (std::size_t i = 0, size = skinList->GetSelectedSize(); i < size; ++i)
		{
			size_t sizeFile = skinList->GetSelectedAt(i)->GetFile().size() * sizeof(wchar_t) + sizeof(wchar_t);

			//memcpy(bufCursor, skinList->GetSelectedAt(i)->GetFile().c_str(), sizeFile);
			memcpy_s(bufCursor, sizeFile, skinList->GetSelectedAt(i)->GetFile().c_str(), sizeFile);
			bufCursor += sizeFile;
		}
		memset(bufCursor, 0, sizeof(wchar_t));
		
		::GlobalUnlock(hMem);

		::SetClipboardData(CF_HDROP, hMem);

		// Also copy focused track Artist - Title to clipboard
		HGLOBAL hMemText = NULL;
		if (skinList->GetFocusNode())
		{
			std::wstring filename, title, album, artist, genre, year;

			if (!skinList->IsRadio())
			{
				dBase.GetSongTags(skinList->GetFocusNode()->idLibrary, skinList->GetFocusNode()->idPlaylist,
								 filename, title, album, artist, genre, year, false);
			}
			else
			{
				if (skinList->GetFocusNode() == skinList->GetPlayNode())
					libAudio.GetRadioTags(title, artist, album);
			}

			if (title.empty())
				title = filename;

			if (!title.empty())
			{
				std::wstring artistTitle = artist.empty() ? title : artist + L" - " + title;

				size_t bufSizeText = artistTitle.size() * sizeof(wchar_t) + sizeof(wchar_t);

				hMemText = ::GlobalAlloc(GMEM_ZEROINIT|GMEM_MOVEABLE, bufSizeText);
				LPVOID bufText = ::GlobalLock(hMemText);

				//memcpy(bufText, artistTitle.c_str(), bufSizeText);
				memcpy_s(bufText, bufSizeText, artistTitle.c_str(), bufSizeText);

				::GlobalUnlock(hMemText);

				::SetClipboardData(CF_UNICODETEXT, hMemText);
			}
		}

		::CloseClipboard();

		::GlobalFree(hMem);
		if (hMemText)
			::GlobalFree(hMemText);

		return true;
	}
}

void WinylWnd::PreTranslateMouseWheel(MSG* msg)
{
	if (msg->message == WM_MOUSEWHEEL)
	{
		if (skinPopup && skinPopup->Wnd() == msg->hwnd) // Just in case
			return;

		bool isReverse = GET_WHEEL_DELTA_WPARAM(msg->wParam) > 0 ? true : false;

		if (skinMini && skinMini->Wnd() == msg->hwnd)
		{
			if (isReverse)
				ActionVolumeUp();
			else
				ActionVolumeDown();
		}
		else
		{
			CPoint pt(GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam));
			::ScreenToClient(thisWnd, &pt);
			HWND wndPoint = ::ChildWindowFromPointEx(thisWnd, pt, CWP_SKIPINVISIBLE);

			if (wndPoint == skinList->Wnd())
			{
				skinList->MouseWheel(isReverse);
			}
			else if (wndPoint == skinTree->Wnd())
			{
				skinTree->MouseWheel(isReverse);
			}
			else if (wndPoint == skinLyrics->Wnd())
			{
				skinLyrics->MouseWheel(isReverse);
			}
			else
			{
				if (isReverse)
					ActionVolumeUp();
				else
					ActionVolumeDown();
			}
		}
	}
}

void WinylWnd::PreTranslateRelayEvent(MSG* msg)
{
	toolTips.RelayEvent(msg);
	if (skinAlpha)
		skinAlpha->toolTips.RelayEvent(msg);
}

void WinylWnd::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TimerValue::FadeID)
	{
		bool isStop = true;

		if (!skinDraw.FadeElement())
			isStop = false;

		if (skinAlpha)
		{
			if (!skinAlpha->skinDraw.FadeElement())
				isStop = false;
		}

		if (skinMini)
		{
			if (!skinMini->skinDraw.FadeElement())
				isStop = false;
		}

		if (isStop)
		{
			::KillTimer(thisWnd, TimerValue::FadeID);
			//::MessageBeep(1);
		}
	}
	else if (nIDEvent == TimerValue::VisID)
	{
		bool isMainWnd = !::IsIconic(thisWnd) && ::IsWindowVisible(thisWnd);
		bool isMiniWnd = skinMini && !::IsIconic(skinMini->Wnd()) && ::IsWindowVisible(skinMini->Wnd());

		if (!isMainWnd && !isMiniWnd)
		{
			// Always kill the timer if all windows are hidded for example
			if (!isMediaPlay || isMediaPause)
			{
				::KillTimer(thisWnd, TimerValue::VisID);
				//::MessageBeep(1);
			}
		}
		else
		{
			// Get FFT data and draw it in visualizers

			float fft[1024];
			libAudio.GetFFT(fft);
			
			bool needStop = true;

			// if (isMainWnd)
			{
				for (std::size_t i = 0, size = visuals.size(); i < size; ++i)
				{
					if (!visuals[i]->SetFFT((isMediaPlay && !isMediaPause) ? fft : nullptr, isMediaPause))
						needStop = false;
				}
			}

			// if (isMiniWnd)
			if (skinMini)
			{
				for (std::size_t i = 0, size = skinMini->visuals.size(); i < size; ++i)
				{
					if (!skinMini->visuals[i]->SetFFT((isMediaPlay && !isMediaPause) ? fft : nullptr, isMediaPause))
						needStop = false;
				}
			}

			if (needStop)
			{
				::KillTimer(thisWnd, TimerValue::VisID);
				//::MessageBeep(1);
			}
		}
	}
	else if (nIDEvent == TimerValue::PosID)
	{
		skinDraw.DrawPosition(libAudio.GetPosition());
	}
	else if (nIDEvent == TimerValue::TimeID)
	{		
		skinDraw.DrawTime(libAudio.GetTimePosition(), libAudio.GetTimeLength(), false);
	}
	else if (nIDEvent == TimerValue::TrackID)
	{
		isTrackTooltip = true;
		::KillTimer(thisWnd, TimerValue::TrackID);

		if (!skinDraw.IsPressElement() && skinDraw.GetHoverElement() && skinDraw.GetHoverElement()->type == SkinElement::Type::Track)
		{
			POINT point = {};
			::GetCursorPos(&point);
			::ScreenToClient(thisWnd, &point);
			int percent = static_cast<SkinSlider*>(skinDraw.GetHoverElement())->CalcPercent(point);
			int pos = (percent * libAudio.GetTimeLength() + 100000 / 2) / 100000;
			std::wstring time = SkinText::TimeString(pos, false, libAudio.GetTimeLength(), true);
			toolTips.TrackingToolTip(true, time, point.x, skinDraw.GetHoverElement()->rcRect.top, true);
		}
	}
}

void WinylWnd::OnContextMenu(HWND hWnd, CPoint point)
{
	if (hWnd == thisWnd)
	{
		if (point.x == -1 && point.y == -1) // Menu key or Shift+F10
			::GetCursorPos(&point);

		contextMenu.ShowMainMenu(thisWnd, point);
	}
}

void WinylWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// "Add to Playlist" menu item
	if (LOWORD(wParam) >= 10000 && LOWORD(wParam) <= 12000)
	{
		std::wstring file; // Playlist db file

		if (LOWORD(wParam) == 10000) // "To New Playlist" menu item
		{
			TreeNodeUnsafe node = dBase.CreatePlaylist(skinTree.get(), lang.GetLineS(Lang::DefaultList, 0) +
				L" " + StringCurrentDate());

			if (node)
				file = node->GetValue();
		}
		else if (LOWORD(wParam) == 10001) // "To Now Playing" menu item
		{
			if (settings.GetNowPlayingType() == (int)SkinTreeNode::Type::Playlist)
				file = settings.GetNowPlayingValue();
		}
		else
		{
			// Let's find out to which playlist we add tracks
			TreeNodeUnsafe plsNode = skinTree->GetPlaylistNode();
			if (plsNode->HasChild())
			{
				int index = LOWORD(wParam) - 10002;

				TreeNodeUnsafe treeNode = plsNode->ChildByIndex(index);
				if (treeNode)
					file = treeNode->GetValue();
			}
		}

		// Add to the playlist
		if (!file.empty())
		{
			BeginWaitCursor();

			int start = 0;

			if (dBase.IsPlaylistOpen())
				start = dBase.FromPlaylistToPlaylist(skinList.get(), file);
			else if (skinList->IsRadio())
				start = dBase.FromRadioToPlaylist(skinList.get(), file);
			else
				start = dBase.FromLibraryToPlaylist(skinList.get(), file);

			if (settings.IsLibraryNowPlaying())
			{
				// If add to a now playing playlist then need to fill the playlist right away
				if (dBase.IsNowPlaying() && !dBase.IsNowPlayingOpen())
				{
					if (settings.GetNowPlayingType() == (int)SkinTreeNode::Type::Playlist &&
						settings.GetNowPlayingValue() == file)
						dBase.FillPlaylistNowPlaying(skinList.get(), settings.GetNowPlayingValue(), start);
				}
			}

			EndWaitCursor();
		}

		return;
	}

	switch(LOWORD(wParam))
	{
	case ID_MENU_LIBRARY:
	case ID_KEY_F3:
		DialogLibrary();
		break;
	case ID_MENU_SKIN:
	case ID_KEY_F4:
		{
			EnableAll(false);
			DlgSkin dlg;
			dlg.SetLanguage(&lang);
			dlg.SetProgramPath(programPath);
			dlg.SetCurrentSkin(settings.GetSkin(), settings.IsSkinPack());
			dlg.ModalDialog(thisWnd, IDD_DLGSKIN);
			EnableAll(true);
		}
		break;
	case ID_MENU_LANGUAGE:
		DialogLanguage();
		break;
	case ID_MENU_PROPERTIES:
		DialogProperties();
		break;
	case ID_MENU_HOTKEY:
	case ID_KEY_F7:
		{
			EnableAll(false);
			hotKeys.UnregisterHotKeys(thisWnd);
			DlgHotKeys dlg;
			dlg.SetLanguage(&lang);
			dlg.SetHotKeys(&hotKeys);
			dlg.ModalDialog(thisWnd, IDD_DLGHOTKEYS);
			hotKeys.RegisterHotKeys(thisWnd);
			EnableAll(true);
		}
		break;
	case ID_MENU_ABOUT:
		{
			EnableAll(false);
			DlgAbout dlg;
			dlg.SetLanguage(&lang);
			dlg.ModalDialog(thisWnd, IDD_DLGABOUT);
			EnableAll(true);
		}
		break;
	case ID_MENU_EQUALIZER:
	case ID_KEY_F6:
		DialogEqualizer();
		break;
	case ID_MENU_STOP:
	case ID_KEY_CTRL_SPACE:
		ActionStop();
		break;
	case ID_MENU_NEXT:
	case ID_KEY_CTRL_RIGHT:
		ActionNextTrack();
		break;
	case ID_MENU_PREV:
	case ID_KEY_CTRL_LEFT:
		ActionPrevTrack();
		break;
	case ID_MENU_PAUSE:
	case ID_KEY_SPACE:
		ActionPauseEx();
		break;
	case ID_MENU_PLAY:
		ActionPlayEx();
		break;
	case ID_MENU_LIST_PLAY:
	case ID_KEY_ENTER:
		ActionPlay(true);
		break;
	case ID_MENU_LIST_REPEAT:
		ActionPlay(true, false, true);
		break;
	case ID_MENU_EXIT:
		if (dlgProgressPtr)
			dlgProgressPtr->DestroyOnStop();
		else
			::DestroyWindow(thisWnd);
		break;
	case ID_MENU_HIDETOTRAY:
		settings.SetHideToTray(!settings.IsHideToTray());
		trayIcon.SetHideToTray(settings.IsHideToTray());
		contextMenu.CheckHideToTray(settings.IsHideToTray());
		break;
	case ID_MENU_MUTE:
	case ID_KEY_MUTE:
		ActionMute(!settings.IsMute());
		break;
	case ID_MENU_SHUFFLE:
	case ID_KEY_SHUFFLE:
		ActionShuffle(!settings.IsShuffle());
		break;
	case ID_MENU_REPEAT:
	case ID_KEY_REPEAT:
		ActionRepeat(!settings.IsRepeat());
		break;
	case ID_MENU_MINI:
	case ID_KEY_F5:
		MiniPlayer(!settings.IsMiniPlayer());
		break;
	case ID_MENU_PL_NEW:
		NewPlaylist(false);
		break;
	case ID_MENU_PL_DELETE:
		DeletePlaylist(false);
		break;
	case ID_MENU_PL_RENAME:
		RenamePlaylist(false);
		break;
	case ID_MENU_SM_NEW:
		NewPlaylist(true);
		break;
	case ID_MENU_SM_DELETE:
		DeletePlaylist(true);
		break;
	case ID_MENU_SM_RENAME:
		RenamePlaylist(true);
		break;
	case ID_MENU_SM_EDIT:
		EditSmartlist();
		break;
	case ID_MENU_OPEN_URL:
	case ID_KEY_PL_URL:
		AddURL();
		break;
	case ID_MENU_DELETE:
	case ID_KEY_DELETE:
		if (::GetFocus() == skinList->Wnd())
		{
			BeginWaitCursor();
			if (dBase.IsPlaylistOpen())
				dBase.DeleteFromPlaylist(skinList.get());
			else if (dBase.IsSmartlistOpen())
				dBase.DeleteFromSmartlist(skinList.get(), settings.GetLibraryValue());
			else
			{
				dBase.DeleteFromLibrary(skinList.get(), settings.IsAddAllToLibrary() ? &libraryFolders : nullptr);
				skinTree->ClearLibrary();
			}
			EndWaitCursor();

			UpdateStatusLine();

			if (isMediaPlay && skinList->GetPlayNode() == nullptr)
				ActionStop();
		}
		break;
	case ID_MENU_OPEN_FILE:
	case ID_KEY_PL_FILE:
		AddFileFolder(false);
		break;
	case ID_MENU_OPEN_FOLDER:
	case ID_KEY_PL_FOLDER:
		AddFileFolder(true);
		break;
	case ID_MENU_OPEN_PLAYLIST:
		ImportPlaylist();
		break;
	case ID_MENU_SELECT_ALL:
	case ID_KEY_SELECT_ALL:
		skinList->SelectAll();
		UpdateStatusLine();
		break;
	case ID_MENU_POPUP:
		settings.SetPopup(!settings.IsPopup());
		contextMenu.CheckPopup(settings.IsPopup());
		break;
	case ID_MENU_POPUP_CONFIG:
		DialogConfig(2);
		break;
	case ID_MENU_POPUP_DISABLE:
		if (skinPopup)
		{
			skinPopup->Disable();
		}
		settings.SetPopup(false);
		contextMenu.CheckPopup(false);
		break;
	case ID_MENU_POPUP_TL:
		if (skinPopup)
		{
			skinPopup->SetPosition(0);
			skinPopup->Popup();
		}
		settings.SetPopupPosition(0);
		contextMenu.CheckPopupPosition(0);
		break;
	case ID_MENU_POPUP_TR:
		if (skinPopup)
		{
			skinPopup->SetPosition(1);
			skinPopup->Popup();
		}
		settings.SetPopupPosition(1);
		contextMenu.CheckPopupPosition(1);
		break;
	case ID_MENU_POPUP_BL:
		if (skinPopup)
		{
			skinPopup->SetPosition(2);
			skinPopup->Popup();
		}
		settings.SetPopupPosition(2);
		contextMenu.CheckPopupPosition(2);
		break;
	case ID_MENU_POPUP_BR:
		if (skinPopup)
		{
			skinPopup->SetPosition(3);
			skinPopup->Popup();
		}
		settings.SetPopupPosition(3);
		contextMenu.CheckPopupPosition(3);
		break;
	case ID_MENU_EFFECT:
		settings.SetEffect(!settings.IsEffect());
		contextMenu.CheckEffect(settings.IsEffect());
		skinDraw.EnableFade(settings.IsEffect());
		if (skinAlpha)
			skinAlpha->skinDraw.EnableFade(settings.IsEffect());
		if (skinMini)
			skinMini->skinDraw.EnableFade(settings.IsEffect());
		break;
	case ID_MENU_SMOOTH:
		settings.SetSmoothScroll(!settings.IsSmoothScroll());
		contextMenu.CheckSmoothScroll(settings.IsSmoothScroll());
		skinList->EnableSmoothScroll(settings.IsSmoothScroll());
		skinTree->EnableSmoothScroll(settings.IsSmoothScroll());
		skinLyrics->EnableSmoothScroll(settings.IsSmoothScroll());
		if (settings.IsSmoothScroll())
		{
			isTimerSmooth = false;
			eventTimerSmooth.Reset();
			threadTimerSmooth.Start(std::bind(&WinylWnd::TimerEffectsThreadRun, this));
		}
		else
		{
			if (threadTimerSmooth.IsJoinable())
			{
				isTimerSmooth = true;
				eventTimerSmooth.Set();
				threadTimerSmooth.Join();
			}
		}
	case ID_MENU_SEARCH_ALL:
		settings.SetSearchType(0);
		contextMenu.CheckSearch(0);
		skinEdit->SetStandartText(lang.GetLineS(Lang::Search, 0));
		if (!skinEdit->IsSearchEmpty())
			FillListSearch();
		break;
	case ID_MENU_SEARCH_TRACK:
		settings.SetSearchType(1);
		contextMenu.CheckSearch(1);
		skinEdit->SetStandartText(lang.GetLineS(Lang::Search, 2));
		if (!skinEdit->IsSearchEmpty())
			FillListSearch();
		break;
	case ID_MENU_SEARCH_ALBUM:
		settings.SetSearchType(2);
		contextMenu.CheckSearch(2);
		skinEdit->SetStandartText(lang.GetLineS(Lang::Search, 3));
		if (!skinEdit->IsSearchEmpty())
			FillListSearch();
		break;
	case ID_MENU_SEARCH_ARTIST:
		settings.SetSearchType(3);
		contextMenu.CheckSearch(3);
		skinEdit->SetStandartText(lang.GetLineS(Lang::Search, 4));
		if (!skinEdit->IsSearchEmpty())
			FillListSearch();
		break;
	case ID_MENU_JUMPTO_ARTIST:
		FillJump(skinList->GetFocusNode(), SkinTreeNode::Type::Artist);
		break;
	case ID_MENU_JUMPTO_ALBUM:
		FillJump(skinList->GetFocusNode(), SkinTreeNode::Type::Album);
		break;
	case ID_MENU_JUMPTO_YEAR:
		FillJump(skinList->GetFocusNode(), SkinTreeNode::Type::Year);
		break;
	case ID_MENU_JUMPTO_GENRE:
		FillJump(skinList->GetFocusNode(), SkinTreeNode::Type::Genre);
		break;
	case ID_MENU_JUMPTO_FOLDER:
		FillJump(skinList->GetFocusNode(), SkinTreeNode::Type::Folder);
		break;
	case ID_MENU_LYRICS_ALIGN_CENTER:
		settings.SetLyricsAlign(0);
		skinLyrics->SetAlign(settings.GetLyricsAlign());
		contextMenu.CheckLyricsAlign(settings.GetLyricsAlign());
		break;
	case ID_MENU_LYRICS_ALIGN_LEFT:
		settings.SetLyricsAlign(1);
		skinLyrics->SetAlign(settings.GetLyricsAlign());
		contextMenu.CheckLyricsAlign(settings.GetLyricsAlign());
		break;
	case ID_MENU_LYRICS_ALIGN_RIGHT:
		settings.SetLyricsAlign(2);
		skinLyrics->SetAlign(settings.GetLyricsAlign());
		contextMenu.CheckLyricsAlign(settings.GetLyricsAlign());
		break;
	case ID_MENU_LYRICS_FONT_DEFAULT:
		settings.SetLyricsFontSize(0);
		settings.SetLyricsFontBold(skinLyrics->GetFontBoldDefault());
		skinLyrics->SetFontSize(settings.GetLyricsFontSize(), settings.GetLyricsFontBold());
		contextMenu.CheckLyricsFontSize(settings.GetLyricsFontSize());
		contextMenu.CheckLyricsFontBold(settings.GetLyricsFontBold());
		break;
	case ID_MENU_LYRICS_FONT_SMALL:
		settings.SetLyricsFontSize(1);
		skinLyrics->SetFontSize(settings.GetLyricsFontSize(), settings.GetLyricsFontBold());
		contextMenu.CheckLyricsFontSize(settings.GetLyricsFontSize());
		break;
	case ID_MENU_LYRICS_FONT_MEDIUM:
		settings.SetLyricsFontSize(2);
		skinLyrics->SetFontSize(settings.GetLyricsFontSize(), settings.GetLyricsFontBold());
		contextMenu.CheckLyricsFontSize(settings.GetLyricsFontSize());
		break;
	case ID_MENU_LYRICS_FONT_LARGE:
		settings.SetLyricsFontSize(3);
		skinLyrics->SetFontSize(settings.GetLyricsFontSize(), settings.GetLyricsFontBold());
		contextMenu.CheckLyricsFontSize(settings.GetLyricsFontSize());
		break;
	case ID_MENU_LYRICS_FONT_LARGEST:
		settings.SetLyricsFontSize(4);
		skinLyrics->SetFontSize(settings.GetLyricsFontSize(), settings.GetLyricsFontBold());
		contextMenu.CheckLyricsFontSize(settings.GetLyricsFontSize());
		break;
	case ID_MENU_LYRICS_FONT_BOLD:
		settings.SetLyricsFontBold(!settings.GetLyricsFontBold());
		skinLyrics->SetFontSize(settings.GetLyricsFontSize(), settings.GetLyricsFontBold());
		contextMenu.CheckLyricsFontBold(settings.GetLyricsFontBold());
		break;
	case ID_MENU_LYRICS_PROV_OFF:
		settings.SetLyricsProviderOff(true);
		settings.SetLyricsProvider(L"");
		contextMenu.CheckLyricsProvider(-1);
		break;
	case ID_MENU_LYRICS_PROV_1:
		settings.SetLyricsProviderOff(false);
		settings.SetLyricsProvider(L"");
		contextMenu.CheckLyricsProvider(0);
		if (isMediaPlay && !isMediaRadio)
		{
			isThreadLyrics = true;
			LyricsThreadStart();
		}
		break;
	case ID_MENU_LYRICS_PROV_2:
	case ID_MENU_LYRICS_PROV_3:
	case ID_MENU_LYRICS_PROV_4:
	case ID_MENU_LYRICS_PROV_5:
	case ID_MENU_LYRICS_PROV_6:
	case ID_MENU_LYRICS_PROV_7:
	case ID_MENU_LYRICS_PROV_8:
	case ID_MENU_LYRICS_PROV_9:
	case ID_MENU_LYRICS_PROV_10:
		settings.SetLyricsProviderOff(false);
		settings.SetLyricsProvider(LyricsLoader::GetLyricsProvider(LOWORD(wParam) - ID_MENU_LYRICS_PROV_1));
		contextMenu.CheckLyricsProvider(LOWORD(wParam) - ID_MENU_LYRICS_PROV_1);
		if (isMediaPlay && !isMediaRadio)
		{
			isThreadLyrics = true;
			LyricsThreadStart();
		}
		break;
	case ID_MENU_LYRICS_GOOGLE:
		if (isMediaPlay && !isMediaRadio)
		{
			if (!lyricsArtist.empty() && !lyricsTitle.empty())
			{
				//std::wstring url = L"https://google.com/search?q=%22" + lyricsArtist + L"%22+%22" + lyricsTitle + L"%22+lyrics";
				std::wstring url = L"https://google.com/search?q=" + lyricsArtist + L"+" + lyricsTitle + L"+lyrics";
				::ShellExecute(NULL, L"open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
			}
		}
		break;
	case ID_MENU_LYRICS_SAVE:
		DialogProperties(true);
		break;
	case ID_MENU_LYRICS_COPY:
		if (::OpenClipboard(thisWnd))
		{
			if (::EmptyClipboard())
			{
				std::wstring lyrics = skinLyrics->GetLyrics();

				size_t bufSizeText = lyrics.size() * sizeof(wchar_t) + sizeof(wchar_t);

				HGLOBAL hMemText = ::GlobalAlloc(GMEM_ZEROINIT|GMEM_MOVEABLE, bufSizeText);
				if (hMemText)
				{
					LPVOID bufText = ::GlobalLock(hMemText);

					//memcpy(bufText, lyrics.c_str(), bufSizeText);
					memcpy_s(bufText, bufSizeText, lyrics.c_str(), bufSizeText);

					::GlobalUnlock(hMemText);

					::SetClipboardData(CF_UNICODETEXT, hMemText);
					::GlobalFree(hMemText);
				}
			}
			::CloseClipboard();
		}
		break;
	case ID_MENU_FILELOCATION:
		{
			ListNodeUnsafe focusNode = skinList->GetFocusNode();
			if (focusNode)
			{
				//std::wstring operation = L"/select, \"" + focusNode->GetFile() + L"\"";
				//::ShellExecute(thisWnd, L"open", L"Explorer", operation.c_str(), NULL, SW_SHOWNORMAL);

				// Here we fix a problem, when url then IE opens and Winyl hangs when close.
				if (focusNode->GetFile().size() > 2 && focusNode->GetFile()[1] == ':' &&
					FileSystem::Exists(focusNode->GetFile()))
				{
					LPITEMIDLIST pidl = NULL;// = ::ILCreateFromPath(focusNode->GetFile().c_str());
					SFGAOF attrIn = SFGAO_FILESYSTEM;
					SFGAOF attrOut = 0;
					::SHParseDisplayName(focusNode->GetFile().c_str(), NULL, &pidl, attrIn, &attrOut);
					if (pidl)
					{
						if (attrOut & SFGAO_FILESYSTEM) // Additional fix for the url bug
							::SHOpenFolderAndSelectItems(pidl, 0, 0, 0);
						::CoTaskMemFree(pidl);//::ILFree(pidl);
					}
				}
			}
		}
		break;
//	case ID_MENU_LASTFM:
//		if (!settings.IsLastFM())
//		{
//			settings.SetLastFM(lastFM.Init());

//			if (settings.IsLastFM())
//				contextMenu.CheckLastFM(true);
//			else
//			{
//				EnableAll(false);
//				MessageBox::Show(thisWnd, lang.GetLine(Lang::MESSAGE, 1), lang.GetLine(Lang::MESSAGE, 7), MessageBox::IconWarning);

//				EnableAll(true);
//			}
//		}
//		else
//		{
//			lastFM.Stop();
//			settings.SetLastFM(false);
//			contextMenu.CheckLastFM(false);
//		}
//		break;
	case ID_MENU_CONFIG:
	case ID_KEY_F8:
		DialogConfig();
		break;
	case ID_MENU_SM_UPDATE:
		if (skinTree->GetFocusNode())
		{
			BeginWaitCursor();
			dBase.FillSmartlist(skinList.get(), skinTree->GetFocusNode()->GetValue(), true);
			EndWaitCursor();
			UpdateStatusLine();
		}
		break;
	case ID_KEY_CTRL_C:
	case ID_KEY_CTRL_INS:
		if (::GetFocus() == skinList->Wnd())
			CopySelectedToClipboard(true);
		break;
	case ID_KEY_CTRL_F:
		skinEdit->SetFocus();
		break;
	}
}

void WinylWnd::NewPlaylist(bool isSmartlist)
{
	if (!isSmartlist)
	{
		TreeNodeUnsafe node = dBase.CreatePlaylist(skinTree.get(), lang.GetLineS(Lang::DefaultList, 0) + L" " + StringCurrentDate());
		skinTree->ScrollToMyNode(node);
	}
	else
	{
		EnableAll(false);
		DlgSmart dlg;
		dlg.SetLanguage(&lang);
		dlg.SetName(lang.GetLineS(Lang::DefaultList, 1) + L" " + StringCurrentDate());
		if (dlg.ModalDialog(thisWnd, IDD_DLGSMART) == IDOK)
		{
			BeginWaitCursor();
			TreeNodeUnsafe node = dBase.CreateSmartlist(skinTree.get(), dlg.GetName(), dlg.GetSmartlist());
			if (node)
			{
				skinTree->SetFocusNode(node);
				skinTree->ScrollToFocusNode();
				dBase.FillSmartlist(skinList.get(), skinTree->GetFocusNode()->GetValue(), true);
				UpdateStatusLine();
			}
			EndWaitCursor();
		}
		EnableAll(true);
	}
}

void WinylWnd::DeletePlaylist(bool isSmartlist)
{
	if (skinTree->GetFocusNode() == nullptr)
		return;

	if (!isSmartlist)
	{
		if (!settings.IsLibraryNowPlaying())
		{
			// If delete the current playlist
			if (settings.GetLibraryType() == (int)SkinTreeNode::Type::Playlist &&
				settings.GetLibraryValue() == skinTree->GetFocusNode()->GetValue())
			{
				dBase.ClosePlaylist();
				dBase.CloseNowPlaying();
			}
		}
		else
		{
			// If delete the current playlist
			if (settings.GetLibraryType() == (int)SkinTreeNode::Type::Playlist &&
				settings.GetLibraryValue() == skinTree->GetFocusNode()->GetValue())
			{
				dBase.ClosePlaylist();
			}
			// If delete the now playing playlist
			if (dBase.IsNowPlaying())
			{
				if (settings.GetNowPlayingType() == (int)SkinTreeNode::Type::Playlist &&
					settings.GetNowPlayingValue() == skinTree->GetFocusNode()->GetValue())
				{
					dBase.CloseNowPlaying();
					skinList->DeleteNowPlaying();
					settings.EmptyNowPlaying(); // Needed or delete the default playlist cause a bug
					ActionStop();
				}
			}
		}

		// Delete playlist
		dBase.DeletePlaylist(skinTree.get());
	}
	else
	{
		// Delete smartlist
		dBase.DeleteTreeSmartlist(skinTree.get());
	}

	// Needed or delete the last playlist cause a bug
	settings.EmptyLibrary();

	// Choose the next playlist or clear the list if latest
	if (skinTree->GetFocusNode())
		FillList(skinTree->GetFocusNode());
	else
	{
		skinList->SetControlRedraw(false);
		skinList->DeleteAllNode();
		skinList->SetControlRedraw(true);
		UpdateStatusLine();
	}
}

void WinylWnd::RenamePlaylist(bool isSmartlist)
{
	if (skinTree->GetFocusNode() == nullptr)
		return;

	EnableAll(false);

	DlgRename dlg;
	dlg.SetLanguage(&lang);
	dlg.SetName(skinTree->GetNodeTitle(skinTree->GetFocusNode()));
	if (dlg.ModalDialog(thisWnd, IDD_DLGRENAME) == IDOK)
	{
		skinTree->SetNodeTitle(skinTree->GetFocusNode(), dlg.GetName());

		if (!isSmartlist)
			dBase.SavePlaylist(skinTree.get());
		else
			dBase.SaveTreeSmartlists(skinTree.get());
	}

	EnableAll(true);
}

void WinylWnd::EditSmartlist()
{
	if (skinTree->GetFocusNode())
	{
		EnableAll(false);
		DlgSmart dlg;
		dlg.SetLanguage(&lang);
		dlg.SetName(skinTree->GetNodeTitle(skinTree->GetFocusNode()));
		dBase.OpenSmartlist(skinTree->GetFocusNode()->GetValue(), dlg.GetSmartlist());
		if (dlg.ModalDialog(thisWnd, IDD_DLGSMART) == IDOK)
		{
			BeginWaitCursor();
			dBase.SaveSmartlist(skinTree->GetFocusNode()->GetValue(), dlg.GetSmartlist());
			dBase.FillSmartlist(skinList.get(), skinTree->GetFocusNode()->GetValue(), true);
			
			skinTree->SetNodeTitle(skinTree->GetFocusNode(), dlg.GetName());
			dBase.SaveTreeSmartlists(skinTree.get());
			UpdateStatusLine();
			EndWaitCursor();
		}
		EnableAll(true);
	}
}

void WinylWnd::EnableAll(bool isEnable)
{
	if (skinAlpha)
	{
		if (!isEnable) ::SetFocus(thisWnd); // Hack to always show centered around the main window

		if (skinAlpha->IsWnd())
			::EnableWindow(skinAlpha->Wnd(), isEnable);
	}

	if (skinMini)
	{
		if (skinMini->IsWnd())
			::EnableWindow(skinMini->Wnd(), isEnable);
	}
}

void WinylWnd::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	if (skinDraw.IsShadowNative() && !skinShadow && futureWin->IsCompositionEnabled())
	{
		bool isShowActivate =
				(lpwndpos->flags & SWP_SHOWWINDOW) ||
				!(lpwndpos->flags & SWP_NOACTIVATE) ||
				((lpwndpos->flags & SWP_NOMOVE) && (lpwndpos->flags & SWP_NOSIZE) &&
				!(lpwndpos->flags & SWP_FRAMECHANGED)); // !(lpwndpos->flags & SWP_NOZORDER)

		if (isShowActivate)
			skinDraw.EnableDwmShadow();
	}

	if (skinAlpha)
	{
		bool isMove = !(lpwndpos->flags & SWP_NOMOVE);
		bool isSize = !(lpwndpos->flags & SWP_NOSIZE);
		bool isShow = !!(lpwndpos->flags & SWP_SHOWWINDOW);

		if (isMove || isSize || isShow)
		{
			CRect rc = skinDraw.GetAlphaBorder();
			::MoveWindow(skinAlpha->Wnd(), lpwndpos->x - rc.left, lpwndpos->y - rc.top,
				lpwndpos->cx + rc.left + rc.right, lpwndpos->cy + rc.top + rc.bottom, FALSE);
		}
	}

	if (skinShadow)
	{
		bool isMove = !(lpwndpos->flags & SWP_NOMOVE);
		bool isSize = !(lpwndpos->flags & SWP_NOSIZE);
		bool isShow = !!(lpwndpos->flags & SWP_SHOWWINDOW);

		if (isMove || isSize || isShow)
			skinShadow->Move(lpwndpos->x, lpwndpos->y, lpwndpos->cx, lpwndpos->cy);
	}
}

void WinylWnd::StopDragDrop()
{
	if (isDragDropOLE)
		return;

	isDragDrop = false;
	wndDragIcon.HideIcon();

	skinList->SetDropMoveStop();
	skinTree->SetDropMoveStop();
	skinTree->SetDropPoint(nullptr);
}

void WinylWnd::DropFilesToPlaylist()
{
	// Drop to a selected playlist

	if (skinTree->GetDropNode()) // Only if drop from the player
	{
		TreeNodeUnsafe dropNode = skinTree->GetDropNode();
		std::wstring file;

		// Drop to "Now Playing"
		if (dropNode->GetNodeType() == SkinTreeNode::NodeType::Head && dropNode->GetType() == SkinTreeNode::Type::NowPlaying)
		{
			if (settings.GetNowPlayingType() == (int)SkinTreeNode::Type::Playlist)
				file = settings.GetNowPlayingValue();
		}
		else // Drop to a playlist
		{
			// Cannot drop to the same playlist
			if (!(dBase.IsPlaylistOpen() && settings.GetLibraryValue() == dropNode->GetValue()))
				file = dropNode->GetValue();
		}

		skinTree->SetDropPoint(nullptr);

		if (file.empty())
			return;

		BeginWaitCursor();

		int start = 0;

		if (dBase.IsPlaylistOpen())
			start = dBase.FromPlaylistToPlaylist(skinList.get(), file);
		else if (skinList->IsRadio())
			start = dBase.FromRadioToPlaylist(skinList.get(), file);
		else
			start = dBase.FromLibraryToPlaylist(skinList.get(), file);

		if (settings.IsLibraryNowPlaying())
		{
			// If drop to a now playing playlist then need to fill the playlist right away
			if (dBase.IsNowPlaying() && !dBase.IsNowPlayingOpen())
			{
				if (settings.GetNowPlayingType() == (int)SkinTreeNode::Type::Playlist &&
					settings.GetNowPlayingValue() == file)
					dBase.FillPlaylistNowPlaying(skinList.get(), settings.GetNowPlayingValue(), start);
			}
		}

		EndWaitCursor();
	}
}

void WinylWnd::OnDropFiles(HDROP hDropInfo)
{
	if (dlgProgressPtr) // Do not add if the library is scanning
		return;

	// Drop to the current open playlist

	std::vector<std::wstring> files;

	UINT numberFiles = ::DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);				
	for (UINT i = 0; i < numberFiles; ++i)
	{
		wchar_t file[MAX_PATH * 16];
		if (::DragQueryFile(hDropInfo, i, file, sizeof(file) / sizeof(wchar_t)))
			files.push_back(file);
	}
	::DragFinish(hDropInfo);

	bool isImportPlaylist = DropIsPlaylist(files);
	bool isDefaultPlaylist = false;

	// Playlist is closed then drop to the default playlist
	if (!dBase.IsPlaylistOpen() && !isImportPlaylist)
	{
		DropNewDefault();
		isDefaultPlaylist = true;
	}

	if (!isImportPlaylist)
		DropAddFiles(files, false, isDefaultPlaylist);
	else
		DropAddPlaylist(files, false);
}

bool WinylWnd::DropIsPlaylist(const std::vector<std::wstring>& files)
{
	if (files.size() == 1)
	{
		std::wstring ext = PathEx::ExtFromFile(files[0]);

		if (ext == L"m3u" || ext == L"m3u8" || ext == L"pls" || ext == L"asx" || ext == L"xspf")
		{
			return true;
		}
	}

	return false;
}

void WinylWnd::DropNewDefault()
{
	// If there is no default playlist then create one
	if (skinTree->GetDefPlaylistNode() == nullptr)
		dBase.CreatePlaylist(skinTree.get(), lang.GetLineS(Lang::DefaultList, 2), true);

	if (skinTree->GetDefPlaylistNode())
	{
		// Select the default playlist
//		skinTree->SetFocusNode(skinTree->GetDefPlaylistNode());
//		skinTree->ScrollToFocusNode();

		// Choose the default playlist if it's not choosen
		if (settings.GetLibraryNoneOld() ||
			!(settings.GetLibraryType() == (int)SkinTreeNode::Type::Playlist &&
			settings.GetLibraryValue() == L"default") || !pointerOpenFiles->empty())
			FillList(skinTree->GetDefPlaylistNode());
	}
}

void WinylWnd::DropAddFiles(const std::vector<std::wstring>& files, bool isPlay, bool isDefaultPlaylist)
{
	assert(!dlgProgressPtr);
	assert(settings.GetLibraryType() == (int)SkinTreeNode::Type::Playlist);
	if (settings.GetLibraryType() != (int)SkinTreeNode::Type::Playlist)
		return;

	skinList->RemoveSelection();
	if (settings.IsPlayFocus())
		skinList->EnablePlayFocus(false);

	// Add files from this position
	int start = dBase.GetPlaylistMax();

	bool isFolder = false;

	// If only 1 file then try to add it fast
	if (files.size() == 1)
	{
		Progress progress;
		progress.SetDataBase(&dBase);
		progress.SetPortableVersion(isPortableVersion);
		progress.SetProgramPath(programPath);
		progress.SetAddAllToLibrary(settings.IsAddAllToLibrary());
		
		if (progress.FastAddFileToPlaylist(files[0], start, isFolder))
		{
			dBase.FillPlaylistOpenFile(skinList.get(), settings.GetLibraryValue(), start);
		}
	}

	// Multiple files or folder
	if (isFolder || files.size() > 1)
	{
		EnableAll(false);

		DlgProgress dlgProgress;
		dlgProgress.SetLanguage(&lang);
		dlgProgress.progress.SetDataBase(&dBase);
		dlgProgress.progress.SetPortableVersion(isPortableVersion);
		dlgProgress.progress.SetProgramPath(programPath);
		dlgProgress.progress.SetPlaylist(start, settings.GetLibraryValue());
		dlgProgress.progress.SetLibraryFolders(files);
		dlgProgress.progress.SetAddAllToLibrary(settings.IsAddAllToLibrary());
		dlgProgress.ModalDialog(thisWnd, IDD_DLGPROGRESS);

		EnableAll(true);

		BeginWaitCursor();
		dBase.FillPlaylistOpenFile(skinList.get(), settings.GetLibraryValue(), start);
		EndWaitCursor();
	}

	if (isPlay) // ActionPlay
	{
		if (skinList->GetSelectedSize() > 0)
		{
			ListNodeUnsafe node = skinList->GetSelectedAt(0); // Get first selected

			skinList->SetFocusNode(node, false); // Focus it

			skinList->ScrollToMyNode(node); // Scroll to it

			ActionPlay(true); // Play
		}
	}
	else
	{
		// Scroll to the area of the added tracks
		if (skinList->GetSelectedSize() > 0)
			skinList->ScrollToMyNode(skinList->GetSelectedAt(0));
	}

	if (settings.IsPlayFocus())
		skinList->EnablePlayFocus(true);

	if (settings.IsAddAllToLibrary())
	{
		skinTree->ClearLibrary();
	}

	if (isDefaultPlaylist && skinTree->GetDefPlaylistNode())
	{
		skinTree->SetFocusNode(skinTree->GetDefPlaylistNode());
		skinTree->ScrollToFocusNode();
	}

	UpdateStatusLine();
}

void WinylWnd::DropAddPlaylist(const std::vector<std::wstring>& files, bool isPlay)
{
	assert(!dlgProgressPtr);

	std::wstring file;
	if (files.size() == 1)
		file = files[0];
	if (file.empty())
		return;

	std::wstring name = PathEx::NameFromPath(file);

	TreeNodeUnsafe node = dBase.CreatePlaylist(skinTree.get(), name);

	if (node == nullptr)
		return;

	EnableAll(false);

	DlgProgress dlgProgress;
	dlgProgress.SetLanguage(&lang);
	dlgProgress.progress.SetDataBase(&dBase);
	dlgProgress.progress.SetPortableVersion(isPortableVersion);
	dlgProgress.progress.SetProgramPath(programPath);
	dlgProgress.progress.SetNewPlaylist(file, node->GetValue());
	dlgProgress.progress.SetAddAllToLibrary(settings.IsAddAllToLibrary());
	dlgProgress.ModalDialog(thisWnd, IDD_DLGPROGRESS);

	EnableAll(true);

	BeginWaitCursor();
	dBase.ClosePlaylist();
	dBase.OpenPlaylist(node->GetValue());
	dBase.FillPlaylist(skinList.get(), node->GetValue());
	skinList->ScrollToFocusNode();
	EndWaitCursor();

	skinTree->SetFocusNode(node);
	skinTree->ScrollToFocusNode();
	FillList(node);

	if (isPlay)
	{
		// Will be a crash without this line if a track is playing
		skinList->DeleteNowPlaying();
		ActionPlay(true);
	}

	if (settings.IsAddAllToLibrary())
	{
		skinTree->ClearLibrary();
	}

	if (node)
	{
		skinTree->SetFocusNode(node);
		skinTree->ScrollToFocusNode();
	}
}

void WinylWnd::OnCopyData(HWND hWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	if (dlgProgressPtr) // Do not add if the library is scanning
		return;

	std::wstring file = (const wchar_t*)pCopyDataStruct->lpData;
		
	std::vector<std::wstring> files;
	DropParseFiles(file, files);

	bool isImportPlaylist = DropIsPlaylist(files);
	bool isDefaultPlaylist = false;
	if (!isImportPlaylist)
	{
		DropNewDefault();
		isDefaultPlaylist = true;
	}

	if (!isImportPlaylist)
	{
		if (pCopyDataStruct->dwData == 1) // The files came from the command line
			DropAddFiles(files, true /*!isMediaPlay*/, isDefaultPlaylist);
		else if (pCopyDataStruct->dwData == 2) // The files came from Windows Shell
			DropAddFiles(files, true, isDefaultPlaylist);
	}
	else
	{
		if (pCopyDataStruct->dwData == 1) // The files came from the command line
			DropAddPlaylist(files, true /*!isMediaPlay*/);
		else if (pCopyDataStruct->dwData == 2) // The files came from Windows Shell
			DropAddPlaylist(files, true);
	}
}

void WinylWnd::DropParseFiles(const std::wstring& file, std::vector<std::wstring>& files)
{
	// Split the line by '|'

	std::size_t start = 0;
	std::size_t find = 0;
	while ((find = file.find('|', find)) != std::string::npos)
	{
		files.push_back(file.substr(start, find - start));
		++find;
		start = find;
	}

	files.push_back(file.substr(start));
}

void WinylWnd::SetWindowCaption(const std::wstring& artist, const std::wstring& title, bool isRadio)
{
	std::wstring artistTitle = title;

	if (!artist.empty())
		artistTitle = artist + L" - " + artistTitle;
	else if (isRadio)
		artistTitle = lang.GetLineS(Lang::Playlist, 3) + L" - " + artistTitle;

	trayIcon.SetString(thisWnd, artistTitle);

	::SetWindowText(thisWnd, artistTitle.c_str());

	if (skinMini)
		::SetWindowText(skinMini->Wnd(), artistTitle.c_str());
}

void WinylWnd::SetWindowCaptionNull()
{
	trayIcon.SetNullString(thisWnd);

	::SetWindowText(thisWnd, L"Winyl");
	if (skinMini)
		::SetWindowText(skinMini->Wnd(), L"Winyl");
}

void WinylWnd::AddFileFolder(bool isFolder)
{
	if (dlgProgressPtr) // Do not add if library scanning
		return;

	std::vector<std::wstring> files;
	bool result = true;

	if (isFolder)
	{
		EnableAll(false);

		FileDialogEx fileDialog;
		fileDialog.SetFolderTitleXP(lang.GetLineS(Lang::LibraryDialog, 7));

		if (fileDialog.DoModalFolder(thisWnd, true))
		{
			// Check the file despite of isMulti flag because WinXP doesn't support multiple folders
			if (!fileDialog.GetFile().empty())
				files.push_back(fileDialog.GetFile());
			else
			{
				for (int i = 0; i < fileDialog.GetMultiCount(); ++i)
					files.push_back(fileDialog.GetMultiFile(i));
			}
		}
		else
			result = false;

		EnableAll(true);
	}
	else
	{
		EnableAll(false);

		FileDialogEx fileDialog;

		FileDialogEx::FILE_TYPES fileTypes[] =
		{
			{L"All Audio Files (*.*)",
				L"*.mp3;*.ogg;*.oga;*.wma;*.asf;*.wav;*.aiff;*.aif;*.flac;*.fla;*.ape;"
				L"*.aac;*.mp4;*.m4a;*.m4b;*.m4r;*.opus;*.spx;*.wv;*.mpc;*.tta;*.cue"},
			{L"MPEG Audio (*.mp3)", L"*.mp3"},
			{L"OGG Vorbis (*.ogg)", L"*.ogg;*.oga"},
			{L"Apple Audio (*.m4a)", L"*.aac;*.mp4;*.m4a;*.m4b;*.m4r"},
			{L"Windows Media Audio (*.wma)", L"*.wma;*.asf"},
			{L"Waveform Audio File Format (*.wav)", L"*.wav"},
			{L"Audio Interchange File Format (*.aiff)", L"*.aiff;*.aif"},
			{L"Free Lossless Audio Codec (*.flac)", L"*.flac;*.fla"},
			{L"Monkey's Audio (*.ape)", L"*.ape"},
			{L"Opus (*.opus)", L"*.opus"},
			{L"Speex (*.spx)", L"*.spx"},
			{L"WavPack (*.wv)", L"*.wv"},
			{L"Musepack (*.mpc)", L"*.mpc"},
			{L"True Audio (*.tta)", L"*.tta"},
			{L"Cue Sheet", L"*.cue"}
		};
		int countTypes = sizeof(fileTypes) / sizeof(fileTypes[0]);

		fileDialog.SetFileTypes(fileTypes, countTypes);

		if (fileDialog.DoModalFile(thisWnd, false, true))
		{
			for (int i = 0; i < fileDialog.GetMultiCount(); ++i)
				files.push_back(fileDialog.GetMultiFile(i));
		}
		else
			result = false;

		EnableAll(true);
	}

	if (result)
	{
		bool isDefaultPlaylist = false;
		if (!dBase.IsPlaylistOpen())
		{
			DropNewDefault();
			isDefaultPlaylist = true;
		}

		DropAddFiles(files, false, isDefaultPlaylist);
	}
}

void WinylWnd::AddURL()
{
	EnableAll(false);

	DlgOpenURL dlg;
	dlg.SetLanguage(&lang);
	if (dlg.ModalDialog(thisWnd, IDD_DLGOPENURL) == IDOK)
	{
		if (!dBase.IsPlaylistOpen())
			DropNewDefault();

		// Below is a simpler version of DropAddFiles
		skinList->RemoveSelection();
		if (settings.IsPlayFocus())
			skinList->EnablePlayFocus(false);

		long long addedTime = FileSystem::GetTimeNow();
		int start = dBase.GetPlaylistMax();
		dBase.AddURLToPlaylist(start + 1, addedTime, dlg.GetURL(), dlg.GetName());
		dBase.FillPlaylistOpenFile(skinList.get(), settings.GetLibraryValue(), start);

		// Scroll to the area of the added tracks
		if (skinList->GetSelectedSize() > 0)
			skinList->ScrollToMyNode(skinList->GetSelectedAt(0));

		if (settings.IsPlayFocus())
			skinList->EnablePlayFocus(true);
	}

	EnableAll(true);
}

void WinylWnd::ImportPlaylist()
{
		EnableAll(false);

		FileDialogEx fileDialog;

		FileDialogEx::FILE_TYPES fileTypes[] =
		{
			{L"All Playlists", L"*.m3u;*.m3u8;*.pls;*.asx;*.xspf"},
			{L"M3U Playlist (*.m3u; *.m3u8)", L"*.m3u;*.m3u8"},
			{L"PLS Playlist (*.pls)", L"*.pls"},
			{L"Advanced Stream Redirector (*.asx)", L"*.asx"}, 
			{L"XML Shareable Playlist Format (*.xspf)", L"*.xspf"},
		};
		int countTypes = sizeof(fileTypes) / sizeof(fileTypes[0]);

		fileDialog.SetFileTypes(fileTypes, countTypes);

		if (fileDialog.DoModalFile(thisWnd, false, false))
		{
			std::vector<std::wstring> files;
			files.push_back(fileDialog.GetFile());
			DropAddPlaylist(files, false);
		}

		EnableAll(true);
}

bool WinylWnd::CoverThread(const std::wstring& file, const std::wstring& album, const std::wstring& artist)
{
	std::wstring path = PathEx::PathFromFile(file);

	if (path == coverPath && album == coverAlbum && artist == coverArtist)
	{
		return false;
	}
	else
	{
		mutexCover.Lock();
		isThreadCover = true;
		coverFile = file;
		mutexCover.Unlock();

		coverPath = path;
		coverAlbum = album;
		coverArtist = artist;
	}

	isCoverShowPopup = true;

	CoverThreadStart();
	return true;
}

void WinylWnd::CoverThreadStart()
{
	if (!threadCover.IsRunning())
	{
		if (threadCover.IsJoinable())
			threadCover.Join();

		threadCover.Start(std::bind(&WinylWnd::CoverThreadRun, this));
	}
}

void WinylWnd::CoverThreadRun()
{
	while (isThreadCover)
	{
		mutexCover.Lock();
		isThreadCover = false;
		std::wstring file = coverFile;
		mutexCover.Unlock();

		if (!file.empty())
		{
			CoverLoader coverLoader;
			coverLoader.LoadCoverImage(file);

			if (isMediaPlay && !isMediaRadio) // If don't press stop while loading
			{
				if (coverLoader.GetImage().IsValid())
				{
					skinDraw.DrawCover(&coverLoader.GetImage());
					if (skinPopup)
						skinPopup->SetCover(&coverLoader.GetImage());
					if (skinMini)
						skinMini->skinDraw.DrawCover(&coverLoader.GetImage());

					win7TaskBar.SetCover(Wnd(), coverLoader.GetImage());
				}
				else
				{
					skinDraw.DrawCover(nullptr);
					if (skinPopup)
						skinPopup->SetCover(nullptr);
					if (skinMini)
						skinMini->skinDraw.DrawCover(nullptr);

					win7TaskBar.EmptyCover(Wnd());
				}
			}
		}
	}

	if (IsWnd()) ::PostMessageW(Wnd(), UWM_COVERDONE, 0, 0);
}

void WinylWnd::CoverThreadDone()
{
	if (isThreadCover)
	{
		CoverThreadStart();
		return;
	}

	if (isMediaPlay && isCoverShowPopup)
		ShowPopup();
}

void WinylWnd::SetCoverNone()
{
	mutexCover.Lock();
	coverFile.clear();
	mutexCover.Unlock();
	coverPath.clear();
	coverAlbum.clear();
	coverArtist.clear();

	skinDraw.DrawCover(nullptr);
	if (skinPopup)
		skinPopup->SetCover(nullptr);
	if (skinMini)
		skinMini->skinDraw.DrawCover(nullptr);

	win7TaskBar.EmptyCover(thisWnd);
}

void WinylWnd::LyricsThread(const std::wstring& file, const std::wstring& title, const std::wstring& artist)
{
	if (!isLyricsWindow)
		return;

	mutexLyrics.Lock();
	isThreadLyrics = true;
	lyricsFile = file;
	lyricsTitle = title;
	lyricsArtist = artist;
	mutexLyrics.Unlock();

	LyricsThreadStart();
}

void WinylWnd::LyricsThreadStart()
{
	if (!isWindowIconic && skinLyrics->IsWindowVisible())
	{
		if (!threadLyrics.IsRunning())
		{
			if (threadLyrics.IsJoinable())
				threadLyrics.Join();

			threadLyrics.Start(std::bind(&WinylWnd::LyricsThreadRun, this));
		}
	}
}

void WinylWnd::LyricsThreadRun()
{
	while (isThreadLyrics)
	{
		mutexLyrics.Lock();
		isThreadLyrics = false;
		std::wstring file = lyricsFile;
		std::wstring title = lyricsTitle;
		std::wstring artist = lyricsArtist;
		mutexLyrics.Unlock();

		lyricsSource = 0;
		LyricsLoader lyricsLoader;
		if (!lyricsLoader.LoadLyricsFromFile(file))
		{
			if (!lyricsLoader.LoadLyricsFromTags(file))
			{
				if (!settings.IsLyricsProviderOff())
				{
					if (IsWnd()) ::PostMessageW(Wnd(), UWM_LYRICSRECV, 0, 0);
					if (lyricsLoader.LoadLyricsFromInternet(artist, title, settings.GetLyricsProvider()))
						lyricsSource = 3;
				}
			}
			else
				lyricsSource = 2;
		}
		else
			lyricsSource = 1;

		lyricsLines = lyricsLoader.MoveLines();
	}

	//Sleep(3300);
	if (IsWnd()) ::PostMessageW(Wnd(), UWM_LYRICSDONE, 0, 0);
}

void WinylWnd::LyricsThreadDone()
{
	if (isThreadLyrics)
	{
		LyricsThreadStart();
		return;
	}

	if (isMediaPlay && !isMediaRadio)
	{
		lyricsFileCur = lyricsFile;
		lyricsTitleCur = lyricsTitle;
		lyricsArtistCur = lyricsArtist;

		skinLyrics->SetState(true, false, false);
		skinLyrics->UpdateLyrics(std::move(lyricsLines));
	}
	else
		lyricsLines.clear();
}

void WinylWnd::SetLyricsNone(bool isRadio)
{
	if (!isLyricsWindow)
		return;

	mutexLyrics.Lock();
	lyricsFile.clear();
	lyricsTitle.clear();
	lyricsArtist.clear();
	mutexLyrics.Unlock();

	lyricsFileCur.clear();
	lyricsTitleCur.clear();
	lyricsArtistCur.clear();

	lyricsLines.clear();

	lyricsSource = 0;

	if (isRadio)
		skinLyrics->SetState(true, false, false);
	else
		skinLyrics->SetState(false, false, false);
	skinLyrics->UpdateLyricsNull();
}

void WinylWnd::LyricsThreadReceiving()
{
	lyricsFileCur.clear();
	lyricsTitleCur.clear();
	lyricsArtistCur.clear();

	lyricsLines.clear();

	skinLyrics->SetState(false, true, false);
	skinLyrics->UpdateLyricsNull();
}

void WinylWnd::LyricsThreadReload()
{
	if (!isLyricsWindow)
		return;

	if (lyricsFile != lyricsFileCur || lyricsTitle != lyricsTitleCur || lyricsArtist != lyricsArtistCur)
	{
		if (isMediaPlay && !isMediaRadio)
		{
			skinLyrics->SetState(false, false, true);
			skinLyrics->UpdateLyricsNull();
			LyricsThreadStart();
		}
		else
			SetLyricsNone(isMediaRadio);
	}
}

void WinylWnd::DialogConfig(int page)
{
	//SetActiveWindow();
	EnableAll(false);
	DlgConfig dlg;
	dlg.SetLanguage(&lang);
	dlg.SetSettings(&settings);
	dlg.SetPage(page);

	dlg.pageGeneral.SetProgramPath(programPath);
	dlg.pageGeneral.SetHideAssoc(isPortableVersion || isPortableProfile);
	dlg.pageGeneral.SetLastFM(&lastFM);
	dlg.pagePopup.SetSkinPopup(skinPopup.get(), &isMediaPlay);
	dlg.pagePopup.SetContextMenu(&contextMenu);
	dlg.pageSystem.SetMainWnd(thisWnd);
	dlg.pageSystem.SetLibAudio(&libAudio);
	dlg.pageSystem.SetRadio(&radio);
	dlg.pageMini.SetMiniPlayer(skinMini.get());

	if (dlg.ModalDialog(thisWnd, IDD_DLGCONFIG) == IDOK)
	{
		SaveSettings();

		if (dlg.pageGeneral.IsUpdateLibrary())
			LoadLibraryView(true);

		skinList->EnablePlayFocus(settings.IsPlayFocus());
	}

	EnableAll(true);
}

void WinylWnd::DialogLanguage()
{
	EnableAll(false);
	DlgLanguage dlg;
	dlg.SetLanguage(&lang);
	dlg.SetProgramPath(programPath);
	if (dlg.ModalDialog(thisWnd, IDD_DLGLANGUAGE) == IDOK)
	{
		if (!lang.ReloadLanguage(dlg.GetLanguageName()))
		{
			lang.ReloadLanguage(settings.GetDefaultLanguage());
			MessageBox::Show(thisWnd, L"Error", L"The language file is corrupted or it's for a different version of the program.", MessageBox::Icon::Warning);
		}

		contextMenu.ReloadMenu();

		contextMenu.CheckRepeat(settings.IsRepeat());
		contextMenu.CheckShuffle(settings.IsShuffle());
		contextMenu.CheckMute(settings.IsMute());
		contextMenu.CheckHideToTray(settings.IsHideToTray());
//		contextMenu.CheckLastFM(settings.IsLastFM());
		contextMenu.CheckPopup(settings.IsPopup());
		contextMenu.CheckEffect(settings.IsEffect());
		contextMenu.CheckSmoothScroll(settings.IsSmoothScroll());

		contextMenu.CheckPopupPosition(settings.GetPopupPosition());

		contextMenu.CheckMiniPlayer(settings.IsMiniPlayer());

		contextMenu.CheckSearch(settings.GetSearchType());

		contextMenu.CheckLyricsAlign(settings.GetLyricsAlign());
		contextMenu.CheckLyricsFontSize(settings.GetLyricsFontSize());
		contextMenu.CheckLyricsFontBold(settings.GetLyricsFontBold());
		if (settings.IsLyricsProviderOff())
			contextMenu.CheckLyricsProvider(-1);
		else
		{
			if (settings.GetLyricsProvider().empty())
				contextMenu.CheckLyricsProvider(0);
			else
			{
				int prov = LyricsLoader::GetLyricsProviderByURL(settings.GetLyricsProvider());
				if (prov > -1)
					contextMenu.CheckLyricsProvider(prov);
				else
				{
					contextMenu.CheckLyricsProvider(0);
					settings.SetLyricsProvider(L"");
				}
			}
		}

		LoadLibraryView(true);		

		skinList->SetNoItemsString(lang.GetLineS(Lang::Playlist, 4));
		if (!skinList->GetRootNode()->HasChild())
			::InvalidateRect(skinList->Wnd(), NULL, FALSE);

		skinLyrics->SetLanguageNoLyrics(lang.GetLineS(Lang::Lyrics, 0));
		skinLyrics->SetLanguageNotFound(lang.GetLineS(Lang::Lyrics, 1));
		skinLyrics->SetLanguageReceiving(lang.GetLineS(Lang::Lyrics, 2));
		if (skinLyrics->IsWindowVisible())
			::InvalidateRect(skinLyrics->Wnd(), NULL, FALSE);

		if (settings.GetSearchType() == 0)
			skinEdit->SetStandartText(lang.GetLineS(Lang::Search, 0));
		else if (settings.GetSearchType() == 1)
			skinEdit->SetStandartText(lang.GetLineS(Lang::Search, 2));
		else if (settings.GetSearchType() == 2)
			skinEdit->SetStandartText(lang.GetLineS(Lang::Search, 3));
		else if (settings.GetSearchType() == 3)
			skinEdit->SetStandartText(lang.GetLineS(Lang::Search, 4));

		UpdateStatusLine();

		Associations assoc;
		if (assoc.QueryAssocFolder())
		{
			assoc.SetProgramPath(programPath);
			assoc.SetPlayInWinylString(lang.GetLineS(Lang::GeneralPage, 8));
			assoc.AddAssocFolder();
		}
	}
	EnableAll(true);
}

void WinylWnd::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// Set min and max window size
	lpMMI->ptMaxTrackSize.x = skinDraw.GetMaxSize().cx;
	lpMMI->ptMaxTrackSize.y = skinDraw.GetMaxSize().cy;

	lpMMI->ptMinTrackSize.x = skinDraw.GetMinSize().cx;
	lpMMI->ptMinTrackSize.y = skinDraw.GetMinSize().cy;

	// Adjust min and max size if alpha window is present
	if (skinDraw.IsLayeredAlpha())
	{
		lpMMI->ptMaxTrackSize.x -= skinDraw.GetAlphaBorder().left + skinDraw.GetAlphaBorder().right;
		lpMMI->ptMaxTrackSize.y -= skinDraw.GetAlphaBorder().top + skinDraw.GetAlphaBorder().bottom;

		lpMMI->ptMinTrackSize.x -= skinDraw.GetAlphaBorder().left + skinDraw.GetAlphaBorder().right;
		lpMMI->ptMinTrackSize.y -= skinDraw.GetAlphaBorder().top + skinDraw.GetAlphaBorder().bottom;
	}

	// Set size for maximized window, only if window is borderless
	if (!skinDraw.IsStyleBorder())
	{
		CRect rcRect;

		// Get work rect (depending on the number of monitors)
		if (::GetSystemMetrics(SM_CMONITORS) > 1)
		{
			HMONITOR hMon = ::MonitorFromWindow(thisWnd, MONITOR_DEFAULTTONEAREST);

			MONITORINFO mi = {};
			mi.cbSize = sizeof(mi);
			::GetMonitorInfoW(hMon, &mi);
			rcRect = mi.rcWork;

			// http://blogs.msdn.com/b/llobo/archive/2006/08/01/maximizing-window-_2800_with-windowstyle_3d00_none_2900_-considering-taskbar.aspx
			// Move work rect for second, third etc. monitor to default (0, 0) coordinates
			// i.e. work rect for any monitor with same resolution and taskbar position must be equal
			rcRect.MoveToX(mi.rcWork.left - mi.rcMonitor.left);
			rcRect.MoveToY(mi.rcWork.top - mi.rcMonitor.top);
		}
		else
		{
			RECT rc = {};
			::SystemParametersInfoW(SPI_GETWORKAREA, 0, &rc, 0);
			rcRect = rc;
		}

		// Add maximize border
		rcRect.left -= skinDraw.GetMaximizeBorder().left;
		rcRect.top -= skinDraw.GetMaximizeBorder().top;
		rcRect.right += skinDraw.GetMaximizeBorder().right;
		rcRect.bottom += skinDraw.GetMaximizeBorder().bottom;

		// Adjust window size if alpha window is present
		if (skinDraw.IsLayeredAlpha())
		{
			rcRect.left += skinDraw.GetAlphaBorder().left;
			rcRect.top += skinDraw.GetAlphaBorder().top;
			rcRect.right -= skinDraw.GetAlphaBorder().right;
			rcRect.bottom -= skinDraw.GetAlphaBorder().bottom;
		}

		lpMMI->ptMaxPosition.x = rcRect.left;
		lpMMI->ptMaxPosition.y = rcRect.top;
		lpMMI->ptMaxSize.x = rcRect.Width();
		lpMMI->ptMaxSize.y = rcRect.Height();
	}
}

void WinylWnd::MiniPlayer(bool isEnable)
{
	if (skinMini)
	{
		settings.SetMiniPlayer(isEnable);
		trayIcon.SetMiniPlayer(isEnable);
		contextMenu.CheckMiniPlayer(isEnable);

		if (isEnable)
		{
			isWindowIconic = true;
			trayIcon.Minimize(thisWnd, true);
			::SetForegroundWindow(skinMini->Wnd());
			skinMini->SetVisible(true);
		}
		else
		{
			isWindowIconic = false;
			skinMini->SetVisible(false);
			trayIcon.Restore(thisWnd, true, settings.IsMaximized());
			if (skinList) // When the skin is changing skinList can be nullptr
				skinList->ScrollToPlayNode();
			LyricsThreadReload();
		}
	}
}

void WinylWnd::ShowPopup()
{
	if (skinPopup && settings.IsPopup())
	{
		if (!(skinMini && ::IsWindowVisible(skinMini->Wnd())))
			skinPopup->Popup();
	}
}

void WinylWnd::DialogEqualizer()
{
	EnableAll(false);
	DlgEqualizer dlg;
	dlg.SetProgramPath(programPath);
	dlg.SetProfilePath(profilePath);
	dlg.SetLanguage(&lang);
	dlg.SetLibAudio(&libAudio);
	dlg.ModalDialog(thisWnd, IDD_DLGEQ);
	EnableAll(true);
}

void WinylWnd::DialogProperties(bool isOpenLyrics)
{
	ListNodeUnsafe node = nullptr;
	if (!isOpenLyrics)
		node = skinList->GetFocusNode();
	else
		node = skinList->GetPlayNode();

	if (node)
	{
		EnableAll(false);
		DlgProperties dlg;
		dlg.SetLanguage(&lang);
		dlg.SetSkinList(skinList.get());
		if (isOpenLyrics)
		{
			dlg.SetSkinListNode(node);
			dlg.SetOpenLyrics(skinLyrics->GetLyrics());
		}
		dlg.SetDataBase(&dBase);
		dlg.SetLibAudio(&libAudio);
		dlg.SetCallbackChanged(std::bind(&WinylWnd::PropertiesChanged, this, std::placeholders::_1));
		dlg.pageCover.SetSaveCoverTags(settings.IsTagEditorCoverTags());
		dlg.pageCover.SetSaveCoverFile(settings.IsTagEditorCoverFile());
		dlg.pageLyrics.SetSaveLyricsTags(settings.IsTagEditorLyricsTags());
		dlg.pageLyrics.SetSaveLyricsFile(settings.IsTagEditorLyricsFile());
		if (dlg.ModalDialog(thisWnd, IDD_DLGPROPERTIES) == IDOK)
		{
			PropertiesChanged(dlg.properties.get());
		}
		settings.SetTagEditorCoverTags(dlg.pageCover.IsSaveCoverTags());
		settings.SetTagEditorCoverFile(dlg.pageCover.IsSaveCoverFile());
		settings.SetTagEditorLyricsTags(dlg.pageLyrics.IsSaveLyricsTags());
		settings.SetTagEditorLyricsFile(dlg.pageLyrics.IsSaveLyricsFile());
		EnableAll(true);
	}
}

void WinylWnd::PropertiesChanged(Properties* properties)
{
	::InvalidateRect(skinList->Wnd(), NULL, false);

	if (properties->IsUpdateLibrary())
		skinTree->ClearLibrary();
	if (properties->IsUpdateCovers())
	{
		skinList->UpdateCovers();

		if (isMediaPlay && !isMediaRadio)
		{
			isCoverShowPopup = false;
			isThreadCover = true;
			CoverThreadStart();
		}
	}
	if (properties->IsUpdateLyrics())
	{
		if (isMediaPlay && !isMediaRadio)
		{
			if (properties->IsMultiple() || skinList->GetPlayNode() == properties->GetSkinListNode())
			{
				isThreadLyrics = true;
				LyricsThreadStart();
			}
		}
	}
}

void WinylWnd::DialogLibrary()
{
	if (dlgProgressPtr) // Do not show if the library is scanning
		return;

	EnableAll(false);
	DlgLibrary dlg;
	dlg.SetLanguage(&lang);
	dlg.pageLibrary.SetPortableVersion(isPortableVersion);
	dlg.pageLibrary.SetProgramPath(programPath);
	dlg.pageLibrary.SetLibraryFolders(&libraryFolders);
	dlg.pageLibraryOpt.SetRemoveMissing(settings.IsRescanRemoveMissing());
	dlg.pageLibraryOpt.SetIgnoreDeleted(settings.IsRescanIgnoreDeleted());
	if (dlg.ModalDialog(thisWnd, IDD_DLGLIBRARY) == IDOK)
	{
		SaveLibraryFolders();
		ScanLibraryStart(dlg.pageLibraryOpt.IsRemoveMissing(), dlg.pageLibraryOpt.IsIgnoreDeleted(),
			dlg.pageLibraryOpt.IsFindMoved(), dlg.pageLibraryOpt.IsRescanAll());
	}
	settings.SetRescanRemoveMissing(dlg.pageLibraryOpt.IsRemoveMissing());
	settings.SetRescanIgnoreDeleted(dlg.pageLibraryOpt.IsIgnoreDeleted());
	EnableAll(true);
}

void WinylWnd::ScanLibraryStart(bool isRemoveMissing, bool isIgnoreDeleted, bool isFindMoved, bool isRescanAll)
{
	dlgProgressPtr.reset(new DlgProgress());

	dlgProgressPtr->SetLanguage(&lang);
	dlgProgressPtr->SetMessageWnd(thisWnd);
	dlgProgressPtr->progress.SetDataBase(&dBase);
	dlgProgressPtr->progress.SetPortableVersion(isPortableVersion);
	dlgProgressPtr->progress.SetProgramPath(programPath);
	dlgProgressPtr->progress.SetLibraryFolders(libraryFolders);
	dlgProgressPtr->progress.SetIgnoreDeleted(isIgnoreDeleted);
	dlgProgressPtr->progress.SetRemoveMissing(isRemoveMissing);
	dlgProgressPtr->progress.SetFindMoved(isFindMoved);
	dlgProgressPtr->progress.SetRescanAll(isRescanAll);
	dlgProgressPtr->progress.SetAddAllToLibrary(settings.IsAddAllToLibrary());

	dlgProgressPtr->SetTaskbarMessage(wmTaskbarButtonCreated);
	dlgProgressPtr->CreateModelessDialog(NULL, IDD_DLGPROGRESS);
	::ShowWindow(dlgProgressPtr->Wnd(), SW_SHOW);
}

void WinylWnd::ScanLibraryFinish(bool isDestroyOnStop)
{
	if (dlgProgressPtr)
	{
		dlgProgressPtr.reset();
	}

	if (isDestroyOnStop)
	{
		::DestroyWindow(thisWnd);
		return;
	}

	skinTree->SetControlRedraw(false);
	
	skinTree->ClearLibrary();

	skinTree->SetControlRedraw(true);
}

void WinylWnd::OnNotify(WPARAM wParam, LPARAM lParam)
{
	LPNMTTDISPINFO nmtt = reinterpret_cast<LPNMTTDISPINFO>(lParam);
	if (nmtt->hdr.hwndFrom == toolTips.GetTipWnd() && nmtt->hdr.code == TTN_NEEDTEXT)
	{
		const std::wstring* text = toolTips.GetText((SkinElement*)wParam);
		if (text)
			nmtt->lpszText = (LPWSTR)text->c_str();
	}
}

bool WinylWnd::LoadLibraryFolders()
{
	std::wstring file = profilePath + L"Library.xml";

	XmlFile xmlFile;

	if (xmlFile.LoadFile(file))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Library");

		if (xmlMain)
		{
			for (XmlNode xmlNode = xmlMain.FirstChild(); xmlNode; xmlNode = xmlNode.NextChild())
			{
				std::wstring path = xmlNode.Attribute16("Path");
				if (!path.empty())
				{
					// Ver: 2.6.0 In old versions the last slash was cutted off
					if (!path.empty() && path.back() != '\\')
						path.push_back('\\');

					libraryFolders.push_back(path);
				}
			}
		}
	}
	else
		return false;

//	libraryFolders.push_back(L"C:\\");
//	libraryFolders.push_back(L"D:\\");
	return true;
}

bool WinylWnd::SaveLibraryFolders()
{
	XmlFile xmlFile;
	XmlNode xmlMain = xmlFile.RootNode().AddChild("Library");
	
	for (std::size_t i = 0, size = libraryFolders.size(); i < size; ++i)
	{
		XmlNode xmlPath = xmlMain.AddChild("Folder");

		xmlPath.AddAttribute16("Path", libraryFolders[i]);
	}
	
	std::wstring file = profilePath + L"Library.xml";

	if (xmlFile.SaveFile(file))
		return true;

	return false;
}

void WinylWnd::UpdateStatusLine()
{
	if (!skinList->IsRadio())
	{
		int count = 0, total = 0, time = 0;
		long long size = 0;

		skinList->CalculateSelectedNodes(count, total, time, size);

		if (total > 0)
			skinDraw.DrawStatusLine(count, total, time, size, &lang);
		else
			skinDraw.DrawStatusLineNone();
	}
	else
		skinDraw.DrawStatusLineNone();
}

void WinylWnd::TimerEffectsThreadRun()
{
	HANDLE timerHandle = ::CreateWaitableTimerW(NULL, FALSE, NULL);
	LARGE_INTEGER dueTime = {};
	::SetWaitableTimer(timerHandle, &dueTime, 16, NULL, NULL, FALSE); // 16 ms = 60 FPS = NO LAGS

	//LARGE_INTEGER freq; QueryPerformanceFrequency(&freq);
	//LARGE_INTEGER counter1; QueryPerformanceCounter(&counter1);

	while (!isTimerSmooth)
	{
		if ((!skinList || !skinList->IsSmoothScrollRun()) &&
			(!skinTree || !skinTree->IsSmoothScrollRun()) &&
			(!skinLyrics || !skinLyrics->IsSmoothScrollRun()))
		{
			eventTimerSmooth.Wait();
		}

		if (IsWnd()) ::PostMessageW(Wnd(), UWM_TIMERTHREAD, 0, 0);

		// Previously I was trying to use WaitForSingleObject on Event and Sleep here,
		// but these functions are very inaccurate with small interval, for example on Win10 under VirtualBox:
		// With WaitForSingleObject on Event and 16ms interval real interval was ~32ms, but with 15ms real interval was ~15ms.
		// With Sleep real interval was always ~32ms, maybe it is just under VirtualBox, but I am not sure.
		// So use much better waitable timer here, it is more accurate and can wait for difference of intervals between waits.
		::WaitForSingleObject(timerHandle, INFINITE);

		//LARGE_INTEGER counter2; QueryPerformanceCounter(&counter2);
		//double diffMs = (counter2.QuadPart - counter1.QuadPart) * 1000.0 / freq.QuadPart;
		//QueryPerformanceCounter(&counter1);

		//AllocConsole();
		//freopen("CONOUT$", "w", stdout);
		//printf("%f\n", diffMs);
	}

	//::CancelWaitableTimer(timerHandle);
	::CloseHandle(timerHandle);

	// Test
	// According to this test the precission of SetWaitableTimer is always 1ms
	// even if set nanosecs with DueTime so there is no point to use DueTime instead of Period
	//LARGE_INTEGER dueTime = {};
	//::GetSystemTimeAsFileTime((LPFILETIME)&dueTime);
	//dueTime.QuadPart += 166666; // 1000 / 60 = 16.6666
	//::SetWaitableTimer(timerHandle, &dueTime, 0, NULL, NULL, FALSE);
	//while (!isTimerSmooth)
	//{
	//	// Do stuff
	//	::WaitForSingleObject(timerHandle, INFINITE);
	//	::GetSystemTimeAsFileTime((LPFILETIME)&dueTime);
	//	dueTime.QuadPart += 166666; // 1000 / 60 = 16.6666
	//	::SetWaitableTimer(timerHandle, &dueTime, 0, NULL, NULL, FALSE);
	//}
}



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

// Settings.cpp : implementation file
//

#include "stdafx.h"
#include "Settings.h"


// Settings

Settings::Settings()
{

}

Settings::~Settings()
{

}

bool Settings::LoadSettings()
{
	std::wstring file = profilePath + L"Settings.xml";

	XmlFile xmlFile;

	if (xmlFile.LoadFile(file))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Settings");

		if (xmlMain)
		{
			int version = 0;
			XmlNode xmlVersion = xmlMain.FirstChild("Version");
			if (xmlVersion)
			{
				xmlVersion.Attribute("ID", &version);
				if (version < 30000)
					convertVersion3 = true;

				bool portable = false;
				xmlVersion.Attribute("Portable", &portable);

				if (isPortableVersion && !portable)
					convertPortableTo = true;
				else if (!isPortableVersion && portable)
					convertPortableFrom = true;
			}

			XmlNode xmlDBVersion = xmlMain.FirstChild("DBVersion");
			if (xmlDBVersion)
			{
				int dbVersion = 0;
				xmlDBVersion.Attribute("ID", &dbVersion);
				if (dbVersion == 20000)
					dbOldBeta = true;
			}

			XmlNode xmlLanguage = xmlMain.FirstChild("Language");
			if (xmlLanguage)
			{
				languageName = xmlLanguage.Attribute16("ID");
			}

			XmlNode xmlSkin = xmlMain.FirstChild("Skin");
			if (xmlSkin)
			{
				skinName = xmlSkin.Attribute16("ID");

				xmlSkin.Attribute("Pack", &isSkinPack);
			}

			//XmlNode xmlCheckForUpdates = xmlMain.FirstChild("CheckForUpdates");
			//if (xmlCheckForUpdates)
			//	xmlCheckForUpdates.Attribute("ID", &isCheckForUpdates);

			XmlNode xmlWinPosition = xmlMain.FirstChild("WinPosition");
			if (xmlWinPosition)
			{
				int left = rcPosition.left;
				int top = rcPosition.top;
				int width = rcPosition.Width();
				int height = rcPosition.Height();

				xmlWinPosition.Attribute("Left", &left);
				xmlWinPosition.Attribute("Top", &top);
				xmlWinPosition.Attribute("Width", &width);
				xmlWinPosition.Attribute("Height", &height);

				xmlWinPosition.Attribute("Maximized", &isMaximized);

				if (left < -10000 || top < -10000)
				{
					// Use default settings (fix a bug in old versions, remove in 2010 year)
				}
				else
					rcPosition = CRect(left, top, left + width, top + height);
			}

			XmlNode xmlWinPositionMini = xmlMain.FirstChild("WinPositionMini");
			if (xmlWinPositionMini)
			{
				int left = rcPositionMini.left;
				int top = rcPositionMini.top;
				int width = rcPositionMini.Width();
				int height = rcPositionMini.Height();

				xmlWinPositionMini.Attribute("Left", &left);
				xmlWinPositionMini.Attribute("Top", &top);
				xmlWinPositionMini.Attribute("Width", &width);
				xmlWinPositionMini.Attribute("Height", &height);

				rcPositionMini = CRect(left, top, left + width, top + height);
			}

			XmlNode xmlSoundVolume = xmlMain.FirstChild("SoundVolume");
			if (xmlSoundVolume)
				xmlSoundVolume.Attribute("ID", &soundVolume);

			// Adjust sound volume (fix a bug in old versions, remove in 2010 year)
			if (soundVolume > 100000) soundVolume = 100000;
			else if (soundVolume < 0) soundVolume = 0;

			/*TiXmlElement* eSoundMute = eMain->FirstChildElement("SoundMute");
			if (eSoundMute)
				eSoundMute->Attribute("ID", &isSoundMute);*/

			XmlNode xmlDevice = xmlMain.FirstChild("Device");
			if (xmlDevice)
			{
				xmlDevice.Attribute("ID", &bassDevice);
				xmlDevice.Attribute("Driver", &bassDriver);
				xmlDevice.Attribute("SoftMix", &bassSoftMix);
				xmlDevice.Attribute("Bit32", &bassBit32);
				xmlDevice.Attribute("NoVolume", &bassNoVolume);
				xmlDevice.Attribute("NoEffect", &bassNoEffect);
				xmlDevice.Attribute("WasapiEvent", &bassWasapiEvent);
				xmlDevice.Attribute("AsioChannel", &bassAsioChannel);

				if (version < 2850)
				{
					if (bassDriver == 0)
						bassDevice = -1;
				}
			}

			XmlNode xmlUserAgent = xmlMain.FirstChild("UserAgent");
			if (xmlUserAgent)
			{
				bassUserAgent = xmlUserAgent.Attribute8("ID");
			}

			XmlNode xmlMediaRepeat = xmlMain.FirstChild("Repeat");
			if (xmlMediaRepeat)
				xmlMediaRepeat.Attribute("ID", &isMediaRepeat);

			XmlNode xmlMediaShuffle = xmlMain.FirstChild("Shuffle");
			if (xmlMediaShuffle)
				xmlMediaShuffle.Attribute("ID", &isMediaShuffle);

			XmlNode xmlHideToTray = xmlMain.FirstChild("HideToTray");
			if (xmlHideToTray)
				xmlHideToTray.Attribute("ID", &isHideToTray);
			else // [v2.1] Remove after couple of versions
			{
				xmlHideToTray = xmlMain.FirstChild("HideInTray");
				if (xmlHideToTray)
					xmlHideToTray.Attribute("ID", &isHideToTray);
			}

			XmlNode xmlSearchType = xmlMain.FirstChild("Search");
			if (xmlSearchType)
				xmlSearchType.Attribute("ID", &searchType);

			XmlNode xmlAnimation = xmlMain.FirstChild("Animation");
			if (xmlAnimation)
				xmlAnimation.Attribute("ID", &isEffectEnable);

			XmlNode xmlSmoothScroll = xmlMain.FirstChild("SmoothScroll");
			if (xmlSmoothScroll)
				xmlSmoothScroll.Attribute("ID", &isSmoothScroll);

			XmlNode xmlLibraryAddAll = xmlMain.FirstChild("LibraryAddAll");
			if (xmlLibraryAddAll)
				xmlLibraryAddAll.Attribute("ID", &isAddAllToLibrary);

			XmlNode xmlPlayFocus = xmlMain.FirstChild("PlayFocus");
			if (xmlPlayFocus)
				xmlPlayFocus.Attribute("ID", &isPlayFocus);

			XmlNode xmlLastFM = xmlMain.FirstChild("LastFM");
			if (xmlLastFM)
				xmlLastFM.Attribute("ID", &isLastFM);

			//XmlNode xmlMSN = xmlMain.FirstChild("MSN");
			//if (xmlMSN)
			//	xmlMSN.Attribute("ID", &isMSN);

			XmlNode xmlMiniPlayer = xmlMain.FirstChild("MiniPlayer");
			if (xmlMiniPlayer)
			{
				xmlMiniPlayer.Attribute("ID", &isMiniPlayer);
				xmlMiniPlayer.Attribute("Transparency", &miniTransparency);
				xmlMiniPlayer.Attribute("ZOrder", &miniZOrder);
			}

			/*TiXmlElement* eAssociations = eMain->FirstChildElement("Associations");
			if (eAssociations)
			{
				eAssociations->Attribute("ID", &isAssociations);
			}*/

			XmlNode xmlPopup = xmlMain.FirstChild("Popup");
			if (xmlPopup)
			{
				xmlPopup.Attribute("ID", &isPopupEnable);
				xmlPopup.Attribute("Position", &popupPosition);
				xmlPopup.Attribute("ShowEffect", &popupShowEffect);
				xmlPopup.Attribute("HideEffect", &popupHideEffect);
				
				xmlPopup.Attribute("Hold", &popupHold);
				xmlPopup.Attribute("Show", &popupShow);
				xmlPopup.Attribute("Hide", &popupHide);
			}

			XmlNode xmlProxy = xmlMain.FirstChild("Proxy");
			if (xmlProxy)
			{
				xmlProxy.Attribute("ID", &proxyType);

				proxyHost  = xmlProxy.Attribute16("Host");
				proxyPort  = xmlProxy.Attribute16("Port");
				proxyLogin = xmlProxy.Attribute16("Login");
				proxyPass  = xmlProxy.Attribute16("Pass");
			}

			XmlNode xmlLastPlayIndex = xmlMain.FirstChild("LastPlay");
			if (xmlLastPlayIndex)
				xmlLastPlayIndex.AttributeLong("ID", &lastPlayIndex);

			XmlNode xmlRescan = xmlMain.FirstChild("Rescan");
			if (xmlRescan)
			{
				xmlRescan.Attribute("RemoveMissing", &isRescanRemoveMissing);
				xmlRescan.Attribute("IgnoreDeleted", &isRescanIgnoreDeleted);
			}

			XmlNode xmlTagEditor = xmlMain.FirstChild("TagEditor");
			if (xmlTagEditor)
			{
				xmlTagEditor.Attribute("CoverTags", &tagEditorCoverTags);
				xmlTagEditor.Attribute("CoverFile", &tagEditorCoverFile);
				xmlTagEditor.Attribute("LyricsTags", &tagEditorLyricsTags);
				xmlTagEditor.Attribute("LyricsFile", &tagEditorLyricsFile);
			}

			XmlNode xmlLyrics = xmlMain.FirstChild("Lyrics");
			if (xmlLyrics)
			{
				if (xmlLyrics.Attribute("ID", &lyricsProviderOff))
					lyricsProviderOff = !lyricsProviderOff;
				xmlLyrics.Attribute16("Provider", &lyricsProvider);
				xmlLyrics.Attribute("TextAlign", &lyricsAlign);
				xmlLyrics.Attribute("FontSize", &lyricsFontSize);
				xmlLyrics.Attribute("FontBold", &lyricsFontBold);
			}

			XmlNode xmlLibraryView = xmlMain.FirstChild("LibraryView");
			if (xmlLibraryView)
			{
				xmlLibraryView.Attribute("Ars", &isLibraryArtists);
				xmlLibraryView.Attribute("Cms", &isLibraryComposers);
				xmlLibraryView.Attribute("Abs", &isLibraryAlbums);
				xmlLibraryView.Attribute("Gns", &isLibraryGenres);
				xmlLibraryView.Attribute("Yrs", &isLibraryYears);
				xmlLibraryView.Attribute("Fds", &isLibraryFolders);
				xmlLibraryView.Attribute("Rds", &isLibraryRadios);
				xmlLibraryView.Attribute("Sms", &isLibrarySmartlists);
			}

			/*XmlNode xmlLibrary = xmlMain.FirstChild("Library");
			if (xmlLibrary)
			{
				XmlNode xmlNode1 = xmlLibrary.FirstChild("Node1");
				if (xmlNode1)
				{
					xmlNode1.Attribute("This", &stLibrary.isType);
					xmlNode1.Attribute("Type", &stLibrary.type);
				}

				XmlNode xmlNode2 = xmlLibrary.FirstChild("Node2");
				if (xmlNode2)
				{
					xmlNode2.Attribute("This", &stLibrary.isValue);
					stLibrary.value = xmlNode2.Attribute16("String");
				}

				XmlNode xmlNode3 = xmlLibrary.FirstChild("Node3");
				if (xmlNode3)
				{
					xmlNode3.Attribute("This", &stLibrary.isAlbum);
					stLibrary.artist = xmlNode3.Attribute16("Artist");
					stLibrary.album =xmlNode3.Attribute16("Album");
				}
			}*/

			XmlNode xmlOpenItem = xmlMain.FirstChild("OpenItem");
			if (xmlOpenItem)
			{
				xmlOpenItem.Attribute("Type", &stLibrary.type);

				if (xmlOpenItem.Attribute16("Value", &stLibrary.value))
				{
					if (xmlOpenItem.Attribute16("Artist", &stLibrary.artist))
					{
						if (xmlOpenItem.Attribute16("Album", &stLibrary.album))
							stLibrary.isAlbum = true;
						else
							stLibrary.isArtist = true;
					}
					else
						stLibrary.isValue = true;
				}
				else
					stLibrary.isType = true;
			}
		}
	}
	else
	{
		isFirstRun = true;
		return false;
	}

	return true;
}

bool Settings::SaveSettings()
{
	XmlFile xmlFile;
	XmlNode xmlMain = xmlFile.RootNode().AddChild("Settings");

	if (xmlMain)
	{
		XmlNode xmlVersion = xmlMain.AddChild("Version");
		if (xmlVersion)
		{
			xmlVersion.AddAttribute("ID", winylVersion);
			if (isPortableVersion)
				xmlVersion.AddAttribute("Portable", isPortableVersion);
		}

		XmlNode nDBVersion = xmlMain.AddChild("DBVersion");
		if (nDBVersion)
			nDBVersion.AddAttribute("ID", dbaseVersion);

		XmlNode xmlLanguage = xmlMain.AddChild("Language");
		if (xmlLanguage)
			xmlLanguage.AddAttribute16("ID", languageName);

		XmlNode xmlSkin = xmlMain.AddChild("Skin");
		if (xmlSkin)
		{
			xmlSkin.AddAttribute16("ID", skinName);
			xmlSkin.AddAttribute("Pack", isSkinPack);
		}

		//XmlNode xmlCheckForUpdates = xmlMain.AddChild("CheckForUpdates");
		//if (xmlCheckForUpdates)
		//	xmlCheckForUpdates.AddAttribute("ID", isCheckForUpdates);

		XmlNode xmlWinPosition = xmlMain.AddChild("WinPosition");
		if (xmlWinPosition)
		{
			xmlWinPosition.AddAttribute("Left", rcPosition.left);
			xmlWinPosition.AddAttribute("Top", rcPosition.top);
			xmlWinPosition.AddAttribute("Width", rcPosition.Width());
			xmlWinPosition.AddAttribute("Height", rcPosition.Height());

			xmlWinPosition.AddAttribute("Maximized", (int)isMaximized);
		}

		XmlNode xmlWinPositionMini = xmlMain.AddChild("WinPositionMini");
		if (xmlWinPositionMini)
		{
			xmlWinPositionMini.AddAttribute("Left", rcPositionMini.left);
			xmlWinPositionMini.AddAttribute("Top", rcPositionMini.top);
			xmlWinPositionMini.AddAttribute("Width", rcPositionMini.Width());
			xmlWinPositionMini.AddAttribute("Height", rcPositionMini.Height());
		}

		XmlNode xmlSoundVolume = xmlMain.AddChild("SoundVolume");
		if (xmlSoundVolume)
			xmlSoundVolume.AddAttribute("ID", soundVolume);

		/*TiXmlElement* eSoundMute = new TiXmlElement("SoundMute");
		if (eSoundMute)
		{
			eSoundMute->SetAttribute("ID", (int)isSoundMute);

			eMain->LinkEndChild(eSoundMute);
		}*/

		XmlNode xmlDevice = xmlMain.AddChild("Device");
		if (xmlDevice)
		{
			xmlDevice.AddAttribute("ID", bassDevice);
			xmlDevice.AddAttribute("Driver", bassDriver);
			xmlDevice.AddAttribute("Bit32", bassBit32);
			xmlDevice.AddAttribute("SoftMix", bassSoftMix);
			if (bassNoVolume)
				xmlDevice.AddAttribute("NoVolume", bassNoVolume);
			if (bassNoEffect)
				xmlDevice.AddAttribute("NoEffect", bassNoEffect);
			if (bassWasapiEvent)
				xmlDevice.AddAttribute("WasapiEvent", bassWasapiEvent);
			if (bassAsioChannel)
				xmlDevice.AddAttribute("AsioChannel", bassAsioChannel);
		}

		if (!bassUserAgent.empty())
		{
			XmlNode xmlUserAgent = xmlMain.AddChild("UserAgent");
			if (xmlUserAgent)
			{
				xmlUserAgent.AddAttribute8("ID", bassUserAgent);
			}
		}

		XmlNode xmlMediaRepeat = xmlMain.AddChild("Repeat");
		if (xmlMediaRepeat)
			xmlMediaRepeat.AddAttribute("ID", (int)isMediaRepeat);

		XmlNode xmlMediaShuffle = xmlMain.AddChild("Shuffle");
		if (xmlMediaShuffle)
			xmlMediaShuffle.AddAttribute("ID", (int)isMediaShuffle);

		XmlNode xmlHideToTray = xmlMain.AddChild("HideToTray");
		if (xmlHideToTray)
			xmlHideToTray.AddAttribute("ID", (int)isHideToTray);

		XmlNode xmlSearchType = xmlMain.AddChild("Search");
		if (xmlSearchType)
			xmlSearchType.AddAttribute("ID", (int)searchType);

		XmlNode xmlAnimation = xmlMain.AddChild("Animation");
		if (xmlAnimation)
			xmlAnimation.AddAttribute("ID", (int)isEffectEnable);

		XmlNode xmlSmoothScroll = xmlMain.AddChild("SmoothScroll");
		if (xmlSmoothScroll)
			xmlSmoothScroll.AddAttribute("ID", (int)isSmoothScroll);

		XmlNode xmlLibraryAddAll = xmlMain.AddChild("LibraryAddAll");
		if (xmlLibraryAddAll)
			xmlLibraryAddAll.AddAttribute("ID", (int)isAddAllToLibrary);

		XmlNode xmlPlayFocus = xmlMain.AddChild("PlayFocus");
		if (xmlPlayFocus)
			xmlPlayFocus.AddAttribute("ID", (int)isPlayFocus);

		XmlNode xmlLastFM = xmlMain.AddChild("LastFM");
		if (xmlLastFM)
			xmlLastFM.AddAttribute("ID", (int)isLastFM);

		//XmlNode xmlMSN = xmlMain.AddChild("MSN");
		//if (xmlMSN)
		//	xmlMSN.AddAttribute("ID", (int)isMSN);

		/*TiXmlElement* eAssociations = new TiXmlElement("Associations");
		if (eAssociations)
		{
			eAssociations->SetAttribute("ID", (int)isAssociations);

			eMain->LinkEndChild(eAssociations);
		}*/

		XmlNode xmlMiniPlayer = xmlMain.AddChild("MiniPlayer");
		if (xmlMiniPlayer)
		{
			xmlMiniPlayer.AddAttribute("ID", (int)isMiniPlayer);
			xmlMiniPlayer.AddAttribute("Transparency", miniTransparency);
			xmlMiniPlayer.AddAttribute("ZOrder", miniZOrder);
		}

		XmlNode xmlPopup = xmlMain.AddChild("Popup");
		if (xmlPopup)
		{
			xmlPopup.AddAttribute("ID", (int)isPopupEnable);
			xmlPopup.AddAttribute("Position", popupPosition);
			xmlPopup.AddAttribute("ShowEffect", popupShowEffect);
			xmlPopup.AddAttribute("HideEffect", popupHideEffect);
			xmlPopup.AddAttribute("Hold", (int)popupHold);
			xmlPopup.AddAttribute("Show", (int)popupShow);
			xmlPopup.AddAttribute("Hide", (int)popupHide);
		}

		XmlNode xmlProxy = xmlMain.AddChild("Proxy");
		if (xmlProxy)
		{
			xmlProxy.AddAttribute("ID", proxyType);
			xmlProxy.AddAttribute16("Host", proxyHost);
			xmlProxy.AddAttribute16("Port", proxyPort);
			xmlProxy.AddAttribute16("Login", proxyLogin);
			xmlProxy.AddAttribute16("Pass", proxyPass);
		}

		XmlNode xmlRescan = xmlMain.AddChild("Rescan");
		if (xmlRescan)
		{
			xmlRescan.AddAttribute("RemoveMissing", isRescanRemoveMissing);
			xmlRescan.AddAttribute("IgnoreDeleted", isRescanIgnoreDeleted);
		}

		XmlNode xmlTagEditor = xmlMain.AddChild("TagEditor");
		if (xmlTagEditor)
		{
			xmlTagEditor.AddAttribute("CoverTags", tagEditorCoverTags);
			xmlTagEditor.AddAttribute("CoverFile", tagEditorCoverFile);
			xmlTagEditor.AddAttribute("LyricsTags", tagEditorLyricsTags);
			xmlTagEditor.AddAttribute("LyricsFile", tagEditorLyricsFile);
		}

		XmlNode xmlLyrics = xmlMain.AddChild("Lyrics");
		if (xmlLyrics)
		{
			xmlLyrics.AddAttribute("ID", !lyricsProviderOff);
			xmlLyrics.AddAttribute16("Provider", lyricsProvider);
			xmlLyrics.AddAttribute("TextAlign", lyricsAlign);
			xmlLyrics.AddAttribute("FontSize", lyricsFontSize);
			xmlLyrics.AddAttribute("FontBold", lyricsFontBold);
		}

		XmlNode xmlLibraryView = xmlMain.AddChild("LibraryView");
		if (xmlLibraryView)
		{
			xmlLibraryView.AddAttribute("Ars", isLibraryArtists);
			xmlLibraryView.AddAttribute("Cms", isLibraryComposers);
			xmlLibraryView.AddAttribute("Abs", isLibraryAlbums);
			xmlLibraryView.AddAttribute("Gns", isLibraryGenres);
			xmlLibraryView.AddAttribute("Yrs", isLibraryYears);
			xmlLibraryView.AddAttribute("Fds", isLibraryFolders);
			xmlLibraryView.AddAttribute("Rds", isLibraryRadios);
			xmlLibraryView.AddAttribute("Sms", isLibrarySmartlists);
		}

		XmlNode xmlLastPlayIndex = xmlMain.AddChild("LastPlay");
		if (xmlLastPlayIndex)
			xmlLastPlayIndex.AddAttributeLong("ID", lastPlayIndex);

/*		XmlNode xmlLibrary = xmlMain.AddChild("Library");
		if (xmlLibrary)
		{
			XmlNode xmlNode1 = xmlLibrary.AddChild("Node1");
			if (xmlNode1)
			{
				xmlNode1.AddAttribute("This", stLibrary.isType);
				xmlNode1.AddAttribute("Type", stLibrary.type);
			}

			XmlNode xmlNode2 = xmlLibrary.AddChild("Node2");
			if (xmlNode2)
			{
				xmlNode2.AddAttribute("This", stLibrary.isValue);
				xmlNode2.AddAttribute16("String", stLibrary.value);
			}

			XmlNode xmlNode3 = xmlLibrary.AddChild("Node3");
			if (xmlNode3)
			{
				xmlNode3.AddAttribute("This", stLibrary.isAlbum);
				xmlNode3.AddAttribute16("Artist", stLibrary.artist);
				xmlNode3.AddAttribute16("Album", stLibrary.album);
			}
		}*/
		XmlNode xmlOpenItem = xmlMain.AddChild("OpenItem");
		if (xmlOpenItem)
		{
			xmlOpenItem.AddAttribute("Type", stLibrary.type);
			if (stLibrary.isValue || stLibrary.isArtist || stLibrary.isAlbum)
			{
				xmlOpenItem.AddAttribute16("Value", stLibrary.value);

				if (stLibrary.isArtist || stLibrary.isAlbum)
				{
					xmlOpenItem.AddAttribute16("Artist", stLibrary.artist);

					if (stLibrary.isAlbum)
						xmlOpenItem.AddAttribute16("Album", stLibrary.album);
				}
			}
		}
	}

	std::wstring file = profilePath + L"Settings.xml";

	if (xmlFile.SaveFile(file))
		return true;

	return false;
}

void Settings::FixWinPosMain(const CSize& szMinSize, const CSize& szMaxSize)
{
	// Adjust the main window position and size by the monitor
	FixRectByMonitor(rcPosition, szMinSize, szMaxSize);
}

void Settings::FixWinPosMini(const CSize& szMinSize, const CSize& szMaxSize)
{
	// Adjust the mini player position and size by the monitor
	FixRectByMonitor(rcPositionMini, szMinSize, szMaxSize);
}

void Settings::FixRectByMonitor(CRect& rcRect, const CSize& szMinSize, const CSize& szMaxSize)
{
	// First, adjust window size by min and max size
	if (rcRect.Width() < szMinSize.cx)
		rcRect.right = rcRect.left + szMinSize.cx;
	else if (rcRect.Width() > szMaxSize.cx)
		rcRect.right = rcRect.left + szMaxSize.cx;

	if (rcRect.Height() < szMinSize.cy)
		rcRect.bottom = rcRect.top + szMinSize.cy;
	else if (rcRect.Height() > szMaxSize.cy)
		rcRect.bottom = rcRect.top + szMaxSize.cy;

	// Next, adjust window position by screen coordinates
	CRect rcWork;

	// Get work rect (depending on the number of monitors)
	if (::GetSystemMetrics(SM_CMONITORS) > 1)
	{
		HMONITOR hMon = MonitorFromRect(rcRect, MONITOR_DEFAULTTONEAREST);

		MONITORINFO mi = {};
		mi.cbSize = sizeof(mi);
		::GetMonitorInfoW(hMon, &mi);
		rcWork = mi.rcWork;

		// Reason to move is the same as below but before this move we need to
		// move work rect for second, third etc. monitor to default (0, 0) coordinates
		// i.e. work rect for any monitor with same resolution and taskbar position must be equal
		rcRect.MoveToX(rcRect.left + (mi.rcWork.left - mi.rcMonitor.left));
		rcRect.MoveToY(rcRect.top + (mi.rcWork.top - mi.rcMonitor.top));
	}
	else
	{
		RECT rc = {};
		::SystemParametersInfoW(SPI_GETWORKAREA, 0, &rc, 0);
		rcWork = rc;

		// Because we got these coordinates from GetWindowPlacement that uses workspace coordinates
		// we move our rect to screen coordinates. Otherwise we got "creep" effect when taskbar on top for example
		// See Remarks: http://msdn.microsoft.com/en-us/library/windows/desktop/ms632611(v=vs.85).aspx
		// http://blogs.msdn.com/b/oldnewthing/archive/2003/09/12/54896.aspx
		rcRect.MoveToX(rcRect.left + rcWork.left);
		rcRect.MoveToY(rcRect.top + rcWork.top);
	}

	// Finally, if window outside a monitor then move it back
	if (rcRect.left < rcWork.left)
		rcRect.MoveToX(rcWork.left);
	else if (rcRect.right > rcWork.right)
		rcRect.MoveToX(rcWork.right - rcRect.Width());

	if (rcRect.top < rcWork.top)
		rcRect.MoveToY(rcWork.top);
	else if (rcRect.bottom > rcWork.bottom)
		rcRect.MoveToY(rcWork.bottom - rcRect.Height());
}

void Settings::FixWinPosAlphaSet(bool isAlpha, const CRect& rcAlpha)
{
	if (isAlpha)
	{
		rcPosition.left -= rcAlpha.left;
		rcPosition.top -= rcAlpha.top;
		rcPosition.right += rcAlpha.right;
		rcPosition.bottom += rcAlpha.bottom;
	}
}

void Settings::FixWinPosAlphaGet(bool isAlpha, const CRect& rcAlpha)
{
	if (isAlpha)
	{
		rcPosition.left += rcAlpha.left;
		rcPosition.top += rcAlpha.top;
		rcPosition.right -= rcAlpha.right;
		rcPosition.bottom -= rcAlpha.bottom;
	}
}

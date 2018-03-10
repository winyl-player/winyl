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

#include "XmlFile.h"
#include "UTF.h"
#include "SkinTreeNode.h"

class Settings
{
public:
	static const int winylVersion = 30300;
	static const int dbaseVersion = 30000;
	static const int apiVersion = 10000;

public:
	Settings();
	virtual ~Settings();

	inline void SetProfilePath(const std::wstring& path) {profilePath = path;}

	inline void SetVolume(int volume) {soundVolume = volume;}
	inline void SetMute(bool isMute) {isSoundMute = isMute;}
	inline void SetRepeat(bool isRepeat) {isMediaRepeat = isRepeat;}
	inline void SetShuffle(bool isShuffle) {isMediaShuffle = isShuffle;}
	inline void SetHideToTray(bool isHide) {isHideToTray = isHide;}
	inline void SetWinPosition(CRect& rcPos) {rcPosition = rcPos;}
	inline void SetWinPositionMini(CRect& rcPos) {rcPositionMini = rcPos;}
	inline void SetLanguage(const std::wstring& name) {languageName = name;}
	inline void SetSkin(const std::wstring& name) {skinName = name;}
	inline void SetSkinPack(bool isPack) {isSkinPack = isPack;}
	inline void SetLastFM(bool isEnable) {isLastFM = isEnable;}
	//inline void SetMSN(bool isEnable) {isMSN = isEnable;}
	inline void SetPopup(bool isEnable) {isPopupEnable = isEnable;}
	inline void SetEffect(bool isEnable) {isEffectEnable = isEnable;}
	inline void SetPopupPosition(int position) {popupPosition = position;}
	inline void SetPopupEffect(int show, int hide)
	{
		popupShowEffect = show;
		popupHideEffect = hide;
	}
	inline void SetPopupDelay(int hold, int show, int hide)
	{
		popupHold = hold;
		popupShow = show;
		popupHide = hide;
	}
	inline void SetToolTips(bool isEnable) {isToolTips = isEnable;}
	inline void SetBassDevice(int device) {bassDevice = device;}
	inline void SetBassDriver(int driver) {bassDriver = driver;}
	inline void SetBassBit32(bool isBit32) {bassBit32 = isBit32;}
	inline void SetBassSoftMix(bool isSoftMix) {bassSoftMix = isSoftMix;}
	inline void SetProxy(int type) {proxyType = type;}
	inline void SetProxyHost(const std::wstring& host) {proxyHost = host;}
	inline void SetProxyPort(const std::wstring& port) {proxyPort = port;}
	inline void SetProxyLogin(const std::wstring& login) {proxyLogin = login;}
	inline void SetProxyPass(const std::wstring& pass) {proxyPass = pass;}
	inline void SetMiniPlayer(bool isPlayer) {isMiniPlayer = isPlayer;}
	inline void SetMaximized(bool isMax) {isMaximized = isMax;}
	inline void SetMiniTransparency(int transparency) {miniTransparency = transparency;}
	inline void SetMiniZOrder(int zOrder) {miniZOrder = zOrder;}
	inline void SetCheckForUpdates(bool isCheck) {isCheckForUpdates = isCheck;}
	inline void SetRescanRemoveMissing(bool isRemoveMissing) {isRescanRemoveMissing = isRemoveMissing;}
	inline void SetRescanIgnoreDeleted(bool isIgnoreDeleted) {isRescanIgnoreDeleted = isIgnoreDeleted;}
	inline void SetLyricsAlign(int align) {lyricsAlign = align;}
	inline void SetLyricsFontSize(int size) {lyricsFontSize = size;}
	inline void SetLyricsFontBold(bool bold) {lyricsFontBold = bold;}
	inline void SetLyricsProvider(const std::wstring& provider) {lyricsProvider = provider;}
	inline void SetLyricsProviderOff(bool off) {lyricsProviderOff = off;}

	inline int GetVolume() {return soundVolume;}
	inline bool IsMute() {return isSoundMute;}
	inline bool IsRepeat() {return isMediaRepeat;}
	inline bool IsShuffle() {return isMediaShuffle;}
	inline bool IsHideToTray() {return isHideToTray;}
	inline CRect& GetWinPosition() {return rcPosition;}
	inline CRect& GetWinPositionMini() {return rcPositionMini;}
	inline const std::wstring& GetLanguage() {return languageName;}
	inline const std::wstring& GetSkin() {return skinName;}
	inline bool IsSkinPack() {return isSkinPack;}
	inline const std::wstring& GetDefaultLanguage() {return languageNameDefault;}
	inline const std::wstring& GetDefaultSkin() {return skinNameDefault;}
	inline bool IsDefaultSkinPack() {return isSkinPackDefault;}
	inline bool IsFirstRun() {return isFirstRun;}
	inline bool IsLastFM() {return isLastFM;}
	//inline bool IsMSN() {return isMSN;}
	inline bool IsPopup() {return isPopupEnable;}
	inline bool IsEffect() {return isEffectEnable;}
	inline bool IsMiniPlayer() {return isMiniPlayer;}
	inline bool IsMaximized() {return isMaximized;}
	inline int GetPopupPosition() {return popupPosition;}
	inline int GetPopupShowEffect() {return popupShowEffect;}
	inline int GetPopupHideEffect() {return popupHideEffect;}
	inline int GetPopupHold() {return popupHold;}
	inline int GetPopupShow() {return popupShow;}
	inline int GetPopupHide() {return popupHide;}
	inline bool IsToolTips() {return isToolTips;}
	inline int GetBassDevice() {return bassDevice;}
	inline int GetBassDriver() {return bassDriver;}
	inline bool IsBassBit32() {return bassBit32;}
	inline bool IsBassSoftMix() {return bassSoftMix;}
	inline bool IsBassNoVolume() {return bassNoVolume;}
	inline bool IsBassNoEffect() {return bassNoEffect;}
	inline bool IsBassWasapiEvent() {return bassWasapiEvent;}
	inline int GetBassAsioChannel() {return bassAsioChannel;}
	inline const std::string& GetBassUserAgent() {return bassUserAgent;}
	inline int GetProxy() {return proxyType;}
	inline const std::wstring& GetProxyHost() {return proxyHost;}
	inline const std::wstring& GetProxyPort() {return proxyPort;}
	inline const std::wstring& GetProxyLogin() {return proxyLogin;}
	inline const std::wstring& GetProxyPass() {return proxyPass;}
	inline int GetMiniTransparency() {return miniTransparency;}
	inline int GetMiniZOrder() {return miniZOrder;}
	inline bool IsCheckForUpdates() {return isCheckForUpdates;}
	inline bool IsRescanRemoveMissing() {return isRescanRemoveMissing;}
	inline bool IsRescanIgnoreDeleted() {return isRescanIgnoreDeleted;}
	inline int GetLyricsAlign() {return lyricsAlign;}
	inline int GetLyricsFontSize() {return lyricsFontSize;}
	inline bool GetLyricsFontBold() {return lyricsFontBold;}
	inline const std::wstring& GetLyricsProvider() {return lyricsProvider;}
	inline bool IsLyricsProviderOff() {return lyricsProviderOff;}

	inline void SetPortableVersion(bool portable) {isPortableVersion = portable;}
	inline bool IsConvertVersion3() {return convertVersion3;}
	inline bool IsConvertPortable() {return (convertPortableTo || convertPortableFrom);}
	inline bool IsConvertPortableTo() {return convertPortableTo;}
	inline bool IsConvertPortableFrom() {return convertPortableFrom;}
	inline bool IsDBOldBeta() {return dbOldBeta;}

	inline void SetSearchType(int type) {searchType = type;}
	inline int GetSearchType() {return searchType;}

	bool LoadSettings();
	bool SaveSettings();

	inline bool IsLibraryType() {return stLibrary.isType;}
	inline bool IsLibraryValue() {return stLibrary.isValue;}
	inline bool IsLibraryArtist() {return stLibrary.isArtist;}
	inline bool IsLibraryAlbum() {return stLibrary.isAlbum;}

	inline void SetLibraryType(bool isThis, int type) {stLibrary.isType = isThis; stLibrary.type = type; stLibrary.isNoneOld = false;}
	inline void SetLibraryValue(bool isThis, const std::wstring& value) {stLibrary.isValue = isThis; stLibrary.value = value;}
	inline void SetLibraryArtist(bool isThis, const std::wstring& artist) {stLibrary.isArtist = isThis; stLibrary.artist = artist;}
	inline void SetLibraryAlbum(bool isThis, const std::wstring& album) {stLibrary.isAlbum = isThis; stLibrary.album = album;}
	inline void SetLibraryNoneOld() {stLibrary.isNoneOld = true;}

	inline int GetLibraryType() {return stLibrary.type;}
	inline const std::wstring& GetLibraryValue() {return stLibrary.value;}
	inline const std::wstring& GetLibraryArtist() {return stLibrary.artist;}
	inline const std::wstring& GetLibraryAlbum() {return stLibrary.album;}
	inline bool GetLibraryNoneOld() {return stLibrary.isNoneOld;}

	void FixWinPosMain(const CSize& szMinSize, const CSize& szMaxSize);
	void FixWinPosMini(const CSize& szMinSize, const CSize& szMaxSize);
	void FixRectByMonitor(CRect& rcRect, const CSize& szMinSize, const CSize& szMaxSize);

	void FixWinPosAlphaSet(bool isAlpha, const CRect& rcAlpha);
	void FixWinPosAlphaGet(bool isAlpha, const CRect& rcAlpha);

	inline void SetLibraryArtists(bool isEnable) {isLibraryArtists = isEnable;}
	inline void SetLibraryComposers(bool isEnable) {isLibraryComposers = isEnable;}
	inline void SetLibraryAlbums(bool isEnable) {isLibraryAlbums = isEnable;}
	inline void SetLibraryGenres(bool isEnable) {isLibraryGenres = isEnable;}
	inline void SetLibraryYears(bool isEnable) {isLibraryYears = isEnable;}
	inline void SetLibraryFolders(bool isEnable) {isLibraryFolders = isEnable;}
	inline void SetLibraryRadios(bool isEnable) {isLibraryRadios = isEnable;}
	inline void SetLibrarySmartlists(bool isEnable) {isLibrarySmartlists = isEnable;}

	inline void SetSmoothScroll(bool isEnable) {isSmoothScroll = isEnable;}
	inline bool IsSmoothScroll() {return isSmoothScroll;}

	inline void SetPlayFocus(bool isEnable) {isPlayFocus = isEnable;}
	inline bool IsPlayFocus() {return isPlayFocus;}

	inline void SetAddAllToLibrary(bool isEnable) {isAddAllToLibrary = isEnable;}
	inline bool IsAddAllToLibrary() {return isAddAllToLibrary;}

	inline void SetLastPlayIndex(long long index) {lastPlayIndex = index;}
	inline long long GetLastPlayIndex() {return lastPlayIndex;}

	inline bool IsLibraryArtists() {return isLibraryArtists;}
	inline bool IsLibraryComposers() {return isLibraryComposers;}
	inline bool IsLibraryAlbums() {return isLibraryAlbums;}
	inline bool IsLibraryGenres() {return isLibraryGenres;}
	inline bool IsLibraryYears() {return isLibraryYears;}
	inline bool IsLibraryFolders() {return isLibraryFolders;}
	inline bool IsLibraryRadios() {return isLibraryRadios;}
	inline bool IsLibrarySmartlists() {return isLibrarySmartlists;}
	inline bool IsLibraryNowPlaying() {return isLibraryNowPlaying;}

	inline void SetTagEditorCoverTags(bool save) {tagEditorCoverTags = save;}
	inline void SetTagEditorCoverFile(bool save) {tagEditorCoverFile = save;}
	inline void SetTagEditorLyricsTags(bool save) {tagEditorLyricsTags = save;}
	inline void SetTagEditorLyricsFile(bool save) {tagEditorLyricsFile = save;}
	inline bool IsTagEditorCoverTags() {return tagEditorCoverTags;}
	inline bool IsTagEditorCoverFile() {return tagEditorCoverFile;}
	inline bool IsTagEditorLyricsTags() {return tagEditorLyricsTags;}
	inline bool IsTagEditorLyricsFile() {return tagEditorLyricsFile;}

	inline void NewNowPlaying()
	{
		if (stLibrary.isNoneOld)
		{
			EmptyNowPlaying();
			return;
		}

		stNowPlaying.type     = stLibrary.type;
		stNowPlaying.isType   = stLibrary.isType;
		stNowPlaying.isValue  = stLibrary.isValue;
		stNowPlaying.isArtist = stLibrary.isArtist;
		stNowPlaying.isAlbum  = stLibrary.isAlbum;
		stNowPlaying.value    = stLibrary.value;		
		stNowPlaying.artist   = stLibrary.artist;
		stNowPlaying.album    = stLibrary.album;
	}
	inline void ReturnNowPlaying()
	{
		stLibrary.type     = stNowPlaying.type;
		stLibrary.isType   = stNowPlaying.isType;
		stLibrary.isValue  = stNowPlaying.isValue;
		stLibrary.isArtist = stNowPlaying.isArtist;
		stLibrary.isAlbum  = stNowPlaying.isAlbum;
		stLibrary.value    = stNowPlaying.value;		
		stLibrary.artist   = stNowPlaying.artist;
		stLibrary.album    = stNowPlaying.album;
	}
	inline bool IsNowPlaying()
	{
		if (stLibrary.type     == stNowPlaying.type &&
			stLibrary.isType   == stNowPlaying.isType &&
			stLibrary.isValue  == stNowPlaying.isValue &&
			stLibrary.isArtist == stNowPlaying.isArtist &&
			stLibrary.isAlbum  == stNowPlaying.isAlbum &&
			stLibrary.value    == stNowPlaying.value &&
			stLibrary.artist   == stNowPlaying.artist &&
			stLibrary.album    == stNowPlaying.album)
			return true;

		return false;
	}
	inline void EmptyNowPlaying()
	{
		stNowPlaying.type = 0;
		stNowPlaying.isType = false;
		stNowPlaying.isValue = false;
		stNowPlaying.isArtist = false;
		stNowPlaying.isAlbum = false;
		stNowPlaying.value.clear();
		stNowPlaying.artist.clear();
		stNowPlaying.album.clear();
	}
	inline void EmptyLibrary()
	{
		stLibrary.type = 0;
		stLibrary.isType = false;
		stLibrary.isValue = false;
		stLibrary.isArtist = false;
		stLibrary.isAlbum = false;
		stLibrary.value.clear();
		stLibrary.artist.clear();
		stLibrary.album.clear();
	}
	inline int GetNowPlayingType() {return stNowPlaying.type;}
	inline const std::wstring& GetNowPlayingValue() {return stNowPlaying.value;}



private:
	std::wstring profilePath;

	bool isFirstRun = false;

	int bassDriver = 0; // 0 - DirectSound, 1 - WASAPI, 2 - ASIO
	int bassDevice = -1; // Default Audio Device
	bool bassBit32 = true;
	bool bassSoftMix = false;

	bool bassNoVolume = false;
	bool bassNoEffect = false;

	bool bassWasapiEvent = false;
	int bassAsioChannel = 0;

	std::string bassUserAgent;

	int soundVolume = 100000;
	bool isSoundMute = false;
	bool isMediaRepeat = false;
	bool isMediaShuffle = false;

	bool isHideToTray = false;
	bool isMaximized = false;

	bool isMiniPlayer = false;
	int miniTransparency = 0;
	int miniZOrder = 0;

	bool isPopupEnable = true;
	int popupPosition = 3;
	int popupShowEffect = 0;
	int popupHideEffect = 0;
	int popupHold = 4000;
	int popupShow = 500;
	int popupHide = 2000;

	int searchType = 0;
	bool isEffectEnable = true;
	bool isSmoothScroll = true;
	bool isLastFM = false;
	//bool isMSN = false;
	bool isToolTips = true;
	bool isCheckForUpdates = true;
	//bool isAssociations = false;

	CRect rcPosition = {200, 120, 200 + 720, 120 + 520};
	CRect rcPositionMini = {200, 120, 200 + 100, 120 + 100};

	std::wstring languageNameDefault =  L"English";
	std::wstring skinNameDefault = L"Flat";
	bool isSkinPackDefault = true;

	std::wstring languageName = languageNameDefault;
	std::wstring skinName = skinNameDefault;
	bool isSkinPack = isSkinPackDefault;

	struct LIBRARY
	{
		int type = 0;
		bool isType = false; // type is used
		bool isValue = false; // type, value are used
		bool isArtist = false; // type, value, artist are used
		bool isAlbum = false; // type, value, artist, album are used
		std::wstring value;
		std::wstring artist;
		std::wstring album;

		// If isNoneOld == true then above values are not for the current list but for the previous
		// It's not used in stNowPlaying but used to fill it
		bool isNoneOld = false;
	}stLibrary, stNowPlaying;

	int proxyType = 0;
	std::wstring proxyHost;
	std::wstring proxyPort = L"80";
	std::wstring proxyLogin;
	std::wstring proxyPass;

	bool isLibraryArtists = true;
	bool isLibraryComposers = false;
	bool isLibraryAlbums = false;
	bool isLibraryGenres = true;
	bool isLibraryYears = false;
	bool isLibraryFolders = true;
	bool isLibraryRadios = true;
	bool isLibrarySmartlists = true;
	bool isLibraryNowPlaying = true;

	bool isPlayFocus = false;

	bool isAddAllToLibrary = true;

	long long lastPlayIndex = 0;

	bool isRescanRemoveMissing = true;
	bool isRescanIgnoreDeleted = true;

	bool isPortableVersion = false;

	int lyricsAlign = 0; // Center
	int lyricsFontSize = 0; // From Skin
	bool lyricsFontBold = false;
	std::wstring lyricsProvider;
	bool lyricsProviderOff = false;

	bool tagEditorCoverTags = true;
	bool tagEditorCoverFile = false;
	bool tagEditorLyricsTags = true;
	bool tagEditorLyricsFile = false;

	bool convertVersion3 = false;
	bool convertPortableTo = false;
	bool convertPortableFrom = false;

	bool dbOldBeta = false;
};

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
#include <functional>
#include "WindowEx.h"
#include "ExImage.h"
#include "SkinScroll.h"
#include "Threading.h"


class SkinLyrics : public WindowEx
{

public:
	SkinLyrics();
	~SkinLyrics();

	bool NewWindow(HWND parent);
	void SetBmBack(HBITMAP* bm) {bmBack = bm;}
	bool LoadSkin(std::wstring& file, ZipFile* zipFile);

	void UpdateLyrics(std::vector<std::wstring>&& newlines);
	void UpdateLyricsNull();

	bool IsWindowVisible() {return isWindowVisible;}

	void RegisterCallbackShowWindow(const std::function<void()>& callback) {callbackShowWindow = callback;}

	void SetFontSize(int size, bool bold);
	bool GetFontBoldDefault() {return fontBoldDefault;}
	void SetAlign(int align);

	std::wstring GetLyrics();

	void SetState(bool play, bool recv, bool empty) {statePlay = play; stateRecv = recv; stateEmpty = empty;}

	void MouseWheel(bool isReverse);

	inline void EnableSmoothScroll(bool isEnable) {isSmoothScrollEnabled = isEnable;}
	inline void SetEventSmoothScroll(Threading::Event* event) {eventSmoothScroll = event;}
	inline bool IsSmoothScrollRun() {return isSmoothScrollRun;}
	void SmoothScrollRun();

	inline void SetLanguageNotFound(const std::wstring& str) {stringNotFound = str;}
	inline void SetLanguageNoLyrics(const std::wstring& str) {stringNoLyrics = str;}
	inline void SetLanguageReceiving(const std::wstring& str) {stringReceiving = str;}

private:
	std::function<void()> callbackShowWindow;

	std::vector<std::wstring> lines;
	std::vector<int> linesHeight;

	bool isWindowVisible = false;
	CSize szWindowHidden = {0, 0};

	int heightLine = 0;
	int heightLines = 0;

	unsigned backColor = 0x00FFFFFF;
	bool isBackTransparent = false;
	HBITMAP* bmBack = nullptr;
	unsigned fontColorLyrics = 0;
	unsigned fontColorNoLyrics = 0;
	HFONT fontLyrics = NULL;
	HFONT fontNoLyrics = NULL;

	int fontSizeDefault = 0;
	bool fontBoldDefault = 0;
	int lyricsAlign = 0;

	bool statePlay = false;
	bool stateRecv = false;
	bool stateEmpty = false;

	std::wstring stringNotFound;
	std::wstring stringNoLyrics;
	std::wstring stringReceiving;

	struct MARGIN
	{
		int left = 0;
		int right = 0;
		int top = 0;
		int bottom = 0;
	} margin;

	int GetHeightLine();
	int GetHeightLines(int cx, int cy);

	void DrawTextLines(HDC dc, int x, int y, int right, int height, int cx);

	void CreateGradient(HDC dc, const CRect& rcClient);
	int fadeTopBottom = 0;
	ExImage fadeTop;
	ExImage fadeBottom;
	int fadeWinSizeX = 0;
	int fadeWinSizeY = 0;

	void ResetScroll(int cx, int cy);
	void ResetScrollImpl(int cx, int cy);

	HWND nativeScroll = NULL; // Native Windows scroll
	std::unique_ptr<SkinScroll> skinScroll; // Skin scroll, if nullptr use native
	int scrollWidth = 0; // Scroll width, if 0 then scroll is hidden

	// Smooth scrolling
	bool isSmoothScrollEnabled = false;
	int smoothScrollAcc = 0;
	int smoothScrollNew = 0;
	std::atomic<bool> isSmoothScrollRun = false;
	Threading::Event* eventSmoothScroll = nullptr;
	void SmoothScroll(int scroll);
	void StopSmoothScroll();

	// Helper functions for scroll (native and skin)
	// The class interact with scroll throught these functions.
	// These functions does not interact with class only with scroll,
	// only exception in HScrollMessage when native scroll.
	int HScrollGetPos();
	bool HScrollSetPos(int pos, bool needRedraw = true);
	int HScrollGetPage();
	void HScrollSetInfo(int min, int max, int line, int page);
	int HScrollGetMax();
	int HScrollGetMin();
	int HScrollGetWidth();
	void HScrollShow(bool show);
	void HScrollResize(int x, int y, int cx, int cy);
	bool HScrollMessage(int message);
	int HScrollMessageGetPosOld();

private: // Messages
	LRESULT WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
	void OnPaint(HDC dc, PAINTSTRUCT& ps);
	void OnTimer(UINT_PTR nIDEvent);
	void OnSize(UINT nType, int cx, int cy);
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnVScroll(UINT nSBCode, UINT nPos, HWND hScrollBar);
	void OnContextMenu(HWND hWnd, CPoint point);
};


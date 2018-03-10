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
#include "SkinLayout.h"
#include "SkinElement.h"
#include "SkinTrigger.h"
#include "CoverLoader.h"
#include "Threading.h"
#include "Language.h"

class SkinDraw
{

public:
	SkinDraw();
	virtual ~SkinDraw();

	bool LoadSkin(const std::wstring& programPath, const std::wstring& skinName, ZipFile* zipFile, bool isMini = false);
	bool LoadSkinAlpha(const std::wstring& programPath, const std::wstring& skinName, ZipFile* zipFile);
	bool LoadSkinSettings(const std::wstring& profilePath, const std::wstring& skinName);
	bool SaveSkinSettings(const std::wstring& profilePath, const std::wstring& skinName);
	void SetWindowStyle(HWND wnd, HWND timer);
	std::unique_ptr<ZipFile> NewZipFile(const std::wstring& programPath, const std::wstring& skinName);

	void Paint(HDC dc, PAINTSTRUCT& ps);
	void Resize(int cx, int cy, bool needRedraw);

	void RefreshWindow();
	void RedrawWindow();

	void Minimized();

	// The following 3 functions return an element that did action or nullptr if no action
	SkinElement* MouseMove(unsigned flags, CPoint& point);
	SkinElement* MouseDown(unsigned flags, CPoint& point);
	SkinElement* MouseUp(unsigned flags, CPoint& point);

	void MouseLeave();

	//inline bool IsLayered() {return isLayered;}
	inline bool IsAeroGlass() {return isAeroGlass;}
	inline bool IsLayeredAlpha() {return isLayeredAlpha;}

	inline bool IsStyleBorder() {return isStyleBorder;}

	inline bool IsPressElement() {return (pressElement ? true:false);} // Element is pressed?
	inline bool IsHoverElement() {return (hoverElement ? true:false);} // Mouse over element?
	inline bool IsClickElement() {return (clickElement ? true:false);} // Used only to check double click

	inline void EmptyClick() {clickElement = nullptr;} // Should be called when click on empty space

	inline SkinElement* GetPressElement() {return pressElement;}
	inline SkinElement* GetHoverElement() {return hoverElement;}

	inline bool IsSplitterHover() {return (hoverElement && hoverElement->GetType() == SkinElement::Type::Splitter);}
	inline bool IsSplitterPress() {return (pressElement && pressElement->GetType() == SkinElement::Type::Splitter);}

	inline bool IsShadowNative() {return isShadowDwm;}
	inline bool IsShadowLayered() {return isShadowLayered;}

	void ChangeTrigger(SkinTrigger* trigger, int state);

	void SplitterLayout(SkinElement* element, int x, int y);
	void SplitterLayoutClick(SkinElement* element, int x, int y);

	inline std::vector<std::unique_ptr<SkinLayout>>& Layouts() {return layouts;}

	///////

	void DrawPosition(int percent);
	void DrawVolume(int percent);
	void DrawPlay(bool isPlay);
	void DrawMute(bool isMute);
	void DrawShuffle(bool isShuffle);
	void DrawRepeat(bool isRepeat);
	void DrawMaximize(bool isMaximize);

	void DrawText(const std::wstring& title, const std::wstring& album, const std::wstring& artist,
				const std::wstring& genre, const std::wstring& year, int time);
	void DrawTextNone();

	void DrawTime(int time, int length = 0, bool isLength = false);

	void DrawStatusLine(int count, int total, int time, long long size, Language* lang);
	void DrawStatusLineNone();
	
	void DrawCover(ExImage::Source* image);

	void DrawRating(int rating);

	void DrawSearchClear(bool isShow);

	void EnableFade(bool isEnable);

	void EnableDwm(bool isEnable);

	void EnableDwmShadow();

	void SetVisible(bool isShow);

	void SetOpacity(int opacity255);

	// Get sizes for owner window
	inline CSize& GetMinSize() {return szMinSize;}
	inline CSize& GetMaxSize() {return szMaxSize;}
	inline CRect& GetResizeBorder() {return rcResizeBorder;}
	inline CRect& GetMaximizeBorder() {return rcMaximizeBorder;}
	inline CRect& GetAlphaBorder() {return rcAlphaBorder;}

	inline void SetSkinDrawMini(SkinDraw* skinDraw) {drawMini = skinDraw;}
	inline void SetSkinDrawAlpha(SkinDraw* skinDraw) {drawAlpha = skinDraw;}

	void DrawTriggerSwitch(SkinElement* element, int state);

	bool FadeElement();

	HBITMAP* GetBmBack() {return &bmBackgd;}

private:
	std::vector<std::unique_ptr<SkinLayout>> layouts; // Virtual layouts that represent the window

	SkinDraw* drawMini = nullptr;
	SkinDraw* drawAlpha = nullptr;

	bool isLayered = false;
	bool isAeroGlass = false;
	bool isDwmEnabled = false;
	bool isLayeredAlpha = false; // If set then the class used by Alpha Window (SkinAlpha)
	bool isMiniPlayer = false;

	bool isStyleBorder = false;
	bool isStyleCaption = false;
	bool isStyleMaximize = false;

	bool isFadeEnabled = false;

	bool isShadowDwm = false;
	bool isShadowLayered = false;

	bool isLayoutsSave = false; // Allow saving skin layouts
	std::string layoutsVersion;

	HWND wndOwner = NULL;
	HWND wndTimer = NULL;

	HDC dcScreen = NULL;

	HBITMAP bmMemory = NULL; // Window cache
	HBITMAP bmBackgd = NULL; // Background cache

	SkinElement* pressElement = nullptr;
	SkinElement* hoverElement = nullptr;
	SkinElement* clickElement = nullptr;

	int valueOpacity = 255; // Window opacity

	int valueCorners = 0; // Cut window corners (only without layered style)

	int splitterLayoutX = 0;
	int splitterLayoutY = 0;

	void UnloadSkin();

	void RedrawElement(SkinElement* element, bool isForceUpdate = false);
	void RedrawWindowDefault();
	void RedrawWindowLayered();
	void RedrawElementDefault(SkinElement* element);
	void RedrawElementLayered(SkinElement* element);

	void RedrawLayout(SkinLayout* layout);
	void RedrawLayoutDefault(SkinLayout* layout);
	//void RedrawLayoutLayered(SkinLayout* layout);

	void ForceUpdate() {if (!isLayered) ::UpdateWindow(wndOwner);}

	void DrawSwitch(SkinElement::Type type, int state);

	// Animation

	void RedrawElementFade(SkinElement* element);

	bool FadeElementDefault();
	bool FadeElementLayered();

	std::vector<SkinElement*> fadeElements;

	// Default values
	struct DefaultSize
	{
		static const int MinSizeX = 20;
		static const int MinSizeY = 20;
		static const int MaxSizeX = 10000;
		static const int MaxSizeY = 10000;

		static const int ResizeLeft   = 0;
		static const int ResizeTop    = 0;
		static const int ResizeRight  = 0;
		static const int ResizeBottom = 0;

		static const int MaxLeft   = 0;
		static const int MaxTop    = 0;
		static const int MaxRight  = 0;
		static const int MaxBottom = 0;
	};

	// Sizes for owner window, not used in this class but it load them
	CSize szMinSize = {DefaultSize::MinSizeX, DefaultSize::MinSizeY};
	CSize szMaxSize = {DefaultSize::MaxSizeX, DefaultSize::MaxSizeY};
	CRect rcResizeBorder = {DefaultSize::ResizeLeft, DefaultSize::ResizeTop, DefaultSize::ResizeRight, DefaultSize::ResizeBottom};
	CRect rcMaximizeBorder = {DefaultSize::MaxLeft, DefaultSize::MaxTop, DefaultSize::MaxRight, DefaultSize::MaxBottom};
	CRect rcAlphaBorder = {0, 0, 0, 0};

	MARGINS mrGlass = {-1, -1, -1, -1};
};

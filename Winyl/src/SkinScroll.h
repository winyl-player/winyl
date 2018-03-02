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

#include "WindowEx.h"
#include "ExImage.h"
#include "UTF.h"
#include "XmlFile.h"


// SkinScroll

class SkinScroll : public WindowEx
{
//	DECLARE_DYNAMIC(SkinScroll)

public:
	SkinScroll();
	virtual ~SkinScroll();

	bool CreateScroll(HWND parent, const std::wstring& fileSkin, ZipFile* zipFile);
	void SetBmBack(HBITMAP* bm) {bmBack = bm;}
	
	bool SetScrollPos(int pos, bool needRedraw = true);
	void ShowScroll(bool isShow);
	void SetScrollInfo(int min, int max, int line, int page);
	inline int GetScrollPos() {return scrollPos;}
	inline int GetScrollPage() {return scrollPage;}
	inline int GetScrollLine() {return scrollLine;}
	inline int GetScrollMin() {return scrollMin;}
	inline int GetScrollMax() {return scrollMax;}
	inline int GetScrollWidth() {return scrollWidth;}

	inline int GetScrollPosOld() {return scrollPosOld;}

private:
	bool LoadSkin(const std::wstring& file, ZipFile* zipFile);

	void DrawScroll(HDC dc);
	void Redraw();

	int scrollPos = 0; // Scroll current position
	int scrollMin = 0; // Scroll min position
	int scrollMax = 100; // Scroll max position
	int scrollPage = 10; // Scroll page size
	int scrollLine = 1; // Scroll line size
	int scrollPosOld = 0; // Scroll old position
	int scrollPosReturn = 0; // Scroll return pos (when mouse leaves scroll area)

	// Values that are used to draw and calc

	int scrollWidth = 20; // Scroll width
	int scrollHeight = 0; // Scroll height

	int sizeArrowUp = 0; // Up arrow size (offset for thumb from above)
	int sizeArrowDown = 0; // Down arrow size (offset for thumb from below)

	int thumbPos = 0; // Thumb current position
	int thumbSize = 0; // Thumb size
	
	int thumbMove = 0; // To calc thumb pos when thumb are moving
	int thumbMinSize = 0; // Thumb min size

	// Values that are used to draw (should be used only to draw)

	bool isPressUp = false; // Up is pressed
	bool isPressDown = false; // Down is pressed
	bool isPressThumb = false; // Thumb is pressed

	bool isHoverUp = false; // Mouse over Up
	bool isHoverDown = false; // Mouse over Down
	bool isHoverThumb = false; // Mouse over Thumb

	enum class ActionType
	{
		None     = 0,
		Thumb    = 1,
		LineUp   = 2,
		LineDown = 3,
		PageUp   = 4,
		PageDown = 5
	};

	ActionType actionType = ActionType::None; // Current action

	bool isShiftDown = false;

	bool isTimer = false; // Timer is started?

	// Images to draw

	int backSize = 0;
	unsigned backColor = 0x00FFFFFF;
	HBITMAP* bmBack = nullptr;

	ExImage imTrack;
	
	ExImage imArrowUp[3];
	ExImage imArrowDown[3];

	ExImage imThumbBegin[3];
	ExImage imThumbFill[3];
	ExImage imThumbEnd[3];

	ExImage imThumbMiddle[3];

	// Functions

	void ScrollThumb(CPoint& pt);
	void ScrollUpDown();
	void ScrollMessage();

	ActionType GetActionType(const CPoint &pt, bool isPageUpDown = true);

	void StartTimer(bool isFast);
	void StopTimer();

	void CalcThumbByPos(int pos); // Calc thumb position by scroll position
	int CalcPosByThumb(); // Calc scroll position by thumb position
	int MulDivide(int number, int numerator, int denominator);

	void SetPress(const CPoint& pt);
	void SetHover(const CPoint& pt);
	void ResetPress();
	void ResetHover();

	// Check if need to redraw one of the states
	bool IsRedrawPressSet(ActionType type);
	bool IsRedrawPressReset(ActionType type);
	bool IsRedrawHoverSet(ActionType type);
	bool IsRedrawHoverReset(ActionType type);

private: // Messages
	LRESULT WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
	void OnPaint(HDC dc, PAINTSTRUCT& ps);
	void OnTimer(UINT_PTR nIDEvent);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnMouseLeave();
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
};



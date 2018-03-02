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

// SkinPopup

#include "WindowEx.h"
#include "ExImage.h"
#include "XmlFile.h"
#include "UTF.h"
#include "SkinPopupElement.h"

class SkinPopup : public WindowEx
{

public:
	SkinPopup();
	virtual ~SkinPopup();

	bool NewWindow(HWND parent);
	static bool IsSkinFile(const std::wstring& programPath, const std::wstring& skinName, ZipFile* zipFile);
	bool LoadSkin(const std::wstring& programPath, const std::wstring& skinName, ZipFile* zipFile);
	void UnloadSkin();
	void SetText(const std::wstring& title, const std::wstring& album, const std::wstring& artist,
				const std::wstring& genre, const std::wstring& year, int time);
	void SetCover(ExImage::Source* image);
	void Popup();
	void Disable();
//	inline bool IsLoaded() {return true;}

	enum class PositionType // Window position
	{
		TopLeft     = 0,
		TopRight    = 1,
		BottomLeft  = 2,
		BottomRight = 3
	};

	enum class EffectType // Window animation effect
	{
		Fade = 0,
		Vert = 1,
		Horz = 2
	};
	
	inline void SetPosition(int pos) {setPosition = (PositionType)pos;}
	inline void SetEffect(int show, int hide)
	{
		setShowEffect = (EffectType)show;
		setHideEffect = (EffectType)hide;
	}
	inline void SetDelay(int hold, int show, int hide)
	{
		setTimerHold = hold;
		setTimerShow = show;
		setTimerHide = hide;
	}

private:
	struct TimerValue // Timer constants
	{
		// 15, 10, 5

		static const int Time = 10; // Update period
		static const int Div = 16;

		static const int ID = 100; // Timer ID
	};

	enum class TimerAction // Current action of the timer
	{
		None = 0, // Timer is stopped (window is hidden)
		Hold = 1, // Timer is waiting
		Show = 2, // Timer is showing the window with animation
		Hide = 3  // Timer is hiding the window with animation
	};

	float timer = 0.0f; // Current value ot the timer (from 0 to 255)
	TimerAction timerAction = TimerAction::None; // Current action of the timer

	int timerHold = 0; // Hold time
	int timerShow = 0; // Show time
	int timerHide = 0; // Hide time

	PositionType position = PositionType::TopLeft; // Window position
	EffectType showEffect = EffectType::Fade; // Show effect
	EffectType hideEffect = EffectType::Fade; // Hide effect

	struct MarginDef
	{
		static const int SizeX = 5; // Offset from corner by X
		static const int SizeY = 5; // Offset from corner by Y
	};

	// Default settings
	int setTimerHold = 4000;
	int setTimerShow = 500;
	int setTimerHide = 2000;
	PositionType setPosition = PositionType::BottomRight;
	EffectType setShowEffect = EffectType::Fade;
	EffectType setHideEffect = EffectType::Fade;

	bool isSkinLoad = false;

	bool isMouseOnWindow = false; // Mouse over the window
	bool isMenuPopup = false; // Popup menu is activated

	bool isLayered = false; // Layered window style
	int valueCorners = 0; // Cut window corners (only without layered style)

	HDC dcScreen = NULL;
	HDC dcWindow = NULL;
	HBITMAP bmMemory = NULL;

	bool isBackground = true;
	ExImage imBackground[3];
	std::vector<std::unique_ptr<SkinPopupElement>> elements;

	void RedrawWindowDefault();
	void RedrawWindowLayered();
	void AlphaWindowLayered(int opacity);

	HWND wndParent = NULL;

	int width = 0; // Window width
	int height = 0; // Window height
	int minWidth = 0; // Min window width
	int maxWidth = 0; // Max window width

	CRect rcWindow = {0, 0, 0, 0};

	void SetTimerAction(TimerAction action);
	void PrepareWindow();
	void FadeWindow(bool isLastStart, bool isLastEnd);
	void SlideVertWindow(bool isLastStart, bool isLastEnd);
	void SlideHorzWindow(bool isLastStart, bool isLastEnd);
	

private: // Messages
	LRESULT WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
	void OnPaint(HDC dc, PAINTSTRUCT& ps);
	void OnTimer(UINT_PTR nIDEvent);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnMouseLeave();
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnContextMenu(HWND hWnd, CPoint point);
};

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

// SkinAlpha

#include "WindowEx.h"
#include "SkinDraw.h"
#include "MoveResize.h"
#include "ContextMenu.h"
#include "ContextMenu.h"
#include "ToolTips.h"
#include "LibAudio.h"

class SkinAlpha : public WindowEx
{

public:
	SkinAlpha();
	virtual ~SkinAlpha();

	bool NewWindow(HWND parent);

	bool LoadSkin(const std::wstring& programPath, const std::wstring& skinName, ZipFile* zipFile);

	void SetLibAudio(LibAudio* bass, bool *ptrPlay, bool *ptrRadio) {libAudio = bass; ptrMediaPlay = ptrPlay; ptrMediaRadio = ptrRadio;}
	void SetMoveResize(MoveResize* move) {moveResize = move;}
	void SetContextMenu(ContextMenu* menu) {contextMenu = menu;}

	SkinDraw skinDraw;
	ToolTips toolTips;

	bool isTrackTooltip = false;

	struct TimerValue
	{
		static const int Track   = 200;
		static const int TrackID = 500;
	};

private:
	HWND wndParent = NULL;

	LibAudio* libAudio = nullptr;
	bool *ptrMediaPlay;
	bool *ptrMediaRadio;

	MoveResize* moveResize = nullptr;
	ContextMenu* contextMenu = nullptr;

	enum class MouseAction
	{
		None = 0,
		Move = 1,
		Down = 2,
		Up   = 3
	};

	void Action(SkinElement* element, MouseAction mouseAction);

private: // Messages
	LRESULT WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
	void OnPaint(HDC dc, PAINTSTRUCT& ps);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnMouseLeave();
	void OnNcHitTest(CPoint point);
	bool OnSetCursor(HWND hWnd, UINT nHitTest, UINT message);
	void OnSize(UINT nType, int cx, int cy);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnContextMenu(HWND hWnd, CPoint point);
	void OnNotify(WPARAM wParam, LPARAM lParam);
	void OnTimer(UINT_PTR nIDEvent);
};



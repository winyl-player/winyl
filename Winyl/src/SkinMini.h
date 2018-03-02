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

// SkinMini

#include "WindowEx.h"
#include "MoveResize.h"
#include "SkinDraw.h"
#include "SkinVis.h"
#include "FutureWin.h"
#include "ContextMenu.h"
#include "Settings.h"

class SkinMini : public WindowEx
{

public:
	SkinMini();
	virtual ~SkinMini();

	bool NewWindow(HWND parent, CRect& rcPos, HICON iconLarge, HICON iconSmall);
	HWND wndParent = NULL;

	void SetContextMenu(ContextMenu* menu) {contextMenu = menu;}

	static bool IsSkinFile(const std::wstring& programPath, const std::wstring& skinName, ZipFile* zipFile);
	bool LoadSkin(const std::wstring& programPath, const std::wstring& skinName, ZipFile* zipFile);

	void SetVisible(bool isVisible);
	void SetTransparency(int transparency);

	void SetZOrder(int newZOrder);
	int GetZOrder() {return zOrder;}

	CRect rcValidRect = {0, 0, 0, 0};

	SkinDraw skinDraw;
	std::vector<std::unique_ptr<SkinVis>> visuals;

private:
	MoveResize moveResize;

	ContextMenu* contextMenu = nullptr;

	int zOrder = 0;

	bool LoadWindows();

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
	void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	void OnSize(UINT nType, int cx, int cy);
	void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnContextMenu(HWND hWnd, CPoint point);
	bool OnSysCommand(WPARAM wParam, LPARAM lParam);
	void OnDestroy();
	void OnDropFiles(HDROP hDropInfo);
};



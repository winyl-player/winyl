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

// ToolTips

#include "Language.h"
#include "SkinDraw.h"
#include "SkinElement.h"

class ToolTips
{

public:
	ToolTips();
	virtual ~ToolTips();

	inline void SetLanguage(Language* language) {lang = language;}
	inline void SetWindow(HWND wnd, SkinDraw* draw) {wndOwner = wnd; skinDraw = draw;}
	void Create();
	void Destroy();
	void RelayEvent(MSG* msg);
	const std::wstring* GetText(SkinElement* element);
	HWND GetTipWnd() {return toolTip;}
	
	void SetToolRect(void* ptrID, CRect& rc);

	void DeactivateTrackingToolTip();
	void TrackingToolTip(bool activate, const std::wstring& value, int x, int y, bool bound = false);

private:
	Language* lang = nullptr;
	HWND toolTip = NULL;
	HWND wndOwner = NULL;

	SkinDraw* skinDraw = nullptr;

	HWND trackingTip = NULL;

	void CreateTrackingToolTip();
};



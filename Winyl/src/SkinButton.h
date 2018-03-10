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

#include "SkinElement.h"

class SkinButton : public SkinElement
{

public:
	SkinButton();
	virtual ~SkinButton();

	ExImage imButton[3];

	bool LoadSkin(const std::wstring& file, ZipFile* zipFile) override;
	void Draw(HDC dc, bool isAlpha) override;
	bool DrawFade(HDC dc, bool isAlpha) override;

	int GetWidth() override;
	int GetHeight() override;

	bool IsRedrawHover() override;
	bool IsRedrawPress() override;

	SkinElement* OnMouseMove(unsigned flags, CPoint& point) override;
	SkinElement* OnButtonDown(unsigned flags, CPoint& point) override;
	SkinElement* OnButtonUp(unsigned flags, CPoint& point) override;
	SkinElement* OnMouseLeave() override;

	// Enable tooltip for this class
	bool SetToolTip(ToolTips* tips) override {toolTip = tips; return true;}
};



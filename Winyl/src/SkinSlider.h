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

class SkinSlider : public SkinElement
{

public:
	SkinSlider();
	virtual ~SkinSlider();

	ExImage imBegin;
	ExImage imEnd;
	ExImage imFill[2];
	ExImage imButton[3];

	int percent = 0;
	int position = 0;
	int lastPosition = 0;


	bool isBegin = false;
	bool isEnd = false;
	bool isBackground = false;
	bool isButton = false;

	bool LoadSkin(const std::wstring& file, ZipFile* zipFile) override;
	void SetRect(CRect& rcDraw) override;
	void Draw(HDC dc, bool isAlpha) override;

	int GetWidth() override;
	int GetHeight() override;

	bool IsRedrawHover() override;
	bool IsRedrawPress() override;

	SkinElement* OnMouseMove(unsigned flags, CPoint& point) override;
	SkinElement* OnButtonDown(unsigned flags, CPoint& point) override;
	SkinElement* OnButtonUp(unsigned flags, CPoint& point) override;
	SkinElement* OnMouseLeave();

	int GetParam() override;
	void SetParam(int param) override;

	int GetThumbPosition();
	int CalcPercent(const CPoint& point);

private:
	void CalcPositionByPoint(CPoint point, int& outPosition, int& outPercent);
};



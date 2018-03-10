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

class SkinSplitter : public SkinElement
{

public:
	SkinSplitter();
	virtual ~SkinSplitter();

	bool LoadSkin(const std::wstring& file, ZipFile* zipFile) override;

	SkinElement* OnMouseMove(unsigned flags, CPoint& point) override;
	SkinElement* OnButtonDown(unsigned flags, CPoint& point) override;
	SkinElement* OnButtonUp(unsigned flags, CPoint& point) override;
	SkinElement* OnMouseLeave() override;

	bool IsHorizontal() {return isHorizontal;}
	bool IsVertical() {return isVertical;}
	CRect& GetMaxLimit() {return rcMax;}

	struct structSplitter
	{
		int id;

		bool isMoveLeft;
		bool isMoveRight;
		bool isMoveTop;
		bool isMoveBottom;
	};

private:
	bool isHorizontal = false;
	bool isVertical = false;

	CRect rcMax = {0, 0, 0, 0};

	std::vector<structSplitter> layouts;

public:
	inline std::vector<structSplitter>& Layouts() {return layouts;}
};



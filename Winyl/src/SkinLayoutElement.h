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

// SkinLayoutElement

#include "XmlFile.h"
#include "UTF.h"

#include "SkinElement.h"
#include "SkinButton.h"
#include "SkinSlider.h"
#include "SkinSwitch.h"
#include "SkinCover.h"
#include "SkinRating.h"
#include "SkinText.h"
//#include "SkinSearch.h"
#include "SkinSplitter.h"

class SkinLayoutElement
{

public:
	SkinLayoutElement();
	virtual ~SkinLayoutElement();
	SkinLayoutElement(const SkinLayoutElement&) = delete;
	SkinLayoutElement& operator=(const SkinLayoutElement&) = delete;

	struct structPos
	{
		int width;   // Width
		int height;  // Height
		int left;    // Left indent
		int right;   // Right indent
		int top;     // Top indent
		int bottom;  // Bottom indent
		int leftRight; // Center indent X
		int topBottom; // Center indent Y
		bool isLeft;   // Left indent in priority
		bool isTop;    // Top indent in priority
		bool isRight;  // Right indent in priority
		bool isBottom; // Bottom indent in priority
		bool isWidth;  // Width in priority
		bool isHeight; // Height in priority
		bool isLeftRight; // Center indent X in priority
		bool isTopBottom; // Center indent Y in priority
	}pos;

	std::unique_ptr<SkinElement> element;

	int width = 0;
	int height = 0;

	void LoadSkin(XmlNode& xmlNode, const std::wstring& path, ZipFile* zipFile, bool isHidden);
	bool LoadPosition(XmlNode& xmlNode);
	void LoadType(XmlNode& xmlNode);
	void UpdateRect(CRect& rcRect);
	void Draw(HDC dc, bool isAlpha);
};



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
#include "ExImage.h"
#include "UTF.h"

class SkinLayoutBack
{
public:
	SkinLayoutBack();
	virtual ~SkinLayoutBack();
	SkinLayoutBack(const SkinLayoutBack&) = delete;
	SkinLayoutBack& operator=(const SkinLayoutBack&) = delete;

	enum class Type // Image type
	{
		None  = -404,

		Image = 0,  // Image
		Line  = 1,  // Outline rectangle
		Fill  = 2   // Filled rectangle
	};

	struct DefaultFill // Default size
	{
		static const int Width  = 20;
		static const int Height = 20;
	};

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
		bool isWidth;  // Width in priority
		bool isHeight; // Height in priority
		bool isLeft;   // Left indent in priority
		bool isTop;    // Top indent in priority
		bool isRight;  // Right indent in priority
		bool isBottom; // Bottom indent in priority
		bool isLeftRight; // Center indent X in priority
		bool isTopBottom; // Center indent Y in priority
	}pos;

	struct structElm
	{
		COLORREF color;
		ExImage image;
		bool tile;
	}elm;

	Type type = Type::None;

	bool LoadSkin(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile);
	Type GetType(XmlNode& xmlNode);
	bool LoadImage(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile);
	bool LoadLine(XmlNode& xmlNode);
	bool LoadFill(XmlNode& xmlNode);
	bool LoadPosition(XmlNode& xmlNode);
	void Draw(HDC dc, CRect& rcRect);
};




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

// SkinTreeBack

#include "XmlFile.h"
#include "UTF.h"
#include "ExImage.h"


class SkinTreeBack
{
public:
	SkinTreeBack();
	virtual ~SkinTreeBack();
	SkinTreeBack(const SkinTreeBack&) = delete;
	SkinTreeBack& operator=(const SkinTreeBack&) = delete;

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
		static const int Height = 2;
	};

	struct structPos
	{
		bool isLoad;
		int left;    // Left indent
		int right;   // Right indent
		int top;     // Top indent
		int bottom;  // Bottom indent
		int width;   // Width
		int height;  // Height
		bool isLeft;   // Left indent in priority (else right indent)
		bool isTop;    // Top indent in priority (else bottom indent)
		bool isWidth;  // Width in priority (else right indent)
		bool isHeight; // Height in priority (else bottom indent)
	}defPos;

	struct structElm
	{
		bool isLoad;
		COLORREF color;
		ExImage image;
	}defElm;

	struct structState
	{
		int stateSelect;
		int stateLine;
	};

	Type type = Type::None;

	// State arrays
	std::vector<structState> listState;
	std::vector<structPos> listPos;
	std::vector<structElm*> listElm;


	bool LoadSkin(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile);
	Type GetType(XmlNode& xmlNode);
	void LoadState(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile, XmlNode& xmlState);
	bool LoadImage(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile, structElm& elm);
	bool LoadLine(XmlNode& xmlNode, structElm& elm);
	bool LoadFill(XmlNode& xmlNode, structElm& elm);
	bool LoadPosition(XmlNode& xmlNode, structPos& pos, structElm& elm);
	void Draw(HDC dc, CRect& rc, int stateSelect, int stateLine);
};



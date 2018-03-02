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


// SkinTreeElement

#include "XmlFile.h"
#include "UTF.h"
#include "ExImage.h"


class SkinTreeElement
{
public:
	SkinTreeElement();
	virtual ~SkinTreeElement();
	SkinTreeElement(const SkinTreeElement&) = delete;
	SkinTreeElement& operator=(const SkinTreeElement&) = delete;

	enum class Type
	{
		None   = -404,

		Icon   = -2,
		Expand = -1,

		Text   =  0,

		EnumCount // Total number (Latest + 1)
	};

	struct structPos
	{
		bool isLoad;
		int left;    // Left indent
		int right;   // Right indent
		int top;     // Top indent
		int bottom;  // Bottom indent
		int width;   // Width
		// int height;  // Height
		bool isLeft;   // Left indent in priority (else right indent)
		bool isTop;    // Top indent in priority (else bottom indent)
		bool isWidth;  // Width in priority (else right indent)
		// bool isHeight; // Height in priority (else bottom indent)
	}defPos;

	struct structElm
	{
		bool isLoad;
		HFONT font; // Font
		COLORREF color; // Text color
		int align; // Text align
		ExImage expand[2]; // Rating
		ExImage icon[10]; // Cover
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
	bool LoadPosition(XmlNode& xmlNode, structPos& pos);
	bool LoadFont(XmlNode& xmlNode, std::wstring& path, structElm& elm);
	bool LoadExpand(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile, structElm& elm);
	bool LoadIcons(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile, structElm& elm);

	void LoadState(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile, XmlNode& xmlState);

	Type GetType(XmlNode& xmlNode);

	void DrawText(HDC dc, CRect& rc, const std::wstring& text, int stateSelect, int stateLine);
	void DrawExpand(HDC dc, CRect& rc, bool isOpen, bool isShowOpen, int stateSelect, int stateLine);
	bool IsClickExpand(CPoint& point, CRect& rc, int stateSelect, int stateLine);
	void DrawIcon(HDC dc, CRect& rc, int iconType, int stateSelect, int stateLine);
};



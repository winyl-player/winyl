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
#include "UTF.h"
#include "ExImage.h"
#include "FutureWin.h"

class SkinListElement
{
public:
	SkinListElement();
	virtual ~SkinListElement();
	SkinListElement(const SkinListElement&) = delete;
	SkinListElement& operator=(const SkinListElement&) = delete;

	enum class Type
	{
		None   = -404,

		ArtistTitle = -101,
		ArtistAlbum = -100,

		Cover  = -2,
		Rating = -1,

		Artist    = 0,
		Album     = 1,
		Title     = 2,
		Year      = 3,
		Track     = 4,
		Time      = 5,
		Genre     = 6,
		Index     = 7,
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
		COLORREF color; // title(color)
		COLORREF color2; // artist(color) - title(color2)
		int align; // Text align
		ExImage rating[2]; // Rating
		ExImage cover[2]; // Cover
	}defElm;

	struct structState
	{
		int stateSelect;
		int statePlay;
		int stateLine;
		int stateLibrary;
	};

	Type type = Type::None;

	std::wstring dash = L" - ";

	// State arrays
	std::vector<structState> listState;
	std::vector<structPos> listPos;
	std::vector<structElm*> listElm;

	bool isStatePlaylist = false;
	bool isStateLibrary = false;


	bool LoadSkin(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile);
	bool LoadPosition(XmlNode& xmlNode, structPos& pos);
	bool LoadFont(XmlNode& xmlNode, std::wstring& path, structElm& elm);
	bool LoadRating(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile, structElm& elm);
	bool LoadCover(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile, structElm& elm);

	void LoadState(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile, XmlNode& xmlState);

	Type GetType(XmlNode& xmlNode);

	void DrawText(HDC dc, CRect& rc, const std::wstring& text, int stateSelect, int statePlay, int stateLine, int stateLibrary);
	void DrawText2(HDC dc, CRect& rc, const std::wstring& text, const std::wstring& text2, int stateSelect, int statePlay, int stateLine, int stateLibrary);
	void DrawRating(HDC dc, CRect& rc, int rating, int stateSelect, int statePlay, int stateLine, int stateLibrary);
	void DrawCover(HDC dc, CRect& rc, ExImage* image, int stateSelect, int statePlay, int stateLine, int stateLibrary, CSize& szCover);
	int GetClickRating(CPoint& point, CRect& rc, int stateSelect, int statePlay, int stateLine, int stateLibrary);
};



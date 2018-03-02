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


// SkinLayout

#include "XmlFile.h"
#include "SkinLayoutBack.h"
#include "SkinLayoutElement.h"


class SkinLayout
{

public:
	SkinLayout();
	virtual ~SkinLayout();

	CRect rcRect = {0, 0, 0, 0};
	CRect rcRectOld = {0, 0, 0, 0};

	int id = 0;
	bool isHidden = false;
	bool isTest = false;

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

		int defWidth;
		int defHeight;
		int defLeft;
		int defRight;
		int defTop;
		int defBottom;
		int defLeftRight;
		int defTopBottom;
	}pos;

	std::vector<std::unique_ptr<SkinLayoutElement>> elements; // Elements array
	std::vector<std::unique_ptr<SkinLayoutBack>> backs; // Background array
	std::vector<std::unique_ptr<SkinLayoutBack>> forwards; // Foreground array (not used)

	inline bool IsHidden() {return isHidden;}

public:
	bool LoadSkin(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile);
	bool LoadPosition(XmlNode& xmlNode);
	bool LoadElements(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile);
	bool LoadBackground(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile);
	bool LoadForeground(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile);

	void UpdateRectLayout(int left, int top, int right, int bottom);
	void UpdateRectElements();
	void DrawElements(HDC dc, bool isAlpha);
	void DrawBackground(HDC dc);	
	bool TriggerLayout(int cx, int cy, const SkinTrigger::structTrigger& trigger, const std::vector<std::unique_ptr<SkinLayout>>& layouts);
	SkinLayout* TriggerFindLayout(int id, const std::vector<std::unique_ptr<SkinLayout>>& layouts);
	bool SplitterLayout(int x, int y, int cx, int cy, const SkinSplitter::structSplitter& splitter, CRect& rcMax);
	void SplitterLayoutClick();
	void HideLayout(bool isHide);
};



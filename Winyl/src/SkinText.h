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
#include "Language.h"

class SkinText : public SkinElement
{

public:
	SkinText();
	virtual ~SkinText();

private:
	HFONT font = NULL; // Text font
	int align = 0; // Text align
	COLORREF colorText = 0x00000000; // Text color
	COLORREF colorText2 = 0x00000000; // Text 2 color (Artist(color) - Title(color2))
	BYTE colorTextAlpha = 0xFF; // Text opacity
	BYTE colorTextAlpha2 = 0xFF; // Text 2 opacity

	// Effects
	struct structEffect
	{
		int glowSize = 0; // Glow effect (only if Vista or newer)
		bool isShadow = false; // Shadow effect is enabled?
		COLORREF colorShadow = 0x00000000; // Shadow color
		BYTE colorShadowAlpha = 0xFF; // Shadow opacity
		int shadowX = 0; // Shadow offset X
		int shadowY = 0; // Shadow offset Y
		bool isFixOpacity = false; // Fix opacity is enabled?
	}effect;

	std::wstring thisText; // Text line
	std::wstring thisText2; // Text line 2
	std::wstring dash; // " - "

	bool isDoubleText = false;
	bool isUpperCase = false;

public:
	int GetWidth() override;
	int GetHeight() override;

	bool LoadSkin(const std::wstring& file, ZipFile* zipFile) override;
	void Draw(HDC dc, bool isAlpha) override;

public:
	static std::wstring TimeString(int time, bool isRemains, int length, bool isLength);
	void SetTime(int time, bool isRemains, int length = 0, bool isLength = false);
	void SetStatusLine(int count, int total, int time, long long size, bool isShort, Language* lang);
	void SetText(const std::wstring& text);
	void SetText2(const std::wstring& text);
	void SetTextEmpty();
	int GetTextWidth(HDC dc);
	void SetDoubleText(bool isDouble) {isDoubleText = isDouble;}

private:
	void DrawTextSimple(HDC dc, bool isAlpha, const std::wstring& text, CRect& rc, COLORREF clr, BYTE alpha, UINT format);
	void DrawTextAlphaXP(HDC dc, const std::wstring& text, CRect& rc, COLORREF clr, BYTE alpha, UINT format);
	void DrawTextAlphaFixXP(HDC dc, const std::wstring& text, CRect& rc, COLORREF clr, BYTE alpha, UINT format);
	void DrawTextAlpha7(HDC dc, const std::wstring& text, CRect& rc, COLORREF clr, BYTE alpha, UINT format);
	void DrawDoubleText(HDC dc, bool isAlpha);
};



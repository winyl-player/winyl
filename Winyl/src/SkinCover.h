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

// SkinCover

#include "SkinElement.h"

class SkinCover : public SkinElement
{

public:
	SkinCover();
	virtual ~SkinCover();

private:
	ExImage imCover;   // Default cover art
	ExImage imShadow;  // Shadow
	ExImage imOverlay; // Overlay
	ExImage imMirror;  // Overlay for mirror
	ExImage imCoverMirror; // Mirrored default cover art

	ExImage imCoverImage; // Cover art
	ExImage imMirrorImage; // Mirrored cover art
	ExImage imBackupImage; // Previous cover art
	ExImage imBackupMirror; // Mirrored previous cover art

	bool isMirror = false;
	bool isFadeEnabled = false;

public:
	int GetWidth() override;
	int GetHeight() override;

	bool LoadSkin(const std::wstring& file, ZipFile* zipFile) override;
	void Draw(HDC dc, bool isAlpha) override;
	bool DrawFade(HDC dc, bool isAlpha) override;

public:
	void SetImage(ExImage::Source* image);
	bool IsImage();
	void EnableFade(bool isEnable);
};



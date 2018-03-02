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

// SkinVis

#include "WindowEx.h"
#include <math.h>
#include "XmlFile.h"
#include "ExImage.h"
#include "UTF.h"

class SkinVis : public WindowEx
{

public:
	SkinVis();
	virtual ~SkinVis();
	SkinVis(const SkinVis&) = delete;
	SkinVis& operator=(const SkinVis&) = delete;

	bool NewWindow(HWND parent);
	bool LoadSkin(const std::wstring& file, ZipFile* zipFile);
	bool SetFFT(float* fft, bool isPause);
	
private:
	void PrepareSkin();

	int backColor = 0x00FFFFFF;
	int bandSize = 0;
	int peakStep = 0;
	bool hasPeaks = false;

	ExImage imMain;
	ExImage imBack;
	ExImage imPeak;

	std::vector<CRect> rects;

	struct structBand
	{
		int band;
		int peak;
		int wait;
	};

	std::vector<structBand> bands;

private: // Messages
	LRESULT WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
	void OnPaint(HDC dc, PAINTSTRUCT& ps);
	void OnSize(UINT nType, int cx, int cy);
	void OnShowWindow(BOOL bShow, UINT nStatus);
};

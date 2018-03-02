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

// SkinVis.cpp : implementation file
//

#include "stdafx.h"
#include "SkinVis.h"


// SkinVis

SkinVis::SkinVis()
{

}

SkinVis::~SkinVis()
{

}

LRESULT SkinVis::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(thisWnd, &ps);
		OnPaint(hdc, ps);
		EndPaint(thisWnd, &ps);
		return 0;
	case WM_ERASEBKGND:
		return 1;
	case WM_SIZE:
		OnSize((UINT)wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_SHOWWINDOW:
		OnShowWindow((BOOL)wParam, (UINT)lParam);
		return 0;
	}

	return ::DefWindowProc(hWnd, message, wParam, lParam);
}

bool SkinVis::NewWindow(HWND parent)
{
	if (CreateClassWindow(parent, L"SkinVis", WS_CHILDWINDOW, 0))
		return true;

	return false;
}

void SkinVis::OnPaint(HDC dc, PAINTSTRUCT& ps)
{
	CRect rc;
	::GetClientRect(thisWnd, rc);

	HDC dcMemory = ::CreateCompatibleDC(dc);

	HBITMAP bmMemory = ExImage::Create32BppBitmap(rc.Width(), rc.Height());

	HGDIOBJ oldMemory = ::SelectObject(dcMemory, bmMemory);

	::SetBkColor(dcMemory, backColor);
	::ExtTextOut(dcMemory, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);


	//imBack.Crop(dcMemory, rc);
	imBack.Draw(dcMemory, rc);

	// Draw bands
	if (!rects.empty())
	{
		for (std::size_t i = 0, size = bands.size(); i < size; ++i)
			imMain.Crop2(dcMemory, (int)i * bandSize, rc.Height() - imMain.Height(), rects[bands[i].band]);

		if (hasPeaks) // Draw peaks
		{
			for (std::size_t i = 0, size = bands.size(); i < size; ++i)
				imPeak.Draw(dcMemory, (int)i * bandSize, rc.Height() - (bands[i].peak * peakStep) - imPeak.Height());
		}
	}


	// Copy memDC //
	::BitBlt(dc, rc.left, rc.top, rc.Width(), rc.Height(), dcMemory, rc.left, rc.top, SRCCOPY);

	// Release resources
	::SelectObject(dcMemory, oldMemory);

	::DeleteObject(bmMemory);
	::DeleteDC(dcMemory);
}

void SkinVis::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (!bShow)
	{
		for (std::size_t i = 0, size = bands.size(); i < size; ++i)
		{
			bands[i].band = 0;
			bands[i].peak = 0;
			bands[i].wait = 0;
		}
	}
}

void SkinVis::OnSize(UINT nType, int cx, int cy)
{
	if (bandSize > 0)
		bands.resize(cx / bandSize);
}


// Return true if the animation is completed.
// If fft == nullptr then complete the animation,
// If previous and isPause == true then complete the animation of peaks only.
bool SkinVis::SetFFT(float* fft, bool isPause)
{
	if (!::IsWindowVisible(thisWnd))
		return (fft == nullptr);

	int sizeBands = (int)bands.size();
	int sizeRects = (int)rects.size();
	
	if (!(sizeBands > 0 && sizeRects > 0))
		return (fft == nullptr);

	bool isStop = true;

	if (!isPause)
	{
		for (int i = 0; i < sizeBands; ++i)
		{
			if (bands[i].band > 0)
			{
				--bands[i].band;
				isStop = false;
			}
		}
	}

	if (hasPeaks)
	{
		for (int i = 0; i < sizeBands; ++i)
		{
			if (bands[i].wait > 0)
				--bands[i].wait;

			if (bands[i].peak > bands[i].band)
			{
				isStop = false;
				if (bands[i].wait == 0)
					--bands[i].peak;
			}
		}
	}

	if (fft == nullptr)
	{
		::InvalidateRect(thisWnd, NULL, TRUE);
		return isStop;
	}

	int curFFT = 50; // Start position is 50 the first values isn't good
	int stepFFT = 1024 / sizeBands / 5; // Step, reduced by 5 times to collect only first values in general
	int maxFFT = 1000; // End position is not 1024 because the last values are almost invisible

	// Step cannot be less than 1
	stepFFT = std::max(1, stepFFT);

	for (int i = 0; i < sizeBands; ++i)
	{
		if (curFFT + stepFFT > maxFFT)
			break;

		int num = 0;
		int sum = 0;

		// Calculate the arithmetic mean, so Bands will be more smooth to each other
		for (int j = curFFT; j < curFFT + stepFFT; j += 1)
		{
			++num;
			sum += (int)(sqrt(fft[j]) * 5 * sizeRects); // (sqrt to make low values more visible)
		}

		num = sum / num;

		// Move FFT
		curFFT += stepFFT;

		// Adjust
		if (num > sizeRects - 1)
			num = sizeRects - 1;

		// Set Band
		if (num > bands[i].band)
			bands[i].band = num;

		// Set Peak
		if (hasPeaks)
		{
			// If Band > Peak, then set Peak and its Wait interval
			if (num > bands[i].peak)
			{
				bands[i].peak = num;
				bands[i].wait = 20;
			}
		}
	}

	::InvalidateRect(thisWnd, NULL, TRUE);
	//::UpdateWindow(thisWnd);

	return false;
}

bool SkinVis::LoadSkin(const std::wstring& file, ZipFile* zipFile)
{
	std::wstring path = PathEx::PathFromFile(file);

	XmlFile xmlFile;

	if (xmlFile.LoadEx(file, zipFile))
	{		
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Spectrum");
		
		if (xmlMain)
		{
			XmlNode xmlSkin = xmlMain.FirstChild("Skin");

			if (xmlSkin)
			{
				std::wstring attr = xmlSkin.Attribute16("File");
				if (!attr.empty())
					imMain.LoadEx(path + attr, zipFile);

				xmlSkin.Attribute("BandSize", &bandSize);
			}

			XmlNode xmlBack = xmlMain.FirstChild("Background");

			if (xmlBack)
			{
				std::wstring attr = xmlBack.Attribute16("File");
				if (!attr.empty())
					imBack.LoadEx(path + attr, zipFile);

				backColor = RGB(255, 255, 255); // Default

				const char* strColor = xmlBack.AttributeRaw("Color");
				if (strColor)
				{
					DWORD c = strtoul(strColor, 0, 16);

					if (strlen(strColor) == 6)
						backColor = RGB(GetBValue(c), GetGValue(c), GetRValue(c));
				}
			}

			XmlNode xmlPeak = xmlMain.FirstChild("Peak");

			if (xmlPeak)
			{
				hasPeaks = true;

				std::wstring attr = xmlPeak.Attribute16("File");
				if (!attr.empty())
					imPeak.LoadEx(path + attr, zipFile);

				xmlPeak.Attribute("PeakStep", &peakStep);
			}
		}

		PrepareSkin();

		return true;
	}

	return false;
}

void SkinVis::PrepareSkin()
{
	int width = imMain.Width();
	int height = imMain.Height();

	if (bandSize > 0)
	{
		for (int i = 0; i + bandSize <= width; i += bandSize)
			rects.push_back(CRect(i, 0, i + bandSize, height));
	}
}

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

#include "stdafx.h"
#include "SkinShadow.h"
#include "XmlFile.h"

SkinShadow::SkinShadow()
{

}

SkinShadow::~SkinShadow()
{

}

void SkinShadow::NewShadow(HWND parent)
{
	NewWindow(windowTop, parent);
	NewWindow(windowBottom, parent);
	NewWindow(windowLeft, parent);
	NewWindow(windowRight, parent);
}

bool SkinShadow::NewWindow(WindowEx& window, HWND parent)
{
	if (window.CreateClassWindow(parent, L"SkinShadow", WS_POPUP, WS_EX_LAYERED|WS_EX_TOOLWINDOW|WS_EX_TRANSPARENT))
		return true;

	return false;
}

bool SkinShadow::LoadSkin(const std::wstring& programPath, const std::wstring& skinName, ZipFile* zipFile)
{
	std::wstring path;
	if (zipFile == nullptr)
	{
		path += programPath;
		path += L"Skin";
		path.push_back('\\');
		path += skinName;
		path.push_back('\\');
	}

	std::wstring file = path + L"Shadow.xml";

	XmlFile xmlFile;

	if (xmlFile.LoadEx(file, zipFile))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Shadow");

		if (xmlMain)
		{
			XmlNode xmlTop = xmlMain.FirstChild("Top");
			if (xmlTop)
			{
				std::wstring attr = xmlTop.Attribute16("File");
				if (!attr.empty())
					shadowTop.LoadEx(path + attr, zipFile);
			}

			XmlNode xmlTopLeft = xmlMain.FirstChild("TopLeft");
			if (xmlTopLeft)
			{
				std::wstring attr = xmlTopLeft.Attribute16("File");
				if (!attr.empty())
					shadowTopLeft.LoadEx(path + attr, zipFile);
			}

			XmlNode xmlTopRight = xmlMain.FirstChild("TopRight");
			if (xmlTopRight)
			{
				std::wstring attr = xmlTopRight.Attribute16("File");
				if (!attr.empty())
					shadowTopRight.LoadEx(path + attr, zipFile);
			}

			XmlNode xmlBottom = xmlMain.FirstChild("Bottom");
			if (xmlBottom)
			{
				std::wstring attr = xmlBottom.Attribute16("File");
				if (!attr.empty())
					shadowBottom.LoadEx(path + attr, zipFile);
			}

			XmlNode xmlBottomLeft = xmlMain.FirstChild("BottomLeft");
			if (xmlBottomLeft)
			{
				std::wstring attr = xmlBottomLeft.Attribute16("File");
				if (!attr.empty())
					shadowBottomLeft.LoadEx(path + attr, zipFile);
			}

			XmlNode xmlBottomRight = xmlMain.FirstChild("BottomRight");
			if (xmlBottomRight)
			{
				std::wstring attr = xmlBottomRight.Attribute16("File");
				if (!attr.empty())
					shadowBottomRight.LoadEx(path + attr, zipFile);
			}

			XmlNode xmlLeft = xmlMain.FirstChild("Left");
			if (xmlLeft)
			{
				std::wstring attr = xmlLeft.Attribute16("File");
				if (!attr.empty())
					shadowLeft.LoadEx(path + attr, zipFile);
			}

			XmlNode xmlLeftTop = xmlMain.FirstChild("LeftTop");
			if (xmlLeftTop)
			{
				std::wstring attr = xmlLeftTop.Attribute16("File");
				if (!attr.empty())
					shadowLeftTop.LoadEx(path + attr, zipFile);
			}

			XmlNode xmlLeftBottom = xmlMain.FirstChild("LeftBottom");
			if (xmlLeftBottom)
			{
				std::wstring attr = xmlLeftBottom.Attribute16("File");
				if (!attr.empty())
					shadowLeftBottom.LoadEx(path + attr, zipFile);
			}

			XmlNode xmlRight = xmlMain.FirstChild("Right");
			if (xmlRight)
			{
				std::wstring attr = xmlRight.Attribute16("File");
				if (!attr.empty())
					shadowRight.LoadEx(path + attr, zipFile);
			}

			XmlNode xmlRightTop = xmlMain.FirstChild("RightTop");
			if (xmlRightTop)
			{
				std::wstring attr = xmlRightTop.Attribute16("File");
				if (!attr.empty())
					shadowRightTop.LoadEx(path + attr, zipFile);
			}

			XmlNode xmlRightBottom = xmlMain.FirstChild("RightBottom");
			if (xmlRightBottom)
			{
				std::wstring attr = xmlRightBottom.Attribute16("File");
				if (!attr.empty())
					shadowRightBottom.LoadEx(path + attr, zipFile);
			}
		}
	}
	else
		return false;

	return true;
}

void SkinShadow::Move(int x, int y, int cx, int cy)
{
	DrawShadow(windowTop.Wnd(), 0, x - shadowLeft.Width(), y - shadowTop.Height(), cx + shadowLeft.Width() + shadowRight.Width(), shadowTop.Height());
	DrawShadow(windowLeft.Wnd(), 2, x - shadowLeft.Width(), y, shadowLeft.Width(), cy);
	DrawShadow(windowRight.Wnd(), 3, x + cx, y, shadowRight.Width(), cy);
	DrawShadow(windowBottom.Wnd(), 1, x - shadowLeft.Width(), y + cy, cx + shadowLeft.Width() + shadowRight.Width(), shadowBottom.Height());
}

void SkinShadow::Show(bool show)
{
	if (isWindowVisible == show)
		return;

	isWindowVisible = show;

	int flag = SWP_SHOWWINDOW;
	if (!show)
		flag = SWP_HIDEWINDOW;

	HDWP dwp = ::BeginDeferWindowPos(4);
	::DeferWindowPos(dwp, windowTop.Wnd(), NULL, 0, 0, 0, 0, flag|SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE);
	::DeferWindowPos(dwp, windowLeft.Wnd(), NULL, 0, 0, 0, 0, flag|SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE);
	::DeferWindowPos(dwp, windowRight.Wnd(), NULL, 0, 0, 0, 0, flag|SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE);
	::DeferWindowPos(dwp, windowBottom.Wnd(), NULL, 0, 0, 0, 0, flag|SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE);
	::EndDeferWindowPos(dwp);
}

void SkinShadow::DrawShadow(HWND wnd, int type, int x, int y, int cx, int cy)
{
	HDC dcMemory = ::CreateCompatibleDC(NULL);
	HBITMAP bmMemory = ExImage::Create32BppBitmap(cx, cy);
	HGDIOBJ oldMemory = ::SelectObject(dcMemory, bmMemory);

	if (type == 0) // Top
	{
		shadowTopLeft.Draw(dcMemory, 0, 0);
		shadowTop.Draw(dcMemory, shadowTopLeft.Width(), 0, cx - shadowTopLeft.Width() - shadowTopRight.Width(), cy);
		shadowTopRight.Draw(dcMemory, cx - shadowTopRight.Width(), 0);
	}
	else if (type == 1) // Bottom
	{
		shadowBottomLeft.Draw(dcMemory, 0, 0);
		shadowBottom.Draw(dcMemory, shadowBottomLeft.Width(), 0, cx - shadowBottomLeft.Width() - shadowBottomRight.Width(), cy);
		shadowBottomRight.Draw(dcMemory, cx - shadowBottomRight.Width(), 0);
	}
	else if (type == 2) // Left
	{
		shadowLeftTop.Draw(dcMemory, 0, 0);
		shadowLeft.Draw(dcMemory, 0, shadowLeftTop.Height(), cx, cy - shadowLeftTop.Height() - shadowLeftBottom.Height());
		shadowLeftBottom.Draw(dcMemory, 0, cy - shadowLeftBottom.Height());
	}
	else if (type == 3) // Right
	{
		shadowRightTop.Draw(dcMemory, 0, 0);
		shadowRight.Draw(dcMemory, 0, shadowRightTop.Height(), cx, cy - shadowRightTop.Height() - shadowRightBottom.Height());
		shadowRightBottom.Draw(dcMemory, 0, cy - shadowRightBottom.Height());
	}

	BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

	POINT ptSrc = {0, 0};
	POINT ptDst = {x, y};
	SIZE sz = {cx, cy};

	if (!futureWin->IsVistaOrLater())
	{
		::UpdateLayeredWindow(wnd, NULL, &ptDst, &sz, dcMemory, &ptSrc, 0, &bf, ULW_ALPHA);
	}
	else
	{
		UPDATELAYEREDWINDOWINFO info = {sizeof(info), NULL, &ptDst, &sz, dcMemory, &ptSrc, 0, &bf, ULW_ALPHA, NULL};
		futureWin->UpdateLayeredWindowIndirect(wnd, &info);
	}

	::SelectObject(dcMemory, oldMemory);
	::DeleteObject(bmMemory);
	::DeleteDC(dcMemory);
}

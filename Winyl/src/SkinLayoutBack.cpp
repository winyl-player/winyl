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

// SkinLayoutBack.cpp : implementation file
//

#include "stdafx.h"
#include "SkinLayoutBack.h"

//#define RGB(r, g, b)(((DWORD)((BYTE)(r))) | ((DWORD)((BYTE)(g)) << 8) \
//                | ((DWORD)((BYTE)(b)) << 16))
//#define GetRValue(rgba)((BYTE)(rgba))
//#define GetGValue(rgba)((BYTE)(((DWORD)(rgba)) >> 8))
//#define GetBValue(rgba)((BYTE)((DWORD)(rgba) >> 16))
#define RGBA(r, g, b, a)(((DWORD)((BYTE)(r))) | ((DWORD)((BYTE)(g)) << 8) \
                | ((DWORD)((BYTE)(b)) << 16) | ((DWORD)((BYTE)(a)) << 24))
#define GetAValue(rgba)((BYTE)((DWORD)(rgba) >> 24))

// SkinLayoutBack

SkinLayoutBack::SkinLayoutBack()
{

}

SkinLayoutBack::~SkinLayoutBack()
{

}

bool SkinLayoutBack::LoadSkin(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile)
{
	// Load type
	type = GetType(xmlNode);

	if (type == Type::None)
		return false;

	// Load element
	if (type == Type::Image)
		LoadImage(xmlNode, path, zipFile);
	else if (type == Type::Line)
		LoadLine(xmlNode);
	else if (type == Type::Fill)
		LoadFill(xmlNode);

	LoadPosition(xmlNode);

	return true;
}

SkinLayoutBack::Type SkinLayoutBack::GetType(XmlNode& xmlNode)
{
	if (strcmp(xmlNode.Name(), "Image") == 0)
		return Type::Image;
	else if (strcmp(xmlNode.Name(), "Rect") == 0)
		return Type::Line;
	else if (strcmp(xmlNode.Name(), "Fill") == 0)
		return Type::Fill;
	else
		return Type::None;
}

bool SkinLayoutBack::LoadPosition(XmlNode& xmlNode)
{
	pos = {};

	int width = DefaultFill::Width;
	int height = DefaultFill::Height;
	if (type == Type::Image)
	{
		width  = elm.image.Width();
		height = elm.image.Height();
	}

	XmlNode xmlPosition = xmlNode.FirstChild("Position");

	if (xmlPosition)
	{
		if (xmlPosition.Attribute("Left", &pos.left))
		{
			pos.isLeft = true;

			if (xmlPosition.Attribute("Width", &pos.width))
				pos.isWidth = true;
			else if (xmlPosition.Attribute("Right", &pos.right))
				pos.isRight = true;
			else if (xmlPosition.Attribute("LeftRight", &pos.leftRight))
				pos.isLeftRight = true;
			else // Default
			{
				pos.isWidth = true; // Default
				pos.width = width; // Default
			}
		}
		else if (xmlPosition.Attribute("Right", &pos.right))
		{
			pos.isRight = true;

			if (xmlPosition.Attribute("Width", &pos.width))
				pos.isWidth = true;
			else if (xmlPosition.Attribute("LeftRight", &pos.leftRight))
				pos.isLeftRight = true;
			else // Default
			{
				pos.isWidth = true; // Default
				pos.width = width; // Default
			}
		}
		else if (xmlPosition.Attribute("LeftRight", &pos.leftRight))
		{
			pos.isLeftRight = true;

			pos.isWidth = true; // Default
			pos.width = width; // Default

			xmlPosition.Attribute("Width", &pos.width);
		}
		else // Default
		{
			pos.isLeft = true; // Default

			pos.isWidth = true; // Default
			pos.width = width; // Default

			xmlPosition.Attribute("Width", &pos.width);
		}

		if (xmlPosition.Attribute("Top", &pos.top))
		{
			pos.isTop = true;

			if (xmlPosition.Attribute("Height", &pos.height))
				pos.isHeight = true;
			else if (xmlPosition.Attribute("Bottom", &pos.bottom))
				pos.isBottom = true;
			else if (xmlPosition.Attribute("TopBottom", &pos.topBottom))
				pos.isTopBottom = true;
			else // Default
			{
				pos.isHeight = true; // Default
				pos.height = height; // Default
			}
		}
		else if (xmlPosition.Attribute("Bottom", &pos.bottom))
		{
			pos.isBottom = true;

			if (xmlPosition.Attribute("Height", &pos.height))
				pos.isHeight = true;
			else if (xmlPosition.Attribute("TopBottom", &pos.topBottom))
				pos.isTopBottom = true;
			else // Default
			{
				pos.isHeight = true; // Default
				pos.height = height; // Default
			}
		}
		else if (xmlPosition.Attribute("TopBottom", &pos.topBottom))
		{
			pos.isTopBottom = true;

			pos.isHeight = true; // Default
			pos.height = height; // Default

			xmlPosition.Attribute("Height", &pos.height);
		}
		else // Default
		{
			pos.isTop = true; // Default

			pos.isHeight = true; // Default
			pos.height = height; // Default

			xmlPosition.Attribute("Height", &pos.height);
		}

		return true;
	}

	return false;
}

bool SkinLayoutBack::LoadImage(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile)
{
	XmlNode xmlImage = xmlNode.FirstChild("Image");

	if (xmlImage)
	{
		std::wstring file = xmlImage.Attribute16("File");
		if (!file.empty())
			elm.image.LoadEx(path + file, zipFile);

		elm.tile = false;
		xmlImage.Attribute("Tile", &elm.tile);

		return true;
	}

	return false;
}

bool SkinLayoutBack::LoadLine(XmlNode& xmlNode)
{
	XmlNode xmlLine = xmlNode.FirstChild("Rect");

	if (xmlLine)
	{
		const char* strColor = xmlLine.AttributeRaw("Color");

		elm.color = 0xFFFFFFFF; // Default (white opaque)

		// LOBYTE(LOWORD(c)) - Alpha
		// HIBYTE(HIWORD(c)) - Red
		// LOBYTE(HIWORD(c)) - Green
		// HIBYTE(LOWORD(c)) - Blue

		if (strColor)
		{
			DWORD c = strtoul(strColor, 0, 16);

			if (strlen(strColor) == 6) // Color in RGB format (convert to ABGR)
				elm.color = MAKELONG(MAKEWORD(LOBYTE(HIWORD(c)), HIBYTE(LOWORD(c))), MAKEWORD(LOBYTE(LOWORD(c)), 0xFF));
			else if (strlen(strColor) == 8) // Color in RGBA format (convert to ABGR)
				elm.color = MAKELONG(MAKEWORD(HIBYTE(HIWORD(c)), LOBYTE(HIWORD(c))), MAKEWORD(HIBYTE(LOWORD(c)), LOBYTE(LOWORD(c))));
		}

		return true;
	}

	return false;
}

bool SkinLayoutBack::LoadFill(XmlNode& xmlNode)
{
	XmlNode xmlRect = xmlNode.FirstChild("Fill");

	if (xmlRect)
	{
		const char* strColor = xmlRect.AttributeRaw("Color");

		elm.color = 0xFFFFFFFF; // Default (white opaque)

		if (strColor)
		{
			DWORD c = strtoul(strColor, 0, 16);

			if (strlen(strColor) == 6) // Color in RGB format (convert to ABGR)
				elm.color = RGBA(GetBValue(c), GetGValue(c), GetRValue(c), 0xFF);
			else if (strlen(strColor) == 8) // Color in RGBA format (convert to ABGR)
				elm.color = RGBA(GetAValue(c), GetBValue(c), GetGValue(c), GetRValue(c));
		}

		return true;
	}

	return false;
}

void SkinLayoutBack::Draw(HDC dc, CRect& rcRect)
{
	if (type == Type::None)
		return;

	CRect rcDraw;

	if (pos.isLeft) // Left location
	{
		rcDraw.left = rcRect.left + pos.left;

		if (pos.isWidth) // Fixed width
			rcDraw.right = rcDraw.left + pos.width;
		else if (pos.isRight) // To right
			rcDraw.right = rcRect.right - pos.right;
		else // To center
			rcDraw.right = rcRect.left + rcRect.Width()/2 + pos.leftRight;
	}
	else if (pos.isRight) // Right location
	{
		if (pos.isWidth) // Fixed width
		{
			rcDraw.left = rcRect.right - pos.width - pos.right;
			rcDraw.right = rcDraw.left + pos.width;
		}
		else // From center
		{
			rcDraw.left = rcRect.left + rcRect.Width()/2 + pos.leftRight;
			rcDraw.right = rcRect.right - pos.right;
		}
	}
	else // Center location
	{
		rcDraw.left = rcRect.left + (rcRect.Width()/2 - pos.width/2) + pos.leftRight;
		rcDraw.right = rcDraw.left + pos.width;
	}

	if (pos.isTop) // Top location
	{
		rcDraw.top = rcRect.top + pos.top;

		if (pos.isHeight) // Fixed height
			rcDraw.bottom = rcDraw.top + pos.height;
		else if (pos.isBottom) // To bottom
			rcDraw.bottom = rcRect.bottom - pos.bottom;
		else // To center
			rcDraw.bottom = rcRect.top + rcRect.Height()/2 + pos.topBottom;
	}
	else if (pos.isBottom) // Bottom location
	{
		if (pos.isHeight) // Fixed height
		{
			rcDraw.top = rcRect.bottom - pos.height - pos.bottom;
			rcDraw.bottom = rcDraw.top + pos.height;
		}
		else // From center
		{
			rcDraw.top = rcRect.top + rcRect.Height()/2 + pos.topBottom;
			rcDraw.bottom = rcRect.bottom - pos.bottom;
		}
	}
	else // Center location
	{
		rcDraw.top = rcRect.top + (rcRect.Height()/2 - pos.height/2) + pos.topBottom;
		rcDraw.bottom = rcDraw.top + pos.height;
	}

	// Draw
	if (type == Type::Image)
	{
		if (!elm.tile)
			elm.image.Draw(dc, rcDraw);
		else
			elm.image.Tile(dc, rcDraw);
	}
	else if (type == Type::Line)
	{
		//Pen pPen(sElm.iColor, 1);
		//g->DrawRectangle(&pPen, rDraw.left, rDraw.top, rDraw.Width()-1, rDraw.Height()-1);
		
		ExImage::DrawAlphaRect(dc, rcDraw, elm.color);
	}
	else if (type == Type::Fill)
	{
		// Use HatchBrush instead of SolidBrush because there is a bug in GDI+ when using layered window with opaque fill
		//HatchBrush bBrush2(HatchStyleCross, sElm.iColor, sElm.iColor);
		//g->FillRectangle(&bBrush2, rDraw.left, rDraw.top, rDraw.Width(), rDraw.Height());

		ExImage::DrawAlphaFill(dc, rcDraw, elm.color);
	}
}


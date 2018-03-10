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
#include "SkinPopupElement.h"

SkinPopupElement::SkinPopupElement()
{

}

SkinPopupElement::~SkinPopupElement()
{

}

void SkinPopupElement::LoadSkin(XmlNode& xmlNode, const std::wstring& path, ZipFile* zipFile)
{
	XmlNode xmlSkin = xmlNode.FirstChild("Skin");

	if (xmlSkin)
	{
		std::wstring attr = xmlSkin.Attribute16("File");
		if (!attr.empty())
		{
			LoadType(xmlNode);

			element->LoadSkin(path + attr, zipFile);

			width = element->GetWidth();
			height = element->GetHeight();
		}
	}

	if (!element)
	{
		element.reset(new SkinElement());
		element->type = SkinElement::Type::None;
	}

	LoadPosition(xmlNode);
}

void SkinPopupElement::LoadType(XmlNode& xmlNode)
{
	if (strcmp(xmlNode.Name(), "Title") == 0)
	{
		element.reset(new SkinText());
		element->type = SkinElement::Type::Title;
	}
	else if (strcmp(xmlNode.Name(), "Artist") == 0)
	{
		element.reset(new SkinText());
		element->type = SkinElement::Type::Artist;
	}
	else if (strcmp(xmlNode.Name(), "Album") == 0)
	{
		element.reset(new SkinText());
		element->type = SkinElement::Type::Album;
	}
	else if (strcmp(xmlNode.Name(), "Year") == 0)
	{
		element.reset(new SkinText());
		element->type = SkinElement::Type::Year;
	}
	else if (strcmp(xmlNode.Name(), "Genre") == 0)
	{
		element.reset(new SkinText());
		element->type = SkinElement::Type::Genre;
	}
	else if (strcmp(xmlNode.Name(), "Cover") == 0)
	{
		element.reset(new SkinCover());
		element->type = SkinElement::Type::Cover;
	}
	else if (strcmp(xmlNode.Name(), "Rating") == 0)
	{
		element.reset(new SkinRating());
		element->type = SkinElement::Type::Rating;
	}
	else if (strcmp(xmlNode.Name(), "TimeLength") == 0)
	{
		element.reset(new SkinText());
		element->type = SkinElement::Type::TimeLength;
	}
	else
	{
		element.reset(new SkinElement());
		element->type = SkinElement::Type::None;
	}
}

bool SkinPopupElement::LoadPosition(XmlNode& xmlNode)
{
	pos = {};

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

void SkinPopupElement::UpdateRect(CRect& rcRect)
{
	if (!element)
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

	element->SetRect(rcDraw);
}

void SkinPopupElement::Draw(HDC dc, bool isAlpha)
{
	if (!element)
		return;

	if (!element->IsHidden())
		element->Draw(dc, isAlpha);
}

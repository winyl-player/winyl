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

// SkinButton.cpp : implementation file
//

#include "stdafx.h"
#include "SkinButton.h"


// SkinButton

SkinButton::SkinButton()
{

}

SkinButton::~SkinButton()
{

}

bool SkinButton::LoadSkin(const std::wstring& file, ZipFile* zipFile)
{
	std::wstring path = PathEx::PathFromFile(file);

	XmlFile xmlFile;

	if (xmlFile.LoadEx(file, zipFile))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Button");

		if (xmlMain)
		{
			XmlNode xmlNormal = xmlMain.FirstChild("Normal");
			if (xmlNormal)
			{
				std::wstring attr = xmlNormal.Attribute16("File");
				if (!attr.empty())
					imButton[0].LoadEx(path + attr, zipFile);
			}

			XmlNode xmlHover = xmlMain.FirstChild("Hover");
			if (xmlHover)
			{
				std::wstring attr = xmlHover.Attribute16("File");
				if (!attr.empty())
					imButton[1].LoadEx(path + attr, zipFile);
			}

			XmlNode xmlPress = xmlMain.FirstChild("Press");
			if (xmlPress)
			{
				std::wstring attr = xmlPress.Attribute16("File");
				if (!attr.empty())
					imButton[2].LoadEx(path + attr, zipFile);
			}

			XmlNode xmlEffect = xmlMain.FirstChild("Effect");
			if (xmlEffect)
				xmlEffect.Attribute("Fade", &isFadeEffect);

			XmlNode xmlTrigger = xmlMain.FirstChild("Trigger");
			if (xmlTrigger)
				skinTrigger.LoadTriggerElement(xmlTrigger);
		}
	}
	else
		return false;

	return true;
}

void SkinButton::Draw(HDC dc, bool isAlpha)
{
	if (isHidden) return;

	if (!isHover)
		imButton[0].Draw(dc, rcRect);
	else if (isPress)
	{
		if (imButton[2].IsValid())
			imButton[2].Draw(dc, rcRect);
		else if (imButton[1].IsValid())
			imButton[1].Draw(dc, rcRect);
		else
			imButton[0].Draw(dc, rcRect);
	}
	else if (isHover)
	{
		if (imButton[1].IsValid())
			imButton[1].Draw(dc, rcRect);
		else
			imButton[0].Draw(dc, rcRect);
	}
}

bool SkinButton::DrawFade(HDC dc, bool isAlpha)
{
	if (isPress)
	{
		if (imButton[2].IsValid())
			imButton[2].Draw(dc, rcRect);
		else
			imButton[1].Draw(dc, rcRect);
		fadeAlpha = 255;
		return true;
	}

	if (!isHover)
	{
		fadeAlpha -= FadeValue::StepOut;
		fadeAlpha = std::max(0, fadeAlpha);

		if (fadeAlpha == 0)
			imButton[0].Draw(dc, rcRect);
		else
		{
			imButton[0].DrawFade(dc, rcRect, 255 - fadeAlpha);
			imButton[1].DrawFade(dc, rcRect, fadeAlpha);
		}
	}
	else if (isHover)
	{
		fadeAlpha += FadeValue::StepIn;
		fadeAlpha = std::min(255, fadeAlpha);

		if (fadeAlpha == 255)
			imButton[1].Draw(dc, rcRect);
		else
		{
			imButton[0].DrawFade(dc, rcRect, 255 - fadeAlpha);
			imButton[1].DrawFade(dc, rcRect, fadeAlpha);
		}
	}

	if (fadeAlpha == 0 || fadeAlpha == 255)
		return true;
	else
		return false;
}

int SkinButton::GetWidth()
{
	return imButton[0].Width();
}

int SkinButton::GetHeight()
{
	return imButton[0].Height();
}

bool SkinButton::IsRedrawHover()
{
	if (isPress)
	{
		if (imButton[2].IsValid())
			return true;
		if (imButton[1].IsValid())
			return true;

		return false;
	}

	if (imButton[1].IsValid())
		return true;

	return false;
}

bool SkinButton::IsRedrawPress()
{
	if (imButton[2].IsValid())
		return true;

	return false;
}

SkinElement* SkinButton::OnMouseMove(unsigned flags, CPoint& point)
{
	if (!isHover && rcRect.PtInRect(point)) // Mouse enters button area
	{
		//fadeAlpha = 0;

		isHover = true;
		return this;
	}
	else if (isHover && !rcRect.PtInRect(point)) // Mouse leaves button area
	{
		isHover = false;
		return this;
	}
	
	return nullptr;
}

SkinElement* SkinButton::OnButtonDown(unsigned flags, CPoint& point)
{
	if (rcRect.PtInRect(point))
	{
		isPress = true;
		return this;
	}

	return nullptr;
}

SkinElement* SkinButton::OnButtonUp(unsigned flags, CPoint& point)
{
//	if (rRect.PtInRect(point))
//	{
	if (isPress)
	{
		if (!isHover)
			fadeAlpha = 0;

		isPress = false;
		return this;
	}

	return nullptr;
//	}
}

SkinElement* SkinButton::OnMouseLeave()
{
	if (isHover)
	{
		isHover = false;
		return this;
	}
	
	return nullptr;
}

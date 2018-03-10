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
#include "SkinSwitch.h"

SkinSwitch::SkinSwitch()
{

}

SkinSwitch::~SkinSwitch()
{

}

bool SkinSwitch::LoadSkin(const std::wstring& file, ZipFile* zipFile)
{
	std::wstring path = PathEx::PathFromFile(file);

	XmlFile xmlFile;

	if (xmlFile.LoadEx(file, zipFile))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Switch");

		if (xmlMain)
		{
			XmlNode xmlState1 = xmlMain.FirstChild("State1");

			if (xmlState1)
			{
				XmlNode xmlNormal = xmlState1.FirstChild("Normal");
				if (xmlNormal)
				{
					std::wstring attr = xmlNormal.Attribute16("File");
					if (!attr.empty())
						imButton[0].LoadEx(path + attr, zipFile);
				}

				XmlNode xmlHover = xmlState1.FirstChild("Hover");
				if (xmlHover)
				{
					std::wstring attr = xmlHover.Attribute16("File");
					if (!attr.empty())
						imButton[1].LoadEx(path + attr, zipFile);
				}

				XmlNode xmlPress = xmlState1.FirstChild("Press");
				if (xmlPress)
				{
					std::wstring attr = xmlPress.Attribute16("File");
					if (!attr.empty())
						imButton[2].LoadEx(path + attr, zipFile);
				}
			}

			XmlNode xmlState2 = xmlMain.FirstChild("State2");

			if (xmlState2)
			{
				XmlNode xmlNormal = xmlState2.FirstChild("Normal");
				if (xmlNormal)
				{
					std::wstring attr = xmlNormal.Attribute16("File");
					if (!attr.empty())
						imButton2[0].LoadEx(path + attr, zipFile);
				}

				XmlNode xmlHover = xmlState2.FirstChild("Hover");
				if (xmlHover)
				{
					std::wstring attr = xmlHover.Attribute16("File");
					if (!attr.empty())
						imButton2[1].LoadEx(path + attr, zipFile);
				}

				XmlNode xmlPress = xmlState2.FirstChild("Press");
				if (xmlPress)
				{
					std::wstring attr = xmlPress.Attribute16("File");
					if (!attr.empty())
						imButton2[2].LoadEx(path + attr, zipFile);
				}
			}

			XmlNode xmlEffect = xmlMain.FirstChild("Effect");
			if (xmlEffect)
				xmlEffect.Attribute("Fade", &isFadeEffect);
		}
	}
	else
		return false;

	return true;
}

void SkinSwitch::Draw(HDC dc, bool isAlpha)
{
	if (!isState)
	{
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
	else
	{
		if (!isHover)
			imButton2[0].Draw(dc, rcRect);
		else if (isPress)
		{
			if (imButton2[2].IsValid())
				imButton2[2].Draw(dc, rcRect);
			else if (imButton2[1].IsValid())
				imButton2[1].Draw(dc, rcRect);
			else
				imButton2[0].Draw(dc, rcRect);
		}
		else if (isHover)
		{
			if (imButton2[1].IsValid())
				imButton2[1].Draw(dc, rcRect);
			else
				imButton2[0].Draw(dc, rcRect);
		}
	}
}

bool SkinSwitch::DrawFade(HDC dc, bool isAlpha)
{
	if (isPress)
	{
		if (!isState)
		{
			if (imButton[2].IsValid())
				imButton[2].Draw(dc, rcRect);
			else
				imButton[1].Draw(dc, rcRect);
		}
		else
		{
			if (imButton2[2].IsValid())
				imButton2[2].Draw(dc, rcRect);
			else
				imButton2[1].Draw(dc, rcRect);
		}

		fadeAlpha = 255;
		return true;
	}

	if (!isState)
	{
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
	}
	else
	{
		if (!isHover)
		{
			fadeAlpha -= FadeValue::StepOut;
			fadeAlpha = std::max(0, fadeAlpha);

			if (fadeAlpha == 0)
				imButton2[0].Draw(dc, rcRect);
			else
			{
				imButton2[0].DrawFade(dc, rcRect, 255 - fadeAlpha);
				imButton2[1].DrawFade(dc, rcRect, fadeAlpha);
			}
		}
		else if (isHover)
		{
			fadeAlpha += FadeValue::StepIn;
			fadeAlpha = std::min(255, fadeAlpha);

			if (fadeAlpha == 255)
				imButton2[1].Draw(dc, rcRect);
			else
			{
				imButton2[0].DrawFade(dc, rcRect, 255 - fadeAlpha);
				imButton2[1].DrawFade(dc, rcRect, fadeAlpha);
			}
		}
	}

	if (fadeAlpha == 0 || fadeAlpha == 255)
		return true;
	else
		return false;
}

int SkinSwitch::GetWidth()
{
	return imButton[0].Width();
}

int SkinSwitch::GetHeight()
{
	return imButton[0].Height();
}

bool SkinSwitch::IsRedrawHover()
{
	if (isPress)
	{
		if (!isState)
		{
			if (imButton[2].IsValid())
				return true;
			if (imButton[1].IsValid())
				return true;
		}
		else
		{
			if (imButton2[2].IsValid())
				return true;
			if (imButton2[1].IsValid())
				return true;
		}

		return false;
	}

	if (!isState)
	{
		if (imButton[1].IsValid())
			return true;
	}
	else
	{
		if (imButton2[1].IsValid())
			return true;
	}

	return false;
}

bool SkinSwitch::IsRedrawPress()
{
	if (!isState)
	{
		if (imButton[2].IsValid())
			return true;
	}
	else
	{
		if (imButton2[2].IsValid())
			return true;
	}

	return false;
}

SkinElement* SkinSwitch::OnMouseMove(unsigned flags, CPoint& point)
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

SkinElement* SkinSwitch::OnButtonDown(unsigned flags, CPoint& point)
{
	if (rcRect.PtInRect(point))
	{
		isPress = true;
		return this;
	}

	return nullptr;
}

SkinElement* SkinSwitch::OnButtonUp(unsigned flags, CPoint& point)
{
	if (isPress)
	{
//		if (rcRect.PtInRect(point))
//			isState = !isState;
		if (!isHover)
			fadeAlpha = 0;

		isPress = false;
		return this;
	}
	
	return nullptr;
}

SkinElement* SkinSwitch::OnMouseLeave()
{
	if (isHover)
	{
		isHover = false;
		return this;
	}
	
	return nullptr;
}

int SkinSwitch::GetParam()
{
	return (int)isState;
}

void SkinSwitch::SetParam(int param)
{
	isState = (param ? true : false);
}


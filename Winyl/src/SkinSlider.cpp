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

// SkinSlider.cpp : implementation file
//

#include "stdafx.h"
#include "SkinSlider.h"


// SkinSlider

SkinSlider::SkinSlider()
{

}

SkinSlider::~SkinSlider()
{

}

bool SkinSlider::LoadSkin(const std::wstring& file, ZipFile* zipFile)
{
	std::wstring path = PathEx::PathFromFile(file);

	XmlFile xmlFile;

	if (xmlFile.LoadEx(file, zipFile))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Slider");

		if (xmlMain)
		{
			XmlNode xmlBackground = xmlMain.FirstChild("Background");
			if (xmlBackground)
			{
				XmlNode xmlFill = xmlBackground.FirstChild("Fill");
				if (xmlFill)
				{
					std::wstring attrFile = xmlFill.Attribute16("File");
					if (!attrFile.empty())
						imFill[0].LoadEx(path + attrFile, zipFile);

					std::wstring attrFile2 = xmlFill.Attribute16("File2");
					if (!attrFile2.empty())
						imFill[1].LoadEx(path + attrFile2, zipFile);

					XmlNode xmlBegin = xmlBackground.FirstChild("Begin");

					if (!xmlBegin) // For compatibility with old skins (added in v2.2)
						xmlBegin = xmlBackground.FirstChild("Start");

					if (xmlBegin)
					{
						isBegin = true;

						std::wstring attr = xmlBegin.Attribute16("File");
						if (!attr.empty())
							imBegin.LoadEx(path + attr, zipFile);
					}

					XmlNode xmlEnd = xmlBackground.FirstChild("End");

					if (xmlEnd)
					{
						isEnd = true;

						std::wstring attr = xmlEnd.Attribute16("File");
						if (!attr.empty())
							imEnd.LoadEx(path + attr, zipFile);
					}
				}
				else
				{
					isBackground = true;

					std::wstring attrFile = xmlBackground.Attribute16("File");
					if (!attrFile.empty())
						imFill[0].LoadEx(path + attrFile, zipFile);

					std::wstring attrFile2 = xmlBackground.Attribute16("File2");
					if (!attrFile2.empty())
						imFill[1].LoadEx(path + attrFile2, zipFile);
				}
			}

			XmlNode xmlButton = xmlMain.FirstChild("Button");
			
			if (xmlButton)
			{
				isButton = true;

				XmlNode xmlNormal = xmlButton.FirstChild("Normal");
				if (xmlNormal)
				{
					std::wstring attr = xmlNormal.Attribute16("File");
					if (!attr.empty())
						imButton[0].LoadEx(path + attr, zipFile);
				}

				XmlNode xmlHover = xmlButton.FirstChild("Hover");
				if (xmlHover)
				{
					std::wstring attr = xmlHover.Attribute16("File");
					if (!attr.empty())
						imButton[1].LoadEx(path + attr, zipFile);
				}

				XmlNode xmlPress = xmlButton.FirstChild("Press");
				if (xmlPress)
				{
					std::wstring attr = xmlPress.Attribute16("File");
					if (!attr.empty())
						imButton[2].LoadEx(path + attr, zipFile);
				}
			}
		}
	}
	else
		return false;

	return true;
}

void SkinSlider::SetRect(CRect& rcDraw)
{
	rcRect = rcDraw;

	if (!isButton)
		position = (percent * rcRect.Width() + 100000 / 2) / 100000;
	else
		position = (percent * (rcRect.Width() - imButton[0].Width()) + 100000 / 2) / 100000;
}

void SkinSlider::Draw(HDC dc, bool isAlpha)
{
	int pos = position;
	int posButton = pos;
	pos += imButton[0].Width() / 2;

	if (isBackground)
	{
		//imFill[0].Draw(dc, rcRect);
		imFill[1].Crop(dc, rcRect.left, rcRect.top, pos, rcRect.Height());
		//imFill[0].Crop(dc, rcRect.left + pos, rcRect.top, rcRect.Width() - pos, rcRect.Height());

		//imFill[1].Crop2(dc, rcRect.left, rcRect.top, CRect(0, 0, iPos, rcRect.Height()));
		imFill[0].Crop2(dc, rcRect.left + pos, rcRect.top, CRect(pos, 0, rcRect.Width(), rcRect.Height()));
	}
	else
	{
		//imFill[0].Draw(dc, rcRect);
		imFill[1].Draw(dc, rcRect.left + imBegin.Width(), rcRect.top, pos - imBegin.Width(), rcRect.Height());
		imFill[0].Draw(dc, rcRect.left + pos, rcRect.top, rcRect.Width() - pos - imEnd.Width(), rcRect.Height());

		imBegin.Draw(dc, rcRect.left, rcRect.top);
		imEnd.Draw(dc, rcRect.right - imEnd.Width(), rcRect.top);
	}

	if (isButton)
	{
		if (!isHover)
			imButton[0].Draw(dc, rcRect.left + posButton, rcRect.top);
		else if (isPress)
		{
			if (imButton[2].IsValid())
				imButton[2].Draw(dc, rcRect.left + posButton, rcRect.top);
			else if (imButton[1].IsValid())
				imButton[1].Draw(dc, rcRect.left + posButton, rcRect.top);
			else
				imButton[0].Draw(dc, rcRect.left + posButton, rcRect.top);
		}
		else if (isHover)
		{
			if (imButton[1].IsValid())
				imButton[1].Draw(dc, rcRect.left + posButton, rcRect.top);
			else
				imButton[0].Draw(dc, rcRect.left + posButton, rcRect.top);
		}
	}
}

int SkinSlider::GetWidth()
{
	if (isBackground)
		return imFill[0].Width();
	else
		return 100;
}

int SkinSlider::GetHeight()
{
	return imFill[0].Height();
}

bool SkinSlider::IsRedrawHover()
{
	return true;
}

bool SkinSlider::IsRedrawPress()
{
	return true;
}

SkinElement* SkinSlider::OnMouseMove(unsigned flags, CPoint& point)
{
	if (isPress)
	{
		CalcPositionByPoint(point, position, percent);

		if (position == lastPosition)
			return nullptr;

/*		AllocConsole();
		freopen ("CONOUT$", "w", stdout ); 
		printf("%i\n", percent);
*/
		lastPosition = position;
		return this;
	}

	if (!isHover && rcRect.PtInRect(point))
	{
		isHover = true;
		if (isButton) return this;
	}
	if (isHover && !rcRect.PtInRect(point))
	{
		isHover = false;
		if (isButton) return this;
	}

	return nullptr;
}

SkinElement* SkinSlider::OnButtonDown(unsigned flags, CPoint& point)
{
	if (rcRect.PtInRect(point))
	{
		isPress = true;

		CalcPositionByPoint(point, position, percent);

		return this;
	}

	return nullptr;
}

SkinElement* SkinSlider::OnButtonUp(unsigned flags, CPoint& point)
{
	if (isPress)
	{
		isPress = false;
		return this;
	}
	
	return nullptr;
}

SkinElement* SkinSlider::OnMouseLeave()
{
	if (isHover)
	{
		isHover = false;
		return this;
	}
	
	return nullptr;
}

int SkinSlider::GetParam()
{
	return percent;
}

void SkinSlider::SetParam(int param)
{
	if (param > 100000) param = 100000;
	else if (param < 0) param = 0;
	
	percent = param;

	if (!isButton)
		position = (percent * rcRect.Width() + 100000 / 2) / 100000;
	else
		position = (percent * (rcRect.Width() - imButton[0].Width()) + 100000 / 2) / 100000;
}

void SkinSlider::CalcPositionByPoint(CPoint point, int& outPosition, int& outPercent)
{
	if (!isButton)
	{
		//point.x += 1; // Adjust

		if (point.x < rcRect.left)
			point.x = rcRect.left;
		if (point.x > rcRect.right)
			point.x = rcRect.right;

		outPosition = point.x - rcRect.left;
		outPercent = (outPosition * 100000 + rcRect.Width() / 2) / rcRect.Width();
	}
	else
	{
		//point.x += 2; // Adjust

		int width = imButton[0].Width();
		int halfLeft = width / 2;
		int halfRight = width - halfLeft;

		if (point.x < rcRect.left + halfLeft)
			point.x = rcRect.left + halfLeft;
		if (point.x > rcRect.right - halfRight)
			point.x = rcRect.right - halfRight;

		outPosition = point.x - (rcRect.left + halfLeft);
		outPercent = (outPosition * 100000 + (rcRect.Width() - width) / 2) / (rcRect.Width() - width);
	}
}

int SkinSlider::GetThumbPosition()
{
	if (isButton)
		return position + imButton[0].Width() / 2;

	return position;
}

int SkinSlider::CalcPercent(const CPoint& point)
{
	int outPosition = 0;
	int outPercent = 0;

	CalcPositionByPoint(point, outPosition, outPercent);

	return outPercent;
}
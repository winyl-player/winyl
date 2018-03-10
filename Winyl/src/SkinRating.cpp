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
#include "SkinRating.h"

int SkinRating::rating = 0;

SkinRating::SkinRating()
{

}

SkinRating::~SkinRating()
{

}

bool SkinRating::LoadSkin(const std::wstring& file, ZipFile* zipFile)
{
	std::wstring path = PathEx::PathFromFile(file);

	XmlFile xmlFile;

	if (xmlFile.LoadEx(file, zipFile))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Rating");

		if (xmlMain)
		{
			XmlNode xmlShow = xmlMain.FirstChild("Show");
			if (xmlShow)
			{
				std::wstring attr = xmlShow.Attribute16("File");
				if (!attr.empty())
					imShow.LoadEx(path + attr, zipFile);
			}

			XmlNode xmlHide = xmlMain.FirstChild("Hide");
			if (xmlHide)
			{
				std::wstring attr = xmlHide.Attribute16("File");
				if (!attr.empty())
					imHide.LoadEx(path + attr, zipFile);
			}

			XmlNode xmlShowHover = xmlMain.FirstChild("ShowHover");
			if (xmlShowHover)
			{
				std::wstring attr = xmlShowHover.Attribute16("File");
				if (!attr.empty())
					imShowHover.LoadEx(path + attr, zipFile);
			}

			XmlNode xmlHideHover = xmlMain.FirstChild("HideHover");
			if (xmlHideHover)
			{
				std::wstring attr = xmlHideHover.Attribute16("File");
				if (!attr.empty())
					imHideHover.LoadEx(path + attr, zipFile);
			}
		}

	}
	else
		return false;

	return true;
}

void SkinRating::Draw(HDC dc, bool isAlpha)
{
	if (!isVisible)
		return;

	int width = imShow.Width();

	if (isHover)
	{
		for (int i = 0; i < 5; ++i)
		{
			if (rating2 > i)
				imShowHover.Draw(dc, rcRect.left + width * i, rcRect.top);
			else
				imHideHover.Draw(dc, rcRect.left + width * i, rcRect.top);
		}
	}
	else
	{
		for (int i = 0; i < 5; ++i)
		{
			if (rating > i)
				imShow.Draw(dc, rcRect.left + width * i, rcRect.top);
			else
				imHide.Draw(dc, rcRect.left + width * i, rcRect.top);
		}
	}
}

int SkinRating::GetWidth()
{
	return (imShow.Width() * 5);
}

int SkinRating::GetHeight()
{
	return imShow.Height();
}

bool SkinRating::IsRedrawHover()
{
	if (imShowHover.IsValid())
		return true;

	return false;
}

bool SkinRating::IsRedrawPress()
{
	return true;
}

SkinElement* SkinRating::OnMouseMove(unsigned flags, CPoint& point)
{
	if (!isEnable)
		return nullptr;

	if (!imShowHover.IsValid() && !imHideHover.IsValid())
		return nullptr;

	if (rcRect.PtInRect(point))
	{
		if (!isHover)
			isHover = true;

		bool isRedraw = false;

		// Calculate which star was clicked
		int star = rcRect.Width() / 5;
		int pos = point.x - rcRect.left;
		for (int i = 1; i <= 5; ++i)
		{
			if (pos <= star * i)
			{	
				if (i != rating2)
				{
					rating2 = i;

					if (rating3 != rating2)
					{
						rating3 = 0;
						isRedraw = true;
					}
				}
				break;
			}
		}

		if (isRedraw)
			return this;
		else
			return nullptr;
	}
	else if (isHover && !rcRect.PtInRect(point))
	{
		isHover = false;

		rating2 = 0;
		rating3 = 0;

		return this;
	}
	
	return nullptr;
}

SkinElement* SkinRating::OnButtonDown(unsigned flags, CPoint& point)
{
	if (!isEnable)
		return nullptr;

	if (rcRect.PtInRect(point))
	{
		isPress = true;

		// Calculate which star was clicked
		int star = rcRect.Width() / 5;
		int pos = point.x - rcRect.left;
		for (int i = 1; i <= 5; ++i)
		{
			if (pos <= star * i)
			{
				if (i == rating)
				{
					rating = 0; //--;
					
					rating3 = rating2;
					rating2 = rating;
				}
				else
					rating = i;
				break;
			}
		}

		return this;
	}

	return nullptr;
}

SkinElement* SkinRating::OnButtonUp(unsigned flags, CPoint& point)
{
	if (isPress)
	{
		isPress = false;
		return this;
	}
	
	return nullptr;
}

SkinElement* SkinRating::OnMouseLeave()
{
	if (isHover)
	{
		isHover = false;
		return this;
	}
	
	return nullptr;
}

void SkinRating::SetParam(int param)
{
	if (param < 0) // Hide
	{
		isVisible = false;
		isEnable = false;
		rating = 0;
	}
	else if (param < 100) // Show
	{
		isVisible = true;
		isEnable = true;
		rating = param;
	}
	else // Disable
	{
		isVisible = true;
		isEnable = false;
		rating = param - 100;
	}
}

int SkinRating::GetParam()
{
	return rating;
}

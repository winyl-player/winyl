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
#include "SkinSplitter.h"

SkinSplitter::SkinSplitter()
{

}

SkinSplitter::~SkinSplitter()
{

}

bool SkinSplitter::LoadSkin(const std::wstring& file, ZipFile* zipFile)
{
	std::wstring path = PathEx::PathFromFile(file);

	XmlFile xmlFile;

	if (xmlFile.LoadEx(file, zipFile))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Splitter");

		if (xmlMain)
		{
			XmlNode xmlSettings = xmlMain.FirstChild("Settings");
			if (xmlSettings)
			{
				xmlSettings.Attribute("Horizontal", &isHorizontal);
				xmlSettings.Attribute("Vertical", &isVertical);

				int maxLeft = 0, maxRight = 0, maxTop = 0, maxBottom = 0;

				xmlSettings.Attribute("MaxLeft", &maxLeft);
				xmlSettings.Attribute("MaxRight", &maxRight);
				xmlSettings.Attribute("MaxTop", &maxTop);
				xmlSettings.Attribute("MaxBottom", &maxBottom);

				rcMax.SetRect(maxLeft, maxTop, maxRight, maxBottom);
			}

			XmlNode xmlLayouts = xmlMain.FirstChild("Layouts");
			if (xmlLayouts)
			{
				for (XmlNode xmlNode = xmlLayouts.FirstChild(); xmlNode; xmlNode = xmlNode.NextChild())
				{
					structSplitter splitter = {};

					xmlNode.Attribute("ID", &splitter.id);

					xmlNode.Attribute("MoveLeft", &splitter.isMoveLeft);
					xmlNode.Attribute("MoveRight", &splitter.isMoveRight);
					xmlNode.Attribute("MoveTop", &splitter.isMoveTop);
					xmlNode.Attribute("MoveBottom", &splitter.isMoveBottom);

					layouts.push_back(splitter);
				}
			}
		}
	}
	else
		return false;

	return true;
}

SkinElement* SkinSplitter::OnMouseMove(unsigned flags, CPoint& point)
{
	if (!isHover && rcRect.PtInRect(point)) // Mouse enters splitter area
	{
		isHover = true;
	}
	else if (isHover && !rcRect.PtInRect(point)) // Mouse leaves splitter area
	{
		isHover = false;
	}
	
	return nullptr;
}

SkinElement* SkinSplitter::OnButtonDown(unsigned flags, CPoint& point)
{
	if (rcRect.PtInRect(point))
	{
		isPress = true;
	}

	return nullptr;
}

SkinElement* SkinSplitter::OnButtonUp(unsigned flags, CPoint& point)
{
	if (isPress)
	{
		isPress = false;
	}

	return nullptr;
}

SkinElement* SkinSplitter::OnMouseLeave()
{
	if (isHover)
	{
		isHover = false;
	}
	
	return nullptr;
}





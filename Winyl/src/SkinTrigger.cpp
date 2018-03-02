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

// SkinTrigger.cpp : implementation file
//

#include "stdafx.h"
#include "SkinTrigger.h"


// SkinTrigger

SkinTrigger::SkinTrigger()
{

}

SkinTrigger::~SkinTrigger()
{

}

void SkinTrigger::LoadTriggerElement(XmlNode& xmlNode)
{
	for (XmlNode xmlTrigger = xmlNode.FirstChild(); xmlTrigger; xmlTrigger = xmlTrigger.NextChild())
	{
		structTrigger trigger;
		trigger.id     = 0;
		trigger.isHide = false;
		trigger.width  = -1;
		trigger.height = -1;
		trigger.left   = -1;
		trigger.right  = -1;
		trigger.top    = -1;
		trigger.bottom = -1;
		trigger.leftRight = -1;
		trigger.topBottom = -1;
		trigger.leftFromRightID = 0;
		trigger.rightFromLeftID = 0;
		trigger.topFromBottomID = 0;
		trigger.bottomFromTopID = 0;
		trigger.leftFromLeftID     = 0;
		trigger.rightFromRightID   = 0;
		trigger.topFromTopID       = 0;
		trigger.bottomFromBottomID = 0;

		xmlTrigger.Attribute("ID",     &trigger.id);
		xmlTrigger.Attribute("Hide",   &trigger.isHide);
		xmlTrigger.Attribute("Width",  &trigger.width);
		xmlTrigger.Attribute("Height", &trigger.height);
		xmlTrigger.Attribute("Left",   &trigger.left);
		xmlTrigger.Attribute("Right",  &trigger.right);
		xmlTrigger.Attribute("Top",    &trigger.top);
		xmlTrigger.Attribute("Bottom", &trigger.bottom);
		xmlTrigger.Attribute("LeftRight", &trigger.leftRight);
		xmlTrigger.Attribute("TopBottom", &trigger.topBottom);
		xmlTrigger.Attribute("LeftFromRightID", &trigger.leftFromRightID);
		xmlTrigger.Attribute("RightFromLeftID", &trigger.rightFromLeftID);
		xmlTrigger.Attribute("TopFromBottomID", &trigger.topFromBottomID);
		xmlTrigger.Attribute("BottomFromTopID", &trigger.bottomFromTopID);
		xmlTrigger.Attribute("LeftFromLeftID",     &trigger.leftFromLeftID);
		xmlTrigger.Attribute("RightFromRightID",   &trigger.rightFromRightID);
		xmlTrigger.Attribute("TopFromTopID",       &trigger.topFromTopID);
		xmlTrigger.Attribute("BottomFromBottomID", &trigger.bottomFromBottomID);

		layouts.push_back(trigger);
	}
}

bool SkinTrigger::LoadTrigger(const std::wstring& file, ZipFile* zipFile)
{
	//Deprecated function

	//assert(false);

	XmlFile xmlFile;

	if (xmlFile.LoadEx(file, zipFile))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Trigger");

		if (xmlMain)
		{
			for (XmlNode xmlTrigger = xmlMain.FirstChild(); xmlTrigger; xmlTrigger = xmlTrigger.NextChild())
			{
				structTrigger trigger;
				trigger.id     = 0;
				trigger.isHide = false;
				trigger.width  = -1;
				trigger.height = -1;
				trigger.left   = -1;
				trigger.right  = -1;
				trigger.top    = -1;
				trigger.bottom = -1;
				trigger.leftRight = -1;
				trigger.topBottom = -1;
				trigger.leftFromRightID = 0;
				trigger.rightFromLeftID = 0;
				trigger.topFromBottomID = 0;
				trigger.bottomFromTopID = 0;
				trigger.leftFromLeftID     = 0;
				trigger.rightFromRightID   = 0;
				trigger.topFromTopID       = 0;
				trigger.bottomFromBottomID = 0;

				xmlTrigger.Attribute("ID",     &trigger.id);
				xmlTrigger.Attribute("Hide",   &trigger.isHide);
				xmlTrigger.Attribute("Width",  &trigger.width);
				xmlTrigger.Attribute("Height", &trigger.height);
				xmlTrigger.Attribute("Left",   &trigger.left);
				xmlTrigger.Attribute("Right",  &trigger.right);
				xmlTrigger.Attribute("Top",    &trigger.top);
				xmlTrigger.Attribute("Bottom", &trigger.bottom);
				xmlTrigger.Attribute("LeftRight", &trigger.leftRight);
				xmlTrigger.Attribute("TopBottom", &trigger.topBottom);
				xmlTrigger.Attribute("LeftFromRightID", &trigger.leftFromRightID);
				xmlTrigger.Attribute("RightFromLeftID", &trigger.rightFromLeftID);
				xmlTrigger.Attribute("TopFromBottomID", &trigger.topFromBottomID);
				xmlTrigger.Attribute("BottomFromTopID", &trigger.bottomFromTopID);
				xmlTrigger.Attribute("LeftFromLeftID",     &trigger.leftFromLeftID);
				xmlTrigger.Attribute("RightFromRightID",   &trigger.rightFromRightID);
				xmlTrigger.Attribute("TopFromTopID",       &trigger.topFromTopID);
				xmlTrigger.Attribute("BottomFromBottomID", &trigger.bottomFromBottomID);

				layouts.push_back(trigger);
			}
		}
	}
	else
		return false;

	return true;
}



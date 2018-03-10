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

#pragma once

#include "XmlFile.h"
#include "UTF.h"

class SkinTrigger
{

public:
	SkinTrigger();
	virtual ~SkinTrigger();

	void LoadTriggerElement(XmlNode& xmlNode);
	bool LoadTrigger(const std::wstring& file, ZipFile* zipFile);

	struct structTrigger
	{
		int id; // Element ID
		bool isHide;
		int width;
		int height;
		int left;
		int right;
		int top;
		int bottom;
		int leftRight;
		int topBottom;
		int leftFromRightID;
		int rightFromLeftID;
		int topFromBottomID;
		int bottomFromTopID;
		int leftFromLeftID;
		int rightFromRightID;
		int topFromTopID;
		int bottomFromBottomID;
	};

private:
	std::vector<structTrigger> layouts;

public:
	inline std::vector<structTrigger>& Layouts() {return layouts;}
};



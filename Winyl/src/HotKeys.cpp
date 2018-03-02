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

// HotKeys.cpp : implementation file
//

#include "stdafx.h"
#include "HotKeys.h"


// HotKeys

HotKeys::HotKeys()
{

}

HotKeys::~HotKeys()
{

}

bool HotKeys::LoadHotKeys()
{
	std::wstring file = profilePath + L"HotKeys.xml";

	XmlFile xmlFile;

	if (xmlFile.LoadFile(file))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("HotKeys");

		// Old version
		if (!xmlMain)
			xmlMain = xmlFile.RootNode().FirstChild("HotKey");

		if (xmlMain)
		{
			for (XmlNode xmlNode = xmlMain.FirstChild("Key"); xmlNode; xmlNode = xmlNode.NextChild("Key"))
			{
				int type = 0;
				if (xmlNode.Attribute("Type", &type))
				{
					structKey skey;
					skey.type = (KeyType)type;

					xmlNode.Attribute("ID", &skey.key);

					xmlNode.Attribute("Ctrl", &skey.isCtrl);
					xmlNode.Attribute("Alt", &skey.isAlt);
					xmlNode.Attribute("Win", &skey.isWin);
					xmlNode.Attribute("Shift", &skey.isShift);
					
					keys.push_back(skey);
				}
			}
		}
	}
	else
	{
		AddDefault();
		return false;
	}

	return true;
}

bool HotKeys::SaveHotKeys()
{
	XmlFile xmlFile;
	XmlNode xmlMain = xmlFile.RootNode().AddChild("HotKeys");
	
	for (std::size_t i = 0, size = keys.size(); i < size; ++i)
	{
		XmlNode xmlKey = xmlMain.AddChild("Key");

		xmlKey.AddAttribute("Type", (int)keys[i].type);
		xmlKey.AddAttribute("ID", keys[i].key);

		if (keys[i].isCtrl)
			xmlKey.AddAttribute("Ctrl", keys[i].isCtrl);
		if (keys[i].isAlt)
			xmlKey.AddAttribute("Alt", keys[i].isAlt);
		if (keys[i].isWin)
			xmlKey.AddAttribute("Win", keys[i].isWin);
		if (keys[i].isShift)
			xmlKey.AddAttribute("Shift", keys[i].isShift);
	}

	std::wstring file = profilePath + L"HotKeys.xml";

	if (xmlFile.SaveFile(file))
		return true;

	return false;
}

void HotKeys::AddKey(KeyType type, int key, bool isCtrl, bool isAlt, bool isWin, bool isShift)
{
	structKey skey;

	skey.type = type;
	skey.key = key;
	skey.isCtrl = isCtrl;
	skey.isAlt = isAlt;
	skey.isWin = isWin;
	skey.isShift = isShift;

	keys.push_back(skey);
}

std::size_t HotKeys::FindKeyByType(KeyType type)
{
	for (std::size_t i = 0, size = keys.size(); i < size; ++i)
	{
		if (keys[i].type == type)
			return i;
	}

	return -1;
}

void HotKeys::AddDefault()
{
//	AddKey(KeyType::Play, '1', true, true, false);
	AddKey(KeyType::Pause, VK_MEDIA_PLAY_PAUSE, false, false, false, false);
	AddKey(KeyType::Stop, VK_MEDIA_STOP, false, false, false, false);
	AddKey(KeyType::Next, VK_MEDIA_NEXT_TRACK, false, false, false, false);
	AddKey(KeyType::Prev, VK_MEDIA_PREV_TRACK, false, false, false, false);
//	AddKey(KeyType::Mute, VK_VOLUME_MUTE, false, false, false);
//	AddKey(KeyType::VolumeUp, VK_VOLUME_UP, false, false, false);
//	AddKey(KeyType::VolumeDown, VK_VOLUME_DOWN, false, false, false);
}

void HotKeys::RegisterHotKeys(HWND wnd)
{
	for (std::size_t i = 0, size = keys.size(); i < size; ++i)
	{
		UINT modifiers = 0;

		if (keys[i].isCtrl)
			modifiers |= MOD_CONTROL;
		if (keys[i].isAlt)
			modifiers |= MOD_ALT;
		if (keys[i].isWin)
			modifiers |= MOD_WIN;
		if (keys[i].isShift)
			modifiers |= MOD_SHIFT;
		
		::RegisterHotKey(wnd, (int)keys[i].type, modifiers, keys[i].key);
	}
}

void HotKeys::UnregisterHotKeys(HWND wnd)
{
	for (std::size_t i = 0, size = keys.size(); i < size; ++i)
	{		
		::UnregisterHotKey(wnd, (int)keys[i].type);
	}
}

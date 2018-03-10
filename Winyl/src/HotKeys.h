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

#include <vector>
#include "XmlFile.h"

class HotKeys
{

public:
	HotKeys();
	virtual ~HotKeys();

	inline void SetProfilePath(const std::wstring& path) {profilePath = path;}
	bool LoadHotKeys();
	bool SaveHotKeys();

	enum class KeyType
	{
		None       = 0,
		Play       = 1,
		Pause      = 2,
		Stop       = 3,
		Next       = 4,
		Prev       = 5,
		Mute       = 6,
		VolumeUp   = 7,
		VolumeDown = 8,
		Rating1    = 9,
		Rating2    = 10,
		Rating3    = 11,
		Rating4    = 12,
		Rating5    = 13,
		Popup      = 14
	};

	struct structKey
	{
		KeyType type = KeyType::None;
		int key      = 0;
		bool isCtrl  = false;
		bool isAlt   = false;
		bool isWin   = false;
		bool isShift = false;
	};

	std::size_t FindKeyByType(KeyType type);
	inline void RemoveAllKeys() {keys.clear();}
	const structKey& GetKey(std::size_t index) {return keys[index];}
	void AddKey(const structKey& key) {keys.push_back(key);}
	void AddKey(KeyType type, int key, bool isCtrl, bool isAlt, bool isWin, bool isShift);

	void RegisterHotKeys(HWND wnd);
	void UnregisterHotKeys(HWND wnd);

private:
	std::wstring profilePath;

	std::vector<structKey> keys;

	void AddDefault();

};



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

class MessageBox
{
public:
	MessageBox();
	virtual ~MessageBox();

	enum class Button
	{
		OK = 0,
		OKCancel = 1,
		YesNo = 2
	};

	enum class Icon
	{
		Information = 0,
		Warning = 1,
		Error = 2,
		Question = 3
	};

	static bool Show(HWND parent, const wchar_t* title, const wchar_t* instruction, Icon icon = Icon::Information, Button buttons = Button::OK);

private:
	static bool ShowXP(HWND parent, const wchar_t* title, const wchar_t* instruction, Icon icon, Button buttons);
	static bool ShowVista(HWND parent, const wchar_t* title, const wchar_t* instruction, Icon icon, Button buttons);

	static HHOOK hhkXP;
	static LRESULT CALLBACK CBTProc(int, WPARAM, LPARAM);
};

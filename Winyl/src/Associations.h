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

#include <string>
#include <vector>

class Associations
{

public:
	Associations();
	virtual ~Associations();

	void RemoveAllAssoc(bool isVistaOrLater);

	// Assotiations for folders
	inline void SetProgramPath(const std::wstring& path) {programFile = path + L"Winyl.exe";}
	inline void SetPlayInWinylString(const std::wstring& string) {playInWinyl = string;}

	bool QueryAssocFolder();
	void AddAssocFolder();
	void RemoveAssocFolder();

	// Helper functions for Windows Vista/7
	void AssocDialogVista();

	// Helper functions for Windows XP
	void PrepareAssocExtXP();
	inline int GetAssocExtCountXP() {return (int)assocExts.size();}
	inline const std::wstring& GetAssocExtXP(int index) {return assocExts[index];}
	inline void ApplyAssocXP() {SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);}
	bool QueryAssocXP(int index);
	bool SetAssocXP(int index, bool isEnable);

	LONG HelperGetStringValue(HKEY hKey, LPCWSTR pszValueName, LPWSTR pszValue, LPDWORD ptrSize);
	LONG HelperSetStringValue(HKEY hKey, LPCWSTR pszValueName, LPCWSTR pszValue);
	LONG HelperDeleteKeyRecursively(HKEY hKey, LPCWSTR pszSubKey);


private:
	std::wstring winyl = L"Winyl";
	std::wstring dropTargetGUID = L"{254F0A10-5970-4149-9C8F-4E08D2525427}";
	std::wstring programFile;
	std::wstring playInWinyl;
	std::vector<std::wstring> assocExts;
};



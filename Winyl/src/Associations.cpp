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

// Associations.cpp : implementation file
//

#include "stdafx.h"
#include "Associations.h"


// Associations

Associations::Associations()
{

}

Associations::~Associations()
{

}

void Associations::RemoveAllAssoc(bool isVistaOrLater)
{
	if (isVistaOrLater)
		RemoveAssocFolder();
	else
	{
		RemoveAssocFolder();

		bool result = false;

		PrepareAssocExtXP();
		for (int i = 0, size = GetAssocExtCountXP(); i < size; ++i)
		{
			if (SetAssocXP(i, false) && !result)
				result = true;
		}

		if (result)
			ApplyAssocXP();
	}
}

bool Associations::QueryAssocFolder()
{
	HKEY key = NULL;
	if (::RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Classes\\Directory\\shell\\Winyl.Play", 0, KEY_READ, &key) == ERROR_SUCCESS)
	{
		::RegCloseKey(key);
		return true;
	}

	return false;
}

void Associations::AddAssocFolder()
{
	if (programFile.empty() || playInWinyl.empty())
		return;

	HKEY key = NULL;
	if (::RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\Classes\\Directory\\shell\\Winyl.Play", 0,
		REG_NONE, REG_OPTION_NON_VOLATILE, KEY_READ|KEY_WRITE, NULL, &key, NULL) == ERROR_SUCCESS)
	{
		HelperSetStringValue(key, NULL, playInWinyl.c_str());

		HKEY key2 = NULL;
		// Write open command
		if (::RegCreateKeyEx(key, L"command", 0,
			REG_NONE, REG_OPTION_NON_VOLATILE, KEY_READ|KEY_WRITE, NULL, &key2, NULL) == ERROR_SUCCESS)
		{
			HelperSetStringValue(key2, NULL, (L"\"" + programFile + L"\" \"%1\"").c_str());
			::RegCloseKey(key2);
		}

		// Write DropTarget GUID
		if (::RegCreateKeyEx(key, L"DropTarget", 0,
			REG_NONE, REG_OPTION_NON_VOLATILE, KEY_READ|KEY_WRITE, NULL, &key2, NULL) == ERROR_SUCCESS)
		{
			HelperSetStringValue(key2, L"Clsid", dropTargetGUID.c_str());
			::RegCloseKey(key2);
		}

		::RegCloseKey(key);
	}
}

void Associations::RemoveAssocFolder()
{
	HKEY key = NULL;
	if (::RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Classes\\Directory\\shell", 0, KEY_READ|KEY_WRITE, &key) == ERROR_SUCCESS)
	{
		HelperDeleteKeyRecursively(key, L"Winyl.Play");
		//::SHDeleteKey(key, L"Winyl.Play");
		//::RegDeleteKey(key, L"Winyl.Play\\command");
		//::RegDeleteKey(key, L"Winyl.Play\\DropTarget");
		//::RegDeleteKey(key, L"Winyl.Play");
		::RegCloseKey(key);
	}
}

void Associations::AssocDialogVista()
{
	AutoCom<IApplicationAssociationRegistrationUI> assocUI;
	if (assocUI.CoCreateInstance(CLSID_ApplicationAssociationRegistrationUI) == S_OK) // __uuidof(ApplicationAssociationRegistrationUI)
		assocUI->LaunchAdvancedAssociationUI(L"Winyl");
}

void Associations::PrepareAssocExtXP()
{
	assocExts.push_back(L".mp3");
	assocExts.push_back(L".cue");
	assocExts.push_back(L".ogg");
	assocExts.push_back(L".oga");
	assocExts.push_back(L".m4a");
	assocExts.push_back(L".aac");
	assocExts.push_back(L".wma");
	assocExts.push_back(L".asf");
	assocExts.push_back(L".wav");
	assocExts.push_back(L".aif");
	assocExts.push_back(L".aiff");
	assocExts.push_back(L".flac");
	assocExts.push_back(L".fla");
	assocExts.push_back(L".ape");
	assocExts.push_back(L".wv");
	assocExts.push_back(L".mpc");
	assocExts.push_back(L".tta");
	assocExts.push_back(L".opus");
	assocExts.push_back(L".spx");
}

bool Associations::QueryAssocXP(int index)
{
	HKEY key = NULL;
	if (::RegOpenKeyEx(HKEY_CLASSES_ROOT, assocExts[index].c_str(), 0, KEY_READ, &key) == ERROR_SUCCESS)
	{
		WCHAR strKey[256] = L"";
		DWORD sizeKey = sizeof(strKey);

		if (HelperGetStringValue(key, NULL, strKey, &sizeKey) == ERROR_SUCCESS)
		{
			if ((winyl + assocExts[index]) == strKey)
			{
				::RegCloseKey(key);
				return true;
			}

		}

		::RegCloseKey(key);
	}

	return false;
}

bool Associations::SetAssocXP(int index, bool isEnable)
{
	HKEY key = NULL;
	if (::RegOpenKeyEx(HKEY_CLASSES_ROOT, assocExts[index].c_str(), 0, KEY_READ|KEY_WRITE, &key) == ERROR_SUCCESS)
	{
		WCHAR strKey[256] = L"";
		DWORD sizeKey = sizeof(strKey);

		if (isEnable)
		{
			if (HelperGetStringValue(key, NULL, strKey, &sizeKey) == ERROR_SUCCESS)
			{
				// The string does not match, write Backup
				if ((winyl + assocExts[index]) != strKey)
				{
					HelperSetStringValue(key, L"Winyl_Backup", strKey);
					HelperSetStringValue(key, NULL, (winyl + assocExts[index]).c_str()); // Write current state

					::RegCloseKey(key);
					return true;
				}
			}
			else
			{
				HelperSetStringValue(key, L"Winyl_Backup", L"");
				HelperSetStringValue(key, NULL, (winyl + assocExts[index]).c_str()); // Write current state

				::RegCloseKey(key);
				return true;
			}
		}
		else
		{
			if (HelperGetStringValue(key, L"Winyl_Backup", strKey, &sizeKey) == ERROR_SUCCESS)
			{
				HelperSetStringValue(key, NULL, strKey); // Restore Backup
				::RegDeleteValue(key, L"Winyl_Backup"); // Delete Backup

				::RegCloseKey(key);
				return true;
			}
		}

		::RegCloseKey(key);
	}

	return false;
}

LONG Associations::HelperGetStringValue(HKEY hKey, LPCWSTR pszValueName, LPWSTR pszValue, LPDWORD ptrSize)
{
	DWORD type = 0;
	LONG result = ::RegQueryValueEx(hKey, pszValueName, 0, &type, (LPBYTE)pszValue, ptrSize);

	if (result != ERROR_SUCCESS)
		return result;
	if (type != REG_SZ)
		return ERROR_INVALID_DATA;
	if (pszValue != NULL) // Check the string for valid data
	{
		if(*ptrSize != 0)
		{
			if ((*ptrSize % sizeof(WCHAR) != 0) || (pszValue[*ptrSize / sizeof(WCHAR) - 1] != 0))
 				return ERROR_INVALID_DATA;
        }
	}
	else
		pszValue[0] = '\0';

	return result;
}

LONG Associations::HelperSetStringValue(HKEY hKey, LPCWSTR pszValueName, LPCWSTR pszValue)
{
	return ::RegSetValueEx(hKey, pszValueName, 0, REG_SZ, (const BYTE*)pszValue, (lstrlen(pszValue)+1) * sizeof(WCHAR));
}

LONG Associations::HelperDeleteKeyRecursively(HKEY hKey, LPCWSTR pszSubKey)
{
	LONG lRet;
	HKEY hSubKey;

	lRet = ::RegOpenKeyEx(hKey, pszSubKey, 0, MAXIMUM_ALLOWED, &hSubKey);
	if (ERROR_SUCCESS == lRet)
	{
		DWORD dwIndex;
		WCHAR szSubKeyName[MAX_PATH + 1];
		DWORD cchSubKeyName = ARRAYSIZE(szSubKeyName);
		WCHAR szClass[MAX_PATH];
		DWORD cbClass = ARRAYSIZE(szClass);

		lRet = ::RegQueryInfoKey(hSubKey, szClass, &cbClass, NULL, &dwIndex, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

		if (NO_ERROR == lRet)
		{
			while (ERROR_SUCCESS == RegEnumKey(hSubKey, --dwIndex, szSubKeyName, cchSubKeyName))
			{
				HelperDeleteKeyRecursively(hSubKey, szSubKeyName);
			}
		}

		::RegCloseKey(hSubKey);

		lRet = ::RegDeleteKey(hKey, pszSubKey);
	}

	return lRet;
}

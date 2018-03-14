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
#include "FontsLoader.h"

FontsLoader::FontsLoader()
{

}

FontsLoader::~FontsLoader()
{

}

void FontsLoader::FreeSkinFonts()
{
	for (std::size_t i = 0, size = fontFiles.size(); i < size; ++i)
		::RemoveFontResourceEx(fontFiles[i].c_str(), FR_PRIVATE|FR_NOT_ENUM, NULL);
	fontFiles.clear();

	for (std::size_t i = 0, size = fontHandles.size(); i < size; ++i)
		::RemoveFontMemResourceEx(fontHandles[i]);
	fontHandles.clear();
}

bool FontsLoader::LoadSkinFonts(const std::wstring& programPath, const std::wstring& skinName, ZipFile* zipFile, bool isReload)
{
	if (isReload)
		FreeSkinFonts();

	std::wstring path;
	if (zipFile == nullptr)
	{
		path = programPath;
		path += L"Skin";
		path.push_back('\\');
		path += skinName;
		path.push_back('\\');
	}
	
	std::wstring file = path + L"Fonts.xml";

	XmlFile xmlFile;

	if (xmlFile.LoadEx(file, zipFile))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Fonts");

		if (xmlMain)
		{
			for (XmlNode xmlNode = xmlMain.FirstChild(); xmlNode; xmlNode = xmlNode.NextChild())
			{
				std::wstring attr = xmlNode.Attribute16("File");
				if (!attr.empty())
				{
					if (zipFile == nullptr)
					{
						std::wstring fontFile = path + attr;
						int numberFonts = AddFontResourceEx(fontFile.c_str(), FR_PRIVATE|FR_NOT_ENUM, NULL);
						if (numberFonts > 0)
							fontFiles.push_back(fontFile);
					}
					else
					{
						zipFile->UnzipToBuffer(attr);
						DWORD numberFonts = 0;
						HANDLE fontHandle = AddFontMemResourceEx(zipFile->GetBuffer(), zipFile->GetBufferSize(), 0, &numberFonts);						
						zipFile->FreeBuffer();
						if (fontHandle)
							fontHandles.push_back(fontHandle);
					}
				}
			}
		}
	}
	else
		return false;

	return true;
}

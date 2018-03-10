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
#include "Language.h"

Language::Language()
{

}

Language::~Language()
{

}

bool Language::LoadLanguage(const std::wstring& language)
{
	std::wstring file = programPath;
	file += L"Language";
	file.push_back('\\');
	file += language;
	file.push_back('\\');
	file += L"Main.xml";

	XmlFile xmlFile;

	if (xmlFile.LoadFile(file))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Language");

		// Check language version
		std::string version = xmlMain.Attribute8("Version");
		if (version != "3.3.0")
			return false;

		for (int i = 0, size = (int)Lang::EnumCount; i < size; ++i)
		{
			XmlNode xmlGroup;

			switch(i)
			{
				case Lang::MainMenu:
					xmlGroup = xmlMain.FirstChild("MainMenu");
					break;
				case Lang::TrayMenu:
					xmlGroup = xmlMain.FirstChild("TrayMenu");
					break;
				case Lang::ListMenu:
					xmlGroup = xmlMain.FirstChild("ListMenu");
					break;
				case Lang::TreeMenu:
					xmlGroup = xmlMain.FirstChild("TreeMenu");
					break;
				case Lang::Library:
					xmlGroup = xmlMain.FirstChild("Library");
					break;
				case Lang::SkinDialog:
					xmlGroup = xmlMain.FirstChild("SkinDialog");
					break;
				case Lang::LibraryDialog:
					xmlGroup = xmlMain.FirstChild("LibraryDialog");
					break;
				case Lang::LanguageDialog:
					xmlGroup = xmlMain.FirstChild("LanguageDialog");
					break;
				case Lang::ProcessDialog:
					xmlGroup = xmlMain.FirstChild("ProcessDialog");
					break;
				case Lang::PropertiesDialog:
					xmlGroup = xmlMain.FirstChild("PropertiesDialog");
					break;
				case Lang::RenameDialog:
					xmlGroup = xmlMain.FirstChild("RenameDialog");
					break;
				case Lang::AboutDialog:
					xmlGroup = xmlMain.FirstChild("AboutDialog");
					break;
				case Lang::Message:
					xmlGroup = xmlMain.FirstChild("Messages");
					break;
				case Lang::DonateDialog:
					xmlGroup = xmlMain.FirstChild("DonateDialog");
					break;
				case Lang::EqualizerDialog:
					xmlGroup = xmlMain.FirstChild("EqualizerDialog");
					break;
				case Lang::Playlist:
					xmlGroup = xmlMain.FirstChild("Playlist");
					break;
				case Lang::HotkeyDialog:
					xmlGroup = xmlMain.FirstChild("HotKeyDialog");
					break;
				case Lang::OpenURLDialog:
					xmlGroup = xmlMain.FirstChild("OpenURLDialog");
					break;
				case Lang::PopupMenu:
					xmlGroup = xmlMain.FirstChild("PopupMenu");
					break;
				case Lang::Tooltips:
					xmlGroup = xmlMain.FirstChild("ToolTips");
					break;
				case Lang::ConfigDialog:
					xmlGroup = xmlMain.FirstChild("ConfigDialog");
					break;
				case Lang::GeneralPage:
					xmlGroup = xmlMain.FirstChild("GeneralPage");
					break;
				case Lang::SystemPage:
					xmlGroup = xmlMain.FirstChild("SystemPage");
					break;
				case Lang::PopupPage:
					xmlGroup = xmlMain.FirstChild("PopupPage");
					break;
				case Lang::Search:
					xmlGroup = xmlMain.FirstChild("Search");
					break;
				case Lang::MiniPage:
					xmlGroup = xmlMain.FirstChild("MiniPlayerPage");
					break;
				case Lang::DefaultList:
					xmlGroup = xmlMain.FirstChild("DefaultList");
					break;
				case Lang::DialogSmart:
					xmlGroup = xmlMain.FirstChild("DlgSmartlist");
					break;
				case Lang::SmartFilter:
					xmlGroup = xmlMain.FirstChild("DlgSmartlistFilter");
					break;
				case Lang::StatusLine:
					xmlGroup = xmlMain.FirstChild("StatusLine");
					break;
				case Lang::TagsPage:
					xmlGroup = xmlMain.FirstChild("TagsPage");
					break;
				case Lang::CoverPage:
					xmlGroup = xmlMain.FirstChild("CoverPage");
					break;
				case Lang::LyricsPage:
					xmlGroup = xmlMain.FirstChild("LyricsPage");
					break;
				case Lang::Lyrics:
					xmlGroup = xmlMain.FirstChild("Lyrics");
					break;
				case Lang::LyricsMenu:
					xmlGroup = xmlMain.FirstChild("LyricsMenu");
					break;
				case Lang::NewVersionDialog:
					xmlGroup = xmlMain.FirstChild("NewVersionDialog");
					break;
			}

			if (xmlGroup)
			{
				for (XmlNode xmlNode = xmlGroup.FirstChild(); xmlNode; xmlNode = xmlNode.NextChild())
				{
					lines[i].push_back(xmlNode.Attribute16("Line"));
				}
			}
		}
	}
	else
		return false;

	currentLanguage = language;
	return true;
}

const wchar_t* Language::GetLine(Lang type, int index)
{
	return lines[(std::size_t)type][index].c_str();
}

const std::wstring& Language::GetLineS(Lang type, int index)
{
	return lines[(std::size_t)type][index];
}

bool Language::ReloadLanguage(const std::wstring& language)
{
	for (int i = 0, size = (int)Lang::EnumCount; i < size; ++i)
	{
		lines[i].clear();
		lines[i].shrink_to_fit();
	}

	return LoadLanguage(language);
}

std::wstring Language::GetSystemLanguage()
{
	WORD id = PRIMARYLANGID(GetUserDefaultUILanguage());

	// http://msdn.microsoft.com/en-us/library/windows/desktop/dd318693(v=vs.85).aspx

	switch (id)
	{
//	case LANG_BELARUSIAN:
//		return L"Belarusian";
	case LANG_CHINESE: // LANG_CHINESE_SIMPLIFIED
		return L"Chinese (Simple)";
	case LANG_CHINESE_TRADITIONAL:
		return L"Chinese (Traditional)";
//	case LANG_CROATIAN: // LANG_BOSNIAN LANG_SERBIAN
//		return L"Serbian";
	case LANG_CZECH:
		return L"Czech";
	case LANG_DUTCH:
		return L"Dutch";
	case LANG_ENGLISH:
		return L"English";
	case LANG_FRENCH:
		return L"French";
	case LANG_GERMAN:
		return L"German";
	case LANG_GREEK:
		return L"Greek";
	case LANG_HUNGARIAN:
		return L"Hungarian";
	case LANG_INDONESIAN:
		return L"Indonesian";
	case LANG_ITALIAN:
		return L"Italian";
	case LANG_JAPANESE:
		return L"Japanese";
	case LANG_KOREAN:
		return L"Korean";
	case LANG_LATVIAN:
		return L"Latvian";
//	case LANG_LITHUANIAN:
//		return L"Lithuanian";
//	case LANG_NORWEGIAN:
//		return L"Norwegian";
	case LANG_POLISH:
		return L"Polish";
	case LANG_PORTUGUESE:
		return L"Portuguese (Brazil)";
	case LANG_ROMANIAN:
		return L"Romanian";
	case LANG_RUSSIAN:
		return L"Russian";
	case LANG_SLOVAK:
		return L"Slovak";
	case LANG_SLOVENIAN:
		return L"Slovenian";
	case LANG_SPANISH:
		return L"Spanish";
	case LANG_SWEDISH:
		return L"Swedish";
	case LANG_TURKISH:
		return L"Turkish";
	case LANG_UKRAINIAN:
		return L"Ukrainian";
	case LANG_VIETNAMESE:
		return L"Vietnamese";
	}

	return L"English";
}


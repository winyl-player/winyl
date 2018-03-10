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
#include "Radio.h"
#include "RadioList.h"

Radio::Radio()
{

}

Radio::~Radio()
{

}

bool Radio::LoadTree(SkinTree* skinTree, TreeNodeUnsafe node)
{
	// http://www.181.fm - http://www.181.fm/index.php?p=mp3links
	// http://somafm.com
	// http://977music.com/index.php?p=custom_page&page_name=SHOUTcast
	// http://www.di-radio.com
	// http://www.radioparadise.com - http://www.radioparadise.com/rp_2.php?#name=Listen&file=links
	// http://megarockradio.net - http://www.megarockradio.net/player - http://megarockradio.net/asx/radio.pls



	skinTree->SetControlRedraw(false);
	
	skinTree->InsertRadio(node, L"181.FM", L"181.FM");
	skinTree->InsertRadio(node, L"SomaFM.com", L"SomaFM.com");
	skinTree->InsertRadio(node, L"997music.com", L"997music.com");
	skinTree->InsertRadio(node, L"DI-Radio.com", L"DI-Radio.com");
	skinTree->InsertRadio(node, L"MegarockRadio.net", L"MegarockRadio.net");
	skinTree->InsertRadio(node, L"RadioParadise.com", L"RadioParadise.com");

	skinTree->SetControlRedraw(true);

	return true;
}

bool Radio::LoadList(SkinList* skinList, const std::wstring& genre)
{
	int count = 0;

	if (genre == L"181.FM")
	{
		skinList->SetControlRedraw(false);
		skinList->DeleteAllNode();
		skinList->EnableRadio(true);
		skinList->SetViewPlaylist(true);

		std::size_t size = std::size(radio_181);
		assert(size % 3 == 0);
		for (std::size_t i = 0; i < size; i += 3)
		{
			count++;
			ListNodeUnsafe node = skinList->InsertTrack(nullptr, radio_181[i + 2], 0, 0, 0, 0, 0, 0);

			skinList->SetNodeString(node, SkinListElement::Type::Artist, std::wstring(L"181.FM: ") + radio_181[i]);
			skinList->SetNodeString(node, SkinListElement::Type::Title, radio_181[i + 1]);
			skinList->SetNodeString(node, SkinListElement::Type::Time, L"8:88");
		}

		skinList->ResetIndex();
		skinList->SetControlRedraw(true);
	}
	else if (genre == L"997music.com")
	{
		skinList->SetControlRedraw(false);
		skinList->DeleteAllNode();
		skinList->EnableRadio(true);
		skinList->SetViewPlaylist(true);

		std::size_t size = std::size(radio_977);
		assert(size % 2 == 0);
		for (std::size_t i = 0; i < size; i += 2)
		{
			count++;
			ListNodeUnsafe node = skinList->InsertTrack(nullptr, radio_977[i + 1], 0, 0, 0, 0, 0, 0);

			skinList->SetNodeString(node, SkinListElement::Type::Artist, L"997music.com");
			skinList->SetNodeString(node, SkinListElement::Type::Title, radio_977[i]);
			skinList->SetNodeString(node, SkinListElement::Type::Time, L"8:88");
		}

		skinList->ResetIndex();
		skinList->SetControlRedraw(true);
	}
	else if (genre == L"SomaFM.com")
	{
		skinList->SetControlRedraw(false);
		skinList->DeleteAllNode();
		skinList->EnableRadio(true);
		skinList->SetViewPlaylist(true);

		std::size_t size = std::size(radio_Soma);
		assert(size % 2 == 0);
		for (std::size_t i = 0; i < size; i += 2)
		{
			count++;
			ListNodeUnsafe node = skinList->InsertTrack(nullptr, radio_Soma[i + 1], 0, 0, 0, 0, 0, 0);

			skinList->SetNodeString(node, SkinListElement::Type::Artist, L"SomaFM.com");
			skinList->SetNodeString(node, SkinListElement::Type::Title, radio_Soma[i]);
			skinList->SetNodeString(node, SkinListElement::Type::Time, L"8:88");
		}

		skinList->ResetIndex();
		skinList->SetControlRedraw(true);
	}
	else if (genre == L"DI-Radio.com")
	{
		skinList->SetControlRedraw(false);
		skinList->DeleteAllNode();
		skinList->EnableRadio(true);
		skinList->SetViewPlaylist(true);

		std::size_t size = std::size(radio_DI);
		assert(size % 2 == 0);
		for (std::size_t i = 0; i < size; i += 2)
		{
			//count++;
			ListNodeUnsafe node = skinList->InsertTrack(nullptr, radio_DI[i + 1], 0, 0, 0, 0, 0, 0);

			skinList->SetNodeString(node, SkinListElement::Type::Artist, L"DI-Radio.com");
			skinList->SetNodeString(node, SkinListElement::Type::Title, radio_DI[i]);
			skinList->SetNodeString(node, SkinListElement::Type::Time, L"8:88");
		}

		skinList->ResetIndex();
		skinList->SetControlRedraw(true);
	}
	else if (genre == L"RadioParadise.com")
	{
		skinList->SetControlRedraw(false);
		skinList->DeleteAllNode();
		skinList->EnableRadio(true);
		skinList->SetViewPlaylist(true);

		ListNodeUnsafe node = skinList->InsertTrack(nullptr, L"https://www.radioparadise.com/m3u/aac-320.m3u", 0, 0, 0, 0, 0, 0);

		skinList->SetNodeString(node, SkinListElement::Type::Artist, L"RadioParadise.com");
		skinList->SetNodeString(node, SkinListElement::Type::Title, L"Radio Paradise (320k AAC)");
		skinList->SetNodeString(node, SkinListElement::Type::Time, L"8:88");

		node = skinList->InsertTrack(nullptr, L"http://www.radioparadise.com/m3u/aac-128.m3u", 0, 0, 0, 0, 0, 0);

		skinList->SetNodeString(node, SkinListElement::Type::Artist, L"RadioParadise.com");
		skinList->SetNodeString(node, SkinListElement::Type::Title, L"Radio Paradise (128k AAC)");
		skinList->SetNodeString(node, SkinListElement::Type::Time, L"8:88");

		node = skinList->InsertTrack(nullptr, L"http://www.radioparadise.com/m3u/mp3-192.m3u", 0, 0, 0, 0, 0, 0);

		skinList->SetNodeString(node, SkinListElement::Type::Artist, L"RadioParadise.com");
		skinList->SetNodeString(node, SkinListElement::Type::Title, L"Radio Paradise (192k MP3)");
		skinList->SetNodeString(node, SkinListElement::Type::Time, L"8:88");

		node = skinList->InsertTrack(nullptr, L"http://stream-dc1.radioparadise.com/rp_192m.ogg", 0, 0, 0, 0, 0, 0);

		skinList->SetNodeString(node, SkinListElement::Type::Artist, L"RadioParadise.com");
		skinList->SetNodeString(node, SkinListElement::Type::Title, L"Radio Paradise (192k OGG)");
		skinList->SetNodeString(node, SkinListElement::Type::Time, L"8:88");

		skinList->ResetIndex();
		skinList->SetControlRedraw(true);
	}
	else if (genre == L"MegarockRadio.net")
	{
		skinList->SetControlRedraw(false);
		skinList->DeleteAllNode();
		skinList->EnableRadio(true);
		skinList->SetViewPlaylist(true);

		ListNodeUnsafe node = skinList->InsertTrack(nullptr, L"http://megarockradio.net/asx/radio.pls", 0, 0, 0, 0, 0, 0);

		skinList->SetNodeString(node, SkinListElement::Type::Artist, L"MegarockRadio.net");
		skinList->SetNodeString(node, SkinListElement::Type::Title, L"Megarock Radio (320k MP3)");
		skinList->SetNodeString(node, SkinListElement::Type::Time, L"8:88");

		skinList->ResetIndex();
		skinList->SetControlRedraw(true);
	}

	return true;
}





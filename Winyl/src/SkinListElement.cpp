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
#include "SkinListElement.h"
#include "SkinListNode.h"

SkinListElement::SkinListElement()
{
	defElm.font = NULL;
}

SkinListElement::~SkinListElement()
{
	if (defElm.font)
		::DeleteObject(defElm.font);

	for (std::size_t i = 0, size = listElm.size(); i < size; ++i)
	{
		if (listElm[i]->font)
			::DeleteObject(listElm[i]->font);
		delete listElm[i];
	}
}

bool SkinListElement::LoadSkin(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile)
{
	// Load type
	type = GetType(xmlNode);

	if (type == Type::None)
		return false;

	// Load default element
	if ((int)type >= 0) // Font
		LoadFont(xmlNode, path, defElm);
	else if (type == Type::Rating)
		LoadRating(xmlNode, path, zipFile, defElm);
	else if (type == Type::Cover)
		LoadCover(xmlNode, path, zipFile, defElm);
	else if (type == Type::ArtistAlbum || type == Type::ArtistTitle)
		LoadFont(xmlNode, path, defElm);

	// Load default position
	LoadPosition(xmlNode, defPos);

	// Load states
	for (XmlNode xmlState = xmlNode.FirstChild("State"); xmlState; xmlState = xmlState.NextChild("State"))
	{
		LoadState(xmlNode, path, zipFile, xmlState);
	}

	if (!isStateLibrary && !isStatePlaylist)
	{
		isStateLibrary = true;
		isStatePlaylist = true;
	}

	return true;
}

void SkinListElement::LoadState(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile, XmlNode& xmlState)
{
	// Add new state
	listState.push_back(structState());
	structState& state2 = listState.back();

	listPos.push_back(structPos());
	structPos& pos2 = listPos.back();

	listElm.push_back(new structElm);
	structElm& elm2 = *listElm.back();
	
	elm2.font = NULL;

	// Reset states to find out what loaded
	state2.stateSelect = 0;
	state2.statePlay = 0;
	state2.stateLine = 0;
	state2.stateLibrary = 0;

	int state = 0;

	if (xmlState.Attribute("Normal", &state))
	{
		if (state == 1) state2.stateSelect |= SkinListNode::StateFlag::Normal;
	}
	if (xmlState.Attribute("Select", &state))
	{
		if (state == 1) state2.stateSelect |= SkinListNode::StateFlag::Select;
	}
	if (xmlState.Attribute("Focus", &state))
	{
		if (state == 1) state2.stateSelect |= SkinListNode::StateFlag::Focus;
	}
	// Select state is not loaded, use for all
	if (state2.stateSelect == 0)
		state2.stateSelect = SkinListNode::StateFlag::Normal | SkinListNode::StateFlag::Select | SkinListNode::StateFlag::Focus;

	if (xmlState.Attribute("Stop", &state))
	{
		if (state == 1) state2.statePlay |= SkinListNode::StateFlag::Stop;
	}
	if (xmlState.Attribute("Play", &state))
	{
		if (state == 1) state2.statePlay |= SkinListNode::StateFlag::Play;
	}
	if (xmlState.Attribute("Pause", &state))
	{
		if (state == 1) state2.statePlay |= SkinListNode::StateFlag::Pause;
	}
	// Play state is not loaded, use for all
	if (state2.statePlay == 0)
		state2.statePlay = SkinListNode::StateFlag::Stop | SkinListNode::StateFlag::Play | SkinListNode::StateFlag::Pause;

	if (xmlState.Attribute("Line1", &state))
	{
		if (state == 1) state2.stateLine |= SkinListNode::StateFlag::Line1;
	}
	if (xmlState.Attribute("Line2", &state))
	{
		if (state == 1) state2.stateLine |= SkinListNode::StateFlag::Line2;
	}
	// Line state is not loaded, use for all
	if (state2.stateLine == 0)
		state2.stateLine = SkinListNode::StateFlag::Line1 | SkinListNode::StateFlag::Line2;


	if (xmlState.Attribute("Library", &state))
	{
		if (state == 1) {state2.stateLibrary |= SkinListNode::StateFlag::Library; isStateLibrary = true;}
	}
	if (xmlState.Attribute("Playlist", &state))
	{
		if (state == 1) {state2.stateLibrary |= SkinListNode::StateFlag::Playlist; isStatePlaylist = true;}
	}

	if (state2.stateLibrary == 0)
		state2.stateLibrary = SkinListNode::StateFlag::Library | SkinListNode::StateFlag::Playlist;


	// Load element for the state, if empty load default
	if ((int)type >= 0) // Font
	{
		if (!LoadFont(xmlState, path, elm2))
			LoadFont(xmlNode, path, elm2); // elm2 = defElm
	}
	else if (type == Type::Rating)
	{
		if (!LoadRating(xmlState, path, zipFile, elm2))
			LoadRating(xmlNode, path, zipFile, elm2); // elm2 = defElm
	}
	else if (type == Type::Cover)
	{
		if (!LoadCover(xmlState, path, zipFile, elm2))
			LoadCover(xmlNode, path, zipFile, elm2); // elm2 = defElm
	}
	else if (type == Type::ArtistAlbum || type == Type::ArtistTitle)
	{
		if (!LoadFont(xmlState, path, elm2))
			LoadFont(xmlNode, path, elm2); // elm2 = defElm
	}

	// Load position for the state, if empty load default
	if (!LoadPosition(xmlState, pos2))
		LoadPosition(xmlNode, pos2); // pos2 = defPos
}

SkinListElement::Type SkinListElement::GetType(XmlNode& xmlNode)
{
	if (strcmp(xmlNode.Name(), "Artist") == 0)
		return Type::Artist;
	if (strcmp(xmlNode.Name(), "Album") == 0)
		return Type::Album;
	if (strcmp(xmlNode.Name(), "Year") == 0)
		return Type::Year;
	if (strcmp(xmlNode.Name(), "Title") == 0)
		return Type::Title;
	if (strcmp(xmlNode.Name(), "Number") == 0)
		return Type::Track;
	if (strcmp(xmlNode.Name(), "Time") == 0)
		return Type::Time;
	if (strcmp(xmlNode.Name(), "Genre") == 0)
		return Type::Genre;
	if (strcmp(xmlNode.Name(), "Index") == 0)
		return Type::Index;
	if (strcmp(xmlNode.Name(), "ArtistTitle") == 0)
		return Type::ArtistTitle;
	if (strcmp(xmlNode.Name(), "Rating") == 0)
		return Type::Rating;
	if (strcmp(xmlNode.Name(), "Cover") == 0)
		return Type::Cover;
	if (strcmp(xmlNode.Name(), "ArtistAlbum") == 0)
		return Type::ArtistAlbum;

	return Type::None;
}

bool SkinListElement::LoadPosition(XmlNode& xmlNode, structPos& pos)
{
	// Default
	pos.left   = 0;
	pos.right  = 0;
	pos.top    = 0;   
	pos.bottom = 0;
	pos.width  = 0;
	// pos.height = 0;
	pos.isLeft   = true; 
	pos.isTop    = true;
	pos.isWidth  = true; 
	// pos.height = true;

	XmlNode xmlPosition = xmlNode.FirstChild("Position");

	if (xmlPosition)
	{
		if (xmlPosition.Attribute("Left", &pos.left))
		{
			pos.isLeft = true;

			if (xmlPosition.Attribute("Width", &pos.width))
				pos.isWidth = true;
			else if (xmlPosition.Attribute("Right", &pos.right))
				pos.isWidth = false;
		}
		else if (xmlPosition.Attribute("Right", &pos.right))
		{
			pos.isLeft = false;

			if (xmlPosition.Attribute("Width", &pos.width))
				pos.isWidth = true;
		}
		else // Default
		{
			pos.isLeft = true;

			if (xmlPosition.Attribute("Width", &pos.width))
				pos.isWidth = true;
		}

		if (xmlPosition.Attribute("Top", &pos.top))
			pos.isTop = true;
		else if (xmlPosition.Attribute("Bottom", &pos.bottom))
			pos.isTop = false;
		else // Default
			pos.isTop = true;

		pos.isLoad = true;
	}
	else
		pos.isLoad = false;

	return pos.isLoad;
}

bool SkinListElement::LoadFont(XmlNode& xmlNode, std::wstring& path, structElm& elm)
{
	XmlNode xmlFont = xmlNode.FirstChild("Font");
	
	if (xmlFont)
	{
		const char* fname = nullptr; int fsize = 0; int fbold = 0; int fitalic = 0; int fclear = 0;

		fname = xmlFont.AttributeRaw("Name");
		xmlFont.Attribute("Size", &fsize);
		xmlFont.Attribute("Bold", &fbold);
		xmlFont.Attribute("Italic", &fitalic);
		xmlFont.Attribute("ClearType", &fclear);

		elm.font = ExImage::CreateElementFont(fname, fsize, fbold, fitalic, fclear);

		elm.align = 0; // Default
		xmlFont.Attribute("Align", &elm.align);

		const char* strColor = xmlFont.AttributeRaw("Color");

		elm.color = RGB(0, 0, 0); // Default

		if (strColor)
		{
			DWORD c = strtoul(strColor, 0, 16);

			if (strlen(strColor) == 6)
				elm.color = RGB(GetBValue(c), GetGValue(c), GetRValue(c));
		}

		const char* strColor2 = xmlFont.AttributeRaw("Color2");

		elm.color2 = elm.color; // Default

		if (strColor2)
		{
			DWORD c = strtoul(strColor2, 0, 16);

			if (strlen(strColor2) == 6)
				elm.color2 = RGB(GetBValue(c), GetGValue(c), GetRValue(c));
		}

		elm.isLoad = true;
	}
	else
		elm.isLoad = false;

	return elm.isLoad;
}

bool SkinListElement::LoadRating(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile, structElm& elm)
{
	XmlNode xmlRating = xmlNode.FirstChild("Skin");
	
	if (xmlRating)
	{
		std::wstring attr = xmlRating.Attribute16("File");
		if (!attr.empty())
		{
			std::wstring file = path + attr;

			XmlFile xmlFile;

			if (xmlFile.LoadEx(file, zipFile))
			{
				std::wstring path2 = PathEx::PathFromFile(file);

				XmlNode xmlMain = xmlFile.RootNode().FirstChild("ListRating");

				if (!xmlMain) // Old
					xmlMain = xmlFile.RootNode().FirstChild("ListRate");

				if (xmlMain)
				{
					XmlNode xmlShow = xmlMain.FirstChild("Show");
					if (xmlShow)
					{
						std::wstring file2 = xmlShow.Attribute16("File");
						if (!file2.empty())
							elm.rating[0].LoadEx(path2 + file2, zipFile);
					}

					XmlNode xmlHide = xmlMain.FirstChild("Hide");
					if (xmlHide)
					{
						std::wstring file2 = xmlHide.Attribute16("File");
						if (!file2.empty())
							elm.rating[1].LoadEx(path2 + file2, zipFile);
					}
				}
			}
		}

		elm.isLoad = true;
	}
	else
		elm.isLoad = false;

	return elm.isLoad;
}

bool SkinListElement::LoadCover(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile, structElm& elm)
{
	XmlNode xmlCover = xmlNode.FirstChild("Skin");
	
	if (xmlCover)
	{
		std::wstring attr = xmlCover.Attribute16("File");
		if (!attr.empty())
		{
			std::wstring file = path + attr;

			XmlFile xmlFile;

			if (xmlFile.LoadEx(file, zipFile))
			{
				std::wstring path2 = PathEx::PathFromFile(file);

				XmlNode xmlMain = xmlFile.RootNode().FirstChild("ListCover");

				if (xmlMain)
				{
					XmlNode xmlCover2 = xmlMain.FirstChild("Cover");
					if (xmlCover2)
					{
						std::wstring file2 = xmlCover2.Attribute16("File");
						if (!file2.empty())
							elm.cover[0].LoadEx(path2 + file2, zipFile);
					}

					XmlNode xmlOverlay = xmlMain.FirstChild("Overlay");
					if (xmlOverlay)
					{
						std::wstring file2 = xmlOverlay.Attribute16("File");
						if (!file2.empty())
							elm.cover[1].LoadEx(path2 + file2, zipFile);
					}
				}
			}
		}

		elm.isLoad = true;
	}
	else
		elm.isLoad = false;

	return elm.isLoad;
}

void SkinListElement::DrawText(HDC dc, CRect& rc, const std::wstring& text, int stateSelect, int statePlay, int stateLine, int stateLibrary)
{
	if (type == Type::None)
		return;

	if (text.empty())
		return;

	structPos* pos = &defPos;
	structElm* elm = &defElm;

	// If element or position for the state exists then use it else use default
	for (std::size_t i = 0, size = listState.size(); i < size; ++i)
	{
		// Check all states
		if ((listState[i].stateLibrary & stateLibrary) &&
			(listState[i].stateSelect & stateSelect) &&
			(listState[i].statePlay & statePlay) &&
			(listState[i].stateLine & stateLine))
		{
			pos = &listPos[i];
			elm = listElm[i];
			break;
		}
	}

	if (!pos->isLoad || !elm->isLoad)
		return;

	// Get text size
	HGDIOBJ oldFont = ::SelectObject(dc, elm->font);

	CSize szText;
	::GetTextExtentPoint32(dc, text.c_str(), (int)text.size(), &szText);

	CRect rcText;

	if (pos->isLeft) // Left location
	{
		rcText.left = rc.left + pos->left;

		if (pos->isWidth) // Fixed width
			rcText.right = rcText.left + pos->width; //szText.cx;
		else // To right
			rcText.right = rc.right - pos->right;
	}
	else // Right location
	{
		rcText.left = rc.right - szText.cx - pos->right;
		rcText.right = rcText.left + szText.cx;
	}

	if (pos->isTop) // Top location
	{
		rcText.top = rc.top + pos->top;
		rcText.bottom = rcText.top + szText.cy;
	}
	else // Bottom location
	{
		rcText.top = rc.bottom - szText.cy - pos->bottom;
		rcText.bottom = rcText.top + szText.cy;
	}

	// Draw

	::SetTextColor(dc, elm->color);

//	TEXTMETRIC tm;
//	::GetTextMetricsW(dc, &tm);
//	rcText.top -= tm.tmExternalLeading;
//	szText.cx += tm.tmOverhang;
//	szText.cy = tm.tmHeight - tm.tmExternalLeading;

	//if (pos->right)
	//{
		if (elm->align == 0)
			::DrawText(dc, text.c_str(), (int)text.size(), rcText, DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX|DT_NOCLIP/*|DT_VCENTER*/);
		else if (elm->align == 1)
			::DrawText(dc, text.c_str(), (int)text.size(), rcText, DT_RIGHT|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX|DT_NOCLIP/*|DT_VCENTER*/);
		else // if (elm->align == 2)
			::DrawText(dc, text.c_str(), (int)text.size(), rcText, DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX|DT_NOCLIP/*|DT_VCENTER*/);
	//}
	//else
	//	::DrawText(dc, text.c_str(), (int)text.size(), rcText, DT_LEFT|DT_SINGLELINE|DT_NOPREFIX|DT_NOCLIP/*|DT_VCENTER*/);

/*	DTTOPTS opt = {};
	opt.dwSize = sizeof(opt);
	opt.dwFlags = DTT_TEXTCOLOR|DTT_COMPOSITED;
	opt.crText = elm2->color2;

	if (pos->right > 0)
	{
		if (elm->align == 1)
			futureWin->DrawThemeTextEx(pFutureWin->hTheme, dc, 0, 0, text.c_str(), -1, DT_RIGHT|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX, rcText, &opt);
		else if (elm->align == 2)
			futureWin->DrawThemeTextEx(pFutureWin->hTheme, dc, 0, 0, text.c_str(), -1, DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX, rcText, &opt);
		else
			futureWin->DrawThemeTextEx(pFutureWin->hTheme, dc, 0, 0, text.c_str(), -1, DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX, rcText, &opt);
	}
	else
		futureWin->DrawThemeTextEx(pFutureWin->hTheme, dc, 0, 0, text.c_str(), -1, DT_LEFT|DT_SINGLELINE|DT_NOPREFIX, rcText, &opt);*/

	::SelectObject(dc, oldFont);
}

void SkinListElement::DrawText2(HDC dc, CRect& rc, const std::wstring& text, const std::wstring& text2, int stateSelect, int statePlay, int stateLine, int stateLibrary)
{
	if (type == Type::None)
		return;

	if (text2.empty())
		return;

	structPos* pos = &defPos;
	structElm* elm = &defElm;

	// If element or position for the state exists then use it else use default
	for (std::size_t i = 0, size = listState.size(); i < size; ++i)
	{
		// Check all states
		if ((listState[i].stateLibrary & stateLibrary) &&
			(listState[i].stateSelect & stateSelect) &&
			(listState[i].statePlay & statePlay) &&
			(listState[i].stateLine & stateLine))
		{
			pos = &listPos[i];
			elm = listElm[i];
			break;
		}
	}

	if (!pos->isLoad || !elm->isLoad)
		return;

	// Get text size
	HGDIOBJ oldFont = ::SelectObject(dc, elm->font);

	CSize szText;
	::GetTextExtentPoint32(dc, text2.c_str(), (int)text2.size(), &szText);

	CRect rcText;

	if (pos->isLeft) // Left location
	{
		rcText.left = rc.left + pos->left;

		if (pos->isWidth) // Fixed width
			rcText.right = rcText.left + pos->width; //szText.cx;
		else // To right
			rcText.right = rc.right - pos->right;
	}
	else // Right location
	{
		rcText.left = rc.right - szText.cx - pos->right;
		rcText.right = rcText.left + szText.cx;
	}

	if (pos->isTop) // Top location
	{
		rcText.top = rc.top + pos->top;
		rcText.bottom = rcText.top + szText.cy;
	}
	else // Bottom location
	{
		rcText.top = rc.bottom - szText.cy - pos->bottom;
		rcText.bottom = rcText.top + szText.cy;
	}

	// Draw

	if (!text.empty())
	{
		CSize szText1, szDash;
		::GetTextExtentPoint32(dc, text.c_str(), (int)text.size(), &szText1);
		::GetTextExtentPoint32(dc, dash.c_str(), (int)dash.size(), &szDash);

		CRect rcText1 = CRect(rcText.left, rcText.top, rcText.left + szText1.cx, rcText.bottom);
		CRect rcDash = CRect(rcText1.right, rcText.top, rcText1.right + szDash.cx, rcText.bottom);
		CRect rcText2 = CRect(rcDash.right, rcText.top, rcText.right, rcText.bottom);

//		if (pos->right > 0)
//		{
			::SetTextColor(dc, elm->color);
			//::SetTextAlign(dc, TA_UPDATECP);

			::DrawText(dc, text.c_str(), (int)text.size(), rcText, DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX|DT_NOCLIP/*|DT_VCENTER*/);

			//if (rcDash.right < rcText.right) // If there is a place to draw the second text
			if (rcDash.right + szDash.cx < rcText.right) // If there is a place to draw the second text
			{
				::DrawText(dc, dash.c_str(), (int)dash.size(), rcDash, DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX|DT_NOCLIP/*|DT_VCENTER*/);

				::SetTextColor(dc, elm->color2);

				::DrawText(dc, text2.c_str(), (int)text2.size(), rcText2, DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX|DT_NOCLIP/*|DT_VCENTER*/);
			}
//		}
//		else
//		{
//			::SetTextColor(dc, elm->color);
//
//			::DrawText(dc, text.c_str(), (int)text.size(), rcText, DT_LEFT|DT_SINGLELINE|DT_NOPREFIX|DT_NOCLIP/*|DT_VCENTER*/);
//			::DrawText(dc, dash.c_str(), (int)dash.size(), rcDash, DT_LEFT|DT_SINGLELINE|DT_NOPREFIX|DT_NOCLIP/*|DT_VCENTER*/);
//
//			::SetTextColor(dc, elm->color2);
//
//			::DrawText(dc, text.c_str(), (int)text.size(), rcText2, DT_LEFT|DT_SINGLELINE|DT_NOPREFIX|DT_NOCLIP/*|DT_VCENTER*/);
//		}
	}
	else
	{
		::SetTextColor(dc, elm->color2);

		//if (pos->right > 0)
			::DrawText(dc, text2.c_str(), (int)text2.size(), rcText, DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX|DT_NOCLIP/*|DT_VCENTER*/);
		//else
		//	::DrawText(dc, text2.c_str(), (int)text2.size(), rcText, DT_LEFT|DT_SINGLELINE|DT_NOPREFIX|DT_NOCLIP/*|DT_VCENTER*/);
	}

	::SelectObject(dc, oldFont);
}

void SkinListElement::DrawRating(HDC dc, CRect& rc, int rating, int stateSelect, int statePlay, int stateLine, int stateLibrary)
{
	if (type == Type::None)
		return;

	structPos* pos = &defPos;
	structElm* elm = &defElm;

	// If element or position for the state exists then use it else use default
	for (std::size_t i = 0, size = listState.size(); i < size; ++i)
	{
		// Check all states
		if ((listState[i].stateLibrary & stateLibrary) &&
			(listState[i].stateSelect & stateSelect) &&
			(listState[i].statePlay & statePlay) &&
			(listState[i].stateLine & stateLine))
		{
			pos = &listPos[i];
			elm = listElm[i];
			break;
		}
	}

	if (!pos->isLoad || !elm->isLoad)
		return;


	int width = elm->rating[0].Width();
	int height = elm->rating[0].Height();
	int width5 = width * 5;

	CRect rcDraw;

	if (pos->isLeft) // Left location
	{
		rcDraw.left = rc.left + pos->left;
		rcDraw.right = rcDraw.left + width5;
	}
	else // Right location
	{
		rcDraw.left = rc.right - width5 - pos->right;
		rcDraw.right = rcDraw.left + width5;
	}

	if (pos->isTop) // Top location
	{
		rcDraw.top = rc.top + pos->top;
		rcDraw.bottom = rcDraw.top + height;
	}
	else // Bottom location
	{
		rcDraw.top = rc.bottom - height - pos->bottom;
		rcDraw.bottom = rcDraw.top + height;
	}

	// Draw
	for (int i = 0; i < 5; ++i)
	{
		if (rating > i)
			elm->rating[0].Draw(dc, rcDraw.left + width * i, rcDraw.top);
		else
			elm->rating[1].Draw(dc, rcDraw.left + width * i, rcDraw.top);
	}
}

int SkinListElement::GetClickRating(CPoint& point, CRect& rc, int stateSelect, int statePlay, int stateLine, int stateLibrary)
{
	if (type == Type::None)
		return 0;

	structPos* pos = &defPos;
	structElm* elm = &defElm;

	// If element or position for the state exists then use it else use default
	for (std::size_t i = 0, size = listState.size(); i < size; ++i)
	{
		// Check all states
		if ((listState[i].stateLibrary & stateLibrary) &&
			(listState[i].stateSelect & stateSelect) &&
			(listState[i].statePlay & statePlay) &&
			(listState[i].stateLine & stateLine))
		{
			pos = &listPos[i];
			elm = listElm[i];
			break;
		}
	}

	if (!pos->isLoad || !elm->isLoad)
		return 0;


	int width = elm->rating[0].Width();
	int height = elm->rating[0].Height();
	int width5 = width * 5;

	CRect rcDraw;

	if (pos->isLeft) // Left location
	{
		rcDraw.left = rc.left + pos->left;
		rcDraw.right = rcDraw.left + width5;
	}
	else // Right location
	{
		rcDraw.left = rc.right - width5 - pos->right;
		rcDraw.right = rcDraw.left + width5;
	}

	if (pos->isTop) // Top location
	{
		rcDraw.top = rc.top + pos->top;
		rcDraw.bottom = rcDraw.top + height;
	}
	else // Bottom location
	{
		rcDraw.top = rc.bottom - height - pos->bottom;
		rcDraw.bottom = rcDraw.top + height;
	}

	// Check which star is clicked
	if (rcDraw.PtInRect(point))
	{
		int star = rcDraw.Width() / 5;
		int pos = point.x - rcDraw.left;
		for (int i = 1; i <= 5; ++i)
		{
			if (pos <= star * i)
			{
				return i;
			}
		}
	}

	return 0;
}

void SkinListElement::DrawCover(HDC dc, CRect& rc, ExImage* image, int stateSelect, int statePlay, int stateLine, int stateLibrary, CSize& szCover)
{
	if (type == Type::None)
		return;

	structPos* pos = &defPos;
	structElm* elm = &defElm;

	// If element or position for the state exists then use it else use default
	for (std::size_t i = 0, size = listState.size(); i < size; ++i)
	{
		// Check all states
		if ((listState[i].stateLibrary & stateLibrary) &&
			(listState[i].stateSelect & stateSelect) &&
			(listState[i].statePlay & statePlay) &&
			(listState[i].stateLine & stateLine))
		{
			pos = &listPos[i];
			elm = listElm[i];
			break;
		}
	}

	if (!pos->isLoad || !elm->isLoad)
		return;

	int width = elm->cover[0].Width();
	int height = elm->cover[0].Height();

	CRect rcDraw;

	if (pos->isLeft) // Left location
	{
		rcDraw.left = rc.left + pos->left;
		rcDraw.right = rcDraw.left + width;
	}
	else // Right location
	{
		rcDraw.left = rc.right - width - pos->right;
		rcDraw.right = rcDraw.left + width;
	}

	if (pos->isTop) // Top location
	{
		rcDraw.top = rc.top + pos->top;
		rcDraw.bottom = rcDraw.top + height;
	}
	else // Bottom location
	{
		rcDraw.top = rc.bottom - height - pos->bottom;
		rcDraw.bottom = rcDraw.top + height;
	}

	// Draw
	if (image && image->IsValid())
	{
		image->Draw(dc, rcDraw.left, rcDraw.top);
		elm->cover[1].Draw(dc, rcDraw.left, rcDraw.top);
	}
	else
		elm->cover[0].Draw(dc, rcDraw.left, rcDraw.top);

	szCover.cx = width;
	szCover.cy = height;
}

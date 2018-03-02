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

// SkinTreeElement.cpp : implementation file
//

#include "stdafx.h"
#include "SkinTreeElement.h"
#include "SkinTreeNode.h"


// SkinTreeElement

SkinTreeElement::SkinTreeElement()
{
	defElm.font = NULL;
}

SkinTreeElement::~SkinTreeElement()
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

bool SkinTreeElement::LoadSkin(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile)
{
	// Load type
	type = GetType(xmlNode);

	if (type == Type::None)
		return false;

	// Load default element
	if ((int)type >= 0) // Font
		LoadFont(xmlNode, path, defElm);
	else if (type == Type::Expand)
		LoadExpand(xmlNode, path, zipFile, defElm);
	else if (type == Type::Icon)
		LoadIcons(xmlNode, path, zipFile, defElm);

	// Load default position
	LoadPosition(xmlNode, defPos);

	// Load states
	for (XmlNode xmlState = xmlNode.FirstChild("State"); xmlState; xmlState = xmlState.NextChild("State"))
	{
		LoadState(xmlNode, path, zipFile, xmlState);
	}

	return true;
}

void SkinTreeElement::LoadState(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile, XmlNode& xmlState)
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
	state2.stateLine = 0;

	int state = 0;

	if (xmlState.Attribute("Normal", &state))
	{
		if (state == 1) state2.stateSelect |= SkinTreeNode::StateFlag::Normal;
	}
	if (xmlState.Attribute("Select", &state))
	{
		if (state == 1) state2.stateSelect |= SkinTreeNode::StateFlag::Select;
	}
	if (xmlState.Attribute("Focus", &state))
	{
		if (state == 1) state2.stateSelect |= SkinTreeNode::StateFlag::Focus;
	}
	// Select state is not loaded, use for all
	if (state2.stateSelect == 0)
		state2.stateSelect = SkinTreeNode::StateFlag::Normal | SkinTreeNode::StateFlag::Select | SkinTreeNode::StateFlag::Focus;


	if (xmlState.Attribute("Line1", &state))
	{
		if (state == 1) state2.stateLine |= SkinTreeNode::StateFlag::Line1;
	}
	if (xmlState.Attribute("Line2", &state))
	{
		if (state == 1) state2.stateLine |= SkinTreeNode::StateFlag::Line2;
	}
	// Line state is not loaded, use for all
	if (state2.stateLine == 0)
		state2.stateLine = SkinTreeNode::StateFlag::Line1 | SkinTreeNode::StateFlag::Line2;


	// Load element for the state, if empty load default
	if ((int)type >= 0) // Font
	{
		if (!LoadFont(xmlState, path, elm2))
			LoadFont(xmlNode, path, elm2); // elm2 = defElm
	}
	else if (type == Type::Expand)
	{
		if (!LoadExpand(xmlState, path, zipFile, elm2))
			LoadExpand(xmlNode, path, zipFile, elm2); // elm2 = defElm
	}
	else if (type == Type::Icon)
	{
		if (!LoadIcons(xmlState, path, zipFile, elm2))
			LoadIcons(xmlNode, path, zipFile, elm2); // elm2 = defElm
	}

	// Load position for the state, if empty load default
	if (!LoadPosition(xmlState, pos2))
		LoadPosition(xmlNode, pos2); // pos2 = defPos
}

SkinTreeElement::Type SkinTreeElement::GetType(XmlNode& xmlNode)
{
	if (strcmp(xmlNode.Name(), "Text") == 0)
		return Type::Text;
	if (strcmp(xmlNode.Name(), "Expand") == 0)
		return Type::Expand;
	if (strcmp(xmlNode.Name(), "Icon") == 0)
		return Type::Icon;
	
	return Type::None;
}

bool SkinTreeElement::LoadPosition(XmlNode& xmlNode, structPos& pos)
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

bool SkinTreeElement::LoadFont(XmlNode& xmlNode, std::wstring& path, structElm& elm)
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

		elm.isLoad = true;
	}
	else
		elm.isLoad = false;

	return elm.isLoad;
}

bool SkinTreeElement::LoadExpand(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile, structElm& elm)
{
	XmlNode xmlExpand = xmlNode.FirstChild("Skin");

	if (xmlExpand)
	{
		std::wstring attr = xmlExpand.Attribute16("File");
		if (!attr.empty())
		{
			std::wstring file = path + attr;

			XmlFile xmlFile;

			if (xmlFile.LoadEx(file, zipFile))
			{
				std::wstring path2 = PathEx::PathFromFile(file);

				XmlNode xmlMain = xmlFile.RootNode().FirstChild("TreeExpand");

				if (xmlMain)
				{
					XmlNode xmlOpen = xmlMain.FirstChild("Open");
					if (xmlOpen)
					{
						std::wstring file2 = xmlOpen.Attribute16("File");
						if (!file2.empty())
							elm.expand[0].LoadEx(path2 + file2, zipFile);
					}

					XmlNode xmlClose = xmlMain.FirstChild("Close");
					if (xmlClose)
					{
						std::wstring file2 = xmlClose.Attribute16("File");
						if (!file2.empty())
							elm.expand[1].LoadEx(path2 + file2, zipFile);
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

bool SkinTreeElement::LoadIcons(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile, structElm& elm)
{
	XmlNode xmlIcons = xmlNode.FirstChild("Skin");

	if (xmlIcons)
	{
		std::wstring attr = xmlIcons.Attribute16("File");
		if (!attr.empty())
		{
			std::wstring file = path + attr;

			XmlFile xmlFile;

			if (xmlFile.LoadEx(file, zipFile))
			{
				std::wstring path2 = PathEx::PathFromFile(file);

				XmlNode xmlMain = xmlFile.RootNode().FirstChild("TreeIcons");

				if (xmlMain)
				{
					XmlNode xmlNowPlaying = xmlMain.FirstChild("NowPlaying");
					if (xmlNowPlaying)
					{
						std::wstring file2 = xmlNowPlaying.Attribute16("File");
						if (!file2.empty())
							elm.icon[0].LoadEx(path2 + file2, zipFile);
					}

					XmlNode xmlPlaylist = xmlMain.FirstChild("Playlist");
					if (xmlPlaylist)
					{
						std::wstring file2 = xmlPlaylist.Attribute16("File");
						if (!file2.empty())
							elm.icon[1].LoadEx(path2 + file2, zipFile);
					}

					XmlNode xmlSmartlist = xmlMain.FirstChild("Smartlist");
					if (xmlSmartlist)
					{
						std::wstring file2 = xmlSmartlist.Attribute16("File");
						if (!file2.empty())
							elm.icon[2].LoadEx(path2 + file2, zipFile);
					}

					std::wstring fileArtist;
					XmlNode xmlArtist = xmlMain.FirstChild("Artist");
					if (xmlArtist)
					{
						fileArtist = xmlArtist.Attribute16("File");
						if (!fileArtist.empty())
							elm.icon[3].LoadEx(path2 + fileArtist, zipFile);
					}

					XmlNode xmlComposer = xmlMain.FirstChild("Composer");
					if (xmlComposer)
					{
						std::wstring file2 = xmlComposer.Attribute16("File");
						if (!file2.empty())
							elm.icon[4].LoadEx(path2 + file2, zipFile);
						else
							elm.icon[4].LoadEx(path2 + fileArtist, zipFile);
					}
					else
						elm.icon[4].LoadEx(path2 + fileArtist, zipFile);

					XmlNode xmlAlbum = xmlMain.FirstChild("Album");
					if (xmlAlbum)
					{
						std::wstring file2 = xmlAlbum.Attribute16("File");
						if (!file2.empty())
							elm.icon[5].LoadEx(path2 + file2, zipFile);
					}

					XmlNode xmlGenre = xmlMain.FirstChild("Genre");
					if (xmlGenre)
					{
						std::wstring file2 = xmlGenre.Attribute16("File");
						if (!file2.empty())
							elm.icon[6].LoadEx(path2 + file2, zipFile);
					}

					XmlNode xmlYear = xmlMain.FirstChild("Year");
					if (xmlYear)
					{
						std::wstring file2 = xmlYear.Attribute16("File");
						if (!file2.empty())
							elm.icon[7].LoadEx(path2 + file2, zipFile);
					}

					XmlNode xmlFolder = xmlMain.FirstChild("Folder");
					if (xmlFolder)
					{
						std::wstring file2 = xmlFolder.Attribute16("File");
						if (!file2.empty())
							elm.icon[8].LoadEx(path2 + file2, zipFile);
					}

					XmlNode xmlRadio = xmlMain.FirstChild("Radio");
					if (xmlRadio)
					{
						std::wstring file2 = xmlRadio.Attribute16("File");
						if (!file2.empty())
							elm.icon[9].LoadEx(path2 + file2, zipFile);
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

void SkinTreeElement::DrawText(HDC dc, CRect& rc, const std::wstring& text, int stateSelect, int stateLine)
{
	if (type == Type::None)
		return;

	structPos* pos = &defPos;
	structElm* elm = &defElm;

	// If element or position for the state exists then use it else use default
	for (std::size_t i = 0, size = listState.size(); i < size; ++i)
	{
		// Check all states
		if ((listState[i].stateSelect & stateSelect) &&
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

/*	TEXTMETRIC tm;
	::GetTextMetricsW(dc, &tm);
	szText.cx += tm.tmOverhang;
	szText.cy = tm.tmHeight - tm.tmExternalLeading;*/
//	szText.cx += 3;

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

//	if (sPos2->iRight > 0)
//	{
		if (elm->align == 0)
			::DrawText(dc, text.c_str(), (int)text.size(), rcText, DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX|DT_NOCLIP/*|DT_VCENTER*/);
		else if (elm->align == 1)
			::DrawText(dc, text.c_str(), (int)text.size(), rcText, DT_RIGHT|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX|DT_NOCLIP/*|DT_VCENTER*/);
		else // if (elm->align == 2)
			::DrawText(dc, text.c_str(), (int)text.size(), rcText, DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX|DT_NOCLIP/*|DT_VCENTER*/);
//	}
//	else
//		::DrawText(dc, text.c_str(), (int)text.size(), rcText, DT_LEFT|DT_SINGLELINE|DT_NOPREFIX|DT_NOCLIP/*|DT_VCENTER*/);

	::SelectObject(dc, oldFont);
}

void SkinTreeElement::DrawExpand(HDC dc, CRect& rc, bool isOpen, bool isShowOpen, int stateSelect, int stateLine)
{
	if (type == Type::None || !isShowOpen)
		return;

	structPos* pos = &defPos;
	structElm* elm = &defElm;

	// If element or position for the state exists then use it else use default
	for (std::size_t i = 0, size = listState.size(); i < size; ++i)
	{
		// Check all states
		if ((listState[i].stateSelect & stateSelect) &&
			(listState[i].stateLine & stateLine))
		{
			pos = &listPos[i];
			elm = listElm[i];
			break;
		}
	}

	if (!pos->isLoad || !elm->isLoad)
		return;


	int width = elm->expand[0].Width();
	int height = elm->expand[0].Height();


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
	if (isOpen)
		elm->expand[0].Draw(dc, rcDraw.left, rcDraw.top);
	else
		elm->expand[1].Draw(dc, rcDraw.left, rcDraw.top);

//	rcDraw.top = rc.top;
//	rcDraw.bottom = rc.bottom;

//	rcExpand = rcDraw;
}

bool SkinTreeElement::IsClickExpand(CPoint& point, CRect& rc, int stateSelect, int stateLine)
{
	if (type == Type::None)
		return false;

	structPos* pos = &defPos;
	structElm* elm = &defElm;

	// If element or position for the state exists then use it else use default
	for (std::size_t i = 0, size = listState.size(); i < size; ++i)
	{
		// Check all states
		if ((listState[i].stateSelect & stateSelect) &&
			(listState[i].stateLine & stateLine))
		{
			pos = &listPos[i];
			elm = listElm[i];
			break;
		}
	}

	if (!pos->isLoad || !elm->isLoad)
		return false;


	int width = elm->expand[0].Width();
	int height = elm->expand[0].Height();


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

	// Check if expand is clicked
	rcDraw.top = rc.top;
	rcDraw.bottom = rc.bottom;
	if (pos->isLeft)
		rcDraw.left = 0;
	else
		rcDraw.right = rc.right;

	if (rcDraw.PtInRect(point))
		return true;

	return false;
}

void SkinTreeElement::DrawIcon(HDC dc, CRect& rc, int iconType, int stateSelect, int stateLine)
{
	if (type == Type::None)
		return;

	structPos* pos = &defPos;
	structElm* elm = &defElm;

	// If element or position for the state exists then use it else use default
	for (std::size_t i = 0, size = listState.size(); i < size; ++i)
	{
		// Check all states
		if ((listState[i].stateSelect & stateSelect) &&
			(listState[i].stateLine & stateLine))
		{
			pos = &listPos[i];
			elm = listElm[i];
			break;
		}
	}

	if (!pos->isLoad || !elm->isLoad)
		return;


	int width = elm->icon[0].Width();
	int height = elm->icon[0].Height();


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
	switch (iconType)
	{
	case SkinTreeNode::Type::NowPlaying:
		elm->icon[0].Draw(dc, rcDraw.left, rcDraw.top);
		break;
	case SkinTreeNode::Type::Playlist:
		elm->icon[1].Draw(dc, rcDraw.left, rcDraw.top);
		break;
	case SkinTreeNode::Type::Smartlist:
		elm->icon[2].Draw(dc, rcDraw.left, rcDraw.top);
		break;
	case SkinTreeNode::Type::Artist:
		elm->icon[3].Draw(dc, rcDraw.left, rcDraw.top);
		break;
	case SkinTreeNode::Type::Composer:
		elm->icon[4].Draw(dc, rcDraw.left, rcDraw.top);
		break;
	case SkinTreeNode::Type::Album:
		elm->icon[5].Draw(dc, rcDraw.left, rcDraw.top);
		break;
	case SkinTreeNode::Type::Genre:
		elm->icon[6].Draw(dc, rcDraw.left, rcDraw.top);
		break;
	case SkinTreeNode::Type::Year:
		elm->icon[7].Draw(dc, rcDraw.left, rcDraw.top);
		break;
	case SkinTreeNode::Type::Folder:
		elm->icon[8].Draw(dc, rcDraw.left, rcDraw.top);
		break;
	case SkinTreeNode::Type::Radio:
		elm->icon[9].Draw(dc, rcDraw.left, rcDraw.top);
		break;
	}
}

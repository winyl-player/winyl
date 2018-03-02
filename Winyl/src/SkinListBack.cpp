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

// SkinListBack.cpp : implementation file
//

#include "stdafx.h"
#include "SkinListBack.h"
#include "SkinListNode.h"


// SkinListBack

SkinListBack::SkinListBack()
{

}

SkinListBack::~SkinListBack()
{
	for (std::size_t i = 0, size = listElm.size(); i < size; ++i)
		delete listElm[i];
}

bool SkinListBack::LoadSkin(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile)
{
	// Load type
	type = GetType(xmlNode);

	if (type == Type::None)
		return false;

	// Load default element
	if (type == Type::Image)
		LoadImage(xmlNode, path, zipFile, defElm);
	else if (type == Type::Line)
		LoadLine(xmlNode, defElm);
	else if (type == Type::Fill)
		LoadFill(xmlNode, defElm);

	// Load default position
	LoadPosition(xmlNode, defPos, defElm);

	// Load states
	for (XmlNode xmlState = xmlNode.FirstChild("State"); xmlState; xmlState = xmlState.NextChild("State"))
	{
		LoadState(xmlNode, path, zipFile, xmlState);
	}

	return true;
}

void SkinListBack::LoadState(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile, XmlNode& xmlState)
{
	// Add new state
	listState.push_back(structState());
	structState& state2 = listState.back();

	listPos.push_back(structPos());
	structPos& pos2 = listPos.back();

	listElm.push_back(new structElm);
	structElm& elm2 = *listElm.back();

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
		if (state == 1) state2.stateLibrary |= SkinListNode::StateFlag::Library;
	}
	if (xmlState.Attribute("Playlist", &state))
	{
		if (state == 1) state2.stateLibrary |= SkinListNode::StateFlag::Playlist;
	}

	if (state2.stateLibrary == 0)
		state2.stateLibrary = SkinListNode::StateFlag::Library | SkinListNode::StateFlag::Playlist;


	// Load element for the state, if empty load default
	if (type == Type::Image)
	{
		if (!LoadImage(xmlState, path, zipFile, elm2))
			LoadImage(xmlNode, path, zipFile, elm2); // elm2 = defElm
	}
	else if (type == Type::Line)
	{
		if (!LoadLine(xmlState, elm2))
			LoadLine(xmlNode, elm2); // elm2 = defElm
	}
	else if (type == Type::Fill)
	{
		if (!LoadFill(xmlState, elm2))
			LoadFill(xmlNode, elm2); // elm2 = defElm
	}

	// Load position for the state, if empty load default
	if (!LoadPosition(xmlState, pos2, elm2))
		LoadPosition(xmlNode, pos2, elm2); // pos2 = defPos
}

SkinListBack::Type SkinListBack::GetType(XmlNode& xmlNode)
{
	if (strcmp(xmlNode.Name(), "Image") == 0)
		return Type::Image;
	if (strcmp(xmlNode.Name(), "Line") == 0)
		return Type::Line;
	if (strcmp(xmlNode.Name(), "Fill") == 0)
		return Type::Fill;
	
	return Type::None;
}

bool SkinListBack::LoadPosition(XmlNode& xmlNode, structPos& pos, structElm& elm)
{
	// Default
	pos.left   = 0;
	pos.right  = 0;
	pos.top    = 0;
	pos.bottom = 0;
	if (type == Type::Image)
	{
		pos.width = elm.image.Width();
		pos.height = elm.image.Height();
	}
	else if (type == Type::Line || type == Type::Fill)
	{
		pos.width = DefaultFill::Width;
		pos.height = DefaultFill::Height;
	}
	pos.isLeft   = true; 
	pos.isTop    = true;
	pos.isWidth  = true; 
	pos.isHeight = true;

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
		{
			pos.isTop = true;

			if (xmlPosition.Attribute("Height", &pos.height))
				pos.isHeight = true;
			else if (xmlPosition.Attribute("Bottom", &pos.bottom))
				pos.isHeight = false;
		}
		else if (xmlPosition.Attribute("Bottom", &pos.bottom))
		{
			pos.isTop = false;

			if (xmlPosition.Attribute("Height", &pos.height))
				pos.isHeight = true;
		}
		else // Default
		{
			pos.isTop = true;

			if (xmlPosition.Attribute("Height", &pos.height))
				pos.isHeight = true;
		}

		pos.isLoad = true;
	}
	else
		pos.isLoad = false;

	return pos.isLoad;
}

bool SkinListBack::LoadImage(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile, structElm& elm)
{
	XmlNode xmlImage = xmlNode.FirstChild("Image");

	if (xmlImage)
	{
		std::wstring file = xmlImage.Attribute16("File");
		if (!file.empty())
			elm.image.LoadEx(path + file, zipFile);

		elm.isLoad = true;
	}
	else
		elm.isLoad = false;

	return elm.isLoad;
}

bool SkinListBack::LoadLine(XmlNode& xmlNode, structElm& elm)
{
	XmlNode xmlLine = xmlNode.FirstChild("Line");

	if (xmlLine)
	{
		const char* strColor = xmlLine.AttributeRaw("Color");

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

bool SkinListBack::LoadFill(XmlNode& xmlNode, structElm& elm)
{
	XmlNode xmlRect = xmlNode.FirstChild("Fill");

	if (xmlRect)
	{
		const char* strColor = xmlRect.AttributeRaw("Color");

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

void SkinListBack::Draw(HDC dc, CRect& rc, int stateSelect, int statePlay, int stateLine, int stateLibrary)
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

	CRect rcDraw;

	if (pos->isLeft) // Left location
	{
		rcDraw.left = rc.left + pos->left;

		if (pos->isWidth) // Fixed width
			rcDraw.right = rcDraw.left + pos->width;
		else // To right
			rcDraw.right = rc.right - pos->right;
	}
	else // Right location
	{
		rcDraw.left = rc.right - pos->width - pos->right;
		rcDraw.right = rcDraw.left + pos->width;
	}

	if (pos->isTop) // Top location
	{
		rcDraw.top = rc.top + pos->top;

		if (pos->isHeight) // Fixed height
			rcDraw.bottom = rcDraw.top + pos->height;
		else // To bottom
			rcDraw.bottom = rc.bottom - pos->bottom;
	}
	else // Bottom location
	{
		rcDraw.top = rc.bottom - pos->height - pos->bottom;
		rcDraw.bottom = rcDraw.top + pos->height;
	}

	// Draw
	switch (type)
	{
		case Type::Image:
			elm->image.Draw(dc, rcDraw);
			break;
		case Type::Line:
		case Type::Fill:
			::SetBkColor(dc, elm->color);
			::ExtTextOut(dc, 0, 0, ETO_OPAQUE, rcDraw, NULL, 0, NULL);
			break;
	}
}


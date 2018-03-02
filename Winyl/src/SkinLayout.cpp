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

// SkinLayout.cpp : implementation file
//

#include "stdafx.h"
#include "SkinLayout.h"


// SkinLayout

SkinLayout::SkinLayout()
{

}

SkinLayout::~SkinLayout()
{

}

void SkinLayout::DrawElements(HDC dc, bool isAlpha)
{
	for (std::size_t i = 0, size = elements.size(); i < size; ++i)
	{
		elements[i]->Draw(dc, isAlpha);
	}
}

void SkinLayout::UpdateRectElements()
{
		for (std::size_t i = 0, size = elements.size(); i < size; ++i)
		{
			elements[i]->UpdateRect(rcRect);
		}
}

void SkinLayout::HideLayout(bool isHide)
{
	isHidden = isHide;

	for (std::size_t i = 0, size = elements.size(); i < size; ++i)
	{
		elements[i]->element->HideLayout(isHidden);
	}
}

bool SkinLayout::TriggerLayout(int cx, int cy, const SkinTrigger::structTrigger &trigger, const std::vector<std::unique_ptr<SkinLayout>>& layouts)
{
	bool isOldHidden = isHidden;

	isHidden = trigger.isHide;

	for (std::size_t i = 0, size = elements.size(); i < size; ++i)
	{
		elements[i]->element->HideLayout(isHidden);
	}

	if (trigger.width  > -1) pos.width  = trigger.width;
	if (trigger.height > -1) pos.height = trigger.height;
	if (trigger.left   > -1) pos.left   = trigger.left;
	if (trigger.right  > -1) pos.right  = trigger.right;
	if (trigger.top    > -1) pos.top    = trigger.top;
	if (trigger.bottom > -1) pos.bottom = trigger.bottom;
	if (trigger.leftRight > -1) pos.leftRight = trigger.leftRight;
	if (trigger.topBottom > -1) pos.topBottom = trigger.topBottom;

	///////

	CRect rcOld = rcRect;

	if (trigger.leftFromRightID)
	{
		SkinLayout* layout = TriggerFindLayout(trigger.leftFromRightID, layouts);
		if (layout) rcRect.left = layout->rcRect.right;
	}
	if (trigger.rightFromLeftID)
	{
		SkinLayout* layout = TriggerFindLayout(trigger.rightFromLeftID, layouts);
		if (layout) rcRect.right = layout->rcRect.left;
	}
	if (trigger.topFromBottomID)
	{
		SkinLayout* layout = TriggerFindLayout(trigger.topFromBottomID, layouts);
		if (layout) rcRect.top = layout->rcRect.bottom;
	}
	if (trigger.bottomFromTopID)
	{
		SkinLayout* layout = TriggerFindLayout(trigger.bottomFromTopID, layouts);
		if (layout) rcRect.bottom = layout->rcRect.top;
	}

	if (trigger.leftFromLeftID)
	{
		SkinLayout* layout = TriggerFindLayout(trigger.leftFromRightID, layouts);
		if (layout) rcRect.left = layout->rcRect.left;
	}
	if (trigger.rightFromRightID)
	{
		SkinLayout* layout = TriggerFindLayout(trigger.rightFromLeftID, layouts);
		if (layout) rcRect.right = layout->rcRect.right;
	}
	if (trigger.topFromTopID)
	{
		SkinLayout* layout = TriggerFindLayout(trigger.topFromBottomID, layouts);
		if (layout) rcRect.top = layout->rcRect.top;
	}
	if (trigger.bottomFromBottomID)
	{
		SkinLayout* layout = TriggerFindLayout(trigger.bottomFromTopID, layouts);
		if (layout) rcRect.bottom = layout->rcRect.bottom;
	}

	// Calculate new pos using new rect
	if (trigger.leftFromRightID || trigger.rightFromLeftID || trigger.leftFromLeftID || trigger.rightFromRightID)
	{
		if (pos.isLeft) // Left location
		{
			pos.left = rcRect.left;

			if (pos.isWidth) // Fixed width
				pos.width = rcRect.right - rcRect.left;
			else // To right
				pos.right = cx - rcRect.right;
		}
		else if (pos.isRight) // Right location
		{
			pos.width = rcRect.right - rcRect.left;
			pos.right = cx - rcRect.left - pos.width;
		}
	}

	if (trigger.topFromBottomID || trigger.bottomFromTopID || trigger.topFromTopID || trigger.bottomFromBottomID)
	{
		if (pos.isTop) // Top location
		{
			pos.top = rcRect.top;

			if (pos.isHeight) // Fixed height
				pos.height = rcRect.bottom - rcRect.top;
			else // To bottom
				pos.bottom = cy - rcRect.bottom;
		}
		else if (pos.isBottom) // Bottom location
		{
			pos.height = rcRect.bottom - rcRect.top;
			pos.bottom = cy - rcRect.top - pos.height;
		}
	}

	///////

	UpdateRectLayout(0, 0, cx, cy);

	if (isOldHidden == isHidden && rcOld == rcRect)
		return false;

	return true;
}

SkinLayout* SkinLayout::TriggerFindLayout(int id, const std::vector<std::unique_ptr<SkinLayout>>& layouts)
{
	for (std::size_t i = 0, size = layouts.size(); i < size; ++i)
	{
		if (layouts[i]->id == id)
			return layouts[i].get();
	}

	return nullptr;
}

bool SkinLayout::SplitterLayout(int x, int y, int cx, int cy, const SkinSplitter::structSplitter& splitter, CRect& rcMax)
{
	CRect rcDef;

	// First, restore default rect
	if (splitter.isMoveLeft || splitter.isMoveRight)
	{
		if (pos.isLeft) // Left location
		{
			rcDef.left = pos.defLeft;

			if (pos.isWidth) // Fixed width
				rcDef.right = rcDef.left + pos.defWidth;
			else // To right
				rcDef.right = cx - pos.defRight;
		}
		else if (pos.isRight) // Right location
		{
			rcDef.left = cx - pos.defWidth - pos.defRight;
			rcDef.right = rcDef.left + pos.defWidth;
		}
	}

	if (splitter.isMoveTop || splitter.isMoveBottom)
	{
		if (pos.isTop) // Top location
		{
			rcDef.top = pos.defTop;

			if (pos.isHeight) // Fixed height
				rcDef.bottom = rcDef.top + pos.defHeight;
			else // To bottom
				rcDef.bottom = cy - pos.defBottom;
		}
		else if (pos.isBottom) // Bottom location
		{
			rcDef.top = cy - pos.defHeight - pos.defBottom;
			rcDef.bottom = rcDef.top + pos.defHeight;
		}
	}

	CRect rcOld = rcRect;

	// Next, calculate new rect
	if (splitter.isMoveLeft)
	{
		rcRect.left = rcRectOld.left + x;

		if (rcRect.left > rcDef.left + rcMax.right)
			rcRect.left = rcDef.left + rcMax.right;
		if (rcRect.left < rcDef.left - rcMax.left)
			rcRect.left = rcDef.left - rcMax.left;
	}
	if (splitter.isMoveRight)
	{
		rcRect.right = rcRectOld.right + x;

		if (rcRect.right > rcDef.right + rcMax.right)
			rcRect.right = rcDef.right + rcMax.right;
		if (rcRect.right < rcDef.right - rcMax.left)
			rcRect.right = rcDef.right - rcMax.left;
	}
	if (splitter.isMoveTop)
	{
		rcRect.top = rcRectOld.top + y;

		if (rcRect.top > rcDef.top + rcMax.bottom)
			rcRect.top = rcDef.top + rcMax.bottom;
		if (rcRect.top < rcDef.top - rcMax.top)
			rcRect.top = rcDef.top - rcMax.top;
	}
	if (splitter.isMoveBottom)
	{
		rcRect.bottom = rcRectOld.bottom + y;

		if (rcRect.bottom > rcDef.bottom + rcMax.bottom)
			rcRect.bottom = rcDef.bottom + rcMax.bottom;
		if (rcRect.bottom < rcDef.bottom - rcMax.top)
			rcRect.bottom = rcDef.bottom - rcMax.top;
	}

	// Do not redraw if new rect == old rect
	if (rcRect == rcOld)
		return false;

	// Finally, calculate new pos using new rect
	if (splitter.isMoveLeft || splitter.isMoveRight)
	{
		if (pos.isLeft) // Left location
		{
			pos.left = rcRect.left;

			if (pos.isWidth) // Fixed width
				pos.width = rcRect.right - rcRect.left;
			else // To right
				pos.right = cx - rcRect.right;
		}
		else if (pos.isBottom) // Right location
		{
			pos.width = rcRect.right - rcRect.left;
			pos.right = cx - rcRect.left - pos.width;
		}
	}

	if (splitter.isMoveTop || splitter.isMoveBottom)
	{
		if (pos.isTop) // Top location
		{
			pos.top = rcRect.top;

			if (pos.isHeight) // Fixed height
				pos.height = rcRect.bottom - rcRect.top;
			else // To bottom
				pos.bottom = cy - rcRect.bottom;
		}
		else if (pos.isBottom) // Bottom location
		{
			pos.height = rcRect.bottom - rcRect.top;
			pos.bottom = cy - rcRect.top - pos.height;
		}
	}

	return true;
}

void SkinLayout::SplitterLayoutClick()
{
	rcRectOld = rcRect;
}

void SkinLayout::DrawBackground(HDC dc)
{
	for (std::size_t i = 0, size = backs.size(); i < size; ++i)
	{
		backs[i]->Draw(dc, rcRect);
	}
}

void SkinLayout::UpdateRectLayout(int left, int top, int right, int bottom)
{
	if (pos.isLeft) // Left location
	{
		rcRect.left = left + pos.left;

		if (pos.isWidth) // Fixed width
			rcRect.right = rcRect.left + pos.width;
		else if (pos.isRight) // To right
			rcRect.right = right - pos.right;
		else // To center
			rcRect.right = left + (right - left)/2 + pos.leftRight;
	}
	else if (pos.isRight) // Right location
	{
		if (pos.isWidth) // Fixed width
		{
			rcRect.left = right - pos.width - pos.right;
			rcRect.right = rcRect.left + pos.width;
		}
		else // From center
		{
			rcRect.left = left + (right - left)/2 + pos.leftRight;
			rcRect.right = right - pos.right;
		}
	}
	else // Center location
	{
		rcRect.left = left + ((right - left)/2 - pos.width/2) + pos.leftRight;
		rcRect.right = rcRect.left + pos.width;
	}

	if (pos.isTop) // Top location
	{
		rcRect.top = top + pos.top;

		if (pos.isHeight) // Fixed height
			rcRect.bottom = rcRect.top + pos.height;
		else if (pos.isBottom) // To bottom
			rcRect.bottom = bottom - pos.bottom;
		else // To center
			rcRect.bottom = top + (bottom - top)/2 + pos.topBottom;
	}
	else if (pos.isBottom) // Bottom location
	{
		if (pos.isHeight) // Fixed height
		{
			rcRect.top = bottom - pos.height - pos.bottom;
			rcRect.bottom = rcRect.top + pos.height;
		}
		else // From center
		{
			rcRect.top = top + (bottom - top)/2 + pos.topBottom;
			rcRect.bottom = bottom - pos.bottom;
		}
	}
	else // Center location
	{
		rcRect.top = top + ((bottom - top)/2 - pos.height/2) + pos.topBottom;
		rcRect.bottom = rcRect.top + pos.height;
	}
}

bool SkinLayout::LoadPosition(XmlNode& xmlNode)
{
	pos = {};

	XmlNode xmlPosition = xmlNode.FirstChild("Position");

	if (xmlPosition)
	{
		if (xmlPosition.Attribute("Left", &pos.left))
		{
			pos.isLeft = true;

			if (xmlPosition.Attribute("Width", &pos.width))
				pos.isWidth = true;
			else if (xmlPosition.Attribute("Right", &pos.right))
				pos.isRight = true;
			else if (xmlPosition.Attribute("LeftRight", &pos.leftRight))
				pos.isLeftRight = true;
			else // Default
				pos.isWidth = true; // Default
		}
		else if (xmlPosition.Attribute("Right", &pos.right))
		{
			pos.isRight = true;

			if (xmlPosition.Attribute("Width", &pos.width))
				pos.isWidth = true;
			else if (xmlPosition.Attribute("LeftRight", &pos.leftRight))
				pos.isLeftRight = true;
			else // Default
				pos.isWidth = true; // Default
		}
		else if (xmlPosition.Attribute("LeftRight", &pos.leftRight))
		{
			pos.isLeftRight = true;

			pos.isWidth = true; // Default

			xmlPosition.Attribute("Width", &pos.width);
		}
		else // Default
		{
			pos.isLeft = true; // Default

			pos.isWidth = true; // Default

			xmlPosition.Attribute("Width", &pos.width);
		}

		if (xmlPosition.Attribute("Top", &pos.top))
		{
			pos.isTop = true;

			if (xmlPosition.Attribute("Height", &pos.height))
				pos.isHeight = true;
			else if (xmlPosition.Attribute("Bottom", &pos.bottom))
				pos.isBottom = true;
			else if (xmlPosition.Attribute("TopBottom", &pos.topBottom))
				pos.isTopBottom = true;
			else // Default
				pos.isHeight = true; // Default
		}
		else if (xmlPosition.Attribute("Bottom", &pos.bottom))
		{
			pos.isBottom = true;

			if (xmlPosition.Attribute("Height", &pos.height))
				pos.isHeight = true;
			else if (xmlPosition.Attribute("TopBottom", &pos.topBottom))
				pos.isTopBottom = true;
			else // Default
				pos.isHeight = true; // Default
		}
		else if (xmlPosition.Attribute("TopBottom", &pos.topBottom))
		{
			pos.isTopBottom = true;

			pos.isHeight = true; // Default

			xmlPosition.Attribute("Height", &pos.height);
		}
		else // Default
		{
			pos.isTop = true; // Default

			pos.isHeight = true; // Default

			xmlPosition.Attribute("Height", &pos.height);
		}

		pos.defWidth     = pos.width;
		pos.defHeight    = pos.height;
		pos.defLeft      = pos.left;
		pos.defRight     = pos.right;
		pos.defTop       = pos.top;
		pos.defBottom    = pos.bottom;
		pos.defLeftRight = pos.leftRight;
		pos.defTopBottom = pos.topBottom;

		return true;
	}

	return false;
}

bool SkinLayout::LoadBackground(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile)
{
	XmlNode xmlBackground = xmlNode.FirstChild("Background");

	if (xmlBackground)
	{
		for (XmlNode xmlLoad = xmlBackground.FirstChild(); xmlLoad; xmlLoad = xmlLoad.NextChild())
		{
			backs.emplace_back(new SkinLayoutBack());
			backs.back()->LoadSkin(xmlLoad, path, zipFile);
		}

		return true;
	}
	return false;
}

bool SkinLayout::LoadForeground(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile)
{
	XmlNode xmlForeground = xmlNode.FirstChild("Foreground");

	if (xmlForeground)
	{
		for (XmlNode xmlLoad = xmlForeground.FirstChild(); xmlLoad; xmlLoad = xmlLoad.NextChild())
		{
			forwards.emplace_back(new SkinLayoutBack());
			forwards.back()->LoadSkin(xmlLoad, path, zipFile);
		}

		return true;
	}
	return false;
}

bool SkinLayout::LoadElements(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile)
{
	XmlNode xmlElements = xmlNode.FirstChild("Elements");

	if (xmlElements)
	{
		for (XmlNode xmlLoad = xmlElements.FirstChild(); xmlLoad; xmlLoad = xmlLoad.NextChild())
		{
			elements.emplace_back(new SkinLayoutElement());
			elements.back()->LoadSkin(xmlLoad, path, zipFile, isHidden);
		}

		return true;
	}
	return false;
}

bool SkinLayout::LoadSkin(XmlNode& xmlNode, std::wstring& path, ZipFile* zipFile)
{
	id = 0;
	xmlNode.Attribute("ID", &id);

	isHidden = false;
	xmlNode.Attribute("Hide", &isHidden);

	LoadPosition(xmlNode);

	isTest = true;

	if (LoadElements(xmlNode, path, zipFile))
		isTest = false;

	if (LoadBackground(xmlNode, path, zipFile))
		isTest = false;

//	if (LoadForeground(xmlNode, path, zipFile))
//		isTest = false;

	return true;
}

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

// SkinElement.cpp : implementation file
//

#include "stdafx.h"
#include "SkinElement.h"
#include "ToolTips.h"

SkinElement::SkinElement()
{

}

SkinElement::~SkinElement()
{

}

bool SkinElement::LoadSkin(const std::wstring& file, ZipFile* zipFile)
{
	skinName = file;
	this->zipFile = zipFile;
	return true;
}

void SkinElement::SetRect(CRect& rcDraw)
{
	rcRect = rcDraw;

	// Check IsWindow because there was strange asserts when start the program in maximize mode and change the skin

	if (toolTip)
	{
		CRect rc;

		// Set empty rect for invisible tooltips or they conflict with visible
		if (!isHidden) rc = rcRect;
		else rc.SetRectEmpty();

		toolTip->SetToolRect(this, rc);
	}

	if (window && ::IsWindow(window))
	{
		if (isHidden && ::IsWindowVisible(window))
			::ShowWindow(window, SW_HIDE);
		else if (!isHidden && !::IsWindowVisible(window))
			::ShowWindow(window, SW_SHOW);

		::MoveWindow(window, rcRect.left, rcRect.top, rcRect.Width(), rcRect.Height(), FALSE);
	}
}

void SkinElement::Draw(HDC dc, bool isAlpha)
{

}

bool SkinElement::DrawFade(HDC dc, bool isAlpha)
{
	return true;
}

int SkinElement::GetWidth()
{
	return 10;
}

int SkinElement::GetHeight()
{
	return 10;
}

SkinElement* SkinElement::OnMouseMove(UINT nFlags, CPoint& point)
{
	return nullptr;
}

SkinElement* SkinElement::OnButtonDown(UINT nFlags, CPoint& point)
{
	return nullptr;
}

SkinElement* SkinElement::OnButtonUp(UINT nFlags, CPoint& point)
{
	return nullptr;
}

SkinElement* SkinElement::OnMouseLeave()
{
	return nullptr;
}

int SkinElement::GetParam()
{
	return 0;
}

void SkinElement::SetParam(int param)
{

}

bool SkinElement::IsRedrawHover()
{
	return false;
}

bool SkinElement::IsRedrawPress()
{
	return false;
}


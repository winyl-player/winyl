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
#include "ToolTips.h"

ToolTips::ToolTips()
{

}

ToolTips::~ToolTips()
{

}

void ToolTips::Create()
{
	CreateTrackingToolTip();

	if (wndOwner && skinDraw)
	{
		toolTip = ::CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, L"", WS_POPUP|TTS_ALWAYSTIP|TTS_NOPREFIX,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, wndOwner, NULL, ::GetModuleHandle(NULL), NULL);

		CRect rc;
		rc.SetRectEmpty();
		::GetClientRect(wndOwner, rc);

		for (std::size_t i = 0, isize = skinDraw->Layouts().size(); i < isize; ++i)
		{
			for (std::size_t j = 0, jsize = skinDraw->Layouts()[i]->elements.size(); j < jsize; ++j)
			{
				SkinElement* element = skinDraw->Layouts()[i]->elements[j]->element.get();
				if (element->SetToolTip(this))
				{
					TOOLINFO tool = {};
					tool.uFlags = 0; // TTF_SUBCLASS
					tool.cbSize = sizeof(TOOLINFO);
					tool.hwnd = wndOwner;
					tool.uId = (UINT_PTR)element;
					tool.lpszText = LPSTR_TEXTCALLBACK;
					tool.rect = rc;

					::SendMessage(toolTip, TTM_ADDTOOL, 0, (LPARAM)&tool);
				}
			}
		}

		::SendMessage(toolTip, TTM_ACTIVATE, TRUE, 0);
	}
}

void ToolTips::Destroy()
{
	if (toolTip)
	{
		::DestroyWindow(toolTip);
		toolTip = NULL;
	}

	wndOwner = NULL;
	skinDraw = NULL;

	if (trackingTip)
	{
		::DestroyWindow(trackingTip);
		trackingTip = NULL;
	}
}

void ToolTips::SetToolRect(void* ptrID, CRect& rc)
{
	TOOLINFO tool = {};
	tool.cbSize = sizeof(TOOLINFO);
	tool.hwnd = wndOwner;
	tool.uId = (UINT_PTR)ptrID;
	tool.rect = rc;
	::SendMessage(toolTip, TTM_NEWTOOLRECT, 0, (LPARAM)&tool);
}

const std::wstring* ToolTips::GetText(SkinElement* element)
{
	if (element->IsHidden())
		return nullptr;

	switch (element->GetType())
	{
		case SkinElement::Type::PlayPause:
			if (element->GetParam() == 0)
				return &lang->GetLineS(Lang::Tooltips, 4);
			else
				return &lang->GetLineS(Lang::Tooltips, 5);
			break;
		case SkinElement::Type::Stop:
			return &lang->GetLineS(Lang::Tooltips, 6);
			break;
		case SkinElement::Type::Next:
			return &lang->GetLineS(Lang::Tooltips, 7);
			break;
		case SkinElement::Type::Prev:
			return &lang->GetLineS(Lang::Tooltips, 8);
			break;
		case SkinElement::Type::Equalizer:
			return &lang->GetLineS(Lang::Tooltips, 9);
			break;
		case SkinElement::Type::Button:
		case SkinElement::Type::Switch:
			return &lang->GetLineS(Lang::Tooltips, 10);
			break;
		case SkinElement::Type::Mute:
			if (element->GetParam() == 0)
				return &lang->GetLineS(Lang::Tooltips, 11);
			else
				return &lang->GetLineS(Lang::Tooltips, 12);
			break;
		case SkinElement::Type::Repeat:
			if (element->GetParam() == 0)
				return &lang->GetLineS(Lang::Tooltips, 13);
			else
				return &lang->GetLineS(Lang::Tooltips, 14);
			break;
		case SkinElement::Type::Shuffle:
			if (element->GetParam() == 0)
				return &lang->GetLineS(Lang::Tooltips, 15);
			else
				return &lang->GetLineS(Lang::Tooltips, 16);
			break;
		case SkinElement::Type::MainMenu:
			return &lang->GetLineS(Lang::Tooltips, 17);
			break;
	}

	return nullptr;
}

void ToolTips::RelayEvent(MSG* msg)
{
	if (wndOwner == msg->hwnd)
	{
		//if (msg->message == WM_MOUSEMOVE ||
		//	msg->message == WM_LBUTTONDOWN ||
		//	msg->message == WM_LBUTTONUP ||
		//	msg->message == WM_RBUTTONDOWN ||
		//	msg->message == WM_RBUTTONUP)
		if (msg->message >= WM_MOUSEMOVE && msg->message < WM_MOUSEWHEEL)
		{
			TTHITTESTINFO hit = {};
			hit.hwnd = wndOwner;
			hit.pt.x = LOWORD(msg->lParam);
			hit.pt.y = HIWORD(msg->lParam);
			hit.ti.cbSize = sizeof(TOOLINFO);

			if (::SendMessage(toolTip, TTM_HITTEST, 0, (LPARAM)&hit))
				::SendMessage(toolTip, TTM_RELAYEVENT, 0, (LPARAM)msg);

			if (msg->message == WM_LBUTTONDOWN)
				::SendMessage(toolTip, TTM_POP, 0, 0);
		}
	}
}

void ToolTips::CreateTrackingToolTip()
{
	if (trackingTip)
		return;

    trackingTip = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL, WS_POPUP|TTS_ALWAYSTIP|TTS_NOPREFIX, 
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, wndOwner, NULL, ::GetModuleHandle(NULL), NULL);

	TOOLINFO tool = {};
	tool.cbSize   = sizeof(TOOLINFO);
	tool.uFlags   = TTF_TRACK|TTF_ABSOLUTE;
	tool.hwnd     = wndOwner;
	tool.uId      = 0;
    tool.lpszText = NULL;
	::GetClientRect(wndOwner, &tool.rect);

	::SendMessage(trackingTip, TTM_ADDTOOL, 0, (LPARAM)&tool);	
}

void ToolTips::DeactivateTrackingToolTip()
{
	if (trackingTip == NULL)
		return;

	TOOLINFO tool = {};
	tool.cbSize = sizeof(TOOLINFO);
	tool.hwnd = wndOwner;
	tool.uId = 0;

	::SendMessage(trackingTip, TTM_TRACKACTIVATE, FALSE, (LPARAM)&tool);
}

void ToolTips::TrackingToolTip(bool activate, const std::wstring& value, int x, int y, bool bound)
{
	if (trackingTip == NULL)
		return;

	TOOLINFO tool = {};
	tool.cbSize = sizeof(TOOLINFO);
	tool.hwnd = wndOwner;
	tool.uId = 0;

	tool.lpszText = const_cast<wchar_t*>(value.c_str());

	::SendMessage(trackingTip, TTM_UPDATETIPTEXT, 0, (LPARAM)&tool);

	// TTM_TRACKACTIVATE should be after TTM_UPDATETIPTEXT and before retrieving tooltip size or size will be incorrect
	if (activate)
		::SendMessage(trackingTip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&tool);

	// Get tooltip size
	CRect rc;
	::GetClientRect(trackingTip, rc);
	// Alternative way to get tooltip size
	//LRESULT result = SendMessage(trackingTip, TTM_GETBUBBLESIZE, 0, (LPARAM)&tool);
	//SIZE sz = {GET_X_LPARAM(result), GET_Y_LPARAM(result)};

	POINT pt = {x, y};
	::ClientToScreen(wndOwner, &pt);

	int posx = pt.x - rc.Width() / 2;
	int posy = pt.y - 4 - rc.Height();

	if (bound)
	{
		CRect rcWindow;
		::GetWindowRect(wndOwner, rcWindow);
		if (posx < rcWindow.left)
			posx = rcWindow.left;
		if (posx + rc.Width() > rcWindow.right)
			posx = rcWindow.right - rc.right;
	}

	::SendMessage(trackingTip, TTM_TRACKPOSITION, 0, (LPARAM)MAKELONG(posx, posy));
}

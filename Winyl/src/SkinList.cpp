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

// SkinList.cpp : implementation file
//

#include "stdafx.h"
#include "SkinList.h"


// SkinList

SkinList::SkinList()
{
	rootNode.reset(new SkinListNode());

	randomEngine.seed((unsigned)std::chrono::system_clock::now().time_since_epoch().count());
	//randomEngine.seed(std::random_device{}());
}

SkinList::~SkinList()
{
	if (fontNoItems)
		::DeleteObject(fontNoItems);
}

LRESULT SkinList::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(thisWnd, &ps);
		OnPaint(hdc, ps);
		EndPaint(thisWnd, &ps);
		return 0;
	case WM_ERASEBKGND:
		return 1;
	case WM_TIMER:
		OnTimer((UINT_PTR)wParam);
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove((UINT)wParam, CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		return 0;
	case WM_SIZE:
		OnSize((UINT)wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEWHEEL:
		OnMouseWheel(GET_KEYSTATE_WPARAM(wParam), GET_WHEEL_DELTA_WPARAM(wParam), CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		return 0;
	case WM_VSCROLL:
		OnVScroll(LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
		return 0;
	case WM_LBUTTONDOWN:
		OnLButtonDown((UINT)wParam, CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		return 0;
	case WM_LBUTTONUP:
		OnLButtonUp((UINT)wParam, CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		return 0;
	case WM_LBUTTONDBLCLK:
		OnLButtonDblClk((UINT)wParam, CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		return 0;
	case WM_RBUTTONDOWN:
		OnRButtonDown((UINT)wParam, CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		return 0;
	case WM_CONTEXTMENU:
		OnContextMenu((HWND)wParam, CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
		return 0;
	case WM_KEYDOWN:
		OnKeyDown((UINT)wParam, HIWORD(lParam), LOWORD(lParam));
		return 0;
	}

	return ::DefWindowProc(hWnd, message, wParam, lParam);
}

bool SkinList::NewWindow(HWND parent)
{
	if (CreateClassWindow(parent, L"SkinList", WS_CHILDWINDOW|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, 0, true)) //WS_EX_COMPOSITED
	{
		listThread.SetParentWnd(thisWnd);
		return true;
	}

	return false;
}

void SkinList::SetControlRedraw(bool isRedraw)
{
	isControlRedraw = isRedraw;

	if (isControlRedraw)
	{
//		SetRedraw(TRUE);
		ResetScrollBar();
		::InvalidateRect(thisWnd, NULL, FALSE);
	}
//	else
//		SetRedraw(FALSE);
}

void SkinList::ResetIndex(bool isNowPlaying)
{
	if (!isNowPlaying && !isViewPlaylist)
		return;

	ListNodeUnsafe tempNode = rootNode.get();
	if (isNowPlaying)
		tempNode = rootNodePlay.get();

	if (tempNode->HasChild())
	{
		std::wstring format;
		if      (tempNode->ccount < 10)   format = L"%d"; // L"%.d";
		else if (tempNode->ccount < 100)  format = L"%02d"; // L"%.2d";
		//else if (tempNode->ccount < 1000) format = L"%03d"; // L"%.3d";
		else                              format = L"%03d"; // L"%.3d";

		int i = 0;
		for (ListNodeUnsafe node = tempNode->Child(); node != nullptr; node = node->Next())
		{
			++i;
			wchar_t number[64];
			if (i < 1000)
				swprintf_s(number, format.c_str(), i);
			else
				swprintf_s(number, format.c_str(), i % 1000);
			node->labels[(std::size_t)SkinListElement::Type::Index] = number;
		}
	}
}

ListNodeUnsafe SkinList::InsertHead(ListNodeUnsafe parent, const std::wstring& file)
{
	if (parent == nullptr)
		parent = rootNode.get();

	ListNodeUnsafe newNode = new SkinListNode();

	newNode->labels.resize((std::size_t)SkinListElement::Type::EnumCount);

	newNode->nodeType = SkinListNode::NodeType::Head;

	// File name of the first track (use it to get a cover from the track folder)
	newNode->file = file;

	parent->AddChild(newNode);

	if (isLeftShow)
	{
		newNode->AddLeft();

		newNode->Left()->nodeType = SkinListNode::NodeType::Left;

		newNode->Left()->file = file;

		newNode->Left()->labels.resize((std::size_t)SkinListElement::Type::EnumCount);
	}

	if (isRightShow)
	{
		newNode->AddRight();

		newNode->Right()->nodeType = SkinListNode::NodeType::Right;

		newNode->Right()->file = file;

		newNode->Right()->labels.resize((std::size_t)SkinListElement::Type::EnumCount);
	}

	SetControlRedraw(isControlRedraw);

	return newNode;
}

ListNodeUnsafe SkinList::InsertTrack(ListNodeUnsafe parent, const std::wstring& file, long long idLibrary, long long idPlaylist,
	int rating, int time, unsigned size, long long cue, bool isSelect)
{
	if (parent == nullptr)
		parent = rootNode.get();

	ListNodeUnsafe newNode = new SkinListNode();

	newNode->labels.resize((std::size_t)SkinListElement::Type::EnumCount);

	newNode->nodeType = SkinListNode::NodeType::Song;
	
	newNode->file = file;

	newNode->idLibrary = idLibrary;
	newNode->idPlaylist = idPlaylist;
	newNode->rating = rating;
	newNode->trackCue = cue;
	newNode->trackTime = time;
	newNode->trackSize = size;

	if (isSelect)
	{
		newNode->isSelect = true;
		newNode->stateSelect = SkinListNode::StateFlag::Select;
		selectedNodes.emplace_back(newNode);

		//if (!isFocus)
		//	newNode->stateSelect = SkinListNode::STATE_SELECT;
		//else
		//{
		//	if (focusNode)
		//		focusNode->stateSelect = SkinListNode::STATE_SELECT;

		//	newNode->stateSelect = SkinListNode::STATE_FOCUS;
		//	focusNode = newNode;
		//}
	}

	if (isViewPlaylist)
		newNode->stateLibrary = SkinListNode::StateFlag::Playlist;
	else
		newNode->stateLibrary = SkinListNode::StateFlag::Library;

	parent->AddChild(newNode);

	countNodes++;
	totalTime += time;
	totalSize += size;

	SetControlRedraw(isControlRedraw);

	return newNode;
}

ListNodeUnsafe SkinList::InsertTrackToNowPlaying(ListNodeUnsafe parent, const std::wstring& file, long long idLibrary, long long idPlaylist,
	int rating, int time, unsigned size, long long cue)
{
	if (parent == nullptr)
		parent = rootNodePlay.get();

	ListNodeUnsafe newNode = new SkinListNode();

	newNode->labels.resize((std::size_t)SkinListElement::Type::EnumCount);

	newNode->nodeType = SkinListNode::NodeType::Song;
	
	newNode->file = file;

	newNode->idLibrary = idLibrary;
	newNode->idPlaylist = idPlaylist;
	newNode->rating = rating;
	newNode->trackCue = cue;
	newNode->trackTime = time;
	newNode->trackSize = size;

	newNode->stateLibrary = SkinListNode::StateFlag::Playlist;

	parent->AddChild(newNode);

	countNodesPlay++;
	totalTimePlay += time;
	totalSizePlay += size;

	return newNode;
}

void SkinList::SetNodeString(ListNodeUnsafe node, SkinListElement::Type type, const std::wstring& label)
{
	node->labels[(std::size_t)type] = label;
}

void SkinList::SetNodeString2(ListNodeUnsafe node, SkinListElement::Type type, const std::wstring& label)
{
	if (isHeadUpperCase)
	{
		std::wstring labelUpper = StringEx::ToUpper(label);
		node->labels[(std::size_t)type] = labelUpper;
	}
	else
		node->labels[(std::size_t)type] = label;
}

void SkinList::SetLeftString(ListNodeUnsafe node, SkinListElement::Type type, const std::wstring& label)
{
	node->Left()->labels[(std::size_t)type] = label;
}

void SkinList::SetRightString(ListNodeUnsafe node, SkinListElement::Type type, const std::wstring& label)
{
	node->Right()->labels[(std::size_t)type] = label;
}

void SkinList::SetPlayRating(int rating)
{
	if (playNode)
	{
		playNode->rating = rating;

		if (!rootNodePlay || rootNodePlay == rootNode)
			::InvalidateRect(thisWnd, NULL, FALSE);
	}
}

// Beginning of critical functions to remove nodes (check for ending below).
// They are critical because this class designed wrong from the start,
// it uses raw pointers to nodes everywhere and there are so many pointer owners that it hard to control all this mess,
// so we need to carefully control removing of nodes in Delete* functions below (I call them "dead" nodes there),
// or otherwise it cause random crashes with random errors like heap corruption, recursive destructor calls etc.
// Need to redesign this class!
// 5.04.2011 After Now Playing view added it became even worse, it just became too overcomlicated,
// there is no point to redesign anymore, I'll try to refactor some of the code later.
// 10.03.2012 There are 2 typedefs now: ListNodeUnsafe and ListNodeSafe for raw and safe pointers,
// safe pointers uses smart pointers, it helps to control all this mess (also check a comment in RemoveChild function).
// I've rewritten the code and use ListNodeUnsafe for pointers used at the same place where received
// and ListNodeSafe for pointers used at different places of the code (it was easier this way).

void SkinList::DeleteAllNode()
{
	shiftNode.reset();
	dropNodeMove.reset();

	if (rootNodePlay)
	{
		DeleteAllNodePlay(false);
		return;
	}

	StopSmoothScroll();

	// Reinit cover thread (always do this before clear)
	listThread.ReInit();

	// Clear list styles
	isSwapEnabled = false;
	isViewPlaylist = false;
	isRadioEnabled = false;

	countNodes = 0;
	totalTime = 0;
	totalSize = 0;

	// Clear visible nodes because they can contain pointers to "dead" nodes
	visibleNodes.clear();

	// Clear everything that can contain pointers to "dead" nodes
	shuffleIndex = 0;
	shuffleNodes.clear();
	selectedNodes.clear();
	focusNode.reset();
	playNode.reset();
	lastPlayNode.reset();
	tempNextNode.reset();

	// And only after all above clear all nodes
	rootNode.reset(new SkinListNode());

	HScrollSetPos(0, false);

	SetControlRedraw(isControlRedraw);
}

void SkinList::DeleteAllNodePlay(bool isReturnToDefault)
{
	StopSmoothScroll();

	// Reinit cover thread (always do this before clear)
	listThread.ReInit();

	// Clear visible nodes because they can contain pointers to "dead" nodes
	visibleNodes.clear();

	if (isReturnToDefault) // Switch to "Now Playing" view
	{
		// Restore list styles
		isSwapEnabled = isSwapEnabledPlay;
		isViewPlaylist = isViewPlaylistPlay;
		isRadioEnabled = isRadioEnabledPlay;

		countNodes = countNodesPlay;
		totalTime = totalTimePlay;
		totalSize = totalSizePlay;

		// Restore selection
		selectedNodes = selectedNodesPlay;
		selectedNodesPlay.clear();

		// Restore focus
		focusNode = focusNodePlay;
		focusNodePlay.reset();

		// Root node = "Now Playing" root node
		rootNode = rootNodePlay;

		// Restore scroll, do this without SetControlRedraw because need to recalculate scroll before set new pos.
		// Only with isReturnToDefault flag we can do this, everything below should be with SetControlRedraw.
		ResetScrollBar();
		HScrollSetPos(scrollPosPlay, true);
		if (playNode) ScrollToPlayNode();
		::InvalidateRect(thisWnd, NULL, FALSE);
	}
	else if (rootNodePlay == rootNode) // Switch from "Now Playing" view
	{
		// Backup list styles
		isSwapEnabledPlay = isSwapEnabled;
		isViewPlaylistPlay = isViewPlaylist;
		isRadioEnabledPlay = isRadioEnabled;

		countNodesPlay = countNodes;
		totalTimePlay = totalTime;
		totalSizePlay = totalSize;

		countNodes = 0;
		totalTime = 0;
		totalSize = 0;

		// Clear list styles
		isSwapEnabled = false;
		isViewPlaylist = false;
		isRadioEnabled = false;

		// Backup selection
		selectedNodesPlay = selectedNodes;
		selectedNodes.clear();

		// Backup focus
		focusNodePlay = focusNode;
		focusNode.reset();

		// Backup scroll
		scrollPosPlay = HScrollGetPos();

		rootNode.reset(new SkinListNode());

		HScrollSetPos(0, false);
		SetControlRedraw(isControlRedraw);
	}
	else // Switch between NOT "Now Playing" views
	{
		// Clear everything
		isSwapEnabled = false;
		isViewPlaylist = false;
		isRadioEnabled = false;

		countNodes = 0;
		totalTime = 0;
		totalSize = 0;

		selectedNodes.clear();
		focusNode.reset();

		rootNode.reset(new SkinListNode());

		HScrollSetPos(0, false);
		SetControlRedraw(isControlRedraw);
	}
}

void SkinList::NewNowPlaying()
{
	if (!rootNodePlay)
		rootNodePlay = rootNode;
}

void SkinList::DeleteNowPlaying()
{
	if (!rootNodePlay)
		return;

	if (rootNodePlay == rootNode) // "Now Playing" is current view
		rootNodePlay.reset();
	else
	{
		focusNodePlay.reset();
		selectedNodesPlay.clear();
		scrollPosPlay = 0;

		countNodesPlay = 0;
		totalTimePlay = 0;
		totalSizePlay = 0;

		playNode.reset();
		lastPlayNode.reset();
		tempNextNode.reset();

		shuffleIndex = 0;
		shuffleNodes.clear();

		rootNodePlay.reset();
	}
}

void SkinList::NowPlayingList()
{
	if (!rootNodePlay || rootNodePlay == rootNode)
	{
		if (!rootNodePlay)
		{
			rootNodePlay.reset();
			DeleteAllNode();
		}
		if (playNode) {StopSmoothScroll(); ScrollToPlayNode();}
		return;
	}

	DeleteAllNodePlay(true);
}

void SkinList::DeleteSelected()
{
	SetControlRedraw(false);

	// Search for next node
	ListNodeSafe nextNode;
	if (!selectedNodes.empty())
	{
		nextNode.reset(FindNextNode(selectedNodes.back().get()));
		if (!nextNode)
			nextNode.reset(FindPrevNode(selectedNodes.front().get()));
	}

	// Clear visible nodes because they can contain pointers to "dead" nodes
	visibleNodes.clear();

	// Must clear shuffle too because shuffle array can contain pointers to "dead" nodes
	if (!rootNodePlay || rootNodePlay == rootNode)
		RemoveShuffle();

	// Clear everything that can contain pointers to "dead" nodes
	focusNode.reset();
	shiftNode.reset();

	for (std::size_t i = 0, size = selectedNodes.size(); i < size; ++i)
	{
		ListNodeUnsafe node = selectedNodes[i].get();
		selectedNodes[i].reset();

		// Clear playing node if needed
		if (node == playNode.get())
			playNode.reset();

		if (node == lastPlayNode.get())
			lastPlayNode.reset();

		if (node == tempNextNode.get())
			tempNextNode.reset();

		countNodes--;
		totalTime -= node->trackTime;
		totalSize -= node->trackSize;

		ListNodeUnsafe parent = node->Parent();

		// Remove child from parent node
		parent->RemoveChild(node);

		// If all children are removed then remove parent
		if (parent != rootNode.get() && !parent->HasChild())
		{
			parent->Parent()->RemoveChild(parent);
		}
	}

	selectedNodes.clear();

	// Select next node
	if (nextNode)
	{
		nextNode->stateSelect = SkinListNode::StateFlag::Focus;
		if (!nextNode->isSelect)
		{
			nextNode->isSelect = true;
			selectedNodes.emplace_back(nextNode);
		}

		focusNode = nextNode;
	}

	SetControlRedraw(isControlRedraw);
}

// Ending of critical functions to remove nodes.

void SkinList::OnPaint(HDC dc, PAINTSTRUCT& ps)
{
	// ::Sleep(50) // To test smooth scrolling with lags

    CRect rc = ps.rcPaint;

	visibleNodes.clear();

	// New dcMem //
	HDC dcMemory = ::CreateCompatibleDC(dc);

	HBITMAP bmMemory = ExImage::Create32BppBitmap(rc.Width(), rc.Height());
	
	HGDIOBJ oldMemory = ::SelectObject(dcMemory, bmMemory);
	// End dcMem //

	::SetBkColor(dcMemory, backColor);
	ExImage::FillDC(dcMemory, 0, 0, rc.Width(), rc.Height());

	::SetBkMode(dcMemory, TRANSPARENT);

	if (imageBack.IsValid())
		imageBack.Crop(dcMemory, 0, 0, rc.Width(), rc.Height()); // Draw background image

	CRect rc2;
	::GetClientRect(thisWnd, rc2);
	
	// Recursively draw all nodes
	if (isControlRedraw)
	{
		if (rootNode->HasChild())
		{
			DrawNodesRecursive(dcMemory, rootNode.get(), -rc.left, -HScrollGetPos() - rc.top,
				rc2.right - scrollWidth - rc.left, rc.Height());

			VisibleNodesRecursive(rootNode.get(), -HScrollGetPos(), rc2.Height());
		}
		else
		{
			HGDIOBJ oldFont = ::SelectObject(dcMemory, fontNoItems);
			::SetTextColor(dcMemory, fontColorNoItems);
			CRect rcFont(0, 20, rc2.right - scrollWidth, rc2.bottom);
			//rcFont.MoveToXY(rcFont.left + (rc2.left - rc.left), rcFont.top + (rc2.top - rc.top));
			rcFont.OffsetRect(rc2.left - rc.left, rc2.top - rc.top);
			::DrawText(dcMemory, stringNoItems.c_str(), (int)stringNoItems.size(), rcFont, DT_CENTER|DT_SINGLELINE|DT_NOPREFIX|DT_NOCLIP);
			::SelectObject(dcMemory, oldFont);
		}
	}

	// Draw move line
	if (isMoveElement)
	{
		if (dropPosScroll == HScrollGetPos())
		{
			::SetBkColor(dcMemory, dropColor);
			CRect rcLine;

			rcLine.SetRect(0, dropPosMove - 2, 1, dropPosMove - 2 + 5);
			rcLine.OffsetRect(rc2.left - rc.left, rc2.top - rc.top);
			ExImage::FillDC(dcMemory, rcLine);

			rcLine.SetRect(1, dropPosMove - 1, 1 + 1, dropPosMove - 1 + 3);
			rcLine.OffsetRect(rc2.left - rc.left, rc2.top - rc.top);
			ExImage::FillDC(dcMemory, rcLine);

			rcLine.SetRect(2, dropPosMove, 2 + 1, dropPosMove + 1);
			rcLine.OffsetRect(rc2.left - rc.left, rc2.top - rc.top);
			ExImage::FillDC(dcMemory, rcLine);

			rcLine.SetRect(4, dropPosMove, rc2.right, dropPosMove + 1);
			rcLine.OffsetRect(rc2.left - rc.left, rc2.top - rc.top);
			ExImage::FillDC(dcMemory, rcLine);
		}
	}


	// Copy dcMem //
	::BitBlt(dc, rc.left, rc.top, rc.Width(), rc.Height(), dcMemory, 0, 0, SRCCOPY);

	::SelectObject(dcMemory, oldMemory);

	::DeleteObject(bmMemory);
	::DeleteDC(dcMemory);
	// Copy dcMem //

	// Start cover loader thread
	listThread.DrawCover();
}

int SkinList::VisibleNodesRecursive(SkinListNode* recursiveNode, int y, int height)
{
	// If there are no nodes to draw just return
	//if (!recursiveNode->HasChild())
	//	return y;

	int y2 = y;

	for (ListNodeUnsafe node = recursiveNode->Child(); node != nullptr; node = node->Next())
	{
		if (y > height) // Do not draw nodes that beyond window bottom
			return y;

		if (node->nodeType == SkinListNode::NodeType::Song) // Draw track
		{
			if (y > -trackHeight) // Draw only visible nodes
			{
				visibleNodes.push_back(node); // Fill visible nodes array
			}
		}
		else if (node->nodeType == SkinListNode::NodeType::Head) // Draw header
		{
			if (y > -headHeight) // Draw only visible nodes
			{
				visibleNodes.push_back(node); // Fill visible nodes array
			}
		}

		y += node->rcNode.Height();

///////////////////////////////////////////////////////////////////////////
		// Recursively draw all open subnodes //
		if (node->isOpen && node->HasChild())
			y = VisibleNodesRecursive(node, y, height);
///////////////////////////////////////////////////////////////////////////
	}

	// Recalculate height to show album art
	if (recursiveNode->left && isLeftAlways && (y - y2) < leftHeight)
		y = y2 + leftHeight;
	if (recursiveNode->right && isRightAlways && (y - y2) < rightHeight)
		y = y2 + rightHeight;

	return y;
}

int SkinList::DrawNodesRecursive(HDC dc, SkinListNode* recursiveNode, int x, int y, int right, int height)
{
	// Latest value is height not bottom like expected because it is only a limiter for drawing,
	// also this function was a bit different before, and for the sake of minimal changes it became slightly illogical.

	// If there are no nodes to draw just return
	//if (!recursiveNode->HasChild())
	//	return y;

	int y2 = y; // Save y to use it at the end to draw panes
	bool isShowL = false; // Show left pane?
	bool isShowR = false; // Show right pane?

	for (ListNodeUnsafe node = recursiveNode->Child(); node != nullptr; node = node->Next())
	{
		if (y > height) // Do not draw nodes that beyond window bottom
			break; //return y;


		if (node->nodeType == SkinListNode::NodeType::Song) // Draw track
		{
			if (y > -trackHeight) // Draw only visible nodes
			{
				// If there is a place for a pane then move nodes
				int showL = 0; int showR = 0;
				if (recursiveNode->left)
				{
					if (isLeftAlways || (recursiveNode->ccount * trackHeight) >= leftHeight)
					{
						isShowL = true;
						if (!isLeftOver) // Left pane overlap tracks?
							showL = leftWidth;
					}
				}
				if (recursiveNode->right)
				{
					if (isRightAlways || (recursiveNode->ccount * trackHeight) >= rightHeight)
					{
						isShowR = true;
						if (!isRightOver) // Right pane overlap tracks?
							showR = rightWidth;
					}
				}

				// Node area
				CRect rc(x + showL, y, right - showR, y + trackHeight);

				// Draw background
				for (std::size_t i = 0, size = skinTrackB.size(); i < size; ++i)
				{
					skinTrackB[i]->Draw(dc, rc, node->stateSelect, node->statePlay, node->stateLine, node->stateLibrary);
				}

				// Draw elements
				for (std::size_t i = 0, size = skinTrack.size(); i < size; ++i)
				{
					if ((int)skinTrack[i]->type >= 0)
						skinTrack[i]->DrawText(dc, rc, node->labels[(std::size_t)skinTrack[i]->type], node->stateSelect, node->statePlay, node->stateLine, node->stateLibrary);
					else if (skinTrack[i]->type == SkinListElement::Type::Rating)
						skinTrack[i]->DrawRating(dc, rc, node->rating, node->stateSelect, node->statePlay, node->stateLine, node->stateLibrary/*, rcRate*/);
					else if (skinTrack[i]->type == SkinListElement::Type::ArtistTitle)
						skinTrack[i]->DrawText2(dc, rc, node->labels[0], node->labels[2], node->stateSelect, node->statePlay, node->stateLine, node->stateLibrary);
				}

				// Fill visible nodes array
				//visibleNodes.push_back(node);
			}
		}
		else if (node->nodeType == SkinListNode::NodeType::Head) // Draw header
		{
			if (y > -headHeight) // Draw only visible nodes
			{
				// Node area
				CRect rc(x, y, right, y + headHeight);

				// Draw background
				for (std::size_t i = 0, size = skinHeadB.size(); i < size; ++i)
				{
					skinHeadB[i]->Draw(dc, rc, node->stateSelect, node->statePlay, node->stateLine, node->stateLibrary);
				}

				// Draw elements
				for (std::size_t i = 0, size = skinHead.size(); i < size; ++i)
				{
					if ((int)skinHead[i]->type >= 0)
						skinHead[i]->DrawText(dc, rc, node->labels[(std::size_t)skinHead[i]->type], node->stateSelect, node->statePlay, node->stateLine, node->stateLibrary);
					else if (skinHead[i]->type == SkinListElement::Type::Cover)
					{
						skinHead[i]->DrawCover(dc, rc, nullptr, node->stateSelect, node->statePlay, node->stateLine, node->stateLibrary, node->szCover);
						listThread.AddCover(node);
					}
					else if (skinHead[i]->type == SkinListElement::Type::ArtistAlbum)
						skinHead[i]->DrawText2(dc, rc, node->labels[0], node->labels[1], node->stateSelect, node->statePlay, node->stateLine, node->stateLibrary);
				}

				// Fill visible nodes array
				//visibleNodes.push_back(node);
			}
		}

		y += node->rcNode.Height();

///////////////////////////////////////////////////////////////////////////
		// Recursively draw all open subnodes //
		if (node->isOpen && node->HasChild())
			y = DrawNodesRecursive(dc, node, x + nodeIndent, y, right, height);
///////////////////////////////////////////////////////////////////////////
	}


	// Draw left pane if present and visible
	if (recursiveNode->left && y2 > -leftHeight)
	{
		if (isLeftAlways || isShowL) // Show left pane?
		{
			// Left pane use Header node states for drawing
			ListNodeUnsafe node = recursiveNode;

			// Node area
			CRect rc(x, y2, x + leftWidth, y2 + leftHeight);

			// Draw background
			for (std::size_t i = 0, size = skinLeftB.size(); i < size; ++i)
				skinLeftB[i]->Draw(dc, rc, node->stateSelect, node->statePlay, node->stateLine, node->stateLibrary);

			// Draw elements
			for (std::size_t i = 0, size = skinLeft.size(); i < size; ++i)
			{
				if ((int)skinLeft[i]->type >= 0)
					skinLeft[i]->DrawText(dc, rc, node->left->labels[(std::size_t)skinLeft[i]->type], node->stateSelect, node->statePlay, node->stateLine, node->stateLibrary);
				else if (skinLeft[i]->type == SkinListElement::Type::Cover)
				{
					skinLeft[i]->DrawCover(dc, rc, node->left->cover, node->stateSelect, node->statePlay, node->stateLine, node->stateLibrary, node->left->szCover);
					listThread.AddCover(node->left);
				}
			}

			// Do not fill visible nodes array because a pane is transparent for mouse click
		}
	}

	// Draw right pane if present and visible
	if (recursiveNode->right && y2 > -rightHeight)
	{
		if (isRightAlways || isShowR) // Show right pane?
		{
			// Right pane use Header node states for drawing
			ListNodeUnsafe node = recursiveNode;

			// Node area
			CRect rc(right - rightWidth, y2, right, y2 + rightHeight);

			// Draw background
			for (std::size_t i = 0, size = skinRightB.size(); i < size; ++i)
				skinRightB[i]->Draw(dc, rc, node->stateSelect, node->statePlay, node->stateLine, node->stateLibrary);

			// Draw elements
			for (std::size_t i = 0, size = skinRight.size(); i < size; ++i)
			{
				if ((int)skinRight[i]->type >= 0)
					skinRight[i]->DrawText(dc, rc, node->right->labels[(std::size_t)skinRight[i]->type], node->stateSelect, node->statePlay, node->stateLine, node->stateLibrary);
				else if (skinRight[i]->type == SkinListElement::Type::Cover)
				{
					skinRight[i]->DrawCover(dc, rc, node->right->cover, node->stateSelect, node->statePlay, node->stateLine, node->stateLibrary, node->right->szCover);
					listThread.AddCover(node->right);
				}
			}

			// Do not fill visible nodes array because a pane is transparent for mouse click
		}
	}

	// Recalculate height to show album art
	if (recursiveNode->left && isLeftAlways && (y - y2) < leftHeight)
		y = y2 + leftHeight;
	if (recursiveNode->right && isRightAlways && (y - y2) < rightHeight)
		y = y2 + rightHeight;

	return y;
}

void SkinList::OnLButtonDown(UINT nFlags, CPoint point)
{
	::SetFocus(thisWnd);

	isSelectedElement = false;

	// Search for clicked node
	int clicked = FindNodeByPoint(point, HScrollGetPos());

	if (!isRadioEnabled)
	{
		// Check if click on track rating
		if (clicked > -1)
		{
			ListNodeUnsafe node = visibleNodes[clicked];
			if (node->nodeType == SkinListNode::NodeType::Song)
			{
				CRect rc; ::GetClientRect(thisWnd, rc);
				CRect rcNode = node->rcNode;
				int scrollPos = HScrollGetPos();

				rcNode.top -= scrollPos;
				rcNode.bottom -= scrollPos;
				rcNode.left = rc.left;
				rcNode.right = rc.right - scrollWidth;

				// Check if click on track rating
				int newRating = 0;
				if (node->nodeType == SkinListNode::NodeType::Song)
				{
					for (std::size_t i = 0, size = skinTrack.size(); i < size; ++i)
					{
						if (skinTrack[i]->type == SkinListElement::Type::Rating)
							newRating = skinTrack[i]->GetClickRating(point, rcNode, node->stateSelect, node->statePlay, node->stateLine, node->stateLibrary);
					}
				}

				// Click on track rating
				if (newRating)
				{
					if (newRating == node->rating)
						node->rating = 0;
					else
						node->rating = newRating;

					::InvalidateRect(thisWnd, NULL, FALSE);
					::UpdateWindow(thisWnd);
	
					::SendMessage(thisParentWnd, UWM_RATING, 0, (LPARAM)node);

					return;
				}
			}
		}
	}

	bool isSelectedChanged = false;

	// Click on node
	if (clicked > -1)
	{
		ListNodeUnsafe node = visibleNodes[clicked];

		isPressElement = true;
		ptPress = point;
		::SetCapture(thisWnd);

		// Click on node with Shift, select multiple tracks
		if ((nFlags & MK_SHIFT))
		{
			if (node->nodeType == SkinListNode::NodeType::Song)
			{
				isSelectedChanged = true;

				// Clear selection from all selected nodes
				for (std::size_t i = 0, size = selectedNodes.size(); i < size; ++i)
				{
					selectedNodes[i]->isSelect = false;
					selectedNodes[i]->stateSelect = SkinListNode::StateFlag::Normal;
				}
				selectedNodes.clear();

				if (!shiftNode)
					shiftNode = focusNode;

				// Select new nodes
				if (!shiftNode || node->rcNode.top >= shiftNode->rcNode.top) // Select below
				{
					SelectShift(shiftNode.get(), node);
				}
				else if (node->rcNode.top < shiftNode->rcNode.top) // Select above
				{
					SelectShift(node, shiftNode.get());
				}

				// Focus clicked node
				focusNode.reset(node);
				node->stateSelect = SkinListNode::StateFlag::Focus;
			}
		}
		// Click on track
		else if (node->nodeType == SkinListNode::NodeType::Song)
		{
			shiftNode.reset();

			if (!node->isSelect)
			{
				if ((nFlags & MK_CONTROL) || selectedNodes.size() > 1)
					isSelectedChanged = true;

				// Click on track without Ctrl
				if (!(nFlags & MK_CONTROL))
				{
					// Clear selection from all selected nodes
					for (std::size_t i = 0, size = selectedNodes.size(); i < size; ++i)
					{
						selectedNodes[i]->isSelect = false;
						selectedNodes[i]->stateSelect = SkinListNode::StateFlag::Normal;
					}
					selectedNodes.clear();
				}
				else if (focusNode)
					focusNode->stateSelect = SkinListNode::StateFlag::Select;

				// Focus clicked node
				focusNode.reset(node);
				node->stateSelect = SkinListNode::StateFlag::Focus;

				if (!node->isSelect)
				{
					node->isSelect = true;
					//selectedNodes.push_back(node);
					////////////////
					// Add node to selected nodes array in the same order that it located
					bool isInsert = false;
					for (std::size_t i = 0, size = selectedNodes.size(); i < size; ++i)
					{
						if (selectedNodes[i]->rcNode.top > node->rcNode.top)
						{
							selectedNodes.emplace(selectedNodes.begin() + i, node);
							isInsert = true;
							break;
						}
					}
					if (!isInsert)
						selectedNodes.emplace_back(node);
					////////////////
				}
			}
			else // Track already selected just focus it
			{
				if (nFlags & MK_CONTROL)
					isSelectedChanged = true;

				// Click on track without Ctrl or Shift
				if (!(nFlags & MK_CONTROL) && !(nFlags & MK_SHIFT))
					isSelectedElement = true; // Set flag to reset selection in MouseUp()

				if (focusNode)
					focusNode->stateSelect = SkinListNode::StateFlag::Select;

				focusNode.reset(node);
				focusNode->stateSelect = SkinListNode::StateFlag::Focus;

				// Remove selection if clicked with Ctrl
				if (nFlags & MK_CONTROL)
				{
					for (std::size_t i = 0, size = selectedNodes.size(); i < size; ++i)
					{
						if (selectedNodes[i] == focusNode)
						{
							selectedNodes.erase(selectedNodes.begin() + i);
							focusNode->isSelect = false;
							focusNode->stateSelect = SkinListNode::StateFlag::Normal;
							focusNode.reset();
							break;
						}
					}
				}
			}
		}
		// Click on header, select all tracks in album
		else if (node->nodeType == SkinListNode::NodeType::Head)
		{
			isSelectedChanged = true;

			shiftNode.reset();

			// Click on header without Ctrl
			if (!(nFlags & MK_CONTROL))
			{
				// Clear selection from all selected nodes
				for (std::size_t i = 0, size = selectedNodes.size(); i < size; ++i)
				{
					selectedNodes[i]->isSelect = false;
					selectedNodes[i]->stateSelect = SkinListNode::StateFlag::Normal;
				}
				selectedNodes.clear();
			}
			else if (focusNode)
				focusNode->stateSelect = SkinListNode::StateFlag::Select;

			// Select all tracks in album
			for (ListNodeUnsafe n = node->Child(); n != nullptr; n = n->Next())
			{
				if (n == node->Child()) // Focus first track
				{
					n->stateSelect = SkinListNode::StateFlag::Focus;
					focusNode.reset(n);
				}
				else // Select others
					n->stateSelect = SkinListNode::StateFlag::Select;

				if (!n->isSelect)
				{
					n->isSelect = true;
					selectedNodes.emplace_back(n);
				}
			}
		}

		::InvalidateRect(thisWnd, NULL, FALSE);
	}

	if (isSelectedChanged && !isRadioEnabled)
		::SendMessage(thisParentWnd, UWM_LISTSEL, 0, 0);
}

void SkinList::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (focusNode)
	{
		::SendMessage(thisParentWnd, UWM_PLAYFILE, (WPARAM)focusNode.get(), 0);
	}
}

void SkinList::RemoveSelection()
{
	// Clear selection from all selected nodes
	for (std::size_t i = 0, size = selectedNodes.size(); i < size; ++i)
	{
		selectedNodes[i]->isSelect = false;
		selectedNodes[i]->stateSelect = SkinListNode::StateFlag::Normal;
	}
	selectedNodes.clear();

	focusNode.reset();
}

void SkinList::SetFocusNode(ListNodeUnsafe node, bool needRedraw)
{
	if (focusNode)
		focusNode->stateSelect = SkinListNode::StateFlag::Select;
		
	if (node)
	{
		node->stateSelect = SkinListNode::StateFlag::Focus;
		if (!node->isSelect)
		{
			node->isSelect = true;
			selectedNodes.emplace_back(node);
		}

		focusNode.reset(node);
	}
	
	if (needRedraw)
		::InvalidateRect(thisWnd, NULL, FALSE);
}

void SkinList::SetPlayNode(ListNodeUnsafe node, bool needRedraw)
{
	if (playNode) // Reset play state
	{
		playNode->statePlay = SkinListNode::StateFlag::Stop;
		playNode.reset();
	}

	if (node)
	{
		if (isPlayFocus)
		{
			if (!rootNodePlay || rootNodePlay == rootNode)
			{
				// Clear selection from all selected nodes
				for (std::size_t i = 0, size = selectedNodes.size(); i < size; ++i)
				{
					selectedNodes[i]->isSelect = false;
					selectedNodes[i]->stateSelect = SkinListNode::StateFlag::Normal;
				}
				selectedNodes.clear();

				focusNode.reset();
			}
			else
			{
				// Clear selection from all selected nodes
				for (std::size_t i = 0, size = selectedNodesPlay.size(); i < size; ++i)
				{
					selectedNodesPlay[i]->isSelect = false;
					selectedNodesPlay[i]->stateSelect = SkinListNode::StateFlag::Normal;
				}
				selectedNodesPlay.clear();

				focusNodePlay.reset();
			}
		}

		// Set node play state
		playNode.reset(node);
		playNode->statePlay = SkinListNode::StateFlag::Play;

		lastPlayNode = playNode;

		if (isPlayFocus)
		{
			if (!rootNodePlay || rootNodePlay == rootNode)
			{
				// Select node
				playNode->stateSelect = SkinListNode::StateFlag::Focus;
				if (!playNode->isSelect)
				{
					playNode->isSelect = true;
					selectedNodes.push_back(playNode);
				}

				focusNode = playNode;
			}
			else
			{
				// Select node
				playNode->stateSelect = SkinListNode::StateFlag::Focus;
				if (!playNode->isSelect)
				{
					playNode->isSelect = true;
					selectedNodesPlay.push_back(playNode);
				}

				focusNodePlay = playNode;
			}
		}
	}

	if (needRedraw)
	{
		if (!rootNodePlay || rootNodePlay == rootNode)
			::InvalidateRect(thisWnd, NULL, FALSE);
	}
}

void SkinList::SetPause(bool isPause)
{
	if (playNode)
	{
		if (isPause && playNode->statePlay == SkinListNode::StateFlag::Play)
		{
			playNode->statePlay = SkinListNode::StateFlag::Pause;

			if (!rootNodePlay || rootNodePlay == rootNode)
				::InvalidateRect(thisWnd, NULL, FALSE);
		}
		else if (!isPause && playNode->statePlay == SkinListNode::StateFlag::Pause)
		{
			playNode->statePlay = SkinListNode::StateFlag::Play;

			if (!rootNodePlay || rootNodePlay == rootNode)
				::InvalidateRect(thisWnd, NULL, FALSE);
		}
	}
}

void SkinList::AddShuffle(ListNodeUnsafe node)
{
	if (node)
	{
		if (!node->isShuffle)
		{
			node->isShuffle = true;
			shuffleNodes.emplace_back(node);
			shuffleIndex = shuffleNodes.size() - 1;
		}
	}
}

void SkinList::RemoveShuffle()
{
	if (!shuffleNodes.empty())
	{
		for (std::size_t i = 0, size = shuffleNodes.size(); i < size; ++i)
			shuffleNodes[i]->isShuffle = false;

		shuffleNodes.clear();
		shuffleIndex = 0;
	}
}

ListNodeUnsafe SkinList::FindPrevTrackShuffle()
{
	if (!shuffleNodes.empty() && shuffleIndex > 0)
	{
		shuffleIndex--;
		return shuffleNodes[shuffleIndex].get();
	}
	else
	{
		shuffleIndex = 0;
		return nullptr;
	}
}

ListNodeUnsafe SkinList::FindNextTrackShuffle()
{
	if (!shuffleNodes.empty() && shuffleIndex < shuffleNodes.size() - 1)
	{
		shuffleIndex++;
		return shuffleNodes[shuffleIndex].get();
	}

	int countNew = 0;

	if (!rootNodePlay || rootNodePlay == rootNode)
		countNew = countNodes - (int)shuffleNodes.size();
	else
		countNew = countNodesPlay - (int)shuffleNodes.size();

	if (countNew <= 0)
		return nullptr;

	// Generate a random number
	std::uniform_int<> randomInt(0, countNew - 1);
	int random = randomInt(randomEngine);

	int count = 0;

	// Use recursive function to search for node by the random number
	ListNodeUnsafe node = nullptr;
	if (!rootNodePlay || rootNodePlay == rootNode)
		node = GetRandomNodeR(rootNode.get(), &count, random);
	else
		node = GetRandomNodeR(rootNodePlay.get(), &count, random);

	if (node)
	{
		node->isShuffle = true;
		shuffleNodes.emplace_back(node);
		shuffleIndex = shuffleNodes.size() - 1;
	}

	return node;
}

ListNodeUnsafe SkinList::GetRandomNodeR(ListNodeUnsafe recursiveNode, int* count, int random)
{
	if (!recursiveNode->HasChild())
		return nullptr;

	for (ListNodeUnsafe node = recursiveNode->Child(); node != nullptr; node = node->Next())
	{
		if (node->nodeType == SkinListNode::NodeType::Song)
		{
			if (!node->isShuffle)
			{
				if ((*count) == random)
					return node;

				(*count)++;
			}
		}

		if (node->isOpen && node->HasChild())
		{
			ListNodeUnsafe tempNode = GetRandomNodeR(node, count, random);
			if (tempNode)
				return tempNode;
		}
	}

	return nullptr;
}

ListNodeUnsafe SkinList::FindNodeByIndex(long long index)
{
	if (index == 0)
		return nullptr;

	return FindNodeByIndexR(rootNode.get(), index);
}

ListNodeUnsafe SkinList::FindNodeByIndexR(ListNodeUnsafe recursiveNode, long long index)
{
	if (!recursiveNode->HasChild())
		return nullptr;

	for (ListNodeUnsafe node = recursiveNode->Child(); node != nullptr; node = node->Next())
	{
		if (node->nodeType == SkinListNode::NodeType::Song)
		{
			if (node->idPlaylist)
			{
				if (node->idPlaylist == index)
					return node;
			}
			else if (node->idLibrary)
			{
				if (node->idLibrary == index)
					return node;
			}
		}

		if (node->isOpen && node->HasChild())
		{
			ListNodeUnsafe tempNode = FindNodeByIndexR(node, index);
			if (tempNode)
				return tempNode;
		}
	}

	return nullptr;
}

void SkinList::UpdateCovers()
{
	ClearCoversR(rootNode.get());

	::InvalidateRect(thisWnd, NULL, FALSE);
}

void SkinList::ClearCoversR(ListNodeUnsafe recursiveNode)
{
	if (!recursiveNode->HasChild())
		return;

	for (ListNodeUnsafe node = recursiveNode->Child(); node != nullptr; node = node->Next())
	{
		if (node->nodeType == SkinListNode::NodeType::Head)
		{
			if (node->cover)
			{
				delete node->cover;
				node->cover = nullptr;
				node->isCover = false;
			}
			if (node->left && node->left->cover)
			{
				delete node->left->cover;
				node->left->cover = nullptr;
				node->left->isCover = false;
			}
			if (node->right && node->right->cover)
			{
				delete node->right->cover;
				node->right->cover = nullptr;
				node->right->isCover = false;
			}
		}

		if (node->HasChild())
		{
			ClearCoversR(node);
		}
	}

	return;
}

void SkinList::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (isDragElement)
	{
		SetDropMoveStop();
		return;
	}

	::SetFocus(thisWnd);

	// Search for clicked node
	int clicked = FindNodeByPoint(point, HScrollGetPos());

	// Click on node
	if (clicked > -1)
	{
		ListNodeUnsafe node = visibleNodes[clicked];

		// Select track if clicked on track
		if (node->nodeType == SkinListNode::NodeType::Song)
		{
			if (!node->isSelect)
			{
				if (!(nFlags & MK_CONTROL))
				{
					// Clear selection from all selected nodes
					for (std::size_t i = 0, size = selectedNodes.size(); i < size; ++i)
					{
						selectedNodes[i]->isSelect = false;
						selectedNodes[i]->stateSelect = SkinListNode::StateFlag::Normal;
					}
					selectedNodes.clear();
				}
				else if (focusNode)
					focusNode->stateSelect = SkinListNode::StateFlag::Select;

				// Focus clicked node
				focusNode.reset(node);
				node->stateSelect = SkinListNode::StateFlag::Focus;

				if (!node->isSelect)
				{
					node->isSelect = true;
					selectedNodes.emplace_back(node);
				}
			}
			else // Track already selected just focus it
			{
				isSelectedElement = true; // Set flag to reset selection in MouseUp()

				if (focusNode)
					focusNode->stateSelect = SkinListNode::StateFlag::Select;

				focusNode.reset(node);
				focusNode->stateSelect = SkinListNode::StateFlag::Focus;
			}

			::InvalidateRect(thisWnd, NULL, FALSE);
		}
	}
}

void SkinList::OnContextMenu(HWND hWnd, CPoint point)
{
	if (!focusNode)
		return;

	if (point.x == -1 && point.y == -1) // Menu key or Shift+F10
		::GetCursorPos(&point);

	CPoint temp = point;
	::ScreenToClient(thisWnd, &temp);

	int clicked = FindNodeByPoint(temp, HScrollGetPos());
	if (clicked > -1)
	{
		if (visibleNodes[clicked] == focusNode.get())
		{
			::SendMessage(thisParentWnd, UWM_LISTMENU, point.x, point.y);
		}
	}
}

int SkinList::FindNodeByPoint(const CPoint& point, int scrollPos)
{
	for (std::size_t i = 0, size = visibleNodes.size(); i < size; ++i)
	{
		if (point.y + scrollPos >= visibleNodes[i]->rcNode.top &&
			point.y + scrollPos <= visibleNodes[i]->rcNode.bottom)
			return (int)i;
	}

	return -1;
}

void SkinList::ResetScrollBar()
{
    CRect rcClient;
	::GetClientRect(thisWnd, rcClient);

	heightNodes = CalculateHeight(rootNode.get(), 0, 0);

	if (rcClient.Height() < heightNodes/* + 8*/)
    {
		HScrollSetInfo(0, heightNodes, trackHeight, rcClient.Height());

		scrollWidth = HScrollGetWidth();

		HScrollShow(true);
    }
    else
    {
		HScrollShow(false);

		scrollWidth = 0;

		HScrollSetInfo(0, heightNodes, trackHeight, rcClient.Height());
    }
}

void SkinList::OnVScroll(UINT nSBCode, UINT nPos, HWND hScrollBar)
{
	int oldPos = HScrollMessageGetPosOld();
	if (HScrollMessage((int)nSBCode))
	{
		StopSmoothScroll();

		::ScrollWindowEx(thisWnd, 0, oldPos - HScrollGetPos(), NULL, NULL, NULL, NULL, SW_INVALIDATE);
		::UpdateWindow(thisWnd);
	}
}

void SkinList::OnSize(UINT nType, int cx, int cy)
{
	int scroll = HScrollGetWidth();

	HScrollResize(cx - scroll, 0, scroll, cy);

	if (heightNodes > cy)
	{
		HScrollSetInfo(0, heightNodes, trackHeight, cy);
        
		scrollWidth = scroll;

		HScrollShow(true);
	}
	else
	{
		HScrollShow(false);

		scrollWidth = 0;

		HScrollSetInfo(0, heightNodes, trackHeight, cy);
	}
}

int SkinList::CalculateHeight(SkinListNode* recursiveNode, int y, int nesting)
{
	if (!recursiveNode->HasChild())
		return y;

	int y2 = y;

	int count = 0;
	for (ListNodeUnsafe node = recursiveNode->Child(); node != nullptr; node = node->Next())
	{
		if (node->nodeType == SkinListNode::NodeType::Song)
		{
			node->rcNode.top = y;
			node->rcNode.bottom = y + trackHeight;

			if (count % 2 == 0)
				node->stateLine = SkinListNode::StateFlag::Line1;
			else
				node->stateLine = SkinListNode::StateFlag::Line2;

			count++;
		}
		else if (node->nodeType == SkinListNode::NodeType::Head)
		{
			node->rcNode.top = y;
			node->rcNode.bottom = y + headHeight;
		}

		y += node->rcNode.Height();

		if (node->isOpen && node->HasChild())
			y = CalculateHeight(node, y, nesting + 1);
	}

	if (recursiveNode->left && isLeftAlways && (y - y2) < leftHeight)
		y = y2 + leftHeight;
	if (recursiveNode->right && isRightAlways && (y - y2) < rightHeight)
		y = y2 + rightHeight;

	return y;
}

void SkinList::SelectAll()
{
	selectedNodes.clear();
	SelectAllR(rootNode.get());
	::InvalidateRect(thisWnd, NULL, FALSE);
}

void SkinList::SelectAllR(SkinListNode* recursiveNode)
{
	if (!recursiveNode->HasChild())
		return;

	for (ListNodeUnsafe node = recursiveNode->Child(); node != nullptr; node = node->Next())
	{
		if (node->nodeType == SkinListNode::NodeType::Song)
		{
			if (node->stateSelect != SkinListNode::StateFlag::Focus)
				node->stateSelect = SkinListNode::StateFlag::Select;

			//if (!node->isSelect)
			{
				node->isSelect = true;
				selectedNodes.emplace_back(node);
			}
		}

		if (node->HasChild())
			SelectAllR(node);
	}

	return;
}

void SkinList::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{

}

void SkinList::MouseWheel(bool isReverse)
{
	UINT lineScroll = 3;
	SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &lineScroll, 0);

	int scroll = 0;

	// If scroll by page
	if (lineScroll == WHEEL_PAGESCROLL)
		scroll = HScrollGetPage();
	else
		scroll = lineScroll * trackHeight;

	if (isReverse) scroll = -scroll;

	// Old without smoothing
//	if (HScrollSetPos(HScrollGetPos() + scroll))
//		Invalidate();

	if (!isSmoothScrollEnabled)
	{
		int oldPos = HScrollGetPos();
		if (HScrollSetPos(HScrollGetPos() + scroll))
		{
			::ScrollWindowEx(thisWnd, 0, oldPos - HScrollGetPos(), NULL, NULL, NULL, NULL, SW_INVALIDATE);
			::UpdateWindow(thisWnd);
		}
	}
	else
		SmoothScroll(scroll);
}

void SkinList::StopSmoothScroll()
{
	if (isSmoothScrollEnabled)
	{
		eventSmoothScroll->Reset();
		isSmoothScrollRun = false;
		smoothScrollAcc = 0;
	}
}

void SkinList::SmoothScroll(int scroll)
{
	// For smooth scrolling use two important variables smoothScrollNew and smoothScrollAcc.
	// smoothScrollNew is a new scroll position that the scroll should reach.
	// smoothScrollAcc is the scroll acceleration:
	// if = 0 then scroll and timer are stopped, if > 0 then scroll is moving up, if < 0 moving down.

	if (smoothScrollAcc == 0)
		smoothScrollNew = HScrollGetPos() + scroll;
	else
		smoothScrollNew += scroll;

	// Adjust scroll position
	smoothScrollNew = std::max(HScrollGetMin(), std::min(HScrollGetMax() - HScrollGetPage(), smoothScrollNew));

	// Scroll is already in position
	if (smoothScrollNew == HScrollGetPos())
		return;

	// If scroll is not moving then start timer to move it
	if (smoothScrollAcc == 0)
	{
		// ::SetTimer(thisWnd, 100, 20, NULL); // To test old timer realization (diff / 8)
		isSmoothScrollRun = true;
		eventSmoothScroll->Set();
	}
}

void SkinList::OnTimer(UINT_PTR nIDEvent)
{

}

void SkinList::SmoothScrollRun()
{
	int diff = smoothScrollNew - HScrollGetPos();

	if (diff == 0) return;

	// Slow down scrolling when reaching the destination point
	if (diff > 0)
		smoothScrollAcc = diff / 16 + 1;
	else if (diff < 0)
		smoothScrollAcc = diff / 16 - 1;

	// Scrolling
	int oldPos = HScrollGetPos();
	if (HScrollSetPos(HScrollGetPos() + smoothScrollAcc))
	{
		::ScrollWindowEx(thisWnd, 0, oldPos - HScrollGetPos(), NULL, NULL, NULL, NULL, SW_INVALIDATE);
		// http://stackoverflow.com/questions/17277622/dwm-in-win7-8-gdi
		// Test: redraw main window for smoother scrolling
		//if (futureWin->IsVistaOrLater() && futureWin->IsCompositionEnabled())
		//	::InvalidateRect(thisParentWnd, NULL, FALSE);
		::UpdateWindow(thisWnd);
	}

	if ((smoothScrollAcc > 0 && HScrollGetPos() >= smoothScrollNew) ||
		(smoothScrollAcc < 0 && HScrollGetPos() <= smoothScrollNew))
	{
		// ::KillTimer(thisWnd, 100); // To test old timer realization
		eventSmoothScroll->Reset();
		isSmoothScrollRun = false;
		smoothScrollAcc = 0;
	}
}

void SkinList::OnLButtonUp(UINT nFlags, CPoint point)
{
	bool isSelectedChanged = false;

	if (isSelectedElement && !isDragElement)
	{
		isSelectedElement = false;

		std::size_t size = selectedNodes.size();
		if (size > 1) // More than one are selected, need to clear selection
		{
			// Clear selection from all selected nodes
			for (std::size_t i = 0; i < size; ++i)
			{
				selectedNodes[i]->isSelect = false;
				selectedNodes[i]->stateSelect = SkinListNode::StateFlag::Normal;
			}
			selectedNodes.clear();
			isSelectedChanged = true;

			// Select focused node
			if (focusNode)
			{
				focusNode->stateSelect = SkinListNode::StateFlag::Focus;

				if (!focusNode->isSelect)
				{
					focusNode->isSelect = true;
					selectedNodes.push_back(focusNode);
				}
			}

			::InvalidateRect(thisWnd, NULL, FALSE);
		}
	}

	if (isPressElement)
	{
		isPressElement = false;
		::ReleaseCapture();

		if (isDragElement)
		{
			isDragElement = false;
			::SendMessage(thisParentWnd, UWM_LISTDRAG, 1, 0);
		}

		if (isMoveElement)
		{
			isMoveElement = false;

			// Insert into this parent
			ListNodeUnsafe newParent = rootNode.get();
			//if (dropNode) // For the future
			//	newParent = dropNode->Parent();

			// If insert into the place of one of seleted nodes then need to find first not selected node after it
			// only then it will insert correctly (only needed if selected multiple nodes)
			if (dropNodeMove && dropNodeMove->isSelect && selectedNodes.size() > 1)
			{
				bool isFindNode = false;
				for (ListNodeUnsafe node = dropNodeMove.get(); node != nullptr; node = node->Next())
				{
					if (!node->isSelect) // Only if the node is not selected
					{
						isFindNode = true;
						dropNodeMove.reset(node);
						break;
					}
				}
				if (!isFindNode) // Does not find by any reason (all selected for example)
					dropNodeMove.reset();
			}

			// Move nodes to new position
			for (std::size_t i = 0, size = selectedNodes.size(); i < size; ++i)
				selectedNodes[i]->Parent()->MoveChild(selectedNodes[i].get(), newParent, dropNodeMove.get());

			dropPosMove = 0;
			dropPosScroll = 0;
			dropNodeMove.reset();

			ResetScrollBar(); // Recalculate position of nodes
			ResetIndex();
			::InvalidateRect(thisWnd, NULL, FALSE);
			::SendMessage(thisParentWnd, UWM_LISTSWAP, 0, 0);
		}
	}

	if (isSelectedChanged && !isRadioEnabled)
		::SendMessage(thisParentWnd, UWM_LISTSEL, 0, 0);
}

void SkinList::SetDropMoveStop()
{
	if (isPressElement)
	{
		isPressElement = false;
		::ReleaseCapture();

		SetDropMovePoint(nullptr);
		isMoveElement = false;
		isDragElement = false;

		dropPosMove = 0;
		dropNodeMove.reset();

		::SendMessage(thisParentWnd, UWM_LISTDRAG, 2, 0);
	}
}

void SkinList::SetDropMovePoint(CPoint* point)
{
	if (!isSwapEnabled)
		return;

	if (point == nullptr)
	{
		if (isMoveElement)
		{
			isMoveElement = false;
			//::InvalidateRect(thisWnd, NULL, FALSE);

			// Invalidate old
			CRect rc;
			::GetClientRect(thisWnd, rc);
			int scrollPos = HScrollGetPos();

			int bottom = rc.bottom;
			rc.top = dropPosMove - 2 + dropPosScroll - scrollPos;
			rc.bottom = dropPosMove + 3 + dropPosScroll - scrollPos;
			if (rc.top + 2 + 3 > 0 && rc.bottom - 2 - 3 < bottom) // Do not redraw offscreen
				::InvalidateRect(thisWnd, rc, FALSE);
		}

		return;
	}
	else if (focusNode)
	{
		CPoint pt = (*point);

		ListNodeUnsafe dropNodeMoveNew = dropNodeMove.get();
		int dropPosMoveNew = dropPosMove;
		pt.y += trackHeight / 2;
		int scrollPos = HScrollGetPos();
		int scrollPage = HScrollGetPage();

		int found = FindNodeByPoint(pt, scrollPos);
		if (found > -1)
		{
			dropNodeMoveNew = visibleNodes[found];
			dropPosMoveNew = visibleNodes[found]->rcNode.top - scrollPos;
		}
		else // Latest node
		{
			dropNodeMoveNew = nullptr;
			dropPosMoveNew = heightNodes - 1 - scrollPos; // -1 to do not redraw offscreen
		}

		// Offscreen check
		if (dropPosMoveNew < 0 || dropPosMoveNew > scrollPage - 1) // -1 do not know why, but the calculation more accurate that way
		{
			dropNodeMoveNew = dropNodeMove.get();
			dropPosMoveNew = dropPosMove;
		}

		if (!isMoveElement || dropPosMove != dropPosMoveNew)
		{
			CRect rc;
			::GetClientRect(thisWnd, rc);

			if (isMoveElement) // Invalidate old
			{
				int bottom = rc.bottom;
				rc.top = dropPosMove - 2 + dropPosScroll - scrollPos;
				rc.bottom = dropPosMove + 3 + dropPosScroll - scrollPos;
				if (rc.top + 2 + 3 > 0 && rc.bottom - 2 - 3 < bottom) // Do not redraw offscreen
					::InvalidateRect(thisWnd, rc, FALSE);
			}
			dropPosScroll = scrollPos;

			isMoveElement = true;

			dropNodeMove.reset(dropNodeMoveNew);
			dropPosMove = dropPosMoveNew;
			//::InvalidateRect(thisWnd, NULL, FALSE);

			// Invalidate new
			rc.top = dropPosMove - 2;
			rc.bottom = dropPosMove + 3;
			::InvalidateRect(thisWnd, rc, FALSE);
		}
	}
}

void SkinList::OnMouseMove(UINT nFlags, CPoint point)
{
	if (isPressElement)
	{
		if (isDragElement)
		{
			::SendMessage(thisParentWnd, UWM_LISTDRAG, 0, MAKELPARAM(point.x, point.y));
		}
		// Check the limit we need to move a mouse to initiate Drag'n'Drop
		else if (point.x > ptPress.x + 10 || point.y > ptPress.y + 10 ||
			point.x < ptPress.x - 10 || point.y < ptPress.y - 10)
		{
			isDragElement = true;
		}
	}
}

void SkinList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_UP)
	{
		ListNodeUnsafe node = FindPrevNode(focusNode.get(), false);

		if (node && node != focusNode.get())
		{
			// Without Shift
//			if (!(GetKeyState(VK_LSHIFT) & 0x8000) && !(GetKeyState(VK_RSHIFT) & 0x8000))
//			{
				// Clear selection from all selected nodes
				for (std::size_t i = 0, size = selectedNodes.size(); i < size; ++i)
				{
					selectedNodes[i]->isSelect = false;
					selectedNodes[i]->stateSelect = SkinListNode::StateFlag::Normal;
				}
				selectedNodes.clear();
//			}
//			else if (focusNode)
//				focusNode->stateSelect = STATE_SELECT;

			node->isSelect = true;
			node->stateSelect = SkinListNode::StateFlag::Focus;
			selectedNodes.emplace_back(node);

			shiftNode.reset();
			focusNode.reset(node);

			ScrollToNode(node);
			::InvalidateRect(thisWnd, NULL, FALSE);
		}
		else // Adjust scroll
		{
			if (ScrollToHome())
				::InvalidateRect(thisWnd, NULL, FALSE);
		}
	}
	else if (nChar == VK_DOWN)
	{
		ListNodeUnsafe node = nullptr;
		if (!focusNode) // Nothing is selected, search for first node
			node = FindPrevNode(nullptr, false);
		else
			node = FindNextNode(focusNode.get(), false);

		if (node && node != focusNode.get())
		{
			// Without Shift
//			if (!(GetKeyState(VK_LSHIFT) & 0x8000) && !(GetKeyState(VK_RSHIFT) & 0x8000))
//			{
				// Clear selection from all selected nodes
				for (std::size_t i = 0, size = selectedNodes.size(); i < size; ++i)
				{
					selectedNodes[i]->isSelect = false;
					selectedNodes[i]->stateSelect = SkinListNode::StateFlag::Normal;
				}
				selectedNodes.clear();
//			}
//			else if (focusNode)
//				focusNode->stateSelect = STATE_SELECT;

			node->isSelect = true;
			node->stateSelect = SkinListNode::StateFlag::Focus;
			selectedNodes.emplace_back(node);

			shiftNode.reset();
			focusNode.reset(node);

			ScrollToNode(node);
			::InvalidateRect(thisWnd, NULL, FALSE);
		}
		else // Adjust scroll
		{
			if (ScrollToEnd())
				::InvalidateRect(thisWnd, NULL, FALSE);
		}
	}
	else if (nChar == VK_PRIOR)
	{
		ListNodeUnsafe node = FindPrevNode(focusNode.get(), true);

		if (node && node != focusNode.get())
		{
			// Without Shift
//			if (!(GetKeyState(VK_LSHIFT) & 0x8000) && !(GetKeyState(VK_RSHIFT) & 0x8000))
//			{
				// Clear selection from all selected nodes
				for (std::size_t i = 0, size = selectedNodes.size(); i < size; ++i)
				{
					selectedNodes[i]->isSelect = false;
					selectedNodes[i]->stateSelect = SkinListNode::StateFlag::Normal;
				}
				selectedNodes.clear();
//			}
//			else if (focusNode)
//				focusNode->stateSelect = STATE_SELECT;

			node->isSelect = true;
			node->stateSelect = SkinListNode::StateFlag::Focus;
			selectedNodes.emplace_back(node);

			shiftNode.reset();
			focusNode.reset(node);

			ScrollToNode(node);
			::InvalidateRect(thisWnd, NULL, FALSE);
		}
		else // Adjust scroll
		{
			if (ScrollToHome())
				::InvalidateRect(thisWnd, NULL, FALSE);
		}
	}
	else if (nChar == VK_NEXT)
	{
		ListNodeUnsafe node = nullptr;
		if (!focusNode) // Nothing is selected, search for first node
			node = FindPrevNode(nullptr, true);
		else
			node = FindNextNode(focusNode.get(), true);

		if (node && node != focusNode.get())
		{
			// Without Shift
//			if (!(GetKeyState(VK_LSHIFT) & 0x8000) && !(GetKeyState(VK_RSHIFT) & 0x8000))
//			{
				// Clear selection from all selected nodes
				for (std::size_t i = 0, size = selectedNodes.size(); i < size; ++i)
				{
					selectedNodes[i]->isSelect = false;
					selectedNodes[i]->stateSelect = SkinListNode::StateFlag::Normal;
				}
				selectedNodes.clear();
//			}
//			else if (focusNode)
//				focusNode->stateSelect = STATE_SELECT;

			node->isSelect = true;
			node->stateSelect = SkinListNode::StateFlag::Focus;
			selectedNodes.emplace_back(node);

			shiftNode.reset();
			focusNode.reset(node);

			ScrollToNode(node);
			::InvalidateRect(thisWnd, NULL, FALSE);
		}
		else // Adjust scroll
		{
			if (ScrollToEnd())
				::InvalidateRect(thisWnd, NULL, FALSE);
		}
	}
	else if (nChar == VK_HOME)
	{
		ListNodeUnsafe node = FindPrevNode(nullptr, false);

		if (node && node != focusNode.get())
		{
			// Clear selection from all selected nodes
			for (std::size_t i = 0, size = selectedNodes.size(); i < size; ++i)
			{
				selectedNodes[i]->isSelect = false;
				selectedNodes[i]->stateSelect = SkinListNode::StateFlag::Normal;
			}
			selectedNodes.clear();

			node->isSelect = true;
			node->stateSelect = SkinListNode::StateFlag::Focus;
			selectedNodes.emplace_back(node);

			shiftNode.reset();
			focusNode.reset(node);

			ScrollToHome();
			::InvalidateRect(thisWnd, NULL, FALSE);
		}
		else // Adjust scroll
		{
			if (ScrollToHome())
				::InvalidateRect(thisWnd, NULL, FALSE);
		}
	}
	else if (nChar == VK_END)
	{
		ListNodeUnsafe node = FindNextNode(nullptr, false);

		if (node && node != focusNode.get())
		{
			// Clear selection from all selected nodes
			for (std::size_t i = 0, size = selectedNodes.size(); i < size; ++i)
			{
				selectedNodes[i]->isSelect = false;
				selectedNodes[i]->stateSelect = SkinListNode::StateFlag::Normal;
			}
			selectedNodes.clear();

			node->isSelect = true;
			node->stateSelect = SkinListNode::StateFlag::Focus;
			selectedNodes.emplace_back(node);

			shiftNode.reset();
			focusNode.reset(node);

			ScrollToEnd();
			::InvalidateRect(thisWnd, NULL, FALSE);
		}
		else // Adjust scroll
		{
			if (ScrollToEnd())
				::InvalidateRect(thisWnd, NULL, FALSE);
		}
	}
}

bool SkinList::ScrollToEnd()
{
	StopSmoothScroll();
	return HScrollSetPos(HScrollGetMax());
}

bool SkinList::ScrollToHome()
{
	StopSmoothScroll();
	return HScrollSetPos(HScrollGetMin());
}

bool SkinList::ScrollToNode(ListNodeUnsafe node, int indent)
{
	int pos = HScrollGetPos();
	int page = HScrollGetPage();

	if (node->rcNode.top < pos + indent)
	{
		StopSmoothScroll();
		return HScrollSetPos(node->rcNode.top - indent);
	}
	else if (node->rcNode.bottom > pos + page - indent)
	{
		StopSmoothScroll();
		return HScrollSetPos(node->rcNode.bottom - page + indent);
	}

	return false;
}

ListNodeUnsafe SkinList::FindNextTrack(ListNodeUnsafe findNode, bool fromCurrentList)
{
	return FindNextNode(findNode, false, fromCurrentList ? nullptr : rootNodePlay.get());
}

ListNodeUnsafe SkinList::FindPrevTrack(ListNodeUnsafe findNode, bool fromCurrentList)
{
	return FindPrevNode(findNode, false, fromCurrentList ? nullptr : rootNodePlay.get());
}

ListNodeUnsafe SkinList::FindNextNode(ListNodeUnsafe findNode, bool isPage, ListNodeUnsafe recursiveNode)
{
	// Search node function, helper for a recursive function below,
	// also stores a variable for the recursive function

	// if findNode == nullptr then the function return the latest node
	// if the function return nullptr then a node searched for is latest

	bool isFind = false;
	ListNodeUnsafe lastNode = nullptr;

	if (recursiveNode == nullptr)
		recursiveNode = rootNode.get();

	ListNodeUnsafe resultNode = FindNextNodeR(recursiveNode, findNode, isFind, isPage, lastNode);

	if (findNode == nullptr)
		return lastNode;
	else if (isPage) // Exception for search by page
	{
		if (findNode == lastNode)
			return nullptr;
		else if (resultNode == nullptr)
			return lastNode;
		else
			return resultNode;
	}
	else
		return resultNode;
}

ListNodeUnsafe SkinList::FindNextNodeR(ListNodeUnsafe recursiveNode, ListNodeUnsafe findNode, bool& isFind, bool isPage, ListNodeUnsafe& lastNode)
{
	if (!recursiveNode->HasChild())
		return nullptr;

	for (ListNodeUnsafe node = recursiveNode->Child(); node != nullptr; node = node->Next())
	{
		if (node->nodeType == SkinListNode::NodeType::Song) // Search only tracks
		{
			lastNode = node; // lastNode always contains the latest track

			if (findNode) // Search for a track (else search for the latest track)
			{
				if (isFind)
				{
					if (!isPage) // Return next (KEY_DOWN)
						return node;
					else // Return next (one page down)
					{
						int page = HScrollGetPage() - trackHeight;

						if (node->rcNode.bottom > findNode->rcNode.top + page)
							return node; // Return next (KEY_PAGEDOWN)
					}
				}

				if (node == findNode) // Found a node
					isFind = true;
			}
		}

		// Recursive search
	    if (node->isOpen && node->HasChild())
		{
			ListNodeUnsafe tempNode = nullptr;
		    tempNode = FindNextNodeR(node, findNode, isFind, isPage, lastNode);
			if (tempNode)
				return tempNode;
		}
	}

	return nullptr;
}

ListNodeUnsafe SkinList::FindPrevNode(ListNodeUnsafe findNode, bool isPage, ListNodeUnsafe recursiveNode)
{
	// Search node function, helper for a recursive function below,
	// also stores a variable for the recursive function

	// if findNode == nullptr then the function return the first node
	// if the function return nullptr then a node searched for is first

	ListNodeUnsafe foundNode = nullptr;
	ListNodeUnsafe firstNode = nullptr;

	if (recursiveNode == nullptr)
		recursiveNode = rootNode.get();

	ListNodeUnsafe resultNode = FindPrevNodeR(recursiveNode, findNode, foundNode, isPage, firstNode);

	if (findNode == nullptr)
		return firstNode;
	else if (isPage) // Exception for search by page
	{
		if (findNode == firstNode)
			return nullptr;
		else if (resultNode == nullptr)
			return firstNode;
		else
			return resultNode;
	}
	else
		return resultNode;
}

ListNodeUnsafe SkinList::FindPrevNodeR(ListNodeUnsafe recursiveNode, ListNodeUnsafe findNode, ListNodeUnsafe& foundNode, bool isPage, ListNodeUnsafe& firstNode)
{
	if (!recursiveNode->HasChild())
		return nullptr;

	for (ListNodeUnsafe node = recursiveNode->Child(); node != nullptr; node = node->Next())
	{
		if (node->nodeType == SkinListNode::NodeType::Song) // Search only tracks
		{
			if (firstNode == nullptr) firstNode = node; // firstNode always contains the first track

			if (findNode == nullptr) // Search for the first track
				return nullptr; // Already found

			if (node == findNode) // Found a node
				return foundNode; // Return previous (KEY_UP)

			if (!isPage) // Store previous
				foundNode = node;
			else // Store previous (one page up)
			{
				int page = HScrollGetPage() - trackHeight;

				if (findNode->rcNode.bottom > node->rcNode.top + page)
					foundNode = node; // Return previous (KEY_PAGEUP)
			}
		}

		// Recursive search
		if (node->isOpen && node->HasChild())
		{
			ListNodeUnsafe tempNode = nullptr;
		    tempNode = FindPrevNodeR(node, findNode, foundNode, isPage, firstNode);
			if (tempNode)
				return tempNode;
		}
	}

	return nullptr;
}

void SkinList::SelectShift(ListNodeUnsafe startNode, ListNodeUnsafe endNode)
{
	bool isFind = false;

	SelectShiftR(rootNode.get(), startNode, endNode, isFind);
}

bool SkinList::SelectShiftR(ListNodeUnsafe recursiveNode, ListNodeUnsafe startNode, ListNodeUnsafe endNode, bool &isFind)
{	
	if (!recursiveNode->HasChild())
		return false;

	for (ListNodeUnsafe node = recursiveNode->Child(); node != nullptr; node = node->Next())
	{
		if (node->nodeType == SkinListNode::NodeType::Song) // Search only tracks
		{
			if (startNode == nullptr || node == startNode) // Found start node
			{
				isFind = true;
				if (!shiftNode)
					shiftNode.reset(node);
			}

			// Select all nodes between start and end
			if (isFind)
			{
				node->stateSelect = SkinListNode::StateFlag::Select;
				if (!node->isSelect)
				{
					node->isSelect = true;
					selectedNodes.emplace_back(node);
				}

				if (node == endNode) // Found end node
					return false;
			}
		}

	    if (node->isOpen && node->HasChild())
		{
		    if(!SelectShiftR(node, startNode, endNode, isFind))
				return false;
		}
	}

    return true;
}

void SkinList::CalculateSelectedNodes(int& outCount, int& outTotal, int& outTotalTime, long long& outTotalSize)
{
	outCount = selectedNodes.size();
	outTotal = countNodes;

	if (outCount <= 1)
	{
		outTotalTime = totalTime;
		outTotalSize = totalSize;
	}
	else
	{
		outTotalTime = 0;
		outTotalSize = 0;

		for (std::size_t i = 0, size = selectedNodes.size(); i < size; ++i)
		{
			outTotalTime += selectedNodes[i]->trackTime;
			outTotalSize += selectedNodes[i]->trackSize;
		}
	}
}

bool SkinList::LoadSkin(std::wstring& file, ZipFile* zipFile)
{
	std::wstring path = PathEx::PathFromFile(file);

	XmlFile xmlFile;

	if (xmlFile.LoadEx(file, zipFile))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Playlist");
		
		if (xmlMain)
		{
			XmlNode xmlHead = xmlMain.FirstChild("Head");

			if (xmlHead)
			{
				xmlHead.Attribute("Height", &headHeight);
				xmlHead.Attribute("UpperCase", &isHeadUpperCase);

				XmlNode xmlHeadElements = xmlHead.FirstChild("Elements");

				if (xmlHeadElements)
				{
					for (XmlNode xmlNode = xmlHeadElements.FirstChild(); xmlNode; xmlNode = xmlNode.NextChild())
					{
						skinHead.emplace_back(new SkinListElement());
						skinHead.back()->LoadSkin(xmlNode, path, zipFile);
					}
				}
				// end if (eHeadElements)

				XmlNode xmlHeadBackground = xmlHead.FirstChild("Background");

				if (xmlHeadBackground)
				{
					for (XmlNode xmlNode = xmlHeadBackground.FirstChild(); xmlNode; xmlNode = xmlNode.NextChild())
					{
						skinHeadB.emplace_back(new SkinListBack());
						skinHeadB.back()->LoadSkin(xmlNode, path, zipFile);
					}
				}
				// end if (eHeadBackground)
			}
			// end if (eHead)


			XmlNode xmlTrack = xmlMain.FirstChild("Song");

			if (xmlTrack)
			{
				xmlTrack.Attribute("Height", &trackHeight);

				XmlNode xmlTrackElements = xmlTrack.FirstChild("Elements");

				if (xmlTrackElements)
				{
					for (XmlNode xmlNode = xmlTrackElements.FirstChild(); xmlNode; xmlNode = xmlNode.NextChild())
					{
						skinTrack.emplace_back(new SkinListElement());
						skinTrack.back()->LoadSkin(xmlNode, path, zipFile);
					}
				}
				// end if (eSongElements)

				XmlNode xmlTrackBackground = xmlTrack.FirstChild("Background");

				if (xmlTrackBackground)
				{
					for (XmlNode xmlNode = xmlTrackBackground.FirstChild(); xmlNode; xmlNode = xmlNode.NextChild())
					{
						skinTrackB.emplace_back(new SkinListBack());
						skinTrackB.back()->LoadSkin(xmlNode, path, zipFile);
					}
				}
				// end if (eSongBackground)
			}
			// end if (eSong)


			XmlNode xmlLeft = xmlMain.FirstChild("Left");

			if (xmlLeft)
			{
				isLeftShow = true;

				xmlLeft.Attribute("Width", &leftWidth);
				xmlLeft.Attribute("Height", &leftHeight);
				xmlLeft.Attribute("Over", &isLeftOver);

				XmlNode xmlLeftElements = xmlLeft.FirstChild("Elements");

				if (xmlLeftElements)
				{
					for (XmlNode xmlNode = xmlLeftElements.FirstChild(); xmlNode; xmlNode = xmlNode.NextChild())
					{
						skinLeft.emplace_back(new SkinListElement());
						skinLeft.back()->LoadSkin(xmlNode, path, zipFile);
					}
				}
				// end if (eLeftElements)

				XmlNode xmlLeftBackground = xmlLeft.FirstChild("Background");

				if (xmlLeftBackground)
				{
					for (XmlNode xmlNode = xmlLeftBackground.FirstChild(); xmlNode; xmlNode = xmlNode.NextChild())
					{
						skinLeftB.emplace_back(new SkinListBack());
						skinLeftB.back()->LoadSkin(xmlNode, path, zipFile);
					}
				}
				// end if (eLeftBackground)
			}
			// end if (eLeft)


			XmlNode xmlRight = xmlMain.FirstChild("Right");

			if (xmlRight)
			{
				isRightShow = true;

				xmlRight.Attribute("Width", &rightWidth);
				xmlRight.Attribute("Height", &rightHeight);
				xmlRight.Attribute("Over", &isRightOver);

				XmlNode xmlRightElements = xmlRight.FirstChild("Elements");

				if (xmlRightElements)
				{
					for (XmlNode xmlNode = xmlRightElements.FirstChild(); xmlNode; xmlNode = xmlNode.NextChild())
					{
						skinRight.emplace_back(new SkinListElement());
						skinRight.back()->LoadSkin(xmlNode, path, zipFile);
					}
				}
				// end if (eRightElements)

				XmlNode xmlRightBackground = xmlRight.FirstChild("Background");

				if (xmlRightBackground)
				{
					for (XmlNode xmlNode = xmlRightBackground.FirstChild(); xmlNode; xmlNode = xmlNode.NextChild())
					{
						skinRightB.emplace_back(new SkinListBack());
						skinRightB.back()->LoadSkin(xmlNode, path, zipFile);
					}
				}
				// end if (eRightBackground)
			}
			// end if (eRight)


			XmlNode xmlBackground = xmlMain.FirstChild("Background");
			if (xmlBackground)
			{
				const char* color = xmlBackground.AttributeRaw("Color");
				if (color && strlen(color) == 6)
				{
					DWORD c = strtoul(color, 0, 16);
					backColor = RGB(GetBValue(c), GetGValue(c), GetRValue(c));
				}

				std::wstring attr = xmlBackground.Attribute16("File");
				if (!attr.empty())
					imageBack.LoadEx(path + attr, zipFile);
			}

			XmlNode xmlFontEmpty = xmlMain.FirstChild("FontNoItems");
			if (xmlFontEmpty)
			{
				const char* fname = nullptr; int fsize = 0; int fbold = 0; int fitalic = 0; int fclear = 0;

				fname = xmlFontEmpty.AttributeRaw("Name");
				xmlFontEmpty.Attribute("Size", &fsize);
				xmlFontEmpty.Attribute("Bold", &fbold);
				xmlFontEmpty.Attribute("Italic", &fitalic);
				xmlFontEmpty.Attribute("ClearType", &fclear);

				fontNoItems = ExImage::CreateElementFont(fname, fsize, fbold, fitalic, fclear);

				const char* color = xmlFontEmpty.AttributeRaw("Color");
				if (color && strlen(color) == 6)
				{
					DWORD c = strtoul(color, 0, 16);
					fontColorNoItems = RGB(GetBValue(c), GetGValue(c), GetRValue(c));
				}
			}

			XmlNode xmlDropLine = xmlMain.FirstChild("DropLine");
			if (xmlDropLine)
			{
				const char* color = xmlDropLine.AttributeRaw("Color");
				if (color && strlen(color) == 6)
				{
					DWORD c = strtoul(color, 0, 16);
					dropColor = RGB(GetBValue(c), GetGValue(c), GetRValue(c));
				}
			}

			XmlNode xmlScroll = xmlMain.FirstChild("Scroll");
			if (xmlScroll)
			{
				std::wstring attr = xmlScroll.Attribute16("File");
				if (!attr.empty())
				{
					skinScroll.reset(new SkinScroll());
					if (!skinScroll->CreateScroll(thisWnd, path + attr, zipFile))
						skinScroll.reset();
				}
			}
		}
		// end if (eMain)
	}
	else
		return false;

	if (!skinScroll)
		nativeScroll = ::CreateWindowEx(0, WC_SCROLLBAR, L"", WS_CHILD|SBS_VERT, 0, 0, 0, 0, thisWnd, NULL, ::GetModuleHandle(NULL), NULL);

	if (fontNoItems == NULL)
	{
		fontNoItems = ExImage::CreateElementFont(NULL, 0, false, false);
		fontColorNoItems = RGB(100, 100, 100);
	}

	return true;
}

int SkinList::HScrollGetPos()
{
	if (skinScroll)
		return skinScroll->GetScrollPos();
	else
		return ::GetScrollPos(nativeScroll, SB_CTL);
		
}

bool SkinList::HScrollSetPos(int pos, bool needRedraw)
{
	if (skinScroll)
		return skinScroll->SetScrollPos(pos, needRedraw);
	else
	{
		int oldPos = ::GetScrollPos(nativeScroll, SB_CTL);
		if (oldPos == ::SetScrollPos(nativeScroll, SB_CTL, pos, needRedraw))
			return false;
		
		return true;
	}
}

int SkinList::HScrollGetPage()
{
	if (skinScroll)
		return skinScroll->GetScrollPage();
	else
	{
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_PAGE;
		::GetScrollInfo(nativeScroll, SB_CTL, &si);

		return (int)si.nPage;
	}		
}

void SkinList::HScrollSetInfo(int min, int max, int line, int page)
{
	if (skinScroll)
	{
		// Get number of lines to scroll from Windows settings
		UINT lineScroll = 3; // default value
		SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &lineScroll, 0);

		skinScroll->SetScrollInfo(min, max, line * lineScroll, page);
	}
	else
	{
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_PAGE|SIF_RANGE;
		si.nMin = min;
		si.nMax = max - 1;
		si.nPage = page;

		::SetScrollInfo(nativeScroll, SB_CTL, &si, TRUE);

		// iMax - 1 and below + 1 because:
		// The SetScrollInfo function performs range checking on the values specified by the nPage and nPos
		// members of the SCROLLINFO structure. The nPage member must specify a value from 0 to nMax - nMin +1.
		// The nPos member must specify a value between nMin and nMax - max( nPage- 1, 0). If either value
		// is beyond its range, the function sets it to a value that is just within the range.
		// SetScrollInfo: http://msdn.microsoft.com/en-us/library/bb787595(VS.85).aspx
		// i.e. we compensate this 1 pixel otherwise for example SmoothScroll never reach the end on 1 pixel.
	}
}

int SkinList::HScrollGetMax()
{
	if (skinScroll)
		return skinScroll->GetScrollMax();
	else
	{
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_RANGE;
		::GetScrollInfo(nativeScroll, SB_CTL, &si);

		return (int)si.nMax + 1;
	}
}

int SkinList::HScrollGetMin()
{
	if (skinScroll)
		return skinScroll->GetScrollMin();
	else
	{
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_RANGE;
		::GetScrollInfo(nativeScroll, SB_CTL, &si);

		return (int)si.nMin;
	}
}

int SkinList::HScrollGetWidth()
{
	if (skinScroll)
		return skinScroll->GetScrollWidth();
	else
		return GetSystemMetrics(SM_CXVSCROLL);
}

void SkinList::HScrollShow(bool show)
{
	if (skinScroll)
	{
		skinScroll->ShowScroll(show);
	}
	else if (nativeScroll)
	{
		::ShowScrollBar(nativeScroll, SB_CTL, show);
	}
}

void SkinList::HScrollResize(int x, int y, int cx, int cy)
{
	if (skinScroll)
	{
		::MoveWindow(skinScroll->Wnd(), x, y, cx, cy, FALSE);
	}
	else if (nativeScroll)
	{
		::MoveWindow(nativeScroll, x, y, cx, cy, FALSE);
	}
}

bool SkinList::HScrollMessage(int message)
{
	if (skinScroll)
	{
		if (message == SB_THUMBPOSITION)
			return true;
	}
	else
	{
		// Get number of lines to scroll from Windows settings
		UINT lineScroll = 3; // default value
		SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &lineScroll, 0);

		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_POS|SIF_TRACKPOS|SIF_RANGE|SIF_PAGE;
		::GetScrollInfo(nativeScroll, SB_CTL, &si);

		int posScroll = (int)si.nPos;

		switch (message)
		{
			case SB_LINEUP:
				posScroll -= trackHeight * lineScroll;
			break;

			case SB_LINEDOWN:
				posScroll += trackHeight * lineScroll;
			break;

			case SB_PAGEUP:
				posScroll -= (int)si.nPage;
			break;

			case SB_PAGEDOWN:
			    posScroll += (int)si.nPage;
			break;

			case SB_TOP:
			    posScroll = (int)si.nMin;
			break;

			case SB_BOTTOM:
				posScroll = (int)si.nMax;
			break;

		    case SB_THUMBTRACK:
			case SB_THUMBPOSITION:
				posScroll = (int)si.nTrackPos;
	    }

		if (HScrollSetPos(posScroll))
			return true;
	}

	return false;
}

int SkinList::HScrollMessageGetPosOld()
{
	if (skinScroll)
		return skinScroll->GetScrollPosOld();
	else
		return ::GetScrollPos(nativeScroll, SB_CTL);
}

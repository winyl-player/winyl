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

// SkinTree.cpp : implementation file
//

#include "stdafx.h"
#include "SkinTree.h"


// SkinTree

SkinTree::SkinTree()
{
	rootNode.reset(new SkinTreeNode());
}

SkinTree::~SkinTree()
{

}

LRESULT SkinTree::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
	case WM_MBUTTONDOWN:
		OnMButtonDown((UINT)wParam, CPoint(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
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

bool SkinTree::NewWindow(HWND parent)
{
	if (CreateClassWindow(parent, L"SkinTree", WS_CHILDWINDOW|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, 0, true)) //WS_EX_COMPOSITED
	{
		return true;
	}

	return false;
}

void SkinTree::SetControlRedraw(bool isRedraw)
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

TreeNodeUnsafe SkinTree::InsertHead(TreeNodeUnsafe parent, const std::wstring& label, SkinTreeNode::Type type, bool isShowOpen, bool isOpen)
{
	if (parent == nullptr)
		parent = rootNode.get();

	TreeNodeUnsafe newNode = new SkinTreeNode();

	if (isHeadUpperCase)
	{
		std::wstring labelUpper = StringEx::ToUpper(label);
		newNode->labels.push_back(labelUpper);
	}
	else
		newNode->labels.push_back(label);
	newNode->isType = true;

	newNode->place = 0;

	newNode->nodeType = SkinTreeNode::NodeType::Head;
	newNode->type = type;

	newNode->isShowOpen = isShowOpen;
	newNode->isOpen = isOpen;

	if (newNode->type == SkinTreeNode::Type::Playlist)
		playlistNode.reset(newNode);
	else if (newNode->type == SkinTreeNode::Type::Smartlist)
		smartlistNode.reset(newNode);

	parent->AddChild(newNode);

	SetControlRedraw(isControlRedraw);

	return newNode;
}

TreeNodeUnsafe SkinTree::InsertNode(TreeNodeUnsafe parent, const std::wstring& label, const std::wstring& value)
{
	if (parent == nullptr)
		parent = rootNode.get();

	parent->isShowOpen = true;

	TreeNodeUnsafe newNode = new SkinTreeNode();

    newNode->labels.push_back(label);
	newNode->value = value;
	newNode->isValue = true;

	newNode->place = parent->place + 1;

	newNode->nodeType = SkinTreeNode::NodeType::Text;
	newNode->type = parent->type;

	if (newNode->type == SkinTreeNode::Type::Album)
		newNode->isShowOpen = false;

	parent->AddChild(newNode);

	SetControlRedraw(isControlRedraw);

	return newNode;
}

TreeNodeUnsafe SkinTree::InsertAlbum(TreeNodeUnsafe parent, const std::wstring& label, const std::wstring& album)
{
	if (parent == nullptr)
		parent = rootNode.get();

	parent->isShowOpen = true;

	TreeNodeUnsafe newNode = new SkinTreeNode();

    newNode->labels.push_back(label);
	newNode->album = album;
	newNode->isAlbum = true;
	newNode->artist = parent->artist;
	newNode->value = parent->value;

	newNode->place = parent->place + 1;

	newNode->nodeType = SkinTreeNode::NodeType::Text;
	newNode->type = SkinTreeNode::Type::Album;

	newNode->isShowOpen = false;

	parent->AddChild(newNode);

	SetControlRedraw(isControlRedraw);

	return newNode;
}

TreeNodeUnsafe SkinTree::InsertArtist(TreeNodeUnsafe parent, const std::wstring& label, const std::wstring& artist)
{
	if (parent == nullptr)
		parent = rootNode.get();

	parent->isShowOpen = true;

	TreeNodeUnsafe newNode = new SkinTreeNode();

    newNode->labels.push_back(label);
	newNode->artist = artist;
	newNode->isArtist = true;
	newNode->value = parent->value;

	newNode->place = parent->place + 1;

	newNode->nodeType = SkinTreeNode::NodeType::Text;
	newNode->type = SkinTreeNode::Type::Artist;
	newNode->value = parent->value;

	newNode->isShowOpen = true;

	parent->AddChild(newNode);

	SetControlRedraw(isControlRedraw);

	return newNode;
}

TreeNodeUnsafe SkinTree::InsertRadio(TreeNodeUnsafe parent, const std::wstring& label, const std::wstring& value)
{
	if (parent == nullptr)
		parent = rootNode.get();

	parent->isShowOpen = true;

	TreeNodeUnsafe newNode = new SkinTreeNode();

    newNode->labels.push_back(label);

	newNode->place = parent->place + 1;

	newNode->nodeType = SkinTreeNode::NodeType::Text;
	newNode->type = SkinTreeNode::Type::Radio;
	newNode->value = value;
	newNode->isValue = true;

	newNode->isShowOpen = false;

	parent->AddChild(newNode);

	SetControlRedraw(isControlRedraw);

	return newNode;
}

TreeNodeUnsafe SkinTree::InsertPlaylist(TreeNodeUnsafe parent, const std::wstring& label, const std::wstring& value, bool insert, bool isDefault)
{
	if (parent == nullptr)
		parent = rootNode.get();

	parent->isShowOpen = true;

	TreeNodeUnsafe newNode = new SkinTreeNode();

    newNode->labels.push_back(label);
	newNode->value = value;
	newNode->isValue = true;

	newNode->place = parent->place + 1;

	newNode->nodeType = SkinTreeNode::NodeType::Text;
	newNode->type = SkinTreeNode::Type::Playlist;

	newNode->isShowOpen = false;

	if (isDefault)
		defPlaylistNode.reset(newNode);

	if (!insert)
		parent->AddChild(newNode);
	else
		parent->InsertChild(newNode);

	SetControlRedraw(isControlRedraw);

	return newNode;
}

TreeNodeUnsafe SkinTree::InsertSmartlist(TreeNodeUnsafe parent, const std::wstring& label, const std::wstring& value)
{
	if (parent == nullptr)
		parent = rootNode.get();

	parent->isShowOpen = true;

	TreeNodeUnsafe newNode = new SkinTreeNode();

    newNode->labels.push_back(label);
	newNode->value = value;
	newNode->isValue = true;

	newNode->place = parent->place + 1;

	newNode->nodeType = SkinTreeNode::NodeType::Text;
	newNode->type = SkinTreeNode::Type::Smartlist;

	newNode->isShowOpen = false;

	parent->AddChild(newNode);

	SetControlRedraw(isControlRedraw);

	return newNode;
}

TreeNodeUnsafe SkinTree::InsertFolder(TreeNodeUnsafe parent, const std::wstring& label, const std::wstring& value)
{
	if (parent == nullptr)
		parent = rootNode.get();

	parent->isShowOpen = true;

	TreeNodeUnsafe newNode = new SkinTreeNode();

    newNode->labels.push_back(label);
	newNode->value = value;
	newNode->isValue = true;

	newNode->place = parent->place + 1;

	newNode->nodeType = SkinTreeNode::NodeType::Text;
	newNode->type = SkinTreeNode::Type::Folder;

	newNode->isShowOpen = false;

	parent->isShowOpen = true;

	parent->AddChild(newNode);

	SetControlRedraw(isControlRedraw);

	return newNode;
}

void SkinTree::ClearLibrary()
{
	bool isNeedRedraw = false;
	bool isNeedUpdate = false;

	if (focusNode && focusNode->nodeType == SkinTreeNode::NodeType::Text)
	{
		// Clear selection if focus node is related to the library
		if (focusNode->type != SkinTreeNode::Type::NowPlaying &&
			focusNode->type != SkinTreeNode::Type::Radio &&
			focusNode->type != SkinTreeNode::Type::Smartlist &&
			focusNode->type != SkinTreeNode::Type::Playlist)
		{
			focusNode->isSelect = false;
			focusNode->stateSelect = SkinTreeNode::StateFlag::Normal;
			focusNode.reset();
		}
	}

	// Close and remove all nodes related to the library
	if (rootNode->HasChild())
	{
		for (TreeNodeUnsafe node = rootNode->Child(); node != nullptr; node = node->Next())
		{
			if (node->type != SkinTreeNode::Type::NowPlaying &&
				node->type != SkinTreeNode::Type::Radio &&
				node->type != SkinTreeNode::Type::Smartlist &&
				node->type != SkinTreeNode::Type::Playlist)
			{
				if (node->isOpen != false || node->isShowOpen != true)
				{
					node->isOpen = false;
					node->isShowOpen = true;
					isNeedRedraw = true;
				}

				if (node->HasChild())
				{
					node->EmptyChild();
					isNeedUpdate = true;
					isNeedRedraw = true;
				}
			}
		}
	}

	if (isNeedUpdate)
		ResetScrollBar();

	if (isNeedRedraw)
		::InvalidateRect(thisWnd, NULL, FALSE);
}

void SkinTree::DeleteAllNode()
{
	dropNodeMove.reset();

	StopSmoothScroll();

	visibleNodes.clear();

	focusNode.reset();
	playlistNode.reset();
	smartlistNode.reset();
	defPlaylistNode.reset();

	rootNode.reset(new SkinTreeNode());

	HScrollSetPos(0, false);

	SetControlRedraw(isControlRedraw);
}

void SkinTree::DeleteSelected(TreeNodeUnsafe node)
{
	if (node == defPlaylistNode.get())
		defPlaylistNode.reset();

	// Search for next node (within one parent)
	TreeNodeSafe nextNode;
	if (focusNode)
	{
		if (focusNode->Next())
			nextNode.reset(focusNode->Next());
		else
			nextNode.reset(focusNode->Prev());
	}

	focusNode.reset();

	// Remove node
	node->Parent()->RemoveChild(node);

	// Select next node
	if (nextNode)
	{
		nextNode->isSelect = true;
		nextNode->stateSelect = SkinTreeNode::StateFlag::Focus;

		focusNode = nextNode;
	}

	SetControlRedraw(isControlRedraw);
}

TreeNodeUnsafe SkinTree::AddPlaylist(const std::wstring& label, const std::wstring& value, bool insert, bool isDefault)
{
	TreeNodeUnsafe resultNode = nullptr;

	if (playlistNode)
	{
		// Open "Playlists" when add playlist
		playlistNode->isOpen = true;

		resultNode = InsertPlaylist(playlistNode.get(), label, value, insert, isDefault);

		SetControlRedraw(isControlRedraw);
	}

	return resultNode;
}

TreeNodeUnsafe SkinTree::AddSmartlist(const std::wstring& label, const std::wstring& value)
{
	TreeNodeUnsafe resultNode = nullptr;

	if (smartlistNode)
	{
		// Open "Smartlists" when add smartlist
		smartlistNode->isOpen = true;

		resultNode = InsertSmartlist(smartlistNode.get(), label, value);

		SetControlRedraw(isControlRedraw);
	}

	return resultNode;
}

void SkinTree::OnPaint(HDC dc, PAINTSTRUCT& ps)
{
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
				-rc.left, rc2.right - scrollWidth - rc.left, rc.Height());

			VisibleNodesRecursive(rootNode.get(), -HScrollGetPos(), rc2.Height());
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
}

int SkinTree::VisibleNodesRecursive(SkinTreeNode* recursiveNode, int y, int height)
{
	// If there are no nodes to draw just return
	//if (!recursiveNode->HasChild())
	//	return y;

	for (TreeNodeUnsafe node = recursiveNode->Child(); node != nullptr; node = node->Next())
	{
		if (y > height) // Do not draw nodes that beyond window bottom
			return y;

		if (node->nodeType == SkinTreeNode::NodeType::Text) // Draw node
		{
			if (y > -nodeHeight) // Draw only visible nodes
			{
				visibleNodes.push_back(node); // Fill visible nodes array
			}
		}
		else if (node->nodeType == SkinTreeNode::NodeType::Head) // Draw header
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

	return y;
}

int SkinTree::DrawNodesRecursive(HDC dc, SkinTreeNode* recursiveNode, int x, int y, int left, int right, int height)
{
	// If there are no nodes to draw just return
	//if (!recursiveNode->HasChild())
	//	return y;

	for (TreeNodeUnsafe node = recursiveNode->Child(); node != nullptr; node = node->Next())
	{
		if (y > height) // Do not draw nodes that beyond window bottom
			break; //return y;

		if (node->nodeType == SkinTreeNode::NodeType::Text) // Draw node
		{
			if (y > -nodeHeight) // Draw only visible nodes
			{
				// Node area
				CRect rc(x, y, right, y + nodeHeight);
				CRect rc2(left, y, right, y + nodeHeight);

				// Draw background
				for (std::size_t i = 0, size = skinNodeB.size(); i < size; ++i)
					skinNodeB[i]->Draw(dc, rc2, node->stateSelect, node->stateLine);

				// Draw elements
				for (std::size_t i = 0, size = skinNode.size(); i < size; ++i)
				{	
					if ((int)skinNode[i]->type >= 0)
						skinNode[i]->DrawText(dc, rc, node->labels[(std::size_t)skinNode[i]->type], node->stateSelect, node->stateLine);
					else if (skinNode[i]->type == SkinTreeElement::Type::Expand)
						skinNode[i]->DrawExpand(dc, rc, node->isOpen, node->isShowOpen, node->stateSelect, node->stateLine/*, rcExpand*/);
					else if (skinNode[i]->type == SkinTreeElement::Type::Icon)
						skinNode[i]->DrawIcon(dc, rc, (int)node->type, node->stateSelect, node->stateLine);
				}
			}
		}
		else if (node->nodeType == SkinTreeNode::NodeType::Head) // Draw header
		{
			if (y > -headHeight) // Draw only visible nodes
			{
				// Node area
				CRect rc(x, y, right, y + headHeight);
				CRect rc2(left, y, right, y + headHeight);

				// Draw background
				for (std::size_t i = 0, size = skinHeadB.size(); i < size; ++i)
					skinHeadB[i]->Draw(dc, rc2, node->stateSelect, node->stateLine);

				// Draw elements
				for (std::size_t i = 0, size = skinHead.size(); i < size; ++i)
				{	
					if ((int)skinHead[i]->type >= 0)
						skinHead[i]->DrawText(dc, rc, node->labels[(std::size_t)skinHead[i]->type], node->stateSelect, node->stateLine);
					else if (skinHead[i]->type == SkinTreeElement::Type::Expand)
						skinHead[i]->DrawExpand(dc, rc, node->isOpen, node->isShowOpen, node->stateSelect, node->stateLine/*, rcExpand*/);
					else if (skinHead[i]->type == SkinTreeElement::Type::Icon)
						skinHead[i]->DrawIcon(dc, rc, (int)node->type, node->stateSelect, node->stateLine);
				}
			}
		}


		y += node->rcNode.Height();

///////////////////////////////////////////////////////////////////////////
		// Recursively draw all open subnodes //
		if (node->isOpen && node->HasChild())
			y = DrawNodesRecursive(dc, node, x + nodeIndent, y, left, right, height);
///////////////////////////////////////////////////////////////////////////
	}

	return y;
}

void SkinTree::SetFocusNode(TreeNodeUnsafe node)
{
	// Clear selection
	if (focusNode)
	{
		focusNode->isSelect = false;
		focusNode->stateSelect = SkinTreeNode::StateFlag::Normal;
	}

	// Select new node
	node->isSelect = true;
	node->stateSelect = SkinTreeNode::StateFlag::Focus;
	focusNode.reset(node);

	::InvalidateRect(thisWnd, NULL, FALSE);
	::UpdateWindow(thisWnd);
}

void SkinTree::ExpandNode(TreeNodeUnsafe node)
{
	if (node->isOpen)
		return;

	node->isOpen = !node->isOpen;

	if (!node->HasChild())
		node->isShowOpen = false;

	// Recalculate position of nodes
	ResetScrollBar();
	isControlRedraw = true;

	::InvalidateRect(thisWnd, NULL, FALSE);
}

void SkinTree::OnMButtonDown(UINT nFlags, CPoint point)
{
	int clicked = FindNodeByPoint(point, HScrollGetPos());
	if (clicked > -1)
	{
		TreeNodeUnsafe node = visibleNodes[clicked];

		// Clear selection
		if (focusNode)
		{
			focusNode->isSelect = false;
			focusNode->stateSelect = SkinTreeNode::StateFlag::Normal;
		}

		// Select new node
		node->isSelect = true;
		node->stateSelect = SkinTreeNode::StateFlag::Focus;
		focusNode.reset(node);

		::InvalidateRect(thisWnd, NULL, FALSE);
		::UpdateWindow(thisWnd);

		::SendMessage(thisParentWnd, UWM_FILLLIST, 0, (LPARAM)node);
		::SendMessage(thisParentWnd, UWM_PLAYFILE, 0, 0);
	}
}

void SkinTree::OnLButtonDown(UINT nFlags, CPoint point)
{
	::SetFocus(thisWnd);

	// Search for clicked node
	int clicked = FindNodeByPoint(point, HScrollGetPos());

	// Click on node
	if (clicked > -1)
	{
		TreeNodeUnsafe node = visibleNodes[clicked];

		CRect rc; ::GetClientRect(thisWnd, rc);
		CRect rcNode = node->rcNode;
		int scrollPos = HScrollGetPos();

		rcNode.top -= scrollPos;
		rcNode.bottom -= scrollPos;
		rcNode.left = rc.left + (node->place * nodeIndent);
		rcNode.right = rc.right - scrollWidth;

		// Click on expand
		bool isClickExpand = false;
		if (node->isShowOpen)
		{
			if (node->nodeType == SkinTreeNode::NodeType::Text)
			{
				for (std::size_t i = 0, size = skinNode.size(); i < size; ++i)
				{
					if (skinNode[i]->type == SkinTreeElement::Type::Expand)
						isClickExpand = skinNode[i]->IsClickExpand(point, rcNode, node->stateSelect, node->stateLine);
				}
			}
			else if (node->nodeType == SkinTreeNode::NodeType::Head)
			{
				for (std::size_t i = 0, size = skinHead.size(); i < size; ++i)
				{
					if (skinHead[i]->type == SkinTreeElement::Type::Expand)
						isClickExpand = skinHead[i]->IsClickExpand(point, rcNode, node->stateSelect, node->stateLine);
				}
			}
		}

		if (isClickExpand)
		{
			// Expand or collapse node
			node->isOpen = !node->isOpen;

			if (node->isOpen && !node->HasChild())
				::SendMessage(thisParentWnd, UWM_FILLTREE, 0, (LPARAM)node);

			if (!node->HasChild())
				node->isShowOpen = false;

			// Recalculate position of nodes
			ResetScrollBar();

			::InvalidateRect(thisWnd, NULL, FALSE);
			isControlRedraw = true;

			return;
		}
	}

	// Click on node
	if (clicked > -1)
	{
		TreeNodeUnsafe node = visibleNodes[clicked];

		// Set move node flag
		if (node->nodeType == SkinTreeNode::NodeType::Text &&
			(node->type == SkinTreeNode::Type::Playlist || node->type == SkinTreeNode::Type::Smartlist))
		{
			isPressElement = true;
			ptPress = point;
			::SetCapture(thisWnd);
		}

		// Clear selection
		if (focusNode)
		{
			focusNode->isSelect = false;
			focusNode->stateSelect = SkinTreeNode::StateFlag::Normal;
		}

		// Select new node
		node->isSelect = true;
		node->stateSelect = SkinTreeNode::StateFlag::Focus;
		focusNode.reset(node);

		::InvalidateRect(thisWnd, NULL, FALSE);
		::UpdateWindow(thisWnd);

		::SendMessage(thisParentWnd, UWM_FILLLIST, 0, (LPARAM)node);
	}
	else
	{
		// Clear selection
		if (focusNode)
		{
			focusNode->isSelect = false;
			focusNode->stateSelect = SkinTreeNode::StateFlag::Normal;
		}

		focusNode.reset();
		::InvalidateRect(thisWnd, NULL, FALSE);
	}
}

void SkinTree::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (focusNode)
	{
		int clicked = FindNodeByPoint(point, HScrollGetPos());
		if (clicked > -1)
		{
			TreeNodeUnsafe node = visibleNodes[clicked];

			if (node == focusNode.get())
			{
				// Expand or collapse node
				node->isOpen = !node->isOpen;

				if (node->isOpen && !node->HasChild())
					::SendMessage(thisParentWnd, UWM_FILLTREE, 0, (LPARAM)node);

				if (!node->HasChild())
					node->isShowOpen = false;

				// Recalculate position of nodes
				ResetScrollBar();

				::InvalidateRect(thisWnd, NULL, FALSE);
				isControlRedraw = true;
			}
		}
	}
}

void SkinTree::SetDropPoint(CPoint* point)
{
	if (point)
	{
		// Search for clicked node
		int clicked = FindNodeByPoint((*point), HScrollGetPos());

		// Click on node
		if (clicked > -1)
		{
			TreeNodeUnsafe node = visibleNodes[clicked];

			if (dropNode.get() == node)
				return;

			if ((node->nodeType == SkinTreeNode::NodeType::Text && node->type == SkinTreeNode::Type::Playlist) ||
				(node->nodeType == SkinTreeNode::NodeType::Head && node->type == SkinTreeNode::Type::NowPlaying))
			{
				if (node != dropNode.get()) // Do not select the same node
				{
					// Clear selection
					if (dropNode && (dropNode != focusNode))
					{
						dropNode->isSelect = false;
						dropNode->stateSelect = SkinTreeNode::StateFlag::Normal;
					}

					// Select new node
					node->isSelect = true;
					node->stateSelect = SkinTreeNode::StateFlag::Focus;
					dropNode.reset(node);

					::InvalidateRect(thisWnd, NULL, FALSE);
				}
			}
			else // Select something else other than playlist, clear selection
				point = nullptr;
		}
		else // Select nothing, clear selection
			point = nullptr;
	}

	if (point == nullptr)
	{
		if (dropNode) // Clear selection
		{
			if (dropNode != focusNode) // Do not clear selection from focus node
			{
				dropNode->isSelect = false;
				dropNode->stateSelect = SkinTreeNode::StateFlag::Normal;

				::InvalidateRect(thisWnd, NULL, FALSE);
			}

			dropNode.reset();
		}
	}
}

void SkinTree::OnRButtonDown(UINT nFlags, CPoint point)
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
		TreeNodeUnsafe node = visibleNodes[clicked];

		if (node->type == SkinTreeNode::Type::Playlist ||
			node->type == SkinTreeNode::Type::Smartlist)
		{
			// Clear selection
			if (focusNode)
			{
				focusNode->isSelect = false;
				focusNode->stateSelect = SkinTreeNode::StateFlag::Normal;
			}

			// Select new node
			node->isSelect = true;
			node->stateSelect = SkinTreeNode::StateFlag::Focus;
			focusNode.reset(node);

			::InvalidateRect(thisWnd, NULL, FALSE);
		}
	}
}

void SkinTree::OnContextMenu(HWND hWnd, CPoint point)
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
			::SendMessage(thisParentWnd, UWM_TREEMENU, point.x, point.y);
		}
	}
}

int SkinTree::FindNodeByPoint(const CPoint& point, int scrollPos)
{
	for (std::size_t i = 0, size = visibleNodes.size(); i < size; ++i)
	{
		if (point.y + scrollPos >= visibleNodes[i]->rcNode.top &&
			point.y + scrollPos <= visibleNodes[i]->rcNode.bottom)
			return (int)i;
	}

	return -1;
}

void SkinTree::ResetScrollBar()
{
    CRect rcClient;
	::GetClientRect(thisWnd, rcClient);

	countNodes = 0;
	heightNodes = CalculateHeight(rootNode.get(), 0, 0);

	if (rcClient.Height() < heightNodes/* + 8*/)
	{
		HScrollSetInfo(0, heightNodes, nodeHeight, rcClient.Height());

		scrollWidth = HScrollGetWidth();

		HScrollShow(true);
    }
    else
    {
		HScrollShow(false);

		scrollWidth = 0;

		HScrollSetInfo(0, heightNodes, nodeHeight, rcClient.Height());
    }
}

void SkinTree::OnVScroll(UINT nSBCode, UINT nPos, HWND hScrollBar)
{
	int oldPos = HScrollMessageGetPosOld();
	if (HScrollMessage((int)nSBCode))
	{
		StopSmoothScroll();

		::ScrollWindowEx(thisWnd, 0, oldPos - HScrollGetPos(), NULL, NULL, NULL, NULL, SW_INVALIDATE);
		::UpdateWindow(thisWnd);
	}
}

void SkinTree::OnSize(UINT nType, int cx, int cy)
{
	int scrollW = HScrollGetWidth();

	HScrollResize(cx - scrollW, 0, scrollW, cy);

	if (heightNodes > cy)
	{
		HScrollSetInfo(0, heightNodes, nodeHeight, cy);
        
		scrollWidth = scrollW;

		HScrollShow(true);
	}
	else
	{
		HScrollShow(false);

		scrollWidth = 0;

		HScrollSetInfo(0, heightNodes, nodeHeight, cy);
	}
}

int SkinTree::CalculateHeight(SkinTreeNode* recursiveNode, int y, int nesting)
{
	if (!recursiveNode->HasChild())
		return y;

	for (TreeNodeUnsafe node = recursiveNode->Child(); node != nullptr; node = node->Next())
	{
		if (countNodes % 2 == 0)
			node->stateLine = SkinTreeNode::StateFlag::Line1;
		else
			node->stateLine = SkinTreeNode::StateFlag::Line2;

		countNodes++;

		if (node->nodeType == SkinTreeNode::NodeType::Text)
		{
			node->rcNode.top = y;
			node->rcNode.bottom = y + nodeHeight;
		}
		else if (node->nodeType == SkinTreeNode::NodeType::Head)
		{
			node->rcNode.top = y;
			node->rcNode.bottom = y + headHeight;
		}

		y += node->rcNode.Height();

		if (node->isOpen && node->HasChild())
			y = CalculateHeight(node, y, nesting + 1);
	}

	return y;
}

void SkinTree::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{

}

void SkinTree::MouseWheel(bool isReverse)
{
	UINT lineScroll = 3; // default value
	SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &lineScroll, 0);

	int scroll = 0;

	if (lineScroll == WHEEL_PAGESCROLL)
		scroll = HScrollGetPage();
	else
		scroll = lineScroll * nodeHeight;

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

void SkinTree::StopSmoothScroll()
{
	if (isSmoothScrollEnabled)
	{
		eventSmoothScroll->Reset();
		isSmoothScrollRun = false;
		smoothScrollAcc = 0;
	}
}

void SkinTree::SmoothScroll(int scroll)
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
		isSmoothScrollRun = true;
		eventSmoothScroll->Set();
	}
}

void SkinTree::OnTimer(UINT_PTR nIDEvent)
{

}

void SkinTree::SmoothScrollRun()
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
		::UpdateWindow(thisWnd);
	}

	if ((smoothScrollAcc > 0 && HScrollGetPos() >= smoothScrollNew) ||
		(smoothScrollAcc < 0 && HScrollGetPos() <= smoothScrollNew))
	{
		eventSmoothScroll->Reset();
		isSmoothScrollRun = false;
		smoothScrollAcc = 0;
	}
}

void SkinTree::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (isPressElement)
	{
		isPressElement = false;
		::ReleaseCapture();

		if (isDragElement)
		{
			isDragElement = false;
			::SendMessage(thisParentWnd, UWM_TREEDRAG, 1, 0);
		}

		if (isMoveElement)
		{
			focusNode->Parent()->MoveChild(focusNode.get(), focusNode->Parent(), dropNodeMove.get());

			dropPosMove = 0;
			dropPosScroll = 0;
			dropNodeMove.reset();

			isMoveElement = false;
			ResetScrollBar(); // Recalculate position of nodes
			::InvalidateRect(thisWnd, NULL, FALSE);
			::SendMessage(thisParentWnd, UWM_TREESWAP, 0, 0);
		}
	}


	//else if (focusNode && focusNode->mainType == SkinTreeNode::NODE_HEAD && focusNode->type == SkinTreeNode::TYPE_NOW)
	//{
	//	focusNode->isSelect = false;
	//	focusNode->stateSelect = SkinTreeNode::STATE_NORMAL;
	//	focusNode = nullptr;
	//	Invalidate();
	//}
}

void SkinTree::SetDropMovePoint(CPoint* point)
{
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
		CPoint& pt = (*point);

		TreeNodeUnsafe dropNodeMoveNew = dropNodeMove.get();
		int dropPosMoveNew = dropPosMove;
		pt.y += nodeHeight / 2;
		int scrollPos = HScrollGetPos();
		int scrollPage = HScrollGetPage();

		int found = FindNodeByPoint(pt, scrollPos);
		if (found > -1 && visibleNodes[found]->Parent() == focusNode->Parent())
		{
			dropNodeMoveNew = visibleNodes[found];
			dropPosMoveNew = visibleNodes[found]->rcNode.top - scrollPos;
		}
		else
		{
			if (found > -1 && visibleNodes[found]->rcNode.top < focusNode->Parent()->Child()->rcNode.top)
			{
				dropNodeMoveNew = focusNode->Parent()->Child();
				dropPosMoveNew = focusNode->Parent()->Child()->rcNode.top - scrollPos;
			}
			else // Latest node
			{
				dropNodeMoveNew = nullptr;
				//iDropPosNew = heightNodes - 1 - HScrollGetPos(); // -1 to do not redraw offscreen
				if (focusNode->Parent()->HasChild() && focusNode->Parent()->Child()->Next())
					dropPosMoveNew = focusNode->Parent()->LastChild()->rcNode.bottom - 1 - scrollPos;
			}
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

void SkinTree::SetDropMoveStop()
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

		//::SendMessage(thisParentWnd, UWM_TREEDRAG, 2, 0);
	}
}

void SkinTree::OnMouseMove(UINT nFlags, CPoint point)
{
	if (isPressElement)
	{
		if (isDragElement)
		{
			::SendMessage(thisParentWnd, UWM_TREEDRAG, 0, MAKELPARAM(point.x, point.y));
		}
		// Check the limit we need to move a mouse to initiate Drag'n'Drop
		else if (point.x > ptPress.x + 10 || point.y > ptPress.y + 10 ||
			point.x < ptPress.x - 10 || point.y < ptPress.y - 10)
		{
			isDragElement = true;
		}
	}
}

void SkinTree::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
//	if ((nFlags & (1 << 14))) // Do not repeat while holding a key
//	{
//		CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
//		return;
//	}

	if (nChar == VK_UP)
	{
		TreeNodeUnsafe node = FindPrevNode(focusNode.get(), false);

		if (node && node != focusNode.get())
		{
			if (focusNode)
			{
				focusNode->isSelect = false;
				focusNode->stateSelect = SkinTreeNode::StateFlag::Normal;
			}

			node->isSelect = true;
			node->stateSelect = SkinTreeNode::StateFlag::Focus;

			focusNode.reset(node);


			ScrollToNode(node);
			::InvalidateRect(thisWnd, NULL, FALSE);

			::SendMessage(thisParentWnd, UWM_FILLLIST, 0, (LPARAM)node);
		}
		else // Adjust scroll
		{
			if (ScrollToHome())
				::InvalidateRect(thisWnd, NULL, FALSE);
		}
	}
	else if (nChar == VK_DOWN)
	{
		TreeNodeUnsafe node = nullptr;
		if (!focusNode) // Nothing is selected, search for first node
			node = FindPrevNode(nullptr, false);
		else
			node = FindNextNode(focusNode.get(), false);

		if (node && node != focusNode.get())
		{
			if (focusNode)
			{
				focusNode->isSelect = false;
				focusNode->stateSelect = SkinTreeNode::StateFlag::Normal;
			}

			node->isSelect = true;
			node->stateSelect = SkinTreeNode::StateFlag::Focus;

			focusNode.reset(node);


			ScrollToNode(node);
			::InvalidateRect(thisWnd, NULL, FALSE);

			::SendMessage(thisParentWnd, UWM_FILLLIST, 0, (LPARAM)node);
		}
		else // Adjust scroll
		{
			if (ScrollToEnd())
				::InvalidateRect(thisWnd, NULL, FALSE);
		}
	}
	else if (nChar == VK_PRIOR)
	{
		TreeNodeUnsafe node = FindPrevNode(focusNode.get(), true);

		if (node && node != focusNode.get())
		{
			if (focusNode)
			{
				focusNode->isSelect = false;
				focusNode->stateSelect = SkinTreeNode::StateFlag::Normal;
			}

			node->isSelect = true;
			node->stateSelect = SkinTreeNode::StateFlag::Focus;

			focusNode.reset(node);


			ScrollToNode(node);
			::InvalidateRect(thisWnd, NULL, FALSE);

			::SendMessage(thisParentWnd, UWM_FILLLIST, 0, (LPARAM)node);
		}
		else // Adjust scroll
		{
			if (ScrollToHome())
				::InvalidateRect(thisWnd, NULL, FALSE);
		}
	}
	else if (nChar == VK_NEXT)
	{
		TreeNodeUnsafe node = nullptr;
		if (!focusNode) // Nothing is selected, search for first node
			node = FindPrevNode(nullptr, true);
		else
			node = FindNextNode(focusNode.get(), true);

		if (node && node != focusNode.get())
		{
			if (focusNode)
			{
				focusNode->isSelect = false;
				focusNode->stateSelect = SkinTreeNode::StateFlag::Normal;
			}

			node->isSelect = true;
			node->stateSelect = SkinTreeNode::StateFlag::Focus;

			focusNode.reset(node);


			ScrollToNode(node);
			::InvalidateRect(thisWnd, NULL, FALSE);

			::SendMessage(thisParentWnd, UWM_FILLLIST, 0, (LPARAM)node);
		}
		else // Adjust scroll
		{
			if (ScrollToEnd())
				::InvalidateRect(thisWnd, NULL, FALSE);
		}
	}
	else if (nChar == VK_HOME)
	{
		TreeNodeUnsafe node = FindPrevNode(nullptr, false);

		if (node && node != focusNode.get())
		{
			// Clear selection
			if (focusNode)
			{
				focusNode->isSelect = false;
				focusNode->stateSelect = SkinTreeNode::StateFlag::Normal;
			}

			node->isSelect = true;
			node->stateSelect = SkinTreeNode::StateFlag::Focus;

			focusNode.reset(node);

			ScrollToHome();
			::InvalidateRect(thisWnd, NULL, FALSE);

			::SendMessage(thisParentWnd, UWM_FILLLIST, 0, (LPARAM)node);
		}
		else // Adjust scroll
		{
			if (ScrollToHome())
				::InvalidateRect(thisWnd, NULL, FALSE);
		}
	}
	else if (nChar == VK_END)
	{
		TreeNodeUnsafe node = FindNextNode(nullptr, false);

		if (node && node != focusNode.get())
		{
			// Clear selection
			if (focusNode)
			{
				focusNode->isSelect = false;
				focusNode->stateSelect = SkinTreeNode::StateFlag::Normal;
			}

			node->isSelect = true;
			node->stateSelect = SkinTreeNode::StateFlag::Focus;

			focusNode.reset(node);

			ScrollToEnd();
			::InvalidateRect(thisWnd, NULL, FALSE);

			::SendMessage(thisParentWnd, UWM_FILLLIST, 0, (LPARAM)node);
		}
		else // Adjust scroll
		{
			if (ScrollToEnd())
				::InvalidateRect(thisWnd, NULL, FALSE);
		}
	}

	else if (nChar == VK_RIGHT || nChar == VK_ADD)
	{
		if (focusNode && focusNode->isShowOpen && !focusNode->isOpen)
		{
			// Expand
			focusNode->isOpen = true;

			if (!focusNode->HasChild())
				::SendMessage(thisParentWnd, UWM_FILLTREE, 0, (LPARAM)focusNode.get());

			ResetScrollBar();

			::InvalidateRect(thisWnd, NULL, FALSE);
			isControlRedraw = true;
		}
	}
	else if (nChar == VK_LEFT || nChar == VK_SUBTRACT)
	{
		if (focusNode && focusNode->isShowOpen && focusNode->isOpen)
		{
			// Collapse
			focusNode->isOpen = false;

			ResetScrollBar();

			::InvalidateRect(thisWnd, NULL, FALSE);
			isControlRedraw = true;
		}
	}
}

bool SkinTree::ScrollToEnd()
{
	StopSmoothScroll();
	return HScrollSetPos(HScrollGetMax());
}

bool SkinTree::ScrollToHome()
{
	StopSmoothScroll();
	return HScrollSetPos(HScrollGetMin());
}

bool SkinTree::ScrollToNode(TreeNodeUnsafe node, int indent)
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

TreeNodeUnsafe SkinTree::FindNextNode(TreeNodeUnsafe findNode, bool isPage)
{
	// Search node function, helper for a recursive function below,
	// also stores a variable for the recursive function

	// if findNode == nullptr then the function return the latest node
	// if the function return nullptr then a node searched for is latest

	bool isFind = false;
	TreeNodeUnsafe lastNode = nullptr;

	TreeNodeUnsafe resultNode = FindNextNodeR(rootNode.get(), findNode, isFind, isPage, lastNode);

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

TreeNodeUnsafe SkinTree::FindNextNodeR(TreeNodeUnsafe recursiveNode, TreeNodeUnsafe findNode, bool& isFind, bool isPage, TreeNodeUnsafe& lastNode)
{
	if (!recursiveNode->HasChild())
		return nullptr;

	for (TreeNodeUnsafe node = recursiveNode->Child(); node != nullptr; node = node->Next())
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
					int page = HScrollGetPage() - nodeHeight;

					if (node->rcNode.bottom > findNode->rcNode.top + page)
						return node; // Return next (KEY_PAGEDOWN)
				}
			}

			if (node == findNode) // Found a node
				isFind = true;
		}

		// Recursive search
	    if (node->isOpen && node->HasChild())
		{
			TreeNodeUnsafe tempNode = nullptr;
		    tempNode = FindNextNodeR(node, findNode, isFind, isPage, lastNode);
			if (tempNode)
				return tempNode;
		}
	}

	return nullptr;
}

TreeNodeUnsafe SkinTree::FindPrevNode(TreeNodeUnsafe findNode, bool isPage)
{
	// Search node function, helper for a recursive function below,
	// also stores a variable for the recursive function

	// if findNode == nullptr then the function return the first node
	// if the function return nullptr then a node searched for is first

	TreeNodeUnsafe foundNode = nullptr;
	TreeNodeUnsafe firstNode = nullptr;

	TreeNodeUnsafe resultNode = FindPrevNodeR(rootNode.get(), findNode, foundNode, isPage, firstNode);
	
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

TreeNodeUnsafe SkinTree::FindPrevNodeR(TreeNodeUnsafe recursiveNode, TreeNodeUnsafe findNode, TreeNodeUnsafe& foundNode, bool isPage, TreeNodeUnsafe& firstNode)
{
	if (!recursiveNode->HasChild())
		return nullptr;

	for (TreeNodeUnsafe node = recursiveNode->Child(); node != nullptr; node = node->Next())
	{
		if (firstNode == nullptr) firstNode = node; // firstNode always contains the first track

		if (firstNode == nullptr) // Search for the first track
			return nullptr; // Already found

		if (node == findNode) // Found a node
			return foundNode; // Return previous (KEY_UP)

		if (!isPage) // Store previous
			foundNode = node;
		else // Store previous (one page up)
		{
			int page = HScrollGetPage() - nodeHeight;

			if (findNode->rcNode.bottom > node->rcNode.top + page)
				foundNode = node; // Return previous (KEY_PAGEUP)
		}

		// Recursive search
		if (node->isOpen && node->HasChild())
		{
			TreeNodeUnsafe tempNode = nullptr;
		    tempNode = FindPrevNodeR(node, findNode, foundNode, isPage, firstNode);
			if (tempNode)
				return tempNode;
		}
	}

	return nullptr;
}

bool SkinTree::LoadSkin(std::wstring& file, ZipFile* zipFile)
{
	std::wstring path = PathEx::PathFromFile(file);

	XmlFile xmlFile;

	if (xmlFile.LoadEx(file, zipFile))
	{
		XmlNode xmlMain = xmlFile.RootNode().FirstChild("Library");
		
		if (xmlMain)
		{
			XmlNode xmlNode2 = xmlMain.FirstChild("Node");

			if (xmlNode2)
			{
				xmlNode2.Attribute("Height", &nodeHeight);

				XmlNode xmlNode2Elements = xmlNode2.FirstChild("Elements");

				if (xmlNode2Elements)
				{
					for (XmlNode xmlNode = xmlNode2Elements.FirstChild(); xmlNode; xmlNode = xmlNode.NextChild())
					{
						skinNode.emplace_back(new SkinTreeElement());
						skinNode.back()->LoadSkin(xmlNode, path, zipFile);
					}
				}
				// end if (eNodeElements)

				XmlNode xmlNode2Background = xmlNode2.FirstChild("Background");

				if (xmlNode2Background)
				{
					for (XmlNode xmlNode = xmlNode2Background.FirstChild(); xmlNode; xmlNode = xmlNode.NextChild())
					{
						skinNodeB.emplace_back(new SkinTreeBack());
						skinNodeB.back()->LoadSkin(xmlNode, path, zipFile);
					}
				}
				// end if (eNodeBackground)
			}
			// end if (eNode)


			XmlNode xmlHead = xmlMain.FirstChild("Head");

			if (!xmlHead)
				xmlHead = xmlNode2;

			if (xmlHead)
			{
				xmlHead.Attribute("Height", &headHeight);
				xmlHead.Attribute("UpperCase", &isHeadUpperCase);

				XmlNode xmlHeadElements = xmlHead.FirstChild("Elements");

				if (xmlHeadElements)
				{
					for (XmlNode xmlNode = xmlHeadElements.FirstChild(); xmlNode; xmlNode = xmlNode.NextChild())
					{
						skinHead.emplace_back(new SkinTreeElement());
						skinHead.back()->LoadSkin(xmlNode, path, zipFile);
					}
				}
				// end if (eHeadElements)

				XmlNode xmlHeadBackground = xmlHead.FirstChild("Background");

				if (xmlHeadBackground)
				{
					for (XmlNode xmlNode = xmlHeadBackground.FirstChild(); xmlNode; xmlNode = xmlNode.NextChild())
					{
						skinHeadB.emplace_back(new SkinTreeBack());
						skinHeadB.back()->LoadSkin(xmlNode, path, zipFile);
					}
				}
				// end if (eHeadBackground)
			}
			// end if (eHead)

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
	
	return true;
}

int SkinTree::HScrollGetPos()
{
	if (skinScroll)
		return skinScroll->GetScrollPos();
	else
		return ::GetScrollPos(nativeScroll, SB_CTL);
		
}

bool SkinTree::HScrollSetPos(int pos, bool needRedraw)
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

int SkinTree::HScrollGetPage()
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

void SkinTree::HScrollSetInfo(int min, int max, int line, int page)
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

int SkinTree::HScrollGetMax()
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

int SkinTree::HScrollGetMin()
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

int SkinTree::HScrollGetWidth()
{
	if (skinScroll)
		return skinScroll->GetScrollWidth();
	else
		return GetSystemMetrics(SM_CXVSCROLL);
}

void SkinTree::HScrollShow(bool show)
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

void SkinTree::HScrollResize(int x, int y, int cx, int cy)
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

bool SkinTree::HScrollMessage(int message)
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
				posScroll -= nodeHeight * lineScroll;
			break;

			case SB_LINEDOWN:
				posScroll += nodeHeight * lineScroll;
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

int SkinTree::HScrollMessageGetPosOld()
{
	if (skinScroll)
		return skinScroll->GetScrollPosOld();
	else
		return ::GetScrollPos(nativeScroll, SB_CTL);
}

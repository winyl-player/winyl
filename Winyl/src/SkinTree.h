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

#pragma once

// SkinTree

#include "WindowEx.h"
#include "ExImage.h"
#include "SkinTreeNode.h"
#include "SkinTreeBack.h"
#include "SkinTreeElement.h"
#include "SkinScroll.h"
#include "Threading.h"

class SkinTree : public WindowEx
{

public:
	SkinTree();
	virtual ~SkinTree();

	bool NewWindow(HWND parent);
	bool LoadSkin(std::wstring& file, ZipFile* zipFile);
	void SetControlRedraw(bool isRedraw);

	TreeNodeUnsafe InsertHead(TreeNodeUnsafe parent, const std::wstring& label, SkinTreeNode::Type type, bool isShowOpen = true, bool isOpen = false);
	TreeNodeUnsafe InsertNode(TreeNodeUnsafe parent, const std::wstring& label, const std::wstring& value);
	TreeNodeUnsafe InsertAlbum(TreeNodeUnsafe parent, const std::wstring& label, const std::wstring& album);
	TreeNodeUnsafe InsertArtist(TreeNodeUnsafe parent, const std::wstring& label, const std::wstring& artist);
	TreeNodeUnsafe InsertPlaylist(TreeNodeUnsafe parent, const std::wstring& label, const std::wstring& value, bool insert = false, bool isDefault = false);
	TreeNodeUnsafe InsertSmartlist(TreeNodeUnsafe parent, const std::wstring& label, const std::wstring& value);
	TreeNodeUnsafe InsertRadio(TreeNodeUnsafe parent, const std::wstring& label, const std::wstring& value);
	TreeNodeUnsafe InsertFolder(TreeNodeUnsafe parent, const std::wstring& label, const std::wstring& value);

	inline TreeNodeUnsafe GetFocusNode() {return focusNode.get();}
	inline TreeNodeUnsafe GetPlaylistNode() {return playlistNode.get();}
	inline TreeNodeUnsafe GetSmartlistNode() {return smartlistNode.get();}
	inline TreeNodeUnsafe GetDefPlaylistNode() {return defPlaylistNode.get();}
	inline TreeNodeUnsafe GetRootNode() {return rootNode.get();}
	inline void SetDefPlaylistNode(TreeNodeUnsafe node) {defPlaylistNode.reset(node);}

	void DeleteSelected(TreeNodeUnsafe node);
	void DeleteAllNode();
	void ClearLibrary();

	TreeNodeUnsafe AddPlaylist(const std::wstring& label, const std::wstring& value, bool insert = false, bool isDefault = false);
	TreeNodeUnsafe AddSmartlist(const std::wstring& label, const std::wstring& value);

	void SetFocusNode(TreeNodeUnsafe node);
	void ExpandNode(TreeNodeUnsafe node);
	inline void ScrollToFocusNode()
	{
		if (!focusNode)
			return;
		else if (ScrollToNode(focusNode.get(), 100))
			::InvalidateRect(thisWnd, NULL, FALSE);
	}
	inline void ScrollToMyNode(TreeNodeUnsafe myNode)
	{
		if (myNode == nullptr)
			return;
		else if (ScrollToNode(myNode, 100))
			::InvalidateRect(thisWnd, NULL, FALSE);
	}

	void SetDropPoint(CPoint* point);
	TreeNodeUnsafe GetDropNode() {return dropNode.get();}
	void SetDropMovePoint(CPoint* point);
	void SetDropMoveStop();

	const std::wstring& GetNodeTitle(TreeNodeUnsafe node) {return node->labels[0];}
	inline void SetNodeTitle(TreeNodeUnsafe node, const std::wstring& title)
	{
		if (node)
		{
			node->labels[0] = title;
			::InvalidateRect(thisWnd, NULL, FALSE);
			::UpdateWindow(thisWnd);
		}
	}

	void MouseWheel(bool isReverse);

	inline void EnableSmoothScroll(bool enable) {isSmoothScrollEnabled = enable;}
	inline void SetEventSmoothScroll(Threading::Event* event) {eventSmoothScroll = event;}
	inline bool IsSmoothScrollRun() {return isSmoothScrollRun;}
	void SmoothScrollRun();

private:
	int nodeHeight = 0;
	int headHeight = 0;
	int nodeIndent = 10; // 12 maybe?
	unsigned backColor = 0x00FFFFFF;
	int countNodes = 0;
	int heightNodes = 0;

	TreeNodeSafe rootNode;
	TreeNodeSafe focusNode;
	TreeNodeSafe playlistNode;
	TreeNodeSafe smartlistNode;
	TreeNodeSafe dropNode;
	TreeNodeSafe defPlaylistNode;

	int dropPosMove = 0;
	int dropPosScroll = 0;
	TreeNodeSafe dropNodeMove;

	bool isMoveElement = false;
	bool isDragElement = false;

	CPoint ptPress;
	bool isPressElement = false;

	ExImage imageBack;
	unsigned dropColor = 0x00000000;

	bool isHeadUpperCase = false;

	bool isControlRedraw = true;

	HWND nativeScroll = NULL; // Native Windows scroll
	std::unique_ptr<SkinScroll> skinScroll; // Skin scroll, if nullptr use native
	int scrollWidth = 0; // Scroll width, if 0 then scroll is hidden

	bool ScrollToNode(TreeNodeUnsafe node, int indent = 0);
	bool ScrollToHome();
	bool ScrollToEnd();

	// Helper functions for scroll (native and skin)
	// The class interact with scroll throught these functions.
	// These functions does not interact with class only with scroll,
	// only exception in HScrollMessage when native scroll.
	int HScrollGetPos();
	bool HScrollSetPos(int pos, bool needRedraw = true);
	int HScrollGetPage();
	void HScrollSetInfo(int min, int max, int line, int page);
	int HScrollGetMax();
	int HScrollGetMin();
	int HScrollGetWidth();
	void HScrollShow(bool show);
	void HScrollResize(int x, int y, int cx, int cy);
	bool HScrollMessage(int message);
	int HScrollMessageGetPosOld();

	// Smooth scrolling
	bool isSmoothScrollEnabled = true;
	int smoothScrollAcc = 0;
	int smoothScrollNew = 0;
	std::atomic<bool> isSmoothScrollRun = false;
	Threading::Event* eventSmoothScroll = nullptr;
	void SmoothScroll(int scroll);
	void StopSmoothScroll();


	std::vector<std::unique_ptr<SkinTreeElement>> skinNode;
	std::vector<std::unique_ptr<SkinTreeBack>> skinNodeB;

	std::vector<std::unique_ptr<SkinTreeElement>> skinHead;
	std::vector<std::unique_ptr<SkinTreeBack>> skinHeadB;

	std::vector<TreeNodeUnsafe> visibleNodes;

	TreeNodeUnsafe FindNextNode(TreeNodeUnsafe findNode, bool isPage = false);
	TreeNodeUnsafe FindNextNodeR(TreeNodeUnsafe recursiveNode, TreeNodeUnsafe findNode, bool& isFind, bool isPage, TreeNodeUnsafe& lastNode);
	TreeNodeUnsafe FindPrevNode(TreeNodeUnsafe findNode, bool isPage = false);
	TreeNodeUnsafe FindPrevNodeR(TreeNodeUnsafe recursiveNode, TreeNodeUnsafe findNode, TreeNodeUnsafe& foundNode, bool isPage, TreeNodeUnsafe& firstNode);

	int DrawNodesRecursive(HDC dc, SkinTreeNode* pNodeMain, int x, int y, int left, int right, int height);
	int VisibleNodesRecursive(SkinTreeNode* recursiveNode, int y, int height);

	int FindNodeByPoint(const CPoint& point, int scrollPos);
	void ResetScrollBar();
	int CalculateHeight(SkinTreeNode* recursiveNode, int y, int nesting);


private: // Messages
	LRESULT WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) override;
	void OnPaint(HDC dc, PAINTSTRUCT& ps);
	void OnTimer(UINT_PTR nIDEvent);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnSize(UINT nType, int cx, int cy);
	void OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	void OnVScroll(UINT nSBCode, UINT nPos, HWND hScrollBar);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnRButtonDown(UINT nFlags, CPoint point);
	void OnMButtonDown(UINT nFlags, CPoint point);
	void OnContextMenu(HWND hWnd, CPoint point);
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

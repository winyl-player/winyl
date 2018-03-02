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

// SkinList

#include <random>
#include <chrono>
#include "WindowEx.h"
#include "ExImage.h"
#include "SkinListNode.h"
#include "SkinListElement.h"
#include "SkinListBack.h"
#include "SkinListThread.h"
#include "SkinScroll.h"

class SkinList : public WindowEx
{

public:
	SkinList();
	virtual ~SkinList();

	bool NewWindow(HWND parent);
	bool LoadSkin(std::wstring& file, ZipFile* zipFile);
	void SetControlRedraw(bool isRedraw);

	ListNodeUnsafe InsertHead(ListNodeUnsafe parent, const std::wstring& file);
	ListNodeUnsafe InsertTrack(ListNodeUnsafe parent, const std::wstring& file, long long idLibrary, long long idPlaylist,
		int rating, int time, unsigned size, long long cue, bool isSelect = false);
	ListNodeUnsafe InsertTrackToNowPlaying(ListNodeUnsafe parent, const std::wstring& file, long long idLibrary, long long idPlaylist,
		int rating, int time, unsigned size, long long cue);
	void SetNodeString(ListNodeUnsafe node, SkinListElement::Type type, const std::wstring& label);
	void SetNodeString2(ListNodeUnsafe node, SkinListElement::Type type, const std::wstring& label);
	void SetLeftString(ListNodeUnsafe node, SkinListElement::Type type, const std::wstring& label);
	void SetRightString(ListNodeUnsafe node, SkinListElement::Type type, const std::wstring& label);

	void DeleteAllNode(); // Warning! Can invalidate unsafe nodes.
	void DeleteSelected(); // Warning! Can invalidate unsafe nodes.

	std::vector<std::unique_ptr<SkinListElement>> skinTrack;
	std::vector<std::unique_ptr<SkinListBack>> skinTrackB;

	std::vector<std::unique_ptr<SkinListElement>> skinHead;
	std::vector<std::unique_ptr<SkinListBack>> skinHeadB;
	
	std::vector<std::unique_ptr<SkinListElement>> skinLeft;
	std::vector<std::unique_ptr<SkinListBack>> skinLeftB;

	std::vector<std::unique_ptr<SkinListElement>> skinRight;
	std::vector<std::unique_ptr<SkinListBack>> skinRightB;

	ListNodeUnsafe FindNextTrack(ListNodeUnsafe findNode, bool fromCurrentList = false);
	ListNodeUnsafe FindPrevTrack(ListNodeUnsafe findNode, bool fromCurrentList = false);
	ListNodeUnsafe FindNextTrackShuffle();
	ListNodeUnsafe FindPrevTrackShuffle();
	void AddShuffle(ListNodeUnsafe node);
	void RemoveShuffle();

	inline ListNodeUnsafe GetPlayNode() {return playNode.get();}
	inline ListNodeUnsafe GetFocusNode() {return focusNode.get();}
	inline ListNodeUnsafe GetFocusNodePlay()
	{
		if (!rootNodePlay ||
			rootNodePlay == rootNode)
			return focusNode.get();

		return focusNodePlay.get();
	}
	
	void SetFocusNode(ListNodeUnsafe node, bool needRedraw = true);
	void SetPlayNode(ListNodeUnsafe node, bool needRedraw = true);
	void SetPlayRating(int rating);
	inline int GetPlayRating() {return (playNode ? playNode->rating : 0);}
	void SetPause(bool isPause);
	
	void RemoveSelection();
	inline void EnablePlayFocus(bool isEnable) {isPlayFocus = isEnable;}

	inline ListNodeUnsafe GetRootNode() {return rootNode.get();}
	inline void EnableRadio(bool isEnable) {isRadioEnabled = isEnable;}
	inline void EnableSwap(bool isEnable) {isSwapEnabled = isEnable;}
	inline void SetViewPlaylist(bool isView) {isViewPlaylist = isView;}

	inline bool IsRadio() {return isRadioEnabled;}

	void UpdateCovers();

	inline void ScrollToFocusNode()
	{
		if (!focusNode)
			return;
		else if (ScrollToNode(focusNode.get(), 100))
			::InvalidateRect(thisWnd, NULL, FALSE);
	}
	inline void ScrollToMyNode(ListNodeUnsafe myNode)
	{
		if (myNode == nullptr)
			return;
		else if (ScrollToNode(myNode, 100))
			::InvalidateRect(thisWnd, NULL, FALSE);
	}
	inline void ScrollToPlayNode()
	{
		if (!rootNodePlay || rootNodePlay == rootNode)
		{
			if (!playNode)
				return;
			else if (ScrollToNode(playNode.get(), 100))
				::InvalidateRect(thisWnd, NULL, FALSE);
		}
	}

	void SelectAll();

	void MouseWheel(bool isReverse);

	int GetTracksCount() {return countNodes;}

	inline std::size_t GetSelectedSize() {return (int)selectedNodes.size();}
	inline ListNodeUnsafe GetSelectedAt(std::size_t index) {return selectedNodes[index].get();}

	void ResetIndex(bool isNowPlaying = false);

	void DeleteNowPlaying(); // Warning! Can invalidate unsafe nodes.
	void NewNowPlaying();
	void DeleteAllNodePlay(bool isReturnToDefault); // Warning! Can invalidate unsafe nodes.
	void NowPlayingList();
	inline bool IsNowPlayingOpen() {return (rootNodePlay && (rootNodePlay == rootNode));}
	inline bool IsNowPlaying() {return (rootNodePlay ? true : false);}

//	inline ListNodeUnsafe GetRootNodeNowPlaying() {return rootNodePlay;}

	inline void EnableSmoothScroll(bool isEnable) {isSmoothScrollEnabled = isEnable;}
	inline void SetEventSmoothScroll(Threading::Event* event) {eventSmoothScroll = event;}
	inline bool IsSmoothScrollRun() {return isSmoothScrollRun;}
	void SmoothScrollRun();

	ListNodeUnsafe FindNodeByIndex(long long index);
	ListNodeUnsafe FindNodeByIndexR(ListNodeUnsafe recursiveNode, long long index);

	void SetDropMovePoint(CPoint* point);
	void SetDropMoveStop();

	inline void SetTempNode(ListNodeUnsafe node) {tempNextNode.reset(node);}
	inline ListNodeUnsafe GetTempNode() {return tempNextNode.get();}

	inline ListNodeUnsafe GetLastPlayNode() {return lastPlayNode.get();}

	inline void SetNoItemsString(const std::wstring& str) {stringNoItems = str;}

	void CalculateSelectedNodes(int& outCount, int& outTotal, int& outTotalTime, long long& outTotalSize);

private:
	int countNodes = 0; // Total number of tracks, only tracks!
	int heightNodes = 0; // Total height of all nodes
	int nodeIndent = 0;
	unsigned backColor = 0x00FFFFFF;
	int totalTime = 0;
	long long totalSize = 0;

	std::mt19937 randomEngine;

    ListNodeSafe rootNode; // Root node
	ListNodeSafe rootNodePlay; // Root node of "Now Playing" view
	ListNodeSafe focusNode; // Focused node
	ListNodeSafe playNode; // Playing node

	ListNodeSafe shiftNode;

	// Variables below used to restore "Now Playing" view
	//ListNodeSafe rootNodePlay; // Root node of "Now Playing" view
	ListNodeSafe focusNodePlay;
	std::vector<ListNodeSafe> selectedNodesPlay;
	int scrollPosPlay = 0;
	int countNodesPlay = 0;
	bool isSwapEnabledPlay = false;
	bool isViewPlaylistPlay = false;
	bool isRadioEnabledPlay = false;
	int totalTimePlay = 0;
	long long totalSizePlay = 0;

	ListNodeSafe tempNextNode;
	ListNodeSafe lastPlayNode;

	bool isPlayFocus = false;

	bool isSwapEnabled = false;
	bool isViewPlaylist = false;
	bool isRadioEnabled = false;

	bool isSelectedElement = false;

	int dropPosMove = 0;
	int dropPosScroll = 0;
	ListNodeSafe dropNodeMove;
	bool isDragElement = false;
	bool isMoveElement = false;
//	bool isSwapElement = false;
	
	CPoint ptPress;
	bool isPressElement = false;

	SkinListThread listThread;

	bool isControlRedraw = true;

	HFONT fontNoItems = NULL;
	unsigned fontColorNoItems = 0x00000000;
	std::wstring stringNoItems;
	unsigned dropColor = 0x00000000;

	bool isHeadUpperCase = false;

	HWND nativeScroll = NULL; // Native Windows scroll
	std::unique_ptr<SkinScroll> skinScroll; // Skin scroll, if nullptr use native
	int scrollWidth = 0; // Scroll width, if 0 then scroll is hidden

	bool ScrollToNode(ListNodeUnsafe node, int indent = 0);
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

	int trackHeight = 0; // Track height
	int headHeight = 0; // Header height

	int leftHeight = 0; // Left pane height
	int leftWidth = 0; // Left pane width
	
	int rightHeight = 0; // Right pane height
	int rightWidth = 0; // Right pane width

	bool isLeftShow = false; // Show left pane?
	bool isRightShow = false; // Show right pane?
	bool isLeftAlways = true; // Always show left pane?
	bool isRightAlways = true; // Always show right pane?

	bool isLeftOver = false; // Left pane overlap tracks?
	bool isRightOver = false; // Right pane overlap tracks?

	ExImage imageBack; // Background image (expiremental)

	std::vector<ListNodeUnsafe> visibleNodes;

	std::vector<ListNodeSafe> selectedNodes;

	std::vector<ListNodeSafe> shuffleNodes;
	std::size_t shuffleIndex = 0;

	int CalculateHeight(SkinListNode* recursiveNode, int y, int nesting);
	int FindNodeByPoint(const CPoint& point, int scrollPos);
	void ResetScrollBar();
	int DrawNodesRecursive(HDC dc, SkinListNode* recursiveNode, int x, int y, int right, int height);
	int VisibleNodesRecursive(SkinListNode* recursiveNode, int y, int height);

	ListNodeUnsafe FindNextNode(ListNodeUnsafe findNode, bool isPage = false, ListNodeUnsafe recursiveNode = nullptr);
	ListNodeUnsafe FindPrevNode(ListNodeUnsafe findNode, bool isPage = false, ListNodeUnsafe recursiveNode = nullptr);
	ListNodeUnsafe FindNextNodeR(ListNodeUnsafe recursiveNode, ListNodeUnsafe findNode, bool& isFind, bool isPage, ListNodeUnsafe& lastNode);
	ListNodeUnsafe FindPrevNodeR(ListNodeUnsafe recursiveNode, ListNodeUnsafe findNode, ListNodeUnsafe& foundNode, bool isPage, ListNodeUnsafe& firstNode);
	ListNodeUnsafe GetRandomNodeR(ListNodeUnsafe recursiveNode, int* count, int random);

	void SelectShift(ListNodeUnsafe startNode, ListNodeUnsafe endNode);
	bool SelectShiftR(ListNodeUnsafe recursiveNode, ListNodeUnsafe startNode, ListNodeUnsafe endNode, bool &isFind);
	void SelectAllR(SkinListNode* recursiveNode);

	void ClearCoversR(ListNodeUnsafe recursiveNode);


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
	void OnContextMenu(HWND hWnd, CPoint point);
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

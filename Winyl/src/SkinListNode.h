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

// SkinListNode

#include "ExImage.h"
#include "SkinListElement.h"

class SkinListNode final
{
public:
	SkinListNode();
	~SkinListNode();
	friend class SkinList;
	friend class SkinListElement;
	friend class SkinListBack;
	friend class SkinListThread;

private:
	SkinListNode* parent = nullptr; // Parent node
	SkinListNode* child = nullptr; // First child node
	SkinListNode* clast = nullptr; // Last child node
	SkinListNode* next = nullptr; // Next node
	SkinListNode* prev = nullptr; // Previous node

	int ccount = 0; // Number of children

	SkinListNode* left = nullptr;
	SkinListNode* right = nullptr;

	int refCount = 0;

public:
	inline SkinListNode* Parent() {return parent;}
	inline SkinListNode* Child() {return child;}
	inline SkinListNode* Next() {return next;}
	inline SkinListNode* Prev() {return prev;}
	inline SkinListNode* LastChild() {return clast;}
	inline bool HasChild() {return (child ? true : false);}
	inline SkinListNode* Left() {return left;}
	inline SkinListNode* Right() {return right;}
	inline void AddLeft() {left = new SkinListNode(); left->parent = this;}
	inline void AddRight() {right = new SkinListNode(); right->parent = this;}
	inline void AddChild(SkinListNode* node)
	{
		node->parent = this;

		if (child == nullptr)
		{
			child = node;
			clast = node;
		}
		else
		{
			clast->next = node;
			node->prev = clast;
			clast = node;
		}
		
		++ccount;
	}
	inline void InsertChild(SkinListNode* node)
	{
		node->parent = this;

		if (child == nullptr)
		{
			child = node;
			clast = node;
		}
		else
		{
			child->prev = node;
			node->next = child;
			child = node;
		}

		++ccount;
	}
	inline void InsertChildTo(SkinListNode* node, SkinListNode* nodeBefore)
	{
		node->parent = this;

		if (child == nodeBefore)
			child = node;
		//if (clast == nodeBefore) // Don't do this! Leave it commented to don't forget.
		//	clast = node;

		if (nodeBefore->prev)
			nodeBefore->prev->next = node;

		node->prev = nodeBefore->prev;
		nodeBefore->prev = node;
		node->next = nodeBefore;

		++ccount;
	}
	inline void RemoveChild(SkinListNode* node)
	{
		if (child == node)
			child = node->next;
		if (clast == node)
			clast = node->prev;

		if (node->prev)
			node->prev->next = node->next;
		if (node->next)
			node->next->prev = node->prev;

		// If this assert was called then 1 of smart pointers still owns the node.
		// In case this error pop up in the release just remove all node's relationships (cause memory leak),
		// or otherwise it will cause undefined behaviour and the program just crash in the best case.
		assert(node->refCount == 0); // Critical error
		if (node->refCount == 0)
			delete node;
		else
		{
			node->parent = nullptr;
			node->next = nullptr;
			node->prev = nullptr;
		}

		--ccount;
	}
	inline void EmptyChild()
	{
		for (SkinListNode* n = child; n != nullptr;)
		{
			SkinListNode* t = n->next;

			// If this assert was called then 1 of smart pointers still owns the node.
			// In case this error pop up in the release just remove all node relationships (cause memory leak),
			// or otherwise it will cause undefined behaviour and the program just crash in the best case.
			assert(n->refCount == 0); // Critical error
			if (n->refCount == 0)
				delete n;
			else
			{
				n->parent = nullptr;
				n->next = nullptr;
				n->prev = nullptr;
			}

			n = t;
		}

		child = nullptr;
		clast = nullptr;

		ccount = 0;
	}
	inline SkinListNode* ChildByIndex(int index)
	{
		int i = 0;
		for (SkinListNode* n = child; n != nullptr; n = n->next)
		{
			if (index == i)
				return n;
			++i;
		}

		return nullptr;
	}
	inline void MoveChild(SkinListNode* node, SkinListNode* newParent, SkinListNode* nodeBefore)
	{
		// node - remove this node from the old place and insert it into the new one
		// newParent - new parent for the node
		// nodeBefore - insert before this node, if nullptr then insert to the end

		if (node == nodeBefore) // Do not insert into the same place
			return;

		// Remove from the old place
		if (child == node)
			child = node->next;
		if (clast == node)
			clast = node->prev;

		if (node->prev)
			node->prev->next = node->next;
		if (node->next)
			node->next->prev = node->prev;

		--ccount;

		node->parent = nullptr;
		node->next = nullptr;
		node->prev = nullptr;

		// Insert into the new place
		if (nodeBefore == nullptr)
			newParent->AddChild(node);
		else
			newParent->InsertChildTo(node, nodeBefore);
	}

public:
	std::wstring file;
	const std::wstring& GetFile() {return file;}
	long long GetCueValue() {return trackCue;}
	const std::wstring& GetLabel(SkinListElement::Type type) {return labels[(std::size_t)type];}
	void SetLabel(SkinListElement::Type type, const std::wstring& label) {labels[(std::size_t)type] = label;}

	int rating = 0;
	long long trackCue = 0;
	int trackTime = 0;
	unsigned trackSize = 0;

	// Only one of the following values should be > 0
	long long idLibrary = 0; // Track index in the library
	long long idPlaylist = 0; // Track index in the playlist

private:
	ExImage* cover = nullptr; // Cover art
	bool isCover = false; // Cover art is loaded?
	CSize szCover;

	enum class NodeType
	{
		None  = 0,
		Song  = 1,
		Head  = 2,
		Left  = 3,
		Right = 4
	};

	CRect rcNode = {0, 0, 0, 0};
	NodeType nodeType = NodeType::None;
	bool isOpen = true;
	bool isSelect = false;
	bool isShuffle = false;
	std::vector<std::wstring> labels;

//	int flag = 0;

	struct StateFlag
	{
		// Selection states
		static const int Normal   = 0x0001;
		static const int Select   = 0x0002;
		static const int Focus    = 0x0004;

		// Play states
		static const int Stop     = 0x0001;
		static const int Play     = 0x0002;
		static const int Pause    = 0x0004;

		// Open states
		static const int Open     = 0x0001;
		static const int Close    = 0x0002;

		// Line number states
		static const int Line1    = 0x0001;
		static const int Line2    = 0x0002;

		// Library or Playlist states
		static const int Library  = 0x0001;
		static const int Playlist = 0x0002;
	};

	int stateSelect = StateFlag::Normal;
	int statePlay = StateFlag::Stop;
	int stateLine = 0;
	int stateLibrary = 0;

	// ListNodeUnsafe (raw pointer) for pointers used at the same place where received.
	// ListNodeSafe (smart pointer) for pointers used at different places of the code.
public:
	class SmartPtr final
	{
	public:
		SmartPtr() : node(nullptr) {}
		~SmartPtr()
		{
			reset();
		}
		SmartPtr(const SmartPtr& smart) : node(smart.node)
		{
			if (node) ++node->refCount;
		}
		SmartPtr& operator=(const SmartPtr& smart)
		{
			if (this != &smart)
				reset(smart.node);
			return *this;
		}
		explicit SmartPtr(SkinListNode* ptr) : node(ptr)
		{
			if (node) ++node->refCount;
		}
		inline SkinListNode& operator*() const {return *node;}
		inline SkinListNode* operator->() const {return node;}
		//operator SkinListNode*() const {return node;}
		inline explicit operator bool() const {return node != nullptr;}
		inline bool operator!=(const SmartPtr& smart) const {return node != smart.node;}
		inline bool operator==(const SmartPtr& smart) const {return node == smart.node;}

		inline SkinListNode* get() {return node;}

		void reset()
		{
			if (node)
			{
				if (--node->refCount == 0 && node->parent == nullptr)
					delete node;
				node = nullptr;
			}
		}
		void reset(SkinListNode* ptr)
		{
			reset();
			node = ptr;
			if (node) ++node->refCount;
		}
	private:
		SkinListNode* node;
	};
};

using ListNodeUnsafe = SkinListNode*;
using ListNodeSafe = SkinListNode::SmartPtr;

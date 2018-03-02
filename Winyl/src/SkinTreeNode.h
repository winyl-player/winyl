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

// SkinTreeNode

#include "ExImage.h"

class SkinTreeNode final
{

public:
	SkinTreeNode();
	~SkinTreeNode();
	friend class SkinTree;
	friend class SkinTreeElement;
	friend class SkinTreeBack;

private:
	SkinTreeNode* parent = nullptr; // Parent node
	SkinTreeNode* child = nullptr; // First child node
	SkinTreeNode* clast = nullptr; // Last child node
	SkinTreeNode* next = nullptr; // Next node
	SkinTreeNode* prev = nullptr; // Previous node

	int refCount = 0;

public:
	inline SkinTreeNode* Parent() {return parent;}
	inline SkinTreeNode* Child() {return child;}
	inline SkinTreeNode* Next() {return next;}
	inline SkinTreeNode* Prev() {return prev;}
	inline SkinTreeNode* LastChild() {return clast;}
	inline bool HasChild() {return (child ? true : false);}
	inline void AddChild(SkinTreeNode* node)
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
	}
	inline void InsertChild(SkinTreeNode* node)
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
	}
	inline void InsertChildTo(SkinTreeNode* node, SkinTreeNode* nodeBefore)
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
	}
	inline void RemoveChild(SkinTreeNode* node)
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
		// In case this error pop up in the release just remove all node relationships (cause memory leak),
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
	}
	inline void EmptyChild()
	{
		for (SkinTreeNode* n = child; n != nullptr;)
		{
			SkinTreeNode* t = n->next;

			// If this assert was called then 1 of smart pointers still owns the node.
			// In case this error pop up in the release just remove all node's relationships (cause memory leak),
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
	}
	inline SkinTreeNode* ChildByIndex(int index)
	{
		int i = 0;
		for (SkinTreeNode* n = child; n != nullptr; n = n->next)
		{
			if (index == i)
				return n;
			++i;
		}

		return nullptr;
	}
	inline void MoveChild(SkinTreeNode* node, SkinTreeNode* newParent, SkinTreeNode* nodeBefore)
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
	inline const std::wstring& GetTitle() {return labels[0];}
	inline const std::wstring& GetValue() {return value;}
	inline const std::wstring& GetArtist() {return artist;}
	inline const std::wstring& GetAlbum() {return album;}
	inline bool IsType() {return isType;}
	inline bool IsValue() {return isValue;}
	inline bool IsArtist() {return isArtist;}
	inline bool IsAlbum() {return isAlbum;}

	enum class NodeType
	{
		None  = 0,
		Text  = 1,
		Head  = 2
	};

	enum class Type
	{
		None       = 0,
		NowPlaying = 1,
		Playlist   = 2,
		Smartlist  = 3,
		Artist     = 4,
		Composer   = 5,
		Album      = 6,
		Genre      = 7,
		Year       = 8,
		Folder     = 9,
		Radio      = 10
		//AudioBook  = 11,
		//Podcast    = 12,
		//Device     = 13
	};

	inline Type GetType() {return type;}
	inline NodeType GetNodeType() {return nodeType;}

private:
	int place = 0;
	CRect rcNode = {0, 0, 0, 0};

	Type type = Type::None;

	NodeType nodeType = NodeType::None;

	std::vector<std::wstring> labels;

	bool isType = false; // type is used
	bool isValue = false; // type, value are used
	bool isArtist = false; // type, value, artist are used
	bool isAlbum = false; // type, value, artist, album are used
	std::wstring value;
	std::wstring artist;
	std::wstring album;

	bool isOpen = false;
	bool isSelect = false;

	bool isShowOpen = true; // Show open/close icon

	struct StateFlag
	{
		// Selection states
		static const int Normal = 0x0001;
		static const int Select = 0x0002;
		static const int Focus  = 0x0004;

		// Play states
		static const int Open   = 0x0001;
		static const int Close  = 0x0002;

		// Line number states
		static const int Line1  = 0x0001;
		static const int Line2  = 0x0002;
	};

	int stateSelect = StateFlag::Normal;
	int stateLine = 0;

	// TreeNodeUnsafe (raw pointer) for pointers used at the same place where received.
	// TreeNodeSafe (smart pointer) for pointers used at different places of the code.
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
		explicit SmartPtr(SkinTreeNode* ptr) : node(ptr)
		{
			if (node) ++node->refCount;
		}
		inline SkinTreeNode& operator*() const {return *node;}
		inline SkinTreeNode* operator->() const {return node;}
		//operator SkinListNode*() const {return node;}
		inline explicit operator bool() const {return node != nullptr;}
		inline bool operator!=(const SmartPtr& smart) const {return node != smart.node;}
		inline bool operator==(const SmartPtr& smart) const {return node == smart.node;}

		inline SkinTreeNode* get() {return node;}

		void reset()
		{
			if (node)
			{
				if (--node->refCount == 0 && node->parent == nullptr)
					delete node;
				node = nullptr;
			}
		}
		void reset(SkinTreeNode* ptr)
		{
			reset();
			node = ptr;
			if (node) ++node->refCount;
		}
	private:
		SkinTreeNode* node;
	};
};

using TreeNodeUnsafe = SkinTreeNode*;
using TreeNodeSafe = SkinTreeNode::SmartPtr;

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
#include <assert.h>

//template<typename T, BOOL(WINAPI* TCloseFunc)(T), T TNull = NULL> // nullT
template<typename T, BOOL(WINAPI* F)(T), T N = NULL>
class AutoHandle
{
public:
	AutoHandle() : handle(N) {}
	explicit AutoHandle(T h) : handle(h) {}
	~AutoHandle() {reset();}

	inline operator bool() {return handle != N;}
	inline T get() {return handle;}

	void reset()
	{
		if (handle != N)
		{
#ifdef _DEBUG
			assert((*F)(handle) == TRUE);
#else
			(*F)(handle);
#endif
			handle = N;
		}
	}
	void reset(T h)
	{
		reset();
		handle = h;
	}

private:
	T handle;
};

typedef AutoHandle<HANDLE, ::FindClose, INVALID_HANDLE_VALUE> FindHandle;
typedef AutoHandle<HMODULE, ::FreeLibrary> LibraryHandle;
// etc.

/////////////////////////////////////

template<typename T>
class AutoCom
{
public:
	AutoCom() : pointer(nullptr) {}
	~AutoCom() {if (pointer) pointer->Release();}

	inline operator bool() {return (pointer ? true : false);}
	inline T& operator*() const {return *pointer;}
	inline T* operator->() const {return pointer;}

	void Release()
	{
		T* temp = pointer;
		if (temp)
		{
			pointer = nullptr;
			temp->Release();
		}
	}

	HRESULT CoCreateInstance(REFCLSID rclsid, LPUNKNOWN pUnkOuter = NULL, DWORD dwClsContext = CLSCTX_ALL)
	{
		//CLSCTX_INPROC_SERVER
		assert(pointer == nullptr);
		return ::CoCreateInstance(rclsid, pUnkOuter, dwClsContext, __uuidof(T), (void**)&pointer);
	}

private:
	T* pointer;
};

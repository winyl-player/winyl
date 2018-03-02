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

#include "MyDropSource.h"

// http://blogs.msdn.com/b/oldnewthing/archive/2014/06/09/10532207.aspx (Vista+)
// http://blogs.msdn.com/b/oldnewthing/archive/2008/03/11/8080077.aspx
// http://blogs.msdn.com/b/oldnewthing/archive/2004/12/06/275659.aspx

class MyDropTarget : public IDropTarget
{
public:
//	MyDropTarget();
//	virtual ~MyDropTarget();

	void SetDropSource(MyDropSource* mySource) {myDropSource = mySource;}

	MyDropSource* myDropSource;

public:
	MyDropTarget() : m_cRef(1) { myDropSource = NULL; }
	STDMETHODIMP QueryInterface(REFIID riid, void** ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();
	STDMETHODIMP DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	STDMETHODIMP DragOver(DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect);
	STDMETHODIMP DragLeave();
	STDMETHODIMP Drop(IDataObject* pdto, DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect);
private:
	LONG m_cRef;
};



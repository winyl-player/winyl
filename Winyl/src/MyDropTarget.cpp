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

// MyDropTarget.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "MyDropTarget.h"


// MyDropTarget

//MyDropTarget::MyDropTarget()
//{
//	myDropSource = NULL;
//}
//
//MyDropTarget::~MyDropTarget()
//{
//}

HRESULT MyDropTarget::QueryInterface(REFIID riid, void** ppv)
{
	IUnknown* punk = NULL;
	if (riid == IID_IUnknown)
	{
		punk = static_cast<IUnknown*>(this);
	}
	else if (riid == IID_IDropTarget)
	{
		punk = static_cast<IDropTarget*>(this);
	}

	*ppv = punk;
	if (punk)
	{
		punk->AddRef();
		return S_OK;
	}
	else
	{
		return E_NOINTERFACE;
	}
}

ULONG MyDropTarget::AddRef()
{
	return ++m_cRef;
}

ULONG MyDropTarget::Release()
{
	ULONG cRef = --m_cRef;
	if (cRef == 0) delete this;
	return cRef;
}

STDMETHODIMP MyDropTarget::DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	if (myDropSource)
		myDropSource->StopDrag();

	*pdwEffect &= DROPEFFECT_COPY;
	return S_OK;
}

STDMETHODIMP MyDropTarget::DragOver(DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect)
{
	*pdwEffect &= DROPEFFECT_COPY;
	return S_OK;
}

STDMETHODIMP MyDropTarget::DragLeave()
{
	return S_OK;
}

STDMETHODIMP MyDropTarget::Drop(IDataObject* pdto, DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect)
{
	*pdwEffect &= DROPEFFECT_COPY;
	return S_OK;
}

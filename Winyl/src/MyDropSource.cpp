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
#include "resource.h"
#include "MyDropSource.h"

//MyDropSource::MyDropSource()
//{
//	bStopDrag = FALSE;
//}
//
//MyDropSource::~MyDropSource()
//{
//}

HRESULT MyDropSource::QueryInterface(REFIID riid, void** ppv)
{
	IUnknown* punk = NULL;
	if (riid == IID_IUnknown)
	{
		punk = static_cast<IUnknown*>(this);
	}
	else if (riid == IID_IDropSource)
	{
		punk = static_cast<IDropSource*>(this);
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

ULONG MyDropSource::AddRef()
{
	return ++m_cRef;
}

ULONG MyDropSource::Release()
{
	ULONG cRef = --m_cRef;
	if (cRef == 0) delete this;
	return cRef;
}

STDMETHODIMP MyDropSource::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)
{
	if (isStopDrag)
		return DRAGDROP_S_CANCEL;

	if (fEscapePressed)
		return DRAGDROP_S_CANCEL;

	if (grfKeyState & MK_RBUTTON)
		return DRAGDROP_S_CANCEL;

	// [Update: missing paren repaired, 7 Dec]
	if (!(grfKeyState & (MK_LBUTTON | MK_RBUTTON)))
		return DRAGDROP_S_DROP;

	return S_OK;
}

STDMETHODIMP MyDropSource::GiveFeedback(DWORD dwEffect)
{
	return DRAGDROP_S_USEDEFAULTCURSORS;
}

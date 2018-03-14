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
#include "MyDataObject.h"

HRESULT MyDataObject::QueryInterface(REFIID riid, void** ppv)
{
	IUnknown* punk = NULL;
	if (riid == IID_IUnknown)
	{
		punk = static_cast<IUnknown*>(this);
	}
	else if (riid == IID_IDataObject)
	{
		punk = static_cast<IDataObject*>(this);
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

ULONG MyDataObject::AddRef()
{
	return ++m_cRef;
}

ULONG MyDataObject::Release()
{
	ULONG cRef = --m_cRef;
	if (cRef == 0) delete this;
	return cRef;
}

void SetFORMATETC(FORMATETC* pfe, UINT cf, TYMED tymed = TYMED_HGLOBAL, LONG lindex = -1, DWORD dwAspect = DVASPECT_CONTENT, DVTARGETDEVICE* ptd = NULL)
{
	pfe->cfFormat = (CLIPFORMAT)cf;
	pfe->tymed    = tymed;
	pfe->lindex   = lindex;
	pfe->dwAspect = dwAspect;
	pfe->ptd      = ptd;
}

MyDataObject::MyDataObject() : m_cRef(1)
{
	SetFORMATETC(&m_rgfe[DATA_TEXT], CF_HDROP);
}

int MyDataObject::GetDataIndex(const FORMATETC* pfe)
{
	for (int i = 0; i < ARRAYSIZE(m_rgfe); ++i)
	{
		if (pfe->cfFormat == m_rgfe[i].cfFormat &&
			(pfe->tymed    &  m_rgfe[i].tymed)  &&
			pfe->dwAspect == m_rgfe[i].dwAspect &&
			pfe->lindex   == m_rgfe[i].lindex)
		{
			return i;
		}
	}
	return DATA_INVALID;
}

HRESULT MyDataObject::GetData(FORMATETC* pfe, STGMEDIUM* pmed)
{
	ZeroMemory(pmed, sizeof(*pmed));

	switch (GetDataIndex(pfe))
	{
		case DATA_TEXT:
			pmed->tymed = TYMED_HGLOBAL;

			// Copy Global Memory
			SIZE_T bufSize = ::GlobalSize(hGlobal);
			pmed->hGlobal  = ::GlobalAlloc(GMEM_ZEROINIT|GMEM_MOVEABLE, bufSize);
			if (pmed->hGlobal)
			{
				LPVOID bufSrc  = ::GlobalLock(hGlobal);
				LPVOID bufDest = ::GlobalLock(pmed->hGlobal);
				CopyMemory(bufDest, bufSrc, bufSize);
				::GlobalUnlock(hGlobal);
				::GlobalUnlock(pmed->hGlobal);
			}
			return S_OK;
	}

	return DV_E_FORMATETC;
}

HRESULT MyDataObject::QueryGetData(FORMATETC* pfe)
{
	return GetDataIndex(pfe) == DATA_INVALID ? S_FALSE : S_OK;
}

HRESULT MyDataObject::EnumFormatEtc(DWORD dwDirection, LPENUMFORMATETC* ppefe)
{
	if (dwDirection == DATADIR_GET)
	{
		return SHCreateStdEnumFmtEtc(ARRAYSIZE(m_rgfe), m_rgfe, ppefe);
	}
	*ppefe = NULL;
	return E_NOTIMPL;
}

HRESULT MyDataObject::GetDataHere(FORMATETC* pfe, STGMEDIUM* pmed)
{
	return E_NOTIMPL;
}

HRESULT MyDataObject::GetCanonicalFormatEtc(FORMATETC* pfeIn, FORMATETC* pfeOut)
{
	*pfeOut = *pfeIn;
	pfeOut->ptd = NULL;
	return DATA_S_SAMEFORMATETC;
}

HRESULT MyDataObject::SetData(FORMATETC* pfe, STGMEDIUM* pmed, BOOL fRelease)
{
	return E_NOTIMPL;
}

HRESULT MyDataObject::DAdvise(FORMATETC* pfe, DWORD grfAdv, IAdviseSink* pAdvSink, DWORD* pdwConnection)
{
	return OLE_E_ADVISENOTSUPPORTED;
}

HRESULT MyDataObject::DUnadvise(DWORD dwConnection)
{
	return OLE_E_ADVISENOTSUPPORTED;
}

HRESULT MyDataObject::EnumDAdvise(LPENUMSTATDATA* ppefe)
{
	return OLE_E_ADVISENOTSUPPORTED;
}

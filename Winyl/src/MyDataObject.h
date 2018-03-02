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

//#include <strsafe.h> // for StringCchCopy
//#include <shlobj.h> // (will be needed in future articles)

// http://blogs.msdn.com/b/oldnewthing/archive/2014/06/09/10532207.aspx (Vista+)
// http://blogs.msdn.com/b/oldnewthing/archive/2008/03/11/8080077.aspx
// http://blogs.msdn.com/b/oldnewthing/archive/2004/12/06/275659.aspx

class MyDataObject : public IDataObject
{
public:
	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObj);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// IDataObject
	STDMETHODIMP GetData(FORMATETC* pfe, STGMEDIUM* pmed);
	STDMETHODIMP GetDataHere(FORMATETC* pfe, STGMEDIUM* pmed);
	STDMETHODIMP QueryGetData(FORMATETC* pfe);
	STDMETHODIMP GetCanonicalFormatEtc(FORMATETC* pfeIn, FORMATETC* pfeOut);
	STDMETHODIMP SetData(FORMATETC* pfe, STGMEDIUM* pmed, BOOL fRelease);
	STDMETHODIMP EnumFormatEtc(DWORD dwDirection, LPENUMFORMATETC* ppefe);
	STDMETHODIMP DAdvise(FORMATETC* pfe, DWORD grfAdv, IAdviseSink* pAdvSink, DWORD* pdwConnection);
	STDMETHODIMP DUnadvise(DWORD dwConnection);
	STDMETHODIMP EnumDAdvise(LPENUMSTATDATA* ppefe);

	MyDataObject();

	HGLOBAL hGlobal;

private:
  enum {
    DATA_TEXT,
    DATA_NUM,
    DATA_INVALID = -1,
  };

  int GetDataIndex(const FORMATETC* pfe);

private:
	ULONG m_cRef;
	FORMATETC m_rgfe[DATA_NUM];
};

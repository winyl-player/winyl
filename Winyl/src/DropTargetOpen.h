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

#include <shlobj.h>
#include <shellapi.h>
#include <new> // for new(nothrow)

#include <string> 
extern std::wstring globalOpenFiles;

// http://blogs.msdn.com/b/oldnewthing/archive/2010/05/03/10006065.aspx
// http://blogs.msdn.com/b/oldnewthing/archive/2010/05/28/10016692.aspx
// OLD MFC: http://www.ureader.com/msg/16601261.aspx
// Windows 7 SDK\Samples\winui\shell\appshellintegration\DropTargetVerb\

class DropTargetOpen
{
public:
	DropTargetOpen(void);
	virtual ~DropTargetOpen(void);

	static bool GetDropFiles(HINSTANCE hInstance, std::wstring& openFiles);

	static void OpenFilesFromDataObject(IDataObject* pdto);

	static std::wstring* pointerOpenFiles;


class ProcessReference : public IUnknown
{
public:
	STDMETHODIMP QueryInterface(REFIID riid, void** ppv)
	{
		if (riid == IID_IUnknown)
		{
			*ppv = static_cast<IUnknown*>(this);
			AddRef();
			return S_OK;
		}
		*ppv = NULL; return E_NOINTERFACE;
	}

	STDMETHODIMP_(ULONG) AddRef()
	{
		return InterlockedIncrement(&m_cRef);
	}

	STDMETHODIMP_(ULONG) Release()
	{
		LONG lRef = InterlockedDecrement(&m_cRef);
		//if (lRef == 0) PostThreadMessage(m_dwThread, WM_NULL, 0, 0);
		return lRef;
	}

	ProcessReference()
		: m_cRef(1), m_dwThread(GetCurrentThreadId())
	{
		SHSetInstanceExplorer(this);
	}

	~ProcessReference()
	{
		SHSetInstanceExplorer(NULL);
		Release();

		//MSG msg;
		//while (m_cRef && GetMessage(&msg, NULL, 0, 0))
		//{
		//	TranslateMessage(&msg);
		//	DispatchMessage(&msg);
		//}
	}

private:
	LONG m_cRef;
	DWORD m_dwThread;
};

static ProcessReference* g_ppr;


class SimpleDropTarget : public IDropTarget
{
public:
	SimpleDropTarget() : m_cRef(1) { g_ppr->AddRef(); }
	~SimpleDropTarget() { g_ppr->Release(); }

	// *** IUnknown ***
	STDMETHODIMP QueryInterface(REFIID riid, void** ppv)
	{
		if (riid == IID_IUnknown || riid == IID_IDropTarget)
		{
			*ppv = static_cast<IUnknown*>(this);
			AddRef();
			return S_OK;
		}
		*ppv = NULL;
		return E_NOINTERFACE;
	}

	STDMETHODIMP_(ULONG) AddRef()
	{
		return InterlockedIncrement(&m_cRef);
	}

	STDMETHODIMP_(ULONG) Release()
	{
		LONG cRef = InterlockedDecrement(&m_cRef);
		if (cRef == 0) delete this;
		return cRef;
	}

	// *** IDropTarget ***
	STDMETHODIMP DragEnter(IDataObject* pdto,
		DWORD grfKeyState, POINTL ptl, DWORD* pdwEffect)
	{
		*pdwEffect &= DROPEFFECT_COPY;
		return S_OK;
	}

	STDMETHODIMP DragOver(DWORD grfKeyState,
		POINTL ptl, DWORD* pdwEffect)
	{
		*pdwEffect &= DROPEFFECT_COPY;
		return S_OK;
	}

	STDMETHODIMP DragLeave()
	{
		return S_OK;
	}

	STDMETHODIMP Drop(IDataObject* pdto, DWORD grfKeyState,
		POINTL ptl, DWORD* pdwEffect)
	{
		OpenFilesFromDataObject(pdto);
		*pdwEffect &= DROPEFFECT_COPY;
		return S_OK;
	}

private:
	LONG m_cRef;
};

class SimpleClassFactory : public IClassFactory
{
public:
	// *** IUnknown ***
	STDMETHODIMP QueryInterface(REFIID riid, void** ppv)
	{
		if (riid == IID_IUnknown || riid == IID_IClassFactory)
		{
			*ppv = static_cast<IUnknown*>(this);
			AddRef();
			return S_OK;
		}

		*ppv = NULL;
		return E_NOINTERFACE;
	}

	STDMETHODIMP_(ULONG) AddRef()
	{
		return 2;
	}

	STDMETHODIMP_(ULONG) Release()
	{
		return 1;
	}

	// *** IClassFactory ***
	STDMETHODIMP CreateInstance(IUnknown* punkOuter, REFIID riid, void** ppv)
	{
		*ppv = NULL;
		if (punkOuter) return CLASS_E_NOAGGREGATION;
		SimpleDropTarget* pdt = new(std::nothrow) SimpleDropTarget();
		if (!pdt) return E_OUTOFMEMORY;
		HRESULT hr = pdt->QueryInterface(riid, ppv);
		pdt->Release();
		return hr;
	}

	STDMETHODIMP LockServer(BOOL fLock)
	{
		if (!g_ppr) return E_FAIL; // server shutting down
		if (fLock) g_ppr->AddRef(); else g_ppr->Release();
		return S_OK;
	}
};

//SimpleClassFactory s_scf;

};

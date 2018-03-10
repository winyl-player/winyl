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
#include "FutureWin.h"

FutureWin::FutureWin()
{
	OSVERSIONINFO osVersion = {};
	osVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	::GetVersionEx(&osVersion);

	if (osVersion.dwMajorVersion >= 6)
	{
		isVistaOrLater = true;

		if (osVersion.dwMinorVersion >= 1 || osVersion.dwMajorVersion > 6)
			isSevenOrLater = true;
		if (osVersion.dwMinorVersion >= 2 || osVersion.dwMajorVersion > 6)
			isEightOrLater = true;
	}

	handleTheme = ::OpenThemeData(NULL, L"Window");

	if (isVistaOrLater)
	{
		libKernel32 = ::LoadLibraryW(L"kernel32.dll");
		libUser32 = ::LoadLibraryW(L"user32.dll");
		libComctl32 = ::LoadLibraryW(L"comctl32.dll");
		//libShell32 = ::LoadLibraryW(L"shell32.dll");
		libDwmApi = ::LoadLibraryW(L"dwmapi.dll");
		libUxTheme = ::LoadLibraryW(L"uxtheme.dll");

		LoadKernel32Functions();
		LoadUser32Functions();
		LoadComctl32Functions();
		LoadShell32Functions();
		LoadDwmApiFunctions();
		LoadUxThemeFunctions();
	}
}

FutureWin::~FutureWin()
{
	if (handleTheme)
		::CloseThemeData(handleTheme);

	if (libKernel32)
		::FreeLibrary(libKernel32);

	if (libUser32)
		::FreeLibrary(libUser32);

	if (libComctl32)
		::FreeLibrary(libShell32);

	if (libShell32)
		::FreeLibrary(libShell32);

	if (libDwmApi)
		::FreeLibrary(libDwmApi);

	if (libUxTheme)
		::FreeLibrary(libUxTheme);
}

void FutureWin::ThemeChanged()
{
	if (handleTheme)
		CloseThemeData(handleTheme);

	handleTheme = OpenThemeData(NULL, L"Window");
}

bool FutureWin::IsCompositionEnabled()
{
	BOOL bEnabled = FALSE;
	DwmIsCompositionEnabled(&bEnabled);

	return bEnabled ? true : false;
}

void FutureWin::LoadKernel32Functions()
{
	if (libKernel32 == NULL)
		return;

	funcCompareStringEx = (COMPARE_STRING_EX)::GetProcAddress(libKernel32, "CompareStringEx");
	funcLCMapStringEx = (LC_MAP_STRING_EX)::GetProcAddress(libKernel32, "LCMapStringEx");
	funcFindNLSStringEx = (FIND_NLS_STRING_EX)::GetProcAddress(libKernel32, "FindNLSStringEx");
	funcCompareStringOrdinal = (COMPARE_STRING_ORDINAL)::GetProcAddress(libKernel32, "CompareStringOrdinal");
	funcFindStringOrdinal = (FIND_STRING_ORDINAL)::GetProcAddress(libKernel32, "FindStringOrdinal");

	funcInitializeConditionVariable = (INITIALIZE_CONDITION_VARIABLE)::GetProcAddress(libKernel32, "InitializeConditionVariable");
	funcSleepConditionVariableCS = (SLEEP_CONDITION_VARIABLE_CS)::GetProcAddress(libKernel32, "SleepConditionVariableCS");
	funcSleepConditionVariableSRW = (SLEEP_CONDITION_VARIABLE_SRW)::GetProcAddress(libKernel32, "SleepConditionVariableSRW");
	funcWakeAllConditionVariable = (WAKE_ALL_CONDITION_VARIABLE)::GetProcAddress(libKernel32, "WakeAllConditionVariable");
	funcWakeConditionVariable = (WAKE_CONDITION_VARIABLE)::GetProcAddress(libKernel32, "WakeConditionVariable");

	funcInitializeCriticalSectionEx = (INITIALIZE_CRITICAL_SECTION_EX)::GetProcAddress(libKernel32, "InitializeCriticalSectionEx");
	funcInitializeSRWLock = (INITIALIZE_SRW_LOCK)::GetProcAddress(libKernel32, "InitializeSRWLock");
	funcAcquireSRWLockExclusive = (ACQUIRE_SRW_LOCK_EXCLUSIVE)::GetProcAddress(libKernel32, "AcquireSRWLockExclusive");
	funcReleaseSRWLockExclusive = (RELEASE_SRW_LOCK_EXCLUSIVE)::GetProcAddress(libKernel32, "ReleaseSRWLockExclusive");
	funcTryAcquireSRWLockExclusive = (TRY_ACQUIRE_SRW_LOCK_EXCLUSIVE)::GetProcAddress(libKernel32, "TryAcquireSRWLockExclusive");
}

void FutureWin::LoadUser32Functions()
{
	if (libUser32 == NULL)
		return;

	funcUpdateLayeredWindowIndirect = (UPDATE_LAYERED_WINDOW_INDIRECT)::GetProcAddress(libUser32, "UpdateLayeredWindowIndirect");
}

void FutureWin::LoadComctl32Functions()
{
	if (libComctl32 == NULL)
		return;

	funcTaskDialog = (TASK_DIALOG)::GetProcAddress(libComctl32, "TaskDialog");
	funcTaskDialogIndirect = (TASK_DIALOG_INDIRECT)::GetProcAddress(libComctl32, "TaskDialogIndirect");
	funcLoadIconMetric = (LOAD_ICON_METRIC)::GetProcAddress(libComctl32, "LoadIconMetric");
}

void FutureWin::LoadShell32Functions()
{
	if (libShell32 == NULL)
		return;

	funcSetCurrentProccesAppID = (SET_CURRENT_PROCCES_APPID)GetProcAddress(libShell32, "SetCurrentProcessExplicitAppUserModelID");
}

void FutureWin::LoadDwmApiFunctions()
{
	if (libDwmApi == NULL)
		return;

	funcDwmIsCompositionEnabled = (DWM_IS_COMPOSITION_ENABLED)::GetProcAddress(libDwmApi, "DwmIsCompositionEnabled");
	funcDwmEnableComposition = (DWM_ENABLE_COMPOSITION)::GetProcAddress(libDwmApi, "DwmEnableComposition");
	funcDwmEnableBlurBehindWindow = (DWM_ENABLE_BLUR_BEHIND_WINDOW)::GetProcAddress(libDwmApi, "DwmEnableBlurBehindWindow");
	funcDwmExtendFrameIntoClientArea = (DWM_EXTEND_FRAME_INTO_CLIENT_AREA)::GetProcAddress(libDwmApi, "DwmExtendFrameIntoClientArea");
	funcDwmSetWindowAttribute = (DWM_SET_WINDOW_ATTRIBUTE)::GetProcAddress(libDwmApi, "DwmSetWindowAttribute");
	funcDwmGetWindowAttribute = (DWM_GET_WINDOW_ATTRIBUTE)::GetProcAddress(libDwmApi, "DwmGetWindowAttribute");
	funcDwmSetIconicThumbnail = (DWM_SET_ICONIC_THUMBNAIL)::GetProcAddress(libDwmApi, "DwmSetIconicThumbnail");
	funcDwmSetIconicLivePreviewBitmap = (DWM_SET_ICONIC_LIVE_PREVIEW_BITMAP)::GetProcAddress(libDwmApi, "DwmSetIconicLivePreviewBitmap");
	funcDwmInvalidateIconicBitmaps = (DWM_INVALIDATE_ICONIC_BITMAPS)::GetProcAddress(libDwmApi, "DwmInvalidateIconicBitmaps");
}

void FutureWin::LoadUxThemeFunctions()
{
	if (libUxTheme == NULL)
		return;

	funcDrawThemeTextEx = (DRAW_THEME_TEXT_EX)::GetProcAddress(libUxTheme, "DrawThemeTextEx");
	funcBufferedPaintInit = (BUFFERED_PAINT_INIT)::GetProcAddress(libUxTheme, "BufferedPaintInit");
	funcBufferedPaintUnInit = (BUFFERED_PAINT_UNINIT)::GetProcAddress(libUxTheme, "BufferedPaintUnInit");
	funcBeginBufferedPaint = (BEGIN_BUFFERED_PAINT)::GetProcAddress(libUxTheme, "BeginBufferedPaint");
	funcEndBufferedPaint = (END_BUFFERED_PAINT)::GetProcAddress(libUxTheme, "EndBufferedPaint");
	funcBufferedPaintSetAlpha = (BUFFERED_PAINT_SET_ALPHA)::GetProcAddress(libUxTheme, "BufferedPaintSetAlpha");
}

int FutureWin::CompareStringEx(LPCWSTR lpLocaleName, DWORD dwCmpFlags, LPCWSTR lpString1, int cchCount1, LPCWSTR lpString2, int cchCount2, LPNLSVERSIONINFO lpVersionInformation, LPVOID lpReserved, LPARAM lParam)
{
	return funcCompareStringEx(lpLocaleName, dwCmpFlags, lpString1, cchCount1, lpString2, cchCount2, lpVersionInformation, lpReserved, lParam);
}

int FutureWin::LCMapStringEx(LPCWSTR lpLocaleName, DWORD dwMapFlags, LPCWSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest, LPNLSVERSIONINFO lpVersionInformation, LPVOID lpReserved, LPARAM sortHandle)
{
	return funcLCMapStringEx(lpLocaleName, dwMapFlags, lpSrcStr, cchSrc, lpDestStr, cchDest, lpVersionInformation, lpReserved, sortHandle);
}

int FutureWin::FindNLSStringEx(LPCWSTR lpLocaleName, DWORD dwFindNLSStringFlags, LPCWSTR lpStringSource, int cchSource, LPCWSTR lpStringValue, int cchValue, LPINT pcchFound, LPNLSVERSIONINFO lpVersionInformation, LPVOID lpReserved, LPARAM sortHandle)
{
	return funcFindNLSStringEx(lpLocaleName, dwFindNLSStringFlags, lpStringSource, cchSource, lpStringValue, cchValue, pcchFound, lpVersionInformation, lpReserved, sortHandle);
}

int FutureWin::CompareStringOrdinal(LPCWSTR lpString1, int cchCount1, LPCWSTR lpString2, int cchCount2, BOOL bIgnoreCase)
{
	return funcCompareStringOrdinal(lpString1, cchCount1, lpString2, cchCount2, bIgnoreCase);
}

int FutureWin::FindStringOrdinal(DWORD dwFindStringOrdinalFlags, LPCWSTR lpStringSource, int cchSource, LPCWSTR lpStringValue, int cchValue, BOOL bIgnoreCase)
{
	return funcFindStringOrdinal(dwFindStringOrdinalFlags, lpStringSource, cchSource, lpStringValue, cchValue, bIgnoreCase);
}

VOID FutureWin::InitializeConditionVariable(PCONDITION_VARIABLE ConditionVariable)
{
	//if (!funcInitializeConditionVariable)
	//	return;

	funcInitializeConditionVariable(ConditionVariable);
}

BOOL FutureWin::SleepConditionVariableCS(PCONDITION_VARIABLE ConditionVariable, PCRITICAL_SECTION CriticalSection, DWORD dwMilliseconds)
{
	//if (!funcSleepConditionVariableCS)
	//	return FALSE;

	return funcSleepConditionVariableCS(ConditionVariable, CriticalSection, dwMilliseconds);
}

BOOL FutureWin::SleepConditionVariableSRW(PCONDITION_VARIABLE ConditionVariable, PSRWLOCK SRWLock, DWORD dwMilliseconds, ULONG Flags)
{
	//if (!funcSleepConditionVariableSRW)
	//	return FALSE;

	return funcSleepConditionVariableSRW(ConditionVariable, SRWLock, dwMilliseconds, Flags);
}

VOID FutureWin::WakeAllConditionVariable(PCONDITION_VARIABLE ConditionVariable)
{
	//if (!funcWakeAllConditionVariable)
	//	return;

	funcWakeAllConditionVariable(ConditionVariable);
}

VOID FutureWin::WakeConditionVariable(PCONDITION_VARIABLE ConditionVariable)
{
	//if (!funcWakeConditionVariable)
	//	return;

	funcWakeConditionVariable(ConditionVariable);
}

BOOL FutureWin::InitializeCriticalSectionEx(LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount, DWORD Flags)
{
	//if (!funcInitializeCriticalSectionEx)
	//	return FALSE;

	return funcInitializeCriticalSectionEx(lpCriticalSection, dwSpinCount, Flags);
}

VOID FutureWin::InitializeSRWLock(PSRWLOCK SRWLock)
{
	//if (!funcInitializeSRWLock)
	//	return;

	funcInitializeSRWLock(SRWLock);
}

VOID FutureWin::AcquireSRWLockExclusive(PSRWLOCK SRWLock)
{
	//if (!funcAcquireSRWLockExclusive)
	//	return;

	funcAcquireSRWLockExclusive(SRWLock);
}

VOID FutureWin::ReleaseSRWLockExclusive(PSRWLOCK SRWLock)
{
	//if (!funcReleaseSRWLockExclusive)
	//	return;

	funcReleaseSRWLockExclusive(SRWLock);
}

BOOL FutureWin::TryAcquireSRWLockExclusive(PSRWLOCK SRWLock)
{
	//if (!funcTryAcquireSRWLockExclusive)
	//	return FALSE;

	return funcTryAcquireSRWLockExclusive(SRWLock);
}

BOOL FutureWin::UpdateLayeredWindowIndirect(HWND hwnd, const UPDATELAYEREDWINDOWINFO* pULWInfo)
{
	if (!funcUpdateLayeredWindowIndirect)
		return FALSE;

	return funcUpdateLayeredWindowIndirect(hwnd, pULWInfo);
}

HRESULT FutureWin::TaskDialog(HWND hWndParent, HINSTANCE hInstance, PCWSTR pszWindowTitle, PCWSTR pszMainInstruction, PCWSTR pszContent, TASKDIALOG_COMMON_BUTTON_FLAGS dwCommonButtons, PCWSTR pszIcon, int* pnButton)
{
	if (!funcTaskDialog)
		return E_FAIL;

	return funcTaskDialog(hWndParent, hInstance, pszWindowTitle, pszMainInstruction, pszContent, dwCommonButtons, pszIcon, pnButton);
}

HRESULT FutureWin::TaskDialogIndirect(const TASKDIALOGCONFIG* pTaskConfig, int* pnButton, int* pnRadioButton, BOOL* pfVerificationFlagChecked)
{
	if (!funcTaskDialogIndirect)
		return E_FAIL;

	return funcTaskDialogIndirect(pTaskConfig, pnButton, pnRadioButton, pfVerificationFlagChecked);
}

HRESULT FutureWin::LoadIconMetric(HINSTANCE hinst, PCWSTR pszName, int lims, HICON *phico)
{
	if (!funcLoadIconMetric)
		return E_FAIL;

	return funcLoadIconMetric(hinst, pszName, lims, phico);
}

HRESULT FutureWin::DwmIsCompositionEnabled(BOOL* pfEnabled)
{
	if (!funcDwmIsCompositionEnabled)
		return E_FAIL;

	return funcDwmIsCompositionEnabled(pfEnabled);
}

HRESULT FutureWin::DwmEnableComposition(UINT uCompositionAction)
{
	if (!funcDwmEnableComposition)
		return E_FAIL;

	return funcDwmEnableComposition(uCompositionAction);
}

HRESULT FutureWin::DwmEnableBlurBehindWindow(HWND hWnd, const DWM_BLURBEHIND* pBlurBehind)
{
	if (!funcDwmEnableBlurBehindWindow)
		return E_FAIL;

	return funcDwmEnableBlurBehindWindow(hWnd, pBlurBehind);
}

HRESULT FutureWin::DwmExtendFrameIntoClientArea(HWND hWnd,const MARGINS* pMarInset)
{
	if (!funcDwmExtendFrameIntoClientArea)
		return E_FAIL;

	return funcDwmExtendFrameIntoClientArea(hWnd, pMarInset);
}

HRESULT FutureWin::DwmSetWindowAttribute(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute)
{
	if (!funcDwmSetWindowAttribute)
        return E_FAIL;

	return funcDwmSetWindowAttribute(hwnd, dwAttribute, pvAttribute, cbAttribute);
}

HRESULT FutureWin::DwmGetWindowAttribute(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute)
{
	if (!funcDwmGetWindowAttribute)
		return E_FAIL;

	return funcDwmGetWindowAttribute(hwnd, dwAttribute, pvAttribute, cbAttribute);
}

HRESULT FutureWin::DwmSetIconicThumbnail(HWND hwnd, HBITMAP hbmp, DWORD dwSITFlags)
{
	if (!funcDwmSetIconicThumbnail)
        return E_FAIL;

	return funcDwmSetIconicThumbnail(hwnd, hbmp, dwSITFlags);
}

HRESULT FutureWin::DwmSetIconicLivePreviewBitmap(HWND hwnd, HBITMAP hbmp, POINT* pptClient, DWORD dwSITFlags)
{
	if (!funcDwmSetIconicLivePreviewBitmap)
        return E_FAIL;

	return funcDwmSetIconicLivePreviewBitmap(hwnd, hbmp, pptClient, dwSITFlags);
}

HRESULT FutureWin::DwmInvalidateIconicBitmaps(HWND hwnd)
{
	if (!funcDwmInvalidateIconicBitmaps)
        return E_FAIL;

	return funcDwmInvalidateIconicBitmaps(hwnd);
}

HRESULT FutureWin::DrawThemeTextEx(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwFlags, LPRECT pRect, const DTTOPTS* pOptions)
{
	if (!funcDrawThemeTextEx)
		return E_FAIL;

	return funcDrawThemeTextEx(hTheme, hdc, iPartId, iStateId, pszText, iCharCount, dwFlags, pRect, pOptions);
}

HRESULT FutureWin::BufferedPaintInit(VOID)
{
	if (!funcBufferedPaintInit)
        return E_FAIL;
	
	return funcBufferedPaintInit();
}

HRESULT FutureWin::BufferedPaintUnInit(VOID)
{
	if (!funcBufferedPaintUnInit)
        return E_FAIL;
	
	return funcBufferedPaintUnInit();
}

HPAINTBUFFER FutureWin::BeginBufferedPaint(HDC hdcTarget, const RECT* prcTarget, BP_BUFFERFORMAT dwFormat, BP_PAINTPARAMS* pPaintParams, HDC* phdc)
{
	if (!funcBeginBufferedPaint)
        return NULL;

	return funcBeginBufferedPaint(hdcTarget, prcTarget, dwFormat, pPaintParams, phdc);
}

HRESULT FutureWin::EndBufferedPaint(HPAINTBUFFER hBufferedPaint, BOOL fUpdateTarget)
{
	if (!funcEndBufferedPaint)
        return E_FAIL;

	return funcEndBufferedPaint(hBufferedPaint, fUpdateTarget);
}

HRESULT FutureWin::BufferedPaintSetAlpha(HPAINTBUFFER hBufferedPaint, const RECT* prc, BYTE alpha)
{
	if (!funcBufferedPaintSetAlpha)
        return E_FAIL;

	return funcBufferedPaintSetAlpha(hBufferedPaint, prc, alpha);
}

HRESULT FutureWin::SetCurrentProcessExplicitAppUserModelID(__in  PCWSTR AppID)
{
	if (!funcSetCurrentProccesAppID)
		return E_FAIL;

	return funcSetCurrentProccesAppID(AppID);
}

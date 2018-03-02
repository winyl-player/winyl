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

typedef struct _DWM_BLURBEHIND {
	DWORD dwFlags;
	BOOL  fEnable;
	HRGN  hRgnBlur;
	BOOL  fTransitionOnMaximized;
} DWM_BLURBEHIND, *PDWM_BLURBEHIND;

// Window attributes
enum DWMWINDOWATTRIBUTE
{
    DWMWA_NCRENDERING_ENABLED = 1,      // [get] Is non-client rendering enabled/disabled
    DWMWA_NCRENDERING_POLICY,           // [set] Non-client rendering policy
    DWMWA_TRANSITIONS_FORCEDISABLED,    // [set] Potentially enable/forcibly disable transitions
    DWMWA_ALLOW_NCPAINT,                // [set] Allow contents rendered in the non-client area to be visible on the DWM-drawn frame.
    DWMWA_CAPTION_BUTTON_BOUNDS,        // [get] Bounds of the caption button area in window-relative space.
    DWMWA_NONCLIENT_RTL_LAYOUT,         // [set] Is non-client content RTL mirrored
    DWMWA_FORCE_ICONIC_REPRESENTATION,  // [set] Force this window to display iconic thumbnails.
    DWMWA_FLIP3D_POLICY,                // [set] Designates how Flip3D will treat the window.
    DWMWA_EXTENDED_FRAME_BOUNDS,        // [get] Gets the extended frame bounds rectangle in screen space
    DWMWA_HAS_ICONIC_BITMAP,            // [set] Indicates an available bitmap when there is no better thumbnail representation.
    DWMWA_DISALLOW_PEEK,                // [set] Don't invoke Peek on the window.
    DWMWA_EXCLUDED_FROM_PEEK,           // [set] LivePreview exclusion information
    DWMWA_LAST
};

// Non-client rendering policy attribute values
enum DWMNCRENDERINGPOLICY
{
	DWMNCRP_USEWINDOWSTYLE, // Enable/disable non-client rendering based on window style
	DWMNCRP_DISABLED,       // Disabled non-client rendering; window style is ignored
	DWMNCRP_ENABLED,        // Enabled non-client rendering; window style is ignored
	DWMNCRP_LAST
};

#define DWM_SIT_DISPLAYFRAME    0x00000001  // Display a window frame around the provided bitmap

#define LOCALE_NAME_USER_DEFAULT            NULL

class FutureWin
{
    private:
		HINSTANCE libKernel32 = NULL;
		HINSTANCE libUser32 = NULL;
		HINSTANCE libComctl32 = NULL;
		HINSTANCE libShell32 = NULL;
		HINSTANCE libDwmApi = NULL;
		HINSTANCE libUxTheme = NULL;

		bool isVistaOrLater = false;
		bool isSevenOrLater = false;
		bool isEightOrLater = false;

    public:
        FutureWin();
        ~FutureWin();
		inline bool IsVistaOrLater() {return isVistaOrLater;}
		inline bool IsSevenOrLater() {return isSevenOrLater;}
		inline bool IsEightOrLater() {return isEightOrLater;}

		void ThemeChanged();
		HTHEME handleTheme;
		bool IsTheme() {return (handleTheme ? true : false);}

		bool IsCompositionEnabled();

		void LoadKernel32Functions();
		void LoadUser32Functions();
		void LoadComctl32Functions();
		void LoadShell32Functions();
		void LoadDwmApiFunctions();
		void LoadUxThemeFunctions();

		// kernel32.dll
		int CompareStringEx(LPCWSTR lpLocaleName, DWORD dwCmpFlags, LPCWSTR lpString1, int cchCount1, LPCWSTR lpString2, int cchCount2, LPNLSVERSIONINFO lpVersionInformation, LPVOID lpReserved, LPARAM lParam);
		int LCMapStringEx(LPCWSTR lpLocaleName, DWORD dwMapFlags, LPCWSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest, LPNLSVERSIONINFO lpVersionInformation, LPVOID lpReserved, LPARAM sortHandle);
		int FindNLSStringEx(LPCWSTR lpLocaleName, DWORD dwFindNLSStringFlags, LPCWSTR lpStringSource, int cchSource, LPCWSTR lpStringValue, int cchValue, LPINT pcchFound, LPNLSVERSIONINFO lpVersionInformation, LPVOID lpReserved, LPARAM sortHandle);
		int CompareStringOrdinal(LPCWSTR lpString1, int cchCount1, LPCWSTR lpString2, int cchCount2, BOOL bIgnoreCase);
		int FindStringOrdinal(DWORD dwFindStringOrdinalFlags, LPCWSTR lpStringSource, int cchSource, LPCWSTR lpStringValue, int cchValue, BOOL bIgnoreCase); // Windows 7

		VOID InitializeConditionVariable(PCONDITION_VARIABLE ConditionVariable);
		BOOL SleepConditionVariableCS(PCONDITION_VARIABLE ConditionVariable, PCRITICAL_SECTION CriticalSection, DWORD dwMilliseconds);
		BOOL SleepConditionVariableSRW(PCONDITION_VARIABLE ConditionVariable, PSRWLOCK SRWLock, DWORD dwMilliseconds, ULONG Flags);
		VOID WakeAllConditionVariable(PCONDITION_VARIABLE ConditionVariable);
		VOID WakeConditionVariable(PCONDITION_VARIABLE ConditionVariable);
		
		BOOL InitializeCriticalSectionEx(LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount, DWORD Flags);
		VOID InitializeSRWLock(PSRWLOCK SRWLock);
		VOID AcquireSRWLockExclusive(PSRWLOCK SRWLock);
		VOID ReleaseSRWLockExclusive(PSRWLOCK SRWLock);
		BOOL TryAcquireSRWLockExclusive(PSRWLOCK SRWLock); // Windows 7

		// user32.dll
		BOOL UpdateLayeredWindowIndirect(HWND hwnd, const UPDATELAYEREDWINDOWINFO* pULWInfo);

		// comctl32.dll
		HRESULT TaskDialog(HWND hWndParent, HINSTANCE hInstance, PCWSTR pszWindowTitle, PCWSTR pszMainInstruction, PCWSTR pszContent, TASKDIALOG_COMMON_BUTTON_FLAGS dwCommonButtons, PCWSTR pszIcon, int* pnButton);
		HRESULT TaskDialogIndirect(const TASKDIALOGCONFIG* pTaskConfig, int* pnButton, int* pnRadioButton, BOOL* pfVerificationFlagChecked);
		HRESULT LoadIconMetric(HINSTANCE hinst, PCWSTR pszName, int lims, HICON *phico);

		// shell32.dll
		HRESULT SetCurrentProcessExplicitAppUserModelID(__in  PCWSTR AppID);

		// dwmapi.dll
		HRESULT DwmIsCompositionEnabled(BOOL* pfEnabled);
		HRESULT DwmEnableComposition(UINT uCompositionAction);
		HRESULT DwmEnableBlurBehindWindow(HWND hWnd, const DWM_BLURBEHIND* pBlurBehind);
		HRESULT DwmExtendFrameIntoClientArea(HWND hWnd, const MARGINS* pMarInset);
		HRESULT DwmSetWindowAttribute(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute);
		HRESULT DwmGetWindowAttribute(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute);
		HRESULT DwmSetIconicThumbnail(HWND hwnd, HBITMAP hbmp, DWORD dwSITFlags);
		HRESULT DwmSetIconicLivePreviewBitmap(HWND hwnd, HBITMAP hbmp, POINT* pptClient, DWORD dwSITFlags);
		HRESULT DwmInvalidateIconicBitmaps(HWND hwnd);

		// uxtheme.dll
		HRESULT DrawThemeTextEx(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwFlags, LPRECT pRect, const DTTOPTS* pOptions);
		HRESULT BufferedPaintInit(VOID);
		HRESULT BufferedPaintUnInit(VOID);
		HPAINTBUFFER BeginBufferedPaint(HDC hdcTarget, const RECT* prcTarget, BP_BUFFERFORMAT dwFormat, BP_PAINTPARAMS* pPaintParams, HDC* phdc);
		HRESULT EndBufferedPaint(HPAINTBUFFER hBufferedPaint, BOOL fUpdateTarget);
		HRESULT BufferedPaintSetAlpha(HPAINTBUFFER hBufferedPaint, const RECT* prc, BYTE alpha);

private:
		// kernel32.dll
		typedef int (__stdcall* COMPARE_STRING_EX)(LPCWSTR lpLocaleName, DWORD dwCmpFlags, LPCWSTR lpString1, int cchCount1, LPCWSTR lpString2, int cchCount2, LPNLSVERSIONINFO lpVersionInformation, LPVOID lpReserved, LPARAM lParam);
		typedef int (__stdcall* LC_MAP_STRING_EX)(LPCWSTR lpLocaleName, DWORD dwMapFlags, LPCWSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest, LPNLSVERSIONINFO lpVersionInformation, LPVOID lpReserved, LPARAM sortHandle);
		typedef int (__stdcall* FIND_NLS_STRING_EX)(LPCWSTR lpLocaleName, DWORD dwFindNLSStringFlags, LPCWSTR lpStringSource, int cchSource, LPCWSTR lpStringValue, int cchValue, LPINT pcchFound, LPNLSVERSIONINFO lpVersionInformation, LPVOID lpReserved, LPARAM sortHandle);
		typedef int (__stdcall* COMPARE_STRING_ORDINAL)(LPCWSTR lpString1, int cchCount1, LPCWSTR lpString2, int cchCount2, BOOL bIgnoreCase);
		typedef int (__stdcall* FIND_STRING_ORDINAL)(DWORD dwFindStringOrdinalFlags, LPCWSTR lpStringSource, int cchSource, LPCWSTR lpStringValue, int cchValue, BOOL bIgnoreCase);

		typedef VOID (__stdcall* INITIALIZE_CONDITION_VARIABLE)(PCONDITION_VARIABLE ConditionVariable);
		typedef BOOL (__stdcall* SLEEP_CONDITION_VARIABLE_CS)(PCONDITION_VARIABLE ConditionVariable, PCRITICAL_SECTION CriticalSection, DWORD dwMilliseconds);
		typedef BOOL (__stdcall* SLEEP_CONDITION_VARIABLE_SRW)(PCONDITION_VARIABLE ConditionVariable, PSRWLOCK SRWLock, DWORD dwMilliseconds, ULONG Flags);
		typedef VOID (__stdcall* WAKE_ALL_CONDITION_VARIABLE)(PCONDITION_VARIABLE ConditionVariable);
		typedef VOID (__stdcall* WAKE_CONDITION_VARIABLE)(PCONDITION_VARIABLE ConditionVariable);
		
		typedef BOOL (__stdcall* INITIALIZE_CRITICAL_SECTION_EX)(LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount, DWORD Flags);
		typedef VOID (__stdcall* INITIALIZE_SRW_LOCK)(PSRWLOCK SRWLock);
		typedef VOID (__stdcall* ACQUIRE_SRW_LOCK_EXCLUSIVE)(PSRWLOCK SRWLock);
		typedef VOID (__stdcall* RELEASE_SRW_LOCK_EXCLUSIVE)(PSRWLOCK SRWLock);
		typedef BOOL (__stdcall* TRY_ACQUIRE_SRW_LOCK_EXCLUSIVE)(PSRWLOCK SRWLock);

		// user32.dll
		typedef BOOL (__stdcall* UPDATE_LAYERED_WINDOW_INDIRECT)(HWND hwnd, const UPDATELAYEREDWINDOWINFO* pULWInfo);

		// comctl32.dll
		typedef HRESULT (__stdcall* TASK_DIALOG)(HWND hWndParent, HINSTANCE hInstance, PCWSTR pszWindowTitle, PCWSTR pszMainInstruction, PCWSTR pszContent, TASKDIALOG_COMMON_BUTTON_FLAGS dwCommonButtons, PCWSTR pszIcon, int* pnButton);
		typedef HRESULT (__stdcall* TASK_DIALOG_INDIRECT)(const TASKDIALOGCONFIG* pTaskConfig, int* pnButton, int* pnRadioButton, BOOL* pfVerificationFlagChecked);
		typedef HRESULT (__stdcall* LOAD_ICON_METRIC)(HINSTANCE hinst, PCWSTR pszName, int lims, HICON *phico);

		// shell32.dll
		typedef HRESULT (__stdcall* SET_CURRENT_PROCCES_APPID) (__in  PCWSTR AppID);

		// dwmapi.dll
		typedef HRESULT (__stdcall* DWM_IS_COMPOSITION_ENABLED)(BOOL* pfEnabled);
		typedef HRESULT (__stdcall* DWM_ENABLE_COMPOSITION)(UINT uCompositionAction);
		typedef HRESULT (__stdcall* DWM_ENABLE_BLUR_BEHIND_WINDOW) (HWND hWnd, const DWM_BLURBEHIND* pBlurBehind);
		typedef HRESULT (__stdcall* DWM_EXTEND_FRAME_INTO_CLIENT_AREA)(HWND hWnd, const MARGINS* pMarInset);
		typedef HRESULT (__stdcall* DWM_SET_WINDOW_ATTRIBUTE)(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute);
		typedef HRESULT (__stdcall* DWM_GET_WINDOW_ATTRIBUTE)(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute);
		typedef HRESULT (__stdcall* DWM_SET_ICONIC_THUMBNAIL)(HWND hwnd, HBITMAP hbmp, DWORD dwSITFlags);
		typedef HRESULT (__stdcall* DWM_SET_ICONIC_LIVE_PREVIEW_BITMAP)(HWND hwnd, HBITMAP hbmp, POINT* pptClient, DWORD dwSITFlags);
		typedef HRESULT (__stdcall* DWM_INVALIDATE_ICONIC_BITMAPS)(HWND hwnd);

		// uxtheme.dll
		typedef HRESULT (__stdcall* DRAW_THEME_TEXT_EX)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwFlags, LPRECT pRect, const DTTOPTS* pOptions);
		typedef HRESULT (__stdcall* BUFFERED_PAINT_INIT)(VOID);
		typedef HRESULT (__stdcall* BUFFERED_PAINT_UNINIT)(VOID);
		typedef HPAINTBUFFER (__stdcall* BEGIN_BUFFERED_PAINT)(HDC hdcTarget, const RECT* prcTarget, BP_BUFFERFORMAT dwFormat, BP_PAINTPARAMS* pPaintParams, HDC* phdc);
		typedef HRESULT (__stdcall* END_BUFFERED_PAINT)(HPAINTBUFFER hBufferedPaint, BOOL fUpdateTarget);
		typedef HRESULT (__stdcall* BUFFERED_PAINT_SET_ALPHA)(HPAINTBUFFER hBufferedPaint, const RECT* prc, BYTE alpha);

		////////////////

		// kernel32.dll
		COMPARE_STRING_EX funcCompareStringEx = nullptr;
		LC_MAP_STRING_EX funcLCMapStringEx = nullptr;
		FIND_NLS_STRING_EX funcFindNLSStringEx = nullptr;
		COMPARE_STRING_ORDINAL funcCompareStringOrdinal = nullptr;
		FIND_STRING_ORDINAL funcFindStringOrdinal = nullptr;

		INITIALIZE_CONDITION_VARIABLE funcInitializeConditionVariable = nullptr;
		SLEEP_CONDITION_VARIABLE_CS funcSleepConditionVariableCS = nullptr;
		SLEEP_CONDITION_VARIABLE_SRW funcSleepConditionVariableSRW = nullptr;
		WAKE_ALL_CONDITION_VARIABLE funcWakeAllConditionVariable = nullptr;
		WAKE_CONDITION_VARIABLE funcWakeConditionVariable = nullptr;

		INITIALIZE_CRITICAL_SECTION_EX funcInitializeCriticalSectionEx = nullptr;
		INITIALIZE_SRW_LOCK funcInitializeSRWLock = nullptr;
		ACQUIRE_SRW_LOCK_EXCLUSIVE funcAcquireSRWLockExclusive = nullptr;
		RELEASE_SRW_LOCK_EXCLUSIVE funcReleaseSRWLockExclusive = nullptr;
		TRY_ACQUIRE_SRW_LOCK_EXCLUSIVE funcTryAcquireSRWLockExclusive = nullptr;

		// user32.dll
		UPDATE_LAYERED_WINDOW_INDIRECT funcUpdateLayeredWindowIndirect = nullptr;

		// comctl32.dll
		TASK_DIALOG funcTaskDialog = nullptr;
		TASK_DIALOG_INDIRECT funcTaskDialogIndirect = nullptr;
		LOAD_ICON_METRIC funcLoadIconMetric;

		// shell32.dll
		SET_CURRENT_PROCCES_APPID funcSetCurrentProccesAppID = nullptr;

		// dwmapi.dll
		DWM_IS_COMPOSITION_ENABLED funcDwmIsCompositionEnabled = nullptr;
		DWM_ENABLE_COMPOSITION funcDwmEnableComposition = nullptr;
		DWM_ENABLE_BLUR_BEHIND_WINDOW funcDwmEnableBlurBehindWindow = nullptr;
		DWM_EXTEND_FRAME_INTO_CLIENT_AREA funcDwmExtendFrameIntoClientArea = nullptr;
		DWM_SET_WINDOW_ATTRIBUTE funcDwmSetWindowAttribute = nullptr;
		DWM_GET_WINDOW_ATTRIBUTE funcDwmGetWindowAttribute = nullptr;
		DWM_SET_ICONIC_THUMBNAIL funcDwmSetIconicThumbnail = nullptr;
		DWM_SET_ICONIC_LIVE_PREVIEW_BITMAP funcDwmSetIconicLivePreviewBitmap = nullptr;
		DWM_INVALIDATE_ICONIC_BITMAPS funcDwmInvalidateIconicBitmaps = nullptr;

		// uxtheme.dll
		DRAW_THEME_TEXT_EX funcDrawThemeTextEx = nullptr;
		BUFFERED_PAINT_INIT funcBufferedPaintInit = nullptr;
		BUFFERED_PAINT_UNINIT funcBufferedPaintUnInit = nullptr;
		BEGIN_BUFFERED_PAINT funcBeginBufferedPaint = nullptr;
		END_BUFFERED_PAINT funcEndBufferedPaint = nullptr;
		BUFFERED_PAINT_SET_ALPHA funcBufferedPaintSetAlpha = nullptr;
};


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
#include "DlgHotKeys.h"

DlgHotKeys::DlgHotKeys()
{

}

DlgHotKeys::~DlgHotKeys()
{
	if (imListKeys)
		::ImageList_Destroy(imListKeys);
}

DlgHotKeys* DlgHotKeys::dlgHotKeys = nullptr;
WNDPROC DlgHotKeys::mainDlgProc = nullptr;

LRESULT DlgHotKeys::DlgKeyProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)  
{
	// Take the entire input and overlap it, also blocks the beep sound (MessageBeep) when press a key
	if (message == WM_GETDLGCODE)
		return DLGC_WANTALLKEYS;
	else if (message == WM_CHAR || message == WM_SYSCHAR) // || message == WM_SYSCHAR
		return 0;

	// Pass the keys to the parent window
	else if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN)
	{
		dlgHotKeys->OnKeyDown((UINT)wParam, HIWORD(lParam), LOWORD(lParam));
		return 0;
	}
	
	return mainDlgProc(hWnd, message, wParam, lParam);  
}

INT_PTR DlgHotKeys::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		// Subclass ListView to overlap keys
		HWND wndSubclass;
		dlgHotKeys = this;
		wndSubclass = ::GetDlgItem(thisWnd, IDC_LIST_KEYS);
		mainDlgProc = (WNDPROC)(LONG_PTR)::GetWindowLongPtr(wndSubclass, GWLP_WNDPROC);
		::SetWindowLongPtr(wndSubclass, GWLP_WNDPROC, (LONG_PTR)&DlgKeyProc);

		OnInitDialog();
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			OnBnClickedOK();
			EndDialog(LOWORD(wParam));
			return TRUE;
		case IDCANCEL:
			OnBnClickedCancel();
			EndDialog(LOWORD(wParam));
			return TRUE;
		}
		return TRUE;
	case WM_MENUCHAR:
		return TRUE;
	}

	return FALSE;
}

void DlgHotKeys::OnInitDialog()
{
	::SetWindowText(thisWnd, lang->GetLine(Lang::HotkeyDialog, 0));
	::SetDlgItemText(thisWnd, IDCANCEL, lang->GetLine(Lang::HotkeyDialog, 1));
	::SetDlgItemText(thisWnd, IDOK, lang->GetLine(Lang::HotkeyDialog, 2)); 
	::SetDlgItemText(thisWnd, IDC_STATIC_LINE1, lang->GetLine(Lang::HotkeyDialog, 3));
	::SetDlgItemText(thisWnd, IDC_STATIC_LINE2, lang->GetLine(Lang::HotkeyDialog, 4));

	listKeys = ::GetDlgItem(thisWnd, IDC_LIST_KEYS);

	// Assign styles and the theme to ListView (Vista/7 selection style like in explorer)
	ListView_SetExtendedListViewStyleEx(listKeys, 0, LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP|LVS_EX_DOUBLEBUFFER);
	::SetWindowTheme(listKeys, L"explorer", NULL);

	// Load icon
	imListKeys = ::ImageList_Create(16, 16, ILC_COLOR32, 0, 0);
	if (imListKeys)
	{
		HICON icon = (HICON)::LoadImageW(::GetModuleHandleW(NULL), MAKEINTRESOURCE(IDI_ICON_HOTKEY), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);

		::ImageList_AddIcon(imListKeys, icon);
		::DestroyIcon(icon);
	
		ListView_SetImageList(listKeys, imListKeys, LVSIL_SMALL);
	}

	CRect rcList;
	::GetClientRect(listKeys, rcList);
	
	LVCOLUMN column = {};
	column.mask = LVCF_WIDTH|LVCF_TEXT;
	column.cx = 150;
	column.pszText = const_cast<wchar_t*>(lang->GetLine(Lang::HotkeyDialog, 5));
	ListView_InsertColumn(listKeys, 0, &column);

	column.cx = rcList.Width() - 150 - GetSystemMetrics(SM_CXVSCROLL);
	column.pszText = const_cast<wchar_t*>(lang->GetLine(Lang::HotkeyDialog, 6));
	ListView_InsertColumn(listKeys, 1, &column);

	SetWindowRedraw(listKeys, FALSE);

	AddKeyToList(lang->GetLineS(Lang::HotkeyDialog, 7), HotKeys::KeyType::Play);
	AddKeyToList(lang->GetLineS(Lang::HotkeyDialog, 8), HotKeys::KeyType::Pause);
	AddKeyToList(lang->GetLineS(Lang::HotkeyDialog, 9), HotKeys::KeyType::Stop);
	AddKeyToList(lang->GetLineS(Lang::HotkeyDialog, 10), HotKeys::KeyType::Next);
	AddKeyToList(lang->GetLineS(Lang::HotkeyDialog, 11), HotKeys::KeyType::Prev);
	AddKeyToList(lang->GetLineS(Lang::HotkeyDialog, 12), HotKeys::KeyType::Mute);
	AddKeyToList(lang->GetLineS(Lang::HotkeyDialog, 13), HotKeys::KeyType::VolumeUp);
	AddKeyToList(lang->GetLineS(Lang::HotkeyDialog, 14), HotKeys::KeyType::VolumeDown);
	AddKeyToList(lang->GetLineS(Lang::HotkeyDialog, 15), HotKeys::KeyType::Rating1);
	AddKeyToList(lang->GetLineS(Lang::HotkeyDialog, 16), HotKeys::KeyType::Rating2);
	AddKeyToList(lang->GetLineS(Lang::HotkeyDialog, 17), HotKeys::KeyType::Rating3);
	AddKeyToList(lang->GetLineS(Lang::HotkeyDialog, 18), HotKeys::KeyType::Rating4);
	AddKeyToList(lang->GetLineS(Lang::HotkeyDialog, 19), HotKeys::KeyType::Rating5);
	AddKeyToList(lang->GetLineS(Lang::HotkeyDialog, 20), HotKeys::KeyType::Popup);
	
	SetWindowRedraw(listKeys, TRUE);

	// Critical error
	if (keys.size() != keysState.size())
		ListView_DeleteAllItems(listKeys);
}

void DlgHotKeys::OnBnClickedOK()
{
	// Critical error
	if (keys.size() != keysState.size())
		return;

	hotKeys->RemoveAllKeys();

	for (std::size_t i = 0, size = keys.size(); i < size; ++i)
	{
		if (keysState[i])
			hotKeys->AddKey(keys[i]);
	}

	hotKeys->SaveHotKeys();
}

void DlgHotKeys::OnBnClickedCancel()
{

}

void DlgHotKeys::AddKeyToList(const std::wstring& typeString, HotKeys::KeyType type)
{
	std::wstring keyString;

	// Search for a hotkey
	std::size_t find = hotKeys->FindKeyByType(type);

	// Compose the hotkey name
	if (find != -1)
	{
		if (hotKeys->GetKey(find).isCtrl)
			keyString += ctrlString;
		if (hotKeys->GetKey(find).isAlt)
			keyString += altString;
		if (hotKeys->GetKey(find).isWin)
			keyString += winString;
		if (hotKeys->GetKey(find).isShift)
			keyString += shiftString;

		keyString += NameFromVKey(hotKeys->GetKey(find).key);
	}

	LVITEM item = {};
	item.mask = LVIF_TEXT|LVIF_IMAGE;
	item.iItem = (int)keys.size();
	item.iSubItem = 0;
	item.iImage = 0;
	item.pszText = const_cast<wchar_t*>(typeString.c_str());

	ListView_InsertItem(listKeys, &item);

	if (find != -1)
	{
		ListView_SetItemText(listKeys, keys.size(), 1, const_cast<wchar_t*>(keyString.c_str()));
		keys.push_back(hotKeys->GetKey(find));
		keysState.push_back(1);
	}
	else
	{
		HotKeys::structKey key;
		key.type = type;
		keys.push_back(key);
		keysState.push_back(0);
	}
}

void DlgHotKeys::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Nothing is selected
	if (ListView_GetItemCount(listKeys) == 0)
		return;

	// Modifier key is pressed
	if (nChar == VK_CONTROL || nChar == VK_MENU || nChar == VK_SHIFT ||
		nChar == VK_LWIN || nChar == VK_RWIN)
		return;

	int index = ListView_GetSelectionMark(listKeys);

	std::wstring keyString = NameFromVKey(nChar);

	// When press "Delete", "Backspace", "F12" or an undefined key remove the hotkey
	// F12 note https://msdn.microsoft.com/en-us/library/windows/desktop/ms646309(v=vs.85).aspx
	// The F12 key is reserved for use by the debugger at all times, so it should not be registered as a hot key.
	// Even when you are not debugging an application, F12 is reserved in case a kernel-mode debugger or a just-in-time debugger is resident.
	if (nChar == VK_DELETE || nChar == VK_BACK || nChar == VK_F12 || keyString.empty())
	{
		ListView_SetItemText(listKeys, index, 1, L"");
		keysState[index] = 0;
		return;
	}

	keysState[index] = 1;

	keys[index].key = (int)nChar;

	keys[index].isCtrl = false;
	keys[index].isAlt = false;
	keys[index].isShift = false;
	keys[index].isWin = false;

	if (HIWORD(GetAsyncKeyState(VK_CONTROL)))
		keys[index].isCtrl = true;
	if (HIWORD(GetAsyncKeyState(VK_MENU)))
		keys[index].isAlt = true;
	if (HIWORD(GetAsyncKeyState(VK_LWIN)) || HIWORD(GetAsyncKeyState(VK_RWIN)))
		keys[index].isWin = true;
	if (HIWORD(GetAsyncKeyState(VK_SHIFT)))
		keys[index].isShift = true;

//	if (!keys[index].isCtrl &&
//		!keys[index].isAlt &&
//		!keys[index].isWin &&
//		!keys[index].isShift)
//		keys[index].isCtrl = true;

	std::wstring keyString2;

	if (keys[index].isCtrl)
		keyString2 += ctrlString;
	if (keys[index].isAlt)
		keyString2 += altString;
	if (keys[index].isWin)
		keyString2 += winString;
	if (keys[index].isShift)
		keyString2 += shiftString;

	keyString2 += keyString;

	DeleteDuplicated(index);

	ListView_SetItemText(listKeys, index, 1, const_cast<wchar_t*>(keyString2.c_str()));
}

void DlgHotKeys::DeleteDuplicated(int index)
{
	// Search for a duplicate hotkey and remove it if found
	for (std::size_t i = 0, size = keys.size(); i < size; ++i)
	{
		if (i == (std::size_t)index)
			continue;

		if (keysState[i])
		{
			if (keys[i].key     == keys[index].key &&
				keys[i].isCtrl  == keys[index].isCtrl &&
				keys[i].isAlt   == keys[index].isAlt &&
				keys[i].isWin   == keys[index].isWin &&
				keys[i].isShift == keys[index].isShift)
			{
				ListView_SetItemText(listKeys, i, 1, L"");
				keysState[i] = 0;
			}
		}
	}
}

std::wstring DlgHotKeys::NameFromVKey(UINT nVK)
{
	switch(nVK)
	{
		// Basic keys names
		case VK_PAUSE:
			return L"Pause";
		case VK_SPACE:
			return L"Space";
		case VK_PRIOR:
			return L"Page Up";
		case VK_NEXT:
			return L"Page Down";
		case VK_END:
			return L"End";
		case VK_HOME:
			return L"Home";
		case VK_LEFT:
			return L"Left";
		case VK_UP:
			return L"Up";
		case VK_RIGHT:
			return L"Right";
		case VK_DOWN:
			return L"Down";
		case VK_INSERT:
			return L"Insert";
		case VK_DELETE:
			return L"Delete";
		case VK_NUMLOCK:
			return L"Num Lock";

		// Media keys names
		case VK_BROWSER_BACK:
			return L"Browser Back";
		case VK_BROWSER_FORWARD:
			return L"Browser Forward";
		case VK_BROWSER_REFRESH:
			return L"Browser Refresh";
		case VK_BROWSER_STOP:
			return L"Browser Stop";
		case VK_BROWSER_SEARCH:
			return L"Browser Search";
		case VK_BROWSER_FAVORITES:
			return L"Browser Favorites";
		case VK_BROWSER_HOME:
			return L"Browser Start/Home";
		case VK_VOLUME_MUTE:
			return L"Volume Mute";
		case VK_VOLUME_DOWN:
			return L"Volume Down";
		case VK_VOLUME_UP:
			return L"Volume Up";
		case VK_MEDIA_NEXT_TRACK:
			return L"Next Track";
		case VK_MEDIA_PREV_TRACK:
			return L"Previous Track";
		case VK_MEDIA_STOP:
			return L"Stop Media";
		case VK_MEDIA_PLAY_PAUSE:
			return L"Play/Pause Media";
		case VK_LAUNCH_MAIL:
			return L"Start Mail";
		case VK_LAUNCH_MEDIA_SELECT:
			return L"Select Media";
		case VK_LAUNCH_APP1:
			return L"Start App 1";
		case VK_LAUNCH_APP2:
			return L"Start App 2";
	}

	// Use GetKeyNameText to get other key names

	UINT scanCode = ::MapVirtualKey(nVK, 0);//, GetKeyboardLayout(0));

	wchar_t key[80] = {};
	::GetKeyNameText(scanCode << 16, key, 80);

	std::wstring keyString = key;

	if (keyString.empty())
		keyString = L"Key [" + std::to_wstring(nVK) + L"]";

	return keyString;
}

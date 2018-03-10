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
#include "DlgPageSystem.h"
#include "HttpClient.h"

DlgPageSystem::DlgPageSystem()
{

}

DlgPageSystem::~DlgPageSystem()
{

}

INT_PTR DlgPageSystem::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		OnInitDialog();
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_RADIO_DIRECTSOUND:
			OnBnClickedRadioDirectSound();
			break;
		case IDC_RADIO_WASAPI:
			OnBnClickedRadioWasapi();
			break;
		case IDC_RADIO_ASIO:
			OnBnClickedRadioAsio();
			break;
		case IDC_BUTTON_ASIO_CONFIG:
			OnBnClickedButtonAsioConfig();
			break;
		case IDC_CHECK_PROXY:
			OnBnClickedCheckProxy();
			break;
		case IDC_CHECK_PROXY_AUTH:
			OnBnClickedCheckProxyAuth();
			break;
		}
		return TRUE;
	}

	return FALSE;
}

void DlgPageSystem::OnInitDialog()
{
	::SetDlgItemText(thisWnd, IDC_STATIC_DEVICE, lang->GetLine(Lang::SystemPage, 0));

	::SetDlgItemText(thisWnd, IDC_CHECK_BIT32, lang->GetLine(Lang::SystemPage, 2));
	::SetDlgItemText(thisWnd, IDC_CHECK_SOFTMIX, lang->GetLine(Lang::SystemPage, 3));

	::SetDlgItemText(thisWnd, IDC_STATIC_PROXY, lang->GetLine(Lang::SystemPage, 4));
	::SetDlgItemText(thisWnd, IDC_CHECK_PROXY, lang->GetLine(Lang::SystemPage, 5));
	::SetDlgItemText(thisWnd, IDC_STATIC_PROXY_HOST, lang->GetLine(Lang::SystemPage, 6));
	::SetDlgItemText(thisWnd, IDC_STATIC_PROXY_PORT, lang->GetLine(Lang::SystemPage, 7));
	::SetDlgItemText(thisWnd, IDC_CHECK_PROXY_AUTH, lang->GetLine(Lang::SystemPage, 8));
	::SetDlgItemText(thisWnd, IDC_STATIC_PROXY_LOGIN, lang->GetLine(Lang::SystemPage, 9));
	::SetDlgItemText(thisWnd, IDC_STATIC_PROXY_PASS, lang->GetLine(Lang::SystemPage, 10));

	::SetDlgItemText(thisWnd, IDC_EDIT_PROXY_HOST, settings->GetProxyHost().c_str());
	::SetDlgItemText(thisWnd, IDC_EDIT_PROXY_PORT, settings->GetProxyPort().c_str());
	::SetDlgItemText(thisWnd, IDC_EDIT_PROXY_LOGIN, settings->GetProxyLogin().c_str());
	::SetDlgItemText(thisWnd, IDC_EDIT_PROXY_PASS, settings->GetProxyPass().c_str());

	::CheckDlgButton(thisWnd, IDC_CHECK_UPDATES_AUTO, settings->IsCheckForUpdates());

	if (settings->IsBassBit32())
		::CheckDlgButton(thisWnd, IDC_CHECK_BIT32, BST_CHECKED);
	if (settings->IsBassSoftMix())
		::CheckDlgButton(thisWnd, IDC_CHECK_SOFTMIX, BST_CHECKED);

	if (settings->GetProxy() > 0)
		::CheckDlgButton(thisWnd, IDC_CHECK_PROXY, BST_CHECKED);
	if (settings->GetProxy() == 2)
		::CheckDlgButton(thisWnd, IDC_CHECK_PROXY_AUTH, BST_CHECKED);

	EnableProxy(settings->GetProxy() > 0);

	if (!futureWin->IsVistaOrLater())
		::EnableWindow(::GetDlgItem(thisWnd, IDC_RADIO_WASAPI), FALSE);

	comboDevice = ::GetDlgItem(thisWnd, IDC_COMBO_DEVICE);

	if (settings->GetBassDriver() == 0) // DirectSound
	{
		oldDriver = 0;
		::CheckRadioButton(thisWnd, IDC_RADIO_DIRECTSOUND, IDC_RADIO_ASIO, IDC_RADIO_DIRECTSOUND);

		AddDirectSoundDevices();
	}
	else if (settings->GetBassDriver() == 1) // WASAPI
	{
		oldDriver = 1;
		::CheckRadioButton(thisWnd, IDC_RADIO_DIRECTSOUND, IDC_RADIO_ASIO, IDC_RADIO_WASAPI);

		::ShowWindow(::GetDlgItem(thisWnd, IDC_CHECK_BIT32), SW_HIDE);
		::ShowWindow(::GetDlgItem(thisWnd, IDC_CHECK_SOFTMIX), SW_HIDE);

		AddWasapiDevices();
	}
	else if (settings->GetBassDriver() == 2) // ASIO
	{
		oldDriver = 2;
		::CheckRadioButton(thisWnd, IDC_RADIO_DIRECTSOUND, IDC_RADIO_ASIO, IDC_RADIO_ASIO);

		::ShowWindow(::GetDlgItem(thisWnd, IDC_CHECK_BIT32), SW_HIDE);
		::ShowWindow(::GetDlgItem(thisWnd, IDC_CHECK_SOFTMIX), SW_HIDE);

		AddAsioDevices();
	}

	// Select a chosen audio device
	for (int i = 0, size = ComboBox_GetCount(comboDevice); i < size; ++i)
	{
		if (settings->GetBassDevice() == (int)ComboBox_GetItemData(comboDevice, i))
		{
			ComboBox_SetCurSel(comboDevice, i);
			break;
		}
	}
}

void DlgPageSystem::OnBnClickedRadioDirectSound()
{
	oldDriver = 0;

	::ShowWindow(::GetDlgItem(thisWnd, IDC_CHECK_BIT32), SW_SHOW);
	::ShowWindow(::GetDlgItem(thisWnd, IDC_CHECK_SOFTMIX), SW_SHOW);

	ComboBox_ResetContent(comboDevice);

	AddDirectSoundDevices();

	ComboBox_SetCurSel(comboDevice, 0);
}

void DlgPageSystem::OnBnClickedRadioWasapi()
{
	oldDriver = 1;

	::ShowWindow(::GetDlgItem(thisWnd, IDC_CHECK_BIT32), SW_HIDE);
	::ShowWindow(::GetDlgItem(thisWnd, IDC_CHECK_SOFTMIX), SW_HIDE);

	ComboBox_ResetContent(comboDevice);

	AddWasapiDevices();

	ComboBox_SetCurSel(comboDevice, 0);
}

void DlgPageSystem::OnBnClickedRadioAsio()
{
	// When navigate from the keyboard if appear "Not found ASIO devices." MessageBox,
	// we can't close it anymore with OK button so fix this strange behaviour.
	if (!::IsDlgButtonChecked(thisWnd, IDC_RADIO_ASIO))
		return;

	std::wstring device;
	if (!libAudio->GetDeviceNameASIO(0, device))
	{
		MessageBox::Show(thisWnd, lang->GetLine(Lang::Message, 2), lang->GetLine(Lang::Message, 9), MessageBox::Icon::Information);

		if (oldDriver == 0)
			::CheckRadioButton(thisWnd, IDC_RADIO_DIRECTSOUND, IDC_RADIO_ASIO, IDC_RADIO_DIRECTSOUND);
		else if (oldDriver == 1)
			::CheckRadioButton(thisWnd, IDC_RADIO_DIRECTSOUND, IDC_RADIO_ASIO, IDC_RADIO_WASAPI);
		else if (oldDriver == 2)
			::CheckRadioButton(thisWnd, IDC_RADIO_DIRECTSOUND, IDC_RADIO_ASIO, IDC_RADIO_ASIO);

		return;
	}

	oldDriver = 2;

	::ShowWindow(::GetDlgItem(thisWnd, IDC_CHECK_BIT32), SW_HIDE);
	::ShowWindow(::GetDlgItem(thisWnd, IDC_CHECK_SOFTMIX), SW_HIDE);

	ComboBox_ResetContent(comboDevice);

	AddAsioDevices();

	ComboBox_SetCurSel(comboDevice, 0);
}

void DlgPageSystem::AddDirectSoundDevices()
{
	// Add default audio device
	ComboBox_AddString(comboDevice, lang->GetLine(Lang::SystemPage, 1));
	ComboBox_SetItemData(comboDevice, 0, (LPARAM)-1);
	//ComboBox_AddString(comboDevice, L"No Sound");
	//ComboBox_SetItemData(comboDevice, 1, (LPARAM)0);

	// Add audio devices
	std::wstring device;
	for (int i = 1; libAudio->GetDeviceName(i, device); ++i)
	{
		if (!device.empty())
		{
			int index = ComboBox_AddString(comboDevice, device.c_str());
			ComboBox_SetItemData(comboDevice, index, (LPARAM)i);
		}
	}
}

void DlgPageSystem::AddWasapiDevices()
{
	// Add default audio device
	ComboBox_AddString(comboDevice, lang->GetLine(Lang::SystemPage, 1));
	ComboBox_SetItemData(comboDevice, 0, (LPARAM)-1);

	// Add audio devices
	std::wstring device;
	for (int i = 0; libAudio->GetDeviceNameWASAPI(i, device); ++i)
	{
		if (!device.empty())
		{
			int index = ComboBox_AddString(comboDevice, device.c_str());
			ComboBox_SetItemData(comboDevice, index, (LPARAM)i);
		}
	}
}

void DlgPageSystem::AddAsioDevices()
{
	// Add audio devices
	std::wstring device;
	for (int i = 0; libAudio->GetDeviceNameASIO(i, device); ++i)
	{
		if (!device.empty())
		{
			int index = ComboBox_AddString(comboDevice, device.c_str());
			ComboBox_SetItemData(comboDevice, index, (LPARAM)i);
		}
	}
}

void DlgPageSystem::EnableProxy(bool isEnable)
{
	::EnableWindow(::GetDlgItem(thisWnd, IDC_STATIC_PROXY_HOST), isEnable);
	::EnableWindow(::GetDlgItem(thisWnd, IDC_STATIC_PROXY_PORT), isEnable);
	::EnableWindow(::GetDlgItem(thisWnd, IDC_EDIT_PROXY_HOST), isEnable);
	::EnableWindow(::GetDlgItem(thisWnd, IDC_EDIT_PROXY_PORT), isEnable);
	::EnableWindow(::GetDlgItem(thisWnd, IDC_CHECK_PROXY_AUTH), isEnable);

	if (!isEnable)
		EnableProxyAuth(false);
	else
		EnableProxyAuth(!!::IsDlgButtonChecked(thisWnd, IDC_CHECK_PROXY_AUTH));
}

void DlgPageSystem::EnableProxyAuth(bool isEnable)
{
	::EnableWindow(::GetDlgItem(thisWnd, IDC_STATIC_PROXY_LOGIN), isEnable);
	::EnableWindow(::GetDlgItem(thisWnd, IDC_STATIC_PROXY_PASS), isEnable);
	::EnableWindow(::GetDlgItem(thisWnd, IDC_EDIT_PROXY_LOGIN), isEnable);
	::EnableWindow(::GetDlgItem(thisWnd, IDC_EDIT_PROXY_PASS), isEnable);
}

void DlgPageSystem::OnBnClickedCheckProxy()
{
	EnableProxy(!!::IsDlgButtonChecked(thisWnd, IDC_CHECK_PROXY));
}

void DlgPageSystem::OnBnClickedCheckProxyAuth()
{
	EnableProxyAuth(!!::IsDlgButtonChecked(thisWnd, IDC_CHECK_PROXY_AUTH));
}

void DlgPageSystem::SaveSettings()
{
	int driver = 0;
	if (::IsDlgButtonChecked(thisWnd, IDC_RADIO_DIRECTSOUND))
		driver = 0;
	else if (::IsDlgButtonChecked(thisWnd, IDC_RADIO_WASAPI))
		driver = 1;
	else if (::IsDlgButtonChecked(thisWnd, IDC_RADIO_ASIO))
		driver = 2;

	int device = (int)ComboBox_GetItemData(comboDevice, ComboBox_GetCurSel(comboDevice));

	bool isBit32 = !!::IsDlgButtonChecked(thisWnd, IDC_CHECK_BIT32);
	bool isSoftMix = !!::IsDlgButtonChecked(thisWnd, IDC_CHECK_SOFTMIX);

	if (driver != settings->GetBassDriver() ||
		device != settings->GetBassDevice() ||
		isBit32 != settings->IsBassBit32() ||
		isSoftMix != settings->IsBassSoftMix())
	{
		settings->SetBassDriver(driver);
		settings->SetBassDevice(device);
		settings->SetBassBit32(isBit32);
		settings->SetBassSoftMix(isSoftMix);

		::SendMessage(wndMain, UWM_STOP, 0, 0);

		libAudio->Free();
		libAudio->Init(nullptr, settings->GetBassDriver(), settings->GetBassDevice(),
			settings->IsBassBit32(), settings->IsBassSoftMix(), false);
		libAudio->SetNoVolumeEffect(settings->IsBassNoVolume(), settings->IsBassNoEffect(), true);
		libAudio->SetPropertiesWA(settings->IsBassWasapiEvent(), settings->GetBassAsioChannel());
	}

	bool isCheckForUpdates = !!::IsDlgButtonChecked(thisWnd, IDC_CHECK_UPDATES_AUTO);
	if (isCheckForUpdates != settings->IsCheckForUpdates())
		settings->SetCheckForUpdates(isCheckForUpdates);


	std::wstring proxyHost  = HelperGetDlgItemText(thisWnd, IDC_EDIT_PROXY_HOST);
	std::wstring proxyPort  = HelperGetDlgItemText(thisWnd, IDC_EDIT_PROXY_PORT);
	std::wstring proxyLogin = HelperGetDlgItemText(thisWnd, IDC_EDIT_PROXY_LOGIN);
	std::wstring proxyPass  = HelperGetDlgItemText(thisWnd, IDC_EDIT_PROXY_PASS);

	settings->SetProxyHost(proxyHost);
	settings->SetProxyPort(proxyPort);

	if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_PROXY))
	{
		if (::IsDlgButtonChecked(thisWnd, IDC_CHECK_PROXY_AUTH))
			settings->SetProxy(2);
		else
			settings->SetProxy(1);
	}
	else
		settings->SetProxy(0);

	if (settings->GetProxy() == 2)
	{
		settings->SetProxyLogin(proxyLogin);
		settings->SetProxyPass(proxyPass);
	}
	else
	{
		settings->SetProxyLogin(L"");
		settings->SetProxyPass(L"");
	}

	HttpClient::SetProxy(settings->GetProxy(), settings->GetProxyHost(),  settings->GetProxyPort(),
		settings->GetProxyLogin(),  settings->GetProxyPass());

	libAudio->SetProxy(settings->GetProxy(), settings->GetProxyHost(),  settings->GetProxyPort(), 
		 settings->GetProxyLogin(),  settings->GetProxyPass());
}

void DlgPageSystem::CancelSettings()
{

}

void DlgPageSystem::OnBnClickedButtonAsioConfig()
{

}

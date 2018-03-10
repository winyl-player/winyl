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
#include "LastFM.h"

LastFM::LastFM()
{

}

LastFM::~LastFM()
{
	Free();
}


// LastFM member functions

bool LastFM::Init()
{
	HKEY keyPath = NULL;
	bool isKeyFound = false;

	// Check that Last.fm is installed
	if (::RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Last.fm\\Client",
		0, KEY_READ, &keyPath) == ERROR_SUCCESS)
	{
		isKeyFound = true;
	}
	else if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Software\\Last.fm\\Client",
		0, KEY_READ, &keyPath) == ERROR_SUCCESS)
	{
		isKeyFound = true;
	}

    if (isKeyFound) // It is installed
    {
		DWORD maxPath = MAX_PATH;
		wchar_t fileReg[MAX_PATH];

		// Get Last.fm exe file
        int result = RegQueryValueEx(keyPath, L"Path", NULL, NULL, (LPBYTE)fileReg, &maxPath);
		RegCloseKey(keyPath);

		if (result == ERROR_SUCCESS)
		{
			std::wstring file = fileReg;
			StringEx::Replace(file, '/', '\\');
			std::wstring path = PathEx::PathFromFile(file);

			if (::GetFileAttributes(file.c_str()) == INVALID_FILE_ATTRIBUTES)
				return false; // exe is not found
			else
			{
				// Start Last.fm
				HINSTANCE h = ::ShellExecute(NULL, L"open", file.c_str(), L"--tray", path.c_str(), SW_SHOWNORMAL);

				if ((INT_PTR)h <= 32) // Startup error
					return false;
			}
		}
		else
			return false;
    }
	else
	{
		RegCloseKey(keyPath);
		return false;
	}


	WSADATA wsaData;
	WORD versionRequested = MAKEWORD(2, 2);
	WSAStartup(versionRequested, &wsaData); // == NO_ERROR

	if (!threadSocket)
	{
		stopThreadSocket = false;
		eventSocket.reset(new Threading::Event());
		mutexSocket.reset(new Threading::Mutex());
		threadSocket.reset(new Threading::Thread());
		threadSocket->Start(std::bind(&LastFM::RunThreadSocket, this));
	}

	return true;
}

void LastFM::Free()
{
	if (threadSocket)
	{
		stopThreadSocket = true;
		eventSocket->Set();
		if (threadSocket->IsJoinable())
			threadSocket->Join();

		eventSocket.reset();
		mutexSocket.reset();
		threadSocket.reset();
	}

	WSACleanup();
}

void LastFM::RunThreadSocket()
{
	while (!stopThreadSocket)
	{
		eventSocket->Wait();

		mutexSocket->Lock();
		while (!commands.empty())
		{
			std::string command = commands.front();
			commands.pop_front();
			mutexSocket->Unlock();

			struct sockaddr_in clientService = {};
			clientService.sin_family = AF_INET;
			clientService.sin_addr.s_addr = inet_addr(clientHost.c_str());
			clientService.sin_port = htons(clientPort);
			
			socketLastFM = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (socketLastFM != INVALID_SOCKET)
			{
				if (connect(socketLastFM, (SOCKADDR*)&clientService, sizeof(clientService)) != SOCKET_ERROR)
					send(socketLastFM, command.c_str(), (int)command.size(), 0);

				closesocket(socketLastFM);
				socketLastFM = INVALID_SOCKET;
			}

			mutexSocket->Lock();
		}

		mutexSocket->Unlock();
	}
}

void LastFM::SendToLastFM(const std::string& command)
{
	mutexSocket->Lock();
	commands.push_back(command);
	mutexSocket->Unlock();
	eventSocket->Set();
}

void LastFM::Start(std::wstring artist, std::wstring title, std::wstring album, int time)
{
	if (!threadSocket)
		return;

	// Replace & to &&
	StringEx::ReplaceString(artist, L"&", L"&&");
	StringEx::ReplaceString(title, L"&", L"&&");
	StringEx::ReplaceString(album, L"&", L"&&");

	std::wstring stime = std::to_wstring(time);

	// Compose the command
	std::string command = "START c=";
	command += clientID;
	command += "&a=" + UTF::UTF8S(artist);
	command += "&t=" + UTF::UTF8S(title);
	command += "&b=" + UTF::UTF8S(album);
	command += "&m=";
	command += "&l=" + UTF::UTF8S(stime);
	command += "&p=";
	command += "\n";

	SendToLastFM(command);
}

void LastFM::Stop()
{
	if (!threadSocket)
		return;

	std::string command = "STOP c=";
	command += clientID;
	command += "\n";

	SendToLastFM(command);
}

void LastFM::Pause()
{
	if (!threadSocket)
		return;

	std::string command = "PAUSE c=";
	command += clientID;
	command += "\n";

	SendToLastFM(command);
}

void LastFM::Resume()
{
	if (!threadSocket)
		return;

	std::string command = "RESUME c=";
	command += clientID;
	command += "\n";

	SendToLastFM(command);
}



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

#include "UTF.h"
#include "Threading.h"
#include <deque>
#include <memory>

class LastFM
{
public:
	LastFM();
	virtual ~LastFM();

	bool Init();
	void Free();
	void Start(std::wstring artist, std::wstring title, std::wstring album, int time);
	void Stop();
	void Pause();
	void Resume();

private:
	std::string clientHost = "127.0.0.1"; // "localhost";
	int clientPort = 33367;
	std::string clientID = "wnl"; // ClientID (I got it from Last.FM support special for Winyl)

	void SendToLastFM(const std::string& command);

	void RunThreadSocket();

	SOCKET socketLastFM = INVALID_SOCKET;
	std::unique_ptr<Threading::Thread> threadSocket;
	std::unique_ptr<Threading::Event> eventSocket;
	std::unique_ptr<Threading::Mutex> mutexSocket;
	std::atomic<bool> stopThreadSocket = false;

	std::deque<std::string> commands;
};



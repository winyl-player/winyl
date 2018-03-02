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

// SkinListThread.cpp : implementation file
//

// SkinListThread

#include "stdafx.h"
#include "SkinListThread.h"

SkinListThread::SkinListThread()
{
	NewThread();
}

SkinListThread::~SkinListThread()
{
	DeleteThread();
}

void SkinListThread::NewThread()
{
	assert(!threadWorker.IsJoinable());

	stopThread = false;
	threadWorker.Start(std::bind(&SkinListThread::RunThread, this));
}

void SkinListThread::DeleteThread()
{
	if (threadWorker.IsJoinable())
	{
		stopThread = true;
		eventThread.Set();
		threadWorker.Join();
	}
}

void SkinListThread::ReInit()
{
	DeleteThread();

	coverNodes.clear();

	NewThread();
}

void SkinListThread::AddCover(ListNodeUnsafe node)
{
	if (!node->isCover)
	{
		node->isCover = true;
		mutexThread.Lock();
		coverNodes.push_back(node);
		mutexThread.Unlock();
	}
}

void SkinListThread::DrawCover()
{
	eventThread.Set();
}

void SkinListThread::RunThread()
{
	while (!stopThread)
	{
		eventThread.Wait();

		mutexThread.Lock();
		while (!stopThread && !coverNodes.empty())
		{
			SkinListNode* node = coverNodes.front();
			coverNodes.pop_front();
			mutexThread.Unlock();

			CoverLoader coverLoader;

			coverLoader.LoadCoverImage(node->GetFile());
			if (coverLoader.GetImage().IsValid())
			{
				ExImage* cover = new ExImage();

				CSize sz = node->szCover;

				if (cover->ThumbnailFromSource(coverLoader.GetImage(), sz.cx, sz.cy))
				{
					//Sleep(1000);

					node->cover = cover;

					::InvalidateRect(wndParent, NULL, FALSE);
				}
				else
					delete cover;
			}

			mutexThread.Lock();
		}

		mutexThread.Unlock();
	}
}

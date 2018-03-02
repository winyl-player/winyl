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

#include <windows.h>
#include <process.h>
#include <cassert>
#include <stdexcept>
#include <functional>
#include <atomic>
#include "FutureWin.h"

namespace Threading
{

// Sleep

inline void ThreadSleep(unsigned int milliseconds)
{
	::Sleep(milliseconds);
}
inline void ThreadYield()
{
	// http://stackoverflow.com/questions/1413630/switchtothread-thread-yield-vs-thread-sleep0-vs-thead-sleep1
	//::SwitchToThread();
	::Sleep(0);
}

// Thread

class Thread
{
public:
	Thread() {}
	~Thread() {assert(threadHandle == NULL);}
	Thread(const Thread&) = delete;
	Thread& operator=(const Thread&) = delete;

	//template<class Function, class... Args>
	//void StartBind(Function&& f, Args&&... args) {Start(std::bind(std::forward<Function>(f), std::forward<Args>(args)...));}

	inline bool IsJoinable() {return (threadHandle ? true : false);}

	enum class Priority
	{
		Normal         = THREAD_PRIORITY_NORMAL,
		BelowNormal    = THREAD_PRIORITY_BELOW_NORMAL,
		Lowest         = THREAD_PRIORITY_LOWEST,
		AboveNormal    = THREAD_PRIORITY_ABOVE_NORMAL,
		Highest        = THREAD_PRIORITY_HIGHEST,
		TimeCritical   = THREAD_PRIORITY_TIME_CRITICAL
	};

	bool Start(const std::function<void(void)>& func)
	{
		assert(threadHandle == NULL);

		stdFunc = func;

		threadHandle = (HANDLE)(ULONG_PTR)_beginthreadex(NULL, 0, Run, this, 0, NULL);

		if (threadHandle == NULL)
			return false;

		return true;
	}
	bool Start(const std::function<void(void)>& func, Priority priority)
	{
		assert(threadHandle == NULL);

		stdFunc = func;

		threadHandle = (HANDLE)(ULONG_PTR)_beginthreadex(NULL, 0, Run, this, CREATE_SUSPENDED, NULL);

		if (threadHandle == NULL)
			return false;

		::SetThreadPriority(threadHandle, (int)priority);

		::ResumeThread(threadHandle);

		return true;
	}
	bool StartBackground(const std::function<void(void)>& func)
	{
		if (!futureWin->IsVistaOrLater()) // Windows XP doesn't support background flag
			return Start(func);

		assert(threadHandle == NULL);

		stdFunc = func;

		threadHandle = (HANDLE)(ULONG_PTR)_beginthreadex(NULL, 0, RunBackground, this, 0, NULL);

		if (threadHandle == NULL)
			return false;

		return true;
	}

	bool IsRunning()
	{
		if (threadHandle)
		{
			// return ::WaitForSingleObjectEx(threadHandle, 0, FALSE) != WAIT_OBJECT_0;

			DWORD ec = 0;
			return GetExitCodeThread(threadHandle, &ec) && ec == STILL_ACTIVE;
		}

		return false;
	}

	void Join()
	{
		assert(threadHandle != NULL);

		if (threadHandle)
		{
			switch (::WaitForSingleObjectEx(threadHandle, INFINITE, FALSE))
			{
			case WAIT_OBJECT_0:
				::CloseHandle(threadHandle);
				threadHandle = NULL;
				return;
			default:
				throw std::runtime_error("Thread Join Error");
			}
		}
	}
	bool TryJoin(unsigned int milliseconds)
	{
		assert(threadHandle != NULL);

		if (threadHandle)
		{
			switch (::WaitForSingleObjectEx(threadHandle, milliseconds, FALSE))
			{
			case WAIT_TIMEOUT:
				return false;
			case WAIT_OBJECT_0:
				::CloseHandle(threadHandle);
				threadHandle = NULL;
				return true;
			default:
				throw std::runtime_error("Thread TryJoin Error");
			}
		}
		return false;
	}

private:
	static unsigned int __stdcall Run(void* param)
	{
		Thread* threadRun = static_cast<Thread*>(param);

		threadRun->stdFunc();

		//_endthreadex(0); // Do we really need this?
		return 0;
	}

	static unsigned int __stdcall RunBackground(void* param)
	{
		Thread* threadRun = static_cast<Thread*>(param);

		::SetThreadPriority(::GetCurrentThread(), THREAD_MODE_BACKGROUND_BEGIN);

		threadRun->stdFunc();

		//::SetThreadPriority(::GetCurrentThread(), THREAD_MODE_BACKGROUND_END); // Do we really need this?

		//_endthreadex(0); // Do we really need this?
		return 0;
	}

private:
	HANDLE threadHandle = NULL;
	std::function<void(void)> stdFunc;
};

// Mutex

class Mutex final
{
public:
	Mutex()
	{
		if (futureWin->IsSevenOrLater()) // TryAcquireSRWLockExclusive available only since Win7
			futureWin->InitializeSRWLock(&srwLock);
		else if (futureWin->IsVistaOrLater())
			futureWin->InitializeCriticalSectionEx(&critSection, 4000, CRITICAL_SECTION_NO_DEBUG_INFO);
		else
			::InitializeCriticalSectionAndSpinCount(&critSection, 4000);
	}
	~Mutex()
	{
		assert(idThread == 0); // Mutex is locked

		if (!futureWin->IsSevenOrLater())
			::DeleteCriticalSection(&critSection);
	}
	Mutex(const Mutex&) = delete;
	Mutex& operator=(const Mutex&) = delete;

	void Lock()
	{
		assert(idThread != ::GetCurrentThreadId()); // Lock on the same thread = deadlock

		if (futureWin->IsSevenOrLater())
			futureWin->AcquireSRWLockExclusive(&srwLock);
		else
			::EnterCriticalSection(&critSection);

#ifndef NDEBUG
		idThread = ::GetCurrentThreadId();
#endif
	}
	void Unlock()
	{
		assert(idThread == ::GetCurrentThreadId()); // Mutex is not locked
#ifndef NDEBUG
		idThread = 0;
#endif
		if (futureWin->IsSevenOrLater())
			futureWin->ReleaseSRWLockExclusive(&srwLock);
		else
			::LeaveCriticalSection(&critSection);
	}
	bool TryLock()
	{
		assert(idThread != ::GetCurrentThreadId()); // TryLock on the same thread = undefined behavior

		if (futureWin->IsSevenOrLater())
		{
			if (futureWin->TryAcquireSRWLockExclusive(&srwLock))
			{
#ifndef NDEBUG
				idThread = ::GetCurrentThreadId();
#endif
				return true;
			}
		}
		else
		{
			if (::TryEnterCriticalSection(&critSection))
			{
#ifndef NDEBUG
				idThread = ::GetCurrentThreadId();
#endif
				return true;
			}
		}

		return false;
	}

	// Windows: exteptions and critical sections
	// http://blogs.msdn.com/b/oldnewthing/archive/2003/12/22/45152.aspx
	// https://msdn.microsoft.com/en-gb/magazine/jj721588.aspx

private:
	SRWLOCK srwLock;
	CRITICAL_SECTION critSection;
#ifndef NDEBUG
	// http://blogs.msdn.com/b/oldnewthing/archive/2004/02/23/78395.aspx
	unsigned long idThread = 0;
#endif
};

class RecursiveMutex final
{
public:
	RecursiveMutex()
	{
		if (futureWin->IsVistaOrLater())
			futureWin->InitializeCriticalSectionEx(&critSection, 4000, CRITICAL_SECTION_NO_DEBUG_INFO);
		else
			::InitializeCriticalSectionAndSpinCount(&critSection, 4000);
	}
	~RecursiveMutex()
	{
		assert(debugCount == 0); // Number of Locks must be equal to Unlocks

		::DeleteCriticalSection(&critSection);
	}
	RecursiveMutex(const RecursiveMutex&) = delete;
	RecursiveMutex& operator=(const RecursiveMutex&) = delete;

	void Lock()
	{
		::EnterCriticalSection(&critSection);
#ifndef NDEBUG
		++debugCount;
#endif
	}
	void Unlock()
	{
		assert(--debugCount >= 0); // Mutex is not locked

		::LeaveCriticalSection(&critSection);
	}
	bool TryLock()
	{
		if (::TryEnterCriticalSection(&critSection))
		{
#ifndef NDEBUG
			++debugCount;
#endif
			return true;
		}

		return false;
	}

private:
	CRITICAL_SECTION critSection;
#ifndef NDEBUG
	long debugCount = 0;
#endif
};

// LockGuard

class LockGuard final
{
public:
	inline LockGuard(Mutex& mutex) : internalMutex(mutex)
	{
		internalMutex.Lock();
	}
	inline ~LockGuard()
	{
		internalMutex.Unlock();
	}
	LockGuard(const LockGuard&) = delete;
	LockGuard& operator=(const LockGuard&) = delete;

private:
	Mutex& internalMutex;
};

// Event

// When drop Windows XP support:
// 1. Remove EventXP and Event classes
// 2. Remove Init and DeInit functions in EventVista
// 3. Uncomment constructor and destructor in EventVista
// 4. Rename EventVista to Event

class Event final
{
private:
// ~10 times faster than WinAPI Event
class EventVista final
{
public:
	//EventVista(bool autoReset = true) : autoFlag(autoReset), stateFlag(false)
	//{
	//	futureWin->InitializeSRWLock(&srwLock);
	//	futureWin->InitializeConditionVariable(&condVar);
	//}
	//~EventVista() {}
	EventVista() {}
	~EventVista() {}
	void Init(bool autoReset = true)
	{
		autoFlag = autoReset;
		stateFlag = false;

		futureWin->InitializeSRWLock(&srwLock);
		futureWin->InitializeConditionVariable(&condVar);
	}
	void DeInit() {}
	EventVista(const EventVista&) = delete;
	EventVista& operator=(const EventVista&) = delete;

	void Set()
	{
		futureWin->AcquireSRWLockExclusive(&srwLock);
		stateFlag = true;
		futureWin->WakeAllConditionVariable(&condVar);
		futureWin->ReleaseSRWLockExclusive(&srwLock);
	}
	void Reset()
	{
		futureWin->AcquireSRWLockExclusive(&srwLock);
		stateFlag = false;
		futureWin->ReleaseSRWLockExclusive(&srwLock);
	}
	void Wait()
	{
		futureWin->AcquireSRWLockExclusive(&srwLock);
		while (!stateFlag)
		{
			if (!futureWin->SleepConditionVariableSRW(&condVar, &srwLock, INFINITE, 0))
			{
				futureWin->ReleaseSRWLockExclusive(&srwLock);
				throw std::runtime_error("Event Vista Wait Error");
			}
		}
		if (autoFlag)
			stateFlag = false;
		futureWin->ReleaseSRWLockExclusive(&srwLock);
	}
	bool TryWait(unsigned int milliseconds)
	{
		bool result = true;
		futureWin->AcquireSRWLockExclusive(&srwLock);
		while (!stateFlag)
		{
			if (!futureWin->SleepConditionVariableSRW(&condVar, &srwLock, milliseconds, 0))
			{
				result = false;
				if (::GetLastError() == ERROR_TIMEOUT)
					break;

				futureWin->ReleaseSRWLockExclusive(&srwLock);
				throw std::runtime_error("Event Vista TryWait Error");
			}
		}
		if (autoFlag && result)
			stateFlag = false;
		futureWin->ReleaseSRWLockExclusive(&srwLock);
		return result;
	}

private:
	bool autoFlag;
	volatile bool stateFlag;
	SRWLOCK srwLock;
	CONDITION_VARIABLE condVar;
};

class EventXP final
{
public:
	//EventXP(bool autoReset = true)
	//{
	//	eventHandle = ::CreateEventW(NULL, autoReset ? FALSE : TRUE, FALSE, NULL);
	//	if (eventHandle == NULL)
	//		throw std::runtime_error("Event XP Init Error");
	//}
	//~EventXP()
	//{
	//	if (eventHandle)
	//		::CloseHandle(eventHandle);
	//}
	EventXP() {}
	~EventXP() {}
	void Init(bool autoReset = true)
	{
		eventHandle = ::CreateEventW(NULL, autoReset ? FALSE : TRUE, FALSE, NULL);
		if (eventHandle == NULL)
			throw std::runtime_error("Event XP Init Error");
	}
	void DeInit()
	{
		if (eventHandle)
			::CloseHandle(eventHandle);
	}
	EventXP(const EventXP&) = delete;
	EventXP& operator=(const EventXP&) = delete;


	void Set()
	{
		if (!::SetEvent(eventHandle))
			throw std::runtime_error("Event XP Set Error");
	}
	void Reset()
	{
		if (!::ResetEvent(eventHandle))
			throw std::runtime_error("Event XP Reset Error");
	}
	void Wait()
	{
		switch (::WaitForSingleObjectEx(eventHandle, INFINITE, FALSE))
		{
		case WAIT_OBJECT_0:
			return;
		default:
			throw std::runtime_error("Event XP Wait Error");
		}
	}
	bool TryWait(unsigned int milliseconds)
	{
		switch (::WaitForSingleObjectEx(eventHandle, milliseconds, FALSE))
		{
		case WAIT_TIMEOUT:
			return false;
		case WAIT_OBJECT_0:
			return true;
		default:
			throw std::runtime_error("Event XP Wait Error");
		}
		return false;
	}

private:
	HANDLE eventHandle = NULL;
};

public:
	Event(bool autoReset = true)
	{
		if (futureWin->IsVistaOrLater())
			eventVista.Init(autoReset);
		else
			eventXP.Init(autoReset);
	}
	~Event()
	{
		if (futureWin->IsVistaOrLater())
			eventVista.DeInit();
		else
			eventXP.DeInit();
	}
	Event(const Event&) = delete;
	Event& operator=(const Event&) = delete;

	void Set()
	{
		if (futureWin->IsVistaOrLater())
			eventVista.Set();
		else
			eventXP.Set();
	}
	void Reset()
	{
		if (futureWin->IsVistaOrLater())
			eventVista.Reset();
		else
			eventXP.Reset();
	}
	void Wait()
	{
		if (futureWin->IsVistaOrLater())
			eventVista.Wait();
		else
			eventXP.Wait();
	}
	bool TryWait(unsigned int milliseconds)
	{
		if (futureWin->IsVistaOrLater())
			return eventVista.TryWait(milliseconds);
		else
			return eventXP.TryWait(milliseconds);
	}

private:
	Event::EventVista eventVista;
	Event::EventXP eventXP;
};

} // namespace Threading

// Global usings
using LockGuard = Threading::LockGuard;

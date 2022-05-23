#pragma once
#include <thread>
#include <condition_variable>
#include <chrono>

class CJoinThread
{
	enum class EnumStatus : unsigned char { eInvalid = 0, eStop, eRunning };
	struct SThreadStatus 
	{
		EnumStatus m_eStatus;
		std::mutex m_mtxLock;
		std::condition_variable m_cv;
		SThreadStatus() :m_eStatus(EnumStatus::eInvalid) {}
	};
public:
	CJoinThread() : m_bWaked(false) {}
	~CJoinThread() {}

	template<class FnCallable, class ...Args>
	bool StartWakeable(FnCallable pfnCallable, Args ...args)
	{
		return CreateThread([&]() {m_tThread = std::thread(&CJoinThread::WakeableWork<FnCallable, Args...>, this, pfnCallable, std::move(args)...); });
	}

	template<typename _Rep, typename _Period, class FnCallable, class ...Args>
	bool StartTimed(const std::chrono::duration<_Rep, _Period> &time, FnCallable pfnCallable, Args ...args)
	{
		return CreateThread([&]() {m_tThread = std::thread(&CJoinThread::TimeWork<_Rep, _Period, FnCallable, Args...>, this, time, pfnCallable, std::move(args)...); });
	}

	template<typename _Rep, typename _Period, class FnCallable, class ...Args>
	bool StartWakeableWithTimed(const std::chrono::duration<_Rep, _Period> &time, FnCallable pfnCallable, Args ...args)
	{
		return CreateThread([&]() {m_tThread = std::thread(&CJoinThread::WakeableWithTimeWork<_Rep, _Period, FnCallable, Args...>, this, time, pfnCallable, std::move(args)...); });
	}

	void RequestStop()
	{
		{
			std::lock_guard<std::mutex> guard(m_aStatus.m_mtxLock);
			m_aStatus.m_eStatus = EnumStatus::eStop;
			m_aStatus.m_cv.notify_one();
		}

		if (m_tThread.joinable())
		{
			m_tThread.join();
		}

		{
			std::lock_guard<std::mutex> guard(m_aStatus.m_mtxLock);
			m_aStatus.m_eStatus = EnumStatus::eInvalid;
		}
	}

	void Wake()
	{
		std::lock_guard<std::mutex> guard(m_aStatus.m_mtxLock);
		m_bWaked = true;
		m_aStatus.m_cv.notify_one();
	}

	auto ThreadID() const ->decltype(std::thread().get_id())
	{
		return m_tThread.get_id();
	}

	auto NativeHandle()->decltype(std::thread().native_handle())
	{
		return m_tThread.native_handle();
	}

private:
	template<class FnCreateThread>
	bool CreateThread(FnCreateThread &&pfnCreateThread)
	{
		{
			std::lock_guard<std::mutex> guard(m_aStatus.m_mtxLock);
			if (EnumStatus::eInvalid != m_aStatus.m_eStatus)
			{
				return false;
			}
			m_aStatus.m_eStatus = EnumStatus::eRunning;
		}

		try
		{
			std::forward<FnCreateThread>(pfnCreateThread)();
		}
		catch (...)
		{
			std::lock_guard<std::mutex> guard(m_aStatus.m_mtxLock);
			m_aStatus.m_eStatus = EnumStatus::eInvalid;

			return false;
		}

		return true;
	}

	template<class FnCallable, class ...Args>
	void WakeableWork(FnCallable pfnCallable, Args &&...args)
	{
		auto pfnHandle = std::bind(pfnCallable, std::forward<Args>(args)...);
		while (true)
		{
			{
				std::unique_lock<std::mutex> uLock(m_aStatus.m_mtxLock);
				m_aStatus.m_cv.wait(uLock, [&]() {return m_bWaked || EnumStatus::eStop == m_aStatus.m_eStatus; });
				if (EnumStatus::eStop == m_aStatus.m_eStatus)
				{
					break;
				}
				m_bWaked = false;
			}

			pfnHandle();
		}
	}

	template<typename _Rep, typename _Period, class FnCallable, class ...Args>
	void TimeWork(const std::chrono::duration<_Rep, _Period> &time, FnCallable pfnCallable, Args &&...args)
	{
		auto pfnHandle = std::bind(pfnCallable, std::forward<Args>(args)...);
		while (true)
		{
			{
				std::unique_lock<std::mutex> uLock(m_aStatus.m_mtxLock);
				m_aStatus.m_cv.wait_for(uLock, time, [&]() {return EnumStatus::eStop == m_aStatus.m_eStatus; });
				if (EnumStatus::eStop == m_aStatus.m_eStatus)
				{
					break;
				}
			}

			pfnHandle();
		}
	}

	template<typename _Rep, typename _Period, class FnCallable, class ...Args>
	void WakeableWithTimeWork(const std::chrono::duration<_Rep, _Period> &time, FnCallable pfnCallable, Args &&...args)
	{
		bool bTimeout = false;
		auto pfnHandle = std::bind(pfnCallable, std::forward<Args>(args)...);
		while (true)
		{
			bTimeout = false;
			{
				std::unique_lock<std::mutex> uLock(m_aStatus.m_mtxLock);
				bTimeout = !m_aStatus.m_cv.wait_for(uLock, time, [&]() {return m_bWaked || EnumStatus::eStop == m_aStatus.m_eStatus; });
				if (EnumStatus::eStop == m_aStatus.m_eStatus)
				{
					break;
				}

				if (!bTimeout)
				{
					m_bWaked = false;
				}
			}
			pfnHandle(bTimeout);
		}
	}

private:
	bool m_bWaked;
	SThreadStatus m_aStatus;
	std::thread m_tThread;
};
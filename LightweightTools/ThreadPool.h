#pragma once
#include <queue>
#include <future>
#include <condition_variable>
#include "NonCopyable.h"

template<class T> using default_thread_pool_task_container = std::queue<T>;

template<template <class> class TaskContainer = default_thread_pool_task_container>
class CThreadPool_ : private CNonCopyable
{
	using task_t = std::function<void()>;
	using task_container_t = TaskContainer<task_t>;
	using thread_container_t = std::vector<std::thread>;

public:
	CThreadPool_(size_t nThreadCount)
		: m_bIsStop(true)
		, m_nThreadCount(nThreadCount)
		, m_nTotalTask(0) 
	{
	}

	~CThreadPool_() {}

	bool Initialize()
	{
		if (m_nThreadCount < 1)
		{
			return false;
		}

		bool bResult = false;

		m_bIsStop = false;
		try
		{
			m_vecThreads.reserve(m_nThreadCount);
			for (size_t i = 0; i < m_nThreadCount; i++)
			{
				m_vecThreads.emplace_back(&CThreadPool_::Work, this);
			}

			bResult = true;
		}
		catch (...)
		{
			Uninitialize();
		}
		
		return bResult;
	}

	void Uninitialize()
	{
		RequestStop();

		size_t nThreadCount = m_vecThreads.size();
		for (size_t i = 0; i < nThreadCount; i++)
		{
			auto &thread = m_vecThreads[i];
			if (thread.joinable())
			{
				thread.join();
			}
		}
		m_vecThreads.swap(std::vector<std::thread>());
	}

	template<class F, class ...Args>
	auto Submit(F &&pfn, Args &&...args)->std::future<decltype(pfn(std::forward<Args>(args)...))>
	{
		using result_t = decltype(pfn(std::forward<Args>(args)...));

		std::future<result_t> fResult;
		try
		{
			auto pTask = std::make_shared<std::packaged_task<result_t()>>(std::bind(std::forward<F>(pfn), std::forward<Args>(args)...));

			fResult = pTask->get_future();
			{
				std::lock_guard<std::mutex> guard(m_mtxTask);
				m_qTasks.emplace([=]() { (*pTask)(); });
			}
			m_cv.notify_one();
			++m_nTotalTask;
		}
		catch (...)
		{
			// do nothing, not throw out
		}

		return fResult;
	}

	void RequestStop()
	{
		m_bIsStop = true;
		m_cv.notify_all();
	}

	bool IsStop() const
	{
		return m_bIsStop;
	}

	auto TaskCount() const ->decltype(task_container_t().size())
	{
		std::unique_lock<std::mutex> uLock(m_mtxTask);
		return m_qTasks.size();
	}

	size_t TotalTaskCount() const
	{
		return m_nTotalTask;
	}

private:
	void Work()
	{
		task_t task;
		while (true)
		{
			{
				std::unique_lock<std::mutex> uLock(m_mtxTask);
				m_cv.wait(uLock, [this]() { return !m_qTasks.empty() || m_bIsStop; });
				if (m_bIsStop)
				{
					break;
				}
				task = std::move(m_qTasks.front());
				m_qTasks.pop();
			}

			task();
		}
	}

private:
	std::atomic<bool> m_bIsStop;
	size_t m_nThreadCount;
	std::atomic<size_t> m_nTotalTask;
	mutable std::mutex m_mtxTask;
	std::condition_variable m_cv;
	task_container_t m_qTasks;
	thread_container_t m_vecThreads;
};
using CThreadPool = CThreadPool_<>;
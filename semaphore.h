#ifndef SEMAPHORE_H
#define SEMAPHORE_H

// ----------------------------------------------------------------------------------------------------

#include <mutex>
#include <condition_variable>

// ----------------------------------------------------------------------------------------------------

class SEMAPHORE
{
public:
	SEMAPHORE(uint32_t max_count = 0xFFFFFFFF) : m_max_count(max_count) {}
	~SEMAPHORE() {}
	
	void release()
	{
		std::scoped_lock<std::mutex> lock(m_mutex);
		if (m_count < m_max_count)
		{
			m_count ++;
		}
		m_cv.notify_one();
	}

	void acquire()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		while(!m_count)
		{
			m_cv.wait(lock);
		}
		m_count --;
	}

	bool try_acquire()
	{
		std::scoped_lock<std::mutex> lock(m_mutex);
		if (m_count)
		{
			m_count --;
			return true;
		}
		return false;
	}
	
	bool try_acquire_for(uint64_t duration)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		if (!m_count)
		{
			m_cv.wait_for(lock, std::chrono::milliseconds(duration));
		}
		if (m_count)
		{
			m_count --;
			return true;
		}
		return false;
	}
	
private:
	uint32_t m_count = 0;
	uint32_t m_max_count = 1;
	std::mutex m_mutex;
	std::condition_variable m_cv;
};

#endif

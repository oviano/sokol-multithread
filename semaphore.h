#ifndef SEMAPHORE_H
#define SEMAPHORE_H

// ----------------------------------------------------------------------------------------------------

#include <mutex>
#include <condition_variable>

// ----------------------------------------------------------------------------------------------------

class SEMAPHORE
{
public:
	SEMAPHORE() {}
	~SEMAPHORE() {}
	
	void release()
	{
		std::scoped_lock<std::mutex> lock(m_mutex);
		m_count ++;
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
	
private:
	uint32_t m_count = 0;
	std::mutex m_mutex;
	std::condition_variable m_cv;
};

#endif

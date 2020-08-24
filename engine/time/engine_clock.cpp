#include "engine_clock.h"
#include <exception>

namespace engine
{
	c_engine_clock::c_engine_clock() : m_start_time(), m_current_time(), m_last_time(), m_frequency()
	{
		m_frequency = get_frequency();
		reset();
	}

	const LARGE_INTEGER* c_engine_clock::start_time() const { return &m_start_time; }
	const LARGE_INTEGER* c_engine_clock::current_time() const { return &m_current_time; }
	const LARGE_INTEGER* c_engine_clock::last_time() const { return &m_last_time; }

	void c_engine_clock::reset()
	{
		get_time(&m_start_time);
		m_current_time = m_start_time;
		m_last_time = m_current_time;
	}
	
	double c_engine_clock::get_frequency() const
	{
		LARGE_INTEGER frequency;
		if (QueryPerformanceFrequency(&frequency) == false)
		{
			// major issue, throw exception
			throw std::exception("Failed to get High Resolution Timer in QueryPerformanceFrequency()");
		}

		return 1000.0 / frequency.QuadPart;
	}

	void c_engine_clock::get_time(LARGE_INTEGER* time) const
	{
		QueryPerformanceCounter(time);
	}

	void c_engine_clock::update_engine_time(c_engine_time* engine_time)
	{
		get_time(&m_current_time);
		double total_time = (m_current_time.QuadPart - m_start_time.QuadPart) * m_frequency;
		double elapsed_time = (m_current_time.QuadPart - m_last_time.QuadPart) * m_frequency;
		engine_time->set_total_time(total_time);
		engine_time->set_elapsed_time(elapsed_time);
		m_last_time = m_current_time;

	}

	void c_engine_clock::update_engine_clock()
	{
		get_time(&m_current_time);
		m_last_time = m_current_time;
	}

}
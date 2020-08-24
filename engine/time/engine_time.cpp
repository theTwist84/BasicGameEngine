#include "engine_time.h"

namespace engine
{
	c_engine_time::c_engine_time() : m_total_time(0), m_elapsed_time(0)
	{

	}

	c_engine_time::c_engine_time(double total_time, double elapsed_time) : m_total_time(total_time), m_elapsed_time(elapsed_time)
	{

	}

	double c_engine_time::total_time() const
	{
		return m_total_time;
	}

	void c_engine_time::set_total_time(double total_time)
	{
		m_total_time = total_time;
	}

	double c_engine_time::elapsed_time() const
	{
		return m_elapsed_time;
	}

	void c_engine_time::set_elapsed_time(double elapsed_time)
	{
		m_elapsed_time = elapsed_time;
	}
}
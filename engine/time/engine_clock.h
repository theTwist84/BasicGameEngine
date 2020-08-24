#ifndef ENGINE_ENGINE_CLOCK_H
#define ENGINE_ENGINE_CLOCK_H

#include "engine_time.h"
#include "windows.h"

namespace engine
{
	/// <summary>
	///
	///	TIMES ARE IN MILLISECONDS
	///
	/// Clock for game engine. Think of it as a watch. Each time you look at it ( with get_time()) function call, it returns the time now.
	/// When initializing or calling reset(), the start_time is recorded. Calling update_engine_time() updates the last_time and current_time while
	/// setting the elapsed and total time in the passed instance. update_engine_clock() only updates last_time and current_time;
	/// </summary>
	class c_engine_clock
	{
	public:
		c_engine_clock();
		const LARGE_INTEGER* start_time() const;
		const LARGE_INTEGER* current_time() const;
		const LARGE_INTEGER* last_time() const;

		void reset();
		double get_frequency() const;
		void get_time(LARGE_INTEGER* time) const;
		void update_engine_time(c_engine_time* engine_time);
		void update_engine_clock();

	private:
		LARGE_INTEGER m_start_time;
		LARGE_INTEGER m_current_time;
		LARGE_INTEGER m_last_time;
		double m_frequency;

	};
}


#endif

#ifndef ENGINE_ENGINE_TIME_H
#define ENGINE_ENGINE_TIME_H


namespace engine
{
	/// <summary>
	///
	///	TIMES ARE IN MILLISECONDS
	///
	/// Engine timer class. Is update with c_engine_clock instance. Contains the elapsed time and total time since initialization.
	/// </summary>
	class c_engine_time
	{
	public:
		c_engine_time();
		c_engine_time(double total_engine_time, double elapsed_engine_time);

		double total_time() const;
		void set_total_time(double total_time);
		double elapsed_time() const;
		void set_elapsed_time(double elapsed_time);

	private:
		double m_total_time;
		double m_elapsed_time;

	};
}

#endif
